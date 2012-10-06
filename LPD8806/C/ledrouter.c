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
    int random;
    int port;
    struct timeval lastchange;
    struct timeval randomsince;
} controller_s;

volatile thread_s recvthread[MAXPORT];
volatile thread_s confthread;
volatile sender_s sendthread[MAXPORT];
volatile controller_s controller;
volatile int timeout;
pthread_t pt[MAXPORT];
pthread_t pts[MAXPORT];
pthread_t ptc;
pthread_mutex_t lock;
pthread_cond_t ready = PTHREAD_COND_INITIALIZER;

void showUsage(char *name)
{
    printf("%s base-port strip-len dest-ip dest-port dest-len [dest-ip dest-port dest-len [dest-ip dest-port dest-len]]\n", name);
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
  int count = 0;
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
    count++;
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
    if(!(count%50))
      printf("Sending thread %d sent %d bytes to %s:%d!\n", s->threadid, numbytes, s->host, s->port);
  }
  pthread_exit((void*) 0);
}

void *confthrd(void *arg)
{
  thread_s *t;
  t = (thread_s *)arg;
  printf("Starting config thread %d, listening on port %d\n", t->threadid, t->port);
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
    printf("Config %d: Got packet (%d bytes) from %s\n", t->threadid, numbytes,
          inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    if(numbytes == 1)
    {
      struct timeval now;
      gettimeofday(&now, NULL);
      pthread_mutex_lock(&lock);
      if(now.tv_sec - controller.lastchange.tv_sec > timeout)
      {
        printf("Config: Changing channel to %d\n", buf[0]);
        if(buf[0] == 100)
        {
          /*
          controller.random = 1;
          controller.randomsince = now;
          */
        }
        else
        {
          controller.current = buf[0];
          controller.lastchange = now;
        }
      }
      else
      { 
        printf("Config: NOT Changing channel to %d... zapping not allowed!\n", buf[0]);
      }
      pthread_mutex_unlock(&lock);
    }
    else
    {
      printf("Configthread %d: Packet has wrong length, ignoring\n", t->threadid);
    }
  }
  printf("Thread %d is shutting down!\n", t->threadid);
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
  int count = 0; 
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
    count++;
    numbytes = recvfrom(sockfd, buf, t->striplen*3, 0, (struct sockaddr *)&their_addr, &addr_len);
    if(numbytes == -1)
    {
      perror("recvfrom");
      pthread_exit((void*)3);
    }
    if(!(count%50))
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
  timeout  = 2;
  int baseport;
  int destport[3];
  int striplen;
  int i;
  int senders = 0;
  char *destip[3];
  int destlen[3];
  int totdestlen = 0;
  if(argc < 6)
    showUsage(argv[0]);
  baseport = atoi(argv[1]);
  striplen = atoi(argv[2]);
  for(i = 0; (3+i*3+2) < argc; i++)
  {
    destip[i] =        argv[3+i*3];
    destport[i] = atoi(argv[3+i*3+1]);
    destlen[i] =  atoi(argv[3+i*3+2]);
    totdestlen += destlen[i];
    senders++;
  }
  if(baseport == 0 || destport == 0 || striplen == 0)
    showUsage(argv[0]);
  if(totdestlen != striplen)
  {
    printf("WTF!? Length does not match up! %d != %d\n", totdestlen, striplen);
    return 1;
  }
  printf("\nWelcome to the LED-router!\n\n");
  printf("Strip length: %d\n", striplen);
  printf("Config port: %d\n", baseport);
  printf("Input ports: %d -> %d\n", baseport+1, baseport + MAXPORT);
  for(i = 0; i < senders; i++)
    printf("Destination: %s:%d\n", destip[i], destport[i]);
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
  for(i = 0; i < senders; i++)
  {
    sendthread[i].threadid = i;
    sendthread[i].port = destport[i];
    sendthread[i].host = destip[i];
    sendthread[i].buffer = malloc(striplen*3);
    sendthread[i].length = destlen[i]; 
    pthread_create(&pts[i], NULL, sendthrd, (void *)&sendthread[i]);
  }
  // Config thread
  confthread.threadid = 0;
  confthread.port = baseport;
  confthread.striplen = striplen;
  confthread.running = 1;
  pthread_create(&ptc, NULL, confthrd, (void *)&confthread);
  printf("Current stream: %d\n", controller.current);
  struct timeval now;
  gettimeofday(&now, NULL);
  controller.lastchange = now;
  int count = 0;
  int random = 0;
  while(1)
  {
    int canChange = 0;
    int shouldChange = 0;
    count++;
    gettimeofday(&now, NULL);
    pthread_mutex_lock(&lock);
    /*
    printf("now       : %ld\n", now.tv_sec);
    printf("lastchange: %ld\n", controller.lastchange.tv_sec);
    printf("lastseen  : %ld\n", recvthread[controller.current].lastseen.tv_sec);
    */
    if(now.tv_sec - controller.lastchange.tv_sec > timeout)
    {
      if(!(count%50))
        printf("We can change source!\n");
      canChange = 1;
    }
    if(now.tv_sec - recvthread[controller.current].lastseen.tv_sec > timeout)
    {
      if(!(count%50))
        printf("We should change source! %ld %ld\n", now.tv_sec, recvthread[controller.current].lastseen.tv_sec);
      shouldChange = 1;
    }
    if(canChange && controller.random)
    {
      int start = (controller.current+1) % MAXPORT;
      int found = 0;
      // Find new source.
      for(i = start; i < MAXPORT; i++)
      {
        if(recvthread[controller.current].lastseen.tv_sec < recvthread[i].lastseen.tv_sec)
        {
          printf("DING! changing to %d %ld\n", i, recvthread[i].lastseen.tv_sec);
          controller.current = i;
          controller.lastchange = now;
          found = 1;
        }
      }
      if(!found)
      for(i = 0; i < start; i++)
      {
        if(recvthread[controller.current].lastseen.tv_sec < recvthread[i].lastseen.tv_sec)
        {
          printf("DING! changing to %d %ld\n", i, recvthread[i].lastseen.tv_sec);
          controller.current = i;
          controller.lastchange = now;
        }
      }
      random = controller.current;

    }
    else if(shouldChange && canChange)
    {
      // Find new source.
      for(i = 0; i < MAXPORT; i++)
      {
        if(recvthread[controller.current].lastseen.tv_sec < recvthread[i].lastseen.tv_sec)
        {
          printf("DING! changing to %d %ld\n", i, recvthread[i].lastseen.tv_sec);
          controller.current = i;
          controller.lastchange = now;
        }
      }
    }
    else if(canChange && (now.tv_sec - recvthread[0].lastseen.tv_sec) < 1)
    {
      printf("MANUAL! changing to %d %ld\n", 0, recvthread[1].lastseen.tv_sec);
      controller.current = 0;
      controller.lastchange = now;
    }
    if(!(count%50))
      printf("Current stream: %d\n", controller.current);
    int start = 0;
    for(i = 0; i < senders; i++)
    {
      memcpy(sendthread[i].buffer, recvthread[controller.current].buffer + start*3, sendthread[i].length*3);
//      printf("COPYING OFFSET %d, LENGTH %d\n", start*3, sendthread[i].length*3);
      start += sendthread[i].length;
    }
    pthread_cond_broadcast(&ready);
    pthread_mutex_unlock(&lock);
    usleep(20000);
  }
  for(i = 0; i < MAXPORT; i++)
  {
    recvthread[i].running = 0;
  }
  return 0;
}
