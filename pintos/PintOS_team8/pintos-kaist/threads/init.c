#include "threads/init.h"
#include <console.h>
#include <debug.h>
#include <limits.h>
#include <random.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "devices/kbd.h"
#include "devices/input.h"
#include "devices/serial.h"
#include "devices/timer.h"
#include "devices/vga.h"
#include "threads/interrupt.h"
#include "threads/io.h"
#include "threads/loader.h"
#include "threads/malloc.h"
#include "threads/mmu.h"
#include "threads/palloc.h"
#include "threads/pte.h"
#include "threads/thread.h"
#ifdef USERPROG
#include "userprog/process.h"
#include "userprog/exception.h"
#include "userprog/gdt.h"
#include "userprog/syscall.h"
#include "userprog/tss.h"
#endif
#include "tests/threads/tests.h"
#ifdef VM
#include "vm/vm.h"
#endif
#ifdef FILESYS
#include "devices/disk.h"
#include "filesys/filesys.h"
#include "filesys/fsutil.h"
#endif

/* Page-map-level-4 with kernel mappings only. */
uint64_t *base_pml4;

#ifdef FILESYS
/* -f: Format the file system? */
static bool format_filesys;
#endif

/* -q: Power off after kernel tasks complete? */
bool power_off_when_done;

bool thread_tests;

static void bss_init (void);
static void paging_init (uint64_t mem_end);

static char **read_command_line (void);
static char **parse_options (char **argv);
static void run_actions (char **argv);
static void usage (void);

static void print_stats (void);


int main (void) NO_RETURN;

/* Pintos main program. */
/* main(void) → run_actions(argv) */
int
main (void) 
{
	uint64_t mem_end;
	char **argv;

	/* STEP 1: BSS 초기화 (.bss에 있는 정적/전역 변수들을 안전하게 0으로 설정) */
	bss_init ();

	/* STEP 2: 커맨드라인 파싱 */
	argv = read_command_line ();	// 커맨드라인 문자열을 공백 단위로 분리하여 argv[] 배열로 구성
	argv = parse_options (argv);	// argv[]에서 실행 명령(run ...)과 부팅 옵션(--format 등)을 분리하여 설정

	/* STEP 3: 커널 스레드 시스템 및 콘솔 초기화 */
	thread_init ();					// 현재 실행 중인 커널 스택을 main thread로 인식하고 스레드 시스템 자료구조 초기화
	console_init ();				// 콘솔 락 초기화 (printf, putchar 등 콘솔 출력을 락 기반으로 동기화하도록 설정)

	/* STEP 4: 메모리 시스템 초기화 */
	mem_end = palloc_init ();		// 물리 메모리 페이지 할당자 초기화 (페이지 풀 구성)
	malloc_init ();					// 커널 힙 메모리 할당자 초기화 (malloc 사용 가능해짐)
	paging_init (mem_end);			// 가상 메모리 페이징 시스템 초기화 (page table 등 설정)

#ifdef USERPROG
	/* STEP 5: 유저 모드 실행을 위한 세그먼트 및 TSS 설정 */
	tss_init ();					// 유저 → 커널 전환 시 사용할 TSS(Task State Segment) 설정
	gdt_init ();					// 세그먼트 기반 보호 모드를 위한 GDT(Global Descriptor Table) 설정
#endif

	/* STEP 6: 인터럽트 및 디바이스 초기화 */
	intr_init ();					// 인터럽트 벡터(IDT), PIC 등 인터럽트 컨트롤러 초기화
	timer_init ();					// 하드웨어 타이머 초기화 (시스템 tick 증가, 선점형 스케줄링에 필요)
	kbd_init ();					// 키보드 디바이스 초기화 및 인터럽트 핸들러 등록
	input_init ();					// 키보드 입력 버퍼 초기화	

#ifdef USERPROG
	exception_init ();				// 예외 처리 핸들러 초기화 (page fault, divide-by-zero 등)
	syscall_init ();				// 시스템 콜 핸들러 초기화 (int 0x30 → syscall_handler)
#endif

	/* STEP 7: idle 스레드 생성 및 인터럽트 활성화 (스케줄러 시작) */
	thread_start ();				// idle 스레드 생성 후 ready_list에 추가
									// 인터럽트를 켜서 타이머 인터럽트를 통한 스케줄러 동작 가능하게 함
	serial_init_queue ();			// 시리얼 포트 큐 초기화 (QEMU용 디바이스 출력 버퍼 등)
	timer_calibrate ();				// busy wait 루프 정확도 측정을 위한 CPU 루프 속도 보정

#ifdef FILESYS
	/* STEP 8: 파일 시스템 초기화 */
	disk_init ();					// 하드디스크(가상 디스크) 장치 초기화
	filesys_init (format_filesys);	// 파일 시스템 초기화 (옵션에 따라 포맷 여부 결정)
#endif

#ifdef VM
	/* STEP 9: 가상 메모리 서브시스템 초기화 */
	vm_init ();						// supplemental page table, frame table, swap table 초기화
#endif

	/* STEP 10: 유저 프로그램 실행 */
	printf ("Boot complete.\n");	// 커널 부팅 완료 메시지 출력
	run_actions (argv);				// parse_options()로부터 받은 명령어를 실행
									// 예: run 'echo x y z' → process_create_initd("echo x y z")

	/* STEP 11: 종료 조건 처리 */
	if (power_off_when_done)	
		power_off ();				// 설정 옵션에 따라 자동으로 시스템 전원 종료
	thread_exit ();					// main 스레드 종료 요청
									// 더 이상 실행할 스레드가 없으면 idle thread만 남고 시스템 종료 흐름 진입
}

/* Clear BSS */
static void
bss_init (void) {
	/* The "BSS" is a segment that should be initialized to zeros.
	   It isn't actually stored on disk or zeroed by the kernel
	   loader, so we have to zero it ourselves.

	   The start and end of the BSS segment is recorded by the
	   linker as _start_bss and _end_bss.  See kernel.lds. */
	extern char _start_bss, _end_bss;
	memset (&_start_bss, 0, &_end_bss - &_start_bss);
}

/* Populates the page table with the kernel virtual mapping,
 * and then sets up the CPU to use the new page directory.
 * Points base_pml4 to the pml4 it creates. */
static void
paging_init (uint64_t mem_end) {
	uint64_t *pml4, *pte;
	int perm;
	pml4 = base_pml4 = palloc_get_page (PAL_ASSERT | PAL_ZERO);

	extern char start, _end_kernel_text;
	// Maps physical address [0 ~ mem_end] to
	//   [LOADER_KERN_BASE ~ LOADER_KERN_BASE + mem_end].
	for (uint64_t pa = 0; pa < mem_end; pa += PGSIZE) {
		uint64_t va = (uint64_t) ptov(pa);

		perm = PTE_P | PTE_W;
		if ((uint64_t) &start <= va && va < (uint64_t) &_end_kernel_text)
			perm &= ~PTE_W;

		if ((pte = pml4e_walk (pml4, va, 1)) != NULL)
			*pte = pa | perm;
	}

	// reload cr3
	pml4_activate(0);
}

/* Breaks the kernel command line into words and returns them as
   an argv-like array. */
static char **
read_command_line (void) {
	static char *argv[LOADER_ARGS_LEN / 2 + 1];
	char *p, *end;
	int argc;
	int i;

	argc = *(uint32_t *) ptov (LOADER_ARG_CNT);
	p = ptov (LOADER_ARGS);
	end = p + LOADER_ARGS_LEN;
	for (i = 0; i < argc; i++) {
		if (p >= end)
			PANIC ("command line arguments overflow");

		argv[i] = p;
		p += strnlen (p, end - p) + 1;
	}
	argv[argc] = NULL;

	/* Print kernel command line. */
	printf ("Kernel command line:");
	for (i = 0; i < argc; i++)
		if (strchr (argv[i], ' ') == NULL)
			printf (" %s", argv[i]);
		else
			printf (" '%s'", argv[i]);
	printf ("\n");

	return argv;
}

/* Parses options in ARGV[]
   and returns the first non-option argument. */
static char **
parse_options (char **argv) {
	for (; *argv != NULL && **argv == '-'; argv++) {
		char *save_ptr;
		char *name = strtok_r (*argv, "=", &save_ptr);
		char *value = strtok_r (NULL, "", &save_ptr);

		if (!strcmp (name, "-h"))
			usage ();
		else if (!strcmp (name, "-q"))
			power_off_when_done = true;
#ifdef FILESYS
		else if (!strcmp (name, "-f"))
			format_filesys = true;
#endif
		else if (!strcmp (name, "-rs"))
			random_init (atoi (value));
		else if (!strcmp (name, "-mlfqs"))
			thread_mlfqs = true;
#ifdef USERPROG
		else if (!strcmp (name, "-ul"))
			user_page_limit = atoi (value);
		else if (!strcmp (name, "-threads-tests"))
			thread_tests = true;
#endif
		else
			PANIC ("unknown option `%s' (use -h for help)", name);
	}

	return argv;
}

/***************************************************************
 * run_task - run 명령어에 지정된 task(유저 프로그램 또는 테스트)를 실행
 *
 * 기능:
 * - argv[1]에 있는 task 이름을 기준으로 유저 프로세스를 생성하거나
 *   커널 스레드 테스트를 실행함
 *
 * 실행 흐름:
 *   main() → run_actions(argv) → run_task(argv)
 *     ↳ argv = {"run", "echo", "hello", ...}
 *        ↳ argv[1] = "echo"
 *
 * 분기:
 * - USERPROG가 정의되어 있으면:
 *     - thread_tests가 true → run_test(task): 커널 스레드 테스트 실행
 *     - false → process_create_initd(task) → 유저 프로세스 실행
 * - USERPROG가 정의되지 않으면:
 *     - 항상 run_test(task) 실행 (Project 1에서 주로 발생)
 * 
 * 함수 실행:
 * main(void) → run_actions(argv) → run_task(char **argv) → process_create_initd(task)
 ***************************************************************/ 
static void
run_task (char **argv)
{
	const char *task = argv[1];				// argv[1](= 실행할 작업 이름 = 유저 프로그램 or 테스트 전체 문자열)을 task에 저장
	printf ("Executing '%s':\n", task);		// 프로그램 실행 시작 메시지 출력

#ifdef USERPROG
	if (thread_tests){						// thread_tests 플래그가 true면 스레드 테스트 모드
		run_test (task);					// 예: tests/threads/alarm-single 등 실행할 때 사용
	} else {								// 유저 프로그램 실행 모드
		// process_create_initd → initd → process_exec → user main()
		process_wait (process_create_initd (task));		// 자식 유저 프로세스가 끝날 때까지 기다렸다가 복귀
	}
#else
	run_test (task);						// USERPROG 설정이 꺼져 있으면 무조건 스레드 테스트 실행
#endif~
	printf ("Execution of '%s' complete.\n", task);		// 프로그램 실행 완료 메시지 출력
}

/* Executes all of the actions specified in ARGV[]
   up to the null pointer sentinel. */
/* main(void) → run_actions(argv) → run_task(char **argv) */
static void
run_actions (char **argv) 
{
	/* An action. */
	struct action {
		char *name;                       /* Action name. */
		int argc;                         /* # of args, including action name. */
		void (*function) (char **argv);   /* Function to execute action. */
	};

	/* Table of supported actions. */
	static const struct action actions[] = {
		{"run", 2, run_task},
#ifdef FILESYS
		{"ls", 1, fsutil_ls},
		{"cat", 2, fsutil_cat},
		{"rm", 2, fsutil_rm},
		{"put", 2, fsutil_put},
		{"get", 2, fsutil_get},
#endif
		{NULL, 0, NULL},
	};

	while (*argv != NULL) {
		const struct action *a;
		int i;

		/* Find action name. */
		for (a = actions; ; a++)
			if (a->name == NULL)
				PANIC ("unknown action `%s' (use -h for help)", *argv);
			else if (!strcmp (*argv, a->name))
				break;

		/* Check for required arguments. */
		for (i = 1; i < a->argc; i++)
			if (argv[i] == NULL)
				PANIC ("action `%s' requires %d argument(s)", *argv, a->argc - 1);

		/* Invoke action and advance. */
		a->function (argv);
		argv += a->argc;
	}

}

/* Prints a kernel command line help message and powers off the
   machine. */
static void
usage (void) {
	printf ("\nCommand line syntax: [OPTION...] [ACTION...]\n"
			"Options must precede actions.\n"
			"Actions are executed in the order specified.\n"
			"\nAvailable actions:\n"
#ifdef USERPROG
			"  run 'PROG [ARG...]' Run PROG and wait for it to complete.\n"
#else
			"  run TEST           Run TEST.\n"
#endif
#ifdef FILESYS
			"  ls                 List files in the root directory.\n"
			"  cat FILE           Print FILE to the console.\n"
			"  rm FILE            Delete FILE.\n"
			"Use these actions indirectly via `pintos' -g and -p options:\n"
			"  put FILE           Put FILE into file system from scratch disk.\n"
			"  get FILE           Get FILE from file system into scratch disk.\n"
#endif
			"\nOptions:\n"
			"  -h                 Print this help message and power off.\n"
			"  -q                 Power off VM after actions or on panic.\n"
			"  -f                 Format file system disk during startup.\n"
			"  -rs=SEED           Set random number seed to SEED.\n"
			"  -mlfqs             Use multi-level feedback queue scheduler.\n"
#ifdef USERPROG
			"  -ul=COUNT          Limit user memory to COUNT pages.\n"
#endif
			);
	power_off ();
}


/* Powers down the machine we're running on,
   as long as we're running on Bochs or QEMU. */
void
power_off (void) {
#ifdef FILESYS
	filesys_done ();
#endif

	print_stats ();

	printf ("Powering off...\n");
	outw (0x604, 0x2000);               /* Poweroff command for qemu */
	for (;;);
}

/* Print statistics about Pintos execution. */
static void
print_stats (void) {
	timer_print_stats ();
	thread_print_stats ();
#ifdef FILESYS
	disk_print_stats ();
#endif
	console_print_stats ();
	kbd_print_stats ();
#ifdef USERPROG
	exception_print_stats ();
#endif
}
