#include "servidor.h"

char *FINISH_FLAG = "================END";

void recv_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path)
{
    int n, fd;
    socklen_t len;
    char buf[MAXLINE];
    char file_name[MAXLINE];
    len = clilen;
    n = recvfrom(sockfd, file_name, MAXLINE, 0, cliaddr, &len);
    file_name[n] = 0;
    char full_path[4096];
    strcpy(full_path,work_path);
    strcat(full_path, "/");
    strcat(full_path, file_name);
    fd = open(full_path, O_RDWR | O_CREAT, 0666);
    if(!fd){
        printf("Error al abrir el fichero\n");
    }
    while ((n = recvfrom(sockfd, buf, MAXLINE, 0, cliaddr, &len))) {
        buf[n] = 0;
        if (!(strcmp(buf, FINISH_FLAG))) {
            printf("El fichero se recibe en %s\n", full_path);
            break;
        }
        write(fd, buf, n);
    }
    close(fd);
}

void get_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path){
    int n, fd;
    char buf[MAXLINE];
    char file_name[MAXLINE];
    char full_path[4096];
    n = recvfrom(sockfd, file_name, MAXLINE, 0, cliaddr, &clilen);
    strcpy(full_path,work_path);
    strcat(full_path, "/");
    strcat(full_path, file_name);
    fd = open(full_path, O_RDONLY);
    int j ;
    while ((n = read(fd, buf, MAXLINE)) > 0) {
        j = sendto(sockfd, buf, n, 0, cliaddr,clilen);
    }
    close(fd);
    sendto(sockfd, FINISH_FLAG, strlen(FINISH_FLAG), 0, cliaddr,clilen);
    printf("Se ha enviado el nombre del fichero %s \n", file_name);

}

void list_files(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path){
    int link[2];
    char info[4096];
    char aux[4096];
    strcpy(aux,"ls -laShp " );
    strcat(aux, work_path);
    strcat(aux, " | grep -v / | awk '{print $9, $5}'");
    pid_t pid;
    int n;
    n = recvfrom(sockfd, info, MAXLINE, 0, cliaddr, &clilen);

    if (pipe(link)==-1){
        die("pipe");
    }
    if ((pid = fork()) == -1){
        die("fork");
    }
    if(!pid){
        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        char * const ptr[]={"/bin/sh","-c",aux ,NULL}; 
        execv("/bin/sh",ptr); 
    }else{
        close(link[1]);
        int nbytes = read(link[0], info, sizeof(info));
        waitpid(pid, NULL,NULL);
        n = sendto(sockfd, info, sizeof(info), 0,cliaddr, clilen);
    }
}

         
int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("ERROR\n");
        exit(0);
    }
    char work_path[400];
    int n;
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    int port = atoi(argv[1]);
    char *directory = argv[2];

    strcpy(work_path,cwd);
    strcat(work_path, "/");
    strcat(work_path, directory);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    char action[MAXLINE];
    n = recvfrom(sockfd, action, MAXLINE, 0, &cliaddr, sizeof(cliaddr));
    int n_action = atoi(action);
    while(n_action != END ){
        if(n_action == SEND){
            recv_file(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr), work_path);
        }else if(n_action == GET){
            get_file(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr), work_path);

        }else if(n_action == LIST){
            list_files(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr), work_path);

        }
        n = recvfrom(sockfd, action, MAXLINE, 0, &cliaddr, sizeof(cliaddr));
        n_action = atoi(action);

    }
    return 0;
}