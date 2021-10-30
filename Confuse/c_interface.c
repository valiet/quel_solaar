#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "imagine.h"

uint confuse_entity_count(CSession *session)
{
	return session->entity_define_count;
}

char *confuse_entity_name_get(CSession *session, uint entity_id)
{
	return session->entity_defines[entity_id].name;
}

uint confuse_entity_name_lookup(CSession *session, char *name, boolean ignore_empty)
{
	uint i, j;
	for(i = 0; i < session->entity_define_count; i++)
	{
		for(j = 0; name[j] == session->entity_defines[i].name[j] && name[j] != 0; j++);
		if(name[j] == session->entity_defines[i].name[j])
			if(session->entity_defines[i].component_count > 0 || !ignore_empty)
				return i;
	}
	return -1;
}

uint confuse_entity_type_count(CSession *session, uint entity_id)
{
	return session->entity_defines[entity_id].stack_type_count;
}

CType *confuse_entity_type_get(CSession *session, uint entity_id, uint type_id)
{
	return &session->entity_defines[entity_id].stack_types[type_id];
}

CType *confuse_entity_type_lookup(CSession *session, uint entity_id, char *name)
{
	uint i, j;
	for(i = 0; i < session->entity_defines[entity_id].stack_type_count; i++)
	{
		for(j = 0; name[j] == session->entity_defines[entity_id].stack_types[i].name[j] && name[j] != 0; j++);
		if(name[j] == session->entity_defines[entity_id].stack_types[i].name[j])
			return &session->entity_defines[entity_id].stack_types[i];
	}
	return NULL;
}

CTypeType confuse_entity_type_get_type(CSession *session, CType *type)
{
	return type->type;
}

CType *confuse_entity_type_specific_lookup(CSession *session, uint entity_id, char *name, CTypeType type)
{
	uint i, j;
	for(i = 0; i < session->entity_defines[entity_id].stack_type_count; i++)
	{
		if(session->entity_defines[entity_id].stack_types[i].type == type)
		{
			for(j = 0; name[j] == session->entity_defines[entity_id].stack_types[i].name[j] && name[j] != 0; j++);
			if(name[j] == session->entity_defines[entity_id].stack_types[i].name[j])
				return &session->entity_defines[entity_id].stack_types[i];
		}
	}
	return NULL;
}

CInstance *confuse_entity_instance_create(CSession *session, uint entity_id)
{
	static uint seed = 0;
	CInstance *inst;
	uint i;
	if(session->instance_allocated == session->instance_count)
	{
		session->instance_allocated += 256;
		session->instances = realloc(session->instances, (sizeof *session->instances) * session->instance_allocated);
	}
	inst = session->instances[session->instance_count++] = malloc(sizeof *session->instances[0]);
	inst->mutex = imagine_mutex_create();
	imagine_mutex_lock(inst->mutex);
	inst->entity = &session->entity_defines[entity_id];
	inst->fire_and_forget = FALSE;
	inst->seed = seed++;
	inst->time = 0;
	inst->ended = FALSE;
	inst->stack = malloc(sizeof(float) * session->entity_defines[entity_id].stack_size);
	for(i = 0; i < session->entity_defines[entity_id].stack_size; i++)
		((float *)inst->stack)[i] = session->entity_defines[entity_id].stack[i];
	inst->updates = malloc(sizeof(float) * session->entity_defines[entity_id].stack_type_count);
	for(i = 0; i < session->entity_defines[entity_id].stack_type_count; i++)
		inst->updates[i] = C_CU_UPDATED;
	return inst;
}

void confuse_entity_instance_destroy(CSession *session, CInstance *instance)
{
	uint i;
	for(i = 0; i < session->instance_count; i++)
	{
		if(session->instances[i] == instance)
		{
			free(instance->stack);
			free(instance->updates);
			free(instance);
			for(session->instance_count--; i < session->instance_count; i++)
				session->instances[i] = session->instances[i + 1];
			return;
		}
	}
	printf("CONFUSE ERROR: confuse_entity_instance_destroy instance is not an instance in the session");
}

CInstance *confuse_entity_instance_global_get(CSession *session)
{
//	return session->primitive_settings;
	return NULL;
}

void confuse_entity_instance_lock(CSession *session, CInstance *instance)
{
}

void confuse_entity_instance_unlock(CSession *session, CInstance *instance)
{
}

void confuse_entity_instance_unlock_fire_and_forget(CSession *session, CInstance *instance)
{
	instance->fire_and_forget = TRUE;
}

void confuse_entity_instance_set_boolean(CSession *session, CInstance *instance, CType *type, uint id, boolean value)
{
	if(value)
		((float *)instance->stack)[type->pos + id] = 0.5;
	else
		((float *)instance->stack)[type->pos + id] = -0.5;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_float(CSession *session, CInstance *instance, CType *type, uint id, float value)
{
	((float *)instance->stack)[type->pos + id] = value;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_uv(CSession *session, CInstance *instance, CType *type, uint id, float u, float v)
{
	uint pos;
	pos = type->pos + id * 2;
	((float *)instance->stack)[pos++] = u;
	((float *)instance->stack)[pos] = v;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_pos(CSession *session, CInstance *instance, CType *type, uint id, float x, float y, float z)
{
	uint pos;
	pos = type->pos + id * 3;
	((float *)instance->stack)[pos++] = x;
	((float *)instance->stack)[pos++] = y;
	((float *)instance->stack)[pos] = z;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_color(CSession *session, CInstance *instance, CType *type, uint id, float red, float green, float blue, float alpha)
{
	uint pos;
	pos = type->pos + id * 4;
	((float *)instance->stack)[pos++] = red;
	((float *)instance->stack)[pos++] = green;
	((float *)instance->stack)[pos++] = blue;
	((float *)instance->stack)[pos] = alpha;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_matrix(CSession *session, CInstance *instance, CType *type, uint id, float *matrix)
{
	uint pos;
	pos = type->pos + id * 16;
	((float *)instance->stack)[pos++] = matrix[0];
	((float *)instance->stack)[pos++] = matrix[1];
	((float *)instance->stack)[pos++] = matrix[2];
	((float *)instance->stack)[pos++] = matrix[3];
	((float *)instance->stack)[pos++] = matrix[4];
	((float *)instance->stack)[pos++] = matrix[5];
	((float *)instance->stack)[pos++] = matrix[6];
	((float *)instance->stack)[pos++] = matrix[7];
	((float *)instance->stack)[pos++] = matrix[8];
	((float *)instance->stack)[pos++] = matrix[9];
	((float *)instance->stack)[pos++] = matrix[10];
	((float *)instance->stack)[pos++] = matrix[11];
	((float *)instance->stack)[pos++] = matrix[12];
	((float *)instance->stack)[pos++] = matrix[13];
	((float *)instance->stack)[pos++] = matrix[14];
	((float *)instance->stack)[pos] = matrix[15];
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_area(CSession *session, CInstance *instance, CType *type, uint id, float x, float y)
{
	uint pos;
	pos = type->pos + id * 2;
	((float *)instance->stack)[pos++] = x;
	((float *)instance->stack)[pos] = y;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_quaternion(CSession *session, CInstance *instance, CType *type, uint id, float *pos, float *quaternion, float *size)
{
	uint p;
	p = type->pos + id * 10;
	((float *)instance->stack)[p++] = pos[0];
	((float *)instance->stack)[p++] = pos[1];
	((float *)instance->stack)[p++] = pos[2];
	((float *)instance->stack)[p++] = quaternion[0];
	((float *)instance->stack)[p++] = quaternion[1];
	((float *)instance->stack)[p++] = quaternion[2];
	((float *)instance->stack)[p++] = quaternion[3];
	((float *)instance->stack)[p++] = size[0];
	((float *)instance->stack)[p++] = size[1];
	((float *)instance->stack)[p] = size[2];
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_uv_section(CSession *session, CInstance *instance, CType *type, uint id, float *uv0, float *uv1, float *uv2, float *uv3)
{
	uint pos;
	pos = type->pos + id * 10;
	((float *)instance->stack)[pos++] = uv0[0];
	((float *)instance->stack)[pos++] = uv0[1];
	((float *)instance->stack)[pos++] = uv1[0];
	((float *)instance->stack)[pos++] = uv1[1];
	((float *)instance->stack)[pos++] = uv2[0];
	((float *)instance->stack)[pos++] = uv2[1];
	((float *)instance->stack)[pos++] = uv3[0];
	((float *)instance->stack)[pos] = uv3[1];
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_image_id(CSession *session, CInstance *instance, CType *type, uint id, uint32 image_id)
{
	((uint32 *)instance->stack)[type->pos + id] = image_id;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_object_id(CSession *session, CInstance *instance, CType *type, uint id, uint32 object_id)
{
	((uint32 *)instance->stack)[type->pos + id] = object_id;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_depth_test(CSession *session, CInstance *instance, CType *type, uint id, boolean enable)
{
	((uint32 *)instance->stack)[type->pos + id] = enable;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_blend_source(CSession *session, CInstance *instance, CType *type, uint id, uint source_factor)
{
	((uint32 *)instance->stack)[type->pos + id] = source_factor;
	instance->updates[type->number] = C_CU_UPDATED;
}

void confuse_entity_instance_set_blend_dest(CSession *session, CInstance *instance, CType *type, uint id, uint dest_factor)
{
	((uint32 *)instance->stack)[type->pos + id] = dest_factor;
	instance->updates[type->number] = C_CU_UPDATED;
}
