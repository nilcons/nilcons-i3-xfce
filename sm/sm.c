#include <X11/SM/SM.h>
#include <X11/SM/SMlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  char err[4096];
  char *client_id;
  SmPointer ctx = NULL;

  SmcConn c = SmcOpenConnection(
				NULL,
				&ctx,
				SmProtoMajor,
				SmProtoMinor,
				0,
				NULL,
				NULL,
				&client_id,
				4096,
				err
				);

  if (c) {
    printf("errge-i3sm: session manager notified of startup (got id %s), exiting...\n", client_id);
  } else {
    printf("%s\n", err);
  }

  return 0;
}
