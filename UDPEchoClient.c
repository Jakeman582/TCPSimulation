#include "utilities.h"

void CatchAlarm(int ignored);            /* Handler for SIGALRM */

int tries = 0;   /* Count of times sent - GLOBAL for signal-handler access */

char buffer[MESSAGE] = "We will get through this. We will get through this together.";

int main(int argc, char *argv[])
{
    int sock;                                   // Socket descriptor
    int echoStringLen;                          // Length of string to echo
    int respStringLen;                          // Size of received datagram
    int base = 0;                               // The first packet in the window, initially 0
    int last_sent = 3;                          // The last packet in the window
    int last_ack_received = -1;                 // Last ACK number received from the server
    int last_ack = (MESSAGE / MAX_DATA) - 1;    // Last ACK we are expecting to receive
    int rlen = 8;                               // Expected number of bytes to receive from the server
    int recv_len;                               // Actual number of bytes received from the server
    int servLen;                                // Size of the server's address
    int index;                                  // Counter used for looping
    int next_window_packet;                     // Largest seuence packet allowed to be sent by the window
    unsigned short echoServPort;                // Echo server port
    unsigned int fromSize;                      // In-out of address size for recvfrom()
    char *servIP;                               // IP address of server
    char *echoString;                           // String to send to echo server
    char echoBuffer[ECHOMAX+1];                 // Buffer for echo string
    struct sockaddr_in echoServAddr;            // Echo server address
    struct sockaddr_in fromAddr;                // Source address of echo
    struct sigaction myAction;                  // For setting signal handler
    struct ack_pkt_t received_packet;           // Packet received from the server
    struct data_pkt_t packet;                   // Data packet to send

    //char buffer[MESSAGE] = "We will get through this. We will get through this together.";

    // Check to make sure the right number of arguments are supplied
    if(argc != 3){
        fprintf(stderr,"Usage: %s <Server IP> <Port>\n", argv[0]);
        exit(1);
    }

    // Get the command line arguments
    servIP = argv[1];               // Server IP address (dotted quad)
    echoServPort = atoi(argv[2]);   // Server port to use

    // Get the length of the server address structure
    servLen = sizeof(echoServAddr);

    // Create a best-effort datagram socket using UDP
    UDP_Client_Socket(&sock, &echoServAddr, servIP, echoServPort);

    // Set signal handler for alarm signal
    myAction.sa_handler = CatchAlarm;
    if (sigfillset(&myAction.sa_mask) < 0)
        DieWithError("sigfillset() failed");
    myAction.sa_flags = 0;

    if (sigaction(SIGALRM, &myAction, 0) < 0)
        DieWithError("sigaction() failed for SIGALRM");

    // Send the first 4 packets
    for(index = base; index <= last_sent; index++) {
        Set_Client_Packet(&packet, index, buffer + (index * MAX_DATA));
        Send_Client_Packet(sock, &packet, echoServAddr);
    }

    // Set the alarm
    alarm(TIMEOUT_SECS);

    // Transmit packets until the last ack_no is received
    while(last_ack_received < last_ack) {

        // Attempt to receive an ACK from the server
        while((recv_len = recvfrom(sock, &received_packet, rlen, 0, (struct sockaddr *)&echoServAddr, &servLen)) < 0) {

            // The alarm went off, so resend a window's worth of packets
            if(errno == EINTR) {
                for(index = base; index <= last_sent; index++) {
                    Set_Client_Packet(&packet, index, buffer + (index * MAX_DATA));
                    Send_Client_Packet(sock, &packet, echoServAddr);
                }

                // Reset the alarm
                alarm(TIMEOUT_SECS);
            } else {
                // Something else happened
                DieWithError("recvfrom() failed");
            }
        }

        // At this point, we received a packet, so temporarily unset the alarm
        alarm(0);

        // We need to know which ACK we received
        last_ack_received = (int) ntohl(received_packet.ack_no);

        // Display information about the received packet
        printf("-------- RECEIVE ACK %d\n", last_ack_received);

        // In this case, the ACK is a duplicate, so we should retransmit the packet with
        // sequence number 'base' only
        if(last_ack_received == (base - 1)) {
            Set_Client_Packet(&packet, base, buffer + (base * MAX_DATA));
            Send_Client_Packet(sock, &packet, echoServAddr);
        }

        // In this case, a new ACK was received, so send a new window's worth of packets
        if(base <= last_ack_received && last_ack_received < last_ack) {
            base = last_ack_received + 1;
            next_window_packet = (base + WINDOW - 1 < last_ack) ? (base + WINDOW - 1) : last_ack;
            for(index = (last_sent + 1); index <= next_window_packet; index++) {
                Set_Client_Packet(&packet, index, buffer + (index * MAX_DATA));
                Send_Client_Packet(sock, &packet, echoServAddr);
            }
            last_sent = index;
        }

        // Reset the alarm
        alarm(TIMEOUT_SECS);
    }

    close(sock);
    exit(0);
}

void CatchAlarm(int ignored)
{
    tries += 1;
}
