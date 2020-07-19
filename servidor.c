#include "servidor.h"

char *FINISH_FLAG = "================END";

void recv_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path)
{
    int n, fd;
    socklen_t len;
    char buf[MAXLINE];
    char *file_name =  malloc(1024 * sizeof(char));
    char *full_path = malloc(1024 * sizeof(char));
    len = clilen;
    n = recvfrom(sockfd, file_name, MAXLINE, 0, cliaddr, &len);
    strcpy(full_path,work_path);
    strcat(full_path, "/");
    strcat(full_path, file_name);
    fd = open(full_path, O_RDWR | O_CREAT, 0666);
    if(!fd){
        printf("Error al abrir el fichero\n");
    }
    int count = 0;
    while ((n = recvfrom(sockfd, buf, MAXLINE, 0, cliaddr, &len))) {
        char ack[13];
        buf[n] = 0;
        if (!(strcmp(buf, FINISH_FLAG))) {
            printf("Se ha recibido el fichero %s\n", file_name);
            break;
        }
        write(fd, buf, n);
        sprintf(ack, "%d", count);                
        n = sendto(sockfd, ack, strlen(ack), 0, cliaddr, len);
        count++;
    }
    count = 0;
    close(fd);
}

void get_file(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path){
    int n, fd;
    int j;
    int count = 0;
    char buf[MAXLINE];
    char *file_name =  malloc(1024 * sizeof(char));
    char *full_path =  malloc(1024 * sizeof(char));
    n = recvfrom(sockfd, file_name, MAXLINE, 0, cliaddr, &clilen);
    strcpy(full_path, work_path);
    strcat(full_path, "/");
    strcat(full_path, file_name);
    fd = open(full_path, O_RDONLY);
    while ((n = read(fd, buf, MAXLINE)) > 0) {
        char *ack = (char*) malloc(MAXLINE * sizeof(char));
        j = sendto(sockfd, buf, n, 0, cliaddr,clilen);
        n = recvfrom(sockfd, ack, MAXLINE, 0, cliaddr, &clilen);
        if(atoi(ack) != count){
            printf("%d %d\n", count, atoi(ack));
            printf("Error enviando paquete  %d %d %d\n",n,j,count);
            break;
        }
        count++;
    }
    close(fd);
    sendto(sockfd, FINISH_FLAG, strlen(FINISH_FLAG), 0, cliaddr,clilen);
    printf("Se ha enviado el fichero %s \n", file_name);

}

void list_files(int sockfd, struct sockaddr *cliaddr, socklen_t clilen, char* work_path){
    int link[2];
    char info[1024];
    char comm[1024];
    char aux[10];
    pid_t pid;
    int n;
    int status;
    strcpy(comm,"ls -laShp " );
    strcat(comm, work_path);
    strcat(comm, " | grep -v / | grep -v total | awk '{print $9, $5}'");
    n = recvfrom(sockfd, aux, MAXLINE, 0, cliaddr, &clilen);

    if (pipe(link) == -1){
        die("pipe");
    }
    if ((pid =fork()) == -1){
        die("fork");
    }
    if(!pid){
        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        char * const ptr[]={"/bin/sh","-c",comm ,NULL}; 
        execv("/bin/sh",ptr); 
    }else{
        close(link[1]);
        int nbytes = read(link[0], info, sizeof(info));
        waitpid(pid, &status,0);
        n = sendto(sockfd, info, sizeof(info), 0,cliaddr, clilen);
    }
    printf("Se listan los ficheros del servidor\n");
}

         
int main(int argc, char **argv){
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
    socklen_t len = sizeof(servaddr);
    bzero(&servaddr, len);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    bind(sockfd, (struct sockaddr *) &servaddr, len);
    char action[MAXLINE];
    n = recvfrom(sockfd, action, MAXLINE, 0, (struct sockaddr *)&cliaddr, (socklen_t *)sizeof(cliaddr));
    int n_action = atoi(action);
    while(n_action != END ){
        if(n_action == SEND){
            recv_file(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr), work_path);
        }else if(n_action == GET){
            get_file(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr), work_path);
        }else if(n_action == LIST){
            list_files(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr), work_path);
        }
        n = recvfrom(sockfd, action, MAXLINE, 0, (struct sockaddr *)&cliaddr, (socklen_t *)sizeof(cliaddr));
        n_action = atoi(action);
    }
    return 0;
}