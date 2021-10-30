#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "imagine.h"
#include "c_internal.h"





uint c_type_size_get(CTypeType type)
{
	static uint size[] = {1, /* C_TYPE_FLOAT */
						2, /* C_TYPE_UV */
						3, /* C_TYPE_POS */
						4, /* C_TYPE_COLOR */
						16, /* C_TYPE_MATRIX */
						8, /* C_TYPE_AREA */
						10, /* C_TYPE_POS_QUATERNION_SIZE */
						8, /* C_TYPE_UV_SECTION */
						0, /* C_TYPE_SAME_FLOAT_AS_OUTPUT */
						1, /* C_TYPE_IMAGE_2D */
						1, /* C_TYPE_IMAGE_3D */
						1, /* C_TYPE_IMAGE_CUBE */
						3, /* C_TYPE_OBJECT */
						1, /* C_TYPE_STATE_DEPTH_TEST */
						1, /* C_TYPE_STATE_BLEND_SOURCE */
						1, /* C_TYPE_STATE_BLEND_DESTINATION */
						7, /* C_TYPE_PARTICLE - Pos, Vec, Age*/
						1};	/* C_TYPE_BOOLEAN */
	if(C_TYPE_SAME_FLOAT_AS_OUTPUT == type)
	{
		uint *a = NULL;
		a[0] = 0;
	}
	return size[type];
}

void c_entity_stack_clear(float *stack, CTypeType type, uint size, uint pos)
{
	uint i, *istack;

	size *= c_type_size_get(type);
	switch(type)
	{
		case C_TYPE_FLOAT :
		for(i = pos; i < pos + size; i++)
			stack[i] = 0.1;
		break;
		case C_TYPE_UV :
		for(i = pos; i < pos + size; i++)
			stack[i] = 0.1;
		break;
		case C_TYPE_POS :
		if(size == 1)
		{
			for(i = pos; i < pos + size; i++)
				stack[i] = 0;
		}else
		{
			for(i = pos; i < pos + size; i++)
				stack[i] = f_randnf(i) * 0.1;
		}
		break;
		case C_TYPE_COLOR :
		if(size == 1)
		{
			for(i = pos; i < pos + size; i++)
				stack[i] = 1.0;
		}else
		{
			for(i = pos; i < pos + size; i++)
				stack[i] = f_randf(i);
		}
		break;
		case C_TYPE_MATRIX :
		for(i = pos; i < pos + size;)
		{
			stack[i++] = 1.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 1.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 1.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 1.0;
		}
		break;
		case C_TYPE_AREA:
		for(i = pos; i < pos + size; i++)
			stack[i] = 0.1;
		break;
		case C_TYPE_POS_QUATERNION_SIZE :
		for(i = pos; i < pos + size;)
		{
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;

			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 1.0;

			stack[i++] = 1.0;
			stack[i++] = 1.0;
			stack[i++] = 1.0;
		}
		break;
		case C_TYPE_UV_SECTION :
		for(i = pos; i < pos + size;)
		{
			stack[i++] = 0.0;
			stack[i++] = 0.0;
			stack[i++] = 1.0;
			stack[i++] = 0.0;
			stack[i++] = 1.0;
			stack[i++] = 1.0;
			stack[i++] = 0.0;
			stack[i++] = 1.0;
		}
		break;
		case C_TYPE_IMAGE_2D :
		case C_TYPE_IMAGE_3D :
		case C_TYPE_IMAGE_CUBE :
			istack = (uint*)&stack[pos];
			for(i = 0; i < size;)
				istack[i++] = -1;
		break;
		case C_TYPE_OBJECT :
			istack = (uint*)&stack[pos];
			for(i = 0; i < size;)
			{
				istack[i++] = -1;
				istack[i++] = 0;
				istack[i++] = 0;
			}
		break;
		case C_TYPE_STATE_DEPTH_TEST :
			istack = (uint*)&stack[pos];
			for(i = 0; i < size;)
				istack[i++] = -1;
		break;
		case C_TYPE_STATE_BLEND_SOURCE :
			istack = (uint*)&stack[pos];
			for(i = 0; i < size;)
				istack[i++] = 1;
		break;
		case C_TYPE_STATE_BLEND_DESTINATION :
			istack = (uint*)&stack[pos];
			for(i = 0; i < size;)
				istack[i++] = 0x0303; /* GL_ONE_MINUS_SRC_ALPHA */
		break;
		case C_TYPE_PARTICLE :
			for(i = pos; i < pos + size;)
			{
				stack[i] = f_randnf(i);
				i++;
				stack[i] = f_randnf(i);
				i++;
				stack[i] = f_randnf(i);
				i++;
				stack[i] = f_randnf(i);
				i++;
				stack[i] = f_randnf(i);
				i++;
				stack[i] = f_randnf(i);
				i++;
				stack[i] = f_randnf(i);
				i++;
			}
		break;
		case C_TYPE_BOOLEAN :
			for(i = pos; i < pos + size; i++)
				stack[i] = 0.5; 
		break;
	}
}

void c_entity_stack_resize(CEntity *entity, uint id, uint new_size)
{
	uint k, size, old_size;
	old_size = entity->stack_types[id].size;
	size = c_type_size_get(entity->stack_types[id].type);
	entity->stack_size += new_size * size;
	entity->stack_size -= old_size * size;
	if(entity->stack_size > entity->stack_alloc)
	{
		entity->stack_alloc = entity->stack_size;
		entity->stack = realloc(entity->stack, (sizeof *entity->stack) * entity->stack_alloc);
	}

	if(id + 1 < entity->stack_type_count)
	{
		if(old_size > new_size)
		{
			for(k = entity->stack_types[id].pos + new_size * size; k + (old_size - new_size) * size < entity->stack_size + (old_size - new_size) * size; k++)
				entity->stack[k] = entity->stack[k + (old_size - new_size) * size];
		}else
		{
			for(k = entity->stack_size - 1; k >= entity->stack_types[id].pos + new_size * size; k--)
				entity->stack[k] = entity->stack[k - (new_size - old_size) * size];
		}
	}
	if(old_size < new_size)
		c_entity_stack_clear(entity->stack, entity->stack_types[id].type, new_size - old_size, entity->stack_types[id].pos + old_size * size);
	for(k = id + 1; k < entity->stack_type_count; k++)
		entity->stack_types[k].pos += (new_size - old_size) * c_type_size_get(entity->stack_types[id].type);
	entity->stack_types[id].size = new_size;
}

void c_entity_stack_name(CEntity *e, uint id, char *name)
{
	char n[64];
	uint i, j, k;
	sprintf(n, "%s", name);
	for(i = j = 0; j < e->stack_type_count; i++)
	{
		for(j = 0; j < e->stack_type_count; j++)
		{
			for(k = 0; e->stack_types[j].name[k] != 0 && e->stack_types[j].name[k] == n[k]; k++);
			if(e->stack_types[j].name[k] == n[k])
				break;
		}
		if(j < e->stack_type_count)
			sprintf(n, "%s_%u", name, i + 1);
	}
	for(i = 0; n[i] != 0 && i < 31; i++)
		e->stack_types[id].name[i] = n[i];
	e->stack_types[id].name[i] = 0;
}

uint c_entity_stack_add(CEntity *e, CTypeType type, char *name, uint size)
{
	if(e->stack_type_count == e->stack_type_alloc)
	{
		e->stack_type_alloc += 16;
		e->stack_types = realloc(e->stack_types, (sizeof *e->stack_types) * e->stack_type_alloc);
	}
	if(e->stack_alloc < e->stack_size + size * c_type_size_get(type))
	{
		e->stack_alloc = e->stack_size + size * c_type_size_get(type) + 16;
		e->stack = realloc(e->stack, (sizeof *e->stack) * e->stack_alloc);
	}

	c_entity_stack_clear(e->stack, type, size, e->stack_size);
	e->stack_types[e->stack_type_count].type = type;

	c_entity_stack_name(e, e->stack_type_count, name);

	e->stack_types[e->stack_type_count].pos = e->stack_size;
	e->stack_size += size * c_type_size_get(type);
	e->stack_types[e->stack_type_count].size = size;
	e->stack_types[e->stack_type_count].dependency = -1;
	e->stack_types[e->stack_type_count].selected = 0;
	e->stack_type_count++;
	return e->stack_type_count - 1;
}

uint c_entity_stack_user_count(CSession *session, CEntity *entity, uint stack_id, boolean output)
{
	uint i, j, param_count, count = 0;
	for(i = 0; i < entity->component_count; i++)
	{
		param_count = confuse_component_param_count_extend_get(session, &entity->components[i]);
		for(j = 0; j < param_count; j++)
			if(entity->components[i].references[j] == stack_id)
				count++;

	}
	if(output && entity->stack_types[stack_id].dependency != -1)
		count++;
	return count;
}

boolean c_entity_stack_remove(CSession *session, CEntity *entity, uint stack_id)
{
	uint count, i, j;
	if(0 != c_entity_stack_user_count(session, entity, stack_id, TRUE))
		return FALSE;

	count = entity->stack_types[stack_id].size * c_type_size_get(entity->stack_types[stack_id].type);
	for(i = entity->stack_types[stack_id].pos; i < entity->stack_size - count; i++)
		entity->stack[i] = entity->stack[i + count];

	entity->stack_type_count--;
	for(i = stack_id; i < entity->stack_type_count; i++)
	{
		entity->stack_types[i] = entity->stack_types[i + 1];
		entity->stack_types[i].pos -= count;
	}
	for(i = 0; i < entity->component_count; i++)
	{
		count = confuse_component_param_count_extend_get(session, &entity->components[i]);
		for(j = 0; j < count; j++)
			if(entity->components[i].references[j] > stack_id)
					entity->components[i].references[j]--;
	}
	return TRUE;
}

boolean c_entity_types_compatible(CTypeType read, CTypeType write)
{
	if(read == write)
		return TRUE;
	if(write == C_TYPE_POS && (read == C_TYPE_PARTICLE || read == C_TYPE_MATRIX))
		return TRUE;
	if(write == C_TYPE_FLOAT && read == C_TYPE_PARTICLE)
		return TRUE;
	if(write == C_TYPE_BOOLEAN && (read == C_TYPE_PARTICLE || write == C_TYPE_FLOAT))
		return TRUE;
	return FALSE;
}

uint c_entity_stack_get(CEntity *e, uint id, CTypeType type, uint *stride, uint *size)
{
	if(e->stack_types[id].size == 0)
		e->stack_types[id].size = 0;
	*size = e->stack_types[id].size;
	if(e->stack_types[id].type == type)
	{
		*stride = c_type_size_get(type);
		return e->stack_types[id].pos;
	}else
	{
		if(type == C_TYPE_UV && e->stack_types[id].type == C_TYPE_UV_SECTION)
		{
			*stride = c_type_size_get(e->stack_types[id].type);
			return e->stack_types[id].pos;
		}
		if(type == C_TYPE_POS)
		{
			*stride = c_type_size_get(e->stack_types[id].type);
			if(e->stack_types[id].type == C_TYPE_MATRIX)
				return e->stack_types[id].pos + 12;
			else if(e->stack_types[id].type == C_TYPE_POS_QUATERNION_SIZE)
				return e->stack_types[id].pos;
			else if(e->stack_types[id].type == C_TYPE_PARTICLE)
				return e->stack_types[id].pos;
		}
		if(type == C_TYPE_FLOAT && e->stack_types[id].type == C_TYPE_PARTICLE)
		{
			*stride = c_type_size_get(C_TYPE_PARTICLE);
			return e->stack_types[id].pos + 6;
		}
		if(type == C_TYPE_BOOLEAN)
		{
			if(e->stack_types[id].type == C_TYPE_FLOAT)
			{
				*stride = c_type_size_get(C_TYPE_FLOAT);
				return e->stack_types[id].pos;
			}
			if(e->stack_types[id].type == C_TYPE_PARTICLE)
			{
				*stride = c_type_size_get(C_TYPE_PARTICLE);
				return e->stack_types[id].pos + 6;
			}
		}
	}
	printf("Confuse ERROR: c_entity_stack_get type conversion failed\n");
	{
		uint *a = NULL;
		a[0] = 0; 
	}
	exit(0);
}
void c_entity_components_add(CSession *session, CEntity *e, uint type, uint output)
{
	CComponent *c;
	uint i, j, k, type_count;
	CTypeType t;
	char buf[64];


	if(e->component_count % 16 == 0)
		e->components = realloc(e->components, (sizeof *e->components) * (e->component_count + 16));
	type_count = confuse_component_param_count_init_get(session, type);

	c = &e->components[e->component_count++];
	c->type = type;
	c->references = malloc((sizeof *c->references) * (type_count + 1));

	for(i = 0; i < type_count; i++)
	{
		char *text;
		t = confuse_component_param_type_get(session, type, i);
		text = confuse_component_param_name_get(session, type, i);
		if(t == C_TYPE_SAME_FLOAT_AS_OUTPUT)
			t = e->stack_types[output].type;
		sprintf(buf, "%s %s", confuse_component_name_get(session, type), confuse_component_param_name_get(session, type, i));
		if(t == C_TYPE_PARTICLE)
		{
			for(j = 0; j < e->stack_type_count && e->stack_types[j].type != C_TYPE_PARTICLE; j++);
			if(j < e->stack_type_count)
				c->references[i] = j;
			else
				c->references[i] = c_entity_stack_add(e, t, buf, 1);
		}else
			c->references[i] = c_entity_stack_add(e, t, buf, 1);
	}
	if(output != -1)
	{
		c->references[i] = output;
		e->stack_types[output].dependency = e->component_count - 1;
	}else
	{
		if(C_CC_OBJECT == confuse_component_category_get(session, type))
		{
			if(C_TYPE_PARTICLE == confuse_component_output_get(session, type))
				c->references[i] = c_entity_stack_add(e, confuse_component_output_get(session, type), "output", 100);
			else
				c->references[i] = c_entity_stack_add(e, confuse_component_output_get(session, type), "output", 1);
		}
	}

	c->special = NULL;


	switch(confuse_component_special_get(session, type))
	{
		case C_SCT_ANIMATION :
		{
			CSpecialAnimation *a;
			a = malloc(sizeof *a);
			a->key_allocated = 2;
			a->key_count = 2;
			a->keys = malloc((sizeof *a->keys) * a->key_allocated);
			a->keys[0].key_time = 0;
			a->keys[0].key_ease[0] = 2.0 / 3.0; 
			a->keys[0].key_ease[1] = 1.0 / 3.0; 
			a->keys[1].key_time = 1;
			a->keys[1].key_ease[0] = 2.0 / 3.0; 
			a->keys[1].key_ease[1] = 1.0 / 3.0;
			c->special = a;
		}
		break;
	}
#ifdef F_MEMORY_DEBUG
	if(f_debug_memory())
	{
		printf("c_entity_components_add Error");
		exit(0);
	}
#endif
}

void c_entity_components_remove(CSession *session, CEntity *e, uint component_id, boolean stack)
{
	CComponent *c;
	uint i;
	c = &e->components[component_id]; 
	if(c->references != NULL)
		free(c->references);
	if(c->special != NULL)
	{
		switch(confuse_component_special_get(session, e->components[component_id].type))
		{
			case C_SCT_ANIMATION :
			{
				CSpecialAnimation *a;
				a = c->special;
				free(a->keys);
				free(a);
			}
			break;
		}
	}
	e->component_count--;
	if(e->component_count > 0)
		for(i = component_id; i < e->component_count; i++)
			e->components[i] = e->components[i + 1];
	for(i = 0; i < e->stack_type_count; i++)
	{
		if(e->stack_types[i].dependency != -1)
		{
			if(e->stack_types[i].dependency == component_id)
				e->stack_types[i].dependency = -1;
			else if(e->stack_types[i].dependency > component_id)
				e->stack_types[i].dependency--;
		}
	}
}


void c_entity_allocate(CEntity *e, char *name)
{
	uint i;
	for(i = 0; i < 31 && name[i] != 0; i++)
		e->name[i] = name[i];
	e->name[i] = 0;
	e->components = NULL;
	e->component_count = 0;

	e->stack_alloc = 0;
	e->stack = NULL;
	e->stack_size = 0;
	e->stack_types = 0;
	e->stack_type_count = 0;
	e->stack_type_alloc = 0;
}


extern void c_execute_component_module_animation_init(CSession *session);
extern void c_execute_component_module_math_init(CSession *session);
extern void c_execute_component_module_uv_init(CSession *session);
extern void c_execute_component_module_initialization_init(CSession *session);
extern void c_execute_component_module_particles_init(CSession *session);
extern void c_execute_component_module_color_init(CSession *session);

CSession *confuse_session_create()
{
	CSession *s;
	uint i;
	s = malloc(sizeof *s);

	s->entity_defines = NULL;
	s->entity_define_count = 0;

	s->instances = NULL;
	s->instance_count = 0;
	s->instance_allocated = 0;

	c_entity_allocate(&s->primitive_settings, "primitives");
	s->prim = NULL;
	s->primitive_count = 0;
	s->component_list = NULL;
	s->component_list_count = 0;


	s->thread_count = 0;
	s->master_mutex = imagine_mutex_create();
	s->gate_mutex = imagine_mutex_create();
	s->gate_counter = 0;
	s->delete_me = FALSE;
	
	c_execute_component_module_animation_init(s);
	c_execute_component_module_uv_init(s);
	c_execute_component_module_math_init(s);
	c_execute_component_module_initialization_init(s);
	c_execute_component_module_particles_init(s);
	c_execute_component_module_color_init(s);
	return s;
}

void confuse_session_free_internal(CSession *session)
{
	CEntity *e;
	uint i, j;
	if(session->entity_defines != NULL)
	{
		for(i = 0; i < session->entity_define_count; i++)
		{
			e = &session->entity_defines[i];
			for(j = 0; j < e->component_count; j++)
				c_entity_components_remove(session, e, i, FALSE);
			if(e->stack_types)
				free(e->stack_types);
			if(e->stack)
				free(e->stack);
		}
		free(session->entity_defines);
	}
	
	e = &session->primitive_settings;
	for(j = 0; j < e->component_count; j++)
		c_entity_components_remove(session, e, i, FALSE);
	if(e->stack_types)
		free(e->stack_types);
	if(e->stack)
		free(e->stack);

	for(i = 0; i < session->instance_count; i++)
	{
		free(session->instances[i]->stack);
		free(session->instances[i]);
	}
	free(session->instances);
	imagine_mutex_destroy(session->master_mutex);
	imagine_mutex_destroy(session->gate_mutex);
	free(session);
}

void confuse_session_free(CSession *session)
{
	if(session->thread_count == 0)
		confuse_session_free_internal(session);
	else
		session->delete_me = TRUE;
}

CEntity *confuse_session_entity_add(CSession *session, char *name)
{
	CEntity *e;
	if(session->entity_define_count % 16 == 0)
		session->entity_defines = realloc(session->entity_defines, (sizeof *session->entity_defines) * (session->entity_define_count + 16));
	e = &session->entity_defines[session->entity_define_count++];
	c_entity_allocate(e, name);
	return e;
}


void confuse_entity_template_create(CSession *session, char *name, uint param_count, CTypeType *param_type, char **param_name)
{
	CEntity *entity;
	uint i;
	entity = confuse_session_entity_add(session, name);
	for(i = 0; i < param_count; i++)
		c_entity_stack_add(entity, param_type[i], param_name[i], 1);
}


void confuse_session_entity_delete(CSession *session, uint entity_id)
{
	CEntity *e;
	uint i;
	e = &session->entity_defines[entity_id];
	for(i = 0; i < e->component_count; i++)
	{
		free(e->components[i].references);
		if(e->components[i].special != NULL)
		{
			CSpecialAnimation *a;
			a = e->components[i].special;
			free(a->keys);
			free(a);
		}
	}
	free(e->stack);
	if(e->stack != NULL)
	{
		free(e->stack);
		free(e->stack_types);
	}
	session->entity_define_count--;
	if(session->entity_define_count != 0)
	{
		for(i = entity_id; i < session->entity_define_count; i++)
			session->entity_defines[i] = session->entity_defines[i + 1];
	}else
		confuse_session_entity_add(session, "Empty");
}


CEntity *confuse_session_entity_clone(CSession *session, CEntity *entity)
{
	CEntity *clone;
	uint i, j;
	clone = confuse_session_entity_add(session, entity->name);

	clone->component_count = entity->component_count;
	clone->components = malloc((sizeof *clone->components) * (clone->component_count + 16));
	for(i = 0; i < clone->component_count; i++)
	{
		clone->components[i].type = entity->components[i].type;
		if(session->component_list[entity->components[i].type].special == C_SCT_ANIMATION)
		{
			CSpecialAnimation *anim, *original;
			clone->components[i].special = anim = malloc(sizeof *anim);
			original = entity->components[i].special;
			anim->key_allocated = original->key_allocated;
			anim->key_count = original->key_count;
			anim->keys = malloc((sizeof *anim->keys) * original->key_allocated);
			for(j = 0; j < anim->key_count; j++)
				anim->keys[j] = original->keys[j];
			clone->components[i].references = malloc((sizeof *clone->components[i].references) * (original->key_allocated + 2));
			for(j = 0; j < anim->key_count + 2; j++)
				clone->components[i].references[j] = entity->components[i].references[j];
		}else
		{
			clone->components[i].special = NULL;
			clone->components[i].references = malloc((sizeof *clone->components[i].references) * (session->component_list[entity->components[i].type].param_count + 1));
			for(j = 0; j < (session->component_list[entity->components[i].type].param_count + 1); j++)
				clone->components[i].references[j] = entity->components[i].references[j];
		}
	}

	clone->stack_alloc = entity->stack_alloc;
	clone->stack_size = entity->stack_size;
	clone->stack_type_count = entity->stack_type_count;
	clone->stack_type_alloc = entity->stack_type_alloc;

	clone->stack = malloc((sizeof *clone->stack) * clone->stack_alloc);
	for(i = 0; i < clone->stack_size; i++)
		clone->stack[i] = entity->stack[i];

	clone->stack_types = malloc((sizeof *clone->stack_types) * clone->stack_type_alloc);
	for(i = 0; i < clone->stack_type_count; i++)
		clone->stack_types[i] = entity->stack_types[i];
	return clone;
}

