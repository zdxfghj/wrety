#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>

#define SEMP "/my_semaphore"
int main(int argc,char* argv[])
{
	sem_t *sem;
	if((sem=sem_open(SEMP,0))==SEM_FAILED){
	perror("SEM_OPEN");
	return 0;
	}
	int readfd;
	char temp[]="/tmp/fifo.1";
	readfd=open(temp,O_RDONLY);
	
	printf("HELLO  %d\n",readfd);
	
	ssize_t n;
	
	printf("%s\n",temp);
	printf("Connected\n");
	char buf[256];
	while (strcmp(buf,"EXIT\n")!=0){
		
		read(readfd,buf,sizeof(buf));
		sem_post(sem);
		printf("%s",buf);
		
	}
	kill(getpid(),SIGINT);
	return 0;
}

