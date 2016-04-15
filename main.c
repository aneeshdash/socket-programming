#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct m{
	char type[10];
	char text[50];
} message;

message server(message m){
	message * res = (message *) malloc(sizeof(message));
	message response= *res;
	FILE * f;
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * ip;
    char * domain;
    int found=0;
	f= fopen ("database.txt","r");
	if (f == NULL)
        exit(EXIT_FAILURE);
    

    if(m.type[0]=='1')
    {
    	
    	while ((read = getline(&line, &len, f)) != -1) {
   		 	  ip = strtok (line," ");
   		 	  domain = strtok (NULL, " \n");

   		 	  if(strcmp(m.text,domain)==0)
   		 	  {
   		 	  		found=1;
   		 	  		break;
   		 	  }	
    	}
    	if(found==1)
    	{
    		
    		strcpy(response.type,"3");
    		strcpy(response.text,ip);
    	}	
    	else
    	{
    		strcpy(response.type,"4");
    		strcpy(response.text,"entry not found in the database");
    	}
	}
	else{
		while ((read = getline(&line, &len, f)) != -1) {
   		 	  ip = strtok (line," ");
   		 	  domain = strtok (NULL, " \n");
   		 	  if(strcmp(m.text,ip)==0)
   		 	  {
   		 	  		found=1;
   		 	  		break;
   		 	  }	
    	}
    	if(found==1)
    	{
    		strcpy(response.type,"3");
    		strcpy(response.text,domain);
    	}	
    	else
    	{
    		strcpy(response.type,"4");
    		strcpy(response.text,"entry not found in the database")	;
    	}
	}
	return response;
}

message proxy(message m);

void client(){
	message * r1;
	message * r2;
	r1= (message *) malloc(sizeof(message));
	r2= (message *) malloc(sizeof(message));
	char flag[10]="continue";
	while(flag[0]='c')
	{
		printf("Enter the type of request\n");
		scanf("%s",r1->type);
		printf("Enter the request\n");
		scanf("%s",r1->text);
		//r2=proxy(r1);
		*r2=server(*r1);
		printf("The Response received is\n");
		printf("%s %s\n",r2->type,r2->text);
		printf("Enter continue to continue, any other string to exit\n");
		scanf("%s",flag);
	}

}
int main(int argc,  char* argv[])
{
	client();
	return 0;
}