#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (unshare(CLONE_NEWNET) == -1) {
        perror("unshare");
        return EXIT_FAILURE;
    }
    execvp(argv[1], &argv[1]);
    perror("execvp");
    return EXIT_FAILURE;
}
