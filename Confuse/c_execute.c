#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "forge.h"
#include "c_internal.h"
#include "imagine.h"

extern uint c_primitive_count;
extern CDrawState *c_primitives;

uint confuse_component_add(CSession *session, char *name, CTypeType *param_types, char **param_names, uint param_count, CTypeType output, uint primitive, void (*exe_func)(CSession *session, CInstance *instance, CComponent *c, uint primitive), CComponentCategory type, CSpecialComponentType special, void (*display_func)(CSession *session, CInstance *instance, CComponent *c, uint primitive))
{
	uint i;
	if(session->component_list_count % 16 == 0)
		session->component_list = realloc(session->component_list, (sizeof *session->component_list) * (session->component_list_count + 16));
	for(i = 0; i < 31 && name[i] != 0; i++)
		session->component_list[session->component_list_count].name[i] = name[i];
	session->component_list[session->component_list_count].name[i] = 0;
	session->component_list[session->component_list_count].type = type;
	session->component_list[session->component_list_count].param_types = param_types;
	session->component_list[session->component_list_count].param_names = param_names;
	session->component_list[session->component_list_count].param_count = param_count;
	session->component_list[session->component_list_count].output = output;
	session->component_list[session->component_list_count].primitive = primitive;
	session->component_list[session->component_list_count].exe_func = exe_func;
	session->component_list[session->component_list_count].display_func = display_func;
	session->component_list[session->component_list_count].special = special;
	session->component_list_count++;
	return session->component_list_count - 1;
}

void confuse_component_remove(CSession *session, uint id)
{
	uint i, j;
	free(session->component_list[id].param_types);
	free(session->component_list[id].param_names);
	session->component_list[id] = session->component_list[--session->component_list_count];
	for(i = 0; i < session->entity_define_count; i++)
	{
		for(j = 0; j < session->entity_defines[i].component_count; j++)
		{
			if(session->entity_defines[i].components[j].type == id)
				c_entity_components_remove(session, &session->entity_defines[i], j--, FALSE);
			else if(session->entity_defines[i].components[j].type == session->component_list_count)
				session->entity_defines[i].components[j].type = id;
		}
	}
	for(j = 0; j < session->primitive_settings.component_count; j++)
	{
		if(session->primitive_settings.components[j].type == id)
			c_entity_components_remove(session, &session->primitive_settings, j--, FALSE);
		else if(session->primitive_settings.components[j].type == session->component_list_count)
			session->primitive_settings.components[j].type = id;
	}
}

uint confuse_component_count_get(CSession *session)
{
	return session->component_list_count;
}

char *confuse_component_name_get(CSession *session, uint component)
{
	return session->component_list[component].name;
}

CComponentCategory confuse_component_category_get(CSession *session, uint component)
{
	return session->component_list[component].type;
}

CTypeType confuse_component_output_get(CSession *session, uint component)
{
	return session->component_list[component].output;
}

uint confuse_component_output_pos_get(CSession *session, CComponent *component)
{
	if(session->component_list[component->type].special == C_SCT_ANIMATION)
		return ((CSpecialAnimation *)component->special)->key_count + 1;
	return session->component_list[component->type].param_count;
}
uint confuse_component_param_count_init_get(CSession *session, uint component)
{
	return session->component_list[component].param_count;
}

uint confuse_component_param_count_extend_get(CSession *session, CComponent *component)
{
	if(component->type == 0)
		return ((CSpecialAnimation *)component->special)->key_count + 2;
	return session->component_list[component->type].param_count;
}

CTypeType confuse_component_param_type_get(CSession *session, uint component, uint id)
{
	return session->component_list[component].param_types[id];
}

char *confuse_component_param_name_get(CSession *session, uint component, uint id)
{
	return session->component_list[component].param_names[id];
}

uint confuse_component_primitive_get(CSession *session, uint component)
{
	return session->component_list[component].primitive;
}

boolean confuse_component_output_available(CSession *session, uint component, CTypeType type)
{
	uint out;
	out = session->component_list[component].output;
	if(out == -1)
		return FALSE;
	if(out == type || (type < C_TYPE_SAME_FLOAT_AS_OUTPUT && out == C_TYPE_SAME_FLOAT_AS_OUTPUT))
		return TRUE;
	return FALSE;
}

CSpecialComponentType confuse_component_special_get(CSession *session, uint component)
{
	return session->component_list[component].special;
}


void confuse_execute(CSession *session, CEntity *entity, float delta_time, boolean reset_time)
{
	static float time = 0;
	static uint seed = 0;
	CInstance instance;
	uint i;

	session->delta_time = delta_time;
	time += delta_time;
	if(reset_time)
		time = 0;

	instance.mutex = 0;
	instance.seed = seed;
	instance.entity = entity;
	instance.stack = entity->stack;
	instance.time = time;
	instance.fire_and_forget = FALSE;
	instance.compute = C_CS_READY;
	instance.ended = FALSE;
	for(i = 0; i < entity->component_count; i++)
		if(session->component_list[entity->components[i].type].primitive == -1)
			session->component_list[entity->components[i].type].exe_func(session, &instance, &entity->components[i], -1);
#ifdef F_MEMORY_DEBUG
	if(f_debug_memory())
	{
		printf("confuse_execute Error");
		exit(0);
	}
#endif
	for(i = 0; i < entity->component_count; i++)
		if(session->component_list[entity->components[i].type].primitive != -1)
			session->component_list[entity->components[i].type].exe_func(session, &instance, &entity->components[i], session->component_list[entity->components[i].type].primitive);
	if(instance.ended)
	{
		seed++;
		time = 0;
	}
	/*
	entity = &session->primitive_settings;
	for(i = 0; i < entity->component_count; i++)
		session->component_list[entity->components[i].type].exe_func(session, entity, entity->stack, &entity->components[i], session->component_list[entity->components[i].type].primitive);
*/
#ifdef F_MEMORY_DEBUG
	if(f_debug_memory())
	{
		printf("confuse_execute Error");
		exit(0);
	}
#endif
}


void confuse_execute_display(CSession *session, uint entity_id)
{
	CInstance instance;
	CEntity *entity;
	uint i;
	entity = &session->entity_defines[entity_id];
	instance.mutex = 0;
	instance.seed = 0;
	instance.entity = entity;
	instance.stack = entity->stack;
	instance.time = 0;
	instance.fire_and_forget = FALSE;
	instance.compute = C_CS_READY;
	instance.ended = FALSE;
	for(i = 0; i < entity->component_count; i++)
		if(session->component_list[entity->components[i].type].display_func != NULL)
			session->component_list[entity->components[i].type].display_func(session, &instance, &entity->components[i], -1);
}

/*
typedef struct{
	CDrawStateType type;
	uint attrib_param_count;
	CDrawStateInput *attrib_params;
	uint uniform_param_count;
	CDrawStateInput *uniform_params;
	uint blend_destination;
	uint blend_source;
	boolean depth_test;
	void *attrib_buffer;
	void *uniform_buffer;
	uint attrib_stride;
	uint uniform_stride;
	uint buffer_size;
	uint attrib_used;
	uint uniform_used;
	float *sort_distance;
	float *sort_buffer;
	char name[16];
	char *shaders[2];
	char *shader_paths[2];
	void *state_shader;
	void *state_array;
	boolean injected;
	uint geometry_checksum;
}CDrawState; /* a primitive */

extern float c_primitives_matrix[9];

boolean confuse_execute_sort_func(uint big, uint small, void *user)
{
	return ((float *)user)[big] > ((float *)user)[small];
}

void confuse_execute_sort(CDrawState *state)
{
	float *buffer_a, *buffer_b, vec[3];
	uint i, j, count, *order;
	if(state->attrib_params == NULL)
		return;
	for(i = 0; i < state->attrib_param_count && state->attrib_params[i].type != C_TYPE_POS; i++);
	if(i == state->attrib_param_count)
		return;
	if(state->sort_distance == NULL)
	{
		state->sort_distance = malloc((sizeof *state->sort_distance) * state->buffer_size);
		state->sort_buffer = malloc(sizeof(float) * state->attrib_stride * state->buffer_size);
	}
	buffer_a = state->sort_distance;
	buffer_b = &((float*)state->attrib_buffer)[state->attrib_params[i].offset];

	count = state->attrib_used / 6;
	for(i = j = 0; i < count; i++)
	{
//		buffer_a[i] = buffer_b[j] * c_primitives_matrix[2] + buffer_b[j + 1] * c_primitives_matrix[5] + buffer_b[j + 2] * c_primitives_matrix[8];
		buffer_a[i] = buffer_b[j] * c_primitives_matrix[6] + buffer_b[j + 1] * c_primitives_matrix[7] + buffer_b[j + 2] * c_primitives_matrix[8];
//		buffer_a[i] = buffer_b[j + 2];
		j += state->attrib_stride * 6;
	}

	order = f_sort_ids(count, NULL, confuse_execute_sort_func, state->sort_distance);

	j = sizeof(float) * state->attrib_stride * 6;

	buffer_a = state->attrib_buffer;
	buffer_b = state->sort_buffer;

/*	for(i = 0; i < count; i++)
		memcpy(&buffer_b[i * state->attrib_stride * 6], &buffer_a[order[i] * state->attrib_stride * 6], j);
	*/
	state->attrib_buffer = buffer_b;
	state->sort_buffer = buffer_a;
	free(order);
}

void confuse_execute_thread_func(void *s)
{
	CSession *session;
	uint i, j, left, frame = 0;
	session = (CSession *)s;
	while(TRUE)
	{
		left = 0;
		while(left < session->instance_count)
		{
			for(i = 0; i < session->instance_count; i++)
			{
				if(imagine_mutex_lock_try(session->instances[i]->mutex))
				{	
					if(session->instances[i]->compute == C_CS_READY)
					{
						for(j = 0; j < session->instances[i]->entity->component_count; j++)
							if(session->component_list[session->instances[i]->entity->components[i].type].primitive == -1)
								session->component_list[session->instances[i]->entity->components[j].type].exe_func(session, session->instances[i], &session->instances[i]->entity->components[j], session->component_list[session->instances[i]->entity->components[j].type].primitive);
						for(j = 0; j < session->instances[i]->entity->component_count; j++)
							if(session->component_list[session->instances[i]->entity->components[i].type].primitive != -1)
								session->component_list[session->instances[i]->entity->components[j].type].exe_func(session, session->instances[i], &session->instances[i]->entity->components[j], session->component_list[session->instances[i]->entity->components[j].type].primitive);
						session->instances[i]->compute = C_CS_COMPUTED;
					}
					if(session->instances[i]->compute != C_CS_LOCKED)
						left++;
					imagine_mutex_unlock(session->instances[i]->mutex);
				}else
					left++;
			}
		}

		imagine_mutex_lock_try(session->master_mutex); /* lock the master mutex so that all threads have a mutex to rest on */
		imagine_mutex_lock(session->gate_mutex); 
		session->gate_counter++; /* tell the main thread that another worker thread is waiting */
		imagine_mutex_unlock(session->gate_mutex);

		imagine_mutex_lock(session->master_mutex); /* wait here for the next frames first unlock */

		imagine_mutex_lock(session->gate_mutex);
		session->gate_counter--;
		imagine_mutex_unlock(session->gate_mutex);
	}
}


/*
Execution order:
-Sims without output
-Sims with outputs
-Primitives

*/
/*
	C_CU_UNCHANGED,
	C_CU_UPDATED,
	C_CU_COMPUTED
	*/
boolean confuse_execute_recursive(CSession *session, CInstance *instance, uint component_id, uint primitive_id)
{
	CComponentDef *component_def;
	CComponent *component_inst;
	CEntity *entity;
	uint i, count;
	boolean update = FALSE;

	entity = instance->entity;
	component_inst = &entity->components[component_id];
	component_def = &session->component_list[component_inst->type];

	if(component_inst->special != NULL)
	{
		CSpecialAnimation *a;
		a = component_inst->special;
		count = a->key_count + 1;
	}else
		count = component_def->param_count;
/*	if(entity->stack_types[component_inst->references[count]].type == C_TYPE_PARTICLE)
	{
		for(i = 0; i  < entity->component_count; i++)
			if(session->component_list[entity->components[i].type] == C_CC_FORCE)
				confuse_execute_recursive(session, instance, i, component_inst->references[count]);
	}
*/	
	for(i = 0; i < count; i++)
	{
		if(instance->updates[component_inst->references[i]] != C_CU_COMPUTED)
		{
			if(entity->stack_types[component_inst->references[i]].dependency != -1)
			{
				if(confuse_execute_recursive(session, instance, entity->stack_types[component_inst->references[i]].dependency, -1))
					update = TRUE;
			}else if(instance->updates[component_inst->references[i]] == C_CU_UPDATED)
				update = TRUE;
			instance->updates[component_inst->references[i]] = C_CU_COMPUTED;
		}
	}

	if(update || component_def->type != C_CC_PROCESS)
	{
		component_def->exe_func(session, instance, component_inst, primitive_id);
		instance->updates[component_inst->references[i]] = C_CU_COMPUTED;
		return TRUE;
	}
	return FALSE;
}

void confuse_execute_tree(CSession *session, CInstance *instance)
{
	CEntity *entity;
	uint i, type;
	entity = instance->entity;
	for(i = 0; i  < entity->component_count; i++)
	{
		type = session->component_list[entity->components[i].type].type;
		if(type == C_CC_DRAW)
			confuse_execute_recursive(session, instance, i, session->component_list[entity->components[i].type].primitive);
		else if(type == C_CC_OBJECT)
			confuse_execute_recursive(session, instance, i, -1);

	}
	for(i = 0; i  < entity->stack_type_count; i++)
		instance->updates[i] = C_CU_UNCHANGED;
}

/*
void confuse_entity_instance_destroy(CSession *session, CInstance *instance)
*/

void confuse_execute_frame_advance(CSession *session, float delta_time)
{
	static float time = 0;
	boolean fire_and_forget = FALSE;
	uint i, j, gates = 0;
	CEntity *entity;
	CInstance instance;
	imagine_mutex_lock_try(session->master_mutex);
	while(gates != session->thread_count)
	{
		imagine_mutex_lock(session->gate_mutex);
		gates = session->gate_counter;
		imagine_mutex_unlock(session->gate_mutex);
	}
	session->delta_time = delta_time; 
	if(session->thread_count == 0)
		for(i = 0; i < session->instance_count; i++)
			if(!session->instances[i]->ended)
				for(j = 0; j < session->instances[i]->entity->component_count; j++)	
					session->component_list[session->instances[i]->entity->components[j].type].exe_func(session, session->instances[i], &session->instances[i]->entity->components[j], session->component_list[session->instances[i]->entity->components[j].type].primitive);
	instance.mutex = 0;
	instance.seed = 0;
	instance.entity = entity = &session->primitive_settings;
	instance.stack = entity->stack;
	if(delta_time < 1)
		time += delta_time;
	instance.time = time;
	instance.fire_and_forget;
	instance.compute = C_CS_READY;

	for(i = 0; i < entity->component_count; i++)
		session->component_list[entity->components[i].type].exe_func(session, &instance, &entity->components[i], session->component_list[entity->components[i].type].primitive);

/*	for(i = 0; i < session->primitive_count; i++)
		if(session->prim[i].type == C_DPT_DRAW_CALL_ATTRIBUTE)
			confuse_execute_sort(&session->prim[i]);
			*/
	for(i = 0; i < session->instance_count; i++)
	{
		session->instances[i]->time += delta_time;
		if(session->instances[i]->fire_and_forget)
		{
			fire_and_forget = TRUE;
			session->instances[i]->compute = C_CS_READY;
		}else
		{
			imagine_mutex_lock(session->instances[i]->mutex);
			session->instances[i]->compute = 
				C_CS_LOCKED;
		}
	}
	if(fire_and_forget) /* there is some thing that needs computing that we can release the threads on.*/
		imagine_mutex_unlock(session->master_mutex);
}

void confuse_execute_threads_launch(CSession *session, uint thread_count)
{
	uint i;
	for(i = 0; i < thread_count; i++)
		imagine_thread(confuse_execute_thread_func, session);
}

void confuse_entity_instance_compute(CSession *session, CInstance *instance)
{
	imagine_mutex_unlock(instance->mutex);
}

void confuse_entity_instance_compute_fire_and_forget(CSession *session, CInstance *instance)
{
	instance->fire_and_forget = TRUE;
	imagine_mutex_unlock(instance->mutex);
}


void confuce_draw_begin(CSession *session, float delta_time)
{
	confuse_execute_frame_advance(session, delta_time);
}

void confuce_draw_end(CSession *session)
{
	uint i;
	for(i = 0; i < session->primitive_count; i++)
	{
		if(session->prim[i].type == C_DPT_DRAW_CALL_ATTRIBUTE)
			session->prim[i].attrib_used = 0;
		session->prim[i].uniform_used = 0;
	}
//	imagine_mutex_unlock(session->master_mutex);
}

/*
Mutex rules:
Add

During full lock:
	Remove add new instances.
	kill all threads.
	Swap render output.
Anywhere else:
	lock instances.

Lock for creation and destruction:



*/