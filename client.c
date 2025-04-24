#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
int PORT = 8080;
#define BUFFER_SIZE 1024
int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    char *message = (char*)malloc(64);
    fd_set read_fds;


    while(1){
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
// Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
// Convert the server IP address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address / Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
// Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection Failed, different port?:");
        PORT+=1;
        close(sockfd);
        if (PORT>8085)
            exit(0);
        continue;

    }
    break;
    }
    printf("type messages:\n");
    while (1)
    {

        int activity = 0;

        while(activity<=0)
        {
            FD_ZERO(&read_fds);
            FD_SET(STDIN_FILENO,&read_fds);
            FD_SET(sockfd,&read_fds);

            activity = select(sockfd+1,&read_fds,NULL,NULL,NULL);
        }
        if (FD_ISSET(STDIN_FILENO,&read_fds)) /// process stdin content
        {
            scanf("%s",message);
            if (strcmp(message,"quit")==0)
            {
                printf("quitting, goodbye!\n");
                break;
            }
            send(sockfd, message, strlen(message), 0);
            //printf("Message sent to server: %s\n", message);
        }
        if (FD_ISSET(sockfd,&read_fds)) /// process sockfd content
        {
            memset(buffer,0,BUFFER_SIZE);
            int bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
            if (bytes_read==0){
                perror("server crashed!\n");
                getchar();
                break;
            }
            else if (bytes_read < 0)
            {
                perror("read failed");
                break;
            }
            else
            {
                //buffer[bytes_read] = '\0';  // Null-terminate the buffer to make it a valid string
                printf("Received from server: %s\n", buffer);
            }
        }




    }
    close(sockfd);
    return 0;
}
