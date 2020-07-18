
#include "common.h"

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

void recv_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path);

void get_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path);

void list_files(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path);

