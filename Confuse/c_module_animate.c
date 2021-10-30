
#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"

void c_execute_component_module_blend_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *a, *b, *control2, *a2, *b2, f;
	uint i, j, count = 1, control_count, control_stride, a_count, a_stride, b_count, b_stride, size;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[3]];
	count = type->size;
	output = &stack[type->pos];
	size = c_type_size_get(type->type);

	control = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_FLOAT, &control_stride, &control_count)];
	a = &stack[c_entity_stack_get(entity, c->references[1], type->type, &a_stride, &a_count)];
	b = &stack[c_entity_stack_get(entity, c->references[2], type->type, &b_stride, &b_count)];

	if(size == 1)
	{
		for(i = 0; i < count; i++)
		{
			f = a[(i % a_count) * a_stride];
			output[i] = f + (b[(i % b_count) * b_stride] - f) * control[(i % control_count) * control_stride];

		}
	}else
	{
		for(i = 0; i < count; i++)
		{
			f = control[(i % control_count) * control_stride];
			a2 = &a[(i % a_count) * a_stride];
			b2 = &b[(i % b_count) * b_stride];
			for(j = 0; j < size; j++)
			{
				output[i * size + j] = a2[j] + (b2[j] - a2[j]) * f;
			}
		}
	}
}



void c_execute_component_module_blend_display_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *a, *b, *control2, *a2, *b2, f, mid[3];
	uint i, j, count = 1, control_count, control_stride, a_count, a_stride, b_count, b_stride, size;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[3]];
	if(type->type != C_TYPE_POS)
		return;
	count = type->size;
	output = &stack[type->pos];
	size = c_type_size_get(type->type);

	control = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_FLOAT, &control_stride, &control_count)];
	a = &stack[c_entity_stack_get(entity, c->references[1], type->type, &a_stride, &a_count)];
	b = &stack[c_entity_stack_get(entity, c->references[2], type->type, &b_stride, &b_count)];

	for(i = 0; i < count; i++)
	{
		a2 = &a[(i % a_count) * a_stride];
		b2 = &b[(i % b_count) * b_stride];
		f = control[(i % control_count) * control_stride];
		mid[0] = a2[0] + (b2[0] - a2[0]) * f;
		mid[1] = a2[1] + (b2[1] - a2[1]) * f;
		mid[2] = a2[2] + (b2[2] - a2[2]) * f;
		c_preview_draw_line(a2[0], a2[1], a2[2], mid[0], mid[1], mid[2]);
		c_preview_draw_line(b2[0], b2[1], b2[2], mid[0], mid[1], mid[2]);
	}

}

void c_execute_component_module_animation_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *out, *control, *a, *b, *time, t;
	uint i, j, k, count = 1, time_count, time_stride, a_count, a_stride, b_count, b_stride, size;
	CSpecialAnimation *anim;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;


	time = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_FLOAT, &time_stride, &time_count)];
	type = &entity->stack_types[c->references[0]];


	anim = c->special;

	/* what are we outputing */
	type = &entity->stack_types[c->references[anim->key_count + 1]];
	count = type->size;
	output = &stack[type->pos];

	size = c_type_size_get(type->type);

	for(i = 0; i < count; i++)
	{
		t = time[(i % time_count) * time_stride];
		
		for(j = 0; j < anim->key_count && anim->keys[j].key_time < t; j++);

		if(j == 0)
		{
			a = &stack[c_entity_stack_get(entity, c->references[1], type->type, &a_stride, &a_count)];
			for(k = 0; k < size; k++)
				output[i * size + k] = a[(i % a_count) * a_stride + k];
		}else if(j == anim->key_count)
		{
			a = &stack[c_entity_stack_get(entity, c->references[anim->key_count], type->type, &a_stride, &a_count)];
			for(k = 0; k < size; k++)
				output[i * size + k] = a[(i % a_count) * a_stride + k];
		}else
		{
			t = (t - anim->keys[j - 1].key_time) / (anim->keys[j].key_time - anim->keys[j - 1].key_time);
			t = f_splinef(t, 0.0, anim->keys[j - 1].key_ease[1], anim->keys[j].key_ease[0], 1.0);
			/* prev key */

			a = &stack[c_entity_stack_get(entity, c->references[j], type->type, &a_stride, &a_count)];

			/* next key */
			b = &stack[c_entity_stack_get(entity, c->references[j + 1], type->type, &b_stride, &b_count)];
			a = &a[(i % a_count) * a_stride];
			b = &b[(i % b_count) * b_stride];
			out = &output[i * size];
			for(k = 0; k < size; k++)
				out[k] = a[k] + (b[k] - a[k]) * t;
		}
	}
}

void c_execute_component_module_loop_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *speed, *spacing, *spread, f;
	uint i, count = 1, speed_count, spacing_count, spread_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[3]];
	count = type->size;
	output = &stack[type->pos];

	type = &entity->stack_types[c->references[0]];
	speed_count = type->size;
	speed = &stack[type->pos];

	type = &entity->stack_types[c->references[1]];
	spacing_count = type->size;
	spacing = &stack[type->pos];

	type = &entity->stack_types[c->references[2]];
	spread_count = type->size;
	spread = &stack[type->pos];


	for(i = 0; i < count; i++)
	{
		f = (float)i / (float)count;
		output[i] = instance->time * speed[i % speed_count] * (1 - f * spread[i % spread_count]) * 10.0 + spacing[i % spacing_count] * f;
		output[i] -= (float)((uint)(output[i]));
	}
}

void c_execute_component_module_event_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *speed, *spacing, *spread, f;
	uint i, count = 1, speed_count, spacing_count, spread_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[1]];
	count = type->size;
	output = &stack[type->pos];

	type = &entity->stack_types[c->references[0]];
	speed_count = type->size;
	speed = &stack[type->pos];

	for(i = 0; i < count; i++)
		output[i] = instance->time * speed[0];
	if(instance->time * speed[0] > 1.0)
		instance->ended = TRUE;
}


void c_execute_component_module_slow_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *input, *rise, *fall, f, *f2;
	uint i, j, size, output_count, output_stirde, input_count, input_stride, rise_count, rise_stride, fall_count, fall_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[3]];
	output_count = type->size;
	output = &stack[type->pos];
	size = c_type_size_get(type->type);
	
	input = &stack[c_entity_stack_get(entity, c->references[0], type->type, &input_stride, &input_count)];
	rise = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &rise_stride, &rise_count)];
	fall = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &fall_stride, &fall_count)];

	for(i = 0; i < output_count; i++)
	{
		for(j = 0; j < size; j++)
		{
			f = input[(i % input_count) * input_stride + j];
			f2 = &output[(i * size) + j];
			if(f > *f2)
			{
				*f2 += rise[(i % rise_count) * rise_stride] * session->delta_time;
				if(*f2 > f)
					*f2 = f; 
			}else if(f < *f2)
			{
				*f2 -= fall[(i % fall_count) * fall_stride] * session->delta_time;
				if(*f2 < f)
					*f2 = f; 
			}
		}
	}
}

void c_execute_component_module_time_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *speed, *spacing, *spread, f;
	uint i, count = 1, speed_count, spacing_count, spread_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;
	type = &entity->stack_types[c->references[3]];
	count = type->size;
	output = &stack[type->pos];

	type = &entity->stack_types[c->references[0]];
	speed_count = type->size;
	speed = &stack[type->pos];

	type = &entity->stack_types[c->references[1]];
	spacing_count = type->size;
	spacing = &stack[type->pos];

	type = &entity->stack_types[c->references[2]];
	spread_count = type->size;
	spread = &stack[type->pos];


	for(i = 0; i < count; i++)
	{
		f = (float)i / (float)count;
		output[i] = instance->time * speed[i % speed_count] * (1 - f * spread[i % spread_count]) * 10.0 + spacing[i % spacing_count] * f;
	}
}

void c_execute_component_module_animation_init(CSession *session)
{
	static CTypeType blend_param_types[3] = {C_TYPE_FLOAT, C_TYPE_SAME_FLOAT_AS_OUTPUT, C_TYPE_SAME_FLOAT_AS_OUTPUT};
	static char *blend_param_names[3] = {"Driver", "A", "B"};

	static CTypeType loop_param_types[3] = {C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *loop_param_names[3] = {"Speed", "Spacing", "Speed Spread"};
	static char *event_param_names[3] = {"Speed"};
	static CTypeType slow_param_types[3] = {C_TYPE_SAME_FLOAT_AS_OUTPUT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *slow_param_names[3] = {"Input", "Rise", "Fall"};

	confuse_component_add(session, "Animation", blend_param_types, blend_param_names, 3, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_animation_func, C_CC_PROCESS, C_SCT_ANIMATION, NULL);	
	confuse_component_add(session, "Blend", blend_param_types, blend_param_names, 3, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_blend_func, C_CC_PROCESS, -1, c_execute_component_module_blend_display_func);

	confuse_component_add(session, "Loop", loop_param_types, loop_param_names, 3, C_TYPE_FLOAT, -1, c_execute_component_module_loop_func, C_CC_TIMER, -1, NULL);
	confuse_component_add(session, "time", loop_param_types, loop_param_names, 3, C_TYPE_FLOAT, -1, c_execute_component_module_time_func, C_CC_TIMER, -1, NULL);
	confuse_component_add(session, "event", loop_param_types, event_param_names, 1, C_TYPE_FLOAT, -1, c_execute_component_module_event_func, C_CC_TIMER, -1, NULL);
	confuse_component_add(session, "slow", slow_param_types, slow_param_names, 3, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_slow_func, C_CC_TIMER, -1, NULL);

}