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


void help() // Command manual
{
	printf("\n\tHELP SECTION.\n\n\tCommand Name: tls (thread ls)\n");
	printf("\tSyntax: .</tls [-h] | [-n i] [-d directory] [ output ]>\n");
	printf("\tDescription: This program recursively traverses a directory tree, generating a report with the most important information for each directory in the tree.\n");
	printf("\tParameters:\n");
	printf("\t-h: Show help section\n");
	printf("\t-n <i>: Number of threads. By default only one thread\n");
	printf("\t-d <directory>: Specifies the directory where you want to run the program. By default, the program runs in the current directory\n");
	printf("\tOutput: Must be the last parameter. By default, the information will be printed on the standard output\n\n");
	
}
// end Command manual

// Functions indicating errors

void no_Number(int num)
{
	if(!num)												
	{
		fprintf(stderr, "An error occured, you must choice a NUMBER of threads\n");
		exit(-1);
	}

}

void wrong_syntax()
{
	fprintf(stderr, "Wrong syntax, please type ./tls -h to obtain help. \n");		
	exit(-1);

}

void permission_denied(int num)
{
	if (num == -1)
	{
		fprintf(stderr, "Permission denied. \n");		
		exit(-1);
	}


}

// end Functions indicating errors


// Function that the threads use to traverse the directories of the directory list not covered

void explore(char *path, List* not_Explored, int id, Report *R) 
{
	int size = 0; 		// Size in bytes 
  	int num = 0; 			
  	DIR *current;
  	char* aux; 									
  	struct dirent *dp;									
	struct stat statbuf;
	current = opendir(path);
	if (!current)										
	{
		printf("Error, Permission denied on: %s\n",path);
		return ;
	}
	else
	{
		while((dp = readdir(current)))	
		{ 
	    	if(strcmp(dp->d_name,".") && strcmp(dp->d_name,".."))	
			{
	        	aux = (char*) malloc(512*sizeof(char));  
	        	strcpy(aux, path);
	        	strcat(strcat(aux,"/"), dp->d_name); 		
	        	stat(aux, &statbuf);       
	        	if (statbuf.st_mode & S_IFDIR) 		
				{ 
	        		pthread_mutex_lock(&not_Explored->mutex_List);   
	        		not_Explored = insert(not_Explored, aux); 		// Critical region
	          		pthread_mutex_unlock(&not_Explored->mutex_List);  
	          		sem_post(&not_Explored->available); 			
	        	}
	        	else 		
	        	{
	        	  	size = size + statbuf.st_size;
	        	  	num++;  
	        	}
	       		free(aux); 
			}		
		}
	} 
	pthread_mutex_lock(&R->mutex_Report);
	R = insert_Rep(R, path, size, num, id); 					// Critical region
	pthread_mutex_unlock(&R->mutex_Report);
	closedir(current);
}

void* thread_Pool(void* Arg)  // Function that sends the created threads to browse the directories that are in the list
{
    char* aux;
    thread* worker = (thread*) Arg;
    while(1)
    { 
    	worker->L->alive[worker->id-1] = 0;
      	sem_wait(&worker->L->available); 	
      	worker->L->alive[worker->id-1] = 1;
      	aux = (char*) malloc(sizeof(char)*512);
      	pthread_mutex_lock(&worker->L->mutex_List);
      	strcpy(aux, worker->L->head->path);					    // Critical region	
      	worker->L = pop(worker->L); 
      	pthread_mutex_unlock(&worker->L->mutex_List);
      	explore(aux, worker->L, worker->id, worker->R);
      	free(aux);
    }
}

List* init_List() 							
{
	List *L;
  	L = (List *) malloc(sizeof(List));
  	L->head = NULL;
  	L->tail = NULL;
  	pthread_mutex_init(&L->mutex_List, NULL);
  	sem_init(&L->available, 0, 0);
  	return L;
}

int empty_List(List *L) 	
{
	if(!L->head)
    	return 1;
  	else
    	return 0;
}

List* insert(List* list, char* path) 
{
	Node *new_element;
	new_element = (Node*) malloc(sizeof(Node));
	new_element->path = (char*) malloc(512*sizeof(char));
	strcpy(new_element->path, path);
	new_element->next = NULL;
	if(empty_List(list)) 
		list->head = new_element;
	else 
		list->tail->next = new_element;
	list->tail = new_element;
	return list;
}

List *pop(List *L) 
{
	Node *aux;
  	if(!empty_List(L))
  	{
    	aux = L->head;
    	L->head=L->head->next;
    	free(aux->path);
    	free(aux);
  	}
  	return L;
}

Report* init_Report() 
{
	Report *R;
  	R = (Report *) malloc(sizeof(Report));
  	R->head = NULL;
  	R->tail = NULL;
  	pthread_mutex_init(&R->mutex_Report, NULL);
  	return R;
}

int empty_Report(Report *R) 	
{
  	if(!R->head)
    	return 1;
    else
   		return 0;
}

Report* insert_Rep(Report * list, char* path, int size, int numFiles, int id) 
{
	Node_R *new_element;
	new_element = (Node_R*) malloc(sizeof(Node_R));
	new_element->path = (char*) malloc(512*sizeof(char));
	strcpy(new_element->path, path);
	new_element->size = size;
	new_element->numFiles = numFiles;
	new_element->id = id;
	new_element->next = NULL;
	if(empty_Report(list)) 
		list->head = new_element;
	else
	    list->tail->next = new_element;
	list->tail = new_element;
	return list;
}

Report *pop_Rep(Report *L) 
{
	Node_R *aux;
  	if(!empty_Report(L))
  	{
    	aux = L->head;
    	L->head = L->head->next;
    	free(aux->path);
    	free(aux);
  	}
  	return L;
}

void generate_Report(Report* L, char* output) 
{ 
	FILE* fp;
	Report *aux;

	if(!(fp = fopen(output,"w")))
	{
		printf("Error, Permission denied. Non generated report\n");
	    return ;
	}
	aux = L;
	while(aux->head->next)
	{
		fprintf(fp, "%s %d %d %d\n",aux->head->path, aux->head->size, aux->head->numFiles, aux->head->id);
		aux = pop_Rep(aux);
	  
	}
	fprintf(fp, "%s %d %d %d\n",aux->head->path, aux->head->size, aux->head->numFiles, aux->head->id); 
	aux = pop_Rep(aux);
	fclose(fp);
}

void print_Report(Report* L) 
{ 
	Report* aux;
	aux = L;
	while(aux->head->next)
	{
		printf("%s %d %d %d\n",aux->head->path, aux->head->size, aux->head->numFiles, aux->head->id); 		
		aux = pop_Rep(aux);		
	}
	printf("%s %d %d %d\n",aux->head->path, aux->head->size, aux->head->numFiles, aux->head->id); 		
	aux = pop_Rep(aux);
}
