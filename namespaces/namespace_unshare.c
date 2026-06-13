#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/sched.h>

int main(int argc, char **argv) {
    if (unshare(CLONE_NEWNET) == -1) {
        perror("unshare");
        return EXIT_FAILURE;
    }
    execvp(argv[1], &argv[1]);
    perror("execvp");
    return EXIT_FAILURE;
}
