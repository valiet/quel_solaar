#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"

float c_primitives_matrix[9] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

void confuse_view_matrix_set(float *matrix)
{
	c_primitives_matrix[0] = matrix[0];
	c_primitives_matrix[3] = matrix[1];
	c_primitives_matrix[6] = matrix[2];

	c_primitives_matrix[1] = matrix[4];
	c_primitives_matrix[4] = matrix[5];
	c_primitives_matrix[7] = matrix[6];

	c_primitives_matrix[2] = matrix[8];
	c_primitives_matrix[5] = matrix[9];
	c_primitives_matrix[8] = matrix[10];
}



void c_execute_component_primitive_qaud_params(CSession *session, CInstance *instance, CComponent *c, uint primitive, uint count)
{
	CDrawState *p;
	float *buffer, *b;
	float *size, *data, *data_b, *draw, vec[6] = {0.01, 0, 0, 0, 0.01, 0}, f, f2;
	uint i, j, k, used, stride, data_size, size_count, data_stride, draw_size, draw_stride, test = 0;
	CType *type;
	CEntity *entity;
	float *stack;
	CComponentDef *comp_def;
	entity = instance->entity;
	stack = instance->stack;
	p = &session->prim[primitive];
	buffer = p->attrib_buffer;
	stride = p->attrib_stride;
	buffer = &buffer[stride * p->attrib_used];
	comp_def = &session->component_list[c->type];
	draw = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count], C_TYPE_BOOLEAN, &draw_stride, &draw_size)];
	for(i = 0; i < p->attrib_param_count; i++)
	{
		data = &stack[c_entity_stack_get(entity, c->references[i], p->attrib_params[i].type, &data_stride, &data_size)];
		if(&((float *)p->attrib_buffer)[stride * p->attrib_used + test] != buffer)
			j = 0;
		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				if(data_size == 1)
				{
					f = *data;
					b = buffer;
					for(j = k = 0; j < count; j++)
					{
						f2 = draw[(j % draw_size) * draw_stride];
						if(f2 <= 1.0 && f2 >= 0.0)
						{
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
						}
					}
				}else
				{
					b = buffer;
					for(j = k = 0; j < count; j++)
					{
						f = draw[(j % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							f = data[(j % data_size) * data_stride];
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
							*b = f;
							b += stride;
						}
					}
				}
				test += 1;
				buffer += 1;
			break;
				case C_TYPE_UV :
				case C_TYPE_UV_SECTION :
				if(data_size == 1)
				{
					
					b = buffer;
					for(j = k = 0; k < count; k++)
					{
						f = draw[(k % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							*b = data[0];
							b[1] = data[1];
							b += stride;
							*b = data[2];
							b[1] = data[3];
							b += stride;
							*b = data[4];
							b[1] = data[5];
							b += stride;
							*b = data[0];
							b[1] = data[1];
							b += stride;
							*b = data[4];
							b[1] = data[5];
							b += stride;
							*b = data[6];
							b[1] = data[7];
							b += stride;
						}
					}
				}else
				{
					b = buffer;
					for(j = k = 0; k < count; k++)
					{
						f = draw[(k % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							data_b = &data[(k % data_size) * data_stride];
							*b = data_b[0];
							b[1] = data_b[1];
							b += stride;
							*b = data_b[2];
							b[1] = data_b[3];
							b += stride;
							*b = data_b[4];
							b[1] = data_b[5];
							b += stride;
							*b = data_b[0];
							b[1] = data_b[1];
							b += stride;
							*b = data_b[4];
							b[1] = data_b[5];
							b += stride;
							*b = data_b[6];
							b[1] = data_b[7];
							b += stride;
						}
					}
				}
			test += 2;
			buffer += 2;
			break;
			case C_TYPE_POS :
			test += 3;
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				if(data_size == 1)
				{
					b = buffer;
					for(j = k = 0; j < count; j++)
					{
						f = draw[(j % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							*b = data[0];
							b[1] = data[1];
							b[2] = data[2];
							b[3] = data[3];
							b += stride;
							*b = data[0];
							b[1] = data[1];
							b[2] = data[2];
							b[3] = data[3];
							b += stride;
							*b = data[0];
							b[1] = data[1];
							b[2] = data[2];
							b[3] = data[3];
							b += stride;
							*b = data[0];
							b[1] = data[1];
							b[2] = data[2];
							b[3] = data[3];
							b += stride;
							*b = data[0];
							b[1] = data[1];
							b[2] = data[2];
							b[3] = data[3];
							b += stride;
							*b = data[0];
							b[1] = data[1];
							b[2] = data[2];
							b[3] = data[3];
							b += stride;
						}
					}
				}else
				{
					b = buffer;
					for(j = k = 0; j < count; j++)
					{
						f = draw[(j % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							data_b = &data[(j % data_size) * data_stride];
							*b = data_b[0];
							b[1] = data_b[1];
							b[2] = data_b[2];
							b[3] = data_b[3];
							b += stride;
							*b = data_b[0];
							b[1] = data_b[1];
							b[2] = data_b[2];
							b[3] = data_b[3];
							b += stride;
							*b = data_b[0];
							b[1] = data_b[1];
							b[2] = data_b[2];
							b[3] = data_b[3];
							b += stride;
							*b = data_b[0];
							b[1] = data_b[1];
							b[2] = data_b[2];
							b[3] = data_b[3];
							b += stride;
							*b = data_b[0];
							b[1] = data_b[1];
							b[2] = data_b[2];
							b[3] = data_b[3];
							b += stride;
							*b = data_b[0];
							b[1] = data_b[1];
							b[2] = data_b[2];
							b[3] = data_b[3];
							b += stride;
						}
					}
				}
				test += 4;
				buffer += 4;
			break;
		}
	}
}

void c_execute_component_primitive_sprite_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	CDrawState *p;
	float *buffer;
	float *size, *data, *data_b, *draw, vec[6] = {0.01, 0, 0, 0, 0.01, 0}, f;
	uint i, j, k, count = 1, used, stride, data_size, size_stride, size_count, data_stride, draw_size, draw_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	CComponentDef *comp_def;
	entity = instance->entity;
	stack = instance->stack;

	p = &session->prim[primitive];
	buffer = p->attrib_buffer;
	stride = p->attrib_stride;
	buffer = &buffer[stride * p->attrib_used];

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(p->attrib_params[i].type == C_TYPE_POS)
		{
			if(type->size > count)
				count = type->size;
		}
	}
	comp_def = &session->component_list[c->type];
	draw = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count], C_TYPE_BOOLEAN, &draw_stride, &draw_size)];
	if(draw_size == 1)
	{
		if(draw[0] > 1.0  || draw[0] < 0.0)
			return;
		if(count > (p->buffer_size - p->attrib_used) / 6)
			count = (p->buffer_size - p->attrib_used) / 6;
		if(count == 0)
			return;
		used = count;
	}else
	{
		if(count > (p->buffer_size - p->attrib_used) / 6)
		{
			for(i = used = 0; i < count; i++)
			{
				f = draw[(i % draw_size) * draw_stride];
				if(f <= 1.0 && f >= 0.0)
				{	
					if(used == (p->buffer_size - p->attrib_used) / 6)
						break;
					used++;
				}
			}
			count = i;
		}else
		{
			for(i = used = 0; i < count; i++)
				if(draw[(i % draw_size) * draw_stride] <= 1.0 && draw[(i % draw_size) * draw_stride] >= 0.0)
					used++;
		}
	}
	if(count > (p->buffer_size - p->attrib_used) / 6)
		count = (p->buffer_size - p->attrib_used) / 6;
	if(count == 0)
		return;

	/* sprite size */
	type = &entity->stack_types[c->references[p->attrib_param_count]];
	size = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count + 1], C_TYPE_AREA, &size_stride, &size_count)];
	if(size_count == 1)
	{
		vec[0] = c_primitives_matrix[0] * size[0];
		vec[1] = c_primitives_matrix[1] * size[0];
		vec[2] = c_primitives_matrix[2] * size[0];
		vec[3] = c_primitives_matrix[3] * size[1];
		vec[4] = c_primitives_matrix[4] * size[1];
		vec[5] = c_primitives_matrix[5] * size[1];
	}
	c_execute_component_primitive_qaud_params(session, instance, c, primitive, count);
	for(i = 0; i < p->attrib_param_count; i++)
	{
		data = &stack[c_entity_stack_get(entity, c->references[i], p->attrib_params[i].type, &data_stride, &data_size)];

		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				buffer += 1;
			break;
			case C_TYPE_UV :
			case C_TYPE_UV_SECTION :
				buffer += 2;
			break;
			case C_TYPE_POS :
			{
				for(j = k = 0; k < count * data_stride; k += data_stride)
				{
					f = draw[((k / data_stride) % draw_size) * draw_stride];
					if(f <= 1.0 && f >= 0.0)
					{
						f = size[((k / data_stride) % size_count) * size_stride + 0];
						vec[0] = c_primitives_matrix[0] * f;
						vec[1] = c_primitives_matrix[1] * f;
						vec[2] = c_primitives_matrix[2] * f;
						f = size[((k / data_stride) % size_count) * size_stride + 1];
						vec[3] = c_primitives_matrix[3] * f;
						vec[4] = c_primitives_matrix[4] * f;
						vec[5] = c_primitives_matrix[5] * f;

						buffer[j + 0] = data[k] - vec[0] - vec[3];
						buffer[j + 1] = data[k + 1] - vec[1] - vec[4];
						buffer[j + 2] = data[k + 2] - vec[2] - vec[5];
						j += stride;
						buffer[j + 0] = data[k] + vec[0] - vec[3];
						buffer[j + 1] = data[k + 1] + vec[1] - vec[4];
						buffer[j + 2] = data[k + 2] + vec[2] - vec[5];
						j += stride;
						buffer[j + 0] = data[k] + vec[0] + vec[3];
						buffer[j + 1] = data[k + 1] + vec[1] + vec[4];
						buffer[j + 2] = data[k + 2] + vec[2] + vec[5];
						j += stride;
						buffer[j + 0] = data[k] - vec[0] - vec[3];
						buffer[j + 1] = data[k + 1] - vec[1] - vec[4];
						buffer[j + 2] = data[k + 2] - vec[2] - vec[5];
						j += stride;
						buffer[j + 0] = data[k] + vec[0] + vec[3];
						buffer[j + 1] = data[k + 1] + vec[1] + vec[4];
						buffer[j + 2] = data[k + 2] + vec[2] + vec[5];
						j += stride;
						buffer[j + 0] = data[k] - vec[0] + vec[3];
						buffer[j + 1] = data[k + 1] - vec[1] + vec[4];
						buffer[j + 2] = data[k + 2] - vec[2] + vec[5];
						j += stride;
					}
				}
			}
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				buffer += 4;
			break;
		}
	}
	p->attrib_used += used * 6;
}


void c_execute_component_primitive_collapsed_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	CDrawState *p;
	float *buffer;
	float  *data, *data_b, *draw, vec[6] = {0.01, 0, 0, 0, 0.01, 0}, f;
	uint i, j, k, l, count = 1, used, stride, data_size,  data_stride, draw_size, draw_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	CComponentDef *comp_def;
	entity = instance->entity;
	stack = instance->stack;

	p = &session->prim[primitive];
	buffer = p->attrib_buffer;
	stride = p->attrib_stride;
	buffer = &buffer[stride * p->attrib_used];

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(p->attrib_params[i].type == C_TYPE_POS)
		{
			if(type->size > count)
				count = type->size;
		}
	}
	comp_def = &session->component_list[c->type];
	draw = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count], C_TYPE_BOOLEAN, &draw_stride, &draw_size)];
	if(draw_size == 1)
	{
		if(draw[0] > 1.0  || draw[0] < 0.0)
			return;
		if(count > (p->buffer_size - p->attrib_used) / 6)
			count = (p->buffer_size - p->attrib_used) / 6;
		if(count == 0)
			return;
		used = count;
	}else
	{
		if(count > (p->buffer_size - p->attrib_used) / 6)
		{
			for(i = used = 0; i < count; i++)
			{
				f = draw[(i % draw_size) * draw_stride];
				if(f <= 1.0 && f >= 0.0)
				{	
					if(used == (p->buffer_size - p->attrib_used) / 6)
						break;
					used++;
				}
			}
			count = i;
		}else
		{
			for(i = used = 0; i < count; i++)
				if(draw[(i % draw_size) * draw_stride] <= 1.0 && draw[(i % draw_size) * draw_stride] >= 0.0)
					used++;
		}
	}
	if(count > (p->buffer_size - p->attrib_used) / 6)
		count = (p->buffer_size - p->attrib_used) / 6;
	if(count == 0)
		return;

	c_execute_component_primitive_qaud_params(session, instance, c, primitive, count);
	for(i = 0; i < p->attrib_param_count; i++)
	{
		data = &stack[c_entity_stack_get(entity, c->references[i], p->attrib_params[i].type, &data_stride, &data_size)];

		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				buffer += 1;
			break;
				case C_TYPE_UV :
				case C_TYPE_UV_SECTION :
			buffer += 2;
			break;
			case C_TYPE_POS :
			{
				for(j = k = 0; k < count * data_stride; k += data_stride)
				{
					l = ((k / data_stride) % data_size) * data_stride;
					f = draw[((k / data_stride) % draw_size) * draw_stride];
					if(f <= 1.0 && f >= 0.0)
					{
						buffer[j + 0] = data[l];
						buffer[j + 1] = data[l + 1];
						buffer[j + 2] = data[l + 2];
						j += stride;
						buffer[j + 0] = data[l];
						buffer[j + 1] = data[l + 1];
						buffer[j + 2] = data[l + 2];
						j += stride;
						buffer[j + 0] = data[l];
						buffer[j + 1] = data[l + 1];
						buffer[j + 2] = data[l + 2];
						j += stride;
						buffer[j + 0] = data[l];
						buffer[j + 1] = data[l + 1];
						buffer[j + 2] = data[l + 2];
						j += stride;
						buffer[j + 0] = data[l];
						buffer[j + 1] = data[l + 1];
						buffer[j + 2] = data[l + 2];
						j += stride;
						buffer[j + 0] = data[l];
						buffer[j + 1] = data[l + 1];
						buffer[j + 2] = data[l + 2];
						j += stride;
					}
				}
			}
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				buffer += 4;
			break;
		}
	}
	p->attrib_used += used * 6;
}



void c_execute_component_primitive_square_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	CDrawState *p;
	float *buffer;
	float *size, *data, *data_b, *draw, vec[6] = {0.01, 0, 0, 0, 0.01, 0}, f, *m;
	uint i, j, k, count = 1, used, stride, data_size, size_count, data_stride, draw_size, draw_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	CComponentDef *comp_def;
	entity = instance->entity;
	stack = instance->stack;

	p = &session->prim[primitive];
	buffer = p->attrib_buffer;
	stride = p->attrib_stride;
	buffer = &buffer[stride * p->attrib_used];

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(p->attrib_params[i].type == C_TYPE_POS)
		{
			if(type->size > count)
				count = type->size;
		}
	}
	comp_def = &session->component_list[c->type];
	draw = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count], C_TYPE_BOOLEAN, &draw_stride, &draw_size)];
	if(draw_size == 1)
	{
		if(draw[0] > 1.0  || draw[0] < 0.0)
			return;
		if(count > (p->buffer_size - p->attrib_used) / 6)
			count = (p->buffer_size - p->attrib_used) / 6;
		if(count == 0)
			return;
		used = count;
	}else
	{
		if(count > (p->buffer_size - p->attrib_used) / 6)
		{
			for(i = used = 0; i < count; i++)
			{
				f = draw[(i % draw_size) * draw_stride];
				if(f <= 1.0 && f >= 0.0)
				{	
					if(used == (p->buffer_size - p->attrib_used) / 6)
						break;
					used++;
				}
			}
			count = i;
		}else
		{
			for(i = used = 0; i < count; i++)
				if(draw[(i % draw_size) * draw_stride] <= 1.0 && draw[(i % draw_size) * draw_stride] >= 0.0)
					used++;
		}
	}
	if(count > (p->buffer_size - p->attrib_used) / 6)
		count = (p->buffer_size - p->attrib_used) / 6;
	if(count == 0)
		return;

	c_execute_component_primitive_qaud_params(session, instance, c, primitive, count);
	for(i = 0; i < p->attrib_param_count; i++)
	{
		data = &stack[c_entity_stack_get(entity, c->references[i], p->attrib_params[i].type, &data_stride, &data_size)];

		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				buffer += 1;
			break;
				case C_TYPE_UV :
				case C_TYPE_UV_SECTION :
			buffer += 2;
			break;
			case C_TYPE_POS :
			{
				data = &stack[c_entity_stack_get(entity, c->references[i], C_TYPE_MATRIX, &data_stride, &data_size)];
				for(j = k = 0; k < count * data_stride; k += data_stride)
				{
					f = draw[((k / data_stride) % draw_size) * draw_stride];
					if(f <= 1.0 && f >= 0.0)
					{
						m = &data[k];
						buffer[j + 0] = m[12] - m[0] - m[4];
						buffer[j + 1] = m[13] - m[1] - m[5];
						buffer[j + 2] = m[14] - m[2] - m[6];
						j += stride;
						buffer[j + 0] = m[12] + m[0] - m[4];
						buffer[j + 1] = m[13] + m[1] - m[5];
						buffer[j + 2] = m[14] + m[2] - m[6];
						j += stride;
						buffer[j + 0] = m[12] + m[0] + m[4];
						buffer[j + 1] = m[13] + m[1] + m[5];
						buffer[j + 2] = m[14] + m[2] + m[6];
						j += stride;
						buffer[j + 0] = m[12] - m[0] - m[4];
						buffer[j + 1] = m[13] - m[1] - m[5];
						buffer[j + 2] = m[14] - m[2] - m[6];
						j += stride;
						buffer[j + 0] = m[12] + m[0] + m[4];
						buffer[j + 1] = m[13] + m[1] + m[5];
						buffer[j + 2] = m[14] + m[2] + m[6];
						j += stride;
						buffer[j + 0] = m[12] - m[0] + m[4];
						buffer[j + 1] = m[13] - m[1] + m[5];
						buffer[j + 2] = m[14] - m[2] + m[6];
						j += stride;
					}
				}
			}
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				buffer += 4;
			break;
		}
	}
	p->attrib_used += used * 6;
}



void c_execute_component_primitive_line_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	CDrawState *p;
	float *buffer;
	float *size, *data, *data_b, *draw, f, vec[6];
	uint i, j, k, a, b, count = 1, stride, data_size, data_b_size, data_b_stride, size_count, size_stride, data_stride, draw_stride, draw_size;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	p = &session->prim[primitive];
	buffer = p->attrib_buffer;
	stride = p->attrib_stride;
	buffer = &buffer[stride * p->attrib_used];

	for(i = 0; i < p->attrib_param_count + 2; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(p->attrib_params[i].type == C_TYPE_POS)
			if(type->size > count)
				count = type->size;
	}
	if(count > (p->buffer_size - p->attrib_used) / 6)
		count = (p->buffer_size - p->attrib_used) / 6;
	if(count == 0)
		return;
	/* sprite size */
	size = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count + 1], C_TYPE_FLOAT, &size_stride, &size_count)];
	draw = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count], C_TYPE_BOOLEAN, &draw_stride, &draw_size)];

	c_execute_component_primitive_qaud_params(session, instance, c, primitive, count);
	for(i = 0; i < p->attrib_param_count; i++)
	{

		data = &stack[c_entity_stack_get(entity, c->references[i], p->attrib_params[i].type, &data_stride, &data_size)];
		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				buffer += 1;
			break;
			case C_TYPE_UV :
			case C_TYPE_UV_SECTION :
			buffer += 2;
			break;
			case C_TYPE_POS :
			data_b = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count + 2], C_TYPE_POS, &data_b_stride, &data_b_size)];

			for(j = k = 0; k < count || k < data_b_size; k++)
			{
				f = draw[(k % draw_size) * draw_stride];
				if(f >= 0.0 && f <= 1.0)
				{
					a = (k % data_size) * data_stride;
					b = (k % data_b_size) * data_b_stride;
					vec[3] = data[a] - data_b[b];
					vec[4] = data[a + 1] - data_b[b + 1];
					vec[5] = data[a + 2] - data_b[b + 2];
					f_cross3f(vec, &vec[3], &c_primitives_matrix[6]);
					f_normalize3f(vec);
					f = size[(k % size_count) * size_stride];
					vec[0] *= f;
					vec[1] *= f;
					vec[2] *= f;
					buffer[j + 0] = data[a] - vec[0];
					buffer[j + 1] = data[a + 1] - vec[1];
					buffer[j + 2] = data[a + 2] - vec[2];
					j += stride;
					buffer[j + 0] = data_b[b] - vec[0];
					buffer[j + 1] = data_b[b + 1] - vec[1];
					buffer[j + 2] = data_b[b + 2] - vec[2];
					j += stride;
					buffer[j + 0] = data_b[b] + vec[0];
					buffer[j + 1] = data_b[b + 1] + vec[1];
					buffer[j + 2] = data_b[b + 2] + vec[2];
					j += stride;
					buffer[j + 0] = data[a] - vec[0];
					buffer[j + 1] = data[a + 1] - vec[1];
					buffer[j + 2] = data[a + 2] - vec[2];
					j += stride;
					buffer[j + 0] = data_b[b] + vec[0];
					buffer[j + 1] = data_b[b + 1] + vec[1];
					buffer[j + 2] = data_b[b + 2] + vec[2];
					j += stride;
					buffer[j + 0] = data[a] + vec[0];
					buffer[j + 1] = data[a + 1] + vec[1];
					buffer[j + 2] = data[a + 2] + vec[2];
					j += stride;
				}
			}
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				buffer += 4;
			break;
		}
	}
	p->attrib_used += count * 6;
}

void c_execute_component_primitive_strip_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	CDrawState *p;
	float *buffer;
	float *size, *data, *draw, f, f2, vec[6], straight[9], tmp[3];
	boolean previous_active, loop;
	uint i, j, k, a, b, bb, count = 1, used, stride, data_size, size_count, size_stride, data_stride, draw_stride, draw_size;
	CType *type;
	CEntity *entity;
	float *stack;
		entity = instance->entity;
	stack = instance->stack;

	p = &session->prim[primitive];
	buffer = p->attrib_buffer;
	stride = p->attrib_stride;
	buffer = &buffer[stride * p->attrib_used];

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(p->attrib_params[i].type == C_TYPE_POS)
		{
			if(type->size > count)
				count = type->size;
		}
	}

	draw = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count + 2], C_TYPE_BOOLEAN, &draw_stride, &draw_size)];
	loop = *draw <= 1.0 && *draw >= 0.0;
	if(!loop)
		count--;	
	draw = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count], C_TYPE_BOOLEAN, &draw_stride, &draw_size)];
	if(draw_size == 1)
	{
		if(draw[0] > 1.0  || draw[0] < 0.0)
			return;
		if(count > (p->buffer_size - p->attrib_used) / 6)
			count = (p->buffer_size - p->attrib_used) / 6;
		if(count == 0)
			return;
		used = count;
	}else
	{
		if(count > (p->buffer_size - p->attrib_used) / 6)
		{
			for(i = used = 0; i < count; i++)
			{
				f = draw[(i % draw_size) * draw_stride];
				if(f <= 1.0 && f >= 0.0)
				{	
					f = draw[((i + 1) % draw_size) * draw_stride];
					if(f <= 1.0 && f >= 0.0)
					{	
						if(used == (p->buffer_size - p->attrib_used) / 6)
							break;
						used++;
					}
				}
			}
			count = i;
		}else
		{
			for(i = used = 0; i < count; i++)
			{
				f = draw[(i % draw_size) * draw_stride];
				if(f <= 1.0 && f >= 0.0)
				{	
					f = draw[((i + 1) % draw_size) * draw_stride];
					if(f <= 1.0 && f >= 0.0)
						used++;	
				}
			}
		}
	}
	if(count > (p->buffer_size - p->attrib_used) / 6)
		count = (p->buffer_size - p->attrib_used) / 6;
	if(count == 0)
		return;
	/* sprite size */
	size = &stack[c_entity_stack_get(entity, c->references[p->attrib_param_count + 1], C_TYPE_FLOAT, &size_stride, &size_count)];

	c_execute_component_primitive_qaud_params(session, instance, c, primitive, count);
	for(i = 0; i < p->attrib_param_count; i++)
	{

		data = &stack[c_entity_stack_get(entity, c->references[i], p->attrib_params[i].type, &data_stride, &data_size)];
		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				if(data_size == 1)
				{
					for(k = j = 0; k < used * 6; k++)
					{
						buffer[j] = data[0];
						j += stride;
					}
				}else
				{
					for(j = k = 0; j < count; j++)
					{
						f = draw[(i % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							f = draw[((j + 1) % draw_size) * draw_stride];
							if(f <= 1.0 && f >= 0.0)
							{
								f = data[(j % data_size) * data_stride];
								f2 = data[((j + 1) % data_size) * data_stride];
								buffer[k] = f;
								k += stride;
								buffer[k] = f2;
								j += stride;
								buffer[k] = f2;
								j += stride;
								buffer[k] = f;
								j += stride;
								buffer[k] = f2;
								j += stride;
								buffer[k] = f;
								j += stride;
							}
						}
					}
				}
				buffer += 1;
			break;
			case C_TYPE_UV :
			case C_TYPE_UV_SECTION :
				{
					for(k = j = 0; k < count; k++)
					{
						f = draw[(k % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							f = draw[((k + 1) % draw_size) * draw_stride];
							if(f <= 1.0 && f >= 0.0)
							{
								f = (float)k / (float)count; 
								f2 = (float)(k + 1) / (float)count; 
								buffer[j] = data[0] + (data[2] - data[0]) * f;
								buffer[j + 1] = data[1];
								j += stride;
								buffer[j] = data[0] + (data[2] - data[0]) * f2;
								buffer[j + 1] = data[3];
								j += stride;
								buffer[j] = data[6] + (data[4] - data[6]) * f2;
								buffer[j + 1] = data[5];
								j += stride;
								buffer[j] = data[0] + (data[2] - data[0]) * f;
								buffer[j + 1] = data[1];
								j += stride;
								buffer[j] = data[6] + (data[4] - data[6]) * f2;
								buffer[j + 1] = data[5];
								j += stride;
								buffer[j] = data[6] + (data[4] - data[6]) * f;
								buffer[j + 1] = data[7];
								j += stride;
							}
						}
					}
				}
				buffer += 2;
			break;
			case C_TYPE_POS :

			b = (1 % data_size) * data_stride;
			tmp[0] = data[0] - data[b];
			tmp[1] = data[1] - data[b + 1];
			tmp[2] = data[2] - data[b + 2];
			f_cross3f(&straight[3], tmp, &c_primitives_matrix[6]);
			f_normalize3f(&straight[3]);
			if(loop)
			{
				a = ((data_size - 1) % data_size) * data_stride;
				tmp[0] = data[a] - data[0];
				tmp[1] = data[a + 1] - data[1];
				tmp[2] = data[a + 2] - data[2];
				f_cross3f(straight, tmp, &c_primitives_matrix[6]);
				f_normalize3f(straight);
				f = *size * 0.5;
				vec[0] = (straight[0] + straight[3]) * f;
				vec[1] = (straight[1] + straight[4]) * f;																																						
				vec[2] = (straight[2] + straight[5]) * f;
				previous_active = TRUE;
			}else
			{
				f = size[0];
				vec[0] = straight[3] * f;
				vec[1] = straight[4] * f;
				vec[2] = straight[5] * f;
				previous_active = FALSE;
			}
			if(draw_size != 1)
			{
				for(j = k = 0; k < count; k++)
				{
					f = draw[(k % draw_size) * draw_stride];
					if(f <= 1.0 && f >= 0.0)
					{
						f = draw[((k + 1) % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							a = ((k + 0) % data_size) * data_stride;
							b = ((k + 1) % data_size) * data_stride;
							if(!previous_active)
							{
								tmp[0] = data[a] - data[b];
								tmp[1] = data[a + 1] - data[b + 1];
								tmp[2] = data[a + 2] - data[b + 2];
								f_cross3f(&straight[3], tmp, &c_primitives_matrix[6]);
								f_normalize3f(&straight[3]);
								f = size[(k % size_count) * size_stride];
								vec[0] = straight[3] * f;
								vec[1] = straight[4] * f;
								vec[2] = straight[5] * f;
							}
							f = draw[((k + 2) % draw_size) * draw_stride];
							if(f <= 1.0 && f >= 0.0 && (loop || k + 1 < count))
							{
								bb = ((k + 2) % data_size) * data_stride;
								tmp[0] = data[b] - data[bb];
								tmp[1] = data[b + 1] - data[bb + 1];
								tmp[2] = data[b + 2] - data[bb + 2];
								f_cross3f(&straight[6], tmp, &c_primitives_matrix[6]);
								f_normalize3f(&straight[6]);
								f = size[((k + 1) % size_count) * size_stride] * 0.5;
								vec[3] = (straight[3] + straight[6]) * f;
								vec[4] = (straight[4] + straight[7]) * f;
								vec[5] = (straight[5] + straight[8]) * f;
							}else
							{
								f = size[(k % size_count) * size_stride];
								vec[3] = straight[3] * f;
								vec[4] = straight[4] * f;
								vec[5] = straight[5] * f;
							}
							buffer[j + 0] = data[a] - vec[0];
							buffer[j + 1] = data[a + 1] - vec[1];
							buffer[j + 2] = data[a + 2] - vec[2];
							j += stride;
							buffer[j + 0] = data[b] - vec[3];
							buffer[j + 1] = data[b + 1] - vec[4];
							buffer[j + 2] = data[b + 2] - vec[5];
							j += stride;
							buffer[j + 0] = data[b] + vec[3];
							buffer[j + 1] = data[b + 1] + vec[4];
							buffer[j + 2] = data[b + 2] + vec[5];
							j += stride;
							buffer[j + 0] = data[a] - vec[0];
							buffer[j + 1] = data[a + 1] - vec[1];
							buffer[j + 2] = data[a + 2] - vec[2];
							j += stride;
							buffer[j + 0] = data[b] + vec[3];
							buffer[j + 1] = data[b + 1] + vec[4];
							buffer[j + 2] = data[b + 2] + vec[5];
							j += stride;
							buffer[j + 0] = data[a] + vec[0];
							buffer[j + 1] = data[a + 1] + vec[1];
							buffer[j + 2] = data[a + 2] + vec[2];
							j += stride;

							vec[0] = vec[3];
							vec[1] = vec[4];
							vec[2] = vec[5];
							straight[3] = straight[6];
							straight[4] = straight[7];
							straight[5] = straight[8];
							previous_active = TRUE;
						}else
							previous_active = FALSE;
					}else
						previous_active = FALSE;
				}
			}else
			{
				for(j = k = 0; k < count; k++)
				{
					a = ((k + 0) % data_size) * data_stride;
					b = ((k + 1) % data_size) * data_stride;
					if(loop || k + 1 < count)
					{
						bb = ((k + 2) % data_size) * data_stride;
						tmp[0] = data[b] - data[bb];
						tmp[1] = data[b + 1] - data[bb + 1];
						tmp[2] = data[b + 2] - data[bb + 2];
						f_cross3f(&straight[6], tmp, &c_primitives_matrix[6]);
						f_normalize3f(&straight[6]);
						f = size[((k + 1) % size_count) * size_stride] * 0.5;
						vec[3] = (straight[3] + straight[6]) * f;
						vec[4] = (straight[4] + straight[7]) * f;
						vec[5] = (straight[5] + straight[8]) * f;
					}else
					{
						f = size[(k % size_count) * size_stride];
						vec[3] = straight[3] * f;
						vec[4] = straight[4] * f;
						vec[5] = straight[5] * f;
					}
					buffer[j + 0] = data[a] - vec[0];
					buffer[j + 1] = data[a + 1] - vec[1];
					buffer[j + 2] = data[a + 2] - vec[2];
					j += stride;
					buffer[j + 0] = data[b] - vec[3];
					buffer[j + 1] = data[b + 1] - vec[4];
					buffer[j + 2] = data[b + 2] - vec[5];
					j += stride;
					buffer[j + 0] = data[b] + vec[3];
					buffer[j + 1] = data[b + 1] + vec[4];
					buffer[j + 2] = data[b + 2] + vec[5];
					j += stride;
					buffer[j + 0] = data[a] - vec[0];
					buffer[j + 1] = data[a + 1] - vec[1];
					buffer[j + 2] = data[a + 2] - vec[2];
					j += stride;
					buffer[j + 0] = data[b] + vec[3];
					buffer[j + 1] = data[b + 1] + vec[4];
					buffer[j + 2] = data[b + 2] + vec[5];
					j += stride;
					buffer[j + 0] = data[a] + vec[0];
					buffer[j + 1] = data[a + 1] + vec[1];
					buffer[j + 2] = data[a + 2] + vec[2];
					j += stride;
					vec[0] = vec[3];
					vec[1] = vec[4];
					vec[2] = vec[5];
					straight[3] = straight[6];
					straight[4] = straight[7];
					straight[5] = straight[8];
				}
			}

			buffer += 3;
			break;
			case C_TYPE_COLOR :
				if(data_size == 1)
				{
					for(k = j = 0; k < used * 6; k++)
					{
						buffer[j] = data[0];
						buffer[j + 1] = data[1];
						buffer[j + 2] = data[2];
						buffer[j + 3] = data[3];
						j += stride;
					}
				}else
				{
					float *color, *color2;
					for(k = j = 0; k < count; k++)
					{
						f = draw[(k % draw_size) * draw_stride];
						if(f <= 1.0 && f >= 0.0)
						{
							f = draw[((k + 1) % draw_size) * draw_stride];
							if(f <= 1.0 && f >= 0.0)
							{
								color = &data[(k % data_size) * data_stride];
								color2 = &data[((k + 1) % data_size) * data_stride];
								buffer[j] = color[0];
								buffer[j + 1] = color[1];
								buffer[j + 2] = color[2];
								buffer[j + 3] = color[3];
								j += stride;
								buffer[j] = color2[0];
								buffer[j + 1] = color2[1];
								buffer[j + 2] = color2[2];
								buffer[j + 3] = color2[3];
								j += stride;
								buffer[j] = color2[0];
								buffer[j + 1] = color2[1];
								buffer[j + 2] = color2[2];
								buffer[j + 3] = color2[3];
								j += stride;
								buffer[j] = color[0];
								buffer[j + 1] = color[1];
								buffer[j + 2] = color[2];
								buffer[j + 3] = color[3];
								j += stride;
								buffer[j] = color2[0];
								buffer[j + 1] = color2[1];
								buffer[j + 2] = color2[2];
								buffer[j + 3] = color2[3];
								j += stride;
								buffer[j] = color[0];
								buffer[j + 1] = color[1];
								buffer[j + 2] = color[2];
								buffer[j + 3] = color[3];
								j += stride;
							}
						}
					}
				}
				buffer += 4;
			break;
		}
	}
	p->attrib_used += used * 6;
}

void c_execute_component_primitive_vertex_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	CDrawState *p;
	float *buffer;
	float *size, *data, *data_b, f, vec[6];
	uint i, j, k, a, b, count = 1, stride, data_size, data_b_size, data_b_stride, size_count, data_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	p = &session->prim[primitive];
	buffer = p->attrib_buffer;
	stride = p->attrib_stride;
	buffer = &buffer[stride * p->attrib_used];
	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(p->attrib_params[i].type == C_TYPE_POS)
			if(type->size > count)
				count = type->size;
	}
	if(count > (p->buffer_size - p->attrib_used))
		count = (p->buffer_size - p->attrib_used);
	if(count == 0)
		return;
	count = (count / 3) * 3;

	for(i = 0; i < p->attrib_param_count; i++)
	{
		data = &stack[c_entity_stack_get(entity, c->references[i], p->attrib_params[i].type, &data_stride, &data_size)];
		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count; j++)
						buffer[j * stride] = f;
				}else
					for(j = 0; j < count; j++)
						buffer[j * stride] = data[(j % data_size) * data_stride];
				buffer += 1;
			break;
			case C_TYPE_UV :
			case C_TYPE_UV_SECTION :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count;)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						j++;
					}
				}else
				{
					for(j = 0; j < count;)
					{
						data_b = &data[(j % data_size) * data_stride];
						buffer[j * stride] = data_b[0];
						buffer[j * stride + 1] = data_b[1];
						j++;
					}
				}
			buffer += 2;
			break;
			case C_TYPE_POS :
			for(j = k = 0; k < count; k++)
			{
				a = (k % data_size) * data_stride;
				buffer[j + 0] = data[a];
				buffer[j + 1] = data[a + 1];
				buffer[j + 2] = data[a + 2];
				j += stride;
			}
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count; j++)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						buffer[j * stride + 2] = data[2];
						buffer[j * stride + 3] = data[3];
					}
				}else
				{
					for(j = 0; j < count; j++)
					{
						buffer[j * stride] = data[(j % data_size) * data_stride + 0];
						buffer[j * stride + 1] = data[(j % data_size) * data_stride + 1];
						buffer[j * stride + 2] = data[(j % data_size) * data_stride + 2];
						buffer[j * stride + 3] = data[(j % data_size) * data_stride + 3];
					}
				}
				buffer += 4;
			break;
		}
	}
	p->attrib_used += count;
}


