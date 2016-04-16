#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string.h>


int main( int argc, char *argv[] ) {
    char * line = NULL;
    size_t len = 0;
    ssize_t read1;
    char * ip;
    char * domain;
    char * req;
    int found;
    int fifo = 0;
    char cache[3][2][255];
    bzero(cache, 3*2*255);

    int client_sockfd, client_newsockfd, client_portno, clilen, server_sockfd, server_portno, n;
    char client_buffer[256], server_buffer[256], temp_buffer[256];
    struct sockaddr_in self_addr, cli_addr, serv_addr;
    struct hostent *server;

    if (argc < 4) {
       fprintf(stderr,"usage %s port server_hostname server_port\n", argv[0]);
       exit(0);
    }

   /* First call to socket() function */
   client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
   server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (client_sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   /* Initialize socket structure */
   bzero((char *) &self_addr, sizeof(self_addr));
   client_portno = atoi(argv[1]);
   server_portno = atoi(argv[3]);

   self_addr.sin_family = AF_INET;
   self_addr.sin_addr.s_addr = INADDR_ANY;
   self_addr.sin_port = htons(client_portno);

   /* Now bind the host address using bind() call.*/
   if (bind(client_sockfd, (struct sockaddr *) &self_addr, sizeof(self_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }

   server = gethostbyname(argv[2]);

   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(server_portno);

   /* Now connect to the server */
   if (connect(server_sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

   listen(client_sockfd,5);
   printf("Server listening on all interfaces on port %d\n", client_portno);
   clilen = sizeof(cli_addr);

   while(1)
   {

       /* Accept actual connection from the client */
       client_newsockfd = accept(client_sockfd, (struct sockaddr *)&cli_addr, &clilen);

       if (client_newsockfd < 0) {
          perror("ERROR on accept");
          exit(1);
       }

       /* If connection is established then start communicating */
       bzero(client_buffer,256);
       n = read( client_newsockfd,client_buffer,255 );

       if (n < 0) {
          perror("ERROR reading from socket");
          exit(1);
       }

       found=0;
        if(client_buffer[0]=='1')
        {
            strcpy(temp_buffer, client_buffer);
            req= strtok (temp_buffer," ");
            req = strtok (NULL, " \n");
            //check in cache else query to server
            for (size_t i = 0; i < 3; i++) {
                if(strcmp(req,cache[i][1])==0)
                {
                    ip = cache[i][0];
                    found=1;
                    break;
                }
            }
            if(found==1)
            {
                printf("Found in cache\n");
                n = write(client_newsockfd,"3 ",2);
                n = write(client_newsockfd,ip,18);

            }
            else
            {
                printf("Not found in cache\n");
                /* Send message to the server */
                n = write(server_sockfd, client_buffer, strlen(client_buffer));
                if (n < 0) {
                   perror("ERROR writing to socket");
                   exit(1);
                }

                /* Now read server response */
                bzero(server_buffer,256);
                printf("hell\n");
                n = read(server_sockfd, server_buffer, 255);
                printf("hello\n");
                if (n < 0) {
                   perror("ERROR reading from socket");
                   exit(1);
                }

                n = write(client_newsockfd, server_buffer, 255);

                if(server_buffer[0] == '3')
                {
                    strcpy(cache[fifo][1], req);
                    req= strtok (server_buffer," ");
                    req = strtok (NULL, " \n");
                    strcpy(cache[fifo][0], req);
                    fifo = (fifo + 1) % 3;
                }

            }
        }
        else if(client_buffer[0] == '2')
        {
            strcpy(temp_buffer, client_buffer);
            req= strtok (temp_buffer," ");
            req = strtok (NULL, " \n");
            //TODO check in buffer else query to server
            for (size_t i = 0; i < 3; i++) {
                if(strcmp(req,cache[i][0])==0)
                {
                    domain = cache[i][1];
                    found=1;
                    break;
                }
            }
            if(found==1)
            {
                printf("Found in cache\n");
                n = write(client_newsockfd,"3 ",2);
                n = write(client_newsockfd,domain,18);
            }
            else
            {
                printf("Not found in cache\n");
                /* Send message to the server */
                n = write(server_sockfd, client_buffer, strlen(client_buffer));

                if (n < 0) {
                   perror("ERROR writing to socket");
                   exit(1);
                }

                /* Now read server response */
                bzero(server_buffer,256);
                n = read(server_sockfd, server_buffer, strlen(server_buffer));

                if (n < 0) {
                   perror("ERROR reading from socket");
                   exit(1);
                }

                n = write(client_newsockfd, server_buffer, strlen(server_buffer));

                if(server_buffer[0] == '3')
                {
                    strcpy(cache[fifo][0], req);
                    req= strtok (server_buffer," ");
                    req = strtok (NULL, " \n");
                    strcpy(cache[fifo][1], req);
                    fifo = (fifo + 1) % 3;
                }
            }
        }

        else {
            n = write(client_newsockfd,"ERROR in request format",23);
        }

       /* Write a response to the client */

       if (n < 0) {
          perror("ERROR writing to socket");
          exit(1);
       }
   }

   return 0;
}
