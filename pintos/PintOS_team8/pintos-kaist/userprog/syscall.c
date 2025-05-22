#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/loader.h"
#include "userprog/gdt.h"
#include "threads/flags.h"
#include "intrinsic.h"
#include "threads/init.h"
#include "filesys/filesys.h"
#include "threads/synch.h"

void syscall_entry (void);
void syscall_handler (struct intr_frame *);
static struct lock filelock;
int process_add_file (struct file *f);


/* System call.
 *
 * Previously system call services was handled by the interrupt handler
 * (e.g. int 0x80 in linux). However, in x86-64, the manufacturer supplies
 * efficient path for requesting the system call, the `syscall` instruction.
 *
 * The syscall instruction works by reading the values from the the Model
 * Specific Register (MSR). For the details, see the manual. */


/* === [1] MSR 레지스터 초기화: syscall 진입점 설정 === */ 
#define MSR_STAR 0xc0000081         /* Segment selector msr */
#define MSR_LSTAR 0xc0000082        /* Long mode SYSCALL target */
#define MSR_SYSCALL_MASK 0xc0000084 /* Mask for the eflags */

void
syscall_init (void) 
{
	lock_init(&filelock);
	write_msr(MSR_STAR, ((uint64_t)SEL_UCSEG - 0x10) << 48  |
			((uint64_t)SEL_KCSEG) << 32);
	write_msr(MSR_LSTAR, (uint64_t) syscall_entry);

	/* The interrupt service rountine should not serve any interrupts
	 * until the syscall_entry swaps the userland stack to the kernel
	 * mode stack. Therefore, we masked the FLAG_FL. */
	write_msr(MSR_SYSCALL_MASK,
			FLAG_IF | FLAG_TF | FLAG_DF | FLAG_IOPL | FLAG_AC | FLAG_NT);
}


/* === [2] 메인 syscall 핸들러 === */
void
syscall_handler (struct intr_frame *f) 
{
	switch(f->R.rax) 
	{
		case SYS_HALT:
			halt();
			break;
		case SYS_EXIT:
			exit(f->R.rdi);
			break;
		case SYS_FORK:
			printf("fork has called!\n\n");
			// 🟩 TODO
			break;
		case SYS_EXEC:
			printf("exec has called!\n\n");
			// 🟩 TODO: 
			break;
		case SYS_WAIT:
			printf("wait has called!\n\n");
			// 🟩 TODO: 
			break;
		case SYS_CREATE:
			validate_address(f->R.rdi);
			f->R.rax = create(f->R.rdi, f->R.rsi);
			break;
		case SYS_REMOVE:
			// 🟩 TODO
			printf("remove has called!\n\n");
			break;
		case SYS_OPEN:
			validate_address(f->R.rdi);
			f->R.rax = open(f->R.rdi);
			break;
		case SYS_FILESIZE:
			// 🟩 TODO
			printf("filesize has called!\n\n");
			break;
		case SYS_READ:
			// 🟩 TODO
			printf("read has called!\n\n");
			break;
		case SYS_WRITE:
			f->R.rax = write(f->R.rdi, f->R.rsi, f->R.rdx);
			break;
		case SYS_SEEK:
			// 🟩 TODO
			printf("seek has called!\n\n");
			break;
		case SYS_TELL:
			// 🟩 TODO
			printf("tell has called!\n\n");
			break;
		case SYS_CLOSE:
			// 🟩 TODO
			printf("close has called!\n\n");
			break;
		default:
			printf("SERIOUS ERROR!! Unknown syscall numbber\n");
			thread_exit();
			break;
	}
}


/* === [3] 시스템 콜 구현 === */
void halt(void)
{
	power_off();
}

void exit(int status) {
	struct thread *cur = thread_current();
	printf("%s: exit(%d)\n", cur -> name, status);
	thread_exit();
}

pid_t fork(const char *thread_name) {
	// TODO
	printf("[stub] fork() not implemented yet.\n");
	return -1;
}

int exec(const char *cmd_line) {
	// TODO
	printf("[stub] exec() not implemented yet.\n");
	return -1;
}

int wait(pid_t pid) {
	// TODO
	printf("[stub] wait() not implemented yet.\n");
	return -1;
}

/***************************************************************
 * create - 주어진 이름과 크기로 새로운 파일을 생성 (열지는 않음)
 * 
 * 기능:
 *  - filesys_create(name, initial_size)를 호출하여 주어진 크기의 새 파일을 생성함
 *  - 생성만 수행하며, 열기는 별도로 open을 호출해야 함
 * 
 * 매개변수:
 *  - const char *file: 생성할 파일 이름 (문자열)
 *  - unsigned initial_size: 파일의 초기 크기 (byte 단위)
 * 
 * 반환값:
 *  - 생성에 성공하면 true, 실패하면 false
 ***************************************************************/
bool 
create (const char *file, unsigned initial_size) 
{
	// lock_acquire(&filesys_lock);
	bool success = filesys_create(file, initial_size);
	// lock_release(&filesys_lock);
	return success;
}

bool remove(const char *file) {
	// TODO
	printf("[stub] remove() not implemented yet.\n");
	return false;
}

/***************************************************************
 * open - 주어진 이름의 파일을 열고 파일 디스크립터(fd) 반환
 *
 * @file: 열고자 하는 파일의 이름 (유저가 전달한 문자열 포인터)
 *
 * 기능:
 * - 파일 시스템에 접근하여 해당 이름의 파일을 엽니다 (filesys_open 호출)
 * - 열린 파일에 대해 커널 내부의 file 구조체를 생성하고 추적 (*f)
 * - process_add_file(): 현재 프로세스의 파일 디스크립터 테이블(fd_table)에 
 * 	 빈 자리를 찾아 등록하고, 등록된 위치 인덱스(fd)를 반환
 * - 전역 락(filelock)을 사용해 동시 파일 접근으로 인한 경쟁 조건 방지
 *
 * 사용 목적:
 * - open() 시스템 콜을 통해 유저가 파일을 읽고 쓰기 위한 파일 디스크립터를 확보하도록 합니다.
 * - 반환된 fd는 read/write/close 등의 시스템 콜에서 참조 키로 사용됩니다.
 *
 * 반환값:
 * - 파일 디스크립터(fd) 할당 성공 시: 2 이상 정수
 * - 실패 시: -1 (예: 파일이 존재하지 않음)
 ***************************************************************/
int 
open(const char *file) 
{
	int fd;
	struct file *f;					// 파일 위치(offset)나 접근 권한 등을 추적하기 위한 핸들 역할
	
	
	lock_acquire(&filelock);		// 동시 접근 방지를 위해 filelock 획득
	f = filesys_open(file);			// 파일 시스템에서 파일 열기
	
	if (f == NULL) {
		lock_release(&filelock);	// 열기에 실패했으므로 락 해제 후 -1 반환
		return -1;
	}

	fd = process_add_file (f);		// 열린 파일을 현재 스레드의 fd_table에 등록하고 fd 할당
	lock_release(&filelock);		// 락 해제 후 fd 반환
	return fd;
}

int filesize(int fd) {
	// TODO
	printf("[stub] filesize() not implemented yet.\n");
	return -1;
}

int read(int fd, void *buffer, unsigned size) {
	// ✅ 유효한 유저 주소인지 확인 (중요!)
	// for (unsigned i = 0; i < size; i++) {
	// 	validate_address((uint8_t *)buffer + i);
	// }

	// if (fd == 0) {
	// 	// ⌨️ 키보드 입력
	// 	for (unsigned i = 0; i < size; i++) {
	// 		((char *)buffer)[i] = input_getc();
	// 	}
	// 	return size;
	// }

	// struct file *file = get_file_from_fd(fd);
	// if (file == NULL)
	// 	return -1;

	// return file_read(file, buffer, size);
}

/***************************************************************
 * write - 열린 파일 또는 콘솔(fd 1)에 buffer로부터 
 * 		   최대 size바이트를 기록하고, 실제 기록한 바이트 수를 반환
 * 
 * 기능:
 *  - 파일의 끝을 넘어서는 쓰기 요청은 파일이 확장되지 않으므로, 가능한 만큼만 기록합니다.
 *  - fd == 1인 경우, 콘솔에 출력하며 putbuf()를 사용해 한 번에 출력합니다.
 * 
 * 매개변수:
 *  - int fd: 기록 대상이 되는 열린 파일의 디스크립터
 *  - const void *buffer: 기록할 데이터가 위치한 버퍼
 *  - unsigned size: 기록할 바이트 수
 ***************************************************************/
int 
write (int fd, const void *buffer, unsigned size) 
{

	/*
	✅ 유저 포인터 유효성 검사
	유저가 전달한 buffer는 유저 공간에 있어야 함
	커널에서 잘못된 주소에 접근하면 page fault 발생
	따라서 접근 전 유효성 체크 필수
	-> 반복문보다 buffer ~ buffer + size - 1까지 한꺼번에 커버하는 검사를 함수화 해보기
		e.g. validate_buffer(buffer, size);
	*/ 
	// for (unsigned i = 0; i < size; i++) {
	// 	validate_address((const uint8_t *)buffer + i);
	// }

	if (fd == 1) {
		putbuf(buffer, size);
		return size;
	}

	/* 
	✅ 파일 디스크립터 -> 파일 구조체 매핑
	유저는 숫자 fd만 넘기므로, 실제 커널 파일 구조체(struct file)를 가져와야 file_write() 호출 가능
	보통 thread_current()->fd_table[fd] 같은 테이블에서 가져옴
	*/ 
	// struct file *file = get_file_from_fd(fd);
	// if (file == NULL)
	// 	return -1;

	// // 4. 실행 중인 파일이면 쓰기 차단
    // if (file == thread_current()->running_file)
    //     return 0;

	/* 
	✅ file_write() 호출
	실제 파일에 데이터를 쓰는 핵심 동작으로, off_t 타입을 반환하므로 리턴값 그대로 유저에게 전달 가능 
	*/
	// return file_write(file, buffer, size);

	// void putbuf(const char *buffer, size_t n);
	// off_t file_write(struct file *file, const void *buffer, off_t size);

}

void seek(int fd, unsigned position) {
	// TODO
	printf("[stub] seek() not implemented yet.\n");
}

unsigned tell(int fd) {
	// TODO
	printf("[stub] tell() not implemented yet.\n");
	return 0;
}

void close(int fd) {
	// TODO
	printf("[stub] close() not implemented yet.\n");
}

/* === [4] 주소 유효성 검사 함수 === */

/***************************************************************
 * validate_address - 유저가 전달한 포인터 주소(addr)가 유효한지 검사
 *
 * @addr: 확인할 유저 주소
 *
 * 기능:
 * - is_user_vaddr: addr가 유저 가상 주소(0x80000000 이하)인지 확인
 * - pml4_get_page: 해당 주소가 현재 스레드의 페이지 테이블에 실제 매핑돼 있는지 확인
 * - 위 두 조건 중 하나라도 틀리면 즉시 exit(-1)로 프로세스를 종료
 *
 * 사용 목적:
 * - 시스템 콜에서 유저 포인터가 커널/물리 메모리를 침범하지 않도록 방어
 ***************************************************************/
void validate_address(const uint64_t addr) 
{
	if (!is_user_vaddr(addr) || pml4_get_page(thread_current()->pml4, addr) == NULL) {
		// printf("🌏 don't look up!!!\n");
		exit(-1);
	}
}

void validate_buffer(const void *buffer, size_t size) 
{
	for (size_t i = 0; i < size; i++) {
		validate_address((const uint8_t *)buffer + i);
	}
}

void validate_cstring(const char *s) 
{	// 길이가 확정되지 않은 문자열
	while (true) {
		validate_address(s);
		if (*s == '\0') break;
		s++;
	}
}

/***************************************************************
 * process_add_file - 현재 스레드의 파일 디스크립터 테이블(fd_table)에
 *                    주어진 파일을 등록하고, 사용 가능한 fd를 할당
 *
 * @f: 커널이 open() 시스템콜을 통해 연 파일을 나타내는 포인터 (struct file*)
 *
 * 기능:
 * - 현재 실행 중인 스레드(thread_current())의 fd_table에서 비어 있는 fd 슬롯을 탐색
 * - 가장 빠르게 사용 가능한 파일 디스크립터(fd)를 찾아 해당 위치에 파일 포인터 등록
 * - fd 할당 후, next_fd 힌트를 필요시 한 칸 앞으로 갱신하여 다음 탐색 효율을 높임
 *
 * 사용 목적:
 * - 시스템 콜 open()을 통해 열린 파일을 현재 프로세스에 등록하고 fd로 추상화
 * - 유저 프로그램은 파일을 직접 다룰 수 없기 때문에, 정수형 fd를 통해 간접적으로 접근
 *
 * 반환값:
 * - 성공 시: 등록된 fd 값 (2 이상 정수)
 * - 실패 시: -1 (모든 fd 슬롯이 사용 중일 경우)
 ***************************************************************/
int
process_add_file (struct file *f)
{
	struct thread *cur = thread_current();
	int fd = 2;				// 항상 2부터 탐색 (stdin=0, stdout=1 제외)

	// printf("🗄️ process is adding file...\n");
	while (fd < FD_MAX && cur->fd_table[fd] != NULL) {
		fd ++;				// 현재 fd가 사용 중이면 다음 슬롯으로 이동
		// printf("1️⃣ fd is now %d\n", fd);
	}
	
	if (fd >= FD_MAX) {
		return -1;			// 유효한 슬롯을 찾지 못했다면 실패 처리
	}

	cur->fd_table[fd] = f;	// 비어있는 슬롯을 찾으면 파일 포인터 등록

	if (cur->next_fd == fd) {
		cur->next_fd++;		// 이번에 할당한 fd가 next_fd라면 next_fd를 한 칸 이동
		// printf("2️⃣ fd is now %d\n", fd);
	}

	// printf("3️⃣ fd is now %d\n", fd);
	return fd;	// 유저에게 fd를 반환 → 이 값을 통해 이후 read/write/close 등을 수행
}