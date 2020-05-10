#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>

#define SEMP "/tmp/semaphore"
#define CHANNEL "/tmp/chanel"

pthread_mutex_t mutex;
int counter = 0;

void *doSmth(void *arg)
{
	int loc_id = *(int *) arg;
	while(1)
	{
		pthread_mutex_lock(&mutex);	
		printf ("Thread: %i ", loc_id);
		printf("Count: %d\n", counter++);
		sleep(0.5);
		pthread_mutex_unlock(&mutex);
	}
}

int main()
{
	int channelID;
	if((channelID = open(CHANNEL,O_RDONLY)) == -1)
	{
		perror("Open Read File:");
		return 0;
	}
	printf("CONNECTED\n");

	int count = 0;
	char buf[256];
	pthread_t  handles[1000];
	pthread_mutex_init(&mutex, NULL);

	while (1)
	{
		read(channelID, buf, sizeof(buf));

		char temp = buf[0];
		switch (temp)
		{
			case('+'):
			{
				  pthread_t thread;
				  pthread_create(&thread, NULL, doSmth, &count);
				  handles[count] = thread;
				  count++;
				  break;
			}
			case('-'):
			{
				  if(count == 0)
				  {
					  printf("No secondary threads create\n");
					  break;
				  }

				  pthread_cancel(handles[count-1]);
				  count--;  
				  break;
			}
			case('Q'):
			{
				while (count != 0)
				{
					pthread_cancel(handles[--count]);
				}

				pthread_mutex_destroy(&mutex);
				return 0;
			}
		}
	}
}
