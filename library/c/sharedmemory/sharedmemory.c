
#include <sys/shm.h>		//Used for shared memory



char * initializeSharedMemory(key_t key, int size) {
	int shmid;
	char * data;

	if ((shmid = shmget(key, size, 0666 | IPC_CREAT)) == -1) {
		perror("shmget");
		exit(1);
	}
	
	data = shmat(shmid, (void *)0, 0);
	
	if (data == (char *)(-1)) {
		perror("shmat");
		exit(1);
	}
	return data;
}


char * getSharedMemoryContent(char * data, int sharedMemorySize) {

	char buffer[sharedMemorySize];

	int index=0;
	
	int i=0;
	int start=0;
	
	for(i=0; i<sharedMemorySize; i++) {
		
		
		if(data[i]=='\0' || data[i]=='\n') {
			if(start) {
				buffer[index]='\0';
				return buffer;
			}
		}
		else {
			start=1;
			buffer[index]=data[i];
			index++;
		}
	}
	return buffer;
}





