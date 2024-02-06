#ifndef PACKET_H
#define PACKET_H
#include <string.h>

#define FILENAME_LEN 100
#define DATA_SIZE 1100

typedef struct packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* file_name;
    char filedata[1000];
} packet;

char* my_atoi(int val);
char* ptos(packet* pack_);
packet* stop(char* packet_str);

#endif