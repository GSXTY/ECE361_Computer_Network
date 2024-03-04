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

#define BUFFER_SIZE 256


bool create_message(char* file_name, char*** message_buffer, int* total_fragements) {
  //open the file
  char full_path[1024];
  snprintf(full_path, sizeof(full_path), "sending/%s", file_name);
  FILE* target_file = fopen(full_path, "rb");
	if (!target_file) {
		fprintf(stderr, "File does not exist\n");
		return false;
	}

	//find the size of the file
	fseek(target_file, 0, SEEK_END);
	long file_size  = ftell(target_file);
	int total_fragment = (file_size / 1000) + 1;
	if (file_size == -1) {
		fprintf(stderr, "ftell fail\n");
		return false;
	}
	fseek(target_file, 0, SEEK_SET);


  //create each packet string and store into message_buffer
  char data_buffer[1000];
	char** message_send = malloc(sizeof(char*) * total_fragment);
	for (int i = 1; i <= total_fragment; ++ i) {
		packet* pack = malloc(sizeof(struct packet));
		pack->total_frag = total_fragment;
		pack->frag_no = i;
		pack->file_name = file_name;

    int size = fread(data_buffer, 1, 1000, target_file);
    pack->size = size;
    memcpy(pack->file_data, data_buffer, size);
    message_send[i - 1] = malloc(DATA_SIZE * sizeof(char));
    message_send[i - 1] = ptos(pack);
    fprintf(stdout, "packet string #%d made\n", i);
	}

  *message_buffer = message_send;
  *total_fragements = total_fragment;

	return true;
}

//send the packet string to server
bool send_message(int socket_fd, struct sockaddr_in* server_address, int total_frag, char** all_message, double RTT) {
  struct timeval tv;
  fd_set readfds;

  double SRTT, RTTVAR, RTO;
  SRTT = RTT;
  RTTVAR = RTT/2;
  RTO = SRTT + 4*RTTVAR + 1;

  FD_ZERO(&readfds);

  clock_t start, end;

  for (int i = 1; i <= total_frag; ++ i) {
    char buffer[BUFFER_SIZE];
    socklen_t server_len = sizeof(*server_address);
    int byte_num = 0;

    byte_num = sendto(socket_fd, all_message[i - 1], DATA_SIZE, 0, (struct sockaddr *) server_address, sizeof(*server_address));
    if (byte_num < 0) {
      fprintf(stderr, "send packet_str number %d fail\n", i);
      return false;
    }
    fprintf(stdout, "packet string #%d send\n", i);

    while (1) {
      tv.tv_sec = RTO;
      tv.tv_usec = 0;
      FD_ZERO(&readfds);
      FD_SET(socket_fd, &readfds);
      
      int rv = select(socket_fd + 1, &readfds, NULL, NULL, &tv);
      if (rv == -1) {
        perror("select fail\n");
        return false;
      } else if (rv == 0) {
        printf("over time pack resend %d\n", i+1);
        i = i - 1;
        break;
      } else {
        char buffer[BUFFER_SIZE] = {0};
        byte_num = recvfrom(socket_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)server_address, &server_len);
        
        if (byte_num < 0) {
          perror("recvfrom fail");
          return false;
        }
        buffer[byte_num] = '\0';
        
        if (strcmp(buffer, "ACK") == 0) {
          break;
        }
      }
    }
    
  }
  // Free the packet string memory
  for(int i = 0; i < total_frag; ++i) {
    free(all_message[i]);
  }
  free(all_message);

  return true;
}


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
  char full_path[1024];
  snprintf(full_path, sizeof(full_path), "sending/%s", file_name);
  FILE *file = fopen(full_path, "r");
  if (!file) {
    fprintf(stderr, "file does not exist\n");
    exit(errno);
  } else {
    fclose(file);
  }

  //for measuring the round trip time
  struct timeval time_begin, time_end;
  gettimeofday(&time_begin, NULL);


  clock_t start, end;
  start = clock();
  //send the message to server
  if (sendto(sfd, "ftp", strlen("ftp"), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    fprintf(stderr, "send file fail\n");
    exit(errno);
  }

  char buffer[BUFFER_SIZE];
  socklen_t server_len = sizeof(server_addr);

  int byte_num = recvfrom(sfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, &server_len);
  end = clock();

  if (byte_num < 0) {
    fprintf(stderr, "reveive message fail\n");
    exit(errno);
  }
  
  double RTT = (double) (end - start) / CLOCKS_PER_SEC;
  printf("RTT: %f\n", RTT);
  // gettimeofday(&time_end, NULL);
  // long time_diff = (time_end.tv_sec - time_begin.tv_sec) * 1000L + (time_end.tv_usec - time_begin.tv_usec) / 1000L;
  // fprintf(stdout, "Round Trip Time = %ld ms.\n", time_diff);

  buffer[byte_num] = '\0';

  if (strncmp(buffer, "yes", BUFFER_SIZE) == 0) {
    fprintf(stdout, "A file transfer can start.\n");
  } else {
    fprintf(stderr, "reveive message no\n");
    exit(errno);
  }

  char** message;
  int total_fragements;
  if (!create_message(file_name, &message, &total_fragements)) {
    fprintf(stderr, "create message str fail\n");
    return false;
  }
  if (!send_message(sfd, &server_addr, total_fragements, message, RTT)) {
    fprintf(stderr, "send message str fail\n");
    return false;
  }
  
  close(sfd);

  return 0;
}