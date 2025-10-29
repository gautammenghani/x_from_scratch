#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <linux/icmp.h>
#include <time.h>
#include <unistd.h>

struct pkt {
	struct icmphdr hdr;
	char msg[64];
};

static long get_cksum(int count, struct pkt *p)
{
	/* Compute Internet Checksum for "count" bytes
	*         beginning at location "addr".
	*/
	register long sum = 0, checksum = 0;
	void *addr;
	addr = &(p->msg);

	while( count > 1 )  {
		/*  This is the inner loop */
		sum += * ((unsigned short *) addr);
		p++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if( count > 0 )
		sum += * (unsigned char *) p;

	/*  Fold 32-bit sum to 16 bits */
	while (sum>>16)
		sum = (sum & 0xffff) + (sum >> 16);

	checksum = ~sum;
	return checksum;
}

static struct pkt prep_packet()
{
	struct pkt p;
	p.hdr.type = ICMP_ECHO;
	p.hdr.code = 0;
	memcpy(&(p.msg), "test msg", 8);
	p.hdr.checksum = get_cksum(64, &p);
	return p;
}

int main(int argc, char **argv){
	int sockfd, rc;
	struct addrinfo hints, *res;
	struct pkt p;
	char rbuffer[128];
	struct sockaddr_in r_addr;
	int addr_len = sizeof(r_addr);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	struct timespec time_start, time_end;

	if (argc != 2) {
		fprintf(stderr, "Target hostname/IP address not specified\n");
		exit(-1);
	}

	rc = getaddrinfo(argv[1], NULL, &hints, &res);
	if (rc) {
		printf("getaddrinfo: %d\n", rc);
		exit(rc);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (sockfd < 0) {
		printf("socket creation failed: %d\n", sockfd);
		exit(sockfd);
	}

	p = prep_packet();
	while (1) {
		clock_gettime(CLOCK_MONOTONIC, &time_start);
		rc = sendto(sockfd, &(p.hdr), 64, 0, res->ai_addr, res->ai_addrlen);
		if (rc < 0) {
			fprintf(stderr, "Packet could not be sent, error : %d\n", rc);
			exit(rc);
		}

		rc = recvfrom(sockfd, rbuffer, sizeof(rbuffer), 0, (struct sockaddr *)&r_addr, &addr_len);
		if (rc < 0) {
			fprintf(stderr, "Packet could not be received, error : %d\n", rc);
			exit(rc);
		}
		clock_gettime(CLOCK_MONOTONIC, &time_end);
		double timems = ((double)(time_end.tv_nsec - time_start.tv_nsec)) / 1000000.0;
		printf("Time elapsed : %.2f ms\n", timems);
		sleep(1);
	}

	return 0;
}
