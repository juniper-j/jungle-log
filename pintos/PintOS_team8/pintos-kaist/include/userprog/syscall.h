#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <threads/thread.h>

void syscall_init (void);               // 시스템 콜 인터럽트 핸들러 및 MSR 레지스터 초기화

/* Process-related syscalls */
void halt(void);                        // 시스템 종료 (전원 OFF)
void exit(int status);                  // 현재 프로세스를 종료하며 상태 코드 전달 (성공이 0, 나머지는 실패)
pid_t fork (const char *thread_name);   // 현재 프로세스를 복제하여 자식 프로세스를 생성
int exec(const char *cmd_line);         // 명령어 라인에 해당하는 프로그램을 현재 프로세스에 로드하여 실행
int wait(tid_t pid);                    // 자식 프로세스가 종료될 때까지 기다리고 종료 코드를 반환

/* File-related syscalls */
bool create(const char *file, unsigned initial_size);   // 새 파일 생성 (initial_size 바이트)
bool remove(const char *file);                          // 파일 삭제
int open(const char *file);                             // 파일 열기 및 파일 디스크립터 반환
int filesize(int fd);                                   // 열려있는 파일의 크기 반환
int read(int fd, void *buffer, unsigned size);          // 파일에서 size 바이트를 읽어 buffer에 저장
int write(int fd, const void *buffer, unsigned size);   // buffer의 데이터를 size 바이트만큼 파일에 쓰기
void seek(int fd, unsigned position);                   // 파일 읽기/쓰기 위치를 지정한 위치로 이동
unsigned tell(int fd);                                  // 현재 파일의 읽기/쓰기 위치 반환
void close(int fd);                                     // 열린 파일을 닫기

void validate_address (const uint64_t addr);

#endif /* userprog/syscall.h */