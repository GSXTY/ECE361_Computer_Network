#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER_SIZE 256

int main (int argc, char const *argv[]) {
  
  //get the port from input argument
  int port = atoi(argv[1]);

  int sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd < 0) {
    fprintf(stderr, "socket_fd fail\n");
    exit(errno);
  }

  struct sockaddr_in server_addr = {0};
  memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  //bind sfd with server address
  if (-1 == bind(sfd, (struct sockaddr *)&server_addr, sizeof(server_addr))) {
    fprintf(stderr, "bind fail\n");
    exit(errno);
  }

  //get the message as buffer
  char buffer[BUFFER_SIZE] = {0};
  struct sockaddr_in client_addr; 
  socklen_t client = sizeof(client_addr);

  if (-1 == recvfrom(sfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client)) {
    fprintf(stderr, "receive fail\n");
    exit(errno);
  }
  
  //sleep for test the rrt
  sleep(1);

  if (strncmp(buffer, "ftp", BUFFER_SIZE) == 0) {
    if (-1 == sendto(sfd, "yes", strlen("yes"), 0, (struct sockaddr *) &client_addr, sizeof(client_addr))) {
      fprintf(stderr, "send message (yes) back fail\n");
      exit(errno);
    }
  } else {
    if (-1 == sendto(sfd, "no", strlen("no"), 0, (struct sockaddr *) &client_addr, sizeof(client_addr))) {
      fprintf(stderr, "send message (no) back fail\n");
      exit(errno);
    }
  }

  close(sfd);

  return 0;
}