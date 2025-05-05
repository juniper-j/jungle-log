/*
 * csapp.h - prototypes and definitions for the CS:APP3e book
 * 
 * 시스템 프로그래밍과 네트워크 프로그래밍에서 자주 쓰이는 함수들을 안전하게 사용하도록 
 * 래핑(wrapping)해놓은 헤더파일. 에러 처리를 일관되게 하고, Robust I/O, socket, pthread 등
 * 다양한 기능을 간편하게 사용할 수 있게 도와준다.
 */

 #ifndef __CSAPP_H__
 #define __CSAPP_H__
 
 /* ==== 표준 및 시스템 라이브러리 ==== */
 #include <stdio.h>       // printf, FILE, fopen 등 표준 입출력 함수
 #include <stdlib.h>      // malloc, exit 등 동적 메모리와 종료 함수
 #include <stdarg.h>      // 가변 인자 함수 관련
 #include <unistd.h>      // fork, read, write, close 등 저수준 I/O 및 프로세스 함수
 #include <string.h>      // 문자열 처리
 #include <ctype.h>       // 문자 검사 함수 (isalpha 등)
 #include <setjmp.h>      // 비정상 흐름 처리용 longjmp/setjmp
 #include <signal.h>      // 시그널 처리
 #include <dirent.h>      // 디렉토리 열기, 읽기, 닫기
 #include <sys/time.h>    // 시간 관련 구조체 및 함수
 #include <sys/types.h>   // 시스템 데이터 타입 (pid_t 등)
 #include <sys/wait.h>    // wait, waitpid 함수
 #include <sys/stat.h>    // 파일 상태 정보
 #include <fcntl.h>       // open, 파일 플래그
 #include <sys/mman.h>    // 메모리 매핑 (mmap 등)
 #include <errno.h>       // 전역 에러 변수
 #include <math.h>        // 수학 함수
 #include <pthread.h>     // POSIX 쓰레드
 #include <semaphore.h>   // 세마포어
 #include <sys/socket.h>  // socket, bind, listen, accept 등 네트워크 함수
 #include <netdb.h>       // getaddrinfo 등 DNS 관련 함수
 #include <netinet/in.h>  // sockaddr_in 구조체 등
 #include <arpa/inet.h>   // IP 주소 변환 함수
 
 /* ==== 파일 생성 시 기본 퍼미션 마스크 ==== */
 /* 예: open() 호출 시 사용 */
 #define DEF_MODE   S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH // 0666: 사용자, 그룹, 기타 모두 읽기/쓰기 허용
 #define DEF_UMASK  S_IWGRP|S_IWOTH                                 // 0022: 그룹과 기타 사용자 쓰기 금지
 
 /* ==== sockaddr 구조체 간단 표기용 별칭 ==== */
 typedef struct sockaddr SA;
 
 /* ==== Robust I/O (Rio) 패키지 구조체 정의 ==== */
 /* 버퍼 기반의 안정적인 입출력을 위한 내부 상태 저장 구조체 */
 #define RIO_BUFSIZE 8192
 typedef struct {
     int rio_fd;                // 대상 파일 디스크립터
     int rio_cnt;               // 현재 버퍼에 남아있는 바이트 수
     char *rio_bufptr;          // 버퍼에서 읽을 다음 위치
     char rio_buf[RIO_BUFSIZE]; // 내부 버퍼
 } rio_t;
 
 /* ==== 외부 전역 변수 ==== */
 extern int h_errno;     // DNS 에러용 전역 변수 (gethostbyname 등에서 사용)
 extern char **environ;  // 환경 변수
 
 /* ==== 상수 정의 ==== */
 #define MAXLINE 8192  // 텍스트 한 줄 최대 길이
 #define MAXBUF  8192  // 일반적인 버퍼 크기
 #define LISTENQ 1024  // 리슨 큐의 최대 연결 대기 수 (backlog)
 
 /* ==== 에러 처리 함수 ==== */
 void unix_error(char *msg);           // UNIX 시스템 콜 에러 처리
 void posix_error(int code, char *msg); // POSIX 함수 에러
 void dns_error(char *msg);            // DNS 관련 에러
 void gai_error(int code, char *msg);  // getaddrinfo 에러
 void app_error(char *msg);            // 일반 애플리케이션 에러
 
 /* ==== 프로세스 제어 함수 래퍼 ==== */
 pid_t Fork(void);                            // fork() 래퍼
 void Execve(const char *filename, char *const argv[], char *const envp[]); // execve() 래퍼
 pid_t Wait(int *status);                     // wait() 래퍼
 pid_t Waitpid(pid_t pid, int *iptr, int options); // waitpid() 래퍼
 void Kill(pid_t pid, int signum);            // kill() 래퍼
 unsigned int Sleep(unsigned int secs);       // sleep() 래퍼
 void Pause(void);                            // pause() 래퍼
 unsigned int Alarm(unsigned int seconds);    // alarm() 래퍼
 void Setpgid(pid_t pid, pid_t pgid);         // setpgid() 래퍼
 pid_t Getpgrp();                             // getpgrp() 래퍼
 
 /* ==== 시그널 처리 함수 ==== */
 typedef void handler_t(int);                 // 핸들러 함수 포인터 타입
 handler_t *Signal(int signum, handler_t *handler); // 시그널 핸들러 설정
 void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset); // 마스크 설정
 void Sigemptyset(sigset_t *set);             // 시그널 집합 초기화
 void Sigfillset(sigset_t *set);              // 전체 시그널 포함
 void Sigaddset(sigset_t *set, int signum);   // 시그널 추가
 void Sigdelset(sigset_t *set, int signum);   // 시그널 제거
 int Sigismember(const sigset_t *set, int signum); // 포함 여부 확인
 int Sigsuspend(const sigset_t *set);         // 시그널 대기
 
 /* ==== 시그널 안전 입출력 (Sio) ==== */
 /* 시그널 핸들러 안에서도 안전하게 사용 가능한 출력 함수들 */
 ssize_t sio_puts(char s[]);
 ssize_t sio_putl(long v);
 void sio_error(char s[]);
 
 /* ==== Sio 래퍼 (에러 처리 포함) ==== */
 ssize_t Sio_puts(char s[]);
 ssize_t Sio_putl(long v);
 void Sio_error(char s[]);
 
 /* ==== Unix I/O 함수 래퍼 ==== */
 /* 각종 시스템 호출을 감싸서 에러 처리를 간편하게 해줌 */
 int Open(const char *pathname, int flags, mode_t mode);
 ssize_t Read(int fd, void *buf, size_t count);
 ssize_t Write(int fd, const void *buf, size_t count);
 off_t Lseek(int fildes, off_t offset, int whence);
 void Close(int fd);
 int Select(int  n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
 int Dup2(int fd1, int fd2);
 void Stat(const char *filename, struct stat *buf);
 void Fstat(int fd, struct stat *buf);
 
 /* ==== 디렉토리 관련 래퍼 ==== */
 DIR *Opendir(const char *name);
 struct dirent *Readdir(DIR *dirp);
 int Closedir(DIR *dirp);
 
 /* ==== 메모리 매핑 래퍼 ==== */
 void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
 void Munmap(void *start, size_t length);
 
 /* ==== 표준 입출력 래퍼 ==== */
 void Fclose(FILE *fp);
 FILE *Fdopen(int fd, const char *type);
 char *Fgets(char *ptr, int n, FILE *stream);
 FILE *Fopen(const char *filename, const char *mode);
 void Fputs(const char *ptr, FILE *stream);
 size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
 void Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
 
 /* ==== 메모리 동적 할당 래퍼 ==== */
 void *Malloc(size_t size);
 void *Realloc(void *ptr, size_t size);
 void *Calloc(size_t nmemb, size_t size);
 void Free(void *ptr);
 
 /* ==== 소켓 인터페이스 래퍼 ==== */
 int Socket(int domain, int type, int protocol);
 void Setsockopt(int s, int level, int optname, const void *optval, int optlen);
 void Bind(int sockfd, struct sockaddr *my_addr, int addrlen);
 void Listen(int s, int backlog);
 int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
 void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
 
 /* ==== 프로토콜 독립 네트워크 함수 래퍼 ==== */
 void Getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
 void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, size_t hostlen, char *serv, size_t servlen, int flags);
 void Freeaddrinfo(struct addrinfo *res);
 void Inet_ntop(int af, const void *src, char *dst, socklen_t size);
 void Inet_pton(int af, const char *src, void *dst);
 
 /* ==== DNS 조회용 함수 ==== */
 struct hostent *Gethostbyname(const char *name);
 struct hostent *Gethostbyaddr(const char *addr, int len, int type);
 
 /* ==== POSIX 쓰레드 함수 래퍼 ==== */
 void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, void * (*routine)(void *), void *argp);
 void Pthread_join(pthread_t tid, void **thread_return);
 void Pthread_cancel(pthread_t tid);
 void Pthread_detach(pthread_t tid);
 void Pthread_exit(void *retval);
 pthread_t Pthread_self(void);
 void Pthread_once(pthread_once_t *once_control, void (*init_function)());
 
 /* ==== POSIX 세마포어 함수 ==== */
 void Sem_init(sem_t *sem, int pshared, unsigned int value);
 void P(sem_t *sem); // down
 void V(sem_t *sem); // up
 
 /* ==== Robust I/O 함수 (직접 호출용) ==== */
 ssize_t rio_readn(int fd, void *usrbuf, size_t n);            // 정확히 n 바이트 읽기
 ssize_t rio_writen(int fd, void *usrbuf, size_t n);           // 정확히 n 바이트 쓰기
 void rio_readinitb(rio_t *rp, int fd);                        // Rio 구조체 초기화
 ssize_t	rio_readnb(rio_t *rp, void *usrbuf, size_t n);        // n 바이트 버퍼에서 읽기
 ssize_t	rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen); // 한 줄 읽기
 
 /* ==== Rio 래퍼 (에러 처리 포함) ==== */
 ssize_t Rio_readn(int fd, void *usrbuf, size_t n);
 void Rio_writen(int fd, void *usrbuf, size_t n);
 void Rio_readinitb(rio_t *rp, int fd);
 ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n);
 ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
 
 /* ==== 클라이언트/서버 헬퍼 함수 ==== */
 int open_clientfd(char *hostname, char *port);
 int open_listenfd(char *port);
 int Open_clientfd(char *hostname, char *port);
 int Open_listenfd(char *port);
 
 #endif /* __CSAPP_H__ */