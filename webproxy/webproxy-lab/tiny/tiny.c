/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 web server.
 *
 * Features:
 *   - Supports only the GET method.
 *   - Serves both static files (HTML, images) and dynamic content (via CGI).
 *   - Uses memory-mapped I/O for efficient static file serving.
 *   - Handles one connection at a time (iterative, not concurrent).
 *
 * Maintained for educational use in CS:APP (Computer Systems: A Programmer's Perspective).
 *
 * Last updated: 11/2019 by droh
 *   - Fixed aliasing issues with sprintf() in serve_static() and clienterror().
 */

/* ───────────────────────────────────────────────────────────────────
📦 Tiny Web Server Request-Handling Flow (HTTP/1.0, GET only)

   [클라이언트 브라우저]
            │
            ▼
     HTTP GET 요청 전송
            │
            ▼
    ┌──────────────────────────────┐
    │          main()              │
    │──────────────────────────────│
    │ 1. 포트번호 인자로 받음           │
    │ 2. Open_listenfd(port) 호출   │← socket + bind + listen
    │ 3. 무한 루프:                  │
    │    ┌──────────────────────┐  │
    │    │ Accept(listenfd)     │──┼▶ 클라이언트 연결 수락 → connfd 생성
    │    │ Getnameinfo()        │  │
    │    │ doit(connfd) 호출     │  │
    │    │ Close(connfd)        │  │
    │    └──────────────────────┘  │
    └──────────────────────────────┘
            │
            ▼
    ┌────────────────────────────────────────────────────────────┐
    │         doit(fd)                                           │
    │────────────────────────────────────────────────────────────│
    │ 1. 요청 라인 읽기               │← Rio_readlineb()           
    │ 2. 메서드/URI/버전 파싱          │← GET 아니면 clienterror()   
    │ 3. 요청 헤더 파싱               │← read_requesthdrs()        
    │ 4. parse_uri(uri, ...) 호출   │← 정적 or 동적 여부 판별        
    │ 5. stat(filename) 호출        │← 파일 존재 확인               
    │                              │                              
    │ ┌───────────────┬───────────────┐
    │ ▼                               ▼
    │ 정적 콘텐츠                     동적 콘텐츠 (CGI)
    │ serve_static(fd, ...)          serve_dynamic(fd, ...)
    │                               ┌──────────────────────┐
    │ - get_filetype()              │ 1. fork()로 자식 생성   │
    │ - mmap() 파일 매핑              │ 2. QUERY_STRING 설정   │
    │ - HTTP 헤더 + 본문 전송          │ 3. dup2(fd, stdout)   │
    │                               │ 4. execve 실행         │
    │                               │ 5. 부모는 waitpid()    │
    │                               └──────────────────────┘
    └──────────────────────────────────────────────────────────┘
            │
            ▼
      연결 종료 (Close(connfd))
──────────────────────────────────────────────────────────────────── */


#include "csapp.h"  // CS:APP에서 제공하는 socket, IO 관련 유틸리티 함수 모음

/************************************************
 * 함수 선언부
 ************************************************/
void doit(int fd);                  // 클라이언트 요청 처리 핵심 함수
void read_requesthdrs(rio_t *rp);   // 요청 헤더를 읽고 출력 (단순히 \r\n까지 읽음)
int parse_uri(char *uri, char *filename, char *cgiargs);    // URI에서 filename과 CGI arguments 분리
void serve_static(int fd, char *filename, int filesize);    // 정적 콘텐츠 제공
void get_filetype(char *filename, char *filetype);          // 파일 확장자로부터 MIME 타입 추정
void serve_dynamic(int fd, char *filename, char *cgiargs);  // 동적 콘텐츠(CGI) 실행 및 응답
void clienterror(int fd, char *cause, char *errnum, 
                 char *shortmsg, char *longmsg);            // 클라이언트에 에러 메시지 HTML 형식으로 응답

/************************************************
 * main - Tiny 웹서버의 진입점
 *
 * 인자:
 *   argc: 명령행 인자의 개수
 *   argv: 명령행 인자의 문자열 배열 (argv[1]은 포트 번호)
 *
 * 반환:
 *   없음 (무한 루프 실행)
 *
 * 기능:
 *   - 지정한 포트 번호로 리스닝 소켓을 열고
 *   - 클라이언트 요청을 반복적으로 수락 및 처리
 ************************************************/
int main(int argc, char **argv)
{
  int listenfd, connfd;                   // listenfd: 서버 리스닝 소켓, connfd: 클라이언트 연결 소켓
  char hostname[MAXLINE], port[MAXLINE];  // 클라이언트의 호스트 이름과 포트 번호를 저장할 버퍼
  socklen_t clientlen;                    // 클라이언트 주소 구조체의 크기
  struct sockaddr_storage clientaddr;     // 클라이언트의 주소 정보를 저장 (IPv4/IPv6 호환)

  // [1] 명령행 인자 확인 (포트 번호가 주어졌는지 확인)
  if (argc != 2)  // 포트 인자가 없으면 에러 출력
  {
    fprintf(stderr, "usage: %s <port>\n", argv[0]); // 사용법 안내
    exit(1);  // 잘못된 실행 → 프로그램 종료
  }

  // [2] 연결 수락을 위한 서버 리스닝 소켓 생성 (주어진 포트 번호로)
  listenfd = Open_listenfd(argv[1]);  

  // [3] 클라이언트의 요청을 무한히 반복 수락
  while (1) // 클라이언트 요청을 무한히 처리
  {
    clientlen = sizeof(clientaddr);   // 구조체 크기 초기화
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);   // 클라이언트 연결 요청 수락
    Getnameinfo((SA *)&clientaddr, clientlen, 
                hostname, MAXLINE, 
                port, MAXLINE, 0); // 클라이언트 주소를 사람이 읽을 수 있는 문자열로 변환 (ex: 127.0.0.1, 8000)
    printf("Accepted connection from (%s, %s)\n", hostname, port);  // 연결 확인 출력
    doit(connfd);  // 클라이언트 요청 처리
    Close(connfd); // 연결 종료
  }
}

/************************************************
 * doit - 클라이언트의 하나의 HTTP 요청을 처리
 *
 * 인자:
 *   fd: 클라이언트 연결 소켓 파일 디스크립터
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - 요청 라인 및 헤더를 파싱
 *   - URI를 분석해 정적/동적 콘텐츠 여부 판단
 *   - 정적이면 파일을 전송, 동적이면 CGI 프로그램 실행
 ************************************************/
void doit(int fd)
{
  int is_static;              // 정적 콘텐츠 여부 플래그
  struct stat sbuf;           // 파일의 상태 정보 저장용 구조체
  char buf[MAXLINE];          // 요청 라인 및 헤더를 저장할 버퍼
  char method[MAXLINE];       // 요청 메서드 (예: GET)
  char uri[MAXLINE];          // 요청 URI (예: /index.html)
  char version[MAXLINE];      // HTTP 버전
  char filename[MAXLINE];     // 로컬에서 찾을 파일 경로
  char cgiargs[MAXLINE];      // CGI 인자 (GET 요청의 쿼리스트링)
  rio_t rio;                  // Robust I/O 버퍼 구조체

  Rio_readinitb(&rio, fd);    // 클라이언트 소켓과 연결된 rio 초기화

  /* 
  책 코드랑 다름. 이걸 체크 안하면, 클라이언트가 소켓을 닫았을 때 readlineb()은 0을 리턴함
  그런데 그걸 검사하지 않으면 buf에는 아무 내용도 없고, sscanf()는 이상한 값 또는 쓰레기 값을 파싱함.
	그 결과, method, uri, version이 정상 해석되지 않고, 이상한 파일 경로로 무한하게 loop를 돌게 됨. 
  */
  if (!Rio_readlineb(&rio, buf, MAXLINE))   // 요청 라인 읽기 (예: GET /index.html HTTP/1.0)
    return;   // 클라이언트가 연결을 끊으면 아무 작어도 안 함

  printf("==== Request Line ====\n%s", buf);      // ← [숙제문제 11.6.A] 요청 라인 echo 추가(디버깅용), [숙제문제 11.6.C] HTTP 버전 출력
  sscanf(buf, "%s %s %s", method, uri, version);  // 요청 라인에서 method, uri, version 추출

  /* Parse URI from GET request */
  read_requesthdrs(&rio);                         // 요청 헤더 읽고 버리기 (\r\n까지 반복)
  is_static = parse_uri(uri, filename, cgiargs);  // URI 분석 → filename과 CGI 인자 추출, 정적/동적 여부 판단
  if (stat(filename, &sbuf) < 0)                  // 요청한 파일의 존재 여부 확인
  {
    clienterror(fd, filename, "404", "Not found",
                "Tiny couldn't find this file");
    return;
  }

  if (is_static)
  { 
    // 일반 파일이 아니거나 읽기 권한이 없으면 접근 금지
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
    {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size);   // 정상적인 정적 파일이면 전송
  }
  else
  {
    // 실행 권한이 없거나 일반 파일이 아니면 접근 금지
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
    {
      clienterror(fd, filename, "403", "Forbidden",
                  "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs);     // CGI 프로그램 실행
  }
}


/************************************************
 * read_requesthdrs - 요청 헤더를 읽고 무시
 *
 * 인자:
 *   rp: Robust I/O 구조체 포인터
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - 클라이언트의 HTTP 헤더를 줄 단위로 읽고 버린다
 *   - 빈 줄(\r\n)까지 읽으면 종료
 ************************************************/
void read_requesthdrs(rio_t *rp)
{
  char buf[MAXLINE];  // 한 줄씩 저장할 버퍼

  Rio_readlineb(rp, buf, MAXLINE);    // 첫 줄 읽기
  while (strcmp(buf, "\r\n"))         // 빈 줄이 아니면 계속
  {
    printf("==== Header ====\n%s", buf);  // ← [숙제문제 11.6.A] 읽은 요청 헤더 echo 추가(디버깅용)
    Rio_readlineb(rp, buf, MAXLINE);      // 다음 줄 읽기
  }
  return;
}


/************************************************
 * parse_uri - URI를 분석해 정적/동적 콘텐츠 구분
 *
 * 인자:
 *   uri: 클라이언트가 요청한 URI
 *   filename: 결과 파일 경로 저장 버퍼
 *   cgiargs: CGI 인자 문자열 저장 버퍼
 *
 * 반환:
 *   1: 정적 콘텐츠 (cgi-bin 포함 X)
 *   0: 동적 콘텐츠 (cgi-bin 포함 O)
 *
 * 기능:
 *   - "cgi-bin" 포함 여부로 정적/동적 판단
 *   - 경로와 인자 문자열을 분리해 filename, cgiargs에 저장
 ************************************************/
int parse_uri(char *uri, char *filename, char *cgiargs)
{
  char *ptr;

  if (!strstr(uri, "cgi-bin"))  // 동적 콘텐츠가 아닌 경우
  {
    strcpy(cgiargs, "");        // CGI 인자 없음
    strcpy(filename, ".");      // 현재 디렉토리 기준 경로 설정
    strcat(filename, uri);      // ./index.html

    if (uri[strlen(uri) - 1] == '/')  // 디렉토리 요청 시 기본 파일 설정
      strcat(filename, "home.html");
    return 1;   // 정적 콘텐츠
  }
  else  // 동적 콘텐츠인 경우 (cgi-bin 포함됨)
  {
    ptr = index(uri, '?');      // ?가 있는지 확인 (인자 구분용)
    if (ptr)
    {
      strcpy(cgiargs, ptr + 1); // ? 이후의 문자열만 저장
      *ptr = '\0';              // ?를 NULL 문자로 바꿔 uri 분리
    }
    else {
      strcpy(cgiargs, "");      // 인자가 없는 경우
    }
    strcpy(filename, ".");      // ./cgi-bin/script
    strcat(filename, uri);
    return 0;   // 동적 콘텐츠
  }
}


/************************************************
 * get_filetype - 파일 이름에서 MIME 타입 추정
 *
 * 인자:
 *   filename: 요청된 파일 경로
 *   filetype: MIME 타입을 저장할 버퍼
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - 파일 확장자(.html, .png 등)에 따라 적절한 MIME 타입 설정
 ************************************************/
void get_filetype(char *filename, char *filetype)
{
  if (strstr(filename, ".html"))      // 파일 이름에 ".html" 포함 → HTML 문서
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))  // GIF 이미지
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))  // PNG 이미지
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))  // JPEG 이미지 (.jpg)
    strcpy(filetype, "image/jpeg");
  else if (strstr(filename, ".mp4"))  // ← [숙제문제 11.7] MPG 비디어 파일 처리 (최신 브라우저는 MPG 재생 지원하지 않으므로 MP4로 대체 재생해보기)
    strcpy(filetype, "video/mp4");
  else                                // 확장자가 없거나 위에 포함되지 않음 → 일반 텍스트 파일로 간주
    strcpy(filetype, "text/plain");   // 기본값
}


/************************************************
 * serve_static - 정적 콘텐츠 전송
 *
 * 인자:
 *   fd: 클라이언트 소켓 디스크립터
 *   filename: 요청된 파일 경로
 *   filesize: 파일 크기
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - MIME 타입 확인 후 응답 헤더 전송
 *   - 파일 내용을 mmap으로 메모리에 매핑 후 클라이언트에 전송
 ************************************************/
void serve_static(int fd, char *filename, int filesize)
{
  int srcfd;                        // 디스크에서 파일을 읽기 위한 파일 디스크립터
  char *srcp;                       // 메모리에 매핑된 파일 주소
  char filetype[MAXLINE];          // MIME 타입 (Content-Type)
  char buf[MAXBUF];                // HTTP 응답 헤더 버퍼
  char *p = buf;                   // 헤더 작성에 사용할 포인터
  int n, remaining = sizeof(buf);  // snprintf로 남은 버퍼 공간 계산

  // [1] MIME 타입 결정 (ex: text/html, image/png)
  get_filetype(filename, filetype);

  // [2] HTTP 응답 헤더 작성 (버퍼에 누적)
  n = snprintf(p, remaining, "HTTP/1.0 200 OK\r\n");  // MIME 타입 결정 (text/html, image/png, …)
  p += n; remaining -= n;

  n = snprintf(p, remaining, "Server: Tiny Web Server\r\n");
  p += n; remaining -= n;

  n = snprintf(p, remaining, "Connection: close\r\n");
  p += n; remaining -= n;

  n = snprintf(p, remaining, "Content-length: %d\r\n", filesize);
  p += n; remaining -= n;

  n = snprintf(p, remaining, "Content-type: %s\r\n\r\n", filetype);
  p += n; remaining -= n;

  // [3] 응답 헤더를 클라이언트에게 전송
  Rio_writen(fd, buf, strlen(buf));
  printf("Response headers:\n%s", buf);

  // [4] 요청된 파일 열기 (읽기 전용)
  srcfd = Open(filename, O_RDONLY, 0);

  // [5] 파일을 메모리로 매핑 (효율적인 전송을 위해 mmap 사용)
  srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0); // 파일 내용을 메모리에 매핑 (빠른 파일 전송 가능)
  Close(srcfd);   // 파일 디스크립터는 닫아도 mmap은 유지됨

  Rio_writen(fd, srcp, filesize); // [6] 매핑된 파일 내용을 클라이언트로 전송
  Munmap(srcp, filesize);         // [7] 매핑 해제
}


/************************************************
 * serve_dynamic - CGI 프로그램 실행 및 응답 전송
 *
 * 인자:
 *   fd: 클라이언트 소켓 디스크립터
 *   filename: 실행할 CGI 프로그램 경로
 *   cgiargs: GET 요청의 인자 문자열
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - HTTP 응답 헤더 전송
 *   - fork 후 자식 프로세스에서 CGI 프로그램 실행 (stdout을 소켓으로 redirect)
 ************************************************/
void serve_dynamic(int fd, char *filename, char *cgiargs)
{
  char buf[MAXLINE], *emptylist[] = {NULL};
  char *p = buf;
  int n, remaining = sizeof(buf);
  pid_t pid;

  // [1] 응답 헤더 작성
  n = snprintf(p, remaining, "HTTP/1.0 200 OK\r\n");
  p += n; remaining -= n;

  n = snprintf(p, remaining, "Server: Tiny Web Server\r\n");
  p += n; remaining -= n;

  Rio_writen(fd, buf, strlen(buf)); // [2] 헤더를 클라이언트로 전송

  // [3] 자식 프로세스 생성 (CGI 실행은 자식이 담당)
  if (Fork() == 0)
  { /* Child */
    /* Real server would set all CGI vars here */
    setenv("QUERY_STRING", cgiargs, 1);   // [3-1] CGI 프로그램에 인자를 전달하기 위해 환경변수(QUERY_STRING) 설정
    Dup2(fd, STDOUT_FILENO);              // [3-2] stdout을 클라이언트 소켓 fd로 리다이렉션 (dup2)
    Execve(filename, emptylist, environ); // CGI 프로그램 실행 (인자 없음, 환경변수는 environ 사용) → 출력이 클라이언트로 전달됨
  
    // 실패 시 오류 출력 후 종료
    perror("Execve error");
    exit(1);
  }
  Wait(NULL); // [4] 부모 프로세스는 자식이 끝날 때까지 기다림
}

/************************************************
 * clienterror - 클라이언트에 HTML 형식 오류 메시지 전송
 *
 * 인자:
 *   fd: 클라이언트 소켓 디스크립터
 *   cause: 오류 발생 원인
 *   errnum: HTTP 상태 코드 (예: "404")
 *   shortmsg: 짧은 오류 메시지 (예: "Not found")
 *   longmsg: 상세 설명 메시지
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - 오류 상태코드, 메시지를 담은 HTML 응답 생성 후 클라이언트에 전송
 ************************************************/
void clienterror(int fd, char *cause, char *errnum,
                char *shortmsg, char *longmsg)
{
  char buf[MAXLINE];  // 헤더 작성용 버퍼
  char body[MAXBUF];  // HTML 본문 버퍼
  char *p = body;
  int n, remaining = sizeof(body);

  // [1] HTML 본문 작성
  n = snprintf(p, remaining, "<html><title>Tiny Error</title>");
  p += n; remaining -= n;

  n = snprintf(p, remaining, "<body bgcolor=\"ffffff\">\r\n");
  p += n; remaining -= n;

  n = snprintf(p, remaining, "%s: %s\r\n", errnum, shortmsg); // ex: 404 Not Found
  p += n; remaining -= n;

  n = snprintf(p, remaining, "<p>%s: %s\r\n", longmsg, cause);
  p += n; remaining -= n;

  n = snprintf(p, remaining, "<hr><em>The Tiny Web server</em>\r\n");
  p += n; remaining -= n;

  // [2] HTTP 응답 헤더 전송
  snprintf(buf, sizeof(buf), "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));

  snprintf(buf, sizeof(buf), "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));

  snprintf(buf, sizeof(buf), "Content-length: %ld\r\n\r\n", strlen(body));
  Rio_writen(fd, buf, strlen(buf));

  // [3] HTML 본문 전송
  Rio_writen(fd, body, strlen(body));
}