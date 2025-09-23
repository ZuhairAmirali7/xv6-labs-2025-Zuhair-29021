#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

static char *separators = " -\r\t\n./,";

static int is_separator(char ch) {
  for (char *p = separators; *p; p++) {
    if (*p == ch) return 1;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(2, "hello\n");
    exit(1);
  }

  for (int fileIdx = 1; fileIdx < argc; fileIdx++) {
    int fd = open(argv[fileIdx], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[fileIdx]);
      continue;
    }

    char buffer[1];
    char numBuffer[32];
    int numPos = 0;
    int canStartNumber = 1;

    while (read(fd, buffer, 1) == 1) {
      char ch = buffer[0];

      if (ch >= '0' && ch <= '9') {
        if (canStartNumber && numPos < (int)sizeof(numBuffer) - 1) {
          numBuffer[numPos++] = ch;
        }
      } else if (is_separator(ch)) {
        if (numPos > 0) {
          numBuffer[numPos] = '\0';
          int value = atoi(numBuffer);
          if (value % 5 == 0 || value % 6 == 0) {
            printf("%d\n", value);
          }
          numPos = 0;
        }
        canStartNumber = 1;
      } else {
        numPos = 0;
        canStartNumber = 0;
      }
    }

    if (numPos > 0) {
      numBuffer[numPos] = '\0';
      int value = atoi(numBuffer);
      if (value % 5 == 0 || value % 6 == 0) {
        printf("%d\n", value);
      }
    }

    close(fd);
  }

  exit(0);
}
