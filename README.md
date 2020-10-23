## TCP Simulation via UDP ##

My program is almost a straightforward implementation as described in the program document

Compiling
    To compile the client:
        linux> gcc UDPEchoClient.c utilities.c -o UDPEchoClient
    To compile the server:
        linux> gcc UDPEchoServer.c utilities.c -o UDPEchoServer

    ***All files in the zip folder must be extracted and be residing in the same directory

Running
    To run the client:
        linux> ./UDPEchoClient <ip address> <port>
    To run the server:
        linux> ./UDPEchoServer <port> [<DROP 1> <DROP 2> ...]

Limitations:
    Currently if the server is specified to drop numbered packets, then the client may report
    that it is trying to send packets with sequence numbers outside of the expected range.
