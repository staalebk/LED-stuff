#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

void showUsage(char *name)
{
    printf("%s dest-ip dest-port command\n", name);
    exit(1);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
   }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char **argv)
{
  char *destport;
  int i;
  char command;
  char *destip;
  if(argc < 4)
    showUsage(argv[0]);
  destip = argv[1];
  destport = argv[2];
  command = atoi(argv[3]);
  if(destport == 0 || command == 0)
    showUsage(argv[0]);
  printf("\nWelcome to the LED-router controller!\n\n");
  printf("Destination: %s:%s\n", destip, destport);
  printf("Command: %d\n", command);


  int sockfd;
  char *buffer;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  rv = getaddrinfo(destip, destport, &hints, &servinfo);
  if (rv != 0)
  {
    fprintf(stderr, "getaddrinfo %s\n", gai_strerror(rv));
    pthread_exit((void*) 1);
  }
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1) {
        perror("talker: socket");
        continue;
    }
    break;
  }
  if (p == NULL)
  {
    fprintf(stderr, "can't bind socket\n");
    pthread_exit((void*) 1);
  }
  numbytes = sendto(sockfd, &command, 1, 0, p->ai_addr, p->ai_addrlen);
  if(numbytes == -1)
  {
    perror("sendto");
  }
  printf("Sent %d bytes to %s:%s!\n", numbytes, destip, destport);

  return 0;
}
