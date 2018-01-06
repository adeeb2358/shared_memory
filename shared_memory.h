#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H


#define TEXT_SZ 2048

struct shared_memory_struct{
	int written_by_you;
	char some_text[TEXT_SZ];
};

void get_option(int argument_count);
void shared_memory_consumer();
void shared_memory_producer();


#endif // SHARED_MEMORY_H
