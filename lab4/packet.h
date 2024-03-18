#ifndef PACKET_H
#define PACKET_H
#include <string.h>
#include <pthread.h>

#define MAX_BUFFER 1024

#define MAX_NAME 32
#define MAX_DATA 512

enum Type {
    LOGIN,
    LO_ACK,
    LO_NAK,
    EXIT,
    JOIN,
    JN_ACK,
    JN_NAK,
    LEAVE_SESS,
    NEW_SESS,
    NS_ACK,
    MESSAGE,
    QUERY,
    QU_ACK
};

typedef struct packet {
    unsigned int type; 
    unsigned int size;
    unsigned char source[MAX_NAME];
    unsigned char data[MAX_DATA];
} packet;

char* ptos(packet* pack_); // convert packet struct into string
packet* stop(char* packet_str); // convert string into packet struct


typedef struct user {
    unsigned char name[MAX_NAME];
    unsigned char password[MAX_DATA];
    int log_status;
    int session_status;
    char* session_id;
    int sockfd;         
    pthread_t p;      
} user;

#endif