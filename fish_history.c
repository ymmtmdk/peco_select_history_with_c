#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define LF_CMD_MARK "\n- cmd: "
#define LF_CMD_MARK_LEN (sizeof(LF_CMD_MARK)-1)
#define LF_WHEN_MARK "\n  when: "
#define LF_WHEN_MARK_LEN (sizeof(LF_WHEN_MARK)-1)
#define UTIME_LEN (sizeof("1468212530")-1)
#define DATE_LEN (sizeof("16-07-11 13:48> ")-1)
#define DATE_FORMAT "%y-%m-%d %H:%M> "

int file_size(FILE* fp){
  fseek(fp, 0L, SEEK_END);
  int size = ftell(fp);

  fseek(fp, 0L, SEEK_SET);
  return size;
}

void date_str(char* date, size_t size, char* when){
  time_t utime;
  when[UTIME_LEN] = 0;
  sscanf(when, "%ld", &utime);
  when[UTIME_LEN] = '\n';
  strftime(date, size, DATE_FORMAT, localtime(&utime));
}

void unescape(char* str){
  char *cp = str;
  while (*cp){
    if (cp[0] == '\\'){
      if (cp[1] == 'n'){
        cp[1] = '\n';
      }
      cp++;
    }

    *str++ = *cp++;
  }
  *str = 0;
}

int main(int argc, const char** argv)
{
  if (argc != 2){
    fputs("illegal argc", stderr);
    return 1;
  }

  FILE *fp;
  if ((fp = fopen(argv[1], "r")) == NULL) {
    fputs("file not found", stderr);
    return 1;
  }

  int lines_buf_size = 10000;
  char** lines = (char**)malloc(sizeof(char*)*lines_buf_size);
  int line_index = 0;
  int size = file_size(fp);
  char* history = (char*)malloc(size+DATE_LEN-LF_CMD_MARK_LEN+1);
  char* hist_ptr = history+DATE_LEN-LF_CMD_MARK_LEN+1;
  fread(hist_ptr, 1, size, fp);
  hist_ptr--;
  *hist_ptr = '\n';

  while (1){
    char *cmd = strstr(hist_ptr, LF_CMD_MARK);
    if (!cmd) break;
    cmd += LF_CMD_MARK_LEN;

    char *when = strstr(cmd, LF_WHEN_MARK);
    if (!when) break;
    when[0] = 0;
    when += LF_WHEN_MARK_LEN;

    char date[DATE_LEN+1];
    if (when[1] == '\n'){
      time_t utime = 0;
      strftime(date, sizeof(date), DATE_FORMAT, localtime(&utime));
      when += 1;
    }else{
      date_str(date, sizeof(date), when);
      when += UTIME_LEN;
    }

    unescape(cmd);
    cmd -= DATE_LEN;
    strncpy(cmd, date, DATE_LEN);
    if (line_index >= lines_buf_size){
      lines_buf_size *= 1.5;
      lines = (char**)realloc(lines, sizeof(char*)*lines_buf_size);
    }

    lines[line_index++] = cmd;
    hist_ptr = when;
  }

  for (line_index -= 1; line_index >= 0; line_index--){
    puts(lines[line_index]);
  }

  free(history);
  free(lines);

  return 0;
}

