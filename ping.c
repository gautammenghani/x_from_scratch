#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <linux/icmp.h>

struct pkt {
	struct icmphdr hdr;
	char msg[64];
};

static long get_cksum(int count, struct pkg *p)
{
	
	/* Compute Internet Checksum for "count" bytes
	*         beginning at location "addr".
	*/
	register long sum = 0, checksum = 0;

	while( count > 1 )  {
		/*  This is the inner loop */
		sum += * (unsigned short) p++;
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
	p.msg = "test msg";
	p.hdr.checksum = get_cksum(64, &p);
	return p;
}

static void send_ping(int sockfd, struct addrinfo *res)
{
	printf("here\n");
}

int main(){
	int sockfd, rc;
	struct addrinfo hints, *res;
	struct pkt p;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	//hints.ai_protocol = IPPROTO_ICMP;

	rc = getaddrinfo("127.0.0.1", NULL, &hints, &res);
	if (rc)
		printf("getaddrinfo: %d\n", rc);

	sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
	if (sockfd < 0) {
		printf("socket creation failed: %d\n", sockfd);
		exit(sockfd);
	}

	p = prep_packet();
	send_ping(sockfd, res);

	// connect
	// rc = connect(sockfd, res->ai_addr, res->ai_addrlen);
	// if (rc)
	// 	printf("connect failed : %d\n", rc);
	// 	exit(rc);

	return 0;
}