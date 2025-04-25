/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
// #include <stdint.h> // 내가 임시로 추가

#include "mm.h"         // mm_malloc, mm_free 인터페이스 정의
#include "memlib.h"     // mem_sbrk 등 가상 메모리 함수 정의

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

/* 8바이트 정렬 기준 (double word) */
#define ALIGNMENT 8

/* size를 8의 배수로 올림 (패딩 포함한 정렬 처리) */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

/* size_t 자료형을 ALIGN 단위로 맞춘 크기 */
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))


/////////////////////////////////////////////////////////////////////////////
//
// 함수 선언 추가
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
// 힙의 시작을 가리키는 포인터
static char *heap_listp = 0;
// #define IS_ALIGNED(p) ((((uintptr_t)(p)) % ALIGNMENT) == 0)  // 내가 임시로 추가


/* Basic constants and macros */
#define WSIZE 4 /* Word and header/footer size (bytes) */
#define DSIZE 8 /* Double word size (bytes) */
#define CHUNKSIZE (1<<12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y)? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
//
/////////////////////////////////////////////////////////////////////////////


/*
 * mm_init - initialize the malloc package. (여기서는 아무것도 안함)
 */
int mm_init(void)
{
    /* 초기 힙 영역: 4워드 할당 (패딩 + 프롤로그 헤더 + 프롤로그 푸터 + 에필로그 헤더) */
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0);                             /* 정렬 패딩 (사용되지 않음) */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));  /* 프롤로그 헤더 */
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));  /* 프롤로그 푸터 */
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));      /* 에필로그 헤더 */
    heap_listp += (2 * WSIZE);                      /* 프롤로그 블록의 payload 위치로 이동 */

    /* CHUNKSIZE만큼 힙 확장 및 초기 가용 블록 생성 */
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}

// 책 보고 넣은 함수
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    // 워드 수를 짝수로 맞춰 더블워드 정렬 유지
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;

    // 힙 공간 요청 (memlib.c에서 정의된 mem_sbrk 사용)
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    // 새 free 블록의 header, footer 초기화
    PUT(HDRP(bp), PACK(size, 0));         // header
    PUT(FTRP(bp), PACK(size, 0));         // footer

    // 새 epilogue header 생성
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // 직전 블록이 free였다면 병합 수행
    return coalesce(bp);
}


static void *find_fit(size_t asize)
{
    /* First-fit search */
    void *bp;

    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL; /* No fit */
}

static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));

    if ((csize - asize) >= (2 * DSIZE)) {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
    } else {
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}


/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 * Always allocate a block whose size is a multiple of the alignment.
 * 실제 할당되는 크기: payload + 헤더용 공간 (size 저장용)
 */
void *mm_malloc(size_t size)
{
    // 기존 코드
    // // 요청한 size + 헤더용 size_t 공간 → ALIGN을 적용해 8의 배수로 올림
    // int newsize = ALIGN(size + SIZE_T_SIZE);

    // // 포인터로 유사힙을 늘림 (이 공간을 실제 메모리처럼 씀)
    // void *p = mem_sbrk(newsize);
    // if (p == (void *)-1) return NULL;    // 메모리 요청 실패 시 NULL 반환
    // else {
    //     // 블록의 맨 앞에 payload의 크기를 저장 (헤더처럼 사용)
    //     *(size_t *)p = size;
    //     // payload 주소를 반환 (헤더를 건너뛴 주소)
    //     return (void *)((char *)p + SIZE_T_SIZE);
    // }

    size_t asize;         /* 조정된 블록 크기 (payload + header/footer + 정렬) */
    size_t extendsize;    /* fit 실패 시 heap을 얼마나 확장할지 */
    char *bp;

    /* 잘못된 요청 무시 */
    if (size == 0)
        return NULL;

    /* 최소 블록 크기 = header + footer + 최소 payload (double word 정렬 충족) */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* 가용 블록 탐색 */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);  // 블록 배치 및 분할 가능
        return bp;
    }

    /* 적합한 블록이 없으면 힙 확장 */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;

    place(bp, asize);  // 확장된 블록에 배치
    return bp;
}


/*
 * mm_free - Freeing a block does nothing.
 * (이 구현은 블록을 절대 재사용하지 않기 때문)  
 */
void mm_free(void *ptr)
{
    void *bp = ptr; // 내가 추가함
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));  // header: free
    PUT(FTRP(bp), PACK(size, 0));  // footer: free
    coalesce(bp);                  // 병합 시도
}


static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {            // Case 1: 양쪽 모두 할당
        return bp;
    }
    else if (prev_alloc && !next_alloc) {      // Case 2: 뒤만 free
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc) {      // Case 3: 앞만 free
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else {                                     // Case 4: 양쪽 모두 free
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}


/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 * 기존 블록에서 새로운 크기로 재할당
 * mm_malloc으로 새 블록 요청 후 복사, 기존 블록은 mm_free 호출
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    // 새로운 크기만큼 공간 확보
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    
    // 이전 블록의 크기를 헤더로부터 읽음
    //copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;  // ⬅ 헤더/푸터 제외한 페이로드 크기

    // 복사 크기는 요청 크기와 기존 크기 중 작은 값
    if (size < copySize) copySize = size;

    // 데이터 복사 (old → new)
    memcpy(newptr, oldptr, copySize);

    // 이전 블록 free (여기선 의미 없음)
    mm_free(oldptr);

    return newptr;
}