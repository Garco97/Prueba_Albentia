#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include "common.h"

void recv_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path);

void get_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path);

void list_files(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path);