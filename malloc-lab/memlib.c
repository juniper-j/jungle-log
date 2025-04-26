/*
 * memlib.c - 가상 메모리 시스템을 시뮬레이션하는 모듈
 *
 * malloc lab에서 학생이 구현한 malloc 패키지와
 * 실제 시스템 malloc이 충돌하지 않도록 하기 위해 사용됨.
 * 내부적으로 malloc()으로 고정 크기 힙을 미리 할당한 뒤,
 * sbrk() 함수처럼 동작하는 인터페이스를 제공함.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "memlib.h"
#include "config.h"



/* ===============================
 * 전역 변수 정의 (private variables)
 * =============================== */
static char *mem_start_brk;  /* points to first byte of heap (MAX_HEAP size) */
static char *mem_brk;        /* points to last byte of heap */
static char *mem_max_addr;   /* largest legal heap address (logical maximum)*/ 



/* ===============================
 * 메모리 시스템 초기화 함수
 * =============================== */

/*
 * mem_init - 가상 메모리 영역 초기화
 * malloc(MAX_HEAP)으로 고정 크기 메모리 확보
 * 초기 brk를 힙의 시작점으로 설정
 */
void mem_init(void)
{
    /* allocate the storage we will use to model the available VM */
    // MAX_HEAP 바이트만큼 공간 확보
    if ((mem_start_brk = (char *)malloc(MAX_HEAP)) == NULL) {
	fprintf(stderr, "mem_init_vm: malloc error\n");
	exit(1);
    }
    mem_max_addr = mem_start_brk + MAX_HEAP;  /* max legal heap address */
    mem_brk = mem_start_brk;                  /* heap is empty initially */
}

/* 
 * mem_deinit - 시뮬레이션 된 힙 메모리 해제
 */
void mem_deinit(void)
{
    free(mem_start_brk);
}

/*
 * mem_reset_brk - brk 포인터를 힙 시작점으로 리셋
 * 힙을 비운 상태로 초기화할 때 사용
 */
void mem_reset_brk()
{
    mem_brk = mem_start_brk;
}



/* ===============================
 * sbrk 기능 시뮬레이션
 * =============================== */

/*
 * mem_sbrk - sbrk() 함수의 간단한 시뮬레이션
 * 힙을 incr 바이트만큼 증가시키고,
 * 증가 전의 포인터(old_brk)를 반환
 * 범위를 초과하거나 음수 요청이면 실패 처리
 */
void *mem_sbrk(int incr) 
{
    char *old_brk = mem_brk;

    // 음수 요청이거나 최대 주소를 초과하는 경우 에러 처리
    if ( (incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
        return (void *)-1;
    }

    mem_brk += incr;            // brk 포인터 증가
    return (void *)old_brk;     // 증가 전 주소 반환
}



/* ===============================
 * 힙 상태 조회 함수들
 * =============================== */

/*
 * mem_heap_lo - 힙의 시작 주소 반환
 */
void *mem_heap_lo()
{
    return (void *)mem_start_brk;
}

/*
 * mem_heap_hi - 힙의 마지막 바이트 주소 반환
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize - 현재 힙 크기(바이트 단위) 반환
 */
size_t mem_heapsize() 
{
    return (size_t)(mem_brk - mem_start_brk);
}

/*
 * mem_pagesize - 시스템 페이지 크기 반환
 * (시뮬레이션 용도 또는 페이지 정렬 확인용)
 */
size_t mem_pagesize()
{
    return (size_t)getpagesize();
}
