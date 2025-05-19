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


/* === [1] MSR 레지스터 초기화: syscall 진입점 설정 === lol */ 
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
syscall_handler (struct intr_frame *f) {
	// TODO: Your implementation goes here.
	printf("syscall handler has called. \n");

	switch(f->R.rax) {
	case SYS_HALT:
		printf("half has called!\n\n");
		halt();
		break;
	case SYS_EXIT:
		printf("exit has called!\n\n");
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
		write(f->R.rdi, (void *)f->R.rsi, f->R.rdx);
		break;
	case SYS_WRITE:
		printf("write has called!\n\n");
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
	printf("Exiting with status: %d\n", status);
	thread_exit();
}

tid_t fork(const char *thread_name) {
	// TODO
	printf("[stub] fork() not implemented yet.\n");
	return -1;
}

int exec(const char *cmd_line) {
	// TODO
	printf("[stub] exec() not implemented yet.\n");
	return -1;
}

int wait(tid_t pid) {
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

int write(int fd, const void *buffer, unsigned size) {
	// ✅ 유저 포인터 유효성 검사
	// for (unsigned i = 0; i < size; i++) {
	// 	check_address((const uint8_t *)buffer + i);
	// }

	// if (fd == 1) {
	// 	putbuf(buffer, size);
	// 	return size;
	// }

	// struct file *file = get_file_from_fd(fd);
	// if (file == NULL)
	// 	return -1;

	// return file_write(file, buffer, size);
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


// void exit(int status)
// {
// 	printf("Exiting : \n");
// 	printf("Name of process: exit(%d)\n", status);
// 	thread_exit();
// }

// tid_t fork (const char *thread_name)
// {
// 	// TODO:
// 	// Create child process and execute program corresponds to cmd_line on it
// 	printf("Not implemented yet");
// }

// int exec(const char *cmd_line)
// {
// 	// TODO :
// 	// Wait for termination of child process whose process id is pid
// 	printf("Not implemented yet");
// }
// // 아니 슬라이드에서는 pid_t를 리턴하는 구현을 요구하는데 뭐지?

// // 여기까지 4개가 System과 직접 연관 된 내용들!

// /*
// 	thread.h 에서 해야 할 일
// 	Pointer to parent process : struct thread*
// 	Pointer to the sibling : struct list
// 	Pointer to the children : struct list_elem
// 	추가 : 시스템 콜과 연계되어야 하여 우선 작성
// */

// int wait(tid_t pid)
// {
// 	// 🟩 TODO: 
// 	printf("Not implemented yet");
// 	// TODO :
// 	// wait for a child process pid to exit and retrieve the child's exit status.
// 	// IF : PID is alive
// 		// wait till it terminates.
// 		// Return the status that pid passed to exit.
// 	// IF : PID did not call exit but was terminated by the kernel, return -1
// 	// A parent process cna call wait for the cild process that has terminated
// 		// - return exit status of the terminated child processes.

// 	// After the child terminates, the parent should deallocatge its process descriptor
// 		// wait fails and return -1 if
// 			// pid does not refer to a direct child of the calling process.
// 			// the process that calls wait has already called wait on pid.
// }
// bool create(const char *file, unsigned initial_size)
// {
// 	printf("Not implemented yet");
// 	// Create file which have size of initial_size
// 	// use bool filesys_create const chr *name, off_t initial_size.
// 	// return true if it is succeeded or false if it is not.
// 	return false;
// }

// bool remove(const char *file)
// {
// 	printf("Not implemented yet");
// 	// Remove file whose name is file.
// 	// Use bool filesys_remove(const char *name)
// 	// Return true if it is succeeded or false if it is not.
// 	// File is removed regardless of whether it is open or closed.
// 	return false;
// }

// int open(const char *file)
// {
// 	printf("Not implemented yet");
// 	// Open the file corresponds to path in "file".
// 	// Return its fd.
// 	// use strung file *filesys_open(const char *name).
// 	return 1;
// }

// int filesize(int fd)
// {
// 	printf("Not implemented yet");
// 	// Return the size, in bytes, of the file open as fd.
// 	// Use off_t file_length(struct file *file).
// 	return 1;
// }

// int read(int fd, void *buffer, unsigned size)
// {
// 	printf("Not implemented yet");
// 	// Read size bytes from the file open as fd into buffer.
// 	// Return the number of bytes actually read (0 at end of file), or -1 if fails.
// 	// If fd is 0, it reads from keyboard using input_getc(), otherwise reads from file using file_read() function.
// 		// uint8_t input_getc(void)
// 		// off_t file_read(struct file *file, void *buffer, off_t size)

// 	// buffer 주소가 유효한지 체크 필요
// 	// if (!is_user_vaddr(buffer) || !is_valid_user_address(buffer)) {
// 	// 	exit(-1);
// 	// }
	
// 	for (unsigned i = 0; i < size; i++) {
// 		check_address((uint8_t *)buffer + i);
// 	}

// 	if (fd == 0) {
// 		for (unsigned i = 0; i < size; i++) {
// 			((char *)buffer)[i] = input_getc();
// 		}
// 		return size;
// 	}
	
// 	struct file *file = get_file_from_fd(fd);
// 	if (file == NULL)
// 		return -1;

// 	return file_read(file, buffer, size);
// }

// int write(int fd, const void *buffer, unsigned size)
// {
// 	printf("Not implemented yet");
// 	if(fd == 1)
// 	{
// 		putbuf(buffer, size);

// 	}
// 	else
// 	{
// 		// off_t file_write(struct file *file, const void *buffer, off_t size)
// 		printf("CANNOT WRITE NOW! Implement first!\n");
// 		// else 영역이 올바르게 구현되지 않았기에 -1 리턴하기.
// 		return -1;
// 	}
// 	// TODO : return 값을 -1로 정의 할 여지를 고민해야함
// 	return size;


// 	// // 🔒 유저 포인터 유효성 검사
// 	// for (unsigned i = 0; i < size; i++) {
// 	// 	check_address((const uint8_t *)buffer + i);
// 	// }

// 	// if (fd == 1) {
// 	// 	putbuf(buffer, size);  // 콘솔 출력
// 	// 	return size;
// 	// }

// 	// // 파일 쓰기
// 	// struct file *file = get_file_from_fd(fd);
// 	// if (file == NULL)
// 	// 	return -1;

// 	// return file_write(file, buffer, size);
// }

// void seek(int fd, unsigned position)
// {
// 	printf("Not implemented yet");
// 	// Changes the next byte to be rtead or written in open file fd to position.
// 	// use void file_seek
// }

// unsigned tell(int fd)
// {
// 	printf("Not implemented yet");
// 	// Return the position of the next byte to be read or written in open file fd.
// 	// use off_t file_tell
// 	return 0;
// }

// void close(int fd)
// {
// 	printf("Not implemented yet");
// 	// close file descriptor fd.
// 	// use void file_close
// }

// void check_address(const void *addr) {
//     if (!is_user_vaddr(addr) || pml4_get_page(thread_current()->pml4, addr) == NULL) {
//         exit(-1);  // 유효하지 않으면 종료
//     }
// }