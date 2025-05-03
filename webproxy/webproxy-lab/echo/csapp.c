/*
 * csapp.c - CS:APP3e 실습용 함수 정의 파일
 *
 * 이 파일은 시스템 호출, 네트워크 프로그래밍, 쓰레드, 시그널 처리 등을 위한
 * 에러 핸들링 래퍼 함수 및 Robust I/O 구현 등을 포함한다.
 * 목적: 저수준 시스템 함수를 안전하게 쓰도록 도와주는 유틸리티 모음
 */

 #include "csapp.h"

 /*********************************************
  * Error-handling functions                  *
  ********************************************/
 
 // Unix 스타일의 에러 메시지를 출력하고 종료 (ex: read, write 실패 등)
 void unix_error(char *msg)
 {
     fprintf(stderr, "%s: %s\n", msg, strerror(errno)); // strerror(errno): errno에 해당하는 시스템 에러 메시지
     exit(0); // 비정상 종료
 }
 
 // POSIX 함수 에러 (ex: pthread 관련) 처리 함수
 void posix_error(int code, char *msg)
 {
     fprintf(stderr, "%s: %s\n", msg, strerror(code)); // strerror(code): 전달받은 에러 코드 출력
     exit(0);
 }
 
 // getaddrinfo 계열 함수에서 발생한 에러 처리
 void gai_error(int code, char *msg)
 {
     fprintf(stderr, "%s: %s\n", msg, gai_strerror(code)); // gai_strerror(): getaddrinfo 에러 메시지
     exit(0);
 }
 
 // 일반적인 애플리케이션 에러 (직접 정의한 에러 메시지 등)
 void app_error(char *msg)
 {
     fprintf(stderr, "%s\n", msg); // 전달된 메시지만 출력
     exit(0);
 }
 
 // DNS 관련 구식 함수(gethostbyname 등)에서의 에러 처리
 void dns_error(char *msg)
 {
     fprintf(stderr, "%s\n", msg);
     exit(0);
 }
 
 
 /***********************************************
  * Wrappers for Unix process control functions *
  ***********************************************/
 
 // fork() 호출 래퍼: 자식 프로세스 생성
 pid_t Fork(void)
 {
     pid_t pid;
 
     if ((pid = fork()) < 0) // 실패 시
         unix_error("Fork error"); // 에러 메시지 출력 후 종료
     return pid; // 성공 시 자식은 0, 부모는 자식 PID
 }
 
 // execve() 호출 래퍼: 새 프로그램으로 현재 프로세스 덮어쓰기
 void Execve(const char *filename, char *const argv[], char *const envp[])
 {
     if (execve(filename, argv, envp) < 0) // 실패 시
         unix_error("Execve error");
 }
 
 // wait(): 자식 프로세스의 종료를 기다림
 pid_t Wait(int *status)
 {
     pid_t pid;
 
     if ((pid = wait(status)) < 0) // 실패 시
         unix_error("Wait error");
     return pid; // 자식 PID 반환
 }
 
 // waitpid(): 특정 자식 프로세스를 기다릴 수 있음
 pid_t Waitpid(pid_t pid, int *iptr, int options)
 {
     pid_t retpid;
 
     if ((retpid = waitpid(pid, iptr, options)) < 0)
         unix_error("Waitpid error");
     return retpid;
 }
 
 // kill(): 대상 프로세스에 시그널 전송
 void Kill(pid_t pid, int signum)
 {
     int rc;
 
     if ((rc = kill(pid, signum)) < 0)
         unix_error("Kill error");
 }
 
 // pause(): 시그널이 오기 전까지 현재 프로세스를 대기 상태로 전환
 void Pause()
 {
     (void)pause(); // pause()는 항상 -1 반환 → 무시
 }
 
 // sleep(): 주어진 시간(초) 동안 현재 프로세스를 대기시킴
 unsigned int Sleep(unsigned int secs)
 {
     unsigned int rc;
 
     if ((rc = sleep(secs)) < 0) // 이론상 실패하지 않음, 형식적 에러 처리
         unix_error("Sleep error");
     return rc;
 }
 
 // alarm(): 일정 시간 후에 SIGALRM 시그널 발생
 unsigned int Alarm(unsigned int seconds)
 {
     return alarm(seconds); // seconds 이후 SIGALRM 발생
 }
 
 // setpgid(): 프로세스 그룹 ID 설정
 void Setpgid(pid_t pid, pid_t pgid)
 {
     int rc;
 
     if ((rc = setpgid(pid, pgid)) < 0)
         unix_error("Setpgid error");
     return;
 }
 
 // getpgrp(): 현재 프로세스의 그룹 ID 반환
 pid_t Getpgrp(void)
 {
     return getpgrp(); // 에러 발생 거의 없음
 }
 
 
 /*********************************************
  * Wrappers for Unix signal functions        *
  *********************************************/
 
 // 시그널 핸들러 등록 함수
 handler_t *Signal(int signum, handler_t *handler) 
 {
     struct sigaction action, old_action;
 
     action.sa_handler = handler;  // 전달받은 핸들러 등록
     sigemptyset(&action.sa_mask); // 해당 시그널 처리 중 다른 시그널 block 하지 않음
     action.sa_flags = SA_RESTART; // 시그널 인터럽트 후 재시도 가능하게 설정
 
     if (sigaction(signum, &action, &old_action) < 0)
         unix_error("Signal error");
     return (old_action.sa_handler); // 이전 핸들러 반환
 }
 
 // 시그널 마스크 변경
 void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
 {
     if (sigprocmask(how, set, oldset) < 0)
         unix_error("Sigprocmask error");
 }
 
 // 시그널 집합 초기화 (모든 시그널 제거)
 void Sigemptyset(sigset_t *set)
 {
     if (sigemptyset(set) < 0)
         unix_error("Sigemptyset error");
 }
 
 // 시그널 집합을 모든 시그널로 채움
 void Sigfillset(sigset_t *set)
 { 
     if (sigfillset(set) < 0)
         unix_error("Sigfillset error");
 }
 
 // 시그널 집합에 시그널 추가
 void Sigaddset(sigset_t *set, int signum)
 {
     if (sigaddset(set, signum) < 0)
         unix_error("Sigaddset error");
 }
 
 // 시그널 집합에서 시그널 제거
 void Sigdelset(sigset_t *set, int signum)
 {
     if (sigdelset(set, signum) < 0)
         unix_error("Sigdelset error");
 }
 
 // 시그널 집합에 특정 시그널이 포함되어 있는지 확인
 int Sigismember(const sigset_t *set, int signum)
 {
     int rc;
     if ((rc = sigismember(set, signum)) < 0)
         unix_error("Sigismember error");
     return rc;
 }
 
 // 일시적으로 시그널 마스크를 바꿔 시그널 대기 (항상 -1 반환)
 int Sigsuspend(const sigset_t *set)
 {
     int rc = sigsuspend(set);
     if (errno != EINTR)
         unix_error("Sigsuspend error");
     return rc;
 }
 
 
 /*************************************************************
  * The Sio (Signal-safe I/O) package - simple reentrant output
  * functions that are safe for signal handlers.
  *************************************************************/
 
 // 문자열 뒤집기 함수 (K&R 참고)
 static void sio_reverse(char s[])
 {
     int c, i, j;
 
     for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
 
 // long형 숫자를 문자열로 변환 (base 지정 가능)
 static void sio_ltoa(long v, char s[], int b) 
 {
     int c, i = 0;
     int neg = v < 0;
 
     if (neg)
         v = -v;
 
     do {
         c = v % b;
         s[i++] = (c < 10) ? c + '0' : c - 10 + 'a';
     } while ((v /= b) > 0);
 
     if (neg)
         s[i++] = '-';
 
     s[i] = '\0';
     sio_reverse(s);  // 문자열 뒤집기
 }
 
 // 문자열 길이 반환
 static size_t sio_strlen(char s[])
 {
     int i = 0;
     while (s[i] != '\0')
         ++i;
     return i;
 }
 
 /*********************************************
  * Public Sio (Signal-safe I/O)              *
  *********************************************/
 
 // 문자열 출력 (stdout)
 ssize_t sio_puts(char s[])
 {
     return write(STDOUT_FILENO, s, sio_strlen(s));
 }
 
 // long 정수 출력
 ssize_t sio_putl(long v)
 {
     char s[128];
     sio_ltoa(v, s, 10);  // 10진수 문자열로 변환
     return sio_puts(s);
 }
 
 // 에러 메시지 출력 후 종료
 void sio_error(char s[])
 {
     sio_puts(s);
     _exit(1);  // 비정상 종료
 }
 
 /********************************************* 
  * Wrappers for the SIO routines             *
 **********************************************/
 
 // long 출력 + 오류 체크
 ssize_t Sio_putl(long v)
 {
     ssize_t n;
     if ((n = sio_putl(v)) < 0)
         sio_error("Sio_putl error");
     return n;
 }
 
 // 문자열 출력 + 오류 체크
 ssize_t Sio_puts(char s[])
 {
     ssize_t n;
     if ((n = sio_puts(s)) < 0)
         sio_error("Sio_puts error");
     return n;
 }
 
 // 에러 메시지 출력 후 종료 (wrapper)
 void Sio_error(char s[])
 {
     sio_error(s);
 }
 
 
 /********************************************* 
  * Wrappers for Unix I/O routines            *
  *********************************************/
 
 // Open: 파일을 열고 파일 디스크립터를 반환. 실패 시 에러 출력 후 종료
 int Open(const char *pathname, int flags, mode_t mode) 
 {
     int rc;
     if ((rc = open(pathname, flags, mode))  < 0)    // open 실패 시
         unix_error("Open error");
     return rc;  // 성공 시 파일 디스크립터 반환
 }
 
 // Read: 파일 디스크립터로부터 데이터를 읽음. 실패 시 종료
 ssize_t Read(int fd, void *buf, size_t count) 
 {
     ssize_t rc;
     if ((rc = read(fd, buf, count)) < 0)    // read 실패 시
         unix_error("Read error");   
     return rc;  // 성공 시 읽은 바이트 수 반환
 }
 
 // Write: 파일 디스크립터에 데이터를 씀. 실패 시 종료
 ssize_t Write(int fd, const void *buf, size_t count) 
 {
     ssize_t rc;
     if ((rc = write(fd, buf, count)) < 0)   // write 실패 시
         unix_error("Write error");
     return rc;  // 성공 시 쓴 바이트 수 반환
 }
 
 // Lseek: 파일 오프셋을 이동. 실패 시 종료
 off_t Lseek(int fildes, off_t offset, int whence) 
 {
     off_t rc;
     if ((rc = lseek(fildes, offset, whence)) < 0)   // lseek 실패 시
         unix_error("Lseek error");
     return rc;  // 성공 시 이동한 파일 위치 반환
 }
 
 // Close: 파일 디스크립터 닫기. 실패 시 종료
 void Close(int fd) 
 {
     int rc;
     if ((rc = close(fd)) < 0)   // close 실패 시
         unix_error("Close error");
 }
 
 // Select: I/O 다중화 함수. 실패 시 종료
 int Select(int  n, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, struct timeval *timeout) 
 {
     int rc;
     if ((rc = select(n, readfds, writefds, exceptfds, timeout)) < 0)    // select 실패 시
         unix_error("Select error");
     return rc;  // 성공 시 준비된 파일 디스크립터 개수 반환
 }
 
 // Dup2: 파일 디스크립터 복제. 실패 시 종료
 int Dup2(int fd1, int fd2) 
 {
     int rc;
     if ((rc = dup2(fd1, fd2)) < 0)  // dup2 실패 시
         unix_error("Dup2 error");
     return rc;  // 새 파일 디스크립터 반환
 }
 
 // Stat: 파일 상태 정보 얻기. 실패 시 종료
 void Stat(const char *filename, struct stat *buf) 
 {
     if (stat(filename, buf) < 0)    // stat 실패 시
         unix_error("Stat error");
 }
 
 // Fstat: 열린 파일 디스크립터 상태 정보 얻기. 실패 시 종료
 void Fstat(int fd, struct stat *buf) 
 {
     if (fstat(fd, buf) < 0)     // fstat 실패 시
         unix_error("Fstat error");
 }
 
 
 /********************************************* 
  * Wrappers for directory functions          *
  *********************************************/
 
 // Opendir: 디렉토리 열기. 실패 시 종료
 DIR *Opendir(const char *name) 
 {
     DIR *dirp = opendir(name); 
 
     if (!dirp)   // opendir 실패 시
         unix_error("opendir error");
     return dirp;  // 성공 시 디렉토리 포인터 반환
 }
 
 // Readdir: 디렉토리 내 항목 하나 읽기. 실패 시 종료
 struct dirent *Readdir(DIR *dirp)
 {
     struct dirent *dep;
 
     errno = 0;  // errno 초기화
     dep = readdir(dirp);  // 항목 읽기
     if ((dep == NULL) && (errno != 0))   // 실패 시
         unix_error("readdir error");
     return dep;  // 성공 시 항목 포인터 반환
 }
 
 // Closedir: 디렉토리 닫기. 실패 시 종료
 int Closedir(DIR *dirp) 
 {
     int rc;
 
     if ((rc = closedir(dirp)) < 0)   // closedir 실패 시
         unix_error("closedir error");
     return rc;
 }
 
 
 /*********************************************
  * Wrappers for memory mapping functions     *
  *********************************************/
 
 // Mmap: 파일을 메모리에 매핑. 실패 시 종료
 void *Mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset) 
 {
     void *ptr;
 
     if ((ptr = mmap(addr, len, prot, flags, fd, offset)) == ((void *) -1))  // 실패 시
         unix_error("mmap error");
     return ptr;  // 매핑된 메모리 주소 반환
 }
 
 // Munmap: 매핑된 메모리 해제. 실패 시 종료
 void Munmap(void *start, size_t length) 
 {
     if (munmap(start, length) < 0)   // 실패 시
         unix_error("munmap error");
 }
 
 
 /***************************************************
  * Wrappers for dynamic storage allocation functions
  ***************************************************/
 
  // Malloc - 메모리 동적 할당, 실패 시 에러 출력 후 종료
 void *Malloc(size_t size) 
 {
     void *p;
 
     if ((p  = malloc(size)) == NULL)
     unix_error("Malloc error");
     return p;
 }
 
 // Realloc - 기존 메모리 공간 재할당, 실패 시 에러 출력 후 종료
 void *Realloc(void *ptr, size_t size) 
 {
     void *p;
 
     if ((p  = realloc(ptr, size)) == NULL)
     unix_error("Realloc error");
     return p;
 }
 
 // Calloc - 0으로 초기화된 메모리 다중 할당, 실패 시 에러 출력 후 종료
 void *Calloc(size_t nmemb, size_t size) 
 {
     void *p;
 
     if ((p = calloc(nmemb, size)) == NULL)
     unix_error("Calloc error");
     return p;
 }
 
 // Free - 동적 할당된 메모리 해제
 void Free(void *ptr) 
 {
     free(ptr);
 }
 
 
 /******************************************
  * Wrappers for the Standard I/O functions.
  ******************************************/
 
 // Fclose - 파일 스트림 닫기, 실패 시 에러 출력 후 종료
 void Fclose(FILE *fp) 
 {
     if (fclose(fp) != 0)
     unix_error("Fclose error");
 }
 
 // Fdopen - 파일 디스크립터를 FILE 스트림으로 변환, 실패 시 에러 출력 후 종료
 FILE *Fdopen(int fd, const char *type) 
 {
     FILE *fp;
 
     if ((fp = fdopen(fd, type)) == NULL)
     unix_error("Fdopen error");
 
     return fp;
 }
 
 // Fgets - 파일 스트림에서 문자열 입력 받기, 오류 발생 시 종료
 char *Fgets(char *ptr, int n, FILE *stream) 
 {
     char *rptr;
 
     if (((rptr = fgets(ptr, n, stream)) == NULL) && ferror(stream))
     app_error("Fgets error");
 
     return rptr;
 }
 
 // Fopen - 파일 열기, 실패 시 에러 출력 후 종료
 FILE *Fopen(const char *filename, const char *mode) 
 {
     FILE *fp;
 
     if ((fp = fopen(filename, mode)) == NULL)
     unix_error("Fopen error");
 
     return fp;
 }
 
 // Fputs - 문자열 출력, 실패 시 에러 출력 후 종료
 void Fputs(const char *ptr, FILE *stream) 
 {
     if (fputs(ptr, stream) == EOF)
     unix_error("Fputs error");
 }
 
 // Fread - 파일에서 데이터 읽기, 실패 시 에러 출력 후 종료
 size_t Fread(void *ptr, size_t size, size_t nmemb, FILE *stream) 
 {
     size_t n;
 
     if (((n = fread(ptr, size, nmemb, stream)) < nmemb) && ferror(stream)) 
     unix_error("Fread error");
     return n;
 }
 
 // Fwrite - 파일에 데이터 쓰기, 실패 시 에러 출력 후 종료
 void Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) 
 {
     if (fwrite(ptr, size, nmemb, stream) < nmemb)
     unix_error("Fwrite error");
 }
 
 
 /********************************************* 
  * Sockets interface wrappers                *
  *********************************************/
 
  // Socket - 소켓 생성, 실패 시 에러 출력 후 종료
 int Socket(int domain, int type, int protocol) 
 {
     int rc;
 
     if ((rc = socket(domain, type, protocol)) < 0)
     unix_error("Socket error");
     return rc;
 }
 
 // Setsockopt - 소켓 옵션 설정, 실패 시 에러 출력 후 종료
 void Setsockopt(int s, int level, int optname, const void *optval, int optlen) 
 {
     int rc;
 
     if ((rc = setsockopt(s, level, optname, optval, optlen)) < 0)
     unix_error("Setsockopt error");
 }
 
 // Bind - 소켓에 주소 할당, 실패 시 에러 출력 후 종료
 void Bind(int sockfd, struct sockaddr *my_addr, int addrlen) 
 {
     int rc;
 
     if ((rc = bind(sockfd, my_addr, addrlen)) < 0)
     unix_error("Bind error");
 }
 
 // Listen - 연결 요청 대기, 실패 시 에러 출력 후 종료
 void Listen(int s, int backlog) 
 {
     int rc;
 
     if ((rc = listen(s,  backlog)) < 0)
     unix_error("Listen error");
 }
 
 // Accept - 클라이언트 연결 수락, 실패 시 에러 출력 후 종료
 int Accept(int s, struct sockaddr *addr, socklen_t *addrlen) 
 {
     int rc;
 
     if ((rc = accept(s, addr, addrlen)) < 0)
     unix_error("Accept error");
     return rc;
 }
 
 // Connect - 클라이언트가 서버에 연결 요청, 실패 시 에러 출력 후 종료
 void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen) 
 {
     int rc;
 
     if ((rc = connect(sockfd, serv_addr, addrlen)) < 0)
     unix_error("Connect error");
 }
 
 
 /********************************************* 
  * Protocol-independent wrappers             *
  *********************************************/
 
 // Getaddrinfo - 호스트명과 서비스명을 소켓 주소로 변환, 실패 시 종료
 void Getaddrinfo(const char *node, const char *service, 
     const struct addrinfo *hints, struct addrinfo **res)
 {
 int rc;
 if ((rc = getaddrinfo(node, service, hints, res)) != 0) 
 gai_error(rc, "Getaddrinfo error");
 }
 
 // Getnameinfo - 소켓 주소를 호스트명/서비스명 문자열로 변환, 실패 시 종료
 void Getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, 
     size_t hostlen, char *serv, size_t servlen, int flags)
 {
 int rc;
 if ((rc = getnameinfo(sa, salen, host, hostlen, serv, servlen, flags)) != 0) 
 gai_error(rc, "Getnameinfo error");
 }
 
 // Freeaddrinfo - getaddrinfo로 할당한 메모리 해제
 void Freeaddrinfo(struct addrinfo *res)
 {
 freeaddrinfo(res);
 }
 
 // Inet_ntop - 바이너리 IP 주소를 문자열로 변환, 실패 시 종료
 void Inet_ntop(int af, const void *src, char *dst, socklen_t size)
 {
 if (!inet_ntop(af, src, dst, size))
 unix_error("Inet_ntop error");
 }
 
 // Inet_pton - 문자열 IP 주소를 바이너리로 변환, 실패 시 종료
 void Inet_pton(int af, const char *src, void *dst) 
 {
 int rc;
 rc = inet_pton(af, src, dst);
 if (rc == 0)
 app_error("inet_pton error: invalid dotted-decimal address");
 else if (rc < 0)
 unix_error("Inet_pton error");
 }
 
 
 /*********************************************
  * DNS interface wrappers (비추천: 스레드 미지원)
  *
  * NOTE: These are obsolete because they are not thread safe.
  * Use getaddrinfo and getnameinfo instead.
  *******************************************/
 
 // Gethostbyname - 호스트명을 IP 주소로 변환 (스레드 미지원), 실패 시 종료
 struct hostent *Gethostbyname(const char *name) 
 {
     struct hostent *p;
     if ((p = gethostbyname(name)) == NULL)
         dns_error("Gethostbyname error");
     return p;
 }
 
 // Gethostbyaddr - IP 주소를 호스트명으로 변환 (스레드 미지원), 실패 시 종료
 struct hostent *Gethostbyaddr(const char *addr, int len, int type) 
 {
     struct hostent *p;
     if ((p = gethostbyaddr(addr, len, type)) == NULL)
         dns_error("Gethostbyaddr error");
     return p;
 }
 
 
 /************************************************
  * Wrappers for Pthreads thread control functions
  ************************************************/
 
 // Pthread_create - 새로운 스레드 생성, 실패 시 에러 출력 후 종료
 void Pthread_create(pthread_t *tidp, pthread_attr_t *attrp, 
     void * (*routine)(void *), void *argp) 
 {
 int rc;
 if ((rc = pthread_create(tidp, attrp, routine, argp)) != 0)
 posix_error(rc, "Pthread_create error");
 }
 
 // Pthread_cancel - 지정한 스레드에 취소 요청, 실패 시 에러 출력 후 종료
 void Pthread_cancel(pthread_t tid) {
 int rc;
 if ((rc = pthread_cancel(tid)) != 0)
 posix_error(rc, "Pthread_cancel error");
 }
 
 // Pthread_join - 스레드 종료 대기 및 반환값 수거, 실패 시 에러 출력 후 종료
 void Pthread_join(pthread_t tid, void **thread_return) {
 int rc;
 if ((rc = pthread_join(tid, thread_return)) != 0)
 posix_error(rc, "Pthread_join error");
 }
 
 // Pthread_detach - 스레드 자원 자동 회수 설정, 실패 시 에러 출력 후 종료
 void Pthread_detach(pthread_t tid) {
 int rc;
 if ((rc = pthread_detach(tid)) != 0)
 posix_error(rc, "Pthread_detach error");
 }
 
 // Pthread_exit - 현재 스레드 종료, 반환값 전달
 void Pthread_exit(void *retval) {
 pthread_exit(retval);
 }
 
 // Pthread_self - 현재 실행 중인 스레드의 ID 반환
 pthread_t Pthread_self(void) {
 return pthread_self();
 }
 
 // Pthread_once - 특정 초기화 함수 한 번만 실행, 멀티스레드 환경에서 안전
 void Pthread_once(pthread_once_t *once_control, void (*init_function)()) {
 pthread_once(once_control, init_function);
 }
 
 
 /************************************************
  * Wrappers for Posix semaphores
  ************************************************/
 
 // Sem_init - 세마포어 초기화, 실패 시 에러 출력 후 종료
 void Sem_init(sem_t *sem, int pshared, unsigned int value) 
 {
     if (sem_init(sem, pshared, value) < 0)
         unix_error("Sem_init error");
 }
 
 // P - 세마포어 wait 연산 (자원 감소), 실패 시 에러 출력 후 종료
 void P(sem_t *sem) 
 {
     if (sem_wait(sem) < 0)
         unix_error("P error");
 }
 
 // V - 세마포어 signal 연산 (자원 증가), 실패 시 에러 출력 후 종료
 void V(sem_t *sem) 
 {
     if (sem_post(sem) < 0)
         unix_error("V error");
 }
 
 
 /************************************************
  * The Rio package - Robust I/O functions
  ************************************************/
 
 // rio_readn - 요청한 바이트 수만큼 정확히 읽기 (unbuffered)
 ssize_t rio_readn(int fd, void *usrbuf, size_t n) 
 {
     size_t nleft = n;
     ssize_t nread;
     char *bufp = usrbuf;
 
     while (nleft > 0) {
         if ((nread = read(fd, bufp, nleft)) < 0) {
             if (errno == EINTR)  // 시그널 인터럽트면 다시 시도
                 nread = 0;
             else
                 return -1;
         } else if (nread == 0)
             break; // EOF
         nleft -= nread;
         bufp += nread;
     }
     return (n - nleft);
 }
 
 // rio_writen - 요청한 바이트 수만큼 정확히 쓰기 (unbuffered)
 ssize_t rio_writen(int fd, void *usrbuf, size_t n) 
 {
     size_t nleft = n;
     ssize_t nwritten;
     char *bufp = usrbuf;
 
     while (nleft > 0) {
         if ((nwritten = write(fd, bufp, nleft)) <= 0) {
             if (errno == EINTR)
                 nwritten = 0;
             else
                 return -1;
         }
         nleft -= nwritten;
         bufp += nwritten;
     }
     return n;
 }
 
 // rio_read - 내부 버퍼에서 사용자 버퍼로 데이터 복사 (필요 시 read로 버퍼 채움)
 static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
 {
     int cnt;
 
     while (rp->rio_cnt <= 0) {
         rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
         if (rp->rio_cnt < 0) {
             if (errno != EINTR)
                 return -1;
         } else if (rp->rio_cnt == 0)
             return 0;
         else
             rp->rio_bufptr = rp->rio_buf;
     }
 
     cnt = (rp->rio_cnt < n) ? rp->rio_cnt : n;
     memcpy(usrbuf, rp->rio_bufptr, cnt);
     rp->rio_bufptr += cnt;
     rp->rio_cnt -= cnt;
     return cnt;
 }
 
 // rio_readinitb - 파일 디스크립터와 버퍼를 연결하고 버퍼 초기화
 void rio_readinitb(rio_t *rp, int fd) 
 {
     rp->rio_fd = fd;  
     rp->rio_cnt = 0;  
     rp->rio_bufptr = rp->rio_buf;
 }
 
 // rio_readnb - 내부 버퍼 사용하여 n바이트 읽기 (buffered)
 ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n) 
 {
     size_t nleft = n;
     ssize_t nread;
     char *bufp = usrbuf;
     
     while (nleft > 0) {
         if ((nread = rio_read(rp, bufp, nleft)) < 0)
             return -1;
         else if (nread == 0)
             break;
         nleft -= nread;
         bufp += nread;
     }
     return (n - nleft);
 }
 
 // rio_readlineb - 한 줄씩 읽기 (버퍼링된 텍스트 입력 처리)
 ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
 {
     int n, rc;
     char c, *bufp = usrbuf;
 
     for (n = 1; n < maxlen; n++) {
         if ((rc = rio_read(rp, &c, 1)) == 1) {
             *bufp++ = c;
             if (c == '\n') {
                 n++;
                 break;
             }
         } else if (rc == 0) {
             if (n == 1)
                 return 0;  // EOF, 아무것도 읽지 않음
             else
                 break;     // EOF, 일부는 읽음
         } else
             return -1;
     }
     *bufp = 0;
     return n - 1;
 }
 
 
 /************************************************
  * Wrappers for robust I/O routines
  ************************************************/
 
 // Rio_readn - rio_readn의 래퍼 함수, 실패 시 에러 출력 후 종료
 ssize_t Rio_readn(int fd, void *ptr, size_t nbytes) 
 {
     ssize_t n;
     if ((n = rio_readn(fd, ptr, nbytes)) < 0)
         unix_error("Rio_readn error");
     return n;
 }
 
 // Rio_writen - rio_writen의 래퍼 함수, 실패 시 에러 출력 후 종료
 void Rio_writen(int fd, void *usrbuf, size_t n) 
 {
     if (rio_writen(fd, usrbuf, n) != n)
         unix_error("Rio_writen error");
 }
 
 // Rio_readinitb - rio_readinitb의 래퍼 함수, 내부 버퍼 초기화
 void Rio_readinitb(rio_t *rp, int fd)
 {
     rio_readinitb(rp, fd);
 } 
 
 // Rio_readnb - rio_readnb의 래퍼 함수, 실패 시 에러 출력 후 종료
 ssize_t Rio_readnb(rio_t *rp, void *usrbuf, size_t n) 
 {
     ssize_t rc;
     if ((rc = rio_readnb(rp, usrbuf, n)) < 0)
         unix_error("Rio_readnb error");
     return rc;
 }
 
 // Rio_readlineb - rio_readlineb의 래퍼 함수, 실패 시 에러 출력 후 종료
 ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) 
 {
     ssize_t rc;
     if ((rc = rio_readlineb(rp, usrbuf, maxlen)) < 0)
         unix_error("Rio_readlineb error");
     return rc;
 } 
 
 
 /************************************************
  * Client/server helper functions
  ************************************************/
 
 /*
  * open_clientfd - Open connection to server at <hostname, port> and
  *     return a socket descriptor ready for reading and writing. This
  *     function is reentrant and protocol-independent.
  *
  *     On error, returns: 
  *       -2 for getaddrinfo error
  *       -1 with errno set for other errors.
  */
 // open_clientfd - hostname:port로 서버 연결 요청, 실패 시 -1 또는 -2 반환
 int open_clientfd(char *hostname, char *port) {
     int clientfd, rc;
     struct addrinfo hints, *listp, *p;
 
     /* Get a list of potential server addresses */
     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_socktype = SOCK_STREAM;  /* Open a connection */
     hints.ai_flags = AI_NUMERICSERV;  /* ... using a numeric port arg. */
     hints.ai_flags |= AI_ADDRCONFIG;  /* Recommended for connections */
     if ((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
         fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", hostname, port, gai_strerror(rc));
         return -2;
     }
   
     /* Walk the list for one that we can successfully connect to */
     for (p = listp; p; p = p->ai_next) {
         /* Create a socket descriptor */
         if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
             continue; /* Socket failed, try the next */
 
         /* Connect to the server */
         if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) 
             break; /* Success */
         if (close(clientfd) < 0) { /* Connect failed, try another */  //line:netp:openclientfd:closefd
             fprintf(stderr, "open_clientfd: close failed: %s\n", strerror(errno));
             return -1;
         } 
     } 
 
     /* Clean up */
     freeaddrinfo(listp);
     if (!p) /* All connects failed */
         return -1;
     else    /* The last connect succeeded */
         return clientfd;
 }
 
 /*  
  * open_listenfd - Open and return a listening socket on port. This
  *     function is reentrant and protocol-independent.
  *
  *     On error, returns: 
  *       -2 for getaddrinfo error
  *       -1 with errno set for other errors.
  */
 // open_listenfd - 주어진 포트로 리스닝 소켓 생성, 실패 시 -1 또는 -2 반환
 int open_listenfd(char *port) 
 {
     struct addrinfo hints, *listp, *p;
     int listenfd, rc, optval=1;
 
     /* Get a list of potential server addresses */
     memset(&hints, 0, sizeof(struct addrinfo));
     hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
     hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
     hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
     if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
         fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
         return -2;
     }
 
     /* Walk the list for one that we can bind to */
     for (p = listp; p; p = p->ai_next) {
         /* Create a socket descriptor */
         if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) 
             continue;  /* Socket failed, try the next */
 
         /* Eliminates "Address already in use" error from bind */
         setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                    (const void *)&optval , sizeof(int));
 
         /* Bind the descriptor to the address */
         if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
             break; /* Success */
         if (close(listenfd) < 0) { /* Bind failed, try the next */
             fprintf(stderr, "open_listenfd close failed: %s\n", strerror(errno));
             return -1;
         }
     }
 
 
     /* Clean up */
     freeaddrinfo(listp);
     if (!p) /* No address worked */
         return -1;
 
     /* Make it a listening socket ready to accept connection requests */
     if (listen(listenfd, LISTENQ) < 0) {
         close(listenfd);
     return -1;
     }
     return listenfd;
 }
 
 
 /****************************************************
  * Wrappers for reentrant protocol-independent helpers
  ****************************************************/
 
 // Open_clientfd - open_clientfd의 래퍼 함수, 실패 시 에러 출력 후 종료
 int Open_clientfd(char *hostname, char *port) 
 {
     int rc;
 
     if ((rc = open_clientfd(hostname, port)) < 0) 
     unix_error("Open_clientfd error");
     return rc;
 }
 
 // Open_listenfd - open_listenfd의 래퍼 함수, 실패 시 에러 출력 후 종료
 int Open_listenfd(char *port) 
 {
     int rc;
 
     if ((rc = open_listenfd(port)) < 0)
     unix_error("Open_listenfd error");
     return rc;
 }
 
 /* $end csapp.c */