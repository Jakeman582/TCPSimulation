#include "utilities.h"

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void UDP_Client_Socket(int *sock, struct sockaddr_in *address, char *ip, unsigned short port) {
    // Create the socket
    *sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sock < 0) {
        DieWithError("socket() failed");
    }

    // Set up the address
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(ip);
    address->sin_port = htons(port);
}

void Set_Address(struct sockaddr_in *address, char *ip, unsigned short port) {
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = inet_addr(ip);
    address->sin_port = htons(port);
}

void UDP_Server_Socket(int *sock, struct sockaddr_in *address, unsigned short port) {
    // Create the socket
    *sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(*sock < 0) {
        DieWithError("socket() failed");
    }

    // Set up the address
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = htonl(INADDR_ANY);
    address->sin_port = htons(port);

    // Bind the socket to the address
    if(bind(*sock, (struct sockaddr *)address, sizeof(*address)) < 0) {
        DieWithError("bind() failed");
    }

    if(fcntl(*sock, F_SETOWN, getpid()) < 0) {
        DieWithError("Unable to set process owner to us");
    }

    if(fcntl(*sock, F_SETFL, O_NONBLOCK | FASYNC) < 0) {
        DieWithError("Unable to put client socket into non-blocking/async mode");
    }

}

void Set_Client_Packet(struct data_pkt_t *packet, int sequence_number, char *position) {
    packet->type = (int) htonl(1);
    packet->sequence_number = (int) htonl(sequence_number);
    packet->length = (int) htonl(MAX_DATA);
    memcpy(packet->data, position, MAX_DATA);
}

void Set_Server_Packet(struct ack_pkt_t *packet, int ack_number) {
    packet->type = (int) htonl(2);
    packet->ack_no = (int) htonl(ack_number);
}
void Send_Client_Packet(int socket, struct data_pkt_t *packet, struct sockaddr_in address) {
    if(sendto(socket, packet, 20, 0, (struct sockaddr *)&address, sizeof(address)) != 20) {
        DieWithError("sendto() sent a different number of bytes than expected");
    }
    printf("SEND PACKET %d\n", (int) ntohl(packet->sequence_number));
}

void Send_Server_Packet(int socket, struct ack_pkt_t *packet, struct sockaddr_in address) {
    if(sendto(socket, packet, 8, 0, (struct sockaddr *)&address, sizeof(address)) != 8) {
        DieWithError("sendto() sent a different number of bytes than expected");
    }
    printf("-------- SEND ACK %d\n", (int) ntohl(packet->ack_no));
}
