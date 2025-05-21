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
// #include "threads/pml4.h"   // ✅ 보통 이게 필요함 -> 🟩 TODO:근데 임포트 못하는거 보니 다른 방법 찾아야 함...

void syscall_entry (void);
void syscall_handler (struct intr_frame *);

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
syscall_init (void) {
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
			// 🟩 TODO: syscall_handler에서 반환값 필요
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
			printf("create has called!\n\n");
			break;
		case SYS_REMOVE:
			printf("remove has called!\n\n");
			break;
		case SYS_OPEN:
			printf("open has called!\n\n");
			break;
		case SYS_FILESIZE:
			printf("filesize has called!\n\n");
			break;
		case SYS_READ:
			printf("read has called!\n\n");
			break;
		case SYS_WRITE:
			write(f->R.rdi, f->R.rsi, f->R.rdx);
			break;
		case SYS_SEEK:
			printf("seek has called!\n\n");
			break;
		case SYS_TELL:
			printf("tell has called!\n\n");
			break;
		case SYS_CLOSE:
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

bool create(const char *file, unsigned initial_size) {
	// TODO
	printf("[stub] create() not implemented yet.\n");
	return false;
}

bool remove(const char *file) {
	// TODO
	printf("[stub] remove() not implemented yet.\n");
	return false;
}

int open(const char *file) {
	// TODO
	printf("[stub] open() not implemented yet.\n");
	return -1;
}

int filesize(int fd) {
	// TODO
	printf("[stub] filesize() not implemented yet.\n");
	return -1;
}

int read(int fd, void *buffer, unsigned size) {
	// ✅ 유효한 유저 주소인지 확인 (중요!)
	// for (unsigned i = 0; i < size; i++) {
	// 	check_address((uint8_t *)buffer + i);
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
int write(int fd, const void *buffer, unsigned size) {

	/*
	✅ 유저 포인터 유효성 검사
	유저가 전달한 buffer는 유저 공간에 있어야 함
	커널에서 잘못된 주소에 접근하면 page fault 발생
	따라서 접근 전 유효성 체크 필수
	-> 반복문보다 buffer ~ buffer + size - 1까지 한꺼번에 커버하는 검사를 함수화 해보기
		e.g. validate_buffer(buffer, size);
	*/ 
	// for (unsigned i = 0; i < size; i++) {
	// 	check_address((const uint8_t *)buffer + i);
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

void check_address(const void *addr) {
	if (!is_user_vaddr(addr) || pml4_get_page(thread_current()->pml4, addr) == NULL) {
		exit(-1);  // 유저 주소가 아니거나 매핑 안 된 주소
	}
}
