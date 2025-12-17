#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/stat.h>

static void mount_fs(const char *path, const char *dir_name, const char *fs_type)
{
    char target[512];
    int rc;

    snprintf(target, sizeof(target), "%s/%s", path, dir_name);

    /* TODO: Implement mkdir properly, for now assume dir exists
    rc = mkdir("/home/gautam/fun/x_from_scratch/containers/ubuntu/proc", 0555);
    */

    rc = mount(fs_type, target, fs_type, 0, NULL);
    if (rc)
        perror("mount failed : ");
}

static int prep_new_root(char *image_name)
{
    int rc;
    char base_path[1024] = "/home/gautam/fun/x_from_scratch/containers/";
    char *path = strcat(base_path, image_name);

    printf("path : %s\n", path);

    mount_fs(path, "proc", "proc");
    mount_fs(path, "tmp", "tmpfs");
    mount_fs(path, "sys", "sysfs");

    rc = chroot("/home/gautam/fun/x_from_scratch/containers");

   if (rc) {
        perror("chroot failed\n");
   }

   rc = chdir("/");
   if (rc) {
        perror("chdir failed\n");
   }

    return rc;
}

static int container_run(int argc, char **argv)
{
    char *image_name;
    int opt, j = 0;
    int cmd_start = 5;
    char *cmnd = argv[cmd_start];
    char* tmp[argc - cmd_start + 1];
    for (int i = cmd_start;i < argc; i++)
        tmp[j++] = argv[i];
    tmp[j] = NULL;

    char **const args = tmp;
    char *const env[] = {NULL};
    int rc;

    pid_t pid = fork();

    // parent
    if (pid > 0) {
        int status;
        pid_t killed_pid = wait(&status);
        if (killed_pid == -1) 
            printf("pid %d exited with %d\n", killed_pid, status);

    } else if (pid == 0) {
        //child

        while ((opt = getopt(argc, argv, "i:c:")) != -1) {
            switch(opt) {
                case 'i':
                    image_name = optarg;
                    fprintf(stdout, "image_name: %s\n", image_name);
                    break;
                default: 
                    break;
            }
        }

        prep_new_root(image_name);
        execve(cmnd, args, env);
    } else {
        fprintf(stderr, "Fork failed\n");
        exit(errno);
    }

    return 0;
}

int main(int argc, char **argv) {
    int rc, opt;
    char *image_name;

    if (argc < 2) {
        fprintf(stderr, "Usage : ./container COMMAND <args>\n");
        fprintf(stderr, "run : Run the given command");
        exit(0);
    }

    if (strcmp(argv[1], "run") == 0) {
        rc = container_run(argc, argv);
        if (rc) {
            fprintf(stderr, "Failed : %s\n", strerror(rc));
        }
    }

	return 0;
}
