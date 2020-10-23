## TCP Simulation via UDP ##

This program simulates a TCP connection via UDP by making sure that UDP packets are sent in sequence in accordance with a sliding window.

**Compiling**  
&emsp;&emsp;To compile the client:  
&emsp;&emsp;&emsp;*linux> gcc UDPEchoClient.c utilities.c -o UDPEchoClient*  
&emsp;&emsp;To compile the server:  
&emsp;&emsp;&emsp;*linux> gcc UDPEchoServer.c utilities.c -o UDPEchoServer*  

**Running**  
&emsp;&emsp;To run the client:  
&emsp;&emsp;&emsp;*linux> ./UDPEchoClient <ip address> <port>*  
&emsp;&emsp;To run the server:  
&emsp;&emsp;&emsp;*linux> ./UDPEchoServer <port> [<DROP 1> <DROP 2> ...]*  


