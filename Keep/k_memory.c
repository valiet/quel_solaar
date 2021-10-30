#include <math.h>
#include <stdio.h>
#include "forge.h"
#include "imagine.h"

typedef unsigned long long uint64;

typedef struct{
	uint cluster;
	uint allocation;
	uint user;
}KAddress;

#define KEEP_CLUSTER_MAX_SIZE (1024 * 1024)
#define KEEP_CLUSTER_MIN_SIZE (1024 * 512)
#define KEEP_CLUSTER_ADD (1024 * 64)
#define KEEP_CLUSTER_ALLOCATION_COUNT (256 * 1)

/*
adresses are cluster plus allocation.
Allocation 8 * 256 
*/

typedef struct{
	uint offset;
	uint size;
	void *lock;
}KAlloc;


typedef struct{
	void *next; /* next cluster in hash*/
	uint cluster_id;  /* next cluster in hash*/
	KAlloc *index; /* index of allocations */
	uint allocation_count;
	uint8 *data; /* data */
	uint allocated; /* memorry allocated */
	uint size; /* size excluding gaps*/
	uint used; /* size including gaps*/
	uint gaps; /* how many gaps are in the cluster*/
	uint locks_locked; /* when this is zero the cluster can be removed*/
	boolean saved; /* does the cluster need to be saved*/
	uint age; /* how long since used */
}KCluster;

typedef struct{
	KCluster **hash;
	uint hash_size;
	uint64 update_icrement;
	uint64 current_footprint;
	uint64 max_footprint;
	uint cluster_count;
	uint save_pos;
	void *lock; /* locks everything except the existance of clusters and the actiual data */
}KSession;


KCluster *keep_cluster_load(uint cluster_id)
{
	char file_name[64];
	KCluster *c;
	FILE *f;
	KAlloc *current;
	uint32 *sizes, offset = 0;
	uint i;
	
	sprintf(file_name, "./archive/%u.KCF", cluster_id);
	f = fopen(file_name, "rb");
	if(f == NULL)
		return NULL;
	c = malloc(sizeof *c);
	c->cluster_id = cluster_id;

	sizes = malloc((sizeof *sizes) * KEEP_CLUSTER_ALLOCATION_COUNT);
	if(KEEP_CLUSTER_ALLOCATION_COUNT != fread(sizes, (sizeof *sizes), KEEP_CLUSTER_ALLOCATION_COUNT, f))
	{
		free(c);
		free(sizes);
		fclose(f);
		return NULL;
	}

	current = c->index = malloc((sizeof *current) * KEEP_CLUSTER_ALLOCATION_COUNT);
	for(i = 0; i < KEEP_CLUSTER_ALLOCATION_COUNT; i++)
	{
		current->offset = offset;
		current->size = sizes[i];
		offset += sizes[i];
		current->lock = NULL;
		current++;
	}
	free(sizes);
	c->allocated = offset;
	c->size = offset;
	c->used = offset;
	c->allocation_count = 0;
	c->gaps = 0;
	c->locks_locked = 0;
	if(c->size < KEEP_CLUSTER_MAX_SIZE)
	{
		c->allocated += KEEP_CLUSTER_ADD;
		if(c->allocated < KEEP_CLUSTER_MIN_SIZE)
			c->allocated = KEEP_CLUSTER_MIN_SIZE;
		else if(c->allocated > KEEP_CLUSTER_MAX_SIZE)
			c->allocated = KEEP_CLUSTER_MAX_SIZE;
	}
	c->data = malloc(c->allocated);
	if(offset != fread(c->data, 1, offset, f))
	{
		free(c);
		free(c->index);
		free(c->data);
		fclose(f);
		return NULL;
	}
	fclose(f);
	c->saved = TRUE;
	return c;
}


boolean keep_cluster_save(KCluster *c)
{
	char file_name[64];
	FILE *f;
	KAlloc *current;
	uint32 *sizes, offset = 0;
	uint i;
	sprintf(file_name, "./archive/%u.KCF", c->cluster_id);
	f = fopen(file_name, "wb");
	if(f == NULL)
		return FALSE;
	sizes = malloc((sizeof *sizes) * KEEP_CLUSTER_ALLOCATION_COUNT);
	for(i = 0; i < KEEP_CLUSTER_ALLOCATION_COUNT; i++)	
		sizes[i] = c->index[i].size;
	if(KEEP_CLUSTER_ALLOCATION_COUNT != fwrite(sizes, (sizeof *sizes), KEEP_CLUSTER_ALLOCATION_COUNT, f))
	{
		free(sizes);
		fclose(f);
		return FALSE;
	}
	free(sizes);
	current = c->index;
	for(i = 0; i < KEEP_CLUSTER_ALLOCATION_COUNT; i++)
	{
		if(current->size != 0 && current->size != fwrite(&c->data[current->offset], 1, current->size, f))
		{
			fclose(f);
			return FALSE;
		}
		current++;
	}
	c->saved = TRUE;
	fclose(f);
	return TRUE;
}

KSession *keep_session_create(uint64 memory_footprint, uint64 user_id)
{
	KSession *session;
	uint i;
	session = malloc(sizeof *session);
	session->hash_size = memory_footprint / KEEP_CLUSTER_MAX_SIZE;
	if(session->hash_size < 8)
		session->hash_size = 8;
	session->hash = malloc((sizeof *session->hash) * session->hash_size);
	for(i = 0; i < session->hash_size; i++)
		session->hash[i] = NULL;
	session->update_icrement = 0;
	session->current_footprint = 0;
	session->max_footprint = memory_footprint;
	session->cluster_count = 0;
	session->save_pos = 0;
	session->lock = imagine_mutex_create();
	return session;
}

boolean keep_session_destroy(KSession *session)
{
	uint i;
	KCluster *c;
	for(i = 0; i < session->hash_size; i++)
	{
		while(session->hash[i] != NULL)
		{
			c = session->hash[i];
			if(!c->saved)
				if(!keep_cluster_save(c))
					return FALSE;
			session->hash[i] = c->next;
			free(c->index);
			free(c->data);
			free(c);
		}
	}
	imagine_mutex_destroy(session->lock);
	free(session->hash);
	free(session);
	return TRUE;
}

boolean keep_session_service(KSession *session)
{
	uint i;
	KCluster *c, *best;
	imagine_mutex_lock(session->lock); 
	while(session->current_footprint > session->max_footprint || session->cluster_count > session->hash_size)
	{
		best = NULL;
		for(i = 0; i < session->hash_size; i++)
			for(c = session->hash[i]; c != NULL; c = c->next)
				if(c->locks_locked == 0 && (best == NULL || best->age > c->age))
					best = c;
		if(best == NULL)
			break;
		else
		{
			if(!best->saved && !keep_cluster_save(best))
			{
				imagine_mutex_unlock(session->lock); 
				return TRUE;
			}
			i = best->cluster_id % session->hash_size;
			if(session->hash[i] == best)
				session->hash[i] = best->next;
			else
			{
				for(c = session->hash[i]; c->next != best; c = c->next);
				c->next = best->next;
			}
			session->current_footprint -= best->allocated;
			session->cluster_count--;
			free(best->index);
			free(best->data);
			free(best);
		}
	
	}
	imagine_mutex_unlock(session->lock); 
	return FALSE;
}


extern void		*imagine_mutex_create(); /* Creates a Mutex. Mutex locks are unlocked when created.*/
extern void		imagine_mutex_lock(void *mutex); /* Lock a mutex. If the lock is already locked, the thread will wait on the lock until the lock is unlocked so that it can lock it */
extern boolean	imagine_mutex_lock_try(void *mutex); /* The thread will atempt to lock the thread, if the lock is already locked if will returne FALSE and fail, If is is not locked, it will lock the mutex and return TRUE */
extern void		imagine_mutex_unlock(void *mutex); /* Un locks the Mutex */
extern void		imagine_mutex_destroy(void *mutex); /* Destroys the mutex */
extern void		imagine_thread(void (*func)(void *data), void *data); /* launches a thread that will execute the function pointer. The void pointer will be given as i parameter. Onec the function returns the thread will be deleted.*/



KCluster *keep_lookup(KSession *session, uint64 address)
{
	uint64 cluster_id;
	KCluster *c;
	cluster_id = address / KEEP_CLUSTER_ALLOCATION_COUNT;
	if(address == 512)
		cluster_id = cluster_id;
	for(c = session->hash[cluster_id % session->hash_size]; c != NULL && c->cluster_id != cluster_id; c = c->next);
	if(c == NULL)
	{
		c = keep_cluster_load(cluster_id);
		if(c == NULL)
			return NULL;
		c->next = session->hash[cluster_id % session->hash_size];
		session->hash[cluster_id % session->hash_size] = c;
	}
	return c;
}

void *keep_get(KSession *session, uint64 address, uint *size)
{
	uint64 alloc;
	KCluster *c;
	imagine_mutex_lock(session->lock); 
	c = keep_lookup(session, address);
	c->age = session->update_icrement++;
	alloc = address % KEEP_CLUSTER_ALLOCATION_COUNT;
	if(c->index[alloc].size == 0)
		return NULL;
	if(c->index[alloc].lock == NULL)
		c->index[alloc].lock = imagine_mutex_create();
	c->locks_locked++;
	imagine_mutex_unlock(session->lock); 
	imagine_mutex_lock(c->index[alloc].lock);
	if(size != NULL)
		*size = c->index[alloc].size;
	return &c->data[c->index[alloc].offset];
}

void keep_return(KSession *session, uint64 address, boolean modifyed)
{
	KCluster *c;
	imagine_mutex_lock(session->lock); 
	c = keep_lookup(session, address);
	if(modifyed)
		c->saved = FALSE;
	imagine_mutex_unlock(session->lock); 
	imagine_mutex_unlock(c->index[address % KEEP_CLUSTER_ALLOCATION_COUNT].lock);
	imagine_mutex_lock(session->lock); 
	c->locks_locked--;
	imagine_mutex_unlock(session->lock); 
}

KCluster *keep_cluster_add(KSession *session, uint64 size, uint64 address)
{
	KCluster *c;
	uint i;
	c = malloc(sizeof *c);
	c->next = session->hash[address % session->hash_size];
	session->hash[address % session->hash_size] = c;
	c->cluster_id = address;
	c->index = malloc((sizeof *c->index) * KEEP_CLUSTER_ALLOCATION_COUNT);
	for(i = 0; i < KEEP_CLUSTER_ALLOCATION_COUNT; i++)
	{
		c->index[i].lock = NULL;
		c->index[i].size = 0;
		c->index[i].offset = 0;
	}
	c->allocation_count = 0;
	c->allocated = size + KEEP_CLUSTER_ADD;
	if(c->allocated < KEEP_CLUSTER_MIN_SIZE)
		c->allocated = KEEP_CLUSTER_MIN_SIZE;
	session->current_footprint += c->allocated;
	session->cluster_count++;
	c->data = malloc(c->allocated);
	c->size = size; /* size excluding gaps*/
	c->used = size; /* size including gaps*/
	c->gaps = 0; /* how many gaps are in the cluster*/
	c->locks_locked = 0;
	c->saved = FALSE; /* does the cluster need to be saved*/
	c->age = session->update_icrement++; /* how long since used */
	return c;
}

boolean keep_allocate_root(KSession *session, uint64 size, uint64 address)
{
}

uint64 keep_allocate(KSession *session, uint64 size)
{
	static uint64 seed = 8237456435345; /* FIX ME */
	KCluster *c, *found = NULL;
	uint i;
	uint64 best, skip;
	size = ((size + 7) / 8) * 8;
	skip = -1;
	imagine_mutex_lock(session->lock); 
	best = 0;
	for(i = 0; i < session->hash_size && found == NULL; i++)
	{
		for(c = session->hash[i]; c != NULL; c = c->next)
		{
			if(c->used + size < c->allocated && 
				c->allocation_count < KEEP_CLUSTER_ALLOCATION_COUNT &&
				c->age < skip && 
				c->age > best)
			{
				found = c;
				best = c->age;
			}
		}
	}
	if(found == NULL)
	{		
		char file_name[64];		FILE *f = 1;
		seed /= KEEP_CLUSTER_ALLOCATION_COUNT;
		while(f != NULL)
		{
			seed = (seed + 1);
			found = NULL;
			for(i = 0; i < session->hash_size && found == NULL; i++)
				for(c = session->hash[i]; c != NULL; c = c->next)
					if(c->cluster_id == seed)
						found = c;
			if(found == NULL)
			{
				sprintf(file_name, "./archive/%u.KCF", seed);
				if(seed != 0)
					f = fopen(file_name, "rb");
				if(f != NULL)
					fclose(f);
			}
		}
		found = keep_cluster_add(session, size, seed);
	}
	for(i = 0; i < KEEP_CLUSTER_ALLOCATION_COUNT && found->index[i].size != 0; i++);
	found->index[i].size = size;
	found->index[i].offset = found->used;
	found->used += size;
	if(found->used > found->allocated || size == 0)
		i = 0;
	found->allocation_count++;
	imagine_mutex_unlock(session->lock);

	return found->cluster_id * KEEP_CLUSTER_ALLOCATION_COUNT + i;
}

void keep_free(KSession *session, uint64 address)
{
	uint64 alloc;
	KCluster *c;
	imagine_mutex_lock(session->lock); 
	c = keep_lookup(session, address);
	c->size = 0;
	imagine_mutex_unlock(session->lock);
}

#define KEEP_TEST_ALLOC_COUNT 4096
#define KEEP_TEST_ALLOC_SIZE 4096

void keep_index_test();


int main2()
{
	uint64 *addresses;
	KSession *session;
	uint i, j;
	uint8 *buffer;
	keep_index_test();

	addresses = malloc((sizeof *addresses) * KEEP_TEST_ALLOC_COUNT);
	printf("KEEP Session create\n");
	session = keep_session_create(1024 * 1024, 0);

	for(i = 0; i < KEEP_TEST_ALLOC_COUNT; i++)
	{
		addresses[i] = keep_allocate(session, KEEP_TEST_ALLOC_SIZE, i);
		buffer = keep_get(session, addresses[i], NULL);
		for(j = 0; j < KEEP_TEST_ALLOC_SIZE; j++)
			buffer[j] = 65 + i % 32;
		keep_return(session, addresses[i], TRUE);
//		printf("address %u %p\n", (uint)addresses[i], buffer);
		if(i % 5 == 0)
			keep_session_service(session);
	}
	for(i = 0; i < KEEP_TEST_ALLOC_COUNT; i++)
	{
		buffer = keep_get(session, addresses[i], NULL);
		if(buffer[0] != 65 + i % 32)
			j = 0;
		keep_return(session, addresses[i], FALSE);
//		if(i % 5 == 0)
//			keep_session_service(session);
//		printf("read %p\n", buffer);
	}
	keep_session_destroy(session);
	printf("KEEP Session destroy\n");
}