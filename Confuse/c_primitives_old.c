#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"

uint c_primitive_count = 0;
CDrawPrimitive *c_primitives = NULL;
float c_primitives_matrix[9];

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

void c_execute_component_primitive_sprite_func(CEntity *entity, float *stack, CComponent *c, uint primitive)
{
	CDrawPrimitive *p;
	float *buffer;
	float *size, *data, vec[6] = {0.01, 0, 0, 0, 0.01, 0}, f;
	uint i, j, k, count = 1, stride, data_size, size_count;
	CType *type;
	p = &c_primitives[primitive];
	buffer = p->buffer;
	stride = p->buffer_stride / 4;
	buffer = &buffer[stride * p->buffer_used];

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(type->type == C_TYPE_POS)
			if(type->size > count)
				count = type->size;
	}
	if(count > (p->buffer_size - p->buffer_used) / 6)
		count = (p->buffer_size - p->buffer_used) / 6;
	if(count == 0)
		return;

	/* sprite size */

	type = &entity->stack_types[c->references[p->attrib_param_count]];
	size_count = type->size;
	size = &stack[type->pos];

	if(size_count == 1)
	{
		vec[0] = c_primitives_matrix[0] * size[0];
		vec[1] = c_primitives_matrix[1] * size[0];
		vec[2] = c_primitives_matrix[2] * size[0];
		vec[3] = c_primitives_matrix[3] * size[1];
		vec[4] = c_primitives_matrix[4] * size[1];
		vec[5] = c_primitives_matrix[5] * size[1];
	}

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		data_size = type->size;
		data = &stack[type->pos];

		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count * 6; j++)
						buffer[j * stride] = f;
				}else
					for(j = 0; j < count * 6; j++)
						buffer[j * stride] = data[(j / 6) % data_size];
				buffer += 1;
			break;
			case C_TYPE_UV :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count * 6;)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						j++;
						buffer[j * stride] = data[2];
						buffer[j * stride + 1] = data[3];
						j++;
						buffer[j * stride] = data[4];
						buffer[j * stride + 1] = data[5];
						j++;
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						j++;
						buffer[j * stride] = data[4];
						buffer[j * stride + 1] = data[5];
						j++;
						buffer[j * stride] = data[6];
						buffer[j * stride + 1] = data[7];
						j++;
					}
				}else
				{
				}
			buffer += 2;
			break;
			case C_TYPE_POS :
			if(size_count == 1)
			{
				for(j = k = 0; k < count * 3; k += 3)
				{
					buffer[j + 0] = data[k] + vec[0] + vec[3];
					buffer[j + 1] = data[k + 1] + vec[1] + vec[4];
					buffer[j + 2] = data[k + 2] + vec[2] + vec[5];
					j += stride;
					buffer[j + 0] = data[k] + vec[0] - vec[3];
					buffer[j + 1] = data[k + 1] + vec[1] - vec[4];
					buffer[j + 2] = data[k + 2] + vec[2] - vec[5];
					j += stride;
					buffer[j + 0] = data[k] - vec[0] - vec[3];
					buffer[j + 1] = data[k + 1] - vec[1] - vec[4];
					buffer[j + 2] = data[k + 2] - vec[2] - vec[5];
					j += stride;
					buffer[j + 0] = data[k] + vec[0] + vec[3];
					buffer[j + 1] = data[k + 1] + vec[1] + vec[4];
					buffer[j + 2] = data[k + 2] + vec[2] + vec[5];
					j += stride;
					buffer[j + 0] = data[k] - vec[0] - vec[3];
					buffer[j + 1] = data[k + 1] - vec[1] - vec[4];
					buffer[j + 2] = data[k + 2] - vec[2] - vec[5];
					j += stride;
					buffer[j + 0] = data[k] - vec[0] + vec[3];
					buffer[j + 1] = data[k + 1] - vec[1] + vec[4];
					buffer[j + 2] = data[k + 2] - vec[2] + vec[5];
					j += stride;
				}
			}else
			{
				for(j = k = 0; k < count * 3; k += 3)
				{
					vec[0] = size[(k / 3) * 2 + 0];
					vec[4] = size[(k / 3) * 2 + 1];

					buffer[j + 0] = data[k] + vec[0] + vec[3];
					buffer[j + 1] = data[k + 1] + vec[1] + vec[4];
					buffer[j + 2] = data[k + 2] + vec[2] + vec[5];
					j += stride;
					buffer[j + 0] = data[k] + vec[0] - vec[3];
					buffer[j + 1] = data[k + 1] + vec[1] - vec[4];
					buffer[j + 2] = data[k + 2] + vec[2] - vec[5];
					j += stride;
					buffer[j + 0] = data[k] - vec[0] - vec[3];
					buffer[j + 1] = data[k + 1] - vec[1] - vec[4];
					buffer[j + 2] = data[k + 2] - vec[2] - vec[5];
					j += stride;
					buffer[j + 0] = data[k] + vec[0] + vec[3];
					buffer[j + 1] = data[k + 1] + vec[1] + vec[4];
					buffer[j + 2] = data[k + 2] + vec[2] + vec[5];
					j += stride;
					buffer[j + 0] = data[k] - vec[0] - vec[3];
					buffer[j + 1] = data[k + 1] - vec[1] - vec[4];
					buffer[j + 2] = data[k + 2] - vec[2] - vec[5];
					j += stride;
					buffer[j + 0] = data[k] - vec[0] + vec[3];
					buffer[j + 1] = data[k + 1] - vec[1] + vec[4];
					buffer[j + 2] = data[k + 2] - vec[2] + vec[5];
					j += stride;
				}
			}
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count * 6; j++)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						buffer[j * stride + 2] = data[2];
						buffer[j * stride + 3] = data[3];
					}
				}else
				{
					for(j = 0; j < count * 6; j++)
					{
						buffer[j * stride] = data[((j / 6) % data_size) * 4 + 0];
						buffer[j * stride + 1] = data[((j / 6) % data_size) * 4 + 1];
						buffer[j * stride + 2] = data[((j / 6) % data_size) * 4 + 2];
						buffer[j * stride + 3] = data[((j / 6) % data_size) * 4 + 3];
					}
				}
				buffer += 4;
			break;
		}
	}
	p->buffer_used += count * 6;
}

void c_execute_component_primitive_line_func(CEntity *entity, float *stack, CComponent *c, uint primitive)
{
	CDrawPrimitive *p;
	float *buffer;
	float *size, *data, *data_b, vec[6] = {0.01, 0, 0, 0, 0.01, 0}, f;
	uint i, j, k, a, b, count = 1, stride, data_size, data_b_size, size_count;
	CType *type;
	p = &c_primitives[primitive];
	buffer = p->buffer;
	stride = p->buffer_stride / 4;
	buffer = &buffer[stride * p->buffer_used];

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		if(type->type == C_TYPE_POS)
			if(type->size > count)
				count = type->size;
	}
	if(count > (p->buffer_size - p->buffer_used) / 6)
		count = (p->buffer_size - p->buffer_used) / 6;
	if(count == 0)
		return;
	/* sprite size */

	type = &entity->stack_types[c->references[p->attrib_param_count]];
	size_count = type->size;
	size = &stack[type->pos];

	if(size_count == 1)
	{
		vec[0] = c_primitives_matrix[0] * size[0];
		vec[1] = c_primitives_matrix[1] * size[0];
		vec[2] = c_primitives_matrix[2] * size[0];
		vec[3] = c_primitives_matrix[3] * size[1];
		vec[4] = c_primitives_matrix[4] * size[1];
		vec[5] = c_primitives_matrix[5] * size[1];
	}

	for(i = 0; i < p->attrib_param_count; i++)
	{
		type = &entity->stack_types[c->references[i]];
		data_size = type->size;
		data = &stack[type->pos];

		switch(p->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count * 6; j++)
						buffer[j * stride] = f;
				}else
					for(j = 0; j < count * 6; j++)
						buffer[j * stride] = data[(j / 6) % data_size];
				buffer += 1;
			break;
			case C_TYPE_UV :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count * 6;)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						j++;
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1 + 2];
						j++;
						buffer[j * stride] = data[0 + 2];
						buffer[j * stride + 1] = data[1 + 2];
						j++;
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						j++;
						buffer[j * stride] = data[0 + 2];
						buffer[j * stride + 1] = data[1 + 2];
						j++;
						buffer[j * stride] = data[0 + 2];
						buffer[j * stride + 1] = data[1];
						j++;
					}
				}else
				{
				}
			buffer += 2;
			break;
			case C_TYPE_POS :
			type = &entity->stack_types[c->references[p->attrib_param_count + 1]];
			data_b_size = type->size;
			data_b = &stack[type->pos];

			for(j = k = 0; k < count || k < data_b_size; k++)
			{
				a = (k % count) * 3;
				b = (k % data_b_size) * 3;
				vec[3] = data[a] - data_b[k];
				vec[4] = data[a + 1] - data_b[k + 1];
				vec[5] = data[a + 2] - data_b[k + 2];
				f_cross3f(vec, &vec[3], &c_primitives_matrix[6]);
				f_normalize3f(vec);
				f = size[(k / 3) % size_count];
				vec[0] *= f;
				vec[1] *= f;
				vec[2] *= f;
				buffer[j + 0] = data[a] + vec[0];
				buffer[j + 1] = data[a + 1] + vec[1];
				buffer[j + 2] = data[a + 2] + vec[2];
				j += stride;
				buffer[j + 0] = data_b[b] + vec[0];
				buffer[j + 1] = data_b[b + 1] + vec[1];
				buffer[j + 2] = data_b[b + 2] + vec[2];
				j += stride;
				buffer[j + 0] = data_b[b] - vec[0];
				buffer[j + 1] = data_b[b + 1] - vec[1];
				buffer[j + 2] = data_b[b + 2] - vec[2];
				j += stride;
				buffer[j + 0] = data[a] + vec[0];
				buffer[j + 1] = data[a + 1] + vec[1];
				buffer[j + 2] = data[a + 2] + vec[2];
				j += stride;
				buffer[j + 0] = data_b[b] - vec[0];
				buffer[j + 1] = data_b[b + 1] - vec[1];
				buffer[j + 2] = data_b[b + 2] - vec[2];
				j += stride;
				buffer[j + 0] = data[a] - vec[0];
				buffer[j + 1] = data[a + 1] - vec[1];
				buffer[j + 2] = data[a + 2] - vec[2];
				j += stride;
			}
			buffer += 3;
			break;
			case C_TYPE_COLOR :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count * 6; j++)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						buffer[j * stride + 2] = data[2];
						buffer[j * stride + 3] = data[3];
					}
				}else
				{
					for(j = 0; j < count * 6; j++)
					{
						buffer[j * stride] = data[((j / 6) % data_size) * 4 + 0];
						buffer[j * stride + 1] = data[((j / 6) % data_size) * 4 + 1];
						buffer[j * stride + 2] = data[((j / 6) % data_size) * 4 + 2];
						buffer[j * stride + 3] = data[((j / 6) % data_size) * 4 + 3];
					}
				}
				buffer += 4;
			break;
		}
	}
	p->buffer_used += count * 6;
}



uint confuse_primitive_count()
{
	return c_primitive_count;
}

CDrawPrimitiveType confuse_primitive_type(uint id)
{
	return c_primitives[id].type;
}

void confuse_primitive_blend_func(uint id, uint *source, uint *dist)
{
	if(dist != NULL)
		*dist = c_primitives[id].blend_destination;
	if(source != NULL)
		*source = c_primitives[id].blend_source;
}
boolean confuse_primitive_depth_test(uint id)
{
	return c_primitives[id].depth_test;
}

char *confuse_primitive_shader_source(uint id, uint unit)
{
	return c_primitives[id].shaders[unit];
}

char *confuse_primitive_name(uint id)
{
	return c_primitives[id].name;
}

uint confuse_primitive_buffer_stride(CDrawPrimitive *p)
{
	uint i, stride = 0;
	for(i = 0; i < p->attrib_param_count; i++)
	{
		switch(p->attrib_params[i].type)
		{	
			case C_TYPE_FLOAT :
				stride += sizeof(float);
			case C_TYPE_UV :
				stride += 2 * sizeof(float);
			break;
			case C_TYPE_POS :
				stride += 3 * sizeof(float);
			break;
			case C_TYPE_COLOR :
				stride += 4 * sizeof(float);
			break;
			case C_TYPE_MATRIX :
				stride += 16 * sizeof(float);
			break;
			case C_TYPE_POS_QUATERNION_SIZE :
				stride += 16 * sizeof(float);
			break;
		}
	}
	return stride;
}


char *c_primitive_text_load(char *file)
{
	char *buffer;
	uint size;
	FILE *f;
	f = fopen(file, "r");
	if(f == NULL)
	{
		printf("filename %s\n", file);
		exit(0);
		return NULL;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	buffer = malloc(size + 1);
	size = fread(buffer, 1, size, f);
	fclose(f);
	buffer[size] = 0;
	return buffer;
}



char *c_shader_type_names[] = {"float", /* C_TYPE_FLOAT */
									"vec2", /* C_TYPE_UV - C_TYPE_UV_SECTION*/
									"vec3", /* C_TYPE_POS */
									"vec4", /* C_TYPE_COLOR */
									"mat4", /* C_TYPE_MATRIX */
									"no_type_like_this", /* C_TYPE_AREA */
									"sampler2D", /* C_TYPE_IMAGE_2D */
									"sampler3D", /* C_TYPE_IMAGE_3D */
									"samplerCube"/* C_TYPE_IMAGE_CUBE */};
	



void c_shader_parse(CDrawPrimitiveInput *input, uint *count, char *source, char *key, uint start, uint end)
{
	uint i, j, k;
	for(i = 0; source[i] != 0; i++)
	{
		for(j = 0; source[i + j] != 0 && source[i + j] == key[j]; j++);
		if(key[j] == 0)
		{
			i += j;
			while(source[++i] <= 32);

			for(j = start; j < end; j++)
			{
				
				for(k = 0; source[i + k] != 0 && source[i + k] == c_shader_type_names[j][k]; k++);
				if(c_shader_type_names[j][k] == 0)
				{
					i += k;
					for(++i; source[i] <= 32 && source[i] != 0 && source[i] != ';'; i++);
					input[*count].type = j; 
					for(j = 0; source[i + j] > 32 && source[i + j] != 0 && source[i + j] != ';' && j < 32; j++)
						input[*count].name[j] = source[i + j];
					input[*count].name[j] = 0;
					(*count)++;
					break;
				}
			}
		}
	}
}


CDrawPrimitive *c_primitive_add()
{
	CDrawPrimitive *p;
	uint i, j, k;

	if(c_primitive_count % 8 == 0)
		c_primitives = realloc(c_primitives, (sizeof *c_primitives) * (c_primitive_count + 8));
	c_primitive_count++; 
	p = &c_primitives[c_primitive_count - 1];

	p->type = C_DPT_DRAW_CALL_ATTRIBUTE;
	p->blend_destination = 1;
	p->blend_source = 1;
	p->depth_test = FALSE;

	p->shader_paths[0] = NULL;
	p->shader_paths[1] = NULL;
	p->shaders[0] = NULL;
	p->shaders[1] = NULL;
	
	p->attrib_params = malloc((sizeof* p->attrib_params) * 32);
	p->attrib_param_count = 0;

	p->uniform_params = malloc((sizeof* p->uniform_params) * 32);
	p->uniform_param_count = 0;

	p->buffer_stride = 0;
	p->buffer_size = 1000;
	p->buffer_used = 0;
	p->buffer = NULL;
	p->injected = FALSE;
	sprintf(p->name, "shade");
	return p;
}

boolean c_primitive_compile(CDrawPrimitive *p)
{
	char *prefixes[] = {"Sprite", "Line", "Vertex"};
	char *ignore_types[] = {"NormalMatrix", "ModelViewMatrix", "ProjectionMatrix", "ModelViewProjectionMatrix"};
	uint i, j, k;
	char **names, buf[128];
	CTypeType *types;

	if(p->shader_paths[0] == 0 || p->shader_paths[1] == 0)
		return;
	
	p->shaders[0] = c_primitive_text_load(p->shader_paths[0]);
	p->shaders[1] = c_primitive_text_load(p->shader_paths[1]);

	if(p->shaders[0] == 0 || p->shaders[1] == 0)
		return;
	
	p->attrib_param_count = 0;
	c_shader_parse(p->attrib_params, &p->attrib_param_count, p->shaders[0], "attribute", 0, 8);

	p->uniform_param_count = 0;
	c_shader_parse(p->uniform_params, &p->uniform_param_count, p->shaders[0], "uniform", 0, 8);
	c_shader_parse(p->uniform_params, &p->uniform_param_count, p->shaders[1], "uniform", 0, 8);
	for(i = 0; i < p->uniform_param_count; i++)
	{
		for(j = 0; j < 4; j++)
		{
			for(k = 0; p->uniform_params[i].name[k] != 0 && p->uniform_params[i].name[k] == ignore_types[j][k]; k++);
			if(p->uniform_params[i].name[k] == ignore_types[j][k])
			{
				p->uniform_param_count--;
				for(j = i; j < p->uniform_param_count; j++)
					p->uniform_params[j] = p->uniform_params[j + 1];
				break;
			}
		}
	}
	printf("p->uniform_params %u %u\n", p->uniform_param_count, p->attrib_param_count);
	for(i = 0; i < p->uniform_param_count; i++)
		printf("Uniform[%u]= %s\n", i, p->uniform_params[i].name);
	for(i = 0; i < p->attrib_param_count; i++)
		printf("Attrib[%u]= %s\n", i, p->attrib_params[i].name);
	if(p->buffer != NULL)
		free(p->buffer);
	p->buffer_stride = confuse_primitive_buffer_stride(p);
	p->buffer = malloc(p->buffer_stride * p->buffer_size);
	p->buffer_used = 0;
}

boolean c_primitive_inject(CDrawPrimitive *p)
{
	uint i, j, k;
	char **names, buf[128];
	CTypeType *types;

	names = malloc((sizeof *names) * (p->attrib_param_count + 1));
	for(i = 0; i < p->attrib_param_count; i++)
		names[i] = p->attrib_params[i].name;
	names[i] = "Size";
	types = malloc((sizeof *types) * (p->attrib_param_count + 1));
	for(i = 0; i < p->attrib_param_count; i++)
	{
		types[i] = p->attrib_params[i].type;
		if(types[i] == C_TYPE_UV)
			types[i] = C_TYPE_UV_SECTION;
	}
	types[i] = C_TYPE_AREA;
	sprintf(buf, "Sprite %s", p->name);
	confuse_component_add(buf, types, names, p->attrib_param_count + 1, -1, c_primitive_count - 1, c_execute_component_primitive_sprite_func);

	names = malloc((sizeof *names) * (p->attrib_param_count + 2));
	for(i = 0; i < p->attrib_param_count; i++)
		names[i] = p->attrib_params[i].name;
	names[i++] = "Size";
	names[i] = "goal";
	types = malloc((sizeof *types) * (p->attrib_param_count + 2));
	for(i = 0; i < p->attrib_param_count; i++)
	{
		types[i] = p->attrib_params[i].type;
		if(types[i] == C_TYPE_UV)
			types[i] = C_TYPE_UV_SECTION;
	}
	types[i++] = C_TYPE_FLOAT;
	types[i] = C_TYPE_POS;
	sprintf(buf, "Line %s", p->name);
	confuse_component_add(buf, types, names, p->attrib_param_count + 2, -1, c_primitive_count - 1, c_execute_component_primitive_line_func);

}

uint confuse_primitive_param_count(uint id)
{
	return c_primitives[id].attrib_param_count;
}

CTypeType confuse_primitive_param_type(uint id, uint param)
{
	return c_primitives[id].attrib_params[param].type;
}

char *confuse_primitive_param_name(uint id, uint param)
{
	return c_primitives[id].attrib_params[param].name;
}

uint confuse_primitive_buffer_allocated(uint id)
{
	return c_primitives[id].buffer_size;
}

uint confuse_primitive_buffer_count(uint id)
{
	return c_primitives[id].buffer_used;
}

void confuse_primitive_buffer_clear(uint id)
{
	c_primitives[id].buffer_used = 0;
}

void *confuse_primitive_buffer_get(uint id)
{
	return c_primitives[id].buffer;
}

char *confuse_primitive_shader_get(uint id, uint source)
{
	return c_primitives[id].shaders[source];
}

