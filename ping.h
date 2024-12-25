#include <limits.h>
#include <linux/types.h>
#include <ctype.h>
#include <setjmp.h>

/* IPv4 packet size / IPv6 payload size */
#define MAXPACKET       65535
#define MAX_DUP_CHK     0x10000
#define MAXWAIT         10              /* max seconds to wait for response */
#define DEFDATALEN      (64 - 8)        /* default data length */

typedef uint64_t        bitmap_t;

struct rcvd_table {
        bitmap_t bitmap[MAX_DUP_CHK / (sizeof(bitmap_t) * 8)];
};

typedef struct socket_st {
        int fd;
        int socktype;
} socket_st;

/* Node Information query */
struct ping_ni {
        int query;
        int flag;
        void *subject;
        int subject_len;
        int subject_type;
        char *group;
#if PING6_NONCE_MEMORY
        uint8_t *nonce_ptr;
#else
        struct {
                struct timeval tv;
                pid_t pid;
        } nonce_secret;
#endif
};

typedef struct ping_func_set_st {
        int (*send_probe)(struct ping_rts *rts, socket_st *, void *packet, unsigned packet_size);
        int (*receive_error_msg)(struct ping_rts *rts, socket_st *sock);
        int (*parse_reply)(struct ping_rts *rts, socket_st *, struct msghdr *msg, int len, void *addr, struct timeval *);
        void (*install_filter)(struct ping_rts *rts, socket_st *);
} ping_func_set_st;


/*ping runtime state */
struct ping_rts {
        unsigned int mark;
        unsigned char outpack[MAXPACKET];

        struct rcvd_table rcvd_tbl;

        int datalen;
        char *hostname;
        uid_t uid;
        int ident;                      /* process id to identify our packets */

        int sndbuf;
        int ttl;

        long npackets;                  /* max packets to transmit */
        long nreceived;                 /* # of packets we got back */
        long nrepeats;                  /* number of duplicates */
        long ntransmitted;              /* sequence # for outbound packets = #sent */
        long nchecksum;                 /* replies with bad checksum */
        long nerrors;                   /* icmp errors */
        int interval;                   /* interval between packets (msec) */
        int preload;
        int deadline;                   /* time to die */
        int lingertime;
        struct timespec start_time, cur_time;
        volatile int exiting;
        volatile int status_snapshot;
        int confirm;
        int confirm_flag;
        char *device;
        int pmtudisc;

        volatile int in_pr_addr;        /* pr_addr() is executing */
        jmp_buf pr_addr_jmp;

        /* timing */
        int timing;                     /* flag to do timing */
        long tmin;                      /* minimum round trip time */
        long tmax;                      /* maximum round trip time */
        double tsum;                    /* sum of all times, for doing average */
        double tsum2;
        int rtt;
        int rtt_addend;
        uint16_t acked;
        int pipesize;

        ping_func_set_st ping4_func_set;
        ping_func_set_st ping6_func_set;
        uint32_t tclass;
        uint32_t flowlabel;
        struct sockaddr_in6 source6;
        struct sockaddr_in6 whereto6;
        struct sockaddr_in6 firsthop6;
        int multicast;

        /* Used only in ping.c */
        int ts_type;
        int nroute;
        uint32_t route[10];
        struct sockaddr_in whereto;     /* who to ping */
        int optlen;
        int settos;                     /* Set TOS, Precedence or other QOS options */
        int broadcast_pings;
        struct sockaddr_in source;

        /* Used only in ping_common.c */
        int screen_width;
#ifdef HAVE_LIBCAP
        cap_value_t cap_raw;
        cap_value_t cap_admin;
#endif

        /* Used only in ping6_common.c */
        int subnet_router_anycast; /* Subnet-Router anycast (RFC 4291) */
        struct sockaddr_in6 firsthop;
        unsigned char cmsgbuf[4096];
        size_t cmsglen;
        struct ping_ni ni;

        /* boolean option bits */
        unsigned int
                opt_adaptive:1,
                opt_audible:1,
                opt_connect_sk:1,
                opt_flood:1,
                opt_flood_poll:1,
                opt_flowinfo:1,
                opt_force_lookup:1,
                opt_interval:1,
                opt_latency:1,
                opt_mark:1,
                opt_noloop:1,
                opt_numeric:1,
                opt_outstanding:1,
                opt_pingfilled:1,
                opt_ptimeofday:1,
                opt_rtt_precision:1,
                opt_quiet:1,
                opt_rroute:1,
                opt_so_debug:1,
                opt_so_dontroute:1,
                opt_sourceroute:1,
                opt_strictsource:1,
                opt_timestamp:1,
                opt_ttl:1,
                opt_verbose:1;
};

