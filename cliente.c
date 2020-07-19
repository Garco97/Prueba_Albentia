#include "cliente.h"

char *list_flag = "100";
char *get_flag = "101"; 
char *send_flag = "110"; 
char *end_flag = "111";

char *FINISH_FLAG = "================END";

void send_file(int sockfd, struct sockaddr_in servaddr, char *path){
    int n, fd;
    char buf[MAXLINE];
    char file_name[4096];
    char full_path[4096];
    n = sendto(sockfd, send_flag, strlen(send_flag), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("Contenido del directorio: \n"); 
    pid_t pid = fork();
    if(!pid){
        char * const ptr[]={"/bin/sh","-c","ls -laShp | grep -v / | awk '{print $9, $5}'" ,NULL}; 
        execv("/bin/sh",ptr); 
    }
    wait(NULL);
    printf("Selecciona fichero a enviar: ");
    scanf("%s",file_name);
    strcpy(full_path,path);
    strcat(full_path, "/");
    strcat(full_path, file_name);
    printf("\nEnviando el fichero %s\n", file_name);
    sendto(sockfd, file_name, strlen(file_name), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    fd = open(file_name, O_RDONLY);
    int j;
    int count = 0;
    while ((n = read(fd, buf, MAXLINE)) > 0) {
        char *ack = (char*) malloc(MAXLINE * sizeof(char));
        j = sendto(sockfd, buf, n, 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
        n = recvfrom(sockfd, ack, MAXLINE, 0, NULL, NULL);
        if(atoi(ack) != count){
            printf("%d %d %s\n", count, atoi(ack), ack);
            printf("Error enviando paquete  %d %d %d\n",n,j,count);
            break;
        }
        count ++;
    }
    count = 0;
    close(fd);
    sendto(sockfd, FINISH_FLAG, strlen(FINISH_FLAG), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    printf("\nEl fichero %s ha sido enviado\n", file_name);

}

void get_file(int sockfd, struct sockaddr_in servaddr, char *path){
    int n, fd;
    socklen_t len = sizeof(servaddr);
    char buf[MAXLINE];
    char file_name[MAXLINE];
    char full_path[4096];
    n = sendto(sockfd, get_flag, strlen(get_flag), 0, (struct sockaddr *) &servaddr, len);
    printf("Elige fichero que descargar: ");
    strcpy(full_path, path);
    scanf("%s",file_name);
    strcat(full_path,file_name);
    sendto(sockfd, file_name, strlen(file_name), 0, (struct sockaddr *) &servaddr, len);
    fd = open(full_path, O_RDWR | O_CREAT, 0666);
    printf("Solicitado el fichero %s\n", file_name);
    int count = 0;
    while (n = recvfrom(sockfd, buf, MAXLINE, 0, NULL,NULL)) {
        char ack[13];
        buf[n] = 0;
        if (!(strcmp(buf, FINISH_FLAG))) {
            printf("Se ha recibido el fichero %s\n\n", full_path);
            break;
        }
        write(fd, buf, n);
        sprintf(ack, "%d", count);        
        n = sendto(sockfd, ack, strlen(ack), 0, (struct sockaddr *) &servaddr, len);
        count++;
    }
    close(fd);
}

void list_files(int sockfd, struct sockaddr_in servaddr, char *path){
    int n;
    char buf[MAXLINE];
    socklen_t len = sizeof(servaddr);
    n = sendto(sockfd, list_flag, strlen(list_flag), 0, (struct sockaddr *) &servaddr, len);
    n = sendto(sockfd, "check", strlen("check"), 0, (struct sockaddr *) &servaddr, len);
    n = recvfrom(sockfd, buf, MAXLINE, 0, NULL, NULL);
    printf("\n%s\n", buf);
}

void show_help(){
    printf("\n'help' para ver ayuda\n");
    printf("'send' para enviar ficheros al servidor\n");
    printf("'get' para descargar ficheros del servidor\n");
    printf("'list' para ver los ficheros disponibles en el servidor\n");
    printf("'exit' para cerrar el cliente \n\n");
}

int main(int argc, char **argv)
{
    char cwd[PATH_MAX];
    char input[10] = "";
    int sockfd;
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("ERROR\n");
        exit(0);
    }
    strcat(cwd, "/");
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    while(strcmp(input, "exit") != 0){
        printf("\nEscoge la acción que quieres realizar ('help' para ver los comandos): \n");  
        scanf("%s",input);
        if(strcmp(input, "send") == 0){
            send_file(sockfd, servaddr, cwd);
        }else if(strcmp(input, "get") == 0){
            list_files(sockfd, servaddr, cwd);
            get_file(sockfd, servaddr, cwd);
        }else if(strcmp(input, "list") == 0){
            list_files(sockfd, servaddr, cwd);
        }else if(strcmp(input, "help") == 0){
            show_help();
        }
    }
    // Permite cerrar el servidor desde el cliente
    //sendto(sockfd, end_flag, strlen(end_flag), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    return 0;
}