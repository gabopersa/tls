// Simon Bolivar University - Venezuela
// CI-3825 Operation Systems I
// Student: Gabriel Perez Salgado Id: 13-11104
// Last Update: 14-09-2017

#define MAX_T 1000
#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

typedef struct List_Element 						        //	Elements of the list
{   char *path;
  	int numFiles;
  	int size;
  	struct List_Element *next;
}Node;

typedef struct 										               // Structure representing the list of non-routed directories
{
	  int alive[MAX_T];
  	pthread_mutex_t mutex_List;
  	sem_t available;
  	Node *head;
  	Node *tail;
}List;

typedef struct Report_Element  					          // Elements of the report
{
	char *path;
	int numFiles;
  int size;
  int id;
  struct Report_Element *next;
}Node_R;

typedef struct 										                // Structure that represents the list of information that will be printed on screen or saved in a text document
{
  Node_R *head;
	Node_R *tail;
  pthread_mutex_t mutex_Report;
}Report;

typedef struct 										                // Structure that represents a "thread"
{
	List *L;
 	Report *R;
	int id;
}thread;

List* init_List();
void explore(char *path, List* not_Explored, int id, Report *R);
int empty_List(List *L);
List* insert(List * list, char* path);
List *pop(List *L);
void* thread_Pool(void* Arg);
void help();
int empty_Report(Report *L);
void generate_Report(Report* L, char* output);
Report* insert_Rep(Report *R, char* path, int size, int numFiles, int id);
Report* init_Report();
void print_Report(Report * L);
Report *pop_Rep(Report *L);
void no_Number(int num);
void wrong_syntax();
void permission_denied(int num);


#endif
