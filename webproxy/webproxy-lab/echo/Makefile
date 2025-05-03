# Makefile for echo_client and echo_server

# 사용할 컴파일러 지정
CC = gcc

# 컴파일 옵션:
# -Wall: 대부분의 경고 메시지 표시
# -Wextra: 추가적인 경고 메시지 표시
# -g: 디버깅 정보 포함 (gdb로 디버깅 가능)
CFLAGS = -Wall -Wextra -g

# 링크 옵션:
# -lpthread: POSIX 스레드 라이브러리 링크 (서버에서 멀티스레드 처리에 필요)
LDFLAGS = -lpthread

# 소스 디렉토리 (현재 디렉토리 사용)
SRCDIR = .

# 생성할 최종 실행 파일 목록
TARGETS = echo_client echo_server

# 기본 빌드 타겟: make 실행 시 두 개의 타겟 모두 생성
all: $(TARGETS)

# echo_client 실행 파일 생성:
# - 컴파일 대상: echo_client.c, csapp.c
# - 링크: pthread 라이브러리 포함
echo_client: $(SRCDIR)/echo_client.c $(SRCDIR)/csapp.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	# $@ : echo_client (타겟 이름)
	# $^ : 의존 파일 목록 (echo_client.c, csapp.c)

# echo_server 실행 파일 생성:
# - 컴파일 대상: echo_server.c, csapp.c
# - 링크: pthread 라이브러리 포함
echo_server: $(SRCDIR)/echo_server.c $(SRCDIR)/csapp.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	# $@ : echo_server (타겟 이름)
	# $^ : 의존 파일 목록 (echo_server.c, csapp.c)

# make clean 명령어로 빌드된 실행 파일 제거
clean:
	rm -f $(TARGETS)