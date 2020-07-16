#include "cliente.h"

char *list_flag = "100";
char *get_flag = "101"; 
char *send_flag = "110"; 
char *end_flag = "111";

char *FINISH_FLAG = "================END";

void send_file(int sockfd, struct sockaddr_in servaddr, char *path){
    int n, fd;
    char buf[MAXLINE];
    sendto(sockfd, send_flag, strlen(send_flag), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("Contenido del directorio: \n"); 
    pid_t pid = fork();
    if(!pid){
        char * const ptr[]={"/bin/sh","-c","ls -laShp | grep -v / | awk '{print $9, $5}'" ,NULL}; 
        execv("/bin/sh",ptr); 
    }
    wait(NULL);
    printf("Selecciona fichero a enviar: ");
    scanf("%s",path);
    printf("Se envia el path del fichero\n");
    sendto(sockfd, path, strlen(path), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("Se ha enviado el path\n");
    fd = open(path, O_RDONLY);
    while ((n = read(fd, buf, MAXLINE)) > 0) {
        sendto(sockfd, buf, n, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    }
    sendto(sockfd, FINISH_FLAG, strlen(FINISH_FLAG), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
}
void get_file(int sockfd, struct sockaddr_in servaddr, char *path){
    sendto(sockfd, get_flag, strlen(send_flag), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

}
void list_files(int sockfd, struct sockaddr_in servaddr, char *path){
        sendto(sockfd, list_flag, strlen(list_flag), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

}

int main(int argc, char **argv)
{
    printf("%s %s %s\n", argv[1],argv[2],argv[3]);
    int sockfd;
    struct sockaddr_in servaddr;
    char path[400];
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    char input[10] = "";
    printf("Escoge la acción que quieres realizar: ");  
    scanf("%s",input);
    while(strcmp(input, "exit") != 0){
        if(strcmp(input, "send") == 0){
            send_file(sockfd, servaddr, path);
        }else if(strcmp(input, "get") == 0){
            get_file(sockfd, servaddr, path);
        }else if(strcmp(input, "list") == 0){
            list_files(sockfd, servaddr, path);
        }
        printf("Escoge la acción que quieres realizar: ");  
        scanf("%s",input);
    }
    sendto(sockfd, end_flag, strlen(end_flag), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    return 0;
}