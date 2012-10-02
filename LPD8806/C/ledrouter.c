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

#define MAXPORT 10

typedef struct {
    int threadid;
    int port;
    int striplen;
    int running;
    char *buffer;
    struct timeval lastseen;
} thread_s;

typedef struct {
    int threadid;
    int port;
    int length;
    char *host;
    char *buffer;
} sender_s;

typedef struct {
    int current;
    int port;
    struct timeval lastchange;
} controller_s;

thread_s recvthread[MAXPORT];
sender_s sendthread[MAXPORT];
controller_s controller;
pthread_t pt[MAXPORT];
pthread_t pts[MAXPORT];
pthread_mutex_t lock;
pthread_cond_t ready = PTHREAD_COND_INITIALIZER;

void showUsage(char *name)
{
    printf("%s base-port dest-ip dest-port strip-len [timeout]\n", name);
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

void *sendthrd(void *arg)
{
  sender_s *s;
  s = (sender_s *)arg;
  printf("Starting sending thread %d, sending to %s:%d!\n", s->threadid, s->host, s->port);
  int sockfd;
  char *buffer;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  char port[10];
  buffer = malloc(s->length*3);
  sprintf(port, "%d", s->port);
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  rv = getaddrinfo(s->host, port, &hints, &servinfo);
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
  while(1)
  {
    pthread_mutex_lock(&lock);
    pthread_cond_wait(&ready, &lock);
    memcpy(buffer, s->buffer, s->length*3);
    pthread_mutex_unlock(&lock);
    numbytes = sendto(sockfd, buffer, s->length*3, 0, p->ai_addr, p->ai_addrlen);
    if(numbytes == -1)
    {
      perror("sendto");
      pthread_exit((void*) 2);
    }
    printf("Sending thread %d sent %d bytes to %s:%d!\n", s->threadid, numbytes, s->host, s->port);
  }
  pthread_exit((void*) 0);
}

void *recvthrd(void *arg)
{
  thread_s *t;
  t = (thread_s *)arg;
  printf("Starting thread %d, listening on port %d\n", t->threadid, t->port);
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  struct sockaddr_storage their_addr;
  char *buf;
  buf = malloc(t->striplen*3);
  char port[10];
  sprintf(port, "%d", t->port);
  socklen_t addr_len;
  char s[INET6_ADDRSTRLEN];
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  
  if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    pthread_exit((void*) 1);
  }
  for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
                           p->ai_protocol)) == -1) {
          perror("listener: socket");
          continue;
      }
      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
      {
        close(sockfd);
        perror("listener: bind");
        continue;
      }
      break;
  }
  freeaddrinfo(servinfo);
  printf("Thread: %d - Waiting for connections\n", t->threadid);
  addr_len = sizeof their_addr;
  while(t->running)
  {
    numbytes = recvfrom(sockfd, buf, t->striplen*3, 0, (struct sockaddr *)&their_addr, &addr_len);
    if(numbytes == -1)
    {
      perror("recvfrom");
      pthread_exit((void*)3);
    }
    printf("Thread %d: Got packet (%d bytes) from %s\n", t->threadid, numbytes,
          inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    if(numbytes == t->striplen*3)
    {
      pthread_mutex_lock(&lock);
      memcpy(t->buffer, buf, t->striplen*3);
      gettimeofday(&t->lastseen, NULL);
      pthread_mutex_unlock(&lock);
    }
    else
    {
      printf("Thread %d: Packet has wrong length, ignoring\n", t->threadid);
    }
  }
  printf("Thread %d is shutting down!\n", t->threadid);
  pthread_exit((void*) 0);
}

int main(int argc, char **argv)
{
  int timeout  = 5;
  int baseport;
  int destport;
  int striplen;
  int i;
  char *destip;
  if(argc < 5)
    showUsage(argv[0]);
  baseport = atoi(argv[1]);
  destip = argv[2];
  destport = atoi(argv[4]);
  striplen = atoi(argv[3]);
  if(baseport == 0 || destport == 0 || striplen == 0)
    showUsage(argv[0]);
  if(argc == 6)
    timeout = atoi(argv[5]);
  if (timeout == 0)
    timeout = 5;
  printf("\nWelcome to the LED-router!\n\n");
  printf("Strip length: %d\n", striplen);
  printf("Config port: %d\n", baseport);
  printf("Input ports: %d -> %d\n", baseport+1, baseport + MAXPORT);
  printf("Destination: %s:%d\n", destip, destport);
  printf("Timeout: %d\n", timeout);
 
  pthread_mutex_init(&lock, NULL); 
  // Set up the receiving threads!
  for(i = 0; i < MAXPORT; i++)
  {
    recvthread[i].threadid = i;
    recvthread[i].port = baseport + 1 + i;
    recvthread[i].striplen = striplen;
    recvthread[i].buffer = malloc(striplen*3);
    recvthread[i].running = 1;
    pthread_create(&pt[i], NULL, recvthrd, (void *)&recvthread[i]);
  }
  // Set up the sending threads!
  for(i = 0; i < 1; i++)
  {
    sendthread[i].threadid = i;
    sendthread[i].port = destport;
    sendthread[i].host = destip;
    sendthread[i].buffer = malloc(striplen*3);
    sendthread[i].length = striplen; 
    pthread_create(&pts[i], NULL, sendthrd, (void *)&sendthread[i]);
  }
  printf("Current stream: %d\n", controller.current);
  while(1)
  {
    int canChange = 0;
    int shouldChange = 0;
    struct timeval now;
    gettimeofday(&now, NULL);
    pthread_mutex_lock(&lock);
    if(now.tv_sec - controller.lastchange.tv_sec > timeout)
    {
      printf("We can change source!\n");
      canChange = 1;
    }
    if(now.tv_sec - recvthread[controller.current].lastseen.tv_sec > timeout)
    {
      printf("We should change source!\n");
      shouldChange = 1;
    }
    if(shouldChange && canChange)
    {
      // Find new source.
      for(i = 0; i < MAXPORT; i++)
      {
        if(recvthread[controller.current].lastseen.tv_sec > recvthread[i].lastseen.tv_sec)
        {
          controller.current = i;
          controller.lastchange = now;
        }
      }
    }
    printf("Current stream: %d\n", controller.current);
    pthread_cond_broadcast(&ready);
    pthread_mutex_unlock(&lock);
    sleep(1);
  }
  for(i = 0; i < MAXPORT; i++)
  {
    recvthread[i].running = 0;
  }
  return 0;
}
