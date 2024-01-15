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

#define BUFFER_SIZE 256

int main (int argc, char const *argv[]) {
  //get the port from input argument
  int port = atoi(argv[2]);

  int sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd < 0) {
    fprintf(stderr, "socket_fd fail\n");
    exit(errno);
  }

  struct sockaddr_in server_addr = {0};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  if (0 >= inet_pton(AF_INET, argv[1], &server_addr.sin_addr)) {
    fprintf(stderr, "ip convertion fail\n");
    exit(errno);
  }
  
  //get the input
  char command[4];
  char file_name[BUFFER_SIZE];

  //check if input command is correct
  scanf("%3s %255s", command, file_name);
  if (strcmp(command, "ftp") != 0) {
    fprintf(stderr, "invalid command \n");
    exit(errno);
  }

  //check if file exist
  FILE *file = fopen(file_name, "r");
  if (!file) {
    fprintf(stderr, "file does not exist\n");
    exit(errno);
  } else {
    fclose(file);
  }

  //send the message to server
  if (sendto(sfd, "ftp", strlen("ftp"), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    fprintf(stderr, "send file fail\n");
    exit(errno);
  }

  char buffer[BUFFER_SIZE];
  socklen_t server_len = sizeof(server_addr);

  int byte_num = recvfrom(sfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &server_len);
  if (byte_num < 0) {
    fprintf(stderr, "reveive message fail\n");
    exit(errno);
  }

  buffer[byte_num] = '\0';

  if (strncmp(buffer, "yes", BUFFER_SIZE) == 0) {
    printf("A file transfer can start.\n");
  } else {
    fprintf(stderr, "reveive message no\n");
    exit(errno);
  }

  close(sfd);

  return 0;
}