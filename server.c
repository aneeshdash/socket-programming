#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

int main( int argc, char *argv[] ) {
    FILE * f;
    char * line = NULL;
    size_t len = 0;
    ssize_t read1;
    char * ip;
    char * domain;
    char * req;
    int found;
    f= fopen ("database.txt","r");
    if (f == NULL)
        exit(EXIT_FAILURE);
    

   int sockfd, newsockfd, portno, clilen;
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int  n;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = 5001;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
      
   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    
   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }
   
   /* If connection is established then start communicating */
   bzero(buffer,256);
   n = read( newsockfd,buffer,255 );
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   
   found=0;
    if(buffer[0]=='1')
    {
        req= strtok (buffer," ");
        req = strtok (NULL, " \n");

        while ((read1 = getline(&line, &len, f)) != -1) {
              ip = strtok (line," ");
              domain = strtok (NULL, " \n");

              if(strcmp(req,domain)==0)
              {
                    found=1;
                    break;
              } 
        }
        if(found==1)
        {
            n = write(newsockfd,"3 ",2);
            n = write(newsockfd,ip,18);

        }   
        else
        {
           n = write(newsockfd,"4 entry not found in the database",35);
            
        }
    }
    else{

         req= strtok (buffer," ");
        req = strtok (NULL, " \n");
        while ((read1 = getline(&line, &len, f)) != -1) {
              ip = strtok (line," ");
              domain = strtok (NULL, " \n");
              if(strcmp(req,ip)==0)
              {
                    found=1;
                    break;
              } 
        }
        if(found==1)
        {
            n = write(newsockfd,"3 ",2);
            n = write(newsockfd,domain,18);
        }   
        else
        {
             n = write(newsockfd,"4 entry not found in the database",35);
        }
    }
   
   /* Write a response to the client */
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
      
   return 0;
}
