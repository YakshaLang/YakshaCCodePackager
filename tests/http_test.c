#define YK__HTTP_IMPLEMENTATION
#include "yk__http.h"
int main(int argc, char **argv) {
  (void) argc, argv;
  yk__http_t *request =
      yk__http_get("http://news.ycombinator.com", NULL);
  if (!request) {
    printf("Invalid request.\n");
    return 1;
  }
  yk__http_status_t status = YK__HTTP_STATUS_PENDING;
  int prev_size = -1;
  while (status == YK__HTTP_STATUS_PENDING) {
    status = yk__http_process(request);
    if (prev_size != (int) request->response_size) {
      printf("%d byte(s) received.\n", (int) request->response_size);
      prev_size = (int) request->response_size;
    }
  }
  if (status == YK__HTTP_STATUS_FAILED) {
    printf("HTTP request failed (%d): %s.\n", request->status_code,
           request->reason_phrase);
    yk__http_release(request);
    return 1;
  }
  printf("\nContent type: %s\n\n%s\n", request->content_type,
         (char const *) request->response_data);
  yk__http_release(request);
  return 0;
}