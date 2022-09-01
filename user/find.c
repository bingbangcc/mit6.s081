#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 输入为：/a/b/c.txt
// 输出为：c.txt
char* fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void find(char *path, char* target_name)
{
//   printf("the path is : %s\n", path);
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  char* file_name;
//   char* sub_dir;
    // printf("the path is: %s  and the type is: %d\n", path, st.type);

  switch(st.type){
    case T_FILE:
        file_name = fmtname(path);
        if (strcmp(file_name, target_name) == 0) {
            printf("%s\n", path);
        }
        break;
    // 如果当前路径是目录的话
    case T_DIR:
        // printf("come here\n");
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            // .和..也是有的文件名
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                continue;
            }
            // de 里存的就是该目录下每个文件的信息，name和inum
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            // 此时buf里存的是当前目录下某个文件的完整地址
            // 目录地址 a/b, 其下文件地址 a/b/c.txt
            if(stat(buf, &st) < 0){
                printf("ls: cannot stat %s\n", buf);
                continue;
            }

            if (strcmp(de.name, target_name) == 0) {
                printf("%s\n", buf);
            }
            // printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
            // memcmp(sub_dir, buf, strlen(buf));
            // memmove(sub_dir, buf, strlen(buf));
            // printf("the next file name is : %s\n", buf);
            find(buf, target_name);
        }
        break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("error input\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
