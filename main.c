#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/capability.h>

int create_socket() {
	int fd;

	fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (fd == -1)
		printf("err: %s\n", strerror(errno));

	return fd;
}

void enable_cap_raw(cap_value_t cap, cap_flag_value_t status) {
	cap_t caps;
	cap_flag_value_t is_raw;
	int ret = -1;

	caps = cap_get_proc();
	cap_get_flag(caps, cap, CAP_PERMITTED, &is_raw);
	if (is_raw) {
		ret = cap_set_flag(caps, CAP_EFFECTIVE, 1, &cap, status);
	} else {
		ret = status? -1: 0;
	}

	if (!ret){
		cap_set_proc(caps);
	}
	cap_free(caps);

	if (ret)
		printf("error : %s\n", strerror(errno));
}

int main() {
	int fd, ret;

	/* 1. Enable the CAP_NET_RAW capability  */
	enable_cap_raw(CAP_NET_RAW, 1);

	/* 2. Create a socket with IPPROTO_ICMP protocol  */
	fd = create_socket();

	close(fd);

	return 0;
}
