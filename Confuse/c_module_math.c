#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"


void c_execute_component_module_perlin_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *frequency, *a, *b, f;
	uint i, j, k, count = 1, control_count, frequency_count, a_count, b_count, size;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[4]]; 
	count = type->size;
	output = &stack[type->pos];

	type = &entity->stack_types[c->references[0]];
	control_count = type->size;
	control = &stack[type->pos];

	type = &entity->stack_types[c->references[1]];
	frequency_count = type->size;
	frequency = &stack[type->pos];
			
	type = &entity->stack_types[c->references[2]];
	a_count = type->size;
	a = &stack[type->pos];

	type = &entity->stack_types[c->references[3]];
	b_count = type->size;
	b = &stack[type->pos];

	size = c_type_size_get(type->type);

	count *= size;
	a_count *= size;
	b_count *= size;

	for(i = k = 0; i < count; k++)
	{
		f = 0.5 + 0.5 * f_noiserf(control[k % control_count] * frequency[k % frequency_count] * 100.0, 3);
		for(j = 0; j < size; j++)
		{
			output[i] = a[i % a_count] + (b[i % b_count] - a[i % a_count]) * f;
			i++;
		}
	}
}


void c_execute_component_module_sin_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *frequency, *a, *b, f;
	uint i, j, k, count = 1, control_count, frequency_count, a_count, b_count, size;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[4]]; 
	count = type->size;
	output = &stack[type->pos];

	type = &entity->stack_types[c->references[0]];
	control_count = type->size;
	control = &stack[type->pos];

	type = &entity->stack_types[c->references[1]];
	frequency_count = type->size;
	frequency = &stack[type->pos];
			
	type = &entity->stack_types[c->references[2]];
	a_count = type->size;
	a = &stack[type->pos];

	type = &entity->stack_types[c->references[3]];
	b_count = type->size;
	b = &stack[type->pos];

	size = c_type_size_get(type->type);

	count *= size;
	a_count *= size;
	b_count *= size;

	for(i = k = 0; i < count; k++)
	{
		f = 0.5 + 0.5 * sin(control[k % control_count] * frequency[k % frequency_count] * 100.0);
		for(j = 0; j < size; j++)
		{
			output[i] = a[i % a_count] + (b[i % b_count] - a[i % a_count]) * f;
			i++;
		}
	}
}


void c_execute_component_module_blink_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *frequency, *a, *b, *fade, f, f2;
	uint i, j, k, count = 1, control_count, frequency_count, a_count, b_count, fade_count, size;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[5]]; 
	count = type->size;
	output = &stack[type->pos];

	type = &entity->stack_types[c->references[0]];
	control_count = type->size;
	control = &stack[type->pos];

	type = &entity->stack_types[c->references[1]];
	frequency_count = type->size;
	frequency = &stack[type->pos];
			
	type = &entity->stack_types[c->references[2]];
	a_count = type->size;
	a = &stack[type->pos];

	type = &entity->stack_types[c->references[3]];
	b_count = type->size;
	b = &stack[type->pos];

	size = c_type_size_get(type->type);

	type = &entity->stack_types[c->references[4]];
	fade_count = type->size;
	fade = &stack[type->pos];


	count *= size;
	a_count *= size;
	b_count *= size;

	for(i = k = 0; i < count; k++)
	{
		f = control[k % control_count] * frequency[k % frequency_count] * 100.0;
		f -= (float)((int)f);
		f *= 2.0;
		if(f < 1.0)
		{
			f2 = fade[k % fade_count];
			if(f2 > f)
			{
				f /= f2;
				for(j = 0; j < size; j++)
				{
					output[i] = b[i % a_count] + (a[i % b_count] - b[i % a_count]) * f;
					i++;
				}
			}else
			{
				for(j = 0; j < size; j++)
				{
					output[i] = a[i % a_count];
					i++;
				}
			}
		}else
		{
			f = f - 1.0;
			f2 = fade[k % fade_count];
			if(f2 > f)
			{
				f /= f2;
				for(j = 0; j < size; j++)
				{
					output[i] = a[i % a_count] + (b[i % b_count] - a[i % a_count]) * f;
					i++;
				}
			}else
			{
				for(j = 0; j < size; j++)
				{
					output[i] = b[i % b_count];
					i++;
				}
			}
		}
	}
}

void c_execute_component_module_wiggle_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *frequency, *size, *center, f, add;
	uint i, j, k, count = 1, control_count, frequency_count, size_count, center_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[4]]; 
	count = type->size;
	output = &stack[type->pos];

	type = &entity->stack_types[c->references[0]];
	control_count = type->size;
	control = &stack[type->pos];

	type = &entity->stack_types[c->references[1]];
	frequency_count = type->size;
	frequency = &stack[type->pos];
			
	type = &entity->stack_types[c->references[2]];
	size_count = type->size;
	size = &stack[type->pos];

	type = &entity->stack_types[c->references[3]];
	center_count = type->size * 3;
	center = &stack[type->pos];

	count *= 3;
	add = 0.0;
	for(i = k = 0; i < count; k++)
	{
		f_wiggle3df(&output[i], control[k % control_count] * frequency[k % frequency_count] * 100.0 + add, size[k % size_count]);
		output[i] += center[i % center_count];
		i++;
		output[i] += center[i % center_count];
		i++;
		output[i] += center[i % center_count];
		i++;
		add += 3.12;
	}
}


void c_execute_component_module_wiggle_display_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *size, *center;
	uint size_count, center_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[2]];
	size_count = type->size;
	size = &stack[type->pos];

	type = &entity->stack_types[c->references[3]];
	center_count = type->size;
	center = &stack[type->pos];

	if(center_count == 1 && size_count == 1)
		c_preview_draw_sphere(center[0], center[1], center[2], size[0] * 2.0);
}


void c_execute_component_module_towards_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *base, *towards, *dist, f, vec[3], *tmp, *tmp2;
	uint i, j, k, count = 1, base_count, towards_count, dist_count, base_stride, towards_stride, dist_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[3]]; 
	count = type->size;
	output = &stack[type->pos];
	
	base = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &base_stride, &base_count)];
	towards = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &towards_stride, &towards_count)];
	dist = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &dist_stride, &dist_count)];

	for(i = j = 0; i < count; i++)
	{
		tmp = &base[(i % base_count) * base_stride];
		tmp2 = &towards[(i % towards_count) * towards_stride];
		vec[0] = tmp2[0] - tmp[0];
		vec[1] = tmp2[1] - tmp[1];
		vec[2] = tmp2[2] - tmp[2];
		f_normalize3f(vec);
		f = dist[(i % dist_count) * dist_stride];
		output[j++] = tmp[0] + vec[0] * f;
		output[j++] = tmp[1] + vec[1] * f;
		output[j++] = tmp[2] + vec[2] * f;
	}
}


void c_execute_component_module_distance_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *pos_a, *pos_b, *dist, f, vec[3], *tmp, *tmp2;
	uint i, j, k, count = 1, pos_a_count, pos_b_count, dist_count, pos_a_stride, pos_b_stride, dist_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[3]]; 
	count = type->size;
	output = &stack[type->pos];
	
	pos_a = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &pos_a_stride, &pos_a_count)];
	pos_b = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &pos_b_stride, &pos_b_count)];
	dist = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &dist_stride, &dist_count)];

	for(i = j = 0; i < count; i++)
	{
		tmp = &pos_a[(i % pos_a_count) * pos_a_stride];
		tmp2 = &pos_b[(i % pos_b_count) * pos_b_stride];
		vec[0] = tmp2[0] - tmp[0];
		vec[1] = tmp2[1] - tmp[1];
		vec[2] = tmp2[2] - tmp[2];
		output[j++] = f_length3f(vec) * dist[(i % dist_count) * dist_stride];
	}
}

extern void f_matrix_to_pos_quaternion_scalef(float *matrix, float *pos, float *quaternion, float *scale);
extern void f_pos_quaternion_scale_to_matrixf(float *pos, float *quaternion, float *scale, float *matrix);

void c_execute_component_module_qaternion_to_matrix_func(CSession *session, CInstance *instance, CComponent *component, uint primitive)
{
	float *output, *a, *pos;
	uint i, count, a_count, a_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[component->references[1]]; 
	count = type->size;
	output = &stack[type->pos];

	a = &stack[c_entity_stack_get(entity, component->references[0], C_TYPE_POS_QUATERNION_SIZE, &a_stride, &a_count)];
	
	for(i = 0; i < count; i++)
	{
		pos = &a[(i % a_count) * a_stride];
		f_pos_quaternion_scale_to_matrixf(pos, &pos[3], &pos[7], &output[i * 16]);
	}
}


void c_execute_component_module_matrix_to_qaternion_func(CSession *session, CInstance *instance, CComponent *component, uint primitive)
{
	float *output, *a, *pos;
	uint i, count, a_count, a_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[component->references[1]]; 
	count = type->size;
	output = &stack[type->pos];

	a = &stack[c_entity_stack_get(entity, component->references[0], C_TYPE_MATRIX, &a_stride, &a_count)];

	for(i = 0; i < count; i++)
	{
		pos = &a[(i % a_count) * a_stride];
		f_matrix_to_pos_quaternion_scalef(pos, &output[i * 10], &output[i * 10 + 3], &output[i * 10 + 7]);
	}
}

void c_execute_component_module_math_init(CSession *session)
{
	static CTypeType perlin_param_types[5] = {C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_SAME_FLOAT_AS_OUTPUT, C_TYPE_SAME_FLOAT_AS_OUTPUT, C_TYPE_FLOAT};
	static char *perlin_param_names[5] = {"Driver", "Frequency", "A", "B", "Fader"};
	
	static CTypeType wiggle_param_types[4] = {C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_POS};
	static char *wiggle_param_names[4] = {"Driver", "Frequency", "Size", "Center"};
	
	static CTypeType towards_param_types[3] = {C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT};
	static char *towards_param_names[3] = {"Base", "goal", "distance"};
	
	static CTypeType distance_param_types[3] = {C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT};
	static char *distance_param_names[3] = {"Pos A", "Pos B", "distance"};

	static CTypeType q2m_param_types = C_TYPE_POS_QUATERNION_SIZE;
	static char *q2m_param_names = "Quaternion";

	static CTypeType m2q_param_types = C_TYPE_MATRIX;
	static char *m2q_param_names = "Matrix";
	// time
	// * loop
	// * blink
	// * sin
	// distance / inv 
	// towards
	// Rotate UV
	confuse_component_add(session, "Perlin", perlin_param_types, perlin_param_names, 4, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_perlin_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "Wiggle", wiggle_param_types, wiggle_param_names, 4, C_TYPE_POS, -1, c_execute_component_module_wiggle_func, C_CC_PROCESS, -1, c_execute_component_module_wiggle_display_func);
	confuse_component_add(session, "Sin", perlin_param_types, perlin_param_names, 4, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_sin_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "blink", perlin_param_types, perlin_param_names, 5, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_blink_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "towards", towards_param_types, towards_param_names, 3, C_TYPE_POS, -1, c_execute_component_module_towards_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "distance", distance_param_types, distance_param_names, 3, C_TYPE_FLOAT, -1, c_execute_component_module_distance_func, C_CC_PROCESS, -1, NULL);

	confuse_component_add(session, "Q2M", &q2m_param_types, &q2m_param_names, 1, C_TYPE_MATRIX, -1, c_execute_component_module_qaternion_to_matrix_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "M2Q", &m2q_param_types, &m2q_param_names, 1, C_TYPE_POS_QUATERNION_SIZE, -1, c_execute_component_module_matrix_to_qaternion_func, C_CC_PROCESS, -1, NULL);


}




