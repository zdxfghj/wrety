#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define FILE_MODE (S_IRWXO | S_IRWXG | S_IRWXU)
#define FIFO "/tmp/fifo.1"
#define SEMP "/my_semaphore"
int main()
{
	printf("HELLO WORLD!!\n");
	sem_t *sem;
	if( (sem=sem_open(SEMP,O_CREAT,0777,0)) == SEM_FAILED){
		perror("sem_open");
		return 0; 
	}
	unlink(FIFO);
	pid_t pid;
	int writefd;
	char str[256];
	
	char rv[10];
	
	int idpid=getpid();
	sprintf(rv,"%d",idpid);
	
	if(mkfifo(FIFO,0777)==-1)
	{
		perror("mkfifo");
	   	return 0;
	}
	
	
	if((pid=fork())==0){					
	   execl("/usr/bin/gnome-terminal","gnome-terminal","--","./pa",NULL);
	}else{	
	
		if((writefd=open(FIFO,O_WRONLY))==-1)
		{
			printf("Error");
			return 0;
		}
	}
	printf("Connected\n");
	while(strcmp(str,"EXIT\n")!=0){
	    fgets(str,256,stdin);
	    write(writefd,str,strlen(str)+1);
	    if(sem_wait(sem)==0){
	    	printf("\t\t\tIt's OK!\n");
	    }
	}
	sleep(2);
return 0;
}

