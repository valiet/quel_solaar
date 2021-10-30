#include <stdlib.h>
#include <stdio.h>
#include "forge.h"

uint *f_sort_ids(uint length, uint *ids, boolean (*compate_func)(uint biger, uint smaller, void *user), void *user)
{
	uint i, size = 1, *a, *b, *c, pos0, pos1, end0, end1, put;
	a = malloc((sizeof *a) * length);
	b = malloc((sizeof *b) * length);
	if(ids == NULL)
	{
		for(i = 0; i < length; i++)
			a[i] = i;
	}else
		for(i = 0; i < length; i++)
			a[i] = ids[i];

	while(size < length)
	{
		put = 0;
		for(i = 0; i < length; i += size * 2)
		{
			pos0 = i; 
			pos1 = i + size;
			end0 = pos1;
			end1 = pos1 + size;
			if(end0 > length)
				end0 = length;
			if(end1 > length)
				end1 = length;
			while(pos0 < end0 && pos1 < end1)
			{
				if(compate_func(a[pos0], a[pos1], user))
					b[put++] = a[pos1++];  
				else
					b[put++] = a[pos0++];  
			}
			while(pos0 < end0)
				b[put++] = a[pos0++];
			while(pos1 < end1)
				b[put++] = a[pos1++];
		}
		c = b;
		b = a;
		a = c;
		size *= 2;
	}
	free(b);
	return a;
}


void **f_sort_pointers(uint length, void **pointers, boolean (*compate_func)(void *biger, void *smaller, void *user), void *user)
{
	uint i, size = 1, pos0, pos1, end0, end1, put;
	void **a, **b, **c;
	a = malloc((sizeof *a) * length);
	b = malloc((sizeof *b) * length);
	
	for(i = 0; i < length; i++)
		a[i] = pointers[i];

	while(size < length)
	{
		put = 0;
		for(i = 0; i < length; i += size * 2)
		{
			pos0 = i; 
			pos1 = i + size;
			end0 = pos1;
			end1 = pos1 + size;
			if(end0 > length)
				end0 = length;
			if(end1 > length)
				end1 = length;
			while(pos0 < end0 && pos1 < end1)
			{
				if(compate_func(a[pos0], a[pos1], user))
					b[put++] = a[pos1++];  
				else
					b[put++] = a[pos0++];  
			}
			while(pos0 < end0)
				b[put++] = a[pos0++];
			while(pos1 < end1)
				b[put++] = a[pos1++];
		}
		c = b;
		b = a;
		a = c;
		size *= 2;
	}
	free(b);
	return a;
}

boolean compate_func(uint big, uint small, void *user)
{
	float *array;
	array = user;
	return array[big] > array[small];
}

void f_sort_test()
{
	float array[100];
	uint *sorted, i;
	for(i = 0; i < 100; i++)
		array[i] = f_randf(i);
	sorted = f_sort_ids(100, NULL, compate_func, array);

	for(i = 0; i < 100; i++)
		printf("sort[%u] %u = %f\n", i, sorted[i], array[sorted[i]]);
	exit(0);
}


uint *f_sort_quad_tri_neighbor(uint *ref, uint quad_length, uint tri_length, uint vertex_count)
{
	uint i, cor, clear = 0, *n, *v, a, b;
	uint counter = 0, laps = 0;
	n = malloc((sizeof *n) * (quad_length + tri_length));
	for(i = 0; i < (quad_length + tri_length); i++)
		n[i] = -1;
	v = malloc((sizeof *v) * vertex_count);
	for(i = 0; i < vertex_count; i++)
		v[i] = -1;
	while(clear < quad_length + tri_length)
	{
		for(i = 0; i < quad_length && clear < quad_length + tri_length; i++)
		{
			counter++;
			cor = v[ref[i]];
			if(cor == -1)
			{
				if(n[i] == -1 || n[(i / 4) * 4 + (i + 3) % 4] == -1)
					v[ref[i]] = i;
		//		else
		//			printf("jump!");
			}
			else if(cor == i)
				v[ref[i]] = -1;
			else
			{
				if(cor >= quad_length)
				{	/* other poly is a tri */
					a = (i / 4) * 4;
					b = quad_length + ((cor - quad_length) / 3) * 3;
					if((n[cor] == -1 && n[a + (i + 3) % 4] == -1) && ref[a + (i + 3) % 4] == ref[b + (cor - b + 1) % 3])
					{
						n[a + (i + 3) % 4] = cor;
						n[cor] = a + (i + 3) % 4;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[b + (cor - b + 2) % 3] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
					}
					if((n[i] == -1 && n[b + (cor - b + 2) % 3] == -1) && ref[a + (i + 1) % 4] == ref[b + (cor - b + 2) % 3])
					{
						n[i] = b + (cor - b + 2) % 3;						
						n[b + (cor - b + 2) % 3] = i;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[cor] != -1)
						{
							if(n[a + (i + 3) % 4] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
				}else
				{	
					/* other poly is a quad */
					a = (i / 4) * 4;
					b = (cor / 4) * 4;
					if((n[cor] == -1 && n[a + (i + 3) % 4] == -1) && ref[a + (i + 3) % 4] == ref[b + (cor + 1) % 4])
					{
						n[a + (i + 3) % 4] = cor;
						n[cor] = a + (i + 3) % 4;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;	
//						v[ref[i]] = -1;
						if(n[b + (cor + 3) % 4] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
					}
					if((n[i] == -1 && n[b + (cor + 3) % 4] == -1) && ref[a + (i + 1) % 4] == ref[b + (cor + 3) % 4])
					{
						n[i] = b + (cor + 3) % 4;
						n[b + (cor + 3) % 4] = i;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;	
						if(n[cor] != -1)
						{
							if(n[a + (i + 3) % 4] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
					}
				}						
			}
			clear++;
		}
		for(; i < quad_length + tri_length && clear < quad_length + tri_length; i++)
		{
			cor = v[ref[i]];
			if(cor == -1)
			{
			//	if(ncor == -1)
				v[ref[i]] = i;
			}
			else if(cor == i)
				v[ref[i]] = -1;
			else 
			{
				if(cor >= quad_length)
				{	/* other poly is a tri */
					a = quad_length + ((i - quad_length) / 3) * 3;
					b = quad_length + ((cor - quad_length) / 3) * 3;
					if((n[cor] == -1 && n[a + (i - a + 2) % 3] == -1) && ref[a + (i - a + 2) % 3] == ref[b + (cor - b + 1) % 3])
					{
						n[a + (i - a + 2) % 3] = cor;
						n[cor] = a + (i - a + 2) % 3;
		//				printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[b + (cor - b + 2) % 3] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
					if((n[i] == -1 && n[b + (cor - b + 2) % 3] == -1) && ref[a + (i - a + 1) % 3] == ref[b + (cor - b + 2) % 3])
					{
						n[i] = b + (cor - b + 2) % 3;						
						n[b + (cor - b + 2) % 3] = i;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[cor] != -1)
						{
							if(n[a + (i - a + 2) % 3] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
				}else
				{
					/* other poly is a quad */
					a = quad_length + ((i - quad_length) / 3) * 3;
					b = (cor / 4) * 4;
					if((n[cor] == -1 && n[a + (i - a + 2) % 3] == -1) && ref[a + (i - a + 2) % 3] == ref[b + (cor + 1) % 4])
					{
						n[a + (i - a + 2) % 3] = cor;
						n[cor] = a + (i - a + 2) % 3;
//						printf("i = %u clear = %u\n", i, clear); 
						clear = 0;
						if(n[b + (cor + 3) % 4] != -1)
						{
							if(n[i] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
					if((n[i] == -1 && n[(cor - b + 3) % 4] == -1) && ref[a + (i - a + 1) % 3] == ref[b + (cor + 3) % 4])
					{
						n[i] = b + (cor + 3) % 4;
						n[b + (cor + 3) % 4] = i;				
//						printf("i = %u clear = %u\n", i, clear); 						
						clear = 0;
						if(n[cor] != -1)
						{
							if(n[a + (i - a + 2) % 3] == -1)
								v[ref[i]] = i;
							else
								v[ref[i]] = -1;
						}
//						v[ref[i]] = -1;
					}
				}						
			}
			counter++;
			clear++;
		}
		laps++;
		
	}
	counter = 0;
	free(v);
	return n;
}



uint *f_sort_tri_neighbor(uint *ref, uint tri_length, uint vertex_count)
{
	uint i, cor, clear = 0, *n, *v, a, b;
	uint counter = 0, laps = 0;
	n = malloc((sizeof *n) * tri_length);
	for(i = 0; i < tri_length; i++)
		n[i] = -1;
	v = malloc((sizeof *v) * vertex_count);
	for(i = 0; i < vertex_count; i++)
		v[i] = -1;
	while(clear < tri_length)
	{
		for(i = 0; i < tri_length && clear < tri_length; i++)
		{
			counter++;
			clear++;
			cor = v[ref[i]];
			if(cor == -1)
			 {
				 if(n[i] == -1 || n[(i / 3) * 3 + (i + 2) % 3] == -1)
					v[ref[i]] = i;
			}else if(cor == i)
				v[ref[i]] = -1;
			else 
			{
				a = (i / 3) * 3;
				b = (cor / 3) * 3;
				if((n[cor] == -1 && n[a + (i + 2) % 3] == -1) && ref[a + (i + 2) % 3] == ref[b + (cor + 1) % 3])
				{
					n[a + (i + 2) % 3] = cor;
					n[cor] = a + (i + 2) % 3;
					clear = 0;
					if(n[b + (cor + 2) % 3] != -1)
					{
						if(n[i] == -1)
							v[ref[i]] = i;
						else
							v[ref[i]] = -1;
					}
				}
				if((n[i] == -1 && n[b + (cor + 2) % 3] == -1) && ref[a + (i + 1) % 3] == ref[b + (cor + 2) % 3])
				{
					n[i] = b + (cor + 2) % 3;						
					n[b + (cor + 2) % 3] = i;
					clear = 0;
					if(n[cor] != -1)
					{
						if(n[a + (i + 2) % 3] == -1)
							v[ref[i]] = i;
						else
							v[ref[i]] = -1;
					}
				}
			}
		}
		laps++;
	}
	counter = 0;
	free(v);
	return n;
}
