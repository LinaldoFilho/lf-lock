#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// gcc -pthread -o test test.c lf_lock_manager.c
// printf("%s:%d :=: %s\r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

#include "lf_lock_manager.h"


void *myThreadFun(void *vargp)
{
	int *pp = vargp;
	sleep(5);
	printf("%s:%d :=: %s Thread Unlock ref i: %d \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, lf_unlock(pp));

	return NULL;
}

int main()
{
	printf("%s:%d :=: %s Init lf_lock_manager \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
	lf_init_lock_manager();

	int i = 100;
	printf("%s:%d :=: %s Lock ref i:  %d \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, lf_lock(&i));
	printf("%s:%d :=: %s TryLock ref i:  %d \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, lf_trylock(&i));
	printf("%s:%d :=: %s Unlock ref i:  %d \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, lf_unlock(&i));
	printf("%s:%d :=: %s TryLock ref i:  %d \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, lf_trylock(&i));

	pthread_t thread_id;
	pthread_create(&thread_id, NULL, myThreadFun, &i);
	printf("After Thread\n");

	while (lf_trylock(&i))
	{
		printf("%s:%d :=: %s ... \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);
		sleep(1);
	}

	printf("%s:%d :=: %s Destroy lock ref i:  %d \r\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, lf_lock_destroy(&i));
	
	return(0);
}

//void test()
//{
//
//
//	char buf[100];
//	va_list vl;
//	va_start(vl, frm);
//
//	vsnprintf(buf, sizeof(buf), frm, vl);
//
//	va_end(vl);
//
//	//printf("Line: %d  \n", line);
//	printf(buf);
//
//	return;
//}