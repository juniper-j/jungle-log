/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
   */

   #include "threads/synch.h"
   #include <stdio.h>
   #include <string.h>
   #include "threads/interrupt.h"
   #include "threads/thread.h"
   
   /* Initializes semaphore SEMA to VALUE.  A semaphore is a
	  nonnegative integer along with two atomic operators for
	  manipulating it:
   
	  - down or "P": wait for the value to become positive, then
	  decrement it.
   
	  - up or "V": increment the value (and wake up one waiting
	  thread, if any). */
   void
   sema_init (struct semaphore *sema, unsigned value) {
	   ASSERT (sema != NULL);
   
	   sema->value = value;
	   list_init (&sema->waiters);
   }
   
   /*************************************************************
	* sema_down - 세마포어 자원을 얻기 위한 요청 (P 연산 또는 "wait" 연산)
	*
	* 요약:
	* - sema->value > 0일 경우 즉시 자원을 획득하고 value를 1 감소시킴
	* - sema->value == 0이면, 자원이 사용 중이므로 현재 스레드를 
	*   차단(BLOCKED) 시키고 대기(waiters 리스트에 삽입)
	* 
	* 호출 시기:
	* - critical section에 진입하기 직전, 즉 공유 자원을 얻기 위해 진입 요청할 때
	* - e.g. 락(lock_acquire()), 조건 변수(cond_wait()), 공유 버퍼 접근 등
	*
	* 제약:
	* - 이 함수는 스레드를 BLOCKED 시킬 수 있으므로, 인터럽트 핸들러에서는 절대 호출하면 안 됨
	* - 호출 시 인터럽트를 꺼야 하며, 이후 다시 복원해야 함
	*
	* 구현:
	* - waiters 리스트에 현재 스레드를 우선순위 기준으로 삽입
	* - cmp_priority를 사용하여 높은 우선순위가 먼저 깨어나도록 보장
	*************************************************************/
   void
   sema_down (struct semaphore *sema) {
	   enum intr_level old_level;
   
	   ASSERT (sema != NULL);
	   ASSERT (!intr_context ());
   
	   old_level = intr_disable ();  // 동기화를 위해 인터럽트 비활성화 (동기화)
	   while (sema->value == 0) 
	  {  // waiters 리스트에 현재 스레드를 우선순위 기준 정렬 삽입 
		   list_insert_ordered (&sema->waiters, &thread_current ()->elem, cmp_priority, NULL);
		   thread_block ();           // 현재 스레드 BLOCKED 상태로 전환
	   }
	   sema->value--;                // 세마포어 값 1 감소
	   intr_set_level (old_level);   // 인터럽트 상태 복원
   }
   
   /* Down or "P" operation on a semaphore, but only if the
	  semaphore is not already 0.  Returns true if the semaphore is
	  decremented, false otherwise.
   
	  This function may be called from an interrupt handler. */
   bool
   sema_try_down (struct semaphore *sema) {
	   enum intr_level old_level;
	   bool success;
   
	   ASSERT (sema != NULL);
   
	   old_level = intr_disable ();     
	   if (sema->value > 0)
	   {
		   sema->value--;
		   success = true;
	   }
	   else
		   success = false;
	   intr_set_level (old_level);
   
	   return success;
   }
   
   /*************************************************************
	* sema_up - 세마포어 자원을 반환하는 동작 (V 연산 또는 "signal" 연산)
	*
	* 요약:
	* - 세마포어 값을 1 증가시킴
	* - waiters 리스트에 대기 중인 스레드가 있다면,
	*   우선순위가 가장 높은 스레드 1개를 READY 상태로 전환
	*
	* 호출 시기:
	* - 임계 구역(critical section)을 벗어났을 때, 즉 공유 자원을 다 사용한 후 반환할 때
	* - e.g. 락(lock_release()), 조건 변수(cond_signal()) 내부에서 unlock 전에 호출
	*
	* 구현:
	* - 인터럽트 핸들러 내에서도 호출 가능함 (sleep이 없기 때문)
	* - waiters 리스트가 우선순위 기준으로 정렬되어 있어야 하므로 list_sort() 후 thread_unblock() 호출
	* - 깨운 스레드가 현재 실행 중인 스레드보다 priority가 높을 수 있으므로 preempt_priority() 호출 
	*
	* ✅ TODO: waiters list를 priority 순서대로 정렬
	*************************************************************/
   void
   sema_up (struct semaphore *sema) {
	   enum intr_level old_level;
   
	   ASSERT (sema != NULL);
   
	   old_level = intr_disable ();        // 동기화를 위해 인터럽트 비활성화 (동기화)
	   if (!list_empty (&sema->waiters)) 
	  {
		 list_sort(&sema->waiters, cmp_priority, NULL);  // 우선선위가 변경되었을 경우를 고려해 우선순위 순으로 정렬
		   thread_unblock (list_entry (list_pop_front (&sema->waiters),
					   struct thread, elem));  // 가장 우선순위 높은 스레드 깨움
	  }
	   sema->value++;                      // 세마포어 값 1 증가 (자원 반환)
	  preempt_priority();                 // 현재 스레드보다 높은 우선순위가 깨어났다면 양보
	   intr_set_level (old_level);         // 인터럽트 상태 복원
   }
   
   static void sema_test_helper (void *sema_);
   
   /* Self-test for semaphores that makes control "ping-pong"
	  between a pair of threads.  Insert calls to printf() to see
	  what's going on. */
   void
   sema_self_test (void) {
	   struct semaphore sema[2];
	   int i;
   
	   printf ("Testing semaphores...");
	   sema_init (&sema[0], 0);
	   sema_init (&sema[1], 0);
	   thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
	   for (i = 0; i < 10; i++)
	   {
		   sema_up (&sema[0]);
		   sema_down (&sema[1]);
	   }
	   printf ("done.\n");
   }
   
   /* Thread function used by sema_self_test(). */
   static void
   sema_test_helper (void *sema_) {
	   struct semaphore *sema = sema_;
	   int i;
   
	   for (i = 0; i < 10; i++)
	   {
		   sema_down (&sema[0]);
		   sema_up (&sema[1]);
	   }
   }
   
   /* Initializes LOCK.  A lock can be held by at most a single
	  thread at any given time.  Our locks are not "recursive", that
	  is, it is an error for the thread currently holding a lock to
	  try to acquire that lock.
   
	  A lock is a specialization of a semaphore with an initial
	  value of 1.  The difference between a lock and such a
	  semaphore is twofold.  First, a semaphore can have a value
	  greater than 1, but a lock can only be owned by a single
	  thread at a time.  Second, a semaphore does not have an owner,
	  meaning that one thread can "down" the semaphore and then
	  another one "up" it, but with a lock the same thread must both
	  acquire and release it.  When these restrictions prove
	  onerous, it's a good sign that a semaphore should be used,
	  instead of a lock. */
   void
   lock_init (struct lock *lock) {
	   ASSERT (lock != NULL);
   
	   lock->holder = NULL;
	   sema_init (&lock->semaphore, 1);
   }
   
   /* Acquires LOCK, sleeping until it becomes available if
	  necessary.  The lock must not already be held by the current
	  thread.
   
	  This function may sleep, so it must not be called within an
	  interrupt handler.  This function may be called with
	  interrupts disabled, but interrupts will be turned back on if
	  we need to sleep. 
	  
	  ✅ TODO: lock을 점유할 수 없는 경우에 대한 처리
	   1. 현재 스레드가 대기 중인 락의 주소를 추적하기 위해 thread->wait_on_lock 멤버변수에 락 주소를 저장한다.
   
	   2. 우선순위 기부 (Priority Donation)
		  - 현재 스레드의 우선순위가 락 소유자보다 높다면 base_priority를 저장해두고 락 소유자에게 우선순위를 기부한다.
		  - 이 과정에서 대기 중인 락이 또 다른 스레드에 의해 점유중이라면 재귀적으로 우선순위를 전파해야 한다. (Nested Donation)
   
	   3. 기부된 스레드(donated thread) 관리
		  - donations 리스트를 통해 기부 가능한 우선순위를 추적한다.
		  - 가장 높은 우선순위 스레드에게 기부받은 락과 우선순위를 관리한다. -> 구조체 vs 리스트 방식 중 선택할것
		  - 락이 해제되면 해당 락과 관련된 donation 항목을 donations 리스트에서 제거한다.
   
	   4. 락 획득 시 대기 중인 락 초기화
		  - 락을 획득하면 thread->wait_on_lock 멤버변수를 NULL로 초기화해서 대기 상태를 해제한다.
	  */
   void
   lock_acquire (struct lock *lock) {
	   ASSERT (lock != NULL);
	   ASSERT (!intr_context ());
	   ASSERT (!lock_held_by_current_thread (lock));
   
	   sema_down (&lock->semaphore);
	   lock->holder = thread_current ();
   }
   
   /* Tries to acquires LOCK and returns true if successful or false
	  on failure.  The lock must not already be held by the current
	  thread.
   
	  This function will not sleep, so it may be called within an
	  interrupt handler. */
   bool
   lock_try_acquire (struct lock *lock) {
	   bool success;
   
	   ASSERT (lock != NULL);
	   ASSERT (!lock_held_by_current_thread (lock));
   
	   success = sema_try_down (&lock->semaphore);
	   if (success)
		   lock->holder = thread_current ();
	   return success;
   }
   
   /* Releases LOCK, which must be owned by the current thread.
	  This is lock_release function.
   
	  An interrupt handler cannot acquire a lock, so it does not
	  make sense to try to release a lock within an interrupt
	  handler. 
   
	  ✅ TODO: 
	   1. 현재 스레드가 소유하고 있던 락 해제 후 락 대기하고 있던 스레드들 중 가장 높은 우선순위를 가진 스레드를 UNBLOCK
	   2. 기부받았던 우선순위에서 원래 우선순위로 복구
	  */
   void
   lock_release (struct lock *lock) {
	   ASSERT (lock != NULL);
	   ASSERT (lock_held_by_current_thread (lock));
   
	   lock->holder = NULL;
	   sema_up (&lock->semaphore);
   }
   
   /* Returns true if the current thread holds LOCK, false
	  otherwise.  (Note that testing whether some other thread holds
	  a lock would be racy.) */
   bool
   lock_held_by_current_thread (const struct lock *lock) {
	   ASSERT (lock != NULL);
   
	   return lock->holder == thread_current ();
   }
   
   /* One semaphore in a list. */
   struct semaphore_elem {
	   struct list_elem elem;              /* List element. */
	   struct semaphore semaphore;         /* This semaphore. */
   };
   
   /* Initializes condition variable COND.  A condition variable
	  allows one piece of code to signal a condition and cooperating
	  code to receive the signal and act upon it. */
   void
   cond_init (struct condition *cond) {
	   ASSERT (cond != NULL);
   
	   list_init (&cond->waiters);
   }
   
   /*************************************************************
	* cmp_sema_priority - 조건 변수 waiters 정렬용 비교 함수
	*
	* 역할:
	* - 각 semaphore_elem 내부 waiters 리스트를 참조하여,
	*   대기 중인 스레드의 우선순위를 비교함.
	* - 우선순위(priority)가 높은 스레드가 먼저 깨어나도록 정렬.
	* - tie-breaker로는 wakeup_ticks를 기준으로 FIFO 순서 보장.
	*
	* 인자:
	* - a, b: struct semaphore_elem의 리스트 요소 포인터
	*
	* 반환값:
	* - true: a가 더 높은 우선순위를 가짐 (a 우선)
	* - false: b가 우선 또는 우선순위 동일하며 wakeup_tick이 더 느림
	*
	* 사용 위치:
	* - cond_signal(), cond_broadcast()에서 cond_waiters 정렬 시 사용
	*************************************************************/
   bool
   cmp_sema_priority(const struct list_elem *a, const struct list_elem *b, void *aux UNUSED)
   {
	   struct semaphore_elem *sema_a = list_entry(a, struct semaphore_elem, elem);		// a 요소를 semaphore_elem 구조체로 변환
	   struct semaphore_elem *sema_b = list_entry(b, struct semaphore_elem, elem);		// b 요소를 semaphore_elem 구조체로 변환
   
	   struct list *waiters_a = &(sema_a->semaphore.waiters);
	   struct list *waiters_b = &(sema_b->semaphore.waiters);
   
	   if (list_empty(waiters_a)) return false;
	   if (list_empty(waiters_b)) return true;
   
	   struct thread *thread_a = list_entry(list_front(waiters_a), struct thread, elem);
	   struct thread *thread_b = list_entry(list_front(waiters_b), struct thread, elem);
   
	   if (thread_a->priority == thread_b->priority)				 // 우선순위가 같은 경우 (tie-breaker)
		   return thread_a->wakeup_ticks < thread_b->wakeup_ticks;  // wakeup_thicks가 빠른 스레드를 우선 배치 (FIFO)
	   return thread_a->priority > thread_b->priority;				 // 우선순위가 높은 (값이 큰) 스레드를 먼저 배치
   }
   
   /*************************************************************
	* cond_wait - 조건 변수(COND)에서 대기하며, LOCK을 원자적으로 해제 후 다시 획득
	*
	* 기능:
	* - 현재 스레드는 조건 변수(cond)에서 signal을 받을 때까지 대기
	* - 대기 전 lock을 해제하고, signal 이후 lock을 다시 획득
	* - Mesa-style 모니터 방식: signal과 wait이 동시에 일어나지 않으므로
	*   signal 후에도 조건을 재확인해야 함
	*
	* 제약 사항:
	* - 반드시 lock을 획득한 상태에서 호출해야 함
	* - 인터럽트 핸들러 내에서는 호출할 수 없음
	* - 대기 중 sleep할 수 있으며, 이 경우 인터럽트는 다시 활성화됨
	*
	* ✅ TODO: cond->waiters 리스트에 스레드를 priority 순서대로 삽입
	*************************************************************/
   void
   cond_wait (struct condition *cond, struct lock *lock) {
	   struct semaphore_elem waiter; 
   
	   ASSERT (cond != NULL);
	   ASSERT (lock != NULL);
	   ASSERT (!intr_context ());                    // 인터럽트 핸들러 내에서는 호출 불가
	   ASSERT (lock_held_by_current_thread (lock));  // 현재 스레드가 lock을 보유하고 있어야 함
   
	   sema_init (&waiter.semaphore, 0);
   
	  // cond->waiters 리스트에 현재 스레드를 우선순위 기준으로 삽입
	   list_insert_ordered (&cond->waiters, &waiter.elem, cmp_priority, NULL);
   
	   lock_release (lock);                         // 락을 해제하여 다른 스레드가 공유 자원에 접근 가능하게 함
	   sema_down (&waiter.semaphore);               // 세마포어 down 수행 (signal 될 때까지 대기)
	   lock_acquire (lock);                         // signal을 받으면 다시 락을 획득하고 함수 복귀
   }
   
   /*************************************************************
	* cond_signal - 조건 변수(COND)에서 대기 중인 스레드 중 하나를 깨움
	*
	* 기능:
	* - 조건 변수에 대기 중인 스레드가 있다면 그 중 우선순위가 가장 높은 스레드 하나를 깨움
	* - signal을 보낼 때는 반드시 lock을 소유한 상태여야 하며, interrupt context에서는 호출 불가
	* - 조건 변수는 하나의 lock과 연관되어 있으며, lock이 여러 조건 변수를 가질 수 있음
	*
	* Mesa-style 모니터 특성상, signal은 대기 중인 스레드에게 wake-up만 보장하며,
	* wake-up 후 조건을 재확인하고 필요시 다시 대기해야 함
	*
	* ✅ TODO: cond->waiters 리스트를 우선순위 기준으로 정렬하여 
	*          가장 높은 우선순위 스레드가 먼저 깨어나도록 함
	*************************************************************/     
   void
   cond_signal (struct condition *cond, struct lock *lock UNUSED) {
	   ASSERT (cond != NULL);
	   ASSERT (lock != NULL);
	   ASSERT (!intr_context ());                   // 인터럽트 핸들러 내에서는 호출 불가
	   ASSERT (lock_held_by_current_thread (lock)); // 현재 스레드가 lock을 보유하고 있어야 함
   
	   if (!list_empty (&cond->waiters)) {
		 // TODO: 우선순위 높은 순서대로 정렬 (priority scheduling을 반영하기 위함)
		 list_sort (&cond->waiters, cmp_priority, NULL);
		 // 가장 앞에 있는 (우선순위 가장 높은) waiter를 꺼내어 해당 세마포어를 up → 스레드 깨움
		   sema_up (&list_entry (list_pop_front (&cond->waiters),
					   struct semaphore_elem, elem)->semaphore);
	  }
   }
   
   /* Wakes up all threads, if any, waiting on COND (protected by
	  LOCK).  LOCK must be held before calling this function.
   
	  An interrupt handler cannot acquire a lock, so it does not
	  make sense to try to signal a condition variable within an
	  interrupt handler. */
   void
   cond_broadcast (struct condition *cond, struct lock *lock) {
	   ASSERT (cond != NULL);
	   ASSERT (lock != NULL);
   
	   while (!list_empty (&cond->waiters))
		   cond_signal (cond, lock);
   }
   