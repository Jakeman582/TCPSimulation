#include "utilities.h"

void UseIdleTime();                     /* Function to use idle time */
void SIGIOHandler(int signalType);      /* Function to handle SIGIO */

int sock;               // Socket -- GLOBAL for signal handler
int packet_drop_index;  // Index into the arg array to determine packet drops, if any
int arg_count;          // Make argc available globally
char** arg_vector;      // Make argv available globally

int packet_received[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char buffer[MESSAGE + 1];

int main(int argc, char *argv[])
{
    struct sockaddr_in echoServAddr; /* Server address */
    unsigned short echoServPort;     /* Server port */
    struct sigaction handler;        /* Signal handling action definition */

    /* Test for correct number of parameters */
    arg_count = argc;
    arg_vector = argv;
    if (argc < 2)
    {
        fprintf(stderr,"Usage:  %s <SERVER PORT> [<DROP 1> <DROP 2> ...]\n", argv[0]);
        exit(1);
    }

    // Make sure we reference the first packet to force drop
    packet_drop_index = 2;

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    // Set up a socket for the given port on this server
    UDP_Server_Socket(&sock, &echoServAddr, echoServPort);

    /* Set signal handler for SIGIO */
    handler.sa_handler = SIGIOHandler;
    /* Create mask that mask all signals */
    if (sigfillset(&handler.sa_mask) < 0)
        DieWithError("sigfillset() failed");
    /* No flags */
    handler.sa_flags = 0;

    if (sigaction(SIGIO, &handler, 0) < 0)
        DieWithError("sigaction() failed for SIGIO");

    // Prepare the buffer to hold data
    buffer[MESSAGE] = '\0';

    /* Go off and do real work; echoing happens in the background */

    for (;;)
        UseIdleTime();

    /* NOTREACHED */
}

void UseIdleTime() {
    printf(".\n");
    sleep(3);     /* 3 seconds of activity */
}

void SIGIOHandler(int signalType) {

    int sequence_number;                // The sequence number of the received packet
    int rlen = 20;                      // The expected number of bytes in the received packet
    int recv_len;                       // The actual number of bytes received from the client
    int done = 0;                       // Was the last expected sequence number received?
    int recvMsgSize;                    // Size of datagram
    int drop_packet;                    // Do we drop the packet?
    int sequence_max;                   // Maximum sequence number starting at 0
    unsigned int clntLen;               // Address length
    char echoBuffer[ECHOMAX];           // Datagram buffer
    struct sockaddr_in echoClntAddr;    // Address of datagram source
    struct data_pkt_t receive_dp;       // Data packet received from the client
    struct ack_pkt_t ack_packet;        // ACK packet to be sent to the client

    do { /* As long as there is input... */

        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        //Wait to receive a packet
        if((recv_len = recvfrom(sock, &receive_dp, rlen, 0, (struct sockaddr *)&echoClntAddr, &clntLen)) < 0) {

            /* Only acceptable error: recvfrom() would have blocked */
            if (errno != EWOULDBLOCK) {
                DieWithError("recvfrom() failed");
            }

        // A packet was received, so process it and send a reply
        } else {

            if(!done) {

                // We need to know which packet was received
                sequence_number = (int) ntohl(receive_dp.sequence_number);

                // Determine if we drop the packet, assuming we don't want to do so in the first place
                drop_packet = 0;
                if((packet_drop_index < arg_count) && (sequence_number == atoi(arg_vector[packet_drop_index]))) {
                    drop_packet = 1;
                    packet_drop_index++;
                }

                if(!drop_packet) {

                    // Display the received packet
                    printf("RECEIVE PACKET %d\n", sequence_number);

                    // Only process packets not yet received
                    if(!packet_received[sequence_number]) {

                        // Record this packet number
                        packet_received[sequence_number] = 1;

                        // Copy its data into the server's buffer
                        memcpy(buffer + (sequence_number * MAX_DATA), receive_dp.data, MAX_DATA);

                        // Figure out the maximum sequence number of packets received starting at 0
                        sequence_max = 0;
                        while(sequence_max < 10 && packet_received[sequence_max] == 1) {
                            sequence_max++;
                        }
                        sequence_max--;

                        // Send a response
                        Set_Server_Packet(&ack_packet, sequence_max);
                        Send_Server_Packet(sock, &ack_packet, echoClntAddr);

                        // Check if we're done
                        if(sequence_number == 9) {
                            done = 1;
                            printf("%s\n", buffer);
                        }
                    }
                }
            }
        }
    }  while (recv_len >= 0);

    /* Nothing left to receive */
}
