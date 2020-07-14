#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#define MAXLINE 1024
  
/*Protocolo:
    - 00: Final
    - 01: Listado
    - 10: Solicitar fichero
    - 11: Subir fichero
*/

char *END_FLAG = "================END";

void recv_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path)
{
    int n, fd;
    socklen_t len;
    char buf[MAXLINE];
    char file_name[MAXLINE];
    len = clilen;
    n = recvfrom(sockfd, file_name, MAXLINE, 0, cliaddr, &len);
    file_name[n] = 0;
    sendto(sockfd, "ok", strlen("ok"), 0, cliaddr, len);
    strcat(work_path, "/");
    strcat(work_path, file_name);

    printf("Se ha recibido el fichero %s\n", file_name);

    fd = open(work_path, O_RDWR | O_CREAT, 0666);
    if(!fd){
        printf("Error al abrir el fichero\n");
    }
    while ((n = recvfrom(sockfd, buf, MAXLINE, 0, cliaddr, &len))) {
        buf[n] = 0;
        if (!(strcmp(buf, END_FLAG))) {
            break;
        }
        write(fd, buf, n);
    }
    close(fd);
}


         
int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    }

    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int port = atoi(argv[1]);
    char *directory = argv[2];
    char work_path[400];
    strcpy(work_path,cwd);
    printf("%s\n",work_path);
    strcat(work_path, "/");
    strcat(work_path, directory);
    printf("%s\n",work_path);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    int n;
    char action[MAXLINE];
    n = recvfrom(sockfd, action, MAXLINE, 0, &cliaddr, sizeof(cliaddr));
    printf("%d %d\n",n,atoi(action));

    recv_file(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr), work_path);

    return 0;
}