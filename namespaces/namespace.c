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

#define STACK_SIZE 1024 * 1024

static char child_stack[STACK_SIZE];
extern char **environ;

char *get_cmnd_path(char *cmnd) {
    char *const env[] = {NULL};
    char *path = strdup(getenv("PATH"));
    char *path_val;
    char *t1;
    t1 = calloc(1,50);

    path_val = strtok(path, ":");
    do {
        strcat(t1, path_val);
        strcat(t1,"/");
        strcat(t1, cmnd);
        if (access(t1, F_OK) == 0)
            break;
        path_val = strtok(NULL,":");
        memset(t1, 0, 50);
    } while (path_val);

    printf("Running the command %s\n", t1);
    return t1;
}

int sandbox(void *args) {
    int rc, cnt = 0;
    char **argv = (char **)args;

    char *cmnd = get_cmnd_path(argv[0]);

    rc = execve(cmnd, argv, environ);
    perror("execve");
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {
    int rc, i, status;
    pid_t pid;
    int flags = CLONE_NEWNET | SIGCHLD;
    char *stack;
    char *args[argc - 1];

    // Prep the args array to pass to child process
    i = 0;
    while (argv[i+1])
    {
        args[i] = argv[i+1];
        i++;
    }
    args[i] = NULL;

    pid = clone(sandbox, child_stack + STACK_SIZE, flags, args);
    printf("child pid: %d\n", pid);
    if (pid == -1) {
        perror("clone");
        return EXIT_FAILURE;
    }

    pid_t killed_pid = waitpid(pid, NULL, 0);
    if (killed_pid == -1)
        printf("pid %d exited with %d\n", killed_pid, status);

	return 0;
}
