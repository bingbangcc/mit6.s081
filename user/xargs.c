// xargs.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

int main(int argc, char *argv[]){ 
    char buf[128];
    char* xargs_argv[32];
    int i;
    for (i = 1; i < argc; ++i) {
        xargs_argv[i-1] = argv[i];
    }

    while (1) {
        i = 0;
        while (read(0, &buf[i], 1) > 0) {
            if (buf[i] == '\n') {
                break;
            }
            i++;
        }

        if (i == 0) {
            break;
        }

        // exec的第二个参数要以0结尾
        buf[i] = 0;

        xargs_argv[argc-1] = buf;
        if (fork() == 0) {
            exec(xargs_argv[0], xargs_argv);
        }
        wait(0);
    }
    exit(0);
}