

#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

/************************************************
 * 함수 선언부
 ************************************************/
void *thread(void *vargp);
void doit(int fd);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void read_requesthdrs(rio_t *rp, char *host_header, char *other_header);
void parse_uri(const char *uri, char *hostname, char *port, char *path);
void reassemble_request(char *req, char *path, char *hostname, char *other_header);
void forward_response(int servedf, int fd);


/************************************************
 * main - Proxy 웹서버의 진입점
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
 *   - 클라이언트 요청을 반복적으로 수락
 *   - 각 요청을 스레드로 처리
 ************************************************/
int main(int argc, char **argv)
{
    int listenfd, connfd, clientfd;              // listenfd: 서버 리스닝 소켓, connfd: 클라이언트 연결 소켓, clientfd: 클라이언트
    char cl_hostname[MAXLINE], cl_port[MAXLINE]; // 클라이언트의 호스트 이름과 포트 번호를 저장할 버퍼
    socklen_t clientlen;                         // 클라이언트 주소 구조체의 크기
    struct sockaddr_storage clientaddr;          // 클라이언트의 주소 정보를 저장 (IPv4/IPv6 호환)

    // [1] 명령행 인자 확인 (포트 번호가 주어졌는지 확인)
    if (argc != 2) // 포트 인자가 없으면 에러 출력
    {
        fprintf(stderr, "usage: %s <port>\n", argv[0]); // 사용법 안내
        exit(1);                                        // 잘못된 실행 → 프로그램 종료
    }

    // [2] 연결 수락을 위한 서버 리스닝 소켓 생성 (주어진 포트 번호로)
    listenfd = Open_listenfd(argv[1]);

    // [3] 클라이언트의 요청을 무한히 반복 수락
    while (1) // 클라이언트 요청을 무한히 처리
    {
        clientlen = sizeof(clientaddr);                           // 구조체 크기 초기화
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 클라이언트 연결 요청 수락
        Getnameinfo((SA *)&clientaddr, clientlen,
                    cl_hostname, MAXLINE,
                    cl_port, MAXLINE, 0);                                 // 클라이언트 주소를 사람이 읽을 수 있는 문자열로 변환 (ex: 127.0.0.1, 8000)
        printf("Accepted connection from (%s, %s)\n", cl_hostname, cl_port); // 연결 확인 출력
        doit(connfd);                                                  // 클라이언트 요청 처리
        Close(connfd);  

        // int *connfdp = malloc(sizeof(int));
        // *connfdp = connfd;
        // pthread_t tid;
        // pthread_create(&tid, NULL, thread, connfdp);
    }
}
    

/************************************************
 * thread - 각 클라이언트 요청을 처리할 스레드 함수
 *
 * 인자:
 *   vargp: connfd 포인터 (동적 할당된 정수 포인터)
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - 스레드 분리 (pthread_detach)
 *   - connfd 복사 및 해제
 *   - 요청 처리 함수 호출 (doit)
 ************************************************/
void *thread(void *vargp)
{

}


// 프록시에서는 정적/동적 판단이 필요 없지만, 요청 처리 흐름을 이해하는데 유용
/************************************************
 * doit - 클라이언트의 요청을 파싱하고 서버로 전달
 *
 * 인자:
 *   fd: 클라이언트 소켓 디스크립터
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - 요청 라인 및 헤더 파싱
 *   - URI에서 서버 주소(hostname), 포트, 경로(path) 추출
 *   - 서버와 연결하여 요청 전달
 *   - 서버 응답을 클라이언트에 전송
 ************************************************/
void doit()
{
}


// 프록시에서는 클라이언트가 보내는 요청 URI에서 host/port를 따로 추출해야 하므로 개념 재활용
/************************************************
 * parse_uri - URI 문자열을 파싱하여 서버 정보 추출
 *
 * 인자:
 *   uri: 요청 URI (http://hostname:port/path)
 *   hostname: 추출된 호스트명 저장 버퍼
 *   port: 추출된 포트 번호 저장 버퍼
 *   path: 요청 경로 저장 버퍼
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - "http://" 이후 문자열에서 호스트, 포트, 경로 분리
 *   - 포트 생략 시 기본 80 사용
 ************************************************/
int parse_uri()
{
}

// 캐시된 파일을 클라이언트에 전송할 때 유사 로직 사용 가능 (ex. malloc + Rio_writen)
void serve_static(int fd, char *filename, int filesize, char *method)
{
}


/************************************************
 * read_requesthdrs - 요청 헤더를 읽고 필요한 헤더만 유지
 *
 * 인자:
 *   rp: Robust I/O 구조체 포인터
 *   other_hdrs: 필요한 추가 헤더를 누적할 버퍼
 *   host_hdr: Host 헤더 저장 버퍼
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - \r\n까지 줄 단위로 헤더를 읽음
 *   - Host 헤더를 분리 저장
 *   - 불필요한 헤더(User-Agent, Connection 등)는 제거
 *   - 나머지는 other_hdrs에 누적
 ************************************************/
void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE]; // 한 줄씩 저장할 버퍼

    Rio_readlineb(rp, buf, MAXLINE); // 첫 줄 읽기
    while (strcmp(buf, "\r\n"))      // 빈 줄이 아니면 계속
    {
        printf("==== Header ====\n%s", buf); // ← [HW 11.6.A] 읽은 요청 헤더 echo 추가(디버깅용)
        Rio_readlineb(rp, buf, MAXLINE);     // 다음 줄 읽기
    }
    return;
}


/************************************************
 * reassemble_request - 서버에 보낼 요청 메시지를 구성
 *
 * 인자:
 *   buf: 최종 요청 메시지를 저장할 버퍼
 *   path: 요청 경로
 *   hostname: Host 헤더 정보
 *   other_hdrs: 추가 요청 헤더
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - 요청 라인 및 헤더를 HTTP/1.0 형식으로 조립
 *   - 필수 헤더 (Host, Connection, Proxy-Connection, User-Agent) 포함
 *   - 기타 헤더는 other_hdrs에서 추가
 ************************************************/
void reassemble_request(char *req, char *path, char *hostname, char *other_header)
{

}


/************************************************
 * forward_response - 서버 응답을 클라이언트에게 전달
 *
 * 인자:
 *   serverfd: 서버 소켓 디스크립터
 *   clientfd: 클라이언트 소켓 디스크립터
 *
 * 반환:
 *   없음
 *
 * 기능:
 *   - Robust I/O로 서버 응답을 줄 단위 또는 블록 단위로 읽음
 *   - 클라이언트 소켓에 그대로 전달
 *   - 이후 연결 종료
 ************************************************/
void forward_response(int servedf, int fd)
{
    
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
    if (strstr(filename, ".html")) // 파일 이름에 ".html" 포함 → HTML 문서
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif")) // GIF 이미지
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png")) // PNG 이미지
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg")) // JPEG 이미지 (.jpg)
        strcpy(filetype, "image/jpeg");
    else if (strstr(filename, ".mp4")) // ← [HW 11.7] MPG 비디어 파일 처리 (최신 브라우저는 MPG 재생 지원하지 않으므로 MP4로 대체 재생해보기)
        strcpy(filetype, "video/mp4");
    else                                // 확장자가 없거나 위에 포함되지 않음 → 일반 텍스트 파일로 간주
        strcpy(filetype, "text/plain"); // 기본값
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
    char buf[MAXLINE]; // 헤더 작성용 버퍼
    char body[MAXBUF]; // HTML 본문 버퍼
    char *p = body;
    int n, remaining = sizeof(body);

    // [1] HTML 본문 작성
    n = snprintf(p, remaining, "<html><title>Tiny Error</title>");
    p += n;
    remaining -= n;

    n = snprintf(p, remaining, "<body bgcolor=\"ffffff\">\r\n");
    p += n;
    remaining -= n;

    n = snprintf(p, remaining, "%s: %s\r\n", errnum, shortmsg); // ex: 404 Not Found
    p += n;
    remaining -= n;

    n = snprintf(p, remaining, "<p>%s: %s\r\n", longmsg, cause);
    p += n;
    remaining -= n;

    n = snprintf(p, remaining, "<hr><em>The Tiny Web server</em>\r\n");
    p += n;
    remaining -= n;

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