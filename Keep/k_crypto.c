#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "forge.h"

#define KEY_SIZE 31 /* I think using prime sized keys might be good*/
#define DATA_SIZE (1024 * 1024 * 16)

/* #define CINT_TYPE_64BIT */
#define CINT_TYPE_32BIT 
/* #define CINT_TYPE_8BIT */

#ifdef CINT_TYPE_64BIT
typedef unsigned __int64 cint;
#endif
#ifdef CINT_TYPE_32BIT
typedef unsigned int cint;
#endif
#ifdef CINT_TYPE_8BIT
typedef unsigned char cint;
#endif

/* Psevdo random number generator, obviusly not secure in the least, but gives us something to work with*/


#define K_RANDOM_NUMBBER_REGISATERS 1031
uint8 random_number_key[K_RANDOM_NUMBBER_REGISATERS];

void k_crypto_rand_init()
{
	uint i, index;
	for(i = 0; i < K_RANDOM_NUMBBER_REGISATERS; i++)
	{
		index = (i << 13) ^ i;
		index = ((index * (index * index * 15731 + 789221) + 1376312589) & 0x7fffffff);
		random_number_key[i] ^= index / 2;
	}
}

void k_crypto_rand_set(uint8 *input, uint size)
{
	static uint pos = 0;
	uint i;
	for(i = 0; i < size; i++)
		random_number_key[pos++ % K_RANDOM_NUMBBER_REGISATERS] ^= input[i];
}

void k_crypto_rand(uint8 *output, uint size)
{
	static uint pos_a = 0, pos_b = 1, pos_c = 2, old_a = 0, counter = 1376312589;
	uint i, sub_key_size;
	sub_key_size = (K_RANDOM_NUMBBER_REGISATERS - 1);
	for(i = 0; i < size; i++)
	{
		old_a = pos_a;
		pos_a = random_number_key[pos_b] % K_RANDOM_NUMBBER_REGISATERS;
		pos_b = (pos_a + 1 + random_number_key[pos_c] % sub_key_size) % K_RANDOM_NUMBBER_REGISATERS;
		pos_c = (pos_a + 1 + random_number_key[old_a] % sub_key_size) % K_RANDOM_NUMBBER_REGISATERS;
		output[i] = random_number_key[pos_a] ^ random_number_key[pos_b];
		random_number_key[pos_c] = (random_number_key[pos_c] << 7) | (random_number_key[pos_c] >> 1);
		random_number_key[pos_a] ^= random_number_key[pos_c];
		random_number_key[pos_b] ^= counter++;
	}
}

/* The actiual cryptography function */

typedef struct{
	cint key_registers[KEY_SIZE];
	uint pos[3];
	cint travel;
}KKey;

void k_key_checksum_init(KKey *key)
{
	uint *buf, i, size;
	size = KEY_SIZE * sizeof(cint) / sizeof(uint);
	buf = (uint *)key->key_registers;
	for(i = 0; i < size; i++)
		buf[i] = f_randi(i);
	key->pos[0] = 0;
	key->pos[1] = 1;
	key->pos[2] = 2;
	key->travel = 0;
}

void k_hash_function(cint *input, uint length, KKey *key)
{
	cint pos_a, pos_b, pos_c, old_a, *k;
	uint i, sub_key_size;
	pos_a = key->pos[0];
	pos_b = key->pos[1];
	pos_c = key->pos[2];
	k = key->key_registers;
	sub_key_size = (KEY_SIZE - 1);
	for(i = 0; i < length; i++)
	{
		old_a = pos_a;
		pos_a = k[pos_b] % KEY_SIZE;
		pos_b = (pos_a + 1 + k[pos_c] % sub_key_size) % KEY_SIZE;
		pos_c = (pos_a + 1 + k[old_a] % sub_key_size) % KEY_SIZE;
#ifdef CINT_TYPE_64BIT
		k[pos_c] = (k[pos_c] << 63) | (k[pos_c] >> 1);
#endif
#ifdef CINT_TYPE_32BIT
		k[pos_c] = (k[pos_c] << 1) | (k[pos_c] >> 31);
#endif
#ifdef CINT_TYPE_8BIT
		key[pos_c] = (k[pos_c] << 7) | (k[pos_c] >> 1);
#endif
		k[pos_a] ^= k[pos_c] ^ input[i];
		k[pos_b] ^= key->travel++;
	}
	key->pos[0] = pos_a;
	key->pos[1] = pos_b;
	key->pos[2] = pos_c;
}



void k_gen_password_hash(cint *key, uint loops, char *pass_string)
{
	cint pos_a, pos_b, pos_c, old_a, *k;
	uint i, sub_key_size, length;
	char *reg;
	pos_a = 0;
	pos_b = 1;
	pos_c = 2;
	k = key;
	for(i = 0; i < KEY_SIZE; i++)
		k[i] = 0;
	reg = (char *)k;
	for(length = 0; pass_string[length]; length++);
	if(length > KEY_SIZE * sizeof(cint))
	{
		for(i = 0; i < length; i++)
			reg[i % (KEY_SIZE * sizeof(cint))] ^= pass_string[i];
	}else
	{
		for(i = 0; i < KEY_SIZE * sizeof(cint); i++)
			reg[i] = pass_string[i % length];
	}
	sub_key_size = (KEY_SIZE - 1);
	for(i = 0; i < loops; i++)
	{
		old_a = pos_a;
		pos_a = k[pos_b] % KEY_SIZE;
		pos_b = (pos_a + 1 + k[pos_c] % sub_key_size) % KEY_SIZE;
		pos_c = (pos_a + 1 + k[old_a] % sub_key_size) % KEY_SIZE;
#ifdef CINT_TYPE_64BIT
		k[pos_c] = (k[pos_c] << 63) | (k[pos_c] >> 1);
#endif
#ifdef CINT_TYPE_32BIT
		k[pos_c] = (k[pos_c] << 1) | (k[pos_c] >> 31);
#endif
#ifdef CINT_TYPE_8BIT
		key[pos_c] = (k[pos_c] << 7) | (k[pos_c] >> 1);
#endif
		k[pos_b] ^= key[pos_a] ^ key[pos_c];
		k[pos_b] ^= i++;
	}
}


void k_crypto_function(cint *output, cint *input, uint length, cint *key, uint key_size, int up)
{
	cint pos_a, pos_b, pos_c, old_a;
	uint i, sub_key_size, a_size[KEY_SIZE], b_size[KEY_SIZE], c_size[KEY_SIZE], combo[KEY_SIZE * KEY_SIZE];
	pos_a = key[0] % key_size;
	pos_b = key[1] % key_size;
	pos_c = key[2] % key_size;

	sub_key_size = (key_size - 1);
	for(i = 0; i < length; i++)
	{
		old_a = pos_a;
		pos_a = key[pos_b] % key_size;
		pos_b = (pos_a + 1 + key[pos_c] % sub_key_size) % key_size;
		pos_c = (pos_a + 1 + key[old_a] % sub_key_size) % key_size;
		output[i] = input[i] ^ key[pos_a] ^ key[pos_b];
#ifdef CINT_TYPE_64BIT
		key[pos_c] = (key[pos_c] << 63) | (key[pos_c] >> 1);
#endif
#ifdef CINT_TYPE_32BIT
		key[pos_c] = (key[pos_c] << 1) | (key[pos_c] >> 31);
#endif
#ifdef CINT_TYPE_8BIT
		key[pos_c] = (key[pos_c] << 7) | (key[pos_c] >> 1);
#endif
		if(up)
			key[pos_a] ^= key[pos_c] ^ output[i]; 
		else
			key[pos_a] ^= key[pos_c] ^ input[i];

		key[i % key_size] ^= i;
	}
}

#define K_CHECKSUM_WRAP 1024

boolean k_file_id_compute(char *file_name, uint8 *check_sum)
{
	cint *buffer, *beginning, *cs;
	FILE *file; 
	KKey key;
	uint size, i;
	file = fopen(file_name, "r");
	if(file == NULL)
		return FALSE;
	k_key_checksum_init(&key);
	beginning = malloc(sizeof(cint) * K_CHECKSUM_WRAP);
/*	size = fread(beginning, sizeof(cint), K_CHECKSUM_WRAP, file);
	if(size < K_CHECKSUM_WRAP)
	{
		for(i = 0; i + size < K_CHECKSUM_WRAP; i += size)
			k_hash_function(beginning, size, &key);
		k_hash_function(beginning, K_CHECKSUM_WRAP - i, &key);
		for(i = 0; i + size < K_CHECKSUM_WRAP; i += size)
			k_hash_function(beginning, size, &key);
		k_hash_function(beginning, K_CHECKSUM_WRAP - i, &key);
		return TRUE;
	}
	k_hash_function(beginning, size, &key);*/
	buffer = malloc(sizeof(cint) * 1024 * 16);
	i = 0;
	while((size = fread(buffer, sizeof(cint), 1024 * 16, file)) != 0)
	{
		k_hash_function(buffer, size, &key);
		i += size;
	}
//	k_hash_function(beginning, K_CHECKSUM_WRAP, &key);
	free(buffer);
	free(beginning);
	for(i = 0; i < KEY_SIZE; i++)
		printf("key[%u] = %u\n", i, key.key_registers[i]);
	return TRUE;
}

void k_record_test();

void k_connection_test(uint node_count, uint friend_count)
{
	uint i, j, k, found = 1, added = 1, *friends, *nodes;
	nodes = malloc((sizeof *nodes) * node_count);
	friends = malloc((sizeof *friends) * node_count * friend_count);
	for(i = 0; i < node_count; i++)
		nodes[i] = -1;
	for(i = 0; i < node_count * friend_count; i++)
		friends[i] = (f_randi(i) / 2) % node_count;

	for(i = 0; i < node_count; i++)
		friends[i * friend_count] = (i + 1) % node_count;


	nodes[0] = 0;
	printf(" Node count %u Friend count %u\n", node_count, friend_count);
	for(i = 0; found < node_count && added != 0; i++)
	{
		added = 0;
		for(j = 0; j < node_count; j++)
		{
			if(nodes[j] == i)
			{
				for(k = 0; k < friend_count; k++)
				{
					if(nodes[friends[j * friend_count + k]] == -1)
					{
						added++;
						nodes[friends[j * friend_count + k]] = i + 1;
					}
				}
			}
		}
		found += added;
//		printf("Gen[%u] added %u found %u\n", i, added, found);
	}
	if(found < node_count)
		printf("Fail\n");
	else
		printf("Complete in %u generations\n", i);
	free(friends);
	free(nodes);
}

int main324()
{
	uint i;
//	for(i = 3; i <= 64; i++)
//		k_connection_test(1000000, i);
	k_record_test();
//	k_file_id_compute("file.bin", NULL);
	return TRUE;
}


int main3(int argc, char **argv)
{
	FILE *f;
	cint key[KEY_SIZE], *decrypted, *encrypted;
	uint i, j, *k, count[256], min, max;

	decrypted = malloc((sizeof *decrypted) * DATA_SIZE);
	encrypted = malloc((sizeof *encrypted) * DATA_SIZE);

	/* lets create a KEY */
	k = (uint *)key;
	for(i = 0; i < KEY_SIZE * (sizeof *key) / (sizeof *k); i++)
		k[i] = f_randi(i);

	/* ecrypt some zeroes (worst case entropy) */
	for(i = 0; i < DATA_SIZE; i++)
		decrypted[i] = 0;
	
	/* Ecrypt */
//	crypto_function(encrypted, decrypted, DATA_SIZE, key, KEY_SIZE, TRUE);

	/* lets re-create the same KEY */
	for(i = 0; i < KEY_SIZE * (sizeof *key) / (sizeof *k); i++)
		k[i] = 0/*randi(i)*/;

	/* Decrypt */
//	crypto_function(decrypted, encrypted, DATA_SIZE, key, KEY_SIZE, FALSE);
/*	for(i = 0; i < DATA_SIZE; i++)
		printf("%u ", encrypted[i]);
	printf("DONE\n");*/
	{
		uint *a = NULL;
		*a = 0;
	}
}
