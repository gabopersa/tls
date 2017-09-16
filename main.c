// Simon Bolivar University - Venezuela
// CI-3825 Operation Systems I
// Student: Gabriel Perez Salgado Id: 13-11104
// Last Update: 14-09-2017

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#include "Header.h"

int main(int argc,char* argv[])
{
	int j, i = 1;
	int ret=-1;
	int verify; 									
	int rep = 0;   
	char* path=(char*) malloc(sizeof(char)*512); 	
	char* output;								 		
	char* aux = (char*) malloc(sizeof(char)*512);
	void* status;
	thread *Arg[MAX_T]; 										
	pthread_t Worker[MAX_T]; 						
	List *L; 										
	Report *R; 									
	L = init_List();
	R = init_Report();
	getcwd(aux,512);
	switch(argc) 									
	{
		case 1: break; 													// Case ./tls 
		
		case 2: 									
				if((!strcmp(argv[1],"-h"))){							// Case ./tls -h 
					help();
					free(path);
					free(aux);
					free(L);
					free(R);
					return 0;
				}
				else 													// Case ./tls output
				{
					output = argv[1];
					rep = 1;
				}
				break;
		
		case 3:
				if(!strcmp(argv[1],"-n")) 								// Case ./tls -n i 
				{
					i = atol(argv[2]);
					no_Number(i);
				}
				else if(!strcmp(argv[1],"-d")) 							// Case ./tls -d directory
				{
					ret = chdir(argv[2]);
					permission_denied(ret);

				}
				else 							
					wrong_syntax();
				break;

		case 4: 
				if(!strcmp(argv[1],"-n")) 								// Case ./tls -n i Salida
				{	
					i = atol(argv[2]);
					no_Number(i);
					rep = 1;	
					output = argv[3];
				}
				else if(!strcmp(argv[1],"-d")) 							// Case ./tls -d Directorio Salida
				{
					ret = chdir(argv[2]);
					permission_denied(ret);
					rep = 1;
					output = argv[3];
				}
				else 		
					wrong_syntax();
				break;

		case 5: 
				if(!strcmp(argv[1],"-n")) 			
				{
					i = atol(argv[2]);
					no_Number(i);
					if(strcmp(argv[3],"-d"))	
						wrong_syntax();
					else 												// Case ./tls -n i -d Directory
					{
						ret = chdir(argv[4]);
						permission_denied(ret);
					}	
				}
				else if(!strcmp(argv[1],"-d")) 		
				{
					ret = chdir(argv[2]);
					permission_denied(ret);
					if(strcmp(argv[3],"-n"))
						wrong_syntax();
					else 												// Case ./tls -d Directory -n i
					{
						i = atol(argv[4]);
						no_Number(i);

					}
				}
				else 			
					wrong_syntax();
				break;

		case 6:
				if(!strcmp(argv[1],"-n"))  
				{
					i = atol(argv[2]);
					no_Number(i);
					if(strcmp(argv[3],"-d"))
						wrong_syntax();
					else 												// Case ./tls -n i -d Directory output
					{
						ret = chdir(argv[4]);
						permission_denied(ret);
						rep = 1;
						output = argv[5];
					}
				}
				else if(!strcmp(argv[1],"-d"))
				{
					ret = chdir(argv[2]);
					permission_denied(ret);
					if(strcmp(argv[3],"-n"))
						wrong_syntax();
					else 												// Case ./tls -d Directory -n i output
					{
						i = atol(argv[4]);
						no_Number(i);
						rep = 1;
						output = argv[5];
					}
				}
				else 	
					wrong_syntax();
				break;

		default: 
					wrong_syntax();		
	}
	getcwd(path, 512); 				
	explore(path, L, 0, R); 		
	for(j = 0; j < i; j++) 			
	{
		Arg[j] = (thread*) malloc(sizeof(thread));
		Arg[j]->L = L;
		Arg[j]->L->alive[j] = 1;
		Arg[j]->R = R;
		Arg[j]->id = j+1;
		pthread_create(&Worker[j], NULL, thread_Pool, (void*) Arg[j]);
	}
	while(1) 			
	{
		for(j = verify = 0; j < i; j++)
		{
			if(L->alive[j])	
				verify = 1;
		}
		if(!verify && empty_List(L)) 
			break;
	}
	for(j = 0; j < i; j++)
	{
		free(Arg[j]);
		pthread_cancel(Worker[j]);
		pthread_join(Worker[j], &status);
	}
	chdir(aux);	
	if(rep)
	{	
		printf("Report generated named %s\n", output);		
		generate_Report(R, output);
	}
	else
		print_Report(R);
	pthread_mutex_destroy(&L->mutex_List);
	pthread_mutex_destroy(&R->mutex_Report);
	free(path);	
	free(L);
	free(R);
	free(aux);
	return 0;
}