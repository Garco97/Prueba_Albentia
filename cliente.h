#include "common.h"

void send_file(int sockfd, struct sockaddr_in servaddr, char *path);
void get_file(int sockfd, struct sockaddr_in servaddr, char *path);
void list_files(int sockfd, struct sockaddr_in servaddr, char *path);
