#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

static int exec_flag;
static char *exec_cmd_base[MAXARG];
static int exec_cmd_count;

static void
run_command(const char *file_path)
{
  char *cmd_argv[MAXARG];
  int ac = 0;

  for (int i = 0; i < exec_cmd_count && ac < MAXARG - 1; i++)
    cmd_argv[ac++] = exec_cmd_base[i];

  if (ac < MAXARG - 1)
    cmd_argv[ac++] = (char *)file_path;
  cmd_argv[ac] = 0;

  int pid = fork();
  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }
  if (pid == 0) {
    exec(cmd_argv[0], cmd_argv);
    fprintf(2, "find: exec %s failed\n", cmd_argv[0]);
    exit(1);
  }
  wait(0);
}

static void
search(const char *current_path, const char *match_name)
{
  int fd = open(current_path, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "find: cannot open %s\n", current_path);
    return;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", current_path);
    close(fd);
    return;
  }

  if (st.type == T_FILE) {
    const char *base_name = current_path;
    for (const char *p = current_path; *p; p++) if (*p == '/') base_name = p + 1;

    if (strcmp((char *)base_name, (char *)match_name) == 0) {
      if (exec_flag) {
        run_command(current_path);
      } else {
        printf("%s\n", current_path);
      }
    }
  } else if (st.type == T_DIR) {
    char path_buf[512];
    int len = strlen((char *)current_path);
    if (len + 1 + DIRSIZ + 1 > sizeof(path_buf)) {
      fprintf(2, "find: path too long: %s\n", current_path);
      close(fd);
      return;
    }

    struct dirent de;
    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0) continue;

      char name[DIRSIZ + 1];
      memmove(name, de.name, DIRSIZ);
      name[DIRSIZ] = 0;

      if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
        continue;

      strcpy(path_buf, (char *)current_path);
      path_buf[len] = '/';
      path_buf[len + 1] = '\0';
      strcpy(path_buf + len + 1, name);

      search(path_buf, match_name);
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "usage: find <start-path> <name> [-exec <cmd> [args...]]\n");
    exit(1);
  }

  exec_flag = 0;
  exec_cmd_count = 0;

  if (argc >= 4 && strcmp(argv[3], "-exec") == 0) {
    exec_flag = 1;
    if (argc < 5) {
      fprintf(2, "find: -exec requires a command\n");
      exit(1);
    }
    for (int i = 4; i < argc && exec_cmd_count < MAXARG - 1; i++) {
      exec_cmd_base[exec_cmd_count++] = argv[i];
    }
    exec_cmd_base[exec_cmd_count] = 0;
  }

  search(argv[1], argv[2]);
  exit(0);
}
