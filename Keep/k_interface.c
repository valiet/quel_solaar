#include <math.h>
#include <stdio.h>
#include "forge.h"
#include "imagine.h"
#include "keep.h"

#define KEY_SIZE 31
#define MODULE_NAME_SIZE 64

typedef enum{
	K_OM_CONNECT,
	K_OM_GAP,
	K_OM_DISCONNECT
}KObfuscationMode;

typedef struct{
	uint64 charcksum[KEY_SIZE];
	char name[MODULE_NAME_SIZE];
	ILib *lib;
	char path[1024];
	void (*translate_func)(KRecord *record, void *state);
	void (*index_func)(KRecord *record, void *state);
	void (*rank_func)(uint64 *input, uint input_length, uint64 *output, uint output_length);
	uint8 *(*obfuscate_func)(KObfuscationMode command);
}KPlugin;


KPlugin		*k_global_plugins = NULL;
uint		k_global_plugin_count = 0;
IInterface 	*k_global_plugg_interface = NULL;

void k_interface_name(char *name)
{
	uint i;
	for(i = 0; name[i] != 0 && i < MODULE_NAME_SIZE - 1; i++)
		k_global_plugins[k_global_plugin_count].name[i] = name[i];
	k_global_plugins[k_global_plugin_count].name[i] = 0;
}

void k_interface_index_string(void *state, char *string)
{
}

void k_interface_store_func_set(void (*func)(KRecord *record, void *state))
{
}

void k_interface_rank_func_set(void (*func)(uint64 *input, uint input_length, uint64 *output, uint output_length))
{
}


void k_interface_obfuscate_func_set(uint8 *(*func)(KObfuscationMode command))
{
}



void k_interface_init()
{
	uint i;	

	k_global_plugg_interface = imagine_library_interface_create();
	imagine_library_interface_register(k_global_plugg_interface, k_interface_name, "betray_plugin_callback_set_main");
	imagine_library_interface_register(k_global_plugg_interface, k_interface_index_string, "betray_plugin_callback_set_main");
	imagine_library_interface_register(k_global_plugg_interface, k_interface_store_func_set, "betray_plugin_callback_set_main");
	imagine_library_interface_register(k_global_plugg_interface, k_interface_rank_func_set, "betray_plugin_callback_set_main");
	imagine_library_interface_register(k_global_plugg_interface, k_interface_obfuscate_func_set, "betray_plugin_callback_set_main");


	for(i = 0; imagine_path_search("." /*the lack of comma is intentional*/ IMAGINE_LIBRARY_EXTENTION, TRUE, IMAGINE_DIR_HOME_PATH, FALSE, i, k_global_plugins[k_global_plugin_count].path, 1024); i++)
	{
		if(k_file_id_compute(k_global_plugins[k_global_plugin_count].path, k_global_plugins[k_global_plugin_count].charcksum))
		{
			k_global_plugins[k_global_plugin_count].index_func = NULL;
			k_global_plugins[k_global_plugin_count].rank_func = NULL;
			k_global_plugins[k_global_plugin_count].obfuscate_func = NULL;

			k_global_plugins[k_global_plugin_count].lib = imagine_library_load(k_global_plugins[k_global_plugin_count].path, k_global_plugg_interface, "Unravel Plugin");
			if(NULL != k_global_plugins[k_global_plugin_count].lib)
			{
				k_global_plugin_count++;
			}
		}
	}
}

#define PASSWORD_CYCLES (1024 * 1024 * 512)
#define KEY_SIZE 31
typedef unsigned int cint;
void k_gen_password_hash(cint *key, uint loops, char *pass_string);

boolean k_interface_account_login(char *string)
{
	cint key[KEY_SIZE];
	uint i, j;
	char *pass_string = "EskilSteenberg";
	k_gen_password_hash(key, 0, pass_string);
	for(i = 0; i < KEY_SIZE; i++)
		printf("key[%u] %u\n", i, key[i]);
//	for(j = 0; j < 1000 * 1000; j++)
	j = 1000 * 1000 * 500;
	{
		k_gen_password_hash(key, PASSWORD_CYCLES, pass_string);
		system("cls");
		printf("%u\n", PASSWORD_CYCLES);
		for(i = 0; i < KEY_SIZE; i++)
			printf("key[%u] %u\n", i, key[i]);
	}
}

KRecord *k_interface_account_create(KSession *session, char *pass_string)
{
	cint key[KEY_SIZE];
	uint64 address, tmp;
	uint i, count;
	void *data;
	k_gen_password_hash(key, PASSWORD_CYCLES, pass_string);
	if(NULL == keep_get(session, key, NULL))
		return NULL;
	address = keep_allocate(session, sizeof(uint64), 0);

	k_crypto_rand(&count, (sizeof count));
	count = count % 9;
	for(i = 0; i < count; i++)
	{
		address = keep_allocate(session, sizeof(uint64), 0);
		data = keep_get(session, address, NULL);
		k_crypto_rand(data, sizeof(uint64));
		keep_return(session, address, TRUE);
	}
/*
extern KSession	*keep_session_create(uint64 memory_footprint, uint64 user_id);
extern boolean	keep_session_destroy(KSession *session);
extern boolean	keep_session_service(KSession *session);
extern void		*keep_get(KSession *session, uint64 address, uint *size);
extern void		keep_return(KSession *session, uint64 address, boolean modifyed);
extern uint64	keep_allocate(KSession *session, uint64 size, uint64 seed);
extern void		keep_free(KSession *session, uint64 address);
*/
}


int main_login()
{
	uint32 time[2], t;
	k_crypto_rand_init();
	imagine_current_time_get(time, &time[1]);
	t = time[0];
	while(t + 2 > time[0])
	{
		k_crypto_rand_set((uint8 *)time, sizeof(uint32) * 2);
		k_crypto_rand((uint8 *)time, sizeof(uint32) * 2);
		imagine_current_time_get(time, &time[1]); 
	}
//	k_interface_login("EskilSteenberg");
	return TRUE;
}