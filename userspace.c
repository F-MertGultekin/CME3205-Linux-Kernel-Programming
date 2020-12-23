#include<stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
int main(int argc,char* argv[]) 
{ 
    
    	char *all="-all";
	char *p="-p";
	char *k="-k";
	char *noarg="noarg";

	//if there is no argument 
	if(argc==1)
	{ 	
	    char *pid="0";
	    char *dest=NULL;
            dest=(char*)malloc(2048);
			// dest is null , but then it returns a information from kernel
    	    int status = syscall(335,noarg,pid,dest);
	    printf("System call sys_processinfo returned %d\n",status);
	    printf("%s\n",dest);
	}
	//if there is one argument 
	if(argc==2) 
    	{ 	
		//if this argument is -all
		if(strcmp(argv[1],all)==0)
		{
			
			char *pid="0";
			char *dest=NULL;
			dest=(char*)malloc(2048);
			int status = syscall(335,all,pid,dest);
			// dest is null , but then it returns a information from kernel
			printf("System call sys_processinfo returned %d\n",status);
			printf("%s\n",dest);
		}

	}
	//if there is two arguments
	if(argc==3)
        {
		//if this argument is -p
		if(strcmp(argv[1],p)==0)
		{
			// dest is null , but then it returns a information from kernel
			char* argpid=argv[2];
			char *pid = argpid;
			char *dest=NULL;
			dest=(char*)malloc(2048);
			int status = syscall(335,p,pid,dest);
			printf("System call sys_processinfo returned %d\n",
status);
 			printf("%s\n",dest);
		}	
		//if this argument is -k
		if(strcmp(argv[1],k)==0)
		{
			char argument='k';
			char* argpid=argv[2];
			char *pid = argpid;
			
			char *dest=NULL;
			dest=(char*)malloc(2048);
			int status = syscall(335,k,pid,dest);
			printf("System call sys_processinfo returned %d\n",
status);
			printf("%s\n",dest);
		}
	}     
     
	return 0; 
} 


