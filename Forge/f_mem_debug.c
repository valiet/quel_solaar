#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define F_NO_MEMORY_DEBUG
#include "forge.h"

extern void f_debug_mem_print(uint min_allocs);

#define F_MEMORY_OVER_ALLOC 32
#define F_MEMORY_MAGIC_NUMBER 132
typedef struct{
	uint size;
	void *buf;
}STMemAllocBuf;

typedef struct{
	uint line;
	char file[256];
	STMemAllocBuf *allocs;
	uint alloc_count;
	uint alloc_alocated;
	uint size;
	uint alocated;
	uint freed;
}STMemAllocLine;

STMemAllocLine f_alloc_lines[1024];
uint f_alloc_line_count = 0;
void *f_alloc_mutex = NULL;
void (*f_alloc_mutex_lock)(void *mutex) = NULL;
void (*f_alloc_mutex_unlock)(void *mutex) = NULL;


void f_debug_memory_init(void (*lock)(void *mutex), void (*unlock)(void *mutex), void *mutex)
{
	f_alloc_mutex = mutex;
	f_alloc_mutex_lock = lock;
	f_alloc_mutex_unlock = unlock;
}

boolean f_debug_memory()
{
	boolean output = FALSE;
	uint i, j, k;
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_lock(f_alloc_mutex);
	for(i = 0; i < f_alloc_line_count; i++)
	{
		for(j = 0; j < f_alloc_lines[i].alloc_count; j++)
		{
			uint8 *buf;
			uint size;
			buf = f_alloc_lines[i].allocs[j].buf;
			size = f_alloc_lines[i].allocs[j].size;
			for(k = 0; k < F_MEMORY_OVER_ALLOC; k++)
				if(buf[size + k] != F_MEMORY_MAGIC_NUMBER)
					break;
			if(k < F_MEMORY_OVER_ALLOC)
			{
				printf("MEM ERROR: Overshoot at line %u in file %s\n", f_alloc_lines[i].line, f_alloc_lines[i].file);
				{
					uint *X = NULL;
					X[0] = 0;
				}
				output = TRUE;
			}
		}
	}
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_unlock(f_alloc_mutex);
	return output;
}

void f_debug_mem_add(void *pointer, uint size, char *file, uint line)
{
	uint i, j;
	for(i = 0; i < F_MEMORY_OVER_ALLOC; i++)
		((uint8 *)pointer)[size + i] = F_MEMORY_MAGIC_NUMBER;

	for(i = 0; i < f_alloc_line_count; i++)
	{
		if(line == f_alloc_lines[i].line)
		{
			for(j = 0; file[j] != 0 && file[j] == f_alloc_lines[i].file[j] ; j++);
			if(file[j] == f_alloc_lines[i].file[j])
				break;
		}
	}
	if(i < f_alloc_line_count)
	{
		if(f_alloc_lines[i].alloc_alocated == f_alloc_lines[i].alloc_count)
		{
			f_alloc_lines[i].alloc_alocated += 1024;
			f_alloc_lines[i].allocs = realloc(f_alloc_lines[i].allocs, (sizeof *f_alloc_lines[i].allocs) * f_alloc_lines[i].alloc_alocated);
		}
		f_alloc_lines[i].allocs[f_alloc_lines[i].alloc_count].size = size;
		f_alloc_lines[i].allocs[f_alloc_lines[i].alloc_count++].buf = pointer;
		f_alloc_lines[i].size += size;
		f_alloc_lines[i].alocated++;
	}else
	{
		if(i < 1024)
		{
			f_alloc_lines[i].line = line;
			for(j = 0; j < 255 && file[j] != 0; j++)
				f_alloc_lines[i].file[j] = file[j];
			f_alloc_lines[i].file[j] = 0;
			f_alloc_lines[i].alloc_alocated = 256;
			f_alloc_lines[i].allocs = malloc((sizeof *f_alloc_lines[i].allocs) * f_alloc_lines[i].alloc_alocated);
			f_alloc_lines[i].allocs[0].size = size;
			f_alloc_lines[i].allocs[0].buf = pointer;
			f_alloc_lines[i].alloc_count = 1;
			f_alloc_lines[i].size = size;
			f_alloc_lines[i].freed = 0;
			f_alloc_lines[i].alocated++;
			f_alloc_line_count++;
		}
	}
}

void *f_debug_mem_malloc(uint size, char *file, uint line)
{
	void *pointer;
	uint i;
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_lock(f_alloc_mutex);
	pointer = malloc(size + F_MEMORY_OVER_ALLOC);

	if(pointer == NULL)
	{
		printf("MEM ERROR: Malloc returns NULL when trying to allocate %u bytes at line %u in file %s\n", size, line, file);
		if(f_alloc_mutex != NULL)
			f_alloc_mutex_unlock(f_alloc_mutex);
		f_debug_mem_print(0);
		exit(0);
	}
	for(i = 0; i < size + F_MEMORY_OVER_ALLOC; i++)
 		((uint8 *)pointer)[i] = F_MEMORY_MAGIC_NUMBER + 1;
	f_debug_mem_add(pointer, size, file, line);
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_unlock(f_alloc_mutex);
	return pointer;
}

boolean f_debug_mem_remove(void *buf)
{
	uint i, j, k;
	for(i = 0; i < f_alloc_line_count; i++)
	{
		for(j = 0; j < f_alloc_lines[i].alloc_count; j++)
		{
			if(f_alloc_lines[i].allocs[j].buf == buf)
			{
				for(k = 0; k < F_MEMORY_OVER_ALLOC; k++)
					if(((uint8 *)buf)[f_alloc_lines[i].allocs[j].size + k] != F_MEMORY_MAGIC_NUMBER)
						break;
				if(k < F_MEMORY_OVER_ALLOC)
					printf("MEM ERROR: Overshoot at line %u in file %s\n", f_alloc_lines[i].line, f_alloc_lines[i].file);
				f_alloc_lines[i].size -= f_alloc_lines[i].allocs[j].size;
				f_alloc_lines[i].allocs[j] = f_alloc_lines[i].allocs[--f_alloc_lines[i].alloc_count];
				f_alloc_lines[i].freed++;
				return TRUE;
			}
		}	
	}
	return FALSE;
}

void f_debug_mem_free(void *buf)
{
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_lock(f_alloc_mutex);
	if(!f_debug_mem_remove(buf))
	{
		uint *X = NULL;
		X[0] = 0;
	}
	free(buf);
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_unlock(f_alloc_mutex);
}


void *f_debug_mem_realloc(void *pointer, uint size, char *file, uint line)
{
	uint i, j, k, move;
	void *pointer2;
	if(pointer == NULL)
		return f_debug_mem_malloc( size, file, line);
	
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_lock(f_alloc_mutex);
	for(i = 0; i < f_alloc_line_count; i++)
	{
		for(j = 0; j < f_alloc_lines[i].alloc_count; j++)
			if(f_alloc_lines[i].allocs[j].buf == pointer)
				break;
		if(j < f_alloc_lines[i].alloc_count)
			break;
	}
	if(i == f_alloc_line_count)
	{
		printf("FORGE Mem debugger error. Trying to reallocate pointer %p in %s line %u. Pointer has never beein allocated\n", pointer, file, line);
		for(i = 0; i < f_alloc_line_count; i++)
		{
			for(j = 0; j < f_alloc_lines[i].alloc_count; j++)
			{
				uint *buf;
				buf = f_alloc_lines[i].allocs[j].buf;
				for(k = 0; k < f_alloc_lines[i].allocs[j].size; k++)
				{
					if(&buf[k] == pointer)
					{
						printf("Trying to reallocate pointer %u bytes (out of %u) in to allocation made in %s on line %u.\n", k, f_alloc_lines[i].allocs[j].size, f_alloc_lines[i].file, f_alloc_lines[i].line);
					}
				}
			}
		}
		exit(0);
	}
	move = f_alloc_lines[i].allocs[j].size;
	
	if(move > size)
		move = size;

	pointer2 = malloc(size + F_MEMORY_OVER_ALLOC);
	if(pointer2 == NULL)
	{
		printf("MEM ERROR: Realloc returns NULL when trying to allocate %u bytes at line %u in file %s\n", size, line, file);
		if(f_alloc_mutex != NULL)
			f_alloc_mutex_unlock(f_alloc_mutex);
		f_debug_mem_print(0);
		exit(0);
	}
	for(i = 0; i < size + F_MEMORY_OVER_ALLOC; i++)
 		((uint8 *)pointer2)[i] = F_MEMORY_MAGIC_NUMBER + 1;
	memcpy(pointer2, pointer, move);

	f_debug_mem_add(pointer2, size, file, line);
	f_debug_mem_remove(pointer);
	free(pointer);

	if(f_alloc_mutex != NULL)
		f_alloc_mutex_unlock(f_alloc_mutex);
	return pointer2;
}

void f_debug_mem_print(uint min_allocs)
{
	uint i;
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_lock(f_alloc_mutex);
	printf("Memory repport:\n----------------------------------------------\n");
	for(i = 0; i < f_alloc_line_count; i++)
	{
		if(min_allocs < f_alloc_lines[i].alocated)
		{
			printf("%s line: %u\n",f_alloc_lines[i].file, f_alloc_lines[i].line);
			printf(" - Bytes allocated: %u\n - Allocations: %u\n - Frees: %u\n\n", f_alloc_lines[i].size, f_alloc_lines[i].alocated, f_alloc_lines[i].freed);
		}
	}
	printf("----------------------------------------------\n");
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_unlock(f_alloc_mutex);
}

uint32 f_debug_mem_consumption()
{
	uint i, sum = 0;

	if(f_alloc_mutex != NULL)
		f_alloc_mutex_lock(f_alloc_mutex);
	for(i = 0; i < f_alloc_line_count; i++)
		sum += f_alloc_lines[i].size;
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_unlock(f_alloc_mutex);
	return sum;
}

void f_debug_mem_reset()
{
	uint i;
	if(f_alloc_mutex != NULL)
		f_alloc_mutex_lock(f_alloc_mutex);
	for(i = 0; i < f_alloc_line_count; i++)
		free(f_alloc_lines[i].allocs);
	f_alloc_line_count = 0;

	if(f_alloc_mutex != NULL)
		f_alloc_mutex_unlock(f_alloc_mutex);
}

void exit_crash(uint i)
{
	uint *a = NULL;
	a[0] = 0;
}