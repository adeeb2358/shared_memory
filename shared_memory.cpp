#include "bigHeader.h"
#include "shared_memory.h"


/*
	function to choose whether the program
	is consumer or producer
*/
void get_option(int argument_count){
	if(argument_count == 1){
		shared_memory_consumer();
	}
	else{
		shared_memory_producer();
	}
}

/*
	consumer creates a shared memory
	segment and it displays any data 
	that is written into it
*/

void shared_memory_consumer(){

	printf("Shared memory consumer is invoked..\n");
	
	int run_status       = 1;
	void *shared_memory  = (void *)0;
	struct shared_memory_struct *shared_memory_data;
	int shared_memory_id = 0;

	srand((unsigned int)getpid());
	
	/*
		code for creation of the shared memory
	*/
	
	shared_memory_id = shmget((key_t)1234,
		sizeof(struct shared_memory_struct),
		0666|IPC_CREAT
		);
	
	/*
		if shared memory is not made
		then
	*/

	if(-1 == shared_memory_id){
		fprintf(stderr,"shmget failed\n");
		return;
	}

	/*
	make the shared memory accessible for the program
	*/
	
	shared_memory = shmat(shared_memory_id,(void*)0,0);
	if((void*)-1 == shared_memory){
		fprintf(stderr,"shmat failed\n");
		return;
	}

	printf("Memory attached at :- %ld\n",(intptr_t)shared_memory);
	/*
		assigns shared memory segment to shared data
	*/

	shared_memory_data = (struct shared_memory_struct*)shared_memory;
	shared_memory_data->written_by_you = 0;
	while(run_status){
		printf("Recieved message:->%s\n",shared_memory_data->some_text);
		sleep(rand()%4);/*make the other process wait for us*/
		shared_memory_data->written_by_you = 0;
		if(strncmp(shared_memory_data->some_text,"end",3) == 0){
			run_status = 0;
		}
	}

	/*
		finally the shared memory is detached and deleted
	*/

	if(shmdt(shared_memory) == -1){
		fprintf(stderr,"shmdt failed\n");
		return;
	}

	if(shmctl(shared_memory_id,IPC_RMID,0) == -1){
		fprintf(stderr,"shmctl(IPC_RMID) failed\n");
		return;
	}

	return;
}

/*
	producer is a program 
	which writes data into the 
	shared memory area of the consumer
*/

void shared_memory_producer(){
	printf("shared memory producer is invoked...\n");
	int run_status      = 1;
	void *shared_memory = (void*)0;
	struct shared_memory_struct *shared_memory_data;
	/*
		bufsiz variable in linux shows the default buffer
		size of a stream by the operating system
	*/
	char memory_buffer[BUFSIZ];
	int shared_memory_id = 0;

	/*
		shared memory is either created or use the exisitng shared memory
	*/

	shared_memory_id = shmget((key_t)1234,sizeof(struct shared_memory_struct),0666|IPC_CREAT);
	if(-1 == shared_memory_id){
		fprintf(stderr,"Shared memory creation failed\n");
		return;
	}

	/*
		attach the sharedmemory to the producer program
	*/
	shared_memory = shmat(shared_memory_id,(void*)0,0);
	if((void*)-1 == shared_memory){
		fprintf(stderr,"shmat failed\n");
		return;
	}

	printf("Memory attached at %ld",(intptr_t)shared_memory);
	shared_memory_data = (struct shared_memory_struct*)shared_memory;
	
	/*
		take the input from the stdin
		and write to the shared memory
	*/

	while(run_status){
		while(shared_memory_data->written_by_you == 1){
			sleep(1);
			fprintf(stderr,"Waiting for client...\n");
		}
		printf("Enter some text: ");
		fgets(memory_buffer,BUFSIZ,stdin);
		strncpy(shared_memory_data->some_text,memory_buffer,TEXT_SZ);
		shared_memory_data->written_by_you = 1;
	
		if(strncmp(memory_buffer,"end",3) == 0){
			run_status = 0;
		}

	}
	/*
		deleting the shared memory
	*/
	if(shmdt(shared_memory) == -1){
		fprintf(stderr,"shared memory deletion failed\n");
		return;
	}

	return;


}