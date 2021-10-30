#include "e_includes.h"
#include "forge.h"
#include "e_types.h"

#define E_ARRAY_CHUNK_SIZE 64

void e_dlut_init(EDynLookUpTable *table)
{
	void **data, **end;
	table->data = malloc((sizeof *table->data) * E_ARRAY_CHUNK_SIZE);
	table->allocated = E_ARRAY_CHUNK_SIZE;
	data = table->data;
	for(end = data + E_ARRAY_CHUNK_SIZE; data < end; data++)
		*data = NULL;
}

void e_dlut_free(EDynLookUpTable *table)
{
	free(table->data);	
}

void *e_dlut_get(EDynLookUpTable *table, uint key)
{
	if(key < table->allocated)
		return table->data[key];
	return NULL;
}

void *e_dlut_next(EDynLookUpTable *table, uint key)
{
	uint i;
	for(i = key; i < table->allocated && table->data[i] == NULL; i++);
	if(i >= table->allocated)
		return NULL;
	return table->data[i];
}

uint e_dlut_next_empty(EDynLookUpTable *table, uint key)
{
	uint i;
	for(i = key; i < table->allocated && table->data[i] != NULL; i++);
	return i;
}

void *e_dlut_add(EDynLookUpTable *table, uint key, void *pointer)
{
	void **data, **end;
	if(key < table->allocated)
	{
		data = table->data[key];
		table->data[key] = pointer;
		return data;
	}
	table->data = realloc(table->data,(sizeof *table->data) * (key + E_ARRAY_CHUNK_SIZE));
	data = table->data + table->allocated;
	table->allocated = key + E_ARRAY_CHUNK_SIZE;
	for(end = table->data + table->allocated; data < end; data++)
		*data = NULL;
	table->data[key] = pointer;
	return NULL;
}

uint e_dlut_add_in_empty(EDynLookUpTable *table, void *pointer)
{
	uint id;
	id = e_dlut_next_empty(table, 0);
	e_dlut_add(table, id, pointer);
	return id;
}

void e_dlut_remove(EDynLookUpTable *table, uint key)
{
	if(key < table->allocated)
	{
		table->data[key] = NULL;
	}
}

uint e_dlut_count(EDynLookUpTable *table)
{
	uint i, count = 0; 
	for(i = 0; i < table->allocated; i++)
		if(table->data[i] != NULL)
			count++;
	return count;
}

void e_dlut_clean(EDynLookUpTable *table)
{
	int i;
	for(i = table->allocated - 1; i > 1 && table->data[i] == NULL; i--);
	if(i + E_ARRAY_CHUNK_SIZE < table->allocated)
	table->data = realloc(table->data,(sizeof *table->data) * (i + E_ARRAY_CHUNK_SIZE));
}

void e_dlut_remove_foreach(EDynLookUpTable *table, void (*func)(uint key, void *pointer, void *user_data), void *user_data)
{
	uint i;
	void **data;
	data = table->data;
	for(i = 0; i < table->allocated; i++)
		if(data[i] != NULL)
			func(i, data[i], user_data);
	free(table->data);	
}
