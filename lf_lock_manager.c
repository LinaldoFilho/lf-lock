/*
	Linaldo Filho
	linaldo.ah@gmail.com
*/

/* System lib include */
#include <pthread.h>
#include <stdlib.h>
#include "lf_lock_manager.h"

/* Type def */
typedef struct lf_lock_data_
{
	void *Ref;
	pthread_mutex_t *Lock;
	struct lf_lock_data_ *Next;
} lf_lock_data;

typedef struct lf_lock_list_
{
	lf_lock_data *data;
	pthread_mutex_t *Lock;
} lf_lock_list;

/* Prototype include */
static lf_lock_list *lf_lock_list_init();
static lf_lock_data *lf_lock_new_data(void *ref);
static lf_lock_data *lf_lock_find_data(void *ref);

/* Global list lock reading ref */
static lf_lock_list *lf_lock_dir;

/* Init lock manager */
int lf_init_lock_manager()
{
	if ((lf_lock_dir = lf_lock_list_init()) == NULL)
		return 0;
	return 1;
}

static lf_lock_list *lf_lock_list_init()
{
	/* Declares new list */
	lf_lock_list *newlist = malloc(sizeof(lf_lock_list));
	if (newlist == NULL)
	{
		return NULL;
	}

	lf_lock_data *pdata = malloc(sizeof(lf_lock_data));
	if (pdata == NULL)
	{
		free(newlist);
		return NULL;
	}
	/* Init list private ref */
	pdata->Ref = (void *)newlist;

	/* Declares Mutex */
	pdata->Lock = malloc(sizeof(pthread_mutex_t));
	if (pdata->Lock == NULL)
	{
		free(newlist);
		free(pdata);
		return NULL;
	}

	pdata->Next = NULL;

	/* Define list */
	newlist->data = pdata;
	newlist->Lock = malloc(sizeof(pthread_mutex_t));
	if (newlist->Lock == NULL)
	{
		free(newlist);
		free(pdata);
		free(pdata->Lock);
		return NULL;
	}
	/* Init Mutex */
	pthread_mutex_init(pdata->Lock, NULL);
	pthread_mutex_init(newlist->Lock, NULL);

	return newlist;
}

static lf_lock_data *lf_lock_find_data(void *ref)
{
	lf_lock_data *node = lf_lock_dir->data;
	do {
		if (node->Ref == ref)
		{
			return node;
		}
		node = node->Next;
	} while (node != NULL);

	return lf_lock_new_data(ref);
}

static lf_lock_data *lf_lock_new_data(void *ref)
{
	lf_lock_data *node = malloc(sizeof(lf_lock_data));
	if (node == NULL)
	{
		return NULL;
	}

	/* Init list private ref */
	node->Ref = ref;

	/* Declares Mutex */
	node->Lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if (node->Lock == NULL)
	{
		free(node);
		return NULL;
	}

	/* Init Mutex */
	pthread_mutex_init(node->Lock, NULL);
	node->Next = NULL;

	/* Join list */
	if (pthread_mutex_lock(lf_lock_dir->Lock))
	{
		pthread_mutex_destroy(node->Lock);
		if (node->Lock != NULL)
			free(node->Lock);
		free(node);
		return NULL;
	}
	lf_lock_data *list = lf_lock_dir->data;
	do {
		if (list->Next == NULL)
		{
			break;
		}
		list = list->Next;
	} while (list != NULL);
	list->Next = node;
	pthread_mutex_unlock(lf_lock_dir->Lock);

	return node;
}

const int lf_trylock(void *ref)
{
	if (ref == NULL)
		return -1;
	lf_lock_data *lock_node = lf_lock_find_data(ref);
	if (lock_node == NULL)
		return 1;
	return pthread_mutex_trylock(lock_node->Lock);
}

const int lf_lock(void *ref)
{
	if (ref == NULL)
		return -1;
	lf_lock_data *lock_node = lf_lock_find_data(ref);
	if (lock_node == NULL)
		return 1;
	return pthread_mutex_lock(lock_node->Lock);
}

const int lf_unlock(void *ref)
{
	if (ref == NULL)
		return -1;

	lf_lock_data *lock_node = lf_lock_find_data(ref);
	if (lock_node == NULL)
		return 1;
	return pthread_mutex_unlock(lock_node->Lock);
}

const int lf_lock_destroy(void *ref)
{
	if (ref == NULL)
		return -1;
	lf_lock_data *list, *node;
	list = lf_lock_dir->data;
	node = NULL;

	if (pthread_mutex_lock(lf_lock_dir->Lock))
		return -1;

	if (list->Ref == ref)
	{
		node = list;
		lf_lock_dir->data = node->Next;

		goto des;
	}

	do {
		if (list->Next->Ref == ref)
		{
			node = list->Next;
			break;
		}
		list = list->Next;
	} while (list != NULL);
	if (node == NULL)
		goto end;
	list->Next = node->Next;

des:
	pthread_mutex_destroy(node->Lock);
	if (node->Lock != NULL)
		free(node->Lock);
	free(node);
end:
	pthread_mutex_unlock(lf_lock_dir->Lock);

	return 0;
}