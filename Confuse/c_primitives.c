#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"

void *(*c_load_shader_func)(char *name, char *vertex, char *fragment, char *debug_buffer, uint *buffer_size, void *user) = NULL;
void *c_load_shader_user;
uint (*c_load_shader_localtion_func)(void *shader, char *name, void *user) = NULL;
void *c_load_shader_localtion_user;

extern void c_execute_component_primitive_sprite_func(CSession *session, CInstance *instance, CComponent *c, uint primitive);
extern void c_execute_component_primitive_line_func(CSession *session, CInstance *instance, CComponent *c, uint primitive);
extern void c_execute_component_primitive_vertex_func(CSession *session, CInstance *instance, CComponent *c, uint primitive);
extern void c_execute_component_primitive_square_func(CSession *session, CInstance *instance, CComponent *c, uint primitive);
extern void c_execute_component_primitive_strip_func(CSession *session, CInstance *instance, CComponent *c, uint primitive);
extern void c_execute_component_primitive_collapsed_func(CSession *session, CInstance *instance, CComponent *c, uint primitive);

void confuce_asset_load_shader_func(void *(*func)(char *name, char *vertex, char *fragment, char *debug_buffer, uint buffer_size, void *user), void *user)
{
	c_load_shader_func = func;
	c_load_shader_user = user;
}

void confuce_asset_load_shader_localtion_func(uint (*func)(void *shader, char *name, void *user), void *user)
{
	c_load_shader_localtion_func = func;
	c_load_shader_localtion_user = user;
}

void *(*c_load_array_func)(char *name, uint8 *data, uint *format_types, uint format_count, uint vertex_count, void *user) = NULL;
void *c_load_array_user = NULL;
void (*c_load_array_free_func)(char *array, void *user) = NULL;
void *c_load_array_free_user = NULL;

void confuce_asset_load_array_func(void *(*func)(char *name, uint8 *data, uint *format_types, uint format_count, uint vertex_count, void *user), void *user)
{
	c_load_array_func = func;
	c_load_array_user = user;
}

void confuce_asset_load_array_free_func(void (*func)(char *array, void *user), void *user)
{
	c_load_array_free_func = func;
	c_load_array_free_user = user;
}


void c_update_primitive_array(CDrawState *state, uint8 *data, uint vertex_count)
{
	uint i, j, k, *format_types;
	format_types = malloc((sizeof *format_types) * state->attrib_param_count);
	for(i = 0; i < state->attrib_param_count; i++)
		format_types[i] = state->attrib_params[i].type;

	if(c_load_array_free_func != NULL && state->state_array != NULL)
		c_load_array_free_func(state->state_array, c_load_array_free_user);
	state->state_array = c_load_array_func(state->name, data, format_types, state->attrib_param_count, vertex_count, c_load_array_user);
	if(state->attrib_buffer != NULL)
		free(state->attrib_buffer);
	state->attrib_buffer = data;
	state->attrib_used = vertex_count;
	printf("state->state_array %p ------------------------------------------------------------------\n", state->state_array);
	free(format_types);
}

void c_execute_component_primitive_uniform_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	CDrawState *p;
	float *buffer;
	float *size, *data, *data_b, vec[6] = {0.01, 0, 0, 0, 0.01, 0}, f;
	uint i, j, k, a, b, count = 1, stride, data_size, data_stride, data_b_size, data_b_stride, size_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	p = &session->prim[primitive];
	buffer = p->uniform_buffer;
	stride = p->uniform_stride;
	buffer = &buffer[stride * p->uniform_used];

	type = &entity->stack_types[c->references[C_BUL_TRANSFORMATION_MATRIX]];
	if(type->size > count)
			count = type->size;

	if(count > p->buffer_size - p->uniform_used)
		count = p->buffer_size - p->uniform_used;
	if(count == 0)
		return;
	for(i = 0; i < p->uniform_param_count; i++)
	{

		data = &stack[c_entity_stack_get(entity, c->references[i], p->uniform_params[i].type, &data_stride, &data_size)];
		switch(p->uniform_params[i].type)
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
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count; j++)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
					}
				}else
				{
					for(j = 0; j < count; j++)
					{
						buffer[j * stride] = data[(j % data_size) * data_stride];
						buffer[j * stride + 1] = data[(j % data_size) * data_stride + 1];
					}
				}
				buffer += 2;
			break;
			case C_TYPE_POS :
			case C_TYPE_OBJECT :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count; j++)
					{
						buffer[j * stride] = data[0];
						buffer[j * stride + 1] = data[1];
						buffer[j * stride + 2] = data[2];
					}
				}else
				{
					for(j = 0; j < count; j++)
					{
						buffer[j * stride] = data[(j % data_size) * data_stride];
						buffer[j * stride + 1] = data[(j % data_size) * data_stride + 1];
						buffer[j * stride + 2] = data[(j % data_size) * data_stride + 2];
					}
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
						buffer[j * stride] = data[(j % data_size) * data_stride];
						buffer[j * stride + 1] = data[(j % data_size) * data_stride + 1];
						buffer[j * stride + 2] = data[(j % data_size) * data_stride + 2];
						buffer[j * stride + 3] = data[(j % data_size) * data_stride + 3];
					}
				}
				buffer += 4;
			break;
			case C_TYPE_MATRIX :
				if(data_size == 1)
				{
					f = *data;
					for(j = 0; j < count; j++)
						for(k = 0; k < 16; k++)
							buffer[j * stride + k] = data[k];

				}else
					for(j = 0; j < count; j++)
						for(k = 0; k < 16; k++)
							buffer[j * stride + k] = data[(j % data_size) * data_stride + k];
				buffer += 16;
			break;
			case C_TYPE_POS_QUATERNION_SIZE :
			break;
			case C_TYPE_IMAGE_2D :
			case C_TYPE_IMAGE_3D :
			case C_TYPE_IMAGE_CUBE :
			case C_TYPE_STATE_DEPTH_TEST :
			case C_TYPE_STATE_BLEND_SOURCE :
			case C_TYPE_STATE_BLEND_DESTINATION :
			{
				uint *idata, *ibuffer;
				idata = (uint *)data;
				ibuffer = (uint *)buffer;
				if(data_size == 1)
				{
					k = *idata;
					for(j = 0; j < count; j++)
						ibuffer[j * stride] = k;
				}else
					for(j = 0; j < count; j++)
						ibuffer[j * stride] = idata[(j % data_size) * data_stride];
				buffer += 1;
			}
			break;
			case C_TYPE_BOOLEAN :
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
		}
	}
	p->uniform_used += count;
}

/*

uint confuse_primitive_count()
{
	return c_primitive_count;
}

CDrawStateType confuse_primitive_type(uint id)
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
*/

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

#define C_SHADER_TYPE_NAME_COUNT 12

char *c_shader_type_names[C_SHADER_TYPE_NAME_COUNT] = {"float", /* C_TYPE_FLOAT */
													"vec2", /* C_TYPE_UV - C_TYPE_UV_SECTION*/
													"vec3", /* C_TYPE_POS */
													"vec4", /* C_TYPE_COLOR */
													"mat4", /* C_TYPE_MATRIX */
													"no_type_like_this", /* C_TYPE_AREA */
													"no_type_like_this", /* C_TYPE_POS_QUATERNION_SIZE */
													"no_type_like_this", /* C_TYPE_UV_SECTION */
													"no_type_like_this", /* C_TYPE_SAME_FLOAT_AS_OUTPUT */
													"sampler2D", /* C_TYPE_IMAGE_2D */
													"sampler3D", /* C_TYPE_IMAGE_3D */
													"samplerCube"/* C_TYPE_IMAGE_CUBE */};


void c_shader_parse(CDrawStateInput *input, uint *count, char *source, char *key, uint start, uint end)
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

CDrawState *c_primitive_add(CSession *session, char *name)
{
	CDrawState *p;
	uint i, j, k;

	if(session->primitive_count % 8 == 0)
		session->prim = realloc(session->prim, (sizeof *session->prim) * (session->primitive_count + 8));
	p = &session->prim[session->primitive_count++];

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

	p->attrib_stride = 0;
	p->uniform_stride = 0;
	p->buffer_size = 100000;
	p->sort_distance = NULL;
	p->sort_buffer = NULL;
	p->attrib_used = 0;
	p->uniform_used = 0;
	p->attrib_buffer = NULL;
	p->uniform_buffer = NULL;
	p->injected = FALSE;
	p->geometry_checksum = 0;
	sprintf(p->name, name);
	return p;
}
boolean c_primitive_compile_test(char *vertex, char *fragment, char *name, char *debug, uint debug_buffer)
{
	/* FIX ME */
	if(c_load_shader_func != NULL)
	{
		return NULL != c_load_shader_func(name, vertex, fragment, debug, debug_buffer, c_load_shader_user);
	}
	return TRUE;
}

boolean c_primitive_compile(CDrawState *p)
{
	char *prefixes[] = {"Sprite", "Line", "Vertex"};
	char *ignore_types[] = {"NormalMatrix", "ModelViewMatrix", "ProjectionMatrix", "ModelViewProjectionMatrix"};
	uint i, j, k;
	char **names, buf[128];
	CTypeType *types;

/*	if(p->shader_paths[0] == 0 || p->shader_paths[1] == 0)
		return FALSE;*/
	if(p->shaders[0] == NULL)
		p->shaders[0] = c_primitive_text_load(p->shader_paths[0]);
	if(p->shaders[1] == NULL)
		p->shaders[1] = c_primitive_text_load(p->shader_paths[1]);

	if(p->shaders[0] == 0 || p->shaders[1] == 0)
		return FALSE;
	if(c_load_shader_func != NULL)
		p->state_shader = c_load_shader_func(p->name, p->shaders[0], p->shaders[1], NULL, 0, c_load_shader_user);
	if(p->state_shader == NULL)
		return FALSE;
	p->attrib_param_count = 0;
	c_shader_parse(p->attrib_params, &p->attrib_param_count, p->shaders[0], "attribute", 0, C_SHADER_TYPE_NAME_COUNT);
	if(p->attrib_param_count == 0)
		p->attrib_param_count = 0;	
	p->uniform_params[C_BUL_DRAW].type = C_TYPE_BOOLEAN;
	f_text_copy(32, p->uniform_params[C_BUL_DRAW].name, "Draw");

	p->uniform_params[C_BUL_DEPTH_TEST].type = C_TYPE_STATE_DEPTH_TEST;
	f_text_copy(32, p->uniform_params[C_BUL_DEPTH_TEST].name, "Depth test");

	p->uniform_params[C_BUL_BLEND_SOURCE].type = C_TYPE_STATE_BLEND_SOURCE;
	f_text_copy(32, p->uniform_params[C_BUL_BLEND_SOURCE].name, "Source");

	p->uniform_params[C_BUL_BLEND_DEST].type = C_TYPE_STATE_BLEND_DESTINATION;
	f_text_copy(32, p->uniform_params[C_BUL_BLEND_DEST].name, "Destination");

	p->uniform_params[C_BUL_OBJECT].type = C_TYPE_OBJECT;
	f_text_copy(32, p->uniform_params[C_BUL_OBJECT].name, "Object");

	p->uniform_params[C_BUL_TRANSFORMATION_MATRIX].type = C_TYPE_MATRIX;
	f_text_copy(32, p->uniform_params[C_BUL_TRANSFORMATION_MATRIX].name, "Position");

	p->uniform_param_count = C_BUL_COUNT;
	if(p->attrib_param_count == 0)
		p->attrib_param_count = 0;	
	c_shader_parse(p->uniform_params, &p->uniform_param_count, p->shaders[0], "uniform", 0, C_SHADER_TYPE_NAME_COUNT);
	c_shader_parse(p->uniform_params, &p->uniform_param_count, p->shaders[1], "uniform", 0, C_SHADER_TYPE_NAME_COUNT);
	for(i = C_BUL_COUNT; i < p->uniform_param_count; i++)
	{
		for(j = 0; j < 4; j++)
		{
			for(k = 0; p->uniform_params[i].name[k] != 0 && p->uniform_params[i].name[k] == ignore_types[j][k]; k++);
			if(p->uniform_params[i].name[k] == ignore_types[j][k])
			{
				p->uniform_param_count--;
				for(j = i; j < p->uniform_param_count; j++)
					p->uniform_params[j] = p->uniform_params[j + 1];
			//	i--;  FIX ME
				break;
			}
		}
	}
	
	p->uniform_params[0].offset = 0;
	for(i = 1; i < p->uniform_param_count; i++)
		p->uniform_params[i].offset = p->uniform_params[i - 1].offset + c_type_size_get(p->uniform_params[i - 1].type);
	p->attrib_params[0].offset = 0;
	for(i = 1; i < p->attrib_param_count; i++)
		p->attrib_params[i].offset = p->attrib_params[i - 1].offset + c_type_size_get(p->attrib_params[i - 1].type);


	printf("p->uniform_params %u %u\n", p->uniform_param_count, p->attrib_param_count);
	for(i = 0; i < p->uniform_param_count; i++)
		printf("Uniform[%u]= %s (offset = %u)\n", i, p->uniform_params[i].name, p->uniform_params[i].offset);
	for(i = 0; i < p->attrib_param_count; i++)
		printf("Attrib[%u]= %s (offset = %u)\n", i, p->attrib_params[i].name, p->attrib_params[i].offset);

	p->attrib_stride = 0;
	for(i = 0; i < p->attrib_param_count; i++)
		p->attrib_stride += c_type_size_get(p->attrib_params[i].type);
	p->uniform_stride = 0;
	for(i = 0; i < p->uniform_param_count; i++)
		p->uniform_stride += c_type_size_get(p->uniform_params[i].type);

	p->attrib_buffer = NULL;
	p->uniform_buffer = NULL;
	p->uniform_used = 0;
	p->attrib_used = 0;
	if(c_load_shader_func != NULL)
	{
		p->state_shader = c_load_shader_func(p->name, p->shaders[0], p->shaders[1], c_load_shader_user, NULL, 0);
		for(i = C_BUL_COUNT; i < p->uniform_param_count; i++)
			p->uniform_params[i].id = c_load_shader_localtion_func(p->state_shader, p->uniform_params[i].name, c_load_shader_localtion_user);
	}
	p->state_array = NULL;

	if(p->attrib_param_count == 0)
		p->attrib_param_count = 0;	
	return TRUE;
}

typedef enum{
	C_PTC_UV_TO_SECTION,
	C_PTC_POS_TO_MATRIX,
	C_PTC_NONE
}CPrimitiveTypeConvert;

void c_primitive_inject_attribute_type(CSession *session, CDrawState *p, char **extension_names, CTypeType *extension_types, uint extension_count, char *name, CPrimitiveTypeConvert convert, void (*func)(CSession *session, CInstance *instance, CComponent *c, uint primitive))
{
	uint i, j, k, *format_types;
	char **names, buf[128];
	CTypeType *types;
	names = malloc((sizeof *names) * (p->attrib_param_count + extension_count));
	for(i = 0; i < p->attrib_param_count; i++)
		names[i] = p->attrib_params[i].name;
	types = malloc((sizeof *types) * (p->attrib_param_count + extension_count));
	for(i = 0; i < p->attrib_param_count; i++)
	{
		types[i] = p->attrib_params[i].type;
		if((convert == C_PTC_UV_TO_SECTION || convert == C_PTC_POS_TO_MATRIX) && types[i] == C_TYPE_UV)
			types[i] = C_TYPE_UV_SECTION;
		else if(types[i] == C_TYPE_UV_SECTION)
			types[i] = C_TYPE_UV;
		if(convert == C_PTC_POS_TO_MATRIX && types[i] == C_TYPE_POS)
			types[i] = C_TYPE_MATRIX;
	}
	for(i = 0; i < extension_count; i++)
	{
		names[i + p->attrib_param_count] = extension_names[i];
		types[i + p->attrib_param_count] = extension_types[i];
	}
	sprintf(buf, "%s %s", name, p->name);
	confuse_component_add(session, buf, types, names, p->attrib_param_count + extension_count, -1, session->primitive_count - 1, func, C_CC_DRAW, -1, NULL);
}


void c_primitive_inject_attribute(CSession *session, CDrawState *p)
{
	uint i, j, k, *format_types;
	char **names, buf[128];
	CTypeType *types;
	char *sprite_names[] = {"Draw", "Size"};
	CTypeType sprite_types[] = {C_TYPE_BOOLEAN, C_TYPE_AREA};
	char *line_names[] = {"Draw", "Size", "Goal"};
	CTypeType line_types[] = {C_TYPE_BOOLEAN, C_TYPE_FLOAT, C_TYPE_POS};
	char *boolean_names[] = {"Draw"};
	CTypeType boolean_types[] = {C_TYPE_BOOLEAN};
	char *strip_names[] = {"Draw", "Size", "Closed"};
	CTypeType strip_types[] = {C_TYPE_BOOLEAN, C_TYPE_FLOAT, C_TYPE_BOOLEAN};

	p->type = C_DPT_DRAW_CALL_ATTRIBUTE;
	p->attrib_buffer = malloc(sizeof(float) * p->attrib_stride * p->buffer_size);
	p->attrib_used = 0;
	format_types = malloc((sizeof *format_types) * p->attrib_param_count);
	for(i = 0; i < p->attrib_param_count; i++)
		format_types[i] = p->attrib_params[i].type;
	if(c_load_array_func != NULL)
		p->state_array = c_load_array_func(p->name, NULL, format_types, p->attrib_param_count, p->buffer_size, c_load_array_user);
	free(format_types);
	
	
	c_primitive_inject_attribute_type(session, p, sprite_names, sprite_types, 2, "Sprite", C_PTC_UV_TO_SECTION, c_execute_component_primitive_sprite_func);
	c_primitive_inject_attribute_type(session, p, line_names, line_types, 3, "Line", C_PTC_UV_TO_SECTION, c_execute_component_primitive_line_func);
	c_primitive_inject_attribute_type(session, p, NULL, NULL, 0, "Vertex", C_PTC_NONE, c_execute_component_primitive_vertex_func);
	c_primitive_inject_attribute_type(session, p, boolean_names, boolean_types, 1, "Square", C_PTC_POS_TO_MATRIX, c_execute_component_primitive_square_func);
	c_primitive_inject_attribute_type(session, p, strip_names, strip_types, 3, "Strip", C_PTC_UV_TO_SECTION, c_execute_component_primitive_strip_func);
	c_primitive_inject_attribute_type(session, p, boolean_names, boolean_types, 1, "Collapsed", C_PTC_UV_TO_SECTION, c_execute_component_primitive_collapsed_func);
}
/*

void c_primitive_inject_attribute(CSession *session, CDrawState *p)
{
	uint i, j, k, *format_types;
	char **names, buf[128];
	CTypeType *types;
	p->type = C_DPT_DRAW_CALL_ATTRIBUTE;

	p->attrib_buffer = malloc(sizeof(float) * p->attrib_stride * p->buffer_size);
	p->attrib_used = 0;


	format_types = malloc((sizeof *format_types) * p->attrib_param_count);
	for(i = 0; i < p->attrib_param_count; i++)
		format_types[i] = p->attrib_params[i].type;
	if(c_load_array_func != NULL)
		p->state_array = c_load_array_func(p->name, NULL, format_types, p->attrib_param_count, p->buffer_size, c_load_array_user);
	free(format_types);

	names = malloc((sizeof *names) * (p->attrib_param_count + 2));
	for(i = 0; i < p->attrib_param_count; i++)
		names[i] = p->attrib_params[i].name;
	names[i++] = "Size";
	names[i] = "Draw";
	types = malloc((sizeof *types) * (p->attrib_param_count + 2));
	for(i = 0; i < p->attrib_param_count; i++)
	{
		types[i] = p->attrib_params[i].type;
		if(types[i] == C_TYPE_UV)
			types[i] = C_TYPE_UV_SECTION;
	}
	types[i++] = C_TYPE_AREA;
	types[i] = C_TYPE_BOOLEAN;
	sprintf(buf, "Sprite %s", p->name);
	confuse_component_add(session, buf, types, names, p->attrib_param_count + 2, -1, session->primitive_count - 1, c_execute_component_primitive_sprite_func, C_CC_DRAW, -1, NULL);

	names = malloc((sizeof *names) * (p->attrib_param_count + 3));
	for(i = 0; i < p->attrib_param_count; i++)
		names[i] = p->attrib_params[i].name;
	names[i++] = "Size";
	names[i++] = "Goal";
	names[i] = "Draw";
	types = malloc((sizeof *types) * (p->attrib_param_count + 3));
	for(i = 0; i < p->attrib_param_count; i++)
	{
		types[i] = p->attrib_params[i].type;
		if(types[i] == C_TYPE_UV)
			types[i] = C_TYPE_UV_SECTION;
	}
	types[i++] = C_TYPE_FLOAT;
	types[i++] = C_TYPE_POS;
	types[i] = C_TYPE_BOOLEAN;
	sprintf(buf, "Line %s", p->name);
	confuse_component_add(session, buf, types, names, p->attrib_param_count + 3, -1, session->primitive_count - 1, c_execute_component_primitive_line_func, C_CC_DRAW, -1, NULL);
}*/

/*
void c_primitive_unifor_geometry_set(CSession *session, CDrawState *p, float *vertices, uint vertex_count)
{
	uint i, j, k, *format_types;
	format_types = malloc((sizeof *format_types) * p->attrib_param_count);
	j = 0;
	for(i = 0; i < p->attrib_param_count; i++)
	{
		format_types[i] = p->attrib_params[i].type;
		j += c_type_size_get(p->attrib_params[i].type);
	}
	p->uniform_buffer = vertices;
	p->state_array = c_load_array_func("ARRAY NAME", vertices, format_types, p->attrib_param_count, vertex_count, c_load_array_user);
	p->attrib_used = vertex_count;
	free(format_types);
}
*/
uint c_primitive_inject_uniform(CSession *session, CDrawState *p, CComponentCategory type, boolean add_component)
{
	uint i, j, k, *format_types;
	char **names, buf[128];	
	CTypeType *types;

	if(type == C_CC_DRAW)
	{
		p->type = C_DPT_DRAW_CALL_UNIFORM;
		p->uniform_buffer = malloc(sizeof(float) * p->uniform_stride * p->buffer_size);
		p->uniform_used = 0;
	}else
	{
		p->type = C_DPT_DRAW_CALL_ATTRIBUTE;
		p->uniform_buffer = malloc(sizeof(float) * p->uniform_stride);
		p->uniform_used = 0;
	}

	names = malloc((sizeof *names) * (p->uniform_param_count + C_BUL_COUNT));
	for(i = 0; i < p->uniform_param_count; i++)
		names[i] = p->uniform_params[i].name;
	types = malloc((sizeof *types) * (p->uniform_param_count + C_BUL_COUNT));
	for(i = 0; i < p->uniform_param_count; i++)
		types[i] = p->uniform_params[i].type;

	j = confuse_component_add(session, p->name, types, names, p->uniform_param_count, -1, session->primitive_count - 1, c_execute_component_primitive_uniform_func, type, -1, NULL);
	if(C_CC_PRIMITIVE_STATE == type && add_component)
		c_entity_components_add(session, &session->primitive_settings, j, -1);
	return j;
}

void c_primitive_load(CSession *session, char *vertex_shader, char *fragment_shader, boolean attribute, boolean uniform, char *name)
{
	char text[128];
	CDrawState *p;
	if(attribute)
	{
		if(uniform)
		{
			sprintf(text, "%s attribute", name);
			p = c_primitive_add(session, text);
		}else
			p = c_primitive_add(session, name);
		p->shader_paths[0] = f_text_copy_allocate(vertex_shader);
		p->shader_paths[1] = f_text_copy_allocate(fragment_shader);
		c_primitive_compile(p);
		c_primitive_inject_uniform(session, p, C_CC_DRAW, FALSE);
	}
	if(uniform)
	{
		if(attribute)
		{
			sprintf(text, "%s uniform", name);
			p = c_primitive_add(session, text);
		}else
			p = c_primitive_add(session, name);
		p->shader_paths[0] = f_text_copy_allocate(vertex_shader);
		p->shader_paths[1] = f_text_copy_allocate(fragment_shader);
		c_primitive_compile(p);
		c_primitive_inject_uniform(session, p, C_CC_PRIMITIVE_STATE, TRUE);
		c_primitive_inject_attribute(session, p);
	}
}

void c_primitive_entity_transition(CSession *session, CEntity *entity, uint prev_component, uint new_component, uint *translate)
{
	CTypeType t;
	uint i, j, *new_array;
	for(i = 0; i < entity->component_count; i++)
	{
		if(entity->components[i].type == prev_component)
		{
			new_array = malloc((sizeof *new_array) * (session->component_list[new_component].param_count + 1));
			for(j = 0; j < session->component_list[new_component].param_count; j++)
			{
				if(translate[j] != -1)
					new_array[j] = entity->components[i].references[translate[j]];
				else
				{
					char buf[1024];
					t = session->component_list[new_component].param_types[j];
					if(t == C_TYPE_SAME_FLOAT_AS_OUTPUT)
						t = entity->stack_types[entity->components[i].references[session->component_list[prev_component].param_count]].type;
					sprintf(buf, "%s %s", confuse_component_name_get(session, new_component), confuse_component_param_name_get(session, new_component, j));
					new_array[j] = c_entity_stack_add(entity, t, buf, 1);
				}
			}
			new_array[j] = entity->components[i].references[session->component_list[prev_component].param_count];

			entity->components[i].type = new_component;
			free(entity->components[i].references);
			entity->components[i].references = new_array;
		}
	}
}

void c_primitive_component_transition(CSession *session, uint prev_component, uint new_component)
{
	uint i, j, k, *translate;
	CComponentDef *ca, *cb;
	ca = &session->component_list[prev_component];
	cb = &session->component_list[new_component];

	translate = malloc((sizeof *translate) * cb->param_count);
	for(i = 0; i < cb->param_count; i++)
	{
		translate[i] = -1;
		for(j = 0; j < ca->param_count; j++)
		{
			if(cb->param_types[i] == ca->param_types[j])
			{
				for(k = 0; cb->param_names[i][k] == ca->param_names[j][k] && cb->param_names[i][k] != 0; k++);
				if(cb->param_names[i][k] == ca->param_names[j][k])
					translate[i] = j;
			}
		}
	}
	for(i = 0; i < ca->param_count; i++)
		printf("translate: A name[%u] = %s\n", i, ca->param_names[i]);
	for(i = 0; i < cb->param_count; i++)
		printf("translate: B name[%u] = %s\n", i, cb->param_names[i]);
	for(i = 0; i < cb->param_count; i++)
		printf("translate: [%u] = %u\n", i, translate[i]);

	for(i = 0; i < session->entity_define_count; i++)
		c_primitive_entity_transition(session, &session->entity_defines[i], prev_component, new_component, translate);
	c_primitive_entity_transition(session, &session->primitive_settings, prev_component, new_component, translate);
	free(translate);

}


void c_primitive_remove(CSession *session, CDrawState *primitive)
{
	uint i, primitive_id;
	for(i = 0; i < session->primitive_count && &session->prim[i] != primitive; i++);
	primitive_id = i;

	for(i = 0; i < session->component_list_count; i++)
		if(session->component_list[i].primitive == primitive_id)
			confuse_component_remove(session, i--);
				
	if(session->prim[primitive_id].attrib_params != NULL)
		free(session->prim[primitive_id].attrib_params);
	if(session->prim[primitive_id].uniform_params != NULL)
		free(session->prim[primitive_id].uniform_params);
	if(session->prim[primitive_id].attrib_buffer != NULL)
		free(session->prim[primitive_id].attrib_buffer);
	if(session->prim[primitive_id].uniform_buffer != NULL)
		free(session->prim[primitive_id].uniform_buffer);
	if(session->prim[primitive_id].sort_distance != NULL)
		free(session->prim[primitive_id].sort_distance);
	if(session->prim[primitive_id].sort_buffer != NULL)
		free(session->prim[primitive_id].sort_buffer);

	free(session->prim[primitive_id].shaders[0]);
	free(session->prim[primitive_id].shaders[1]);

	session->prim[primitive_id] = session->prim[--session->primitive_count];
	for(i = 0; i < session->component_list_count; i++)
		if(session->component_list[i].primitive == session->primitive_count)
			session->component_list[i].primitive = primitive_id;
}

void c_primitive_reload(CSession *session, CDrawState *primitive)
{
	CDrawState *p;
	uint *translate;
	uint i, j, k, new_component;
	p = c_primitive_add(session, primitive->name);
	p->shader_paths[0] = f_text_copy_allocate(primitive->shader_paths[0]);
	p->shader_paths[1] = f_text_copy_allocate(primitive->shader_paths[1]);
	c_primitive_compile(p);
	if(p->type == C_DPT_DRAW_CALL_UNIFORM)
		c_primitive_inject_uniform(session, p, C_CC_DRAW, FALSE);
	if(p->type == C_DPT_DRAW_CALL_ATTRIBUTE)
	{
		new_component = c_primitive_inject_uniform(session, p, C_CC_PRIMITIVE_STATE, FALSE);
		c_primitive_inject_attribute(session, p);
	}

	for(i = 0; i <session->component_list_count; i++)
	{
		if(session->component_list[i].primitive == session->primitive_count - 1)
		{
			boolean found = FALSE;
			for(j = 0; j < session->component_list_count; j++)
			{
				if(&session->prim[session->component_list[j].primitive] == primitive)
				{
					for(k = 0; session->component_list[i].name[k] == session->component_list[j].name[k] && session->component_list[i].name[k] != 0; k++);
					if(session->component_list[i].name[k] == session->component_list[j].name[k])
					{
						found = TRUE;
						c_primitive_component_transition(session, j, i);
					}
				}
			}
		}
	}

	c_primitive_remove(session, primitive);
}

void confuse_primitive_init_hack(CSession *session)
{
	CDrawState *p;
	return;

}