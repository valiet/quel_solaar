#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"

void c_execute_component_module_random_init_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *frequency, *a, *b, f, tmp;
	uint i, j, k, count = 1, size, a_count, b_count, a_stride, b_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[2]]; 
	count = type->size; 
	output = &stack[type->pos];
	
	a = &stack[c_entity_stack_get(entity, c->references[0], type->type, &a_stride, &a_count)];
	b = &stack[c_entity_stack_get(entity, c->references[1], type->type, &b_stride, &b_count)];

	size = c_type_size_get(type->type);
	a_count *= size;
	b_count *= size;
	
	for(i = 0; i < count; i++)
	{
		f = f_randf(instance->seed * count + i);
		for(k = 0; k < size; k++)
		{
			tmp = a[(i % b_count) * b_stride + k];
		//	printf("RANDOM %f %f %f", tmp, (b[(i % b_count) * b_stride + k] - tmp), f);
			output[i * size + k] = tmp + (b[(i % b_count) * b_stride + k] - tmp) * f;
			i++;
		}
	}
}

void c_execute_component_module_random_init_pos(float *pos, float *center, float size, uint *seed)
{
	float dist = 10.0;
	while(dist > 1.0)
	{
		pos[0] = f_randnf((*seed)++);
		pos[1] = f_randnf((*seed)++);
		pos[2] = f_randnf((*seed)++);
		dist = pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];
	}
	pos[0] = pos[0] * size + center[0]; 
	pos[1] = pos[1] * size + center[1]; 
	pos[2] = pos[2] * size + center[2]; 
}

void c_execute_component_module_sphere_init_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *frequency, *a, *b, f, tmp;
	uint i, j, k, count = 1, size, a_count, b_count, a_stride, b_stride, seed, *ioutput;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[2]]; 
	count = type->size; 
	output = &stack[type->pos];
	ioutput = (uint *)output;
	
	a = &stack[c_entity_stack_get(entity, c->references[0],  type->type, &a_stride, &a_count)];
	b = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &b_stride, &b_count)];

	seed = type->size * instance->seed;
	size = c_type_size_get(type->type);

	switch(type->type)
	{
		case C_TYPE_FLOAT :
		for(i = 0; i < count; i++)
		{
			output[i] = a[(i % a_count) * a_stride] + f_randnf(seed++) * b[(i % b_count) * b_stride];
		}
		break;
		case C_TYPE_UV :
		for(i = 0; i < count; i++)
		{
			output[i * 2 + 0] = a[(i % a_count) * a_stride + 0] + f_randnf(seed++) * b[(i % b_count) * b_stride + 0];
			output[i * 2 + 1] = a[(i % a_count) * a_stride + 1] + f_randnf(seed++) * b[(i % b_count) * b_stride + 1];
		}
		break;
		case C_TYPE_POS :
		for(i = 0; i < count; i++)
			c_execute_component_module_random_init_pos(&output[i * 3], &a[(i % a_count) * a_stride], b[(i % b_count) * b_stride], &seed);
		break;
		case C_TYPE_COLOR :
		for(i = 0; i < count; i++)
		{
			f = b[(i % b_count) * b_stride];
			output[i * 4 + 0] = a[(i % a_count) * a_stride + 0] + f_randnf(seed++) * f;
			output[i * 4 + 1] = a[(i % a_count) * a_stride + 1] + f_randnf(seed++) * f;
			output[i * 4 + 2] = a[(i % a_count) * a_stride + 2] + f_randnf(seed++) * f;
			output[i * 4 + 3] = a[(i % a_count) * a_stride + 3] + f_randnf(seed++) * f;
		}
		break;
		case C_TYPE_MATRIX :
		for(i = 0; i < count; i++)
		{
			f = b[(i % b_count) * b_stride];
			output[i * 16 + 0] = a[(i % a_count) * a_stride + 0];
			output[i * 16 + 1] = a[(i % a_count) * a_stride + 1];
			output[i * 16 + 2] = a[(i % a_count) * a_stride + 2];
			output[i * 16 + 3] = a[(i % a_count) * a_stride + 3];
			output[i * 16 + 4] = a[(i % a_count) * a_stride + 4];
			output[i * 16 + 5] = a[(i % a_count) * a_stride + 5];
			output[i * 16 + 6] = a[(i % a_count) * a_stride + 6];
			output[i * 16 + 7] = a[(i % a_count) * a_stride + 7];
			output[i * 16 + 8] = a[(i % a_count) * a_stride + 8];
			output[i * 16 + 9] = a[(i % a_count) * a_stride + 9];
			output[i * 16 + 10] = a[(i % a_count) * a_stride + 10];
			output[i * 16 + 11] = a[(i % a_count) * a_stride + 11];
			c_execute_component_module_random_init_pos(&output[i * 16 + 12], &a[(i % a_count) * a_stride], b[(i % b_count) * b_stride], &seed);
			output[i * 16 + 15] = a[(i % a_count) * a_stride + 15];
		}
		break;
		case C_TYPE_AREA :
		for(i = 0; i < count; i++)
		{
			f = 1.0 - b[(i % b_count) * b_stride] * f_randf(seed++);
			output[i * 2 + 0] = a[((i % a_count) * a_stride) * 2 + 0] * f;
			output[i * 2 + 1] = a[((i % a_count) * a_stride) * 2 + 1] * f;
		}
		break;
		case C_TYPE_POS_QUATERNION_SIZE :
		for(i = 0; i < count; i++)
		{
			f = b[(i % b_count) * b_stride];
			c_execute_component_module_random_init_pos(&output[i * 10], &a[(i % a_count) * a_stride], b[(i % b_count) * b_stride], &seed);
			output[i * 10 + 3] = a[((i % a_count) * a_stride) * 10 + 3];
			output[i * 10 + 4] = a[((i % a_count) * a_stride) * 10 + 4];
			output[i * 10 + 5] = a[((i % a_count) * a_stride) * 10 + 5];
			output[i * 10 + 6] = a[((i % a_count) * a_stride) * 10 + 6];
			output[i * 10 + 7] = a[((i % a_count) * a_stride) * 10 + 7];
			output[i * 10 + 8] = a[((i % a_count) * a_stride) * 10 + 8];
			output[i * 10 + 9] = a[((i % a_count) * a_stride) * 10 + 9];
		}
		break;
		case C_TYPE_UV_SECTION :
		for(i = 0; i < count; i++)
		{
			f = 1.0 - b[(i % b_count) * b_stride] * f_randf(seed++);
			output[i * 2 + 0] = a[((i % a_count) * a_stride) * 2 + 0] * f;
			output[i * 2 + 1] = a[((i % a_count) * a_stride) * 2 + 1] * f;
		}

	}
}

void c_execute_component_module_sphere_init_display_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *control, *frequency, *a, *b, f, tmp;
	uint i, j, k, count = 1, size, a_count, b_count, a_stride, b_stride, seed, *ioutput;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[2]]; 
	if(type->type != C_TYPE_POS)
		return;
	count = type->size; 
	output = &stack[type->pos];
	ioutput = (uint *)output;
	
	a = &stack[c_entity_stack_get(entity, c->references[0],  type->type, &a_stride, &a_count)];
	b = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &b_stride, &b_count)];
	if(a_count == 1 && b_count == 1)
		c_preview_draw_sphere(a[0], a[1], a[2], b[0]);
}



void c_execute_component_module_random_matrix_init_func(CSession *session, CInstance *instance, CComponent *component, uint primitive)
{
	float *output, *control, *frequency, *a, *b, *c, *d, *e, *matrix, f, scale[3], tmp[3], vec_x[3], vec_y[3];
	uint i, j, k, count = 1, size, a_count, b_count, c_count, d_count, e_count, a_stride, b_stride, c_stride, d_stride, e_stride, seed;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[component->references[5]]; 
	count = type->size; 
	output = &stack[type->pos];
	
	a = &stack[c_entity_stack_get(entity, component->references[0], C_TYPE_MATRIX, &a_stride, &a_count)];
	b = &stack[c_entity_stack_get(entity, component->references[1], C_TYPE_FLOAT, &b_stride, &b_count)];
	c = &stack[c_entity_stack_get(entity, component->references[2], C_TYPE_FLOAT, &c_stride, &c_count)];
	d = &stack[c_entity_stack_get(entity, component->references[3], C_TYPE_FLOAT, &d_stride, &d_count)];
	e = &stack[c_entity_stack_get(entity, component->references[4], C_TYPE_FLOAT, &e_stride, &e_count)];

	seed = type->size * instance->seed;
	size = c_type_size_get(type->type);

	for(i = 0; i < count; i++)
	{
		matrix = &a[(i % a_count) * a_stride];
		scale[0] = f_length3f(&matrix[0]);
		scale[1] = f_length3f(&matrix[4]);
		scale[2] = f_length3f(&matrix[8]);
		f = 1 + f_randnf(seed++) * d[(i % d_count) * d_stride];
		scale[0] *= f * (1 + f_randnf(seed++) * e[(i % e_count) * e_stride]);
		scale[1] *= f * (1 + f_randnf(seed++) * e[(i % e_count) * e_stride]);
		scale[2] *= f * (1 + f_randnf(seed++) * e[(i % e_count) * e_stride]);
		tmp[0] = matrix[0];
		tmp[1] = matrix[1];
		tmp[2] = matrix[2];
		f_normalize3f(tmp);
		f = 1.0 - c[(i % c_count) * c_stride];
		tmp[0] *= f;
		tmp[1] *= f;
		tmp[2] *= f;
		c_execute_component_module_random_init_pos(vec_x, tmp, 1 - f, &seed);
		tmp[0] = matrix[4];
		tmp[1] = matrix[5];
		tmp[2] = matrix[6];
		f_normalize3f(tmp);
		tmp[0] *= f;
		tmp[1] *= f;
		tmp[2] *= f;
		c_execute_component_module_random_init_pos(vec_y, tmp, 1 - f, &seed);
		f_matrixxyf(&output[i * 16], NULL, vec_x, vec_y);
		c_execute_component_module_random_init_pos(tmp, &matrix[12], b[(i % b_count) * b_stride], &seed);
		output[i * 16 + 0] *= scale[0];
		output[i * 16 + 1] *= scale[0];
		output[i * 16 + 2] *= scale[0];
		
		output[i * 16 + 4] *= scale[1];
		output[i * 16 + 5] *= scale[1];
		output[i * 16 + 6] *= scale[1];

		output[i * 16 + 8] *= scale[2];
		output[i * 16 + 9] *= scale[2];
		output[i * 16 + 10] *= scale[2];
		
		output[i * 16 + 12] = tmp[0];
		output[i * 16 + 13] = tmp[1];
		output[i * 16 + 14] = tmp[2];
	}
}

void c_execute_component_module_random_quaternion_init_func(CSession *session, CInstance *instance, CComponent *component, uint primitive)
{
	float *output, *control, *frequency, *a, *b, *c, *d, *e, *pqs, f, matrix[16], tmp[3], vec_x[3], vec_y[3];
	uint i, j, k, count = 1, size, a_count, b_count, c_count, d_count, e_count, a_stride, b_stride, c_stride, d_stride, e_stride, seed;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[component->references[5]]; 
	count = type->size; 
	output = &stack[type->pos];
	
	a = &stack[c_entity_stack_get(entity, component->references[0], C_TYPE_POS_QUATERNION_SIZE, &a_stride, &a_count)];
	b = &stack[c_entity_stack_get(entity, component->references[1], C_TYPE_FLOAT, &b_stride, &b_count)];
	c = &stack[c_entity_stack_get(entity, component->references[2], C_TYPE_FLOAT, &c_stride, &c_count)];
	d = &stack[c_entity_stack_get(entity, component->references[3], C_TYPE_FLOAT, &d_stride, &d_count)];
	e = &stack[c_entity_stack_get(entity, component->references[4], C_TYPE_FLOAT, &e_stride, &e_count)];

	seed = type->size * instance->seed;
	size = c_type_size_get(type->type);

	for(i = 0; i < count; i++)
	{
		pqs = &a[(i % a_count) * a_stride];
		f = 1 + f_randnf(seed++) * d[(i % d_count) * d_stride];
		output[i * 10 + 7] = pqs[7] * f * (1 + f_randnf(seed++) * e[(i % e_count) * e_stride]);
		output[i * 10 + 8] = pqs[8] * f * (1 + f_randnf(seed++) * e[(i % e_count) * e_stride]);
		output[i * 10 + 9] = pqs[9] * f * (1 + f_randnf(seed++) * e[(i % e_count) * e_stride]);

		output[i * 10 + 3] = pqs[3];
		output[i * 10 + 4] = pqs[4];
		output[i * 10 + 5] = pqs[5];
		output[i * 10 + 6] = pqs[6];

		f_quaternion_to_matrixf(matrix, pqs[3], pqs[4], pqs[5], pqs[6]);
		tmp[0] = matrix[0];
		tmp[1] = matrix[1];
		tmp[2] = matrix[2];
		f_normalize3f(tmp);
		f = 1.0 - c[(i % c_count) * c_stride];
		tmp[0] *= f;
		tmp[1] *= f;
		tmp[2] *= f;
		c_execute_component_module_random_init_pos(vec_x, tmp, 1 - f, &seed);
		tmp[0] = matrix[4];
		tmp[1] = matrix[5];
		tmp[2] = matrix[6];
		f_normalize3f(tmp);
		tmp[0] *= f;
		tmp[1] *= f;
		tmp[2] *= f;
		c_execute_component_module_random_init_pos(vec_y, tmp, 1 - f, &seed);
		f_matrixxyf(matrix, NULL, vec_x, vec_y);
		f_matrix_to_quaternionf(&output[i * 10 + 3], matrix);

		c_execute_component_module_random_init_pos(&output[i * 10], pqs, b[(i % b_count) * b_stride], &seed);
	}
}

void c_execute_component_module_select_image_2d_func(CSession *session, CInstance *instance, CComponent *component, uint primitive)
{
	float *control, f;
	uint i, j, k, *stack, *output, count = 1, size, found, selected, control_stride, control_count, *ignore_empty, ignore_empty_stride, ignore_empty_count, *option[8], option_count[8], option_stride[8];
	CType *type;
	CEntity *entity;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[component->references[10]]; 
	count = type->size; 
	output = &stack[type->pos];
	
	control = &stack[c_entity_stack_get(entity, component->references[0], C_TYPE_FLOAT, &control_stride, &control_count)];
	ignore_empty = &stack[c_entity_stack_get(entity, component->references[1], C_TYPE_STATE_DEPTH_TEST, &ignore_empty_stride, &ignore_empty_count)];

	for(i = 0; i < 8; i++)
		option[i] = &stack[c_entity_stack_get(entity, component->references[2 + i], C_TYPE_IMAGE_2D, &option_stride[i], &option_count[i])];

	size = c_type_size_get(type->type);
	for(i = 0; i < count; i++)
	{
		if(ignore_empty[(i % ignore_empty_count) * ignore_empty_stride])
		{
			
			printf("AA\n");
			for(j = found = 0; j < 8; j++)
			{
				printf("option[%u] %u\n", j, option[j][(i % option_count[j]) * option_stride[j]]);
				if(option[j][(i % option_count[j]) * option_stride[j]] != -1)
					found++;
			}
			printf("found %u\n", found);
			if(found != 0)
			{
				f = control[(i % control_count) * control_stride] * (float)found;
				
				printf("CONTROL %f\n", control[(i % control_count) * control_stride]);
				found--;
				if(f < 0.0)
					selected = 0;
				else 
				{
					selected = (uint)f;
					if(selected > found)
						selected = found;
				}
				for(j = found = 0; found < selected ; j++)
					if(option[j][(i % option_count[j]) * option_stride[j]] != -1)
						found++;
	
				printf("BB %u, %u\n", found, selected);
				for(j = 0; j < size; j++)
					output[i * size + j] = option[selected][(i % option_count[selected] + j) * option_stride[selected]];
				printf("output %u\n", output[i * size]);
			}else
				for(j = 0; j < size; j++)
					output[i * size + j] = option[0][(i % option_count[0] + j) * option_stride[0]];
		}else
		{
			f = control[(i % control_count) * control_stride] * 8.0;
			printf("CONTROL %f\n", control[(i % control_count) * control_stride]);
			if(f < 0.0)
				selected = 0;
			else if(f > 7.0)
				selected = 7;
			else
				selected = (uint)f;
			for(j = 0; j < size; j++)
				output[i * size + j] = option[selected][(i % option_count[selected] + j) * option_stride[selected]];
			printf("output %u\n", output[i * size]);
			printf("CC %u\n", selected);
		}
	}
}



void c_execute_component_module_circle_init_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *matrix, *m, *rotate, *section, *size, *spiral, x, y, z, f, distance, add, s, vec[9];
	uint i, j, k, count = 1, size_stride, size_count, section_stride, section_count, rotate_stride, rotate_count, matrix_stride, matrix_count, spiral_stride, spiral_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[5]]; 
	count = type->size; 
	output = &stack[type->pos];
	
	matrix = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_MATRIX, &matrix_stride, &matrix_count)]; // matrix
	rotate = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_FLOAT, &rotate_stride, &rotate_count)]; // rotate
	section = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &section_stride, &section_count)]; // angle
	size = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &size_stride, &size_count)]; // size

	spiral = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &spiral_stride, &spiral_count)]; // size


	add = 1.0 / (float)count;
	distance = 0;
	if(matrix_count == 1 && rotate_count == 1 && section_count == 1 && size_count == 1 && spiral_count == 1)
	{
		f = rotate[0] * 2 * PI;
		add *= 2 * PI * section[0];
		z = 0;
		m = matrix;
		vec[0] = m[0] * *size;
		vec[1] = m[1] * *size;
		vec[2] = m[2] * *size;
		vec[3] = m[4] * *size;
		vec[4] = m[5] * *size;
		vec[5] = m[6] * *size;
		vec[6] = m[8] * spiral[0] / (float)count;
		vec[7] = m[9] * spiral[0] / (float)count;
		vec[8] = m[10] * spiral[0] / (float)count;
		for(i = 0; i < count; i++)
		{
			x = sin(f);
			y = cos(f);
			*output++ = vec[0] * x + vec[3] * y + vec[6] * z + m[12];
			*output++ = vec[1] * x + vec[4] * y + vec[7] * z + m[13];
			*output++ = vec[2] * x + vec[5] * y + vec[8] * z + m[14];
			z++;
			f += add;
		}	
	}else
	{
		for(i = 0; i < count; i++)
		{
			f = rotate[(i % rotate_count) * rotate_stride];
			f += distance * section[(i % section_count) * section_stride];
			f *= 2 * PI;
			x = sin(f);
			y = cos(f);
			z = spiral[(i % spiral_count) * spiral_stride] * distance;
			m = &matrix[(i % matrix_count) * matrix_stride];
			s = size[(i % size_count) * size_stride];
			*output++ = (m[0] * x + m[4] * y) * s + m[6] * z + m[12];
			*output++ = (m[1] * x + m[5] * y) * s + m[7] * z + m[13];
			*output++ = (m[2] * x + m[6] * y) * s + m[8] * z + m[14];
			distance += add;
		}
	}
}

/*
init random A->B
init random select

pos cloud
pos sphere
fill shape
*/

void c_execute_component_module_initialization_init(CSession *session)
{
	static CTypeType random_param_types[2] = {C_TYPE_SAME_FLOAT_AS_OUTPUT, C_TYPE_SAME_FLOAT_AS_OUTPUT};
	static char *random_param_names[2] = {"A", "B"};
	static CTypeType sphere_param_types[2] = {C_TYPE_SAME_FLOAT_AS_OUTPUT, C_TYPE_FLOAT};
	static char *sphere_param_names[2] = {"Center", "Size"};
	static CTypeType matrix_param_types[5] = {C_TYPE_MATRIX, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *matrix_param_names[5] = {"transform", "Position", "Rotation", "Size", "Aspect"};
	static CTypeType quaternion_param_types[5] = {C_TYPE_POS_QUATERNION_SIZE, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *quaternion_param_names[5] = {"transform", "Position", "Rotation", "Size", "Aspect"};

	static CTypeType circle_param_types[5] = {C_TYPE_MATRIX, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *circle_param_names[5] = {"Matrix", "Rotate", "Section", "Size", "Spiral"};

	static CTypeType select_param_types[10] = {C_TYPE_FLOAT, C_TYPE_STATE_DEPTH_TEST, C_TYPE_IMAGE_2D, C_TYPE_IMAGE_2D, C_TYPE_IMAGE_2D, C_TYPE_IMAGE_2D, C_TYPE_IMAGE_2D, C_TYPE_IMAGE_2D, C_TYPE_IMAGE_2D, C_TYPE_IMAGE_2D};
	static char *select_param_names[10] = {"selector", "Ignore Empty", "A", "B", "C", "D", "E", "F", "G", "H"};
	
	confuse_component_add(session, "random init", random_param_types, random_param_names, 2, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_random_init_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "random sphere", sphere_param_types, sphere_param_names, 2, C_TYPE_SAME_FLOAT_AS_OUTPUT, -1, c_execute_component_module_sphere_init_func, C_CC_PROCESS, -1, c_execute_component_module_sphere_init_display_func);
	confuse_component_add(session, "random transform", matrix_param_types, matrix_param_names, 5, C_TYPE_MATRIX, -1, c_execute_component_module_random_matrix_init_func,C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "random quaternion", quaternion_param_types, quaternion_param_names, 5, C_TYPE_POS_QUATERNION_SIZE, -1, c_execute_component_module_random_quaternion_init_func, C_CC_PROCESS, -1, NULL);

	confuse_component_add(session, "circle", circle_param_types, circle_param_names, 5, C_TYPE_POS, -1, c_execute_component_module_circle_init_func, C_CC_PROCESS, -1, NULL);

	confuse_component_add(session, "image select", select_param_types, select_param_names, 10, C_TYPE_IMAGE_2D, -1, c_execute_component_module_select_image_2d_func, C_CC_PROCESS, -1, NULL);
	
}

