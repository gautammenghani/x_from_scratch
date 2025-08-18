#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/capability.h>
#include <sys/types.h>
#include <netdb.h>
#include "ping.h"

#define getaddrinfo_flags (AI_CANONNAME)

int run(struct ping_rts *rts, int argc, char **argv, struct addrinfo *ai, socket_st *sock)
{
	return 0;
}

int create_socket(struct ping_rts *rts, socket_st *sock, int family, int socktype, int protocol)
{
	sock->fd = socket(family, socktype, protocol);

	if (sock->fd == -1) {
		printf("err: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	sock->socktype = socktype;

	return 0;
}

void enable_cap_raw(cap_value_t cap, cap_flag_value_t status) {
	cap_t caps;
	cap_flag_value_t is_raw;
	int ret = -1;

	caps = cap_get_proc();
	cap_get_flag(caps, cap, CAP_PERMITTED, &is_raw);
	printf("CAP_NET_RAW permitted? :%d\n", is_raw);
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

int main(int argc, char **argv) {
	int fd, ret;
	struct addrinfo *result;
	char *target;
	socket_st sock = {.fd = -1};
	struct addrinfo hints = {
		.ai_family = AF_INET,
		.ai_protocol = IPPROTO_UDP,
		.ai_socktype = SOCK_DGRAM,
		.ai_flags = getaddrinfo_flags
	};
	static struct ping_rts rts = {
		.interval = 1000,
		.preload = 1,
		.lingertime = MAXWAIT * 1000,
		.confirm_flag = MSG_CONFIRM,
		.tmin = LONG_MAX,
		.pipesize = -1,
		.datalen = DEFDATALEN,
		.ident = -1,
		.screen_width = INT_MAX,
#ifdef HAVE_LIBCAP
		.cap_raw = CAP_NET_RAW,
		.cap_admin = CAP_NET_ADMIN,
#endif
		.pmtudisc = -1,
		.source.sin_family = AF_INET,
		.source6.sin6_family = AF_INET6,
		.ni.query = -1,
		.ni.subject_type = -1,
	};

	target = argv[1];

	/* 1. Enable the CAP_NET_RAW capability  */
	//enable_cap_raw(CAP_NET_RAW, 1);

	/* 2. Create a socket with IPPROTO_ICMP protocol  */
	if (create_socket(&rts, &sock, AF_INET, SOCK_RAW, IPPROTO_ICMP)) {
		printf("error : %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	ret = getaddrinfo(target, NULL, &hints, &result);
	if (ret)
		printf("error : %s\n", strerror(errno));

	ret = run(&rts, argc, argv, result, &sock);
	close(fd);

	return 0;
}
