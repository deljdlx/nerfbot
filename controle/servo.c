#include <stdio.h>
#include <stdlib.h>


#include <inttypes.h>



#include <math.h>

#include <time.h>

#include <unistd.h>

#include <sys/shm.h>		//Used for shared memory


#include <libpiface-1.0/pfio.h>

#include "../library/json-parser/json.c"


#define SHM_SIZE 1024  /* make it a 1K shared memory segment */


long long getCurrentTimestamp() {
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



//extract angles from a string of that form angle1\tangle2\0
//return an array of two int
int * extractAnglesFromString(char * string) {
	int angles[2];
	int index=0;
	
	char angle0[4];
	char angle1[4];
	
	int first=0;
	int tempIndex=0;

	
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
	return angles;
}




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


int * getAnglesFromBuffer(char * data) {
	int * angles;
	angles=extractAnglesFromString(
		getSharedMemoryContent(data)
	);
	return angles;
}



char * getFileContent(char * fileName) {
	char * buffer = 0;
	long length;
	FILE * f = fopen (fileName, "rb");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = malloc (length);
	  if (buffer)
	  {
		fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}
	
	return buffer;
}




int main(int argc, char **argv)
{

	


	
	long upTime180=2000;
	long period=19000;
	
	
	double upTime1=2000;
	double upTime2=2000;
	

	long  key=672213396;
	int sharedMemorySize=1024;
	
	long long lastCycleTime=0;
	long long startTime;
	long long now;
	long long duration;
	
	

	int lastState=0;
	int lastState2=0;
	int * angles;
	

	char *data;


	
	
	char * configurationBuffer;
	
	
	json_value * configuration;
	
	
	configurationBuffer=getFileContent("configuration/configuration.json");
	configuration=json_parse(configurationBuffer, 2048);
	
	
	int configurationIndex=0;
	

	for(configurationIndex=0; configurationIndex<configuration->u.object.length; configurationIndex++) {
		if(strcmp(configuration->u.object.values[configurationIndex].name, "sharedMemoryKey")==0) {
			key=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, key);
		}
		else if(strcmp(configuration->u.object.values[configurationIndex].name, "sharedMemorySize")==0) {
			sharedMemorySize=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, sharedMemorySize);
		}
		else if(strcmp(configuration->u.object.values[configurationIndex].name, "period")==0) {
			period=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, period);
		}
		else if(strcmp(configuration->u.object.values[configurationIndex].name, "upTime180")==0) {
			upTime180=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, upTime180);
		}
	}

	
	

	
	data=initializeSharedMemory(key, sharedMemorySize);




	





	
	if (pfio_init() < 0) {
		exit(-1);
	}

	startTime=getCurrentTimestamp();

	
	while(1) {
		
		angles=getAnglesFromBuffer(data);
		
		
		upTime1=ceil(upTime180*((double) angles[0]/(double)180));
		upTime2=ceil(upTime180*((double) angles[1]/(double)180));
		
		
		printf("%d\t%d\t%f\t%f\n", angles[0], angles[1], upTime1, upTime2);
		
		
		now=getCurrentTimestamp();
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
	
	
	
	/*
	
	unsigned char buffer[BUFFERSIZE];
	FILE                         *instream;

	int c;
	int bufferSize;
	int angleX;
	int angleY;
	

	bufferSize=0;
	instream=fopen("/dev/stdin","r");
	
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
	*/



    pfio_deinit();
    return 0;
}

