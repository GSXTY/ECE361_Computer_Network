#ifndef PACKET_H
#define PACKET_H
#include <string.h>

#define DATA_SIZE 1100

typedef struct packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* file_name;
    char file_data[1000];
} packet;

char* ptos(packet* pack_); // convert packet struct into string
packet* stop(char* packet_str); // convert string into packet struct

#endif