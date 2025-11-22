#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

static int container_run(char **argv)
{
    char *cmnd = argv[2];
    char *const args[] = {argv[3], NULL};
    char *const env[] = {NULL};
    int rc;

    pid_t pid = fork();

    // parent
    if (pid > 0) {
        int status;
        pid_t killed_pid = wait(&status);
        if (WIFEXITED(status)) 
            printf("pid %d exited with %d\n", killed_pid, status);

    } else if (pid == 0) {
        //child

        rc = execve(cmnd, args, env);
        return rc;

    } else {
        fprintf(stderr, "Fork failed\n");
        exit(errno);
    }

    return 0;
}

int main(int argc, char **argv) {
    int rc;

    if (argc < 2) {
        fprintf(stderr, "Usage : ./container COMMAND <args>\n");
        fprintf(stderr, "run : Run the given command");
        exit(0);
    }

    if (strcmp(argv[1], "run") == 0) {
        rc = container_run(argv);
        if (rc) {
            fprintf(stderr, "Failed : %s\n", strerror(rc));
        }
    }

	return 0;
}
