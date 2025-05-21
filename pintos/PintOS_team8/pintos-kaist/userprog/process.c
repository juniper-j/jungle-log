#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userprog/gdt.h"
#include "userprog/tss.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/mmu.h"
#include "threads/vaddr.h"
#include "intrinsic.h"
#ifdef VM
#include "vm/vm.h"
#endif

static void process_cleanup (void);
static bool load (const char *file_name, struct intr_frame *if_);
static void initd (void *f_name);
static void __do_fork (void *);

/* General process initializer for initd and other process. */
static void
process_init (void) {
	struct thread *current = thread_current ();
}

/***************************************************************
 * process_create_initd - 최초 유저 프로그램(initd)을 실행하는 함수
 *
 * 기능:
 * - 주어진 command line 문자열(file_name)을 실행할 새로운 커널 스레드를 생성함
 * - 생성된 스레드는 initd()를 시작점으로 실행되며,
 *   initd 내부에서 command line을 파싱하여 실제 파일 이름만 추출한 뒤
 *   process_exec()을 호출해 유저 ELF 파일을 로드하고 유저 모드로 진입함
 *
 * 주의사항:
 * - 이 함수는 PintOS 부팅 후 단 한 번만 호출되어야 하며,
 *   생성된 initd는 이후 모든 유저 프로세스의 루트(parent) 역할을 수행함
 * - thread_create()는 비동기적으로 실행되므로, 자식 스레드는
 *   이 함수가 리턴되기 전에 실행되거나 종료될 수도 있음 (동기화 주의)
 * - file_name 문자열은 부모 스레드의 사용자 스택에 있을 수 있으므로,
 *   race condition을 방지하기 위해 커널 힙 페이지에 복사한 후 전달함
 *
 * 매개변수:
 * - const char *file_name:
 *     실행할 유저 프로그램 이름과 인자를 포함한 command line 전체 문자열.
 *     예: "echo hello world"
 *     실제 initd()에서 이 문자열을 파싱하여 "echo"만 추출한 뒤 실행함.
 *
 * 반환값:
 * - 성공 시 생성된 스레드의 tid
 * - 실패 시 TID_ERROR
 * 
 * 함수 흐름:
 * main(void) → run_actions(argv) → run_task(char **argv) → process_create_initd(task) 
 *  → thread_create (file_name, PRI_DEFAULT, initd, fn_copy) → initd
 ***************************************************************/
tid_t
process_create_initd (const char *file_name) {
	char *fn_copy;
	tid_t tid;

	/* Make a copy of FILE_NAME.
	 * Otherwise there's a race between the caller and load(). */
	/* 이유: file_name은 caller 함수의 지역 변수일 수 있으므로 
			스레드 생성 이후에도 안전하게 사용하려면 복사본이 필요합니다. */
	fn_copy = palloc_get_page (0);			// 페이지 단위로 복사 공간 확보
	if (fn_copy == NULL)
		return TID_ERROR;
	strlcpy (fn_copy, file_name, PGSIZE);	// 안전하게 최대 페이지 크기만큼 file_name 문자열을 복사

	/* Argument Passing (실행파일 이름만 추출) */
	/* 실행파일 이름만 추출해, 이 이름을 기반으로 스레드를 생성할 때 사용한다. 
	   실제로 여기서는 argv도 안 만들고, rsp도 안건든다. 아직 부모가 자식 스레드를 생성하기 전이며,
	   인자를 어떻게 넣을지는 자식 스레드가 실행될 때 처리한다.*/
	char *save_ptr;
    strtok_r(file_name, " ", &save_ptr);

	/* Create a new thread to execute FILE_NAME. */
	tid = thread_create (file_name, PRI_DEFAULT, initd, fn_copy);	// → 🔥 여기서 fn_copy가 initd()의 f_name으로 전달됨
	if (tid == TID_ERROR)
		palloc_free_page (fn_copy);			// 스레드 생성 실패 시 메모리 누수 방지
	return tid;								// 생성된 스레드의 tid 반환
}

/***************************************************************
 * initd - PintOS에서 최초로 실행되는 유저 프로그램의 시작 함수
 *
 * 기능:
 * - 현재 커널 스레드를 유저 프로세스처럼 초기화하고
 * - 전달된 파일 이름(f_name)을 기반으로 유저 프로그램을 메모리에 로드
 * - 로딩이 완료되면 do_iret()을 통해 유저 모드 main() 함수로 진입
 *
 * 상세 설명:
 * - thread_create()에 의해 생성된 스레드의 진입점으로 실행됨
 * - process_exec(f_name) 내부에서 load(), setup_stack() 등을 수행
 * - 성공 시 복귀하지 않으며, 유저 코드(main)가 실행됨
 * - 실패 시 커널 패닉 발생
 *
 * 매개변수:
 * - f_name: 실행할 유저 프로그램의 이름 (fn_copy)
 ***************************************************************/
static void
initd (void *f_name) {
	// struct thread *curr = thread_current();

	// // ✅ fd table 초기화
	// curr->fdt = palloc_get_page(PAL_ZERO);
	// if (curr->fdt == NULL)
	// 	PANIC("Failed to allocate file descriptor table");

	// for (int i = 0; i < FD_MAX; i++)
	// 	curr->fdt[i] = NULL;

	// curr->next_fd = 2;

#ifdef VM
	/* STEP 1: 가상 메모리 기능이 켜져 있다면 SPT(보조 페이지 테이블) 초기화 
	 * - 페이지 폴트 처리 시 사용할 supplemental page table을 생성합니다.
	 * - 이를 통해 lazy loading, stack growth, mmap 등을 구현할 수 있습니다. */
	supplemental_page_table_init (&thread_current ()->spt);
#endif
	/* STEP 2: 유저 프로세스를 위한 기본 구조 초기화 (fd table 등) 
	 * - 파일 디스크립터 테이블, 자식 리스트, lock 등 프로세스 관리에 필요한 구조를 설정합니다.
	 * - process_exec()에서 사용할 수 있도록 준비하는 과정입니다. */
	process_init ();

	/* STEP 3: 파일 이름 기반으로 유저 프로그램을 로드하고, 유저 모드로 진입 
	 * - ELF 실행 파일을 메모리에 로딩하고, 유저 스택 구성 및 인자 설정을 수행합니다.
	 * - 성공하면 do_iret()를 통해 유저 모드로 진입하며, 이후 이 함수는 더 이상 실행되지 않습니다.
	 * - 실패 시 커널 패닉을 발생시켜 시스템을 중단합니다. */
	if (process_exec (f_name) < 0)
		PANIC("Fail to launch initd\n");

	/* STEP 4: 유저 프로그램이 실행되면 이 함수는 절대 복귀하지 않음 */
	NOT_REACHED ();	
}

/* Clones the current process as `name`. Returns the new process's thread id, or
 * TID_ERROR if the thread cannot be created. */
tid_t
process_fork (const char *name, struct intr_frame *if_ UNUSED) {
	/* Clone current thread to new thread.*/
	return thread_create (name,
			PRI_DEFAULT, __do_fork, thread_current ());
}

#ifndef VM
/* Duplicate the parent's address space by passing this function to the
 * pml4_for_each. This is only for the project 2. */
static bool
duplicate_pte (uint64_t *pte, void *va, void *aux) {
	struct thread *current = thread_current ();
	struct thread *parent = (struct thread *) aux;
	void *parent_page;
	void *newpage;
	bool writable;

	/* 1. TODO: If the parent_page is kernel page, then return immediately. */

	/* 2. Resolve VA from the parent's page map level 4. */
	parent_page = pml4_get_page (parent->pml4, va);

	/* 3. TODO: Allocate new PAL_USER page for the child and set result to
	 *    TODO: NEWPAGE. */

	/* 4. TODO: Duplicate parent's page to the new page and
	 *    TODO: check whether parent's page is writable or not (set WRITABLE
	 *    TODO: according to the result). */

	/* 5. Add new page to child's page table at address VA with WRITABLE
	 *    permission. */
	if (!pml4_set_page (current->pml4, va, newpage, writable)) {
		/* 6. TODO: if fail to insert page, do error handling. */
	}
	return true;
}
#endif

/* A thread function that copies parent's execution context.
 * Hint) parent->tf does not hold the userland context of the process.
 *       That is, you are required to pass second argument of process_fork to
 *       this function. */
static void
__do_fork (void *aux) {
	struct intr_frame if_;
	struct thread *parent = (struct thread *) aux;
	struct thread *current = thread_current ();
	/* TODO: somehow pass the parent_if. (i.e. process_fork()'s if_) */
	struct intr_frame *parent_if;
	bool succ = true;

	/* 1. Read the cpu context to local stack. */
	memcpy (&if_, parent_if, sizeof (struct intr_frame));

	/* 2. Duplicate PT */
	current->pml4 = pml4_create();
	if (current->pml4 == NULL)
		goto error;

	process_activate (current);
#ifdef VM
	supplemental_page_table_init (&current->spt);
	if (!supplemental_page_table_copy (&current->spt, &parent->spt))
		goto error;
#else
	if (!pml4_for_each (parent->pml4, duplicate_pte, parent))
		goto error;
#endif

	/* TODO: Your code goes here.
	 * TODO: Hint) To duplicate the file object, use `file_duplicate`
	 * TODO:       in include/filesys/file.h. Note that parent should not return
	 * TODO:       from the fork() until this function successfully duplicates
	 * TODO:       the resources of parent.*/

	process_init ();

	/* Finally, switch to the newly created process. */
	if (succ)
		do_iret (&if_);
error:
	thread_exit ();
}

/*************************************************************
 * process_exec - 현재 커널 스레드의 사용자 실행 컨텍스트를 교체
 *
 * 기능:
 * - 주어진 프로그램 이름 및 인자를 기반으로 새로운 사용자 프로세스를 실행
 * - 기존 프로세스의 메모리와 자원을 정리한 후, ELF 실행 파일을 로드하고
 *   사용자 스택과 레지스터를 구성한 뒤 유저 모드로 진입함
 *
 * 매개변수:
 * - f_name: 실행할 프로그램 이름과 인자를 포함한 문자열 (예: "echo arg1 arg2")
 *
 * 반환값:
 * - 성공 시: 반환하지 않음 (유저 모드로 전환)
 * - 실패 시: -1 반환
 *
 * 특징:`
 * - 기존 스레드의 실행 컨텍스트를 완전히 교체함
 * - 성공 시 이 함수는 절대 반환하지 않으며, 유저 프로그램이 main부터 실행됨
 * - argument_stack 호출 이후 스택 포인터(rsp)를 기반으로 레지스터 세팅 필요
 *
 * 사용 예시:
 * - initd(), syscall_exec() 등에서 새로운 사용자 프로세스를 실행할 때 호출됨
 * - fork 이후 자식 프로세스가 새로운 실행 파일로 context를 덮어쓸 때 사용
 *************************************************************/
int
process_exec (void *f_name) 
{
	char *file_name = f_name;
	bool success;

	/* 유저 프로그램 실행을 위해 사용할 인터럽트 프레임 초기화
	- 인터럽트 프레임은 사용자 모드 전환 시 CPU 레지스터(rip, rsp, rflags 등)의 값을 저장하는 구조체
	- 이 값들을 바탕으로 do_iret()을 통해 유저 모드로 진입하게 됨 */
	struct intr_frame _if;
	_if.ds = _if.es = _if.ss = SEL_UDSEG;     // 데이터 세그먼트: 유저 데이터
	_if.cs = SEL_UCSEG;                       // 코드 세그먼트: 유저 코드
	_if.eflags = FLAG_IF | FLAG_MBS;          // 인터럽트 허용 플래그 설정 + 반드시 설정해야 하는 비트

	/* 현재 실행 중인 프로세스의 메모리, 페이지 테이블, 파일 등을 정리 */
	process_cleanup ();

	/* Command Line Parsing (실제 argument stack 세팅) */

	/* ------------------ [1단계: 파일 이름 복사본 확보] ------------------ */
	char f_name_copy[128];									// 파괴 가능한 복사본 (stack에 위치)
	strlcpy(f_name_copy, file_name, sizeof(f_name_copy));	// Null 종료 문자 (\0) 포함
	
	/* ------------------ [2단계: 인자 파싱] ------------------ */
	char *arg_parsed[64];									// 각 인자의 포인터 저장할 배열
	// PintOS는 128바이트 제한이 있지만, 테스트에서 사용하는 인자 수는 보통 10~30개 정도로 메모리 절약을 위해 64 사용					
	char *token, *save_ptr;
	int count = 0;											
	// 공백(" ")을 기준으로 문자열을 분리하며, 각 토큰의 시작 주소를 배열에 저장
	for (token = strtok_r(file_name, " ", &save_ptr); 
		 token != NULL; 
		 token = strtok_r(NULL, " ", &save_ptr)) {
		arg_parsed[count++] = token;
	}

	/* ------------------ [3단계: 실행 파일 ELF 로드] ------------------ */
	/* 주어진 실행 파일(file_name)이 ELF 포맷인지 확인한 후,
	   프로그램 헤더를 파싱하여 코드/데이터 세그먼트를 가상 메모리에 매핑한다.
	   - 텍스트(.text)는 읽기+실행, 데이터(.data)는 읽기+쓰기 속성을 부여
	   - 가상 주소 정렬 및 권한 설정까지 포함
	   그 후, ELF 진입 주소(entry point)를 intr_frame.rip에 저장하고,
	   사용자 스택의 초기 위치도 intr_frame.rsp(스택 포인터)에 설정한다.
	   이 과정을 통해 유저 프로그램 실행 준비를 마친다. */
	success = load (file_name, &_if);

	/* ------------------ [4단계: 사용자 스택 세팅] ------------------ */
	/* argument_stack 함수는 다음을 유저 스택에 쌓음:
	   [1] 인자 문자열들 (문자 배열)
	   [2] 각 문자열을 가리키는 포인터들 (argv[])
	   [3] argc 값
	   [4] 가짜 리턴 주소 (보통 0)
	   위 내용을 모두 rsp 아래쪽부터 순서대로 쌓고, rsp를 새로운 위치로 갱신 */
	argument_stack(arg_parsed, count, &_if.rsp);

	/* 유저 프로그램이 main(argc, argv) 형태로 실행될 수 있도록
		레지스터 값을 세팅 (x86-64 리눅스 호출 규약 기준) */
	_if.R.rdi = count;						// 첫 번째 인자: argc
	_if.R.rsi = (char *)_if.rsp + 8;		// 두 번째 인자: argv 배열의 시작 주소

	/* ------------------ [5단계: 스택 구조 디버깅용 출력] ------------------ */
	/* 스택 상의 내용(문자열, 포인터, argc 등)을 16진수로 출력
		argument_stack이 올바르게 구성됐는지 확인하는 데 유용 */
	hex_dump(_if.rsp, _if.rsp, USER_STACK - (uint64_t)_if.rsp, true);

	/* 로딩 실패 시 할당된 페이지 해제 후 -1 반환 */
	palloc_free_page (file_name);
	if (!success)
		return -1;

	/* ------------------ [6단계: 사용자 모드로 전환] ------------------ */
	/* 준비한 인터럽트 프레임을 기반으로 유저 모드로 진입
	   CPU 레지스터와 스택을 세팅한 뒤, 유저 코드의 entry point에서 실행 시작 */
	do_iret (&_if);

	NOT_REACHED ();							// do_iret는 반환하지 않으므로, 이 코드는 도달하지 않음
}

/***************************************************************
 * argument_stack - 사용자 프로그램의 인자들을 스택에 쌓아 main(argc, argv) 형태로 전달
 *
 * 기능:
 * - 인자 문자열들을 스택에 복사
 * - 8바이트 정렬 패딩을 추가
 * - 각 문자열의 주소(argv[i])와 NULL(argv[argc])를 역순으로 저장
 * - 가짜 return address도 스택에 추가
 * 
 * 매개변수:
 * - argv: 파싱된 인자 문자열 배열 (예: ["echo", "foo", "bar"])
 * - argc: 인자의 개수
 * - rsp: 사용자 스택 포인터의 주소 (이 값을 아래로 내리며 스택 구성)
 *
 * 주의사항:
 * - rsp는 PHYS_BASE에서 시작하여 점진적으로 감소해야 하며,
 *   유저 가상 주소 공간을 벗어나지 않도록 주의해야 합니다.
 * - 결과적으로 rsp는 main(argc, argv) 호출을 위한 완성된 스택을 가리킵니다.
 ***************************************************************/
void 
argument_stack (char **argv, int argc, void **rsp)
{	/* arg_parsed: 프로그램 이름과 인자가 담긴 배열
	   count: 인자의 개수
	   rsp: 스택 포인터를 가리키는 주소 값 */

	/* ------------------ [1단계: 프로그램 이름(Name), 인자 문자열(Data) push] ------------------ */
    for (int i = argc - 1; i >= 0; i--) {		// 인자를 뒤에서부터 push
		int argv_len = strlen(argv[i]);			// 현재 인자의 문자열 길이 저장

        for (int j = argv_len; j >= 0; j--) {	// '\0' 포함해 역순으로 문자 push
            (*rsp)--;							// 스택 포인터 1바이트 감소
            **(char **)rsp = argv[i][j];		// 해당 주소에 문자 저장
        }
        argv[i] = *(char **)rsp;	// rsp가 가리키는 주소를 argv[i]에 저장 (나중에 주소 배열 push 시 사용)
    }

    /* ------------------ [2단계: 패딩 정렬 push] ------------------ */
    int padding = (int)*rsp % 8;				// 8바이트 정렬을 위해 필요한 패딩 계산
    for (int i = 0; i < padding; i++) {			
        (*rsp)--;								// 1바이트 씩 스택 감소
        **(uint8_t **)rsp = 0; 					// 해당 위치에 0 저장 (패딩용)
    }

	/* ------------------ [3단계: 인자 문자열 종료를 나타내는 0 push] ------------------ */
    (*rsp) -= 8;								// 8바이트 공간 확보
    **(char ***)rsp = 0; 						// 마지막 argv 뒤에 NULL 포인터 추가


	/* ------------------ [4단계: 각 인자 문자열의 주소 push] ------------------ */
    for (int i = argc - 1; i > -1; i--) {
        (*rsp) -= 8; 							// 8바이트 공간 확보
        **(char ***)rsp = argv[i]; 				// 저장해뒀던 각 인자의 시작 주소를 push
    }

	/* ------------------ [5단계: return address push] ------------------ */
    (*rsp) -= 8;								// 8바이트 공간 확보
    **(void ***)rsp = 0;						// 리턴 주소 dummy (실행 종료 시 사용)
}


// 혼자 해보기
// void
// argument_stack (char **argv, int argc, struct intr_frame *if_)
// {
// 	int stack_shift;		// 포인터를 이동시킬 단위
// 	int stack_ptr;			// 포인터
	
// 	/* ------------------ [1단계: 프로그램 이름(Name), 인자 문자열(Data) push] ------------------ */
	
// }


/* Waits for thread TID to die and returns its exit status.  If
 * it was terminated by the kernel (i.e. killed due to an
 * exception), returns -1.  If TID is invalid or if it was not a
 * child of the calling process, or if process_wait() has already
 * been successfully called for the given TID, returns -1
 * immediately, without waiting.
 *
 * This function will be implemented in problem 2-2.  For now, it
 * does nothing. */
int
process_wait (tid_t child_tid UNUSED) 
{
	/* XXX: Hint) The pintos exit if process_wait (initd), we recommend you
	 * XXX:       to add infinite loop here before
	 * XXX:       implementing the process_wait. */
	for (int i = 0; i < 100000000; i++) {}
	for (int i = 0; i < 100000000; i++) {}
	for (int i = 0; i < 100000000; i++) {}
	
	return -1;
}

/* Exit the process. This function is called by thread_exit (). */
void
process_exit (void) {
	struct thread *curr = thread_current ();
	/* TODO: Your code goes here.
	 * TODO: Implement process termination message (see
	 * TODO: project2/process_termination.html).
	 * TODO: We recommend you to implement process resource cleanup here. */

	process_cleanup ();
}

/* Free the current process's resources. */
static void
process_cleanup (void) {
	struct thread *curr = thread_current ();

#ifdef VM
	supplemental_page_table_kill (&curr->spt);
#endif

	uint64_t *pml4;
	/* Destroy the current process's page directory and switch back
	 * to the kernel-only page directory. */
	pml4 = curr->pml4;
	if (pml4 != NULL) {
		/* Correct ordering here is crucial.  We must set
		 * cur->pagedir to NULL before switching page directories,
		 * so that a timer interrupt can't switch back to the
		 * process page directory.  We must activate the base page
		 * directory before destroying the process's page
		 * directory, or our active page directory will be one
		 * that's been freed (and cleared). */
		curr->pml4 = NULL;
		pml4_activate (NULL);
		pml4_destroy (pml4);
	}
}

/* Sets up the CPU for running user code in the nest thread.
 * This function is called on every context switch. */
void
process_activate (struct thread *next) {
	/* Activate thread's page tables. */
	pml4_activate (next->pml4);

	/* Set thread's kernel stack for use in processing interrupts. */
	tss_update (next);
}

/* We load ELF binaries.  The following definitions are taken
 * from the ELF specification, [ELF1], more-or-less verbatim.  */

/* ELF types.  See [ELF1] 1-2. */
#define EI_NIDENT 16

#define PT_NULL    0            /* Ignore. */
#define PT_LOAD    1            /* Loadable segment. */
#define PT_DYNAMIC 2            /* Dynamic linking info. */
#define PT_INTERP  3            /* Name of dynamic loader. */
#define PT_NOTE    4            /* Auxiliary info. */
#define PT_SHLIB   5            /* Reserved. */
#define PT_PHDR    6            /* Program header table. */
#define PT_STACK   0x6474e551   /* Stack segment. */

#define PF_X 1          /* Executable. */
#define PF_W 2          /* Writable. */
#define PF_R 4          /* Readable. */

/* Executable header.  See [ELF1] 1-4 to 1-8.
 * This appears at the very beginning of an ELF binary. */
struct ELF64_hdr {
	unsigned char e_ident[EI_NIDENT];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

struct ELF64_PHDR {
	uint32_t p_type;
	uint32_t p_flags;
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_align;
};

/* Abbreviations */
#define ELF ELF64_hdr
#define Phdr ELF64_PHDR

static bool setup_stack (struct intr_frame *if_);
static bool validate_segment (const struct Phdr *, struct file *);
static bool load_segment (struct file *file, off_t ofs, uint8_t *upage,
		uint32_t read_bytes, uint32_t zero_bytes,
		bool writable);

/* Loads an ELF executable from FILE_NAME into the current thread.
 * Stores the executable's entry point into *RIP
 * and its initial stack pointer into *RSP.
 * Returns true if successful, false otherwise. */
static bool
load (const char *file_name, struct intr_frame *if_) {
	struct thread *t = thread_current ();
	struct ELF ehdr;
	struct file *file = NULL;
	off_t file_ofs;
	bool success = false;
	int i;

	/* Allocate and activate page directory. */
	t->pml4 = pml4_create ();
	if (t->pml4 == NULL)
		goto done;
	process_activate (thread_current ());

	/* Open executable file. */
	file = filesys_open (file_name);
	if (file == NULL) {
		printf ("load: %s: open failed\n", file_name);
		goto done;
	}

	/* Read and verify executable header. */
	if (file_read (file, &ehdr, sizeof ehdr) != sizeof ehdr
			|| memcmp (ehdr.e_ident, "\177ELF\2\1\1", 7)
			|| ehdr.e_type != 2
			|| ehdr.e_machine != 0x3E // amd64
			|| ehdr.e_version != 1
			|| ehdr.e_phentsize != sizeof (struct Phdr)
			|| ehdr.e_phnum > 1024) {
		printf ("load: %s: error loading executable\n", file_name);
		goto done;
	}

	/* Read program headers. */
	file_ofs = ehdr.e_phoff;
	for (i = 0; i < ehdr.e_phnum; i++) {
		struct Phdr phdr;

		if (file_ofs < 0 || file_ofs > file_length (file))
			goto done;
		file_seek (file, file_ofs);

		if (file_read (file, &phdr, sizeof phdr) != sizeof phdr)
			goto done;
		file_ofs += sizeof phdr;
		switch (phdr.p_type) {
			case PT_NULL:
			case PT_NOTE:
			case PT_PHDR:
			case PT_STACK:
			default:
				/* Ignore this segment. */
				break;
			case PT_DYNAMIC:
			case PT_INTERP:
			case PT_SHLIB:
				goto done;
			case PT_LOAD:
				if (validate_segment (&phdr, file)) {
					bool writable = (phdr.p_flags & PF_W) != 0;
					uint64_t file_page = phdr.p_offset & ~PGMASK;
					uint64_t mem_page = phdr.p_vaddr & ~PGMASK;
					uint64_t page_offset = phdr.p_vaddr & PGMASK;
					uint32_t read_bytes, zero_bytes;
					if (phdr.p_filesz > 0) {
						/* Normal segment.
						 * Read initial part from disk and zero the rest. */
						read_bytes = page_offset + phdr.p_filesz;
						zero_bytes = (ROUND_UP (page_offset + phdr.p_memsz, PGSIZE)
								- read_bytes);
					} else {
						/* Entirely zero.
						 * Don't read anything from disk. */
						read_bytes = 0;
						zero_bytes = ROUND_UP (page_offset + phdr.p_memsz, PGSIZE);
					}
					if (!load_segment (file, file_page, (void *) mem_page,
								read_bytes, zero_bytes, writable))
						goto done;
				}
				else
					goto done;
				break;
		}
	}

	/* Set up stack. */
	if (!setup_stack (if_))
		goto done;

	/* Start address. */
	if_->rip = ehdr.e_entry;

	/* TODO: Your code goes here.
	 * TODO: Implement argument passing (see project2/argument_passing.html). */

	success = true;

done:
	/* We arrive here whether the load is successful or not. */
	file_close (file);
	return success;
}


/* Checks whether PHDR describes a valid, loadable segment in
 * FILE and returns true if so, false otherwise. */
static bool
validate_segment (const struct Phdr *phdr, struct file *file) {
	/* p_offset and p_vaddr must have the same page offset. */
	if ((phdr->p_offset & PGMASK) != (phdr->p_vaddr & PGMASK))
		return false;

	/* p_offset must point within FILE. */
	if (phdr->p_offset > (uint64_t) file_length (file))
		return false;

	/* p_memsz must be at least as big as p_filesz. */
	if (phdr->p_memsz < phdr->p_filesz)
		return false;

	/* The segment must not be empty. */
	if (phdr->p_memsz == 0)
		return false;

	/* The virtual memory region must both start and end within the
	   user address space range. */
	if (!is_user_vaddr ((void *) phdr->p_vaddr))
		return false;
	if (!is_user_vaddr ((void *) (phdr->p_vaddr + phdr->p_memsz)))
		return false;

	/* The region cannot "wrap around" across the kernel virtual
	   address space. */
	if (phdr->p_vaddr + phdr->p_memsz < phdr->p_vaddr)
		return false;

	/* Disallow mapping page 0.
	   Not only is it a bad idea to map page 0, but if we allowed
	   it then user code that passed a null pointer to system calls
	   could quite likely panic the kernel by way of null pointer
	   assertions in memcpy(), etc. */
	if (phdr->p_vaddr < PGSIZE)
		return false;

	/* It's okay. */
	return true;
}

#ifndef VM
/* Codes of this block will be ONLY USED DURING project 2.
 * If you want to implement the function for whole project 2, implement it
 * outside of #ifndef macro. */

/* load() helpers. */
static bool install_page (void *upage, void *kpage, bool writable);

/* Loads a segment starting at offset OFS in FILE at address
 * UPAGE.  In total, READ_BYTES + ZERO_BYTES bytes of virtual
 * memory are initialized, as follows:
 *
 * - READ_BYTES bytes at UPAGE must be read from FILE
 * starting at offset OFS.
 *
 * - ZERO_BYTES bytes at UPAGE + READ_BYTES must be zeroed.
 *
 * The pages initialized by this function must be writable by the
 * user process if WRITABLE is true, read-only otherwise.
 *
 * Return true if successful, false if a memory allocation error
 * or disk read error occurs. */
static bool
load_segment (struct file *file, off_t ofs, uint8_t *upage,
		uint32_t read_bytes, uint32_t zero_bytes, bool writable) {
	ASSERT ((read_bytes + zero_bytes) % PGSIZE == 0);
	ASSERT (pg_ofs (upage) == 0);
	ASSERT (ofs % PGSIZE == 0);

	file_seek (file, ofs);
	while (read_bytes > 0 || zero_bytes > 0) {
		/* Do calculate how to fill this page.
		 * We will read PAGE_READ_BYTES bytes from FILE
		 * and zero the final PAGE_ZERO_BYTES bytes. */
		size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
		size_t page_zero_bytes = PGSIZE - page_read_bytes;

		/* Get a page of memory. */
		uint8_t *kpage = palloc_get_page (PAL_USER);
		if (kpage == NULL)
			return false;

		/* Load this page. */
		if (file_read (file, kpage, page_read_bytes) != (int) page_read_bytes) {
			palloc_free_page (kpage);
			return false;
		}
		memset (kpage + page_read_bytes, 0, page_zero_bytes);

		/* Add the page to the process's address space. */
		if (!install_page (upage, kpage, writable)) {
			printf("fail\n");
			palloc_free_page (kpage);
			return false;
		}

		/* Advance. */
		read_bytes -= page_read_bytes;
		zero_bytes -= page_zero_bytes;
		upage += PGSIZE;
	}
	return true;
}

/* Create a minimal stack by mapping a zeroed page at the USER_STACK */
static bool
setup_stack (struct intr_frame *if_) {
	uint8_t *kpage;
	bool success = false;

	kpage = palloc_get_page (PAL_USER | PAL_ZERO);
	if (kpage != NULL) {
		success = install_page (((uint8_t *) USER_STACK) - PGSIZE, kpage, true);
		if (success)
			if_->rsp = USER_STACK;
		else
			palloc_free_page (kpage);
	}
	return success;
}

/* Adds a mapping from user virtual address UPAGE to kernel
 * virtual address KPAGE to the page table.
 * If WRITABLE is true, the user process may modify the page;
 * otherwise, it is read-only.
 * UPAGE must not already be mapped.
 * KPAGE should probably be a page obtained from the user pool
 * with palloc_get_page().
 * Returns true on success, false if UPAGE is already mapped or
 * if memory allocation fails. */
static bool
install_page (void *upage, void *kpage, bool writable) {
	struct thread *t = thread_current ();

	/* Verify that there's not already a page at that virtual
	 * address, then map our page there. */
	return (pml4_get_page (t->pml4, upage) == NULL
			&& pml4_set_page (t->pml4, upage, kpage, writable));
}
#else
/* From here, codes will be used after project 3.
 * If you want to implement the function for only project 2, implement it on the
 * upper block. */

static bool
lazy_load_segment (struct page *page, void *aux) {
	/* TODO: Load the segment from the file */
	/* TODO: This called when the first page fault occurs on address VA. */
	/* TODO: VA is available when calling this function. */
}

/* Loads a segment starting at offset OFS in FILE at address
 * UPAGE.  In total, READ_BYTES + ZERO_BYTES bytes of virtual
 * memory are initialized, as follows:
 *
 * - READ_BYTES bytes at UPAGE must be read from FILE
 * starting at offset OFS.
 *
 * - ZERO_BYTES bytes at UPAGE + READ_BYTES must be zeroed.
 *
 * The pages initialized by this function must be writable by the
 * user process if WRITABLE is true, read-only otherwise.
 *
 * Return true if successful, false if a memory allocation error
 * or disk read error occurs. */
static bool
load_segment (struct file *file, off_t ofs, uint8_t *upage,
		uint32_t read_bytes, uint32_t zero_bytes, bool writable) {
	ASSERT ((read_bytes + zero_bytes) % PGSIZE == 0);
	ASSERT (pg_ofs (upage) == 0);
	ASSERT (ofs % PGSIZE == 0);

	while (read_bytes > 0 || zero_bytes > 0) {
		/* Do calculate how to fill this page.
		 * We will read PAGE_READ_BYTES bytes from FILE
		 * and zero the final PAGE_ZERO_BYTES bytes. */
		size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
		size_t page_zero_bytes = PGSIZE - page_read_bytes;

		/* TODO: Set up aux to pass information to the lazy_load_segment. */
		void *aux = NULL;
		if (!vm_alloc_page_with_initializer (VM_ANON, upage,
					writable, lazy_load_segment, aux))
			return false;

		/* Advance. */
		read_bytes -= page_read_bytes;
		zero_bytes -= page_zero_bytes;
		upage += PGSIZE;
	}
	return true;
}

/* Create a PAGE of stack at the USER_STACK. Return true on success. */
static bool
setup_stack (struct intr_frame *if_) {
	bool success = false;
	void *stack_bottom = (void *) (((uint8_t *) USER_STACK) - PGSIZE);

	/* TODO: Map the stack on stack_bottom and claim the page immediately.
	 * TODO: If success, set the rsp accordingly.
	 * TODO: You should mark the page is stack. */
	/* TODO: Your code goes here */

	return success;
}
#endif /* VM */
