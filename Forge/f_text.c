
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "forge.h"

uint f_find_next_word(char *text)
{
	uint i;
	for(i = 1; text[i] > 32; i++);
	return i;
}

boolean f_find_word_compare(char *text_a, char *text_b)
{
	uint i;
	for(i = 0; text_a[i] == text_b[i] && text_a[i] > 32 && text_b[i] > 32; i++);
	return (text_a[i] < 32 && text_b[i] < 32);
}

boolean f_text_compare(char *text_a, char *text_b)
{
	uint i;
	for(i = 0; text_a[i] == text_b[i] && text_a[i] != 0; i++);
	return text_a[i] == text_b[i];
}

uint f_text_copy(uint length, char *dest, char *source)
{
	uint i;
	for(i = 0; i < length && source[i] != 0; i++)
		dest[i] = source[i];
	if(i == length)
		return 0;
	dest[i] = source[i];
	return i;
}


char *f_text_copy_allocate(char *source)
{
	uint i;
	char *text;
	for(i = 0; source[i] != 0; i++);
	text = malloc((sizeof *text) * (i + 1));
	for(i = 0; source[i] != 0; i++)
		text[i] = source[i];
	text[i] = 0;
	return text;
}

uint f_word_copy(uint length, char *dest, char *source)
{
	uint i = 0, j;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] > 32)
		dest[i++] = source[j++];
	if(i == length)
		return 0;
	dest[i] = 0;
	return j;
}


uint f_text_copy_until(uint length, char *dest, char *source, char *until)
{
	uint i = 0, j, k, l;
	for(j = 0; j < length && source[j] < 32; j++);
	while( i < length && source[j] != 0)
	{
		for(k = 0; source[j + k] == until[k] && until[k] != 0 ; k++);
		if(until[k] == 0)
		{
			for(i--; dest[i] < 32; i--);
			dest[i] = 0;
			return j + k;
		}
		else
			dest[i++] = source[j++];
	}
	return 0;
}

boolean	f_text_filter(char *text, char *filter)
{
	uint i, j;
	for(i = 0; text[i] != 0; i++)
	{
		if(text[i] == *filter)
		{
			for(j = 1; text[j] != 0 && text[i + j] == filter[j]; j++);
			if(filter[j] == 0)
				return TRUE;
		}
	}
	return FALSE;
}