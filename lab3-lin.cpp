#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_BUFFER_SIZE 255
#define SHARED_MEMORY_NAME "client-server"

using namespace std;

int create_shared_memory(char *name) {
	int fd = shm_open(name, O_RDWR | O_CREAT, 0777);
	ftruncate(fd, MAX_BUFFER_SIZE);
	cout << "Shared memory descriptor: " << fd << endl;
	return fd;
}

int open_shared_memory(char *name) {
	int fd = shm_open(name, O_RDWR, 0644);
	ftruncate(fd, MAX_BUFFER_SIZE);
	cout << "Shared memory descriptor: " << fd << endl;
	return fd;
}

void *get_shared_memory(int descriptor) {
	return mmap(nullptr, MAX_BUFFER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, descriptor, 0);
}

void close_shared_memory(int descriptor) {
	close(descriptor);
}

void child_process() {
	cout << "Starting..." << endl;
	sleep(1);
	int fd = open_shared_memory(SHARED_MEMORY_NAME);

	sem_t *ready, *posted;
	if ((ready = sem_open("readysem", O_CREAT, 0, 1)) == SEM_FAILED) {
		cout << "Error with ready" << endl;
	}
	if ((posted = sem_open("postedsem", O_CREAT, 0, 1)) == SEM_FAILED) {
		cout << "Error with posted" << endl;
	}

	cout << "OKEY" << endl;
	system("clear");

	while (true) {
		cout << "Waiting for messages from server..." << endl;
		sem_wait(ready);
		
		void *buffer = get_shared_memory(fd);
		string message = (char *)buffer;

		if (message == "quit") {
			sem_destroy(ready);
			sem_destroy(posted);
			exit(0);
		}

		system("clear");
		printf("Message from server: %s;\n", message.c_str());
		sem_post(posted);
	}
}


void main_process(char *path) {
	int fd = create_shared_memory(SHARED_MEMORY_NAME);

	sem_t *ready, *posted;
	if ((ready = sem_open("readysem", O_CREAT, 0644, 1)) == SEM_FAILED) {
		int value = errno;
		cout << "Error with ready" << "; " << value << endl;
	}
	if ((posted = sem_open("postedsem", O_CREAT, 0644, 1)) == SEM_FAILED) {
		cout << "Error with posted" << endl;
	}
	sem_post(posted);

	switch (fork()) {
		case -1:
			cout << "Error with fork" << endl;
			exit(0);
		case 0:
			system(("gnome-terminal -- bash -c \"" + string(path) + " 1 2; bash\" >>/dev/null 2>>/dev/null").c_str()) ;
			break;
		default: {
			system("clear");
			cout << "Send message to client: " << endl;
			while (true) {
				sem_wait(posted);

				cout << "> ";
				string message;
				getline(cin, message);

				void *buffer = get_shared_memory(fd);
				strcpy((char *)buffer, message.c_str());

				sem_post(ready);
				if (message == "quit") {
					cout << "BYE BYE" << endl;
					sem_destroy(posted);
					sem_destroy(ready);
					exit(0);
				}
			}
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc > 1) child_process();
	else main_process(argv[0]);

	return 0;
}
