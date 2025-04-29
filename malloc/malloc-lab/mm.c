/*********************************************************
 * Version 6. Explicit Free List + Best-fit
 * Perf index = 52 (util) + 40 (thru) = 92/100
 ********************************************************/

 #include <stdio.h>
 #include <stdlib.h>
 #include <assert.h>
 #include <unistd.h>
 #include <string.h>
 #include "mm.h"    
 #include "memlib.h" 
 
 /*********************************************************
  * NOTE TO STUDENTS: Before you do anything else, please
  * provide your team information in the following struct.
  ********************************************************/
 team_t team = {
     /* Team name */
     "team4",
     /* First member's full name */
     "Juniper Jeong",
     /* First member's email address */
     "jy.juniper.jeong@gmail.com",
     /* Second member's full name (leave blank if none) */
     "Minseok Kim",
     /* Second member's email address (leave blank if none) */
     "Junseok Won"};
 
 /* ==========================================
  * 정렬/기본 단위
  * ========================================== */
 /* 메모리 정렬 기준: 8바이트 (double word) */
 #define ALIGNMENT 16
 /* 정렬을 위한 매크로: size를 8의 배수로 올림 */
 #define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0xF) // ~0x7은 2진수로 1111...10000
 /* size_t의 크기를 ALIGN 단위로 맞춘 값 */
 #define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
 
 /* ==========================================
  * 기본 상수 및 매크로 정의 / Constants & Macros
  * ========================================== */
 #define WSIZE 8  /* 워드 크기 (header/footer용) */
 #define DSIZE 16 /* 더블워드 크기 (payload 최소 단위) */
 #define CHUNKSIZE (1 << 12) /* 기본 힙 확장 크기 (초기 힙 요청 및 fit 실패 시), 4096 bytes = 4KB */
 
 #define MAX(x, y) ((x) > (y) ? (x) : (y))   /* 두 값 중 큰 값 반환 */
 #define PACK(size, alloc) ((size) | (alloc))    /* size와 할당 여부(alloc)를 묶어 하나의 워드로 패킹 */
 
 /* 포인터 p가 가리키는 주소의 값을 읽거나 저장 (header/footer 직접 조작용) */
 #define GET(p) (*(unsigned long *)(p))              // 값 읽기 → (((포인터 p가 가리키는 메모리 주소)에서 8바이트 크기)를 읽어옴)
 #define PUT(p, val) (*(unsigned long *)(p) = (val)) // 값 쓰기 → (((포인터 p가 가리키는 메모리 주소)에 8바이트 크기)로 val 값을 저장함)
 
 /* 주소 p에서 블록 크기와 할당 여부 추출 (블록 메타데이터 읽기) */
 #define GET_SIZE(p) (GET(p) & ~0xF) // 하위 4비트를 제외한 크기
 #define GET_ALLOC(p) (GET(p) & 0x1) // 하위 1비트가 할당 여부
 
 /* 블록 포인터 bp로부터 header/footer 주소 계산 */
 #define HDRP(bp) ((char *)(bp) - WSIZE)                      // header 위치
 #define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // footer 위치
 
 /* 블록 포인터 bp로부터 이전/다음 블록의 주소 계산 */
 #define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) // 다음 블록
 #define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // 이전 블록
 
 /* Explicit Free List에 쓰일 predecessor, successor 포인터 */
 #define PRED(bp) (*(void **)(bp))                   /* 현재 bp가 가리키는 free 블록의 prev pointer */
 #define SUCC(bp) (*(void **)((char *)(bp) + WSIZE)) /* 현재 bp가 가리키는 free 블록의 next pointer */
 
 /* ==========================================
  * 전역 변수 정의 / Global Variables
  * ========================================== */
 static void *heap_listp = 0;
 static void *free_listp = NULL;
 
 /* ==========================================
  * 내부 함수 선언 / Internal Function Prototypes
  * ========================================== */
 static void *extend_heap(size_t words);
 static void *coalesce(void *bp);
 static void *find_fit(size_t asize);
 static void place(void *bp, size_t asize);
 static void insert_free_block(void *bp);
 static void remove_free_block(void *bp);
 


 /*********************************************************
  * 내부 헬퍼 함수 구현
  **********************************************************/
 
 /* ==========================================
  * 힙 확장 및 병합 / Heap Extension & Coalescing
  * ========================================== */
 static void *extend_heap(size_t words)
 {
     char *bp;
     size_t size;
 
     // 워드 수를 짝수로 맞춰 더블워드 정렬 유지
     size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
 
     // 힙 공간 요청 (memlib.c에서 정의된 mem_sbrk 사용)
     if ((long)(bp = mem_sbrk(size)) == -1)
         return NULL;
 
     PUT(HDRP(bp), PACK(size, 0));         // 새로운 가용블록의 header 초기화
     PUT(FTRP(bp), PACK(size, 0));         // 새로운 가용블록의 footer 초기화
     PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); // 새로운 epilogue header 생성
 
     return coalesce(bp);
 }
 
 /* ==========================================
  * 가용 블록 병합 / Coalesce Free Blocks
  * ========================================== */
 static void *coalesce(void *bp)
 {
     size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp))); // 앞 블록 할당 여부
     size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp))); // 뒷 블록 할당 여부
     size_t size = GET_SIZE(HDRP(bp));                   // 현재 블록 크기
 
     void *next = NEXT_BLKP(bp);
     void *prev = PREV_BLKP(bp);
 
     if (prev_alloc && next_alloc)
     { // Case 1: 양쪽 모두 할당 - 병합 없음
     }
     else if (prev_alloc && !next_alloc)
     { // Case 2: 뒷 블록만 가용
         size += GET_SIZE(HDRP(next));
         remove_free_block(next);
         PUT(HDRP(bp), PACK(size, 0));
         PUT(FTRP(bp), PACK(size, 0));
     }
     else if (!prev_alloc && next_alloc)
     { // Case 3: 앞 블록만 가용
         size += GET_SIZE(HDRP(prev));
         remove_free_block(prev);
         PUT(HDRP(prev), PACK(size, 0));
         PUT(FTRP(bp), PACK(size, 0));
         bp = PREV_BLKP(bp);
     }
     else
     { // Case 4: 앞뒤 블록 모두 가용
         size += GET_SIZE(HDRP(next)) + GET_SIZE(HDRP(prev));
         remove_free_block(next);
         remove_free_block(prev);
 
        //  printf("\n==== 실험 시작 ====\n");
        //  printf("prev 헤더 주소: %p\n", HDRP(prev));
        //  printf("next 푸터 주소: %p\n", FTRP(next));
        //  printf("prev 기준 푸터 주소(FTRP(prev)): %p\n", FTRP(prev));
        //  printf("size 합친 후: %zu\n", size);
        //  printf("prev → next 거리: %ld bytes\n", (char *)next - (char *)prev);
 
         bp = prev; // 병합 결과 블록은 prev부터 시작
 
        //  printf("병합 결과 블록 시작(bp): %p\n", bp);
        //  printf("병합 결과 블록 푸터(FTRP(bp)): %p\n", FTRP(bp));
 
        // //  if (FTRP(bp) == FTRP(next))
        // //      printf("✅ FTRP(bp) == FTRP(next) 맞음!\n");
        // //  else
        // //      printf("❌ FTRP(bp) != FTRP(next)\n");
        // //  printf("==== 실험 끝 ====\n\n");
 
         PUT(HDRP(bp), PACK(size, 0));
         PUT(FTRP(bp), PACK(size, 0));
     }
     insert_free_block(bp);
     return bp;
 }
 
 /* ==========================================
  * 가용 블록 탐색 / Find Available Block
  * ========================================== */
 static void *find_fit(size_t asize)
 {
     void *bp;
     void *best_bp = NULL;
     size_t best_size = (size_t)-1;
 
     // free list를 순회 - best-fit
     for (bp = free_listp; bp != NULL; bp = SUCC(bp))
     {
         size_t bsize = GET_SIZE(HDRP(bp));
         if (asize <= bsize)
         {
             if (bsize < best_size)
             {
                 best_size = bsize;
                 best_bp = bp;
                 if (bsize == asize)
                     break; // 완벽한 fit 발견하면 바로 종료
             }
         }
     }
     return best_bp;
 }
 
 /* ==========================================
  * 가용 블록에 메모리 할당 / Place Block
  * ========================================== */
 static void place(void *bp, size_t asize)
 {
     size_t block_size = GET_SIZE(HDRP(bp)); // 현재 블록의 크기
     size_t remain_size = block_size - asize;
 
     remove_free_block(bp);
 
     if (remain_size >= (2 * DSIZE))
     { // 블록을 할당하고 남은 공간이 최소 블록 크기(16B) 이상이면 분할
         // 앞쪽 블록 할당
         PUT(HDRP(bp), PACK(asize, 1));
         PUT(FTRP(bp), PACK(asize, 1));
 
         // 남은 블록 주소로 이동 & 새 가용 블록 생성
         void *next_bp = NEXT_BLKP(bp);
         PUT(HDRP(next_bp), PACK(remain_size, 0));
         PUT(FTRP(next_bp), PACK(remain_size, 0));
         insert_free_block(next_bp);
     }
     else
     { // 분할할 수 없는 크기면 전체 블록 할당
         PUT(HDRP(bp), PACK(block_size, 1));
         PUT(FTRP(bp), PACK(block_size, 1));
     }
 }
 
 /* ==========================================
  * insert_free_block
  * ========================================== */
 static void insert_free_block(void *bp)
 {
     if (bp == NULL)
         return;
     if (bp == free_listp)
         return;
     // 새로 들어올 free block의 next는 현재 free_listp
     SUCC(bp) = free_listp;
     // 새로 들어올 free block의 prev는 NULL (head니까)
     PRED(bp) = NULL;
 
     // 기존 free_listp가 NULL이 아니라면(리스트에 블록이 있었다면)
     if (free_listp != NULL)
         PRED(free_listp) = bp; // 기존 head 블록의 prev를 새 bp로
 
     // free_listp를 새 블록로 갱신
     free_listp = bp;
 }
 
 /* ==========================================
  * remove_free_block
  * ========================================== */
 static void remove_free_block(void *bp)
 {
     if (PRED(bp) != NULL)
         SUCC(PRED(bp)) = SUCC(bp);
     else
         free_listp = SUCC(bp);
 
     if (SUCC(bp) != NULL)
         PRED(SUCC(bp)) = PRED(bp);
 }
 


 /*********************************************************
  * 외부 인터페이스 함수 (main 함수에서 호출)
  **********************************************************/
 
 /* ==========================================
  * 초기화 함수 / Initialization
  * ========================================== */
 int mm_init(void)
 {
     //  1. 힙을 위한 최소 공간 16바이트 확보 (padding + prologue header/footer + epilogue header)
     heap_listp = mem_sbrk(4 * WSIZE);
     if (heap_listp == (void *)-1)
         return -1;
 
     //  2. 초기 블록들 구성하는 단계
     PUT(heap_listp, 0);                            // Unused Alignment Padding
     PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); // Prologue header (8 bytes, alloc 1)
     PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); // Prologue footer (8 bytes, alloc 1)
     PUT(heap_listp + (3 * WSIZE), PACK(0, 1));     // Epilogue header (0 bytes, alloc 1)
 
     // 3. payload 기준 위치로 이동 (Prologue 블록의 payload 포인터)
     heap_listp += (2 * WSIZE);
 
     // 4. 첫 가용 블록을 만들기 위해 CHUNKSIZE만큼 힙 확장
     void *bp = extend_heap(CHUNKSIZE / WSIZE);
     if (bp == NULL)
         return -1;
 
     // 5. 작은 요청을 대비해 추가로 extend_heap
     if (extend_heap(4) == NULL)
         return -1;
 
     // 확보한 free block을 free_list의 시작으로 설정
     bp = coalesce(bp);
     free_listp = bp; // 초기 힙 확장 후 가용 블록의 시작점을 명확히 지정해주기 위함
     return 0;
 }
 
 /* ==========================================
  * 메모리 블록 할당 / Malloc
  * ========================================== */
 void *mm_malloc(size_t size)
 {
     size_t asize;      // 조정된 블록 크기 (payload + header/footer + 정렬)
     size_t extendsize; // fit 실패 시 heap을 얼마나 확장할지
     char *bp;
 
     if (size == 0)
         return NULL; // 잘못된 요청은 무시
 
     // 최소 블록 크기(16B) 충족하도록 조정
     if (size <= DSIZE)
         asize = 2 * DSIZE;
     else
         asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
 
     // 가용 블록 탐색
     if ((bp = find_fit(asize)) != NULL)
     {
         place(bp, asize); // 블록 배치 및 분할 가능
         return bp;
     }
 
     // 적합한 블록이 없으면 힙 확장 후 배치
     extendsize = MAX(asize, CHUNKSIZE);
     if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
         return NULL;
 
     // bp = coalesce(bp); // 예외 free block 병합해 관리 개선
     place(bp, asize); // 확장된 블록에 배치
     return bp;
 }
 
 /* ==========================================
  * 메모리 블록 해제 / Free
  * ========================================== */
 void mm_free(void *ptr)
 {
     size_t size = GET_SIZE(HDRP(ptr));
     // 1. 블록 상태를 free로 표시
     PUT(HDRP(ptr), PACK(size, 0)); // header: free
     PUT(FTRP(ptr), PACK(size, 0)); // footer: free
     // 2. 주변 가용 블록들과 병합 (coalesce)
     coalesce(ptr);
 }
 
 /* ==========================================
  * 메모리 블록 재할당 / Reallocate Block
  * ========================================== */
 void *mm_realloc(void *ptr, size_t size)
 {
     if (ptr == NULL)
         return mm_malloc(size);
 
     if (size == 0)
     {
         mm_free(ptr);
         return NULL;
     }
 
     size_t oldsize = GET_SIZE(HDRP(ptr));                                                            // 기존 블록의 전체 크기 (헤더+푸터 포함)
     size_t asize = (size <= DSIZE) ? (2 * DSIZE) : DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE); // 새로운 요청 크기를 정렬+오버헤드 포함해서 계산
 
     if (asize <= oldsize)
         return ptr;
 
     void *next = NEXT_BLKP(ptr);
 
     if (!GET_ALLOC(HDRP(next)) && (oldsize + GET_SIZE(HDRP(next))) >= asize)
     {
         remove_free_block(next);
         size_t newsize = oldsize + GET_SIZE(HDRP(next));
         PUT(HDRP(ptr), PACK(newsize, 1));
         PUT(FTRP(ptr), PACK(newsize, 1));
         return ptr;
     }
 
     void *newptr = mm_malloc(size);
     if (newptr == NULL)
         return NULL;
 
     size_t copySize = oldsize - DSIZE;
     if (size < copySize)
         copySize = size;
     memcpy(newptr, ptr, copySize);
     mm_free(ptr);
     return newptr;
 }