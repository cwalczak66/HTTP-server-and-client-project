#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() and exit() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define BUFFER_SIZE 1024
void DieWithError(char *errorMessage); /* Error handling function */
void HandleTCPClient(int clntSocket); /* TCP client handling function */
const char token[3] = " /";

int main(int argc, char *argv[])
{ int servSock; /*Socket descriptor for server */
int clntSock; /* Socket descriptor for client */
struct sockaddr_in echoServAddr; /* Local address */
struct sockaddr_in echoClntAddr; /* Client address */
unsigned short echoServPort; /* Server port */
unsigned int clntLen; /* Length of client address data structure */

if (argc != 2) /* Test for correct number of arguments */
{
fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
exit(1);
}
echoServPort = atoi(argv[1]); /* First arg: local port */
/* Create socket for incoming connections */
if ((servSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
DieWithError("socket() failed"); 

/* Construct local address structure */
memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
echoServAddr.sin_family = AF_INET; /* Internet address family */
echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
echoServAddr.sin_port = htons(echoServPort); /* Local port */
/* Bind to the local address */
if (bind (servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr))<0)
DieWithError("bind() failed");
/* Mark the socket so it will listen for incoming connections */
if (listen (servSock, MAXPENDING) < 0)
DieWithError("listen() failed");

for (;;) /* Run forever */
{
/* Set the size of the in-out parameter */
clntLen = sizeof(echoClntAddr); /* Wait for a client to connect */
//printf("waiting on client\n");
if ((clntSock = accept (servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
DieWithError("accept() failed");
/* clntSock is connected to a client! */
printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
HandleTCPClient(clntSock);
}

} 
void DieWithError(char *errorMessage)
{

	printf(errorMessage);

}
void HandleTCPClient(int clntSocket) /* TCP client handling function */
{
	char buf[BUFFER_SIZE];
	char* response_fmt="<HTTP/1.1 200 OK\r\n %s>";
	char* error_fmt="<HTTP/1.1 404 Not Found>";
	int read = recv(clntSocket, buf, BUFFER_SIZE, 0);
	//printf("%s\n", buf);
	if (read < 0) DieWithError("Client read failed\n");
		
	
	char* mess = strtok(buf, token);
	char* filename = strtok(NULL, token);
	
	

	if(!strcmp(filename, "HTTP")) {
		filename = "index.html";
	}

	//printf("file:%s\n", filename);
  	
	char source[1024];

	FILE *fp = fopen(filename, "r");
	if(fp != NULL)
	{
		while(fgets(source, 1024, fp))
		{
			send(clntSocket, source, strlen(source), 0);
		}
		fclose(fp);
	}
	else if(fp == NULL) {
		send(clntSocket, error_fmt, strlen(error_fmt), 0);
	}
	//printf("response:%s\n", source);
	
	// int err = send(clntSocket, buf, read, 0);
    //   	if (err < 0) DieWithError("Client write failed\n");

	close(clntSocket);
}
/* NOT REACHED */
