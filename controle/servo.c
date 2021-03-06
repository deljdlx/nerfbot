#include <stdio.h>
#include <stdlib.h>


#include <inttypes.h>



#include <math.h>

#include <time.h>

#include <unistd.h>




#include <libpiface-1.0/pfio.h>

#include "../library/c/util/file.c"
#include "../library/c/sharedmemory/sharedmemory.c"
#include "../library/c/json-parser/json.c"


#include <sys/ipc.h> 
#include <sys/sem.h>


union semun {
	int val;
	struct semid_ds* buf;
	unsigned short * array;
	struct seminfo* __buf;
} arg;



long long getCurrentTimestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
	long long microseconds = te.tv_sec*1000000LL + te.tv_usec; // caculate milliseconds
    return microseconds;
}







//extract angles from a string of that form angle1\tangle2\0
//return an array of two int
int * extractAnglesFromString(char * string, int sharedMemorySize) {
	int angles[2];
	int index=0;
	
	char angle0[4];
	char angle1[4];
	
	int first=0;
	int tempIndex=0;

	
	for(index=0; index<sharedMemorySize; index++) {
		
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










int * getAnglesFromBuffer(char * data, int sharedMemorySize) {
	int * angles;
	angles=extractAnglesFromString(
		getSharedMemoryContent(data, sharedMemorySize),
		sharedMemorySize
	);
	return angles;
}



int main(int argc, char **argv)
{

	

	long idleTime=200;
	
	long upTime180=2000;
	long upTime0=1000;
	long period=19000;
	
	
	double upTime1=2000;
	double upTime2=2000;
	

	long  key=672213396;
	int sharedMemorySize=1024;
	
	long long lastCycleTime=0;
	long long startTime;
	long long now;
	long long duration;
	
	
	long long checkStart;
	long long checkPeriod;
	long long checkDuration;
	
	

	int lastState=0;
	int lastState2=0;
	int * angles;
	

	char *data;
	
	
	int semaphore;
	int semaphoreKey;


	
	
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
		else if(strcmp(configuration->u.object.values[configurationIndex].name, "checkPeriod")==0) {
			checkPeriod=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, checkPeriod);
		}
		else if(strcmp(configuration->u.object.values[configurationIndex].name, "upTime0")==0) {
			upTime0=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, upTime0);
		}
		else if(strcmp(configuration->u.object.values[configurationIndex].name, "idleTime")==0) {
			idleTime=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, idleTime);
		}
		else if(strcmp(configuration->u.object.values[configurationIndex].name, "semaphoreKey")==0) {
			semaphoreKey=configuration->u.object.values[configurationIndex].value->u.integer;
			printf("%s %d\n", configuration->u.object.values[configurationIndex].name, semaphoreKey);
		}
	}

	
	

	
	data=initializeSharedMemory(key, sharedMemorySize);


	
	if (pfio_init() < 0) {
		exit(-1);
	}

	startTime=getCurrentTimestamp();
	
	checkStart=getCurrentTimestamp();

	
	int lastAngle0=0;
	int lastAngle1=0;

		
	union semun argument;
	
	
	int semaphoreValue;
	
	
	semaphore=semget(semaphoreKey, 1, IPC_CREAT | 0666);
	

	angles=getAnglesFromBuffer(data, sharedMemorySize);
	




	int currentAngle0=angles[0];
	int currentAngle1=angles[1];

	while(1) {

		
		
		now=getCurrentTimestamp();
		duration=now-startTime;
		checkDuration=now-checkStart;
		
		angles[0]=currentAngle0;
		angles[1]=currentAngle1;
		
		if(checkDuration>checkPeriod) {
			semaphoreValue=semctl(semaphore, 0, GETVAL, argument);
			if(semaphoreValue==1) {
				angles=getAnglesFromBuffer(data, sharedMemorySize);
				checkStart=now;
			}
		}
		

		currentAngle0=angles[0];
		currentAngle1=angles[1];
		
		
		upTime1=upTime0+ceil(upTime180*((double) angles[0]/(double)180));
		upTime2=upTime0+ceil(upTime180*((double) angles[1]/(double)180));
		
		
		
		/*
		if((lastAngle0-angles[0])!=0 || (lastAngle1-angles[1])!=0) {
			lastAngle0=(int) angles[0];
			lastAngle1=(int) angles[1];
			printf("%d\t%d\t%d\t%d\t%f\t%f\n",
				angles[0],
				angles[1],
				lastAngle0,
				lastAngle1,
				upTime1,
				upTime2
			);
		}
		*/
		


		

		
		
		
		
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
		
		usleep(idleTime);
	}

    pfio_deinit();
    return 0;
}

