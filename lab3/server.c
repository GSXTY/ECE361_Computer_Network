#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/select.h>
#include "packet.h"
#define BUFFER_SIZE 256


double uniform_rand() {
    return rand() / ((double)RAND_MAX + 1);
}

int main (int argc, char const *argv[]) {
   srand(time(NULL)); // Seed the random number generator
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
  
  //sleep(1);

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


  FILE* file = NULL;
  char pack_str[DATA_SIZE] = {0};
  int byte_num;
  while (1) {
    byte_num = recvfrom(sfd, pack_str, DATA_SIZE, 0, (struct sockaddr *)&client_addr, &client);
    if (byte_num == -1) {
      fprintf(stderr, "receive str fail\n");
      exit(errno);
    }
    //pack_str[byte_num] = '\0';

    if (uniform_rand() <= 0.01) {
      printf("Packet drop simulated.\n");
      continue;
    }
    //conver the packet string into packet struct
    packet* packet_received = stop(pack_str);
    fprintf(stdout, "packet %d recived\n", packet_received->frag_no);

    //declear the new file path
    char file_path[1024];
    snprintf(file_path, sizeof(file_path), "receiving/%s", packet_received->file_name);
    
    if (packet_received->frag_no == 1) {
      file = fopen(file_path, "wb");
      if (!file) {
        fprintf(stderr, "open file fail\n");
        break;
      }
    }

    //write in the file
    if (file && fwrite(packet_received->file_data, sizeof(char), packet_received->size, file) < packet_received->size) {
      fprintf(stderr, "write file in fail\n");
      break;
    }
    int nums = sendto(sfd, "ACK", strlen("ACK"), 0, (struct sockaddr *) &client_addr, sizeof(client_addr));
    if (nums == -1) {
      fprintf(stderr, "send ack fail\n");
    }

    //stop while recive the last packet
    if (packet_received->frag_no == packet_received->total_frag) {
      fprintf(stdout, "transfer completed\n");
      fclose(file);
      file = NULL;
      free(packet_received);
      break;
    }

    free(packet_received);
  }

  close(sfd);

  return 0;
}