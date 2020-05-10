#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#define CHANNEL "/tmp/chanel"
#define SEMP "/tmp/semaphore"

int main()
{	pid_t pid;

	if((pid = fork()) == 0){					
	   execl("/usr/bin/gnome-terminal","gnome-terminal","--","./childProc",NULL);
	}
	printf("Im working...!!!\n");
	
	unlink(CHANNEL);
	
	
	if(mkfifo(CHANNEL, 0777)==-1)
	{
		perror("mkfifo");
	   	return 0;
	}

	int channelID;	
	if((channelID = open(CHANNEL, O_WRONLY)) == -1)
	{
		printf("Error");
		return 0;
	}

	printf("Child process is connected\n");

	printf("+ new thread\n");
	printf("- terminate thread\n");
	printf("Q close child process\n");
	printf("EXIT close main process\n");

	char str[256];
	while(strcmp(str,"EXIT\n") != 0){
		fgets(str, 256, stdin);
		write(channelID, str, strlen(str)+1);
	}
	wait (NULL);
return 0;
}

