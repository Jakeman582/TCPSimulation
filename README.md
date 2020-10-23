## TCP Simulation via UDP ##

This program simulates a TCP connection via UDP by making sure that UDP packets are sent in sequence in accordance with a sliding window.   
In simulating a TCP connection, the ability to drop packets by simply not sending an ACK is provided by specifying  a list of packets to drop.  

**Compiling**  
&emsp;&emsp;To compile the client:  
&emsp;&emsp;&emsp;*linux> gcc UDPEchoClient.c utilities.c -o UDPEchoClient*  
&emsp;&emsp;To compile the server:  
&emsp;&emsp;&emsp;*linux> gcc UDPEchoServer.c utilities.c -o UDPEchoServer*  

**Running**  
&emsp;&emsp;To run the client:  
&emsp;&emsp;&emsp;*linux> ./UDPEchoClient <ip address> <port>*  
&emsp;&emsp;&emsp;&emsp;ip address - IP address of the machine running the server.  
&emsp;&emsp;&emsp;&emsp;port - The port on which the server is listening.  
&emsp;&emsp;To run the server:  
&emsp;&emsp;&emsp;*linux> ./UDPEchoServer <port> [<DROP 1> <DROP 2> ...]*  
&emsp;&emsp;&emsp;&emsp;port - The port on which this server should listen.  
&emsp;&emsp;&emsp;&emsp;DROP n - A packet to be dropped  
