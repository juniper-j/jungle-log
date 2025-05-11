#include "threads/thread.h"
#include <debug.h>
#include <stddef.h>
#include <random.h>
#include <stdio.h>
#include <string.h>
#include "threads/flags.h"
#include "threads/interrupt.h"
#include "threads/intr-stubs.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "intrinsic.h"
#ifdef USERPROG
#include "userprog/process.h"
#endif

/* Random value for struct thread's `magic' member.
   Used to detect stack overflow.  See the big comment at the top
   of thread.h for details. */
#define THREAD_MAGIC 0xcd6abf4b

/* Random value for basic thread
   Do not modify this value. */
#define THREAD_BASIC 0xd42df210

/* List of processes in THREAD_READY state, that is, processes
   that are ready to run but not actually running. */
static struct list ready_list;

/* #1. Alram Clock */
/* List of all threads, including those in the ready, running,
   and blocked states. */
static struct list sleep_list;  	// 블록된 쓰레드들을 위한 슬립 리스트
// static struct list all_list;		// 전체 스레드 리스트
static int64_t awake_closest_tick;	// 가장 빠르게 깨어날 tick 저장

/* Idle thread. */
static struct thread *idle_thread;

/* Initial thread, the thread running init.c:main(). */
static struct thread *initial_thread;

/* Lock used by allocate_tid(). */
static struct lock tid_lock;

/* Thread destruction requests */
static struct list destruction_req;

/* Statistics. */
static long long idle_ticks;    /* # of timer ticks spent idle. */
static long long kernel_ticks;  /* # of timer ticks in kernel threads. */
static long long user_ticks;    /* # of timer ticks in user programs. */

/* Scheduling. */
#define TIME_SLICE 4            /* # of timer ticks to give each thread. */
static unsigned thread_ticks;   /* # of timer ticks since last yield. */

/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
bool thread_mlfqs;

static void kernel_thread (thread_func *, void *aux);

static void idle (void *aux UNUSED);	// running and ready thread가 없는 경우
static struct thread *next_thread_to_run (void);
static void init_thread (struct thread *, const char *name, int priority);
static void do_schedule(int status);
static void schedule (void);
static tid_t allocate_tid (void);

/* Returns true if T appears to point to a valid thread. */
#define is_thread(t) ((t) != NULL && (t)->magic == THREAD_MAGIC)

/* Returns the running thread.
 * Read the CPU's stack pointer `rsp', and then round that
 * down to the start of a page.  Since `struct thread' is
 * always at the beginning of a page and the stack pointer is
 * somewhere in the middle, this locates the curent thread. */
#define running_thread() ((struct thread *) (pg_round_down (rrsp ())))


// Global descriptor table for the thread_start.
// Because the gdt will be setup after the thread_init, we should
// setup temporal gdt first.
static uint64_t gdt[3] = { 0, 0x00af9a000000ffff, 0x00cf92000000ffff };

/* Initializes the threading system by transforming the code
   that's currently running into a thread.  This can't work in
   general and it is possible in this case only because loader.S
   was careful to put the bottom of the stack at a page boundary.

   Also initializes the run queue and the tid lock.

   After calling this function, be sure to initialize the page
   allocator before trying to create any threads with
   thread_create().

   It is not safe to call thread_current() until this function
   finishes. */
void
thread_init (void) {
	ASSERT (intr_get_level () == INTR_OFF);

	/* Reload the temporal gdt for the kernel
	 * This gdt does not include the user context.
	 * The kernel will rebuild the gdt with user context, in gdt_init (). */
	struct desc_ptr gdt_ds = {
		.size = sizeof (gdt) - 1,
		.address = (uint64_t) gdt
	};
	lgdt (&gdt_ds);

	/* Init the globla thread context */
	lock_init (&tid_lock);
	list_init (&ready_list);
	list_init (&sleep_list);		// ⏰ 슬립 스레드 리스트 초기화
	// list_init (&all_list);			// ⏰ 전체 스레드 리스트 초기화
	list_init (&destruction_req);	

	/* Set up a thread structure for the running thread. */
	initial_thread = running_thread ();
	init_thread (initial_thread, "main", PRI_DEFAULT);
	initial_thread->status = THREAD_RUNNING;
	initial_thread->tid = allocate_tid ();
}

/* Starts preemptive thread scheduling by enabling interrupts.
   Also creates the idle thread. */
void
thread_start (void) {
	/* Create the idle thread. */
	struct semaphore idle_started;
	sema_init (&idle_started, 0);
	thread_create ("idle", PRI_MIN, idle, &idle_started);

	/* Start preemptive thread scheduling. */
	intr_enable ();

	/* Wait for the idle thread to initialize idle_thread. */
	sema_down (&idle_started);
}

/* Called by the timer interrupt handler at each timer tick.
   Thus, this function runs in an external interrupt context. */
void
thread_tick (void) {
	struct thread *t = thread_current ();

	/* Update statistics. */
	if (t == idle_thread)
		idle_ticks++;
#ifdef USERPROG
	else if (t->pml4 != NULL)
		user_ticks++;
#endif
	else
		kernel_ticks++;

	/* Enforce preemption. */
	if (++thread_ticks >= TIME_SLICE)
		intr_yield_on_return ();
}

/* Prints thread statistics. */
void
thread_print_stats (void) {
	printf ("Thread: %lld idle ticks, %lld kernel ticks, %lld user ticks\n",
			idle_ticks, kernel_ticks, user_ticks);
}

/* Creates a new kernel thread named NAME with the given initial
   PRIORITY, which executes FUNCTION passing AUX as the argument,
   and adds it to the ready queue.  Returns the thread identifier
   for the new thread, or TID_ERROR if creation fails.

   If thread_start() has been called, then the new thread may be
   scheduled before thread_create() returns.  It could even exit
   before thread_create() returns.  Contrariwise, the original
   thread may run for any amount of time before the new thread is
   scheduled.  Use a semaphore or some other form of
   synchronization if you need to ensure ordering.

   The code provided sets the new thread's `priority' member to
   PRIORITY, but no actual priority scheduling is implemented.
   Priority scheduling is the goal of Problem 1-3. */
tid_t
thread_create (const char *name, int priority,
		thread_func *function, void *aux) {
	struct thread *t;
	tid_t tid;

	ASSERT (function != NULL);

	/* Allocate thread. */
	t = palloc_get_page (PAL_ZERO);
	if (t == NULL)
		return TID_ERROR;

	/* Initialize thread. */
	init_thread (t, name, priority);
	tid = t->tid = allocate_tid ();

	/* Call the kernel_thread if it scheduled.
	 * Note) rdi is 1st argument, and rsi is 2nd argument. */
	t->tf.rip = (uintptr_t) kernel_thread;
	t->tf.R.rdi = (uint64_t) function;
	t->tf.R.rsi = (uint64_t) aux;
	t->tf.ds = SEL_KDSEG;
	t->tf.es = SEL_KDSEG;
	t->tf.ss = SEL_KDSEG;
	t->tf.cs = SEL_KCSEG;
	t->tf.eflags = FLAG_IF;

	/* Add to run queue. */
	thread_unblock (t);

	return tid;
}

/**********************************************************
 * thread_sleep - 현재 실행 중인 스레드를 지정한 틱 수만큼 재움
 *
 * 기능:
 * - idle_thread는 재우지 않음
 * - 현재 스레드를 sleep_list에 추가하고 BLOCKED 상태로 전환
 * - wakeup_tick을 현재 시간 + ticks로 설정
 * - (global) awake_closest_tick 갱신
 * - thread_block() 호출로 스케줄러 대상에서 제외
 *
 * 동기화:
 * - 인터럽트를 비활성화한 상태에서 sleep_list에 접근
 *
 * 호출:
 * - timer_sleep() 함수에서 호출됨
 **********************************************************/
void
thread_sleep (int64_t ticks) 
{

	struct thread *cur;
	enum intr_level old_level;
	old_level = intr_disable(); // 인터럽트 비활성

	cur = thread_current();     // 현재 스레드
	ASSERT(cur != idle_thread); // 현재 스레드가 idle이 아닐 때만

	cur->wakeup_ticks = ticks;     // 일어날 시각 저장
	list_insert_ordered(&sleep_list, &cur->elem, cmp_thread_ticks, NULL); // sleep_list에 추가
	thread_block(); // 현재 스레드 재우기

	intr_set_level(old_level); // 인터럽트 상태를 원래 상태로 변경

	// struct thread *cur = thread_current();			// 현재 실행중인 thread 포인터를 가져옴

	// if (cur == idle_thread) return;					// idle_thread는 sleep할 필요 없음

	// enum intr_level old_level = intr_disable();		// 인터럽트를 비활성화하여 동기화 확보

	// // cur->wakeup_tick = timer_ticks()+ ticks;		// 현재 시간 기준으로 깨어날 시점 설정
	// update_closest_tick(cur->wakeup_tick = ticks);	// 전역 awake_closest_tick 업데이트
	// list_push_back(&sleep_list, &cur->elem); 		// sleep_list에 현재 스레드 추가
	// thread_block(); 								// 현재 스레드를 BLOCKED 상태로 전환 (스케줄러에서 제외됨)

	// intr_set_level(old_level); 						// 이전 인터럽트 상태로 복원
}

/*************************************************************
 * thread_awake - 슬립 리스트에서 깨울 시간이 지난 스레드들을 깨움
 *
 * 기능:
 * - sleep_list에 있는 스레드 중, wakeup_tick ≤ 현재 tick인 스레드를 READY 상태로 전환
 * - list_remove() 및 thread_unblock()을 통해 스레드 깨움
 * - 남아 있는 스레드들의 wakeup_tick 중 가장 이른 값으로 awake_closest_tick 갱신
 *
 * 동기화:
 * - 인터럽트 컨텍스트에서 실행되므로 별도 락 불필요
 * - list_remove() 시 반복자 갱신에 주의 필요
 *
 * 호출 위치:
 * - timer_interrupt() 내부에서, ticks ≥ awake_closest_tick일 때 호출
 *
 * 제약 조건:
 * - BLOCKED 상태가 아닌 스레드를 깨우면 안 됨 (thread_unblock 제약)
 * - thread_block() 호출 금지 (인터럽트 컨텍스트이므로)
 *
 * 요구사항:
 * - busy-wait 없이 정확한 tick 기반 sleep/wakeup 동작 보장
 * - awake_closest_tick 값을 매 tick마다 갱신하여 불필요한 검사 최소화
 *************************************************************/
void
thread_awake (int64_t current_ticks)
{
	enum intr_level old_level;
	old_level = intr_disable(); // 인터럽트 비활성

	struct list_elem *cur_elem = list_begin(&sleep_list);
	while (cur_elem != list_end(&sleep_list))
	{
		struct thread *cur_thread = list_entry(cur_elem, struct thread, elem); // 현재 검사중인 elem의 스레드

		if (current_ticks >= cur_thread->wakeup_ticks) // 깰 시간이 됐으면
		{
			cur_elem = list_remove(cur_elem); // sleep_list에서 제거, curr_elem에는 다음 elem이 담김
			thread_unblock(cur_thread);        // ready_list로 이동
		}
		else
			break;
	}
	intr_set_level(old_level); // 인터럽트 상태를 원래 상태로 변경

	// awake_closest_tick = INT64_MAX;		// 다음 인터럽트에서 깨어날 스레드가 있는지 확인하기 위해 초기화

	// struct list_elem *sleeping;
	// sleeping = list_begin(&sleep_list);	// sleep_list의 첫 번째 요소부터 순회 시작
	
	// while (sleeping != list_end(&sleep_list)) 
	// {	// sleep_list 끝까지 순회
		
	// 	// list_elem 포인터를 thread 포인터로 변환
	// 	struct thread *th = list_entry(sleeping, struct thread, elem);

    //     if (wakeup_ticks >= th->wakeup_ticks)	{			// 깨어날 시간이 현재 시간(wakeup_tick)을 지났으면
    //         sleeping = list_remove(&th->elem);			// 리스트에서 해당 요소를 제거하고 다음 요소 포인터를 반환
    //         thread_unblock(th);         	    		// 해당 스레드를 READY 상태로 전환   
    //     } else {
    //         sleeping = list_next(sleeping);    			// 다음 요소로 이동
    //         update_closest_tick(th->wakeup_ticks);		// 다음 tick에서 확인해야 할 가장 가까운 tick을 갱신
    //     }
    // }
}

// 두 스레드의 wakeup_ticks를 비교해서 작으면 true를 반환하는 함수
bool cmp_thread_ticks(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED)
{
	struct thread *st_a = list_entry(a, struct thread, elem);
	struct thread *st_b = list_entry(b, struct thread, elem);
	return st_a->wakeup_ticks < st_b->wakeup_ticks;
}

/*************************************************************
 * update_closest_tick - 슬립 리스트 내 최소 wakeup_tick을 갱신
 *
 * 기능:
 * - 현재 tick 값이 awake_closest_tick보다 작으면 갱신
 * - 다음 thread_awake() 시점 결정을 위해 사용됨
 *************************************************************/
void
update_closest_tick (int64_t ticks) 
{
	// 기존 awake_closest_tick보다 더 빠른 tick이라면 갱신
	awake_closest_tick = (awake_closest_tick > ticks) ? ticks : awake_closest_tick;
}

/*************************************************************
 * closest_tick - 현재 저장된 가장 이른 wakeup tick 반환
 *
 * 기능:
 * - thread_awake() 호출 여부 판단을 위해 사용됨
 *************************************************************/
int64_t
closest_tick (void)
{
	return awake_closest_tick;
}

/*************************************************************
 * thread_block - 현재 실행 중인 스레드를 BLOCKED 상태로 전환
 *
 * 기능:
 * - 현재 스레드의 상태를 THREAD_BLOCKED로 설정하고,
 *   스케줄러를 호출하여 다른 스레드로 전환함
 * - 이 함수에 의해 차단된 스레드는 이후 thread_unblock()에 의해 깨워질 때까지 실행되지 않음
 *
 * 제약 조건:
 * - 반드시 인터럽트가 비활성화된 상태에서 호출되어야 함
 *   (스레드 상태 전환 중 동기화 문제 방지 목적)
 *
 * 참고:
 * - 일반적인 조건 변수, 세마포어 등을 통한 동기화에는 synch.h의 고수준 API 사용 권장
 *************************************************************/
void
thread_block (void) {
	ASSERT (!intr_context ());             		// 인터럽트 핸들러 내에서 호출되면 안 됨
	ASSERT (intr_get_level () == INTR_OFF); 	// 인터럽트가 꺼진 상태여야 안전함

	thread_current ()->status = THREAD_BLOCKED; // 현재 스레드 상태를 BLOCKED로 설정
	schedule();                                 // 스케줄러 호출하여 문맥 전환 수행
}

/* Transitions a blocked thread T to the ready-to-run state.
   This is an error if T is not blocked.  (Use thread_yield() to
   make the running thread ready.)

   This function does not preempt the running thread.  This can
   be important: if the caller had disabled interrupts itself,
   it may expect that it can atomically unblock a thread and
   update other data. */
void
thread_unblock (struct thread *t) {
	enum intr_level old_level;

	ASSERT (is_thread (t));

	old_level = intr_disable ();
	ASSERT (t->status == THREAD_BLOCKED);
	list_push_back (&ready_list, &t->elem);
	t->status = THREAD_READY;
	intr_set_level (old_level);
}

/* Returns the name of the running thread. */
const char *
thread_name (void) {
	return thread_current ()->name;
}

/*************************************************************
 * thread_current - 현재 CPU에서 실행 중인 스레드를 반환
 *
 * 기능:
 * - running_thread()를 통해 현재 스택 포인터 기반으로 스레드 구조체를 얻음
 * - 해당 구조체가 유효한 스레드인지 두 가지 ASSERT로 검증
 * - 최종적으로 현재 실행 중인 thread 포인터를 반환
 *
 * 주의:
 * - Pintos는 각 스레드를 독립적인 커널 스택과 페이지에 배치하므로,
 *   스택 포인터를 페이지 기준으로 내림(pg_round_down)하여
 *   현재 스레드를 역추적할 수 있음
 *
 * 검증:
 * - is_thread(t): thread magic number 확인
 * - t->status == THREAD_RUNNING: 실행 중 상태인지 확인
 *************************************************************/
struct thread *
thread_current (void) 
{
	struct thread *t = running_thread (); // 현재 스택 포인터 기반으로 thread 구조체 추론

	/* t가 유효한 스레드인지 확인 (magic 필드 검사) */
	/* 현재 스레드 상태가 실행 중인지 확인 */
	ASSERT (is_thread (t));
	ASSERT (t->status == THREAD_RUNNING);

	return t; // 현재 실행 중인 스레드 포인터 반환
}

/* Returns the running thread's tid. */
tid_t
thread_tid (void) {
	return thread_current ()->tid;
}

/* Deschedules the current thread and destroys it.  Never
   returns to the caller. */
void
thread_exit (void) {
	ASSERT (!intr_context ());

#ifdef USERPROG
	process_exit ();
#endif

	/* Just set our status to dying and schedule another process.
	   We will be destroyed during the call to schedule_tail(). */
	intr_disable ();
	do_schedule (THREAD_DYING);
	NOT_REACHED ();
}

/* Yields the CPU.  The current thread is not put to sleep and
   may be scheduled again immediately at the scheduler's whim. */
void
thread_yield (void) {
	struct thread *curr = thread_current ();	// thread_current()는 현재 실행중인 thread를 리턴
	enum intr_level old_level;

	ASSERT (!intr_context ());

	old_level = intr_disable ();	// intr_disable()는 인터럽트를 비활성화하고 이전 상태를 리턴
	if (curr != idle_thread)
		list_push_back (&ready_list, &curr->elem);
	do_schedule (THREAD_READY);
	intr_set_level (old_level);		// intr_set_level(old_level)은 set a state of interrupt to the state passed
}

/* Sets the current thread's priority to NEW_PRIORITY. */
void
thread_set_priority (int new_priority) {
	thread_current ()->priority = new_priority;
}

/* Returns the current thread's priority. */
int
thread_get_priority (void) {
	return thread_current ()->priority;
}

/* Sets the current thread's nice value to NICE. */
void
thread_set_nice (int nice UNUSED) {
	/* TODO: Your implementation goes here */
}

/* Returns the current thread's nice value. */
int
thread_get_nice (void) {
	/* TODO: Your implementation goes here */
	return 0;
}

/* Returns 100 times the system load average. */
int
thread_get_load_avg (void) {
	/* TODO: Your implementation goes here */
	return 0;
}

/* Returns 100 times the current thread's recent_cpu value. */
int
thread_get_recent_cpu (void) {
	/* TODO: Your implementation goes here */
	return 0;
}

/* Idle thread.  Executes when no other thread is ready to run.

   The idle thread is initially put on the ready list by
   thread_start().  It will be scheduled once initially, at which
   point it initializes idle_thread, "up"s the semaphore passed
   to it to enable thread_start() to continue, and immediately
   blocks.  After that, the idle thread never appears in the
   ready list.  It is returned by next_thread_to_run() as a
   special case when the ready list is empty. */
static void
idle (void *idle_started_ UNUSED) {
	struct semaphore *idle_started = idle_started_;

	idle_thread = thread_current ();
	sema_up (idle_started);

	for (;;) {
		/* Let someone else run. */
		intr_disable ();
		thread_block ();

		/* Re-enable interrupts and wait for the next one.

		   The `sti' instruction disables interrupts until the
		   completion of the next instruction, so these two
		   instructions are executed atomically.  This atomicity is
		   important; otherwise, an interrupt could be handled
		   between re-enabling interrupts and waiting for the next
		   one to occur, wasting as much as one clock tick worth of
		   time.

		   See [IA32-v2a] "HLT", [IA32-v2b] "STI", and [IA32-v3a]
		   7.11.1 "HLT Instruction". */
		asm volatile ("sti; hlt" : : : "memory");
	}
}

/* Function used as the basis for a kernel thread. */
static void
kernel_thread (thread_func *function, void *aux) {
	ASSERT (function != NULL);

	intr_enable ();       /* The scheduler runs with interrupts off. */
	function (aux);       /* Execute the thread function. */
	thread_exit ();       /* If function() returns, kill the thread. */
}


/* Does basic initialization of T as a blocked thread named
   NAME. */
static void
init_thread (struct thread *t, const char *name, int priority) {
	ASSERT (t != NULL);
	ASSERT (PRI_MIN <= priority && priority <= PRI_MAX);
	ASSERT (name != NULL);

	memset (t, 0, sizeof *t);
	t->status = THREAD_BLOCKED;
	strlcpy (t->name, name, sizeof t->name);
	t->tf.rsp = (uint64_t) t + PGSIZE - sizeof (void *);
	t->priority = priority;
	t->magic = THREAD_MAGIC;
}

/* Chooses and returns the next thread to be scheduled.  Should
   return a thread from the run queue, unless the run queue is
   empty.  (If the running thread can continue running, then it
   will be in the run queue.)  If the run queue is empty, return
   idle_thread. */
static struct thread *
next_thread_to_run (void) {
	if (list_empty (&ready_list))
		return idle_thread;
	else
		return list_entry (list_pop_front (&ready_list), struct thread, elem);
}

/* Use iretq to launch the thread */
void
do_iret (struct intr_frame *tf) {
	__asm __volatile(
			"movq %0, %%rsp\n"
			"movq 0(%%rsp),%%r15\n"
			"movq 8(%%rsp),%%r14\n"
			"movq 16(%%rsp),%%r13\n"
			"movq 24(%%rsp),%%r12\n"
			"movq 32(%%rsp),%%r11\n"
			"movq 40(%%rsp),%%r10\n"
			"movq 48(%%rsp),%%r9\n"
			"movq 56(%%rsp),%%r8\n"
			"movq 64(%%rsp),%%rsi\n"
			"movq 72(%%rsp),%%rdi\n"
			"movq 80(%%rsp),%%rbp\n"
			"movq 88(%%rsp),%%rdx\n"
			"movq 96(%%rsp),%%rcx\n"
			"movq 104(%%rsp),%%rbx\n"
			"movq 112(%%rsp),%%rax\n"
			"addq $120,%%rsp\n"
			"movw 8(%%rsp),%%ds\n"
			"movw (%%rsp),%%es\n"
			"addq $32, %%rsp\n"
			"iretq"
			: : "g" ((uint64_t) tf) : "memory");
}

/* Switching the thread by activating the new thread's page
   tables, and, if the previous thread is dying, destroying it.

   At this function's invocation, we just switched from thread
   PREV, the new thread is already running, and interrupts are
   still disabled.

   It's not safe to call printf() until the thread switch is
   complete.  In practice that means that printf()s should be
   added at the end of the function. */
static void
thread_launch (struct thread *th) {
	uint64_t tf_cur = (uint64_t) &running_thread ()->tf;
	uint64_t tf = (uint64_t) &th->tf;
	ASSERT (intr_get_level () == INTR_OFF);

	/* The main switching logic.
	 * We first restore the whole execution context into the intr_frame
	 * and then switching to the next thread by calling do_iret.
	 * Note that, we SHOULD NOT use any stack from here
	 * until switching is done. */
	__asm __volatile (
			/* Store registers that will be used. */
			"push %%rax\n"
			"push %%rbx\n"
			"push %%rcx\n"
			/* Fetch input once */
			"movq %0, %%rax\n"
			"movq %1, %%rcx\n"
			"movq %%r15, 0(%%rax)\n"
			"movq %%r14, 8(%%rax)\n"
			"movq %%r13, 16(%%rax)\n"
			"movq %%r12, 24(%%rax)\n"
			"movq %%r11, 32(%%rax)\n"
			"movq %%r10, 40(%%rax)\n"
			"movq %%r9, 48(%%rax)\n"
			"movq %%r8, 56(%%rax)\n"
			"movq %%rsi, 64(%%rax)\n"
			"movq %%rdi, 72(%%rax)\n"
			"movq %%rbp, 80(%%rax)\n"
			"movq %%rdx, 88(%%rax)\n"
			"pop %%rbx\n"              // Saved rcx
			"movq %%rbx, 96(%%rax)\n"
			"pop %%rbx\n"              // Saved rbx
			"movq %%rbx, 104(%%rax)\n"
			"pop %%rbx\n"              // Saved rax
			"movq %%rbx, 112(%%rax)\n"
			"addq $120, %%rax\n"
			"movw %%es, (%%rax)\n"
			"movw %%ds, 8(%%rax)\n"
			"addq $32, %%rax\n"
			"call __next\n"         // read the current rip.
			"__next:\n"
			"pop %%rbx\n"
			"addq $(out_iret -  __next), %%rbx\n"
			"movq %%rbx, 0(%%rax)\n" // rip
			"movw %%cs, 8(%%rax)\n"  // cs
			"pushfq\n"
			"popq %%rbx\n"
			"mov %%rbx, 16(%%rax)\n" // eflags
			"mov %%rsp, 24(%%rax)\n" // rsp
			"movw %%ss, 32(%%rax)\n"
			"mov %%rcx, %%rdi\n"
			"call do_iret\n"
			"out_iret:\n"
			: : "g"(tf_cur), "g" (tf) : "memory"
			);
}

/* Schedules a new process. At entry, interrupts must be off.
 * This function modify current thread's status to status and then
 * finds another thread to run and switches to it.
 * It's not safe to call printf() in the schedule(). */
static void
do_schedule(int status) {
	ASSERT (intr_get_level () == INTR_OFF);
	ASSERT (thread_current()->status == THREAD_RUNNING);
	while (!list_empty (&destruction_req)) {
		struct thread *victim =
			list_entry (list_pop_front (&destruction_req), struct thread, elem);
		palloc_free_page(victim);
	}
	thread_current ()->status = status;
	schedule ();
}

/*************************************************************
 * schedule - 현재 스레드를 스케줄링에서 제거하고 다음 스레드로 전환
 *
 * 기능:
 * - 현재 스레드의 상태에 따라 문맥 전환(context switch)을 수행
 * - 다음에 실행할 스레드(next_thread)를 선택하고 전환함
 * - dying 상태인 스레드는 스레드 구조체 제거를 요청 목록에 추가
 *
 * 전제 조건:
 * - 인터럽트는 반드시 비활성화된 상태여야 함 (atomicity 보장)
 * - curr->status != THREAD_RUNNING 상태여야 함 (RUNNING → READY/_BLOCKED 전환된 상태)
 *
 * 특이 사항:
 * - thread_exit()에 의해 죽은 스레드는 실제 제거가 아닌 나중에 deferred free 처리
 * - 문맥 전환은 thread_launch()를 통해 수행됨
 *************************************************************/
static void
schedule (void) 
{
	struct thread *curr = running_thread ();        // 현재 실행 중인 스레드
	struct thread *next = next_thread_to_run ();    // 다음 실행할 스레드 선택

	ASSERT (intr_get_level () == INTR_OFF);         // 인터럽트는 꺼져 있어야 함
	ASSERT (curr->status != THREAD_RUNNING);        // 현재 스레드는 더 이상 RUNNING 상태가 아니어야 함
	ASSERT (is_thread (next));                      // next가 유효한 스레드인지 확인

	next->status = THREAD_RUNNING;                  // 다음 스레드를 RUNNING 상태로 전환
	thread_ticks = 0;                               // 새 타임 슬라이스 시작

#ifdef USERPROG
	process_activate (next);                        // 사용자 프로그램이면 주소 공간 교체
#endif
	if (curr != next) {
		// 현재 스레드가 죽은 상태라면, 나중에 메모리 해제를 위해 큐에 넣음
		if (curr && curr->status == THREAD_DYING && curr != initial_thread) {
			ASSERT (curr != next);                  // dying 스레드는 당연히 next가 될 수 없음
			list_push_back (&destruction_req, &curr->elem); // 제거 요청 리스트에 추가
		}
		thread_launch (next);						// 실제 문맥 전환 수행 (레지스터/스택 등 전환)
	}
}

/* Returns a tid to use for a new thread. */
static tid_t
allocate_tid (void) {
	static tid_t next_tid = 1;
	tid_t tid;

	lock_acquire (&tid_lock);
	tid = next_tid++;
	lock_release (&tid_lock);

	return tid;
}
