#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/interrupt.h"
#ifdef VM
#include "vm/vm.h"
#endif


/* States in a thread's life cycle. */
enum thread_status {
	THREAD_RUNNING,     /* Running thread. */
	THREAD_READY,       /* Not running but ready to run. */
	THREAD_BLOCKED,     /* Waiting for an event to trigger. */
	THREAD_DYING        /* About to be destroyed. */
};

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;						/* 커널 스레드 ID (Pintos 전반에서 사용됨) */
typedef tid_t pid_t;					/* 유저 프로세스 ID (논리적으로 tid_t와 같지만 API 분리를 위해 사용) */
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

#define FD_MAX 64						/* 파일 디스크립터 동적 할당 최댓값 (논리적으로 제한) */

/* A kernel thread or user process.
 *
 * Each thread structure is stored in its own 4 kB page.  The
 * thread structure itself sits at the very bottom of the page
 * (at offset 0).  The rest of the page is reserved for the
 * thread's kernel stack, which grows downward from the top of
 * the page (at offset 4 kB).  Here's an illustration:
 *
 *      4 kB +---------------------------------+
 *           |          kernel stack           |
 *           |                |                |
 *           |                |                |
 *           |                V                |
 *           |         grows downward          |
 *           |                                 |
 *           |                                 |
 *           |                                 |
 *           |                                 |
 *           |                                 |
 *           |                                 |
 *           |                                 |
 *           |                                 |
 *           +---------------------------------+
 *           |              magic              |
 *           |            intr_frame           |
 *           |                :                |
 *           |                :                |
 *           |               name              |
 *           |              status             |
 *      0 kB +---------------------------------+
 *
 * The upshot of this is twofold:
 *
 *    1. First, `struct thread' must not be allowed to grow too
 *       big.  If it does, then there will not be enough room for
 *       the kernel stack.  Our base `struct thread' is only a
 *       few bytes in size.  It probably should stay well under 1
 *       kB.
 *
 *    2. Second, kernel stacks must not be allowed to grow too
 *       large.  If a stack overflows, it will corrupt the thread
 *       state.  Thus, kernel functions should not allocate large
 *       structures or arrays as non-static local variables.  Use
 *       dynamic allocation with malloc() or palloc_get_page()
 *       instead.
 *
 * The first symptom of either of these problems will probably be
 * an assertion failure in thread_current(), which checks that
 * the `magic' member of the running thread's `struct thread' is
 * set to THREAD_MAGIC.  Stack overflow will normally change this
 * value, triggering the assertion. */
/* The `elem' member has a dual purpose.  It can be an element in
 * the run queue (thread.c), or it can be an element in a
 * semaphore wait list (synch.c).  It can be used these two ways
 * only because they are mutually exclusive: only a thread in the
 * ready state is on the run queue, whereas only a thread in the
 * blocked state is on a semaphore wait list. */

struct thread {
	/* 스레드 식별 및 상태 관련 필드 */
	tid_t tid;                          /* Thread identifier. */
	enum thread_status status;          /* Thread state. */
	char name[16];                      /* Name (for debugging purposes). */
	unsigned magic;                     /* Used to detect stack overflow. */

	/* 스케줄링 및 대기 관련 필드 (alarm, ready/blocked list 등) */
	int priority;                       /* Priority. */
	int base_priority;                  /* 기부 이전의 기본 우선순위 */
	int64_t wakeup_ticks;				/* 깨워야 할 시간 (alarm sleep용) */
	int64_t wakeup_tick;                /* 깨어나기까지 남은 시간 */ 
	struct list_elem elem;              /* ready/blocked 리스트용 element */
	struct lock *wait_on_lock;          /* 대기 중인 lock 객체 */
	struct list donations;              /* 우선순위 donations를 추적하기 위한 리스트 */
	struct list_elem d_elem;            /* donations 리스트에 들어갈 element */

	/* Parent thread (used for wait/exit sync). */
	pid_t pid;                      	/* Process ID (user program) */
	struct thread *parent;          	/* Parent thread */
	struct intr_frame parent_if;    	/* Fork 시 전달될 부모의 intr_frame */
	struct list child_list;         	/* 자식 프로세스 리스트 */
	struct list_elem child_elem;    	/* 부모의 child_list에 들어갈 리스트 노드 */
	struct semaphore *sema_wait;    	/* wait()용 동기화 */
	struct semaphore *sema_exit;    	/* exit()용 동기화 */
	struct semaphore *sema_fork;    	/* fork 완료 여부 대기 */

	/* File Descriptor Table (FDT) */
	struct file *fd_table[FD_MAX];      /* 파일 디스크립터 테이블 */
	int next_fd;                        /* 다음 파일 디스크립터 번호 (보통 2부터 시작) */
	struct file *running;				/* 현재 실행중인 파일 */
	
#ifdef USERPROG
	/* Owned by userprog/process.c. */
	uint64_t *pml4;                     /* Page map level 4 for this thread (address space). */
#endif

#ifdef VM
	/* Table for whole virtual memory owned by thread. */
	struct supplemental_page_table spt;
#endif

	/* Owned by thread.c. */
	struct intr_frame tf;              /* Used during context switching. */
};


/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

/* THREADS #1. Alarm Clock */
void thread_sleep (int64_t ticks);
void thread_awake (int64_t global_ticks);
void update_closest_tick (int64_t ticks);
int64_t closest_tick (void);
bool cmp_priority(const struct list_elem *a, const struct list_elem *b, void *aux);
bool cmp_priority_only(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);
bool cmp_priority_donation(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);
void preempt_priority(void);
bool cmp_sema_priority(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED);

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);
bool cmp_wakeTick(struct list_elem *a, struct list_elem *b, void *aux UNUSED);
void thread_block (void);
void thread_unblock (struct thread *);

// 다른 곳에서도 사용처를 만들어야 하여 헤더에 선언.
void preempt_priority(void);

struct thread *thread_current (void);
tid_t thread_tid (void);
void thread_sleep(int64_t ticks);
void thread_wakeUp(int64_t curTick);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);
int thread_get_recent_cpu (void);
int thread_get_load_avg (void);

void do_iret (struct intr_frame *tf);

#endif /* threads/thread.h */