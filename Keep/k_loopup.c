#include <math.h>
#include <stdio.h>
#include "forge.h"
#include "imagine.h"
typedef unsigned long long uint64;
#include "keep.h"

typedef enum{
	K_I_ADDRESS,
	K_I_TEXT,
	K_I_TIME,
	K_I_LOCATION,
	K_I_COUNT
}KIntex;

typedef struct{
	uint64 address[256];
	uint64 list;
}KIndexAdress;

uint64 keep_index_text_allocate(KSession *session)
{
	uint64 address;
	KIndexAdress *index;
	uint i;
	address = keep_allocate(session, (sizeof *index), 0);
	index = keep_get(session, address, NULL);
	for(i = 0; i < 256; i++)
		index->address[i] = 0;
	index->address[i] = 0;
	index->list = 0;
	keep_return(session, address, TRUE);
	return address;
}



uint64 keep_index_text_add_recursivly(KSession *session, unsigned char *string, uint letter)
{
	uint64 address;
	KIndexAdress *index;
	uint64 *pointer;
	uint i;
	address = keep_allocate(session, (sizeof *index), 0);
	index = keep_get(session, address, NULL);
//	printf("index = %p Address %u\n", index, (uint)address);
	for(i = 0; i < 256; i++)
		index->address[i] = 0;
	if(string[letter] != 0)
	{
		index->address[string[letter]] = keep_index_text_add_recursivly(session, string, letter + 1);
//		printf("index = %p Address %u wrote %u to %u\n", index, (uint)address, (uint)index->address[string[letter]], (uint)string[letter]);	
		index->list = 0;
	}
	index->address[i] = 0;
	keep_return(session, address, TRUE);
	return address;
}

void keep_index_text_add(KSession *session, uint64 root, unsigned char *string, uint64 element)
{
	KIndexAdress *index;
	uint i, length, size;
	uint64 branch, next, *list, *new_list;


	branch = next = root;



	for(length = 0; string[length] != 0; length++);
	for(i = 0; i < length; i++)
	{
		index = keep_get(session, branch, NULL);
		if(index->address[string[i]] == 0)
			break;
		next = index->address[string[i]];
		keep_return(session, branch, FALSE);
		branch = next;
	}
	if(i < length)
	{
		index->address[string[i]] = keep_index_text_add_recursivly(session, string, i + 1);
		for(; i < length; i++)
		{
			next = index->address[string[i]];
			keep_return(session, branch, FALSE);
			branch = next;
			index = keep_get(session, branch, NULL);
		}
	}else
		index = keep_get(session, branch, NULL);
	if(index->list != 0)
	{


		list = keep_get(session, index->list, &size);
		size /= sizeof(uint64);

		for(i = 0; i < size && list[i] != 0; i++);

		if(i < size)
		{
			for(; i != 0 && list[i - 1] > element; i--)
			{
				list[i] = list[i - 1];
			}
			list[i] = element;
			keep_return(session, index->list, TRUE);
			keep_return(session, branch, FALSE);
		}else
		{
			next = keep_allocate(session, (sizeof *list) * (4 + size), 0);
			new_list = keep_get(session, next, NULL);
			for(i = 0; i < size && list[i] < element; i++)
				new_list[i] = list[i];
			new_list[i] = element;
			for(; i < size; i++)
				new_list[i + 1] = list[i];
			for(i++; i < 4 + size; i++)
				new_list[i] = 0;
			keep_free(session, index->list);
			index->list = next;
			keep_return(session, next, TRUE);
			keep_return(session, branch, FALSE);

		}
	}else
	{
		index->list = next = keep_allocate(session, (sizeof *list) * 4, 0);
		keep_return(session, branch, TRUE);
		list = keep_get(session, next, NULL);
		list[0] = element;
		for(i = 1; i < 4; i++)
			list[i] = 0;
		keep_return(session, next, TRUE);
	}
}

uint64 keep_index_text_lookup(KSession *session, uint64 root, unsigned char *string)
{
	KIndexAdress *index;
	uint i, length, size;
	uint64 branch, next, output;
	branch = root;
	index = keep_get(session, branch, NULL);
	for(i = 0; string[i] != 0; i++)
	{
		next = index->address[string[i]];
		if(next == 0)
		{
			keep_return(session, branch, FALSE);
			return 0;
		}
		keep_return(session, next, FALSE);
		next = index->address[string[i]];
		index = keep_get(session, next, NULL);
		branch = next;
	}
	output = index->list;
	keep_return(session, next, FALSE);
	return output;
}

boolean keep_index_list(KSession *session, uint64 root, char *path, uint generation)
{
	uint64 address;
	char directory_path[1024];
	char list[128], *copy;
	IDir *dir;
	uint i, length; 
	dir = imagine_path_open(path);
	if(dir == NULL)
		return FALSE;
	while(imagine_path_next(dir, list, 128))
	{
		if(list[0] != '.')
		{
			sprintf(directory_path, "%s%s/", path, list);
			// printf("path: %s\n", list); 
			if(!keep_index_list(session, root, directory_path, generation + 1))
			{
				for(length = 0; directory_path[length] != 0; length++);
				address = keep_allocate(session, length + 1, 0);
				copy = keep_get(session, address, NULL);
				for(i = 0; directory_path[i] != 0; i++)
					copy[i] = directory_path[i];
				copy[i] = 0;
				keep_return(session, address, TRUE);

				for(length = 0; list[length] != 0; length++);
				for(i = 0; i < length; i++)
				{
					if(65 <= list[i] && 90 >= list[i])
						list[i] += 32;
					if(48 > list[i] || 122 < list[i] || (57 < list[i] && 97 > list[i]))
						list[i] = 0;
				}
				for(i = 0; i < length; i++)
				{
					if(list[i] != 0)
					{
						keep_index_text_add(session, root, &list[i], address);
					//	printf("words: %s\n", &list[i]);
						while(list[i] != 0)
							i++;
					}
				}
			}
		}
	}
	imagine_path_close(dir);
	return TRUE;
}

void keep_index_test()
{
	KSession *session;
	char *keys[4] = {"ABC", "123", "Hej", "Hopp"}, *path;
	uint64 root, address, *read;
	uint i, j, size;
	session = keep_session_create(1024 * 1024, 0);
	root = keep_index_text_allocate(session);
//	root = 256;
/*	for(i = 0; i < 4; i++)
	{
		keep_index_text_add(session, root, keys[i], i + 1);
		
	}
	for(i = 0; i < 4; i++)
	{
		address = keep_index_text_lookup(session, root, keys[i]);
		printf("lookup[%u] = %u\n", i, (uint)address);
		read = keep_get(session, address, NULL);
		printf("read = %u\n", (uint)read[0]);
		keep_return(session, address, FALSE);
	}*/
//	keep_index_list(session, root, "D:/download/", 0);
	keep_index_list(session, root, "D:/", 0);
	printf("\n\n-------------------------------------\n\n");
	for(i = 0; i < 1; i++)
	{
		address = keep_index_text_lookup(session, root, "homeworld");
		if(address != 0)
		{
			read = keep_get(session, address, &size);
			printf("lookup[%u] = %u size %u\n", (uint)address, (uint)read, size);
			for(j = 0; j < size / 8 && read[j] != 0; j++)
				printf("List[%u] = %u\n", j,  (uint)read[j]);

			for(j = 0; j < size / 8 && read[j] != 0; j++)
			{
				path = keep_get(session, read[j], NULL);
				printf("read = %s\n", path);
				keep_return(session, read[j], FALSE);
			}
			keep_return(session, address, FALSE);
		}
	}
	keep_session_destroy(session);
	i = 0;
	exit(0);
}