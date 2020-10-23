#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), sendto(), and recvfrom() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() and alarm() */
#include <fcntl.h>      /* for fcntl() */
#include <sys/file.h>   /* for O_NONBLOCK and FASYNC */
#include <errno.h>      /* for errno and EINTR */
#include <signal.h>     /* for sigaction() */

#define ECHOMAX             255     /* Longest string to echo */
#define TIMEOUT_SECS        4       /* Seconds between retransmits */
#define MAXTRIES            5       /* Tries before giving up */
#define MESSAGE             80      /* Length of the message to transmit */
#define WINDOW              4       /* Size of the sliding window */
#define MAX_DATA            8       /* Maximum amount of data per packet */
#define PACKET_DATA_SIZE    1024    /* Maximum amount of data bytes in a data packet */

struct data_pkt_t {
    int type;
    int sequence_number;
    int length;
    char data[PACKET_DATA_SIZE];
};

struct ack_pkt_t {
    int type;
    int ack_no;
};

void DieWithError(char *errorMessage);   /* Error handling function */

void UDP_Client_Socket(int *sock, struct sockaddr_in *address, char *ip, unsigned short port);
void UDP_Server_Socket(int *sock, struct sockaddr_in *address, unsigned short port);
void Set_Client_Packet(struct data_pkt_t *packet, int sequence_number, char *position);
void Set_Server_Packet(struct ack_pkt_t *packet, int ack_number);
void Set_Address(struct sockaddr_in *address, char *ip, unsigned short port);
void Send_Client_Packet(int socket, struct data_pkt_t *packet, struct sockaddr_in address);
void Send_Server_Packet(int socket, struct ack_pkt_t *packet, struct sockaddr_in address);
