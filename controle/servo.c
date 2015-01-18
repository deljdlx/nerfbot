#include <stdio.h>
#include <stdlib.h>


#include <inttypes.h>



#include <math.h>

#include <time.h>

#include <unistd.h>
#include <libpiface-1.0/pfio.h>


#include <sys/shm.h>		//Used for shared memory

 
#define BUFFERSIZE    255
#define SHM_SIZE 1024  /* make it a 1K shared memory segment */


long long current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
	long long microseconds = te.tv_sec*1000000LL + te.tv_usec; // caculate milliseconds
    return microseconds;
}




char * getSharedMemoryContent(char * data) {

	char buffer[SHM_SIZE];

	int index=0;
	
	int i=0;
	int start=0;
	
	for(i=0; i<SHM_SIZE; i++) {
		
		
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

int * extractAngles(char * string) {
	int angles[2];
	int index=0;
	
	char angle0[4];
	char angle1[4];
	
	int first=0;
	int tempIndex=0;
	
	
	//printf("%s\n", string);
	
	for(index=0; index<SHM_SIZE; index++) {
		
		if(string[index]!='\t' && !first) {
			angle0[tempIndex]=string[index];
		}
		else if(string[index]=='\t') {
			angle0[tempIndex]='\0';
			angles[0]=atoi(angle0);
			first=1;
			tempIndex=-1;
		}
		else if(string[index]!='\0') {
			angle1[tempIndex]=string[index];
		}
		else {
			angle1[tempIndex]='\0';
			angles[1]=atoi(angle1);
			return angles;
		}
		tempIndex++;
	} 
}



int main(int argc, char **argv)
{
	
	unsigned char buffer[BUFFERSIZE];
	FILE                         *instream;

	int c;
	int bufferSize;
	
	int angleX;
	int angleY;
	
	
	long long startTime;
	long long now;
	
	
	long long duration;

	
	double upTime180=2000;
	
	double upTime1=2000;
	double upTime2=2000;
	
	
	long long period=18000;
	
	long long lastCycleTime=0;

	int lastState=0;
	int lastState2=0;
	



	char * sharedMemory;
	char *data;

	key_t key=672213396;

	int shmid;

	
	if ((shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT)) == -1) {
		perror("shmget");
		exit(1);
	}
	
	
	
	data = shmat(shmid, (void *)0, 0);
	
	if (data == (char *)(-1)) {
		perror("shmat");
		exit(1);
	}


	
	if (pfio_init() < 0) {
		exit(-1);
	}
	
	
	
	
	

	bufferSize=0;
	instream=fopen("/dev/stdin","r");
	startTime=current_timestamp();
	
	
	int * angles;
	
	while(1) {

		sharedMemory=getSharedMemoryContent(data);
		
		angles=extractAngles(sharedMemory);
		
		upTime1=ceil(upTime180*((double) angles[0]/(double)180));
		upTime2=ceil(upTime180*((double) angles[1]/(double)180));
		
		
		printf("%d\t%d\t%f\t%f\n", angles[0], angles[1], upTime1, upTime2);
		
		
		now=current_timestamp();
		duration=now-startTime;
		
		
		
		
		
		
		if(lastCycleTime==0) {
			lastCycleTime=now;
			duration=0;
			pfio_digital_write(7,0);
			pfio_digital_write(3,0);
			lastState=0;
			lastState2=0;
		}
		

		if(duration<upTime1 && lastState==1) {
			pfio_digital_write(7,0);
			lastState=0;
		}
		else if(lastState==0 && duration>upTime1){
			pfio_digital_write(7,1);
			lastState=0;
		}
		
		
		if(duration<upTime2 && lastState2==1) {
			pfio_digital_write(3,0);
			lastState2=0;
		}
		else if(lastState2==0 && duration>upTime2){
			pfio_digital_write(3,1);
			lastState2=0;
		}

		
		
		if(duration>=period) {
			startTime=now;
			lastCycleTime=0;
		}
	}
	
	
	
	
	while ((c = fgetc (instream)) != EOF) {
		
		if(c=='\t') {
			buffer[bufferSize]='\0';
			angleX=atoi(buffer);
			bufferSize=0;
		}
		else if(c=='\n') {
			buffer[bufferSize]='\0';
			angleY=atoi(buffer);
			bufferSize=0;
		}
		else {
			buffer[bufferSize]=c;
			bufferSize++;
		}
	}
	
	
	
	fclose (instream);



    pfio_deinit();
    return 0;
}

