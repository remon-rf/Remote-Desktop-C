#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>

#define BUFFER_SIZE 1024
int max_clients = 1;
typedef void(Sigfunc)(int);

int PORT = 8080;






int main()
{
    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char *response = (char*)malloc(64);


    fd_set read_fds;
    int *clients_list = malloc(max_clients*sizeof(int));
    memset(clients_list,0,max_clients*sizeof(int));
    int max_fd, client_fd, activity, i, new_socket;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        close(server_fd);

        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    while(1){ //loop to change port number if 8080 isnt available
        address.sin_port = htons(PORT);
        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("bind failed");
            PORT += 1;
            continue;
        }
        break;
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d\n", PORT);

    while(1)
    {




        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;
        for (i = 0; i < max_clients; i++)
        {
            client_fd = clients_list[i];
            if(client_fd > 0)
                FD_SET(client_fd, &read_fds);
            if(client_fd > max_fd)
                max_fd = client_fd;
        }
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity<=0)
            continue;


        if (FD_ISSET(server_fd, &read_fds))
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
            {
                perror("accept failed");
                continue;
            }

            printf("CONNECTED to a client");


            for (i = 0; i < max_clients; i++)
            {
                if (clients_list[i] == 0)
                {
                    clients_list[i] = new_socket;
                    printf(". adding it as index %d\n", i);
                    break;
                }

                if (i >= max_clients-1){ //extend array
                    int new_max_clients = 2*max_clients;
                    int* new_clients_list = malloc(new_max_clients*sizeof(int));
                    memset(new_clients_list,0,new_max_clients*sizeof(int));
                    memcpy(new_clients_list,clients_list,max_clients*sizeof(int));
                    free(clients_list);
                    clients_list = new_clients_list;
                    max_clients = new_max_clients;
                }
            }
        }


        for (i = 0; i < max_clients; i++)
        {
            client_fd = clients_list[i];

            if (FD_ISSET(client_fd, &read_fds))
            {

                memset(buffer, 0, BUFFER_SIZE);
                int read_size = read(client_fd, buffer, BUFFER_SIZE);

                if (read_size <= 0)
                {

                    printf("client disconnected!\n");
                    close(client_fd);
                    clients_list[i] = 0;
                }
                else
                {

                    printf("Received from client %d: %s\n",i, buffer);
                    if(buffer[0]>='0' && buffer[0]<='9'){
                        for(int j=0; j<buffer[0]-'0'; j++){
                            send(client_fd, buffer, strlen(buffer), 0);
                            usleep(1000);
                        }
                    }

                    send(client_fd, buffer, strlen(buffer), 0);
                    printf("echo sent to client\n");
                }
            }
        }
    }

    close(server_fd);
    free(response);
    return 0;
}
