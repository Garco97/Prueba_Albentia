#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include "common.h"

void send_file(int sockfd, struct sockaddr_in servaddr, char *path);
void get_file(int sockfd, struct sockaddr_in servaddr, char *path);
void list_files(int sockfd, struct sockaddr_in servaddr, char *path);
