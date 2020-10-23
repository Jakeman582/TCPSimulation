## TCP Simulation via UDP ##

This program simulates a TCP connection via UDP by making sure that UDP packets are sent in sequence in accordance with a sliding window.

**Compiling**  
&emsp;&emsp;To compile the client:  
        *linux> gcc UDPEchoClient.c utilities.c -o UDPEchoClient*  
    To compile the server:  
        *linux> gcc UDPEchoServer.c utilities.c -o UDPEchoServer*  

**Running**  
    To run the client:  
        *linux> ./UDPEchoClient <ip address> <port>*  
    To run the server:  
        *linux> ./UDPEchoServer <port> [<DROP 1> <DROP 2> ...]*  


