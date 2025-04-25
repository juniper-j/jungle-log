/*
 * mm-naive.c - 블록을 brk 포인터를 통해 순차적으로 할당하는 가장 단순한 malloc 구현
 *
 * 이 구현에서는 블록에 헤더/푸터를 붙이지 않고, 
 * 한번 할당된 블록은 재사용하거나 병합(coalesce)하지 않음.
 * realloc 또한 malloc과 free를 이용해 새로 복사하는 방식으로 단순 처리함.
 * 
 * 이 코드는 최적화보다는 학습을 목적으로 한 기본 설계 구조를 보여줌.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "mm.h"         // 외부 인터페이스 정의 (mm_malloc, mm_free, etc.)
#include "memlib.h"     // 가상 메모리 함수 정의 (mem_sbrk, mem_heap_lo 등)

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


/***********************************************************************************/
/***********************************************************************************/

/* 메모리 정렬 기준: 8바이트 (double word) */
#define ALIGNMENT 8

/* 정렬을 위한 매크로: size를 8의 배수로 올림 */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

/* size_t의 크기를 ALIGN 단위로 맞춘 값 */
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* 워드 및 더블워드 크기 (단위: 바이트) */
#define WSIZE 4             /* 워드 크기 (header/footer용) */
#define DSIZE 8             /* 더블워드 크기 (payload 최소 단위) */

/* 기본 힙 확장 크기 (초기 힙 요청 및 fit 실패 시) */
#define CHUNKSIZE (1<<12)   /* 4096 bytes = 4KB */

/* 두 값 중 큰 값 반환 */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* size와 할당 여부(alloc)를 묶어 하나의 워드로 패킹 */
#define PACK(size, alloc) ((size) | (alloc))

/* 포인터 p가 가리키는 주소의 값을 읽거나 저장 */
#define GET(p) (*(unsigned int *)(p))         // 값 읽기
#define PUT(p, val) (*(unsigned int *)(p) = (val)) // 값 쓰기

/* 주소 p에서 블록 크기와 할당 여부 추출 */
#define GET_SIZE(p) (GET(p) & ~0x7)   // 하위 3비트를 제외한 크기
#define GET_ALLOC(p) (GET(p) & 0x1)   // 하위 1비트가 할당 여부

/* 블록 포인터 bp로부터 header/footer 주소 계산 */
#define HDRP(bp) ((char *)(bp) - WSIZE)    // header 위치
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)  // footer 위치

/* 블록 포인터 bp로부터 이전/다음 블록의 주소 계산 */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))      // 다음 블록
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // 이전 블록

/* 힙의 시작 위치를 가리키는 전역 포인터 */
static char *heap_listp = 0;

/* 내부 함수 선언 (파일 하단에서 정의됨) */
static void *extend_heap(size_t words);     // 힙 확장
static void *coalesce(void *bp);            // 인접 블록 병합

/***********************************************************************************/
/***********************************************************************************/


/*
 * mm_init - 초기 힙 생성
 * 프롤로그/에필로그 블록 설정 → 힙의 일관성 유지
 * 첫 가용 블록 생성을 위해 힙 확장
 */
int mm_init(void)
{
    // 4워드 공간 확보: 패딩 + 프롤로그 헤더 + 프롤로그 푸터 + 에필로그 헤더
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
        return -1;

    PUT(heap_listp, 0);                             // 패딩
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));  // 프롤로그 헤더
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));  // 프롤로그 푸터
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));      // 에필로그 헤더
    heap_listp += (2 * WSIZE);                      // 프롤로그 블록의 payload 위치로 이동

    // CHUNKSIZE만큼 힙 확장 및 초기 가용 블록 생성
    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
        return -1;

    return 0;
}


/*
 * extend_heap - 힙을 size 워드 만큼 확장하고 새 가용 블록 생성
 * → 항상 double word alignment 유지 (짝수 워드 수로 확장)
 */
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
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));   // 새로운 epilogue header 생성
    return coalesce(bp);    // 직전 블록이 가용 상태면 병합
}


/*
 * find_fit - First-fit 방식으로 가용 블록을 탐색
 * 조건: 할당되지 않은 블록 && 요청 크기보다 크거나 같은 블록
 * 순차적으로 힙의 처음부터 끝까지 탐색
 */
static void *find_fit(size_t asize)
{
    void *bp;

    // 힙의 첫 블록부터 에필로그 직전까지 순회
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        // 사용 중이지 않고, 요청 크기보다 크거나 같으면 반환
        if (!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))) {
            return bp;
        }
    }
    return NULL; // No fit - NULL 반환
}


/*
 * place - 탐색된 가용 블록에 메모리 할당
 * 필요한 크기보다 클 경우 블록을 분할
 */
static void place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));  // 현재 블록의 크기

    if ((csize - asize) >= (2 * DSIZE)) {
        // 블록을 할당하고 남은 공간이 최소 블록 크기(16B) 이상이면 분할
        PUT(HDRP(bp), PACK(asize, 1));          // 앞쪽 블록 할당
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);                     // 남은 블록 주소로 이동
        PUT(HDRP(bp), PACK(csize - asize, 0));  // 남은 블록: 가용 상태
        PUT(FTRP(bp), PACK(csize - asize, 0));
    } else {
        // 분할할 수 없는 크기면 전체 블록 할당
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}


/*
 * mm_malloc - 요청한 size만큼 메모리 블록 할당
 * 요청 크기 기반으로 block 크기 조정 → find_fit → place
 */
void *mm_malloc(size_t size)
{
    size_t asize;         // 조정된 블록 크기 (payload + header/footer + 정렬)
    size_t extendsize;    // fit 실패 시 heap을 얼마나 확장할지
    char *bp;

    if (size == 0) return NULL;     // 잘못된 요청은 무시

    // 최소 블록 크기는 16바이트 (header + footer + 최소 payload = double word 정렬 충족)
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    // 가용 블록 탐색
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);  // 블록 배치 및 분할 가능
        return bp;
    }

    // 적합한 블록이 없으면 힙 확장 후 배치
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;

    place(bp, asize);  // 확장된 블록에 배치
    return bp;
}


/*
 * mm_free - 블록을 해제하여 가용 상태로 변경
 * header/footer의 alloc 비트를 0으로 설정 후, 병합(coalesce)
 */
void mm_free(void *ptr)
{
    void *bp = ptr;
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));  // header: free
    PUT(FTRP(bp), PACK(size, 0));  // footer: free
    coalesce(bp);                  // 주변 가용블록과 병합 시도
}


/*
 * coalesce - 인접한 가용 블록들을 하나로 병합
 * 병합 가능한 경우 4가지:
 * 1. 앞/뒤 모두 할당 → 병합 없음
 * 2. 앞만 할당 → 뒤와 병합
 * 3. 뒤만 할당 → 앞과 병합
 * 4. 앞/뒤 모두 가용 → 양쪽 모두 병합
 */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));     // 앞 블록 할당 여부
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));     // 뒷 블록 할당 여부
    size_t size = GET_SIZE(HDRP(bp));                       // 현재 블록 크기

    if (prev_alloc && next_alloc) {            // Case 1: 양쪽 모두 할당 - 병합 없음
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
    else {                                     // Case 4: 양쪽 모두 free - 양쪽 병합
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}


/*
 * mm_realloc - 블록 재할당
 * 기존 데이터를 새로운 블록으로 복사 후, 기존 블록 free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    // 새로운 크기만큼 공간 먼저 확보
    newptr = mm_malloc(size);
    if (newptr == NULL)
        return NULL;
    
    // 이전 블록의 크기를 헤더로부터 읽음
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    // copySize = GET_SIZE(HDRP(oldptr)) - DSIZE;  // ⬅ 헤더/푸터 제외한 페이로드 크기

    // 실제 복사할 크기는 요청한 size와 기존 크기 중 작은 값
    if (size < copySize) copySize = size;

    memcpy(newptr, oldptr, copySize);   // 새 블록에 데이터 복사 (old → new)
    mm_free(oldptr);                    // 이전 블록 free

    return newptr;
}