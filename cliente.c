#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>


/*Protocolo:
    - 00: Final
    - 01: Listado
    - 10: Solicitar fichero
    - 11: Subir fichero
*/
#define SERV_PORT 49312
#define MAXLINE 1024

char *END_FLAG = "================END";

int main(int argc, char **argv)
{
    int sockfd, n, fd;
    struct sockaddr_in servaddr;
    char buf[MAXLINE];
    char *target, *path;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    path = argv[3];
    target = argv[4];
    sendto(sockfd, "0000", strlen("0000"), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    sendto(sockfd, target, strlen(target), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    n = recvfrom(sockfd, buf, MAXLINE, 0, NULL, NULL);
    if (!strncmp(buf, "ok", 2)) {
        printf("Filename sent.\n");
    }
    
    fd = open(path, O_RDONLY);
    while ((n = read(fd, buf, MAXLINE)) > 0) {
        sendto(sockfd, buf, n, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    }
    sendto(sockfd, END_FLAG, strlen(END_FLAG), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    
    return 0;
}