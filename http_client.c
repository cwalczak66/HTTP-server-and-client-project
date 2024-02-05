#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h> /* for atoi() and exit() */
#include <unistd.h>
#include <sys/time.h>



int main(int argc, char *argv[])
{
    
   char *servURL;  /* url of website */
  char *filename; /* html file */

  const char token[2] = " /"; /* token to check for filename*/

  // when doing a simple GET with no file
  char req_fmt[] = "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";

  // when getting a file
  char req_file_fmt[] = "GET /%s / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n";

  unsigned int requestLen;
  char ipstr[INET6_ADDRSTRLEN];
  int status;
  int sock;                        /* Socket descriptor */
  struct sockaddr_in httpServAddr; /* Echo server address */
  int httpServPort;     /* Echo server port */
  char *ipver;
  struct addrinfo hints, *res, *p;
  struct timeval t1;
  struct timeval t2;
  int showRTT = 0;



  if (argc == 4) {
    char *servURLAll = argv[2];
    servURL = strtok(servURLAll, token);
    filename = strtok(NULL, token);

    printf("filename is: %s\n", filename);

    httpServPort = atoi(argv[3]);
    printf("Using port number %d\n", httpServPort);
    if (!strcmp(argv[1], "-p")) {
      showRTT = 1;
    }

  } else {
    char *servURLAll = argv[1];
    servURL = strtok(servURLAll, token);
    filename = strtok(NULL, token);

    printf("filename is: %s\n", filename);

    httpServPort = atoi(argv[2]);
    printf("Using port number %d\n", httpServPort);
  }
 

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
  hints.ai_socktype = SOCK_STREAM;

  if (status = getaddrinfo(servURL, NULL, &hints, &res) != 0)
  {
    printf("error getting adress\n");
  }
  printf("IP addresses for %s:\n\n", servURL);

  for (p = res; p != NULL; p = p->ai_next)
  {
    void *addr;

    // get the pointer to the address itself,
    // different fields in IPv4 and IPv6:
    if (p->ai_family == AF_INET)
    { // IPv4
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
      // convert the IP to a string and print it:
      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      printf("  %s: %s\n", ipver, ipstr);
      break;
    }
  }

  freeaddrinfo(res);
  printf("IP adress: %s\n", ipstr);
  printf("port: %d\n", httpServPort);

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&httpServAddr, 0, sizeof(httpServAddr)); /* Zero out structure */
    httpServAddr.sin_family = AF_INET; /* Internet address family */
    httpServAddr.sin_addr.s_addr = inet_addr(ipstr); /* Server IP address */
    httpServAddr.sin_port = htons(httpServPort); /* Server port */

    if (connect(sock, (struct sockaddr *) &httpServAddr, sizeof(httpServAddr)) == -1) {
        perror("conection unsucseful");
    }

    

    printf("connection made\n");
    char request[1024];
    int request_len;
    if (filename == NULL) {
        request_len = snprintf(request, 1024, req_fmt, servURL);
    }else {
        request_len = snprintf(request, 1024, req_file_fmt, filename, servURL);
    }
    
    //printf("setting up GET message, request length is %d\n", request_len);
    printf("GET message:   %s\n", request);


   
      gettimeofday(&t1, NULL);
       if (send(sock,request,strlen(request),0)==-1) {
        perror("failed to send\n");
      }
      char response[1024];
      int ret;
      printf("wait for internet response\n");
      while ((ret = read(sock, response, sizeof(response)-1)) > 0) {
        response[ret] = 0x00;
        printf("block read: \n<%s>\n", response);
      }
      gettimeofday(&t2, NULL);
      
    

    close (sock);
    
   
    
    if (showRTT == 1) {
      long elapsed = (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
      printf("RTT: %ld ms\n", (int)(elapsed/1000));
    }
    
    
}