#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "packet.h"

char* ptos(packet* pack_) {
  int s1 = snprintf(NULL, 0, "%d", pack_->type);
  int s2 = snprintf(NULL, 0, "%d", pack_->size);
  int s3 = strlen((char*)pack_->source);

  int total_size = s1 + 1 + s2 + 1 + s3 + 1 + pack_->size + 1;
  char *packet_str = malloc(total_size);
  if (!packet_str) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  int offset = sprintf(packet_str, "%d:%d:%s:", pack_->type, pack_->size, pack_->source);
  
  memcpy(packet_str + offset, pack_->data, pack_->size);
  offset += pack_->size;
  packet_str[offset] = '\0';

  return packet_str;
}

packet* stop(char* packet_str) {
  struct packet * str_packet;
  char *type, *size, *source, *data;
  int ptype, psize;

  type = strtok(packet_str, ":");
  size = strtok(NULL, ":");
  source = strtok(NULL, ":");

  ptype = atoi(type);
  psize = atoi(size);

  if (!source) {
    source = "server"; //default
  }

  int header_size = strlen(type) + strlen(size) + strlen(source) + 3;

  data = malloc(psize * sizeof(char));
  memcpy(data, &packet_str[header_size], psize);

  str_packet = malloc(sizeof(struct packet));
  str_packet->type = ptype;
  str_packet->size = psize;

  memcpy(str_packet->source, source, strlen(source));
  memcpy(str_packet->data, data, psize);

  return str_packet;
}

