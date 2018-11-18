/*
	Linaldo Filho
	linaldo.ah@gmail.com
*/
#ifndef _LF_LOCK_MANAGER_LIB_
#define _LF_LOCK_MANAGER_LIB_
/* System lib include */
	#include <pthread.h>

/* Prototype include */
	
	/* Manager Lock variables */
	int lf_init_lock_manager();

	//Lock memory reference
	const int lf_lock(void *ref);
    
    //Try lock memory reference
    const int lf_trylock(void *ref);
	
	//Unlock memory reference
	const int lf_unlock(void *ref);
	
	//Destroy struct ObjLock the memory reference before free memory reference
	const int lf_lock_destroy(void *ref);
#endif









