#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include <sys/select.h>
#include <stdbool.h>
#include "packet.h"


const char* LOG_IN = "/login";
const char* LOG_OUT = "/logout";
const char* JOIN_SESSION = "/joinsession";
const char* LEAVE_SESSION = "/leavesession";
const char* CREATE_SESSION = "/createsession";
const char* LIST = "/list";
const char* QUIT = "/quit";


bool login(int sfd, struct sockaddr_in* s_addr, char* cid, char* pw, char* sip, int port) {
  bool rec = true;
  do {
    if (connect(sfd, (struct sockaddr *)s_addr, sizeof(*s_addr)) == -1) {
      perror("connet fail\n");
      close(sfd);
      rec = false;
      break;
    }
    printf("connect successed\n");

    packet p = {0};
    strncpy((char*)p.data, pw, MAX_DATA);
    strncpy((char*)p.source, cid, MAX_NAME);
    p.type = LOGIN;
    p.size = strlen((char*)p.data);

    
  } while (0);




  return rec;
}

int main (int argc, char const *argv[]) {
  //get input
  const char* command = argv[0];
  const char* client_id = argv[1];
  const char* password = argv[2];
  const char* ip = argv[3];
  int port = atoi(argv[4]);

  //create sfd
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("create socket fail\n");
    exit(EXIT_FAILURE);
  }

  //prepare server info
  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  if (0 >= inet_pton(AF_INET, ip, &server_addr.sin_addr)) {
    fprintf(stderr, "ip convertion fail\n");
    exit(errno);
  }

  if (strcmp(command, LOG_IN) == 0) {

  } else if (strcmp(command, LOG_OUT) == 0) {
  
  } else if (strcmp(command, JOIN_SESSION) == 0) {
  
  } else if (strcmp(command, LEAVE_SESSION) == 0) {
  
  } else if (strcmp(command, CREATE_SESSION) == 0) {
  
  } else if (strcmp(command, LIST) == 0) {

  } else if (strcmp(command, QUIT) == 0) {
  
  } 





  return 0;
}