/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "csapp.h"

int main(void)
{
  char *buf, *p;
  char raw_query[MAXLINE];       // 원본 QUERY_STRING 보존용
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1 = 0, n2 = 0;

  /* Extract the two arguments */
  if ((buf = getenv("QUERY_STRING")) != NULL)
  {
    strcpy(raw_query, buf);     // 🔹 원본 QUERY_STRING 보존

    p = strchr(buf, '&');
    *p = '\0';                  // 🔹 buf를 "arg1=123"으로 자름
    strcpy(arg1, buf);          // arg1 = "arg1=123"
    strcpy(arg2, p + 1);        // arg2 = "arg2=456"
    n1 = atoi(strchr(arg1, '=') + 1);   // "123"
    n2 = atoi(strchr(arg2, '=') + 1);   // "456"
  }

  /* Make the response body */
  sprintf(content, "QUERY_STRING=%s\r\n<p>", raw_query);    // 🔹 보존된 원본 출력
  sprintf(content + strlen(content), "Welcome to add.com: ");
  sprintf(content + strlen(content), "THE Internet addition portal.\r\n<p>");
  sprintf(content + strlen(content), "The answer is: %d + %d = %d\r\n<p>",
          n1, n2, n1 + n2);
  sprintf(content + strlen(content), "Thanks for visiting!\r\n");

  /* Generate the HTTP response */
  printf("Content-type: text/html\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("\r\n");
  printf("%s", content);
  fflush(stdout);

  exit(0);
}
/* $end adder */
