#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "packet.h"
#include <regex.h>

char* ptos(packet* pack_) {
  int s1 = snprintf(NULL, 0, "%d", pack_->total_frag);
  int s2 = snprintf(NULL, 0, "%d", pack_->frag_no);
  int s3 = snprintf(NULL, 0, "%d", pack_->size);
  int s4 = strlen(pack_->file_name);
  int s5 = pack_->size;
  int total_size = s1 + s2 + s3 + s4 + s5; 

  char * packet_str = malloc((total_size + 4) * sizeof(char));

  int header_offset = sprintf(packet_str, "%d:%d:%d:%s:",
                              pack_->total_frag,
                              pack_->frag_no,
                              pack_->size,
                              pack_->file_name);

  memcpy(&packet_str[header_offset], pack_->filedata, pack_->size);
  return packet_str;
}

packet* stop(char* packet_str) {
  struct packet * str_packet;
  char *total_frag_str, *frag_no_str, *size_str, *filename, *filedata;
  int total_frag, frag_no, size;

  total_frag_str = strtok(packet_str, ":");
  frag_no_str = strtok(NULL, ":");
  size_str = strtok(NULL, ":");
  filename = strtok(NULL, ":");

  total_frag = atoi(total_frag_str);
  frag_no = atoi(frag_no_str);
  size = atoi(size_str);

  int udp_header_size = strlen(total_frag_str) +
                        strlen(frag_no_str) +
                        strlen(size_str) +
                        strlen(filename) +
                        4;

  filedata = malloc(size*sizeof(char));
  memcpy(filedata, &packet_str[udp_header_size], size);

  str_packet = malloc(sizeof(struct packet));
  str_packet->total_frag = total_frag;
  str_packet->frag_no = frag_no;
  str_packet->size = size;
  str_packet->file_name = filename;
  memcpy(str_packet->filedata, filedata, size);

  if(size < 1000){
    str_packet->filedata[size] = '\0';
  }

  return str_packet;
}

