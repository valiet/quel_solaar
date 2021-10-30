#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "s_draw_3d.h"
#include "c_internal.h"


char *CGlobalPointEditor_shader_vertex = 
"attribute vec3 vertex;"
"attribute vec3 uv;"
"attribute vec4 color;"
"uniform mat4 ModelViewProjectionMatrix;"
"uniform mat4 NormalMatrix;"
"uniform float size;"
"varying vec4 selection;"
"varying vec4 c;"
"varying vec2 mapping;"
"void main()"
"{"
"	vec4 v, distance;"
"	v = vec4(uv.xy * vec2(0.005), 0.0, 0.0) * NormalMatrix;"
"	c = color;"
"	selection = vec4(uv.zzz, 1.0);"
"	mapping = uv.xy;"
"	distance = ModelViewProjectionMatrix * vec4(vertex, 1.0);"
"   gl_Position =  ModelViewProjectionMatrix * (vec4(vertex, 1.0) + v * distance.zzzz);"
"}";

char *CGlobalPointEditor_shader_fragment = 
"varying vec4 selection;"
"varying vec4 c;"
"varying vec2 mapping;"
"void main()"
"{"
"	float f;"
"	f = dot(mapping, mapping);"
"	if(f > 1.0)"
"		discard;"
"	else if(f > 0.5)"
"		gl_FragColor = selection;"
"	else"
"		gl_FragColor = c;"
"}";

struct{
	boolean *included;
	uint position;
	uint visualize;
	uint active;
	uint selected_count;
	float selection_center[3];
	float *array;
	void *pool;
	RShader *shader;
	CEntity *entity;
}CGlobalPointEditor;


void confuse_point_editor_init()
{
	char buffer[2048];
	CGlobalPointEditor.shader = r_shader_create_simple(buffer, 2048, CGlobalPointEditor_shader_vertex, CGlobalPointEditor_shader_fragment, "point editor");
	CGlobalPointEditor.included = NULL;
	CGlobalPointEditor.array = NULL;
	CGlobalPointEditor.pool = NULL;
}


void confuse_point_editor_end()
{
	if(CGlobalPointEditor.included != NULL)
		free(CGlobalPointEditor.included);
	CGlobalPointEditor.included = NULL;

	if(CGlobalPointEditor.array != NULL)
		free(CGlobalPointEditor.array);
	CGlobalPointEditor.array = NULL;
	if(CGlobalPointEditor.pool != NULL)
		r_array_free(CGlobalPointEditor.pool);
	CGlobalPointEditor.pool = NULL;
	CGlobalPointEditor.entity = NULL;
}

void confuse_point_editor_pool_update_pos(CEntity *entity)
{
	uint i, stride, size;
	float *pointer;
	switch(entity->stack_types[CGlobalPointEditor.position].type)
	{

		case C_TYPE_POS :
			pointer = &entity->stack[entity->stack_types[CGlobalPointEditor.position].pos];
			stride = 3;
		break;
		case C_TYPE_MATRIX :
			pointer = &entity->stack[entity->stack_types[CGlobalPointEditor.position].pos + 12];
			stride = 16;
		break;
		case C_TYPE_POS_QUATERNION_SIZE :
			pointer = &entity->stack[entity->stack_types[CGlobalPointEditor.position].pos];
			stride = 10;
		break;
		default :
		return;
	}
	size = entity->stack_types[CGlobalPointEditor.position].size;
	for(i = 0; i < size; i++)
	{
		CGlobalPointEditor.array[60 * i + 10 * 0 + 0] = CGlobalPointEditor.array[60 * i + 10 * 1 + 0] = CGlobalPointEditor.array[60 * i + 10 * 2 + 0] = CGlobalPointEditor.array[60 * i + 10 * 3 + 0] = CGlobalPointEditor.array[60 * i + 10 * 4 + 0] = CGlobalPointEditor.array[60 * i + 10 * 5 + 0] = pointer[i * stride + 0]; 
		CGlobalPointEditor.array[60 * i + 10 * 0 + 1] = CGlobalPointEditor.array[60 * i + 10 * 1 + 1] = CGlobalPointEditor.array[60 * i + 10 * 2 + 1] = CGlobalPointEditor.array[60 * i + 10 * 3 + 1] = CGlobalPointEditor.array[60 * i + 10 * 4 + 1] = CGlobalPointEditor.array[60 * i + 10 * 5 + 1] = pointer[i * stride + 1]; 
		CGlobalPointEditor.array[60 * i + 10 * 0 + 2] = CGlobalPointEditor.array[60 * i + 10 * 1 + 2] = CGlobalPointEditor.array[60 * i + 10 * 2 + 2] = CGlobalPointEditor.array[60 * i + 10 * 3 + 2] = CGlobalPointEditor.array[60 * i + 10 * 4 + 2] = CGlobalPointEditor.array[60 * i + 10 * 5 + 2] = pointer[i * stride + 2]; 
	}
}

void confuse_point_editor_pool_update_color(CEntity *entity)
{
	uint i, stride, size;
	float *pointer;
	size = entity->stack_types[CGlobalPointEditor.position].size;

	if(CGlobalPointEditor.visualize < entity->stack_type_count)
	{
		pointer = &entity->stack[entity->stack_types[CGlobalPointEditor.visualize].pos];
		switch(entity->stack_types[CGlobalPointEditor.visualize].type)
		{
			case C_TYPE_FLOAT :
			for(i = 0; i < size; i++)
			{
				CGlobalPointEditor.array[60 * i + 10 * 0 + 6] = CGlobalPointEditor.array[60 * i + 10 * 1 + 6] = CGlobalPointEditor.array[60 * i + 10 * 2 + 6] = CGlobalPointEditor.array[60 * i + 10 * 3 + 6] = CGlobalPointEditor.array[60 * i + 10 * 4 + 6] = CGlobalPointEditor.array[60 * i + 10 * 5 + 6] = 
				CGlobalPointEditor.array[60 * i + 10 * 0 + 7] = CGlobalPointEditor.array[60 * i + 10 * 1 + 7] = CGlobalPointEditor.array[60 * i + 10 * 2 + 7] = CGlobalPointEditor.array[60 * i + 10 * 3 + 7] = CGlobalPointEditor.array[60 * i + 10 * 4 + 7] = CGlobalPointEditor.array[60 * i + 10 * 5 + 7] = 
				CGlobalPointEditor.array[60 * i + 10 * 0 + 8] = CGlobalPointEditor.array[60 * i + 10 * 1 + 8] = CGlobalPointEditor.array[60 * i + 10 * 2 + 8] = CGlobalPointEditor.array[60 * i + 10 * 3 + 8] = CGlobalPointEditor.array[60 * i + 10 * 4 + 8] = CGlobalPointEditor.array[60 * i + 10 * 5 + 8] = pointer[i]; 
				CGlobalPointEditor.array[60 * i + 10 * 0 + 9] = CGlobalPointEditor.array[60 * i + 10 * 1 + 9] = CGlobalPointEditor.array[60 * i + 10 * 2 + 9] = CGlobalPointEditor.array[60 * i + 10 * 3 + 9] = CGlobalPointEditor.array[60 * i + 10 * 4 + 9] = CGlobalPointEditor.array[60 * i + 10 * 5 + 9] = 1; 
			}
			return;
			case C_TYPE_UV :
			for(i = 0; i < size; i++)
			{
				CGlobalPointEditor.array[60 * i + 10 * 0 + 6] = CGlobalPointEditor.array[60 * i + 10 * 1 + 6] = CGlobalPointEditor.array[60 * i + 10 * 2 + 6] = CGlobalPointEditor.array[60 * i + 10 * 3 + 6] = CGlobalPointEditor.array[60 * i + 10 * 4 + 6] = CGlobalPointEditor.array[60 * i + 10 * 5 + 6] = pointer[i * 2];
				CGlobalPointEditor.array[60 * i + 10 * 0 + 7] = CGlobalPointEditor.array[60 * i + 10 * 1 + 7] = CGlobalPointEditor.array[60 * i + 10 * 2 + 7] = CGlobalPointEditor.array[60 * i + 10 * 3 + 7] = CGlobalPointEditor.array[60 * i + 10 * 4 + 7] = CGlobalPointEditor.array[60 * i + 10 * 5 + 7] = pointer[i * 2 + 1];
				CGlobalPointEditor.array[60 * i + 10 * 0 + 8] = CGlobalPointEditor.array[60 * i + 10 * 1 + 8] = CGlobalPointEditor.array[60 * i + 10 * 2 + 8] = CGlobalPointEditor.array[60 * i + 10 * 3 + 8] = CGlobalPointEditor.array[60 * i + 10 * 4 + 8] = CGlobalPointEditor.array[60 * i + 10 * 5 + 8] =  0.5; 
				CGlobalPointEditor.array[60 * i + 10 * 0 + 9] = CGlobalPointEditor.array[60 * i + 10 * 1 + 9] = CGlobalPointEditor.array[60 * i + 10 * 2 + 9] = CGlobalPointEditor.array[60 * i + 10 * 3 + 9] = CGlobalPointEditor.array[60 * i + 10 * 4 + 9] = CGlobalPointEditor.array[60 * i + 10 * 5 + 9] = 1; 
			}
			return;
			case C_TYPE_COLOR :
			for(i = 0; i < size; i++)
			{
				CGlobalPointEditor.array[60 * i + 10 * 0 + 6] = CGlobalPointEditor.array[60 * i + 10 * 1 + 6] = CGlobalPointEditor.array[60 * i + 10 * 2 + 6] = CGlobalPointEditor.array[60 * i + 10 * 3 + 6] = CGlobalPointEditor.array[60 * i + 10 * 4 + 6] = CGlobalPointEditor.array[60 * i + 10 * 5 + 6] = pointer[i * 4 + 0]; 
				CGlobalPointEditor.array[60 * i + 10 * 0 + 7] = CGlobalPointEditor.array[60 * i + 10 * 1 + 7] = CGlobalPointEditor.array[60 * i + 10 * 2 + 7] = CGlobalPointEditor.array[60 * i + 10 * 3 + 7] = CGlobalPointEditor.array[60 * i + 10 * 4 + 7] = CGlobalPointEditor.array[60 * i + 10 * 5 + 7] = pointer[i * 4 + 1]; 
				CGlobalPointEditor.array[60 * i + 10 * 0 + 8] = CGlobalPointEditor.array[60 * i + 10 * 1 + 8] = CGlobalPointEditor.array[60 * i + 10 * 2 + 8] = CGlobalPointEditor.array[60 * i + 10 * 3 + 8] = CGlobalPointEditor.array[60 * i + 10 * 4 + 8] = CGlobalPointEditor.array[60 * i + 10 * 5 + 8] = pointer[i * 4 + 2]; 
				CGlobalPointEditor.array[60 * i + 10 * 0 + 9] = CGlobalPointEditor.array[60 * i + 10 * 1 + 9] = CGlobalPointEditor.array[60 * i + 10 * 2 + 9] = CGlobalPointEditor.array[60 * i + 10 * 3 + 9] = CGlobalPointEditor.array[60 * i + 10 * 4 + 9] = CGlobalPointEditor.array[60 * i + 10 * 5 + 9] = pointer[i * 4 + 3]; 
			}
			return;
		}
	}
	for(i = 0; i < size ; i++)
	{
		CGlobalPointEditor.array[60 * i + 10 * 0 + 6] = CGlobalPointEditor.array[60 * i + 10 * 1 + 6] = CGlobalPointEditor.array[60 * i + 10 * 2 + 6] = CGlobalPointEditor.array[60 * i + 10 * 3 + 6] = CGlobalPointEditor.array[60 * i + 10 * 4 + 6] = CGlobalPointEditor.array[60 * i + 10 * 5 + 6] = 0.2; 
		CGlobalPointEditor.array[60 * i + 10 * 0 + 7] = CGlobalPointEditor.array[60 * i + 10 * 1 + 7] = CGlobalPointEditor.array[60 * i + 10 * 2 + 7] = CGlobalPointEditor.array[60 * i + 10 * 3 + 7] = CGlobalPointEditor.array[60 * i + 10 * 4 + 7] = CGlobalPointEditor.array[60 * i + 10 * 5 + 7] = 0.6; 
		CGlobalPointEditor.array[60 * i + 10 * 0 + 8] = CGlobalPointEditor.array[60 * i + 10 * 1 + 8] = CGlobalPointEditor.array[60 * i + 10 * 2 + 8] = CGlobalPointEditor.array[60 * i + 10 * 3 + 8] = CGlobalPointEditor.array[60 * i + 10 * 4 + 8] = CGlobalPointEditor.array[60 * i + 10 * 5 + 8] = 1; 
		CGlobalPointEditor.array[60 * i + 10 * 0 + 9] = CGlobalPointEditor.array[60 * i + 10 * 1 + 9] = CGlobalPointEditor.array[60 * i + 10 * 2 + 9] = CGlobalPointEditor.array[60 * i + 10 * 3 + 9] = CGlobalPointEditor.array[60 * i + 10 * 4 + 9] = CGlobalPointEditor.array[60 * i + 10 * 5 + 9] = 1; 
	}
}

void confuse_point_editor_pool_generate(CEntity *entity)
{
	SUIFormats vertex_format_types[3] = {SUI_FLOAT, SUI_FLOAT, SUI_FLOAT};
	uint vertex_format_size[3] = {3, 3, 4};
	uint i, size;
	for(i = 0; i < entity->stack_type_count; i++)
		if(CGlobalPointEditor.included[i])
			size = entity->stack_types[i].size;

	if(CGlobalPointEditor.array != NULL)
		free(CGlobalPointEditor.array);
	CGlobalPointEditor.array = malloc((sizeof *CGlobalPointEditor.array) * size * 10 * 6);
	/* set pos */
	confuse_point_editor_pool_update_pos(entity);
	/* set uv */
	for(i = 0; i < size; i++)
	{
		CGlobalPointEditor.array[10 * 6 * i + 10 * 0 + 3] = 1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 0 + 4] = 1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 1 + 3] = 1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 1 + 4] = -1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 2 + 3] = -1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 2 + 4] = -1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 3 + 3] = 1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 3 + 4] = 1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 4 + 3] = -1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 4 + 4] = -1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 5 + 3] = -1.0; 
		CGlobalPointEditor.array[10 * 6 * i + 10 * 5 + 4] = 1.0; 
	}
	/* set select */
	for(i = 0; i < size; i++)
		CGlobalPointEditor.array[10 * 6 * i + 10 * 0 + 5] = CGlobalPointEditor.array[10 * 6 * i + 10 * 1 + 5] = CGlobalPointEditor.array[10 * 6 * i + 10 * 2 + 5] = CGlobalPointEditor.array[10 * 6 * i + 10 * 3 + 5] = CGlobalPointEditor.array[10 * 6 * i + 10 * 4 + 5] = CGlobalPointEditor.array[10 * 6 * i + 10 * 5 + 5] = 0.0;  
	
	/* set color */
	confuse_point_editor_pool_update_color(entity);

	if(CGlobalPointEditor.pool != NULL)
		r_array_free(CGlobalPointEditor.pool);
	CGlobalPointEditor.pool = r_array_allocate(size * 6, vertex_format_types, vertex_format_size, 3, 0);
	r_array_load_vertex(CGlobalPointEditor.pool, NULL, CGlobalPointEditor.array, 0, size * 6);
}

void confuse_point_editor_set(CEntity *entity, uint param)
{
	uint i, size;
	if(CGlobalPointEditor.entity != entity || entity->stack_types[CGlobalPointEditor.position].size != entity->stack_types[param].size)
	{
		confuse_point_editor_end();
	}

	if(CGlobalPointEditor.included == NULL)
	{
		confuse_point_editor_end();
		size = entity->stack_types[param].size;
		CGlobalPointEditor.included = malloc((sizeof *CGlobalPointEditor.included) * entity->stack_type_count);

		for(i = 0; i < entity->stack_type_count; i++)
			CGlobalPointEditor.included[i] = size == entity->stack_types[i].size;
		for(i = 0; i < entity->stack_type_count && 
			(!CGlobalPointEditor.included[i] || (
			entity->stack_types[i].type != C_TYPE_POS &&
			entity->stack_types[i].type != C_TYPE_MATRIX &&
			entity->stack_types[i].type != C_TYPE_POS_QUATERNION_SIZE)); i++);
		if(i < entity->stack_type_count)
			CGlobalPointEditor.position = i;
		CGlobalPointEditor.visualize = -1;
	}

	CGlobalPointEditor.active = param;
	CGlobalPointEditor.entity = entity;
	if(entity->stack_types[param].type == C_TYPE_POS ||
		entity->stack_types[param].type == C_TYPE_MATRIX ||
		entity->stack_types[param].type == C_TYPE_POS_QUATERNION_SIZE)
		CGlobalPointEditor.position = param;
	else
		CGlobalPointEditor.visualize = param;
	confuse_point_editor_pool_generate(entity);
	entity->component_selected = -1;
}

void confuse_point_editor(BInputState *input, CEntity *entity, RMatrix *interface_matrix, RMatrix *camera_matrix)
{ 
	if(CGlobalPointEditor.entity != entity || entity->component_selected != -1)
		confuse_point_editor_end();
	if(input->mode == BAM_DRAW && CGlobalPointEditor.pool != NULL)
	{
		r_matrix_set(camera_matrix);
		r_shader_set(CGlobalPointEditor.shader);
		r_array_section_draw(CGlobalPointEditor.pool, NULL, GL_TRIANGLES, 0, -1);
		if(CGlobalPointEditor.selected_count != 0)
		{
			r_primitive_line_3d(CGlobalPointEditor.selection_center[0] + 0.1,
								CGlobalPointEditor.selection_center[1],
								CGlobalPointEditor.selection_center[2],
								CGlobalPointEditor.selection_center[0] - 0.1,
								CGlobalPointEditor.selection_center[1],
								CGlobalPointEditor.selection_center[2], 1, 0, 0, 1);
			r_primitive_line_3d(CGlobalPointEditor.selection_center[0],
								CGlobalPointEditor.selection_center[1] + 0.1,
								CGlobalPointEditor.selection_center[2],
								CGlobalPointEditor.selection_center[0],
								CGlobalPointEditor.selection_center[1] - 0.1,
								CGlobalPointEditor.selection_center[2], 0, 1, 0, 1);
			r_primitive_line_3d(CGlobalPointEditor.selection_center[0],
								CGlobalPointEditor.selection_center[1],
								CGlobalPointEditor.selection_center[2] + 0.1,
								CGlobalPointEditor.selection_center[0],
								CGlobalPointEditor.selection_center[1],
								CGlobalPointEditor.selection_center[2] - 0.1, 0, 0, 1, 1);
			r_primitive_line_flush();
		}
	}
	if(input->mode == BAM_EVENT && CGlobalPointEditor.array != NULL)
	{
		uint i, j, k, size;
		float output[3], min[2], max[2];
		for(i = 0; i < input->pointer_count; i++)
		{
			if(input->pointers[i].button[0]/* && input->pointers[i].last_button[0]*/)
			{
				if(input->pointers[i].click_pointer_x[0] < input->pointers[i].pointer_x)
				{
					min[0] = input->pointers[i].click_pointer_x[0];
					max[0] = input->pointers[i].pointer_x;
				}else
				{
					max[0] = input->pointers[i].click_pointer_x[0];
					min[0] = input->pointers[i].pointer_x;
				}
				if(input->pointers[i].click_pointer_y[0] < input->pointers[i].pointer_y)
				{
					min[1] = input->pointers[i].click_pointer_y[0];
					max[1] = input->pointers[i].pointer_y;
				}else
				{
					max[1] = input->pointers[i].click_pointer_y[0];
					min[1] = input->pointers[i].pointer_y;
				}
				size = entity->stack_types[CGlobalPointEditor.position].size;

				CGlobalPointEditor.selected_count = 0;
				CGlobalPointEditor.selection_center[0] = 0;
				CGlobalPointEditor.selection_center[1] = 0;
				CGlobalPointEditor.selection_center[2] = 0;

				for(j = 0; j < size; j++)
				{
					r_matrix_projection_screenf(camera_matrix, output, CGlobalPointEditor.array[10 * 6 * j + 0], CGlobalPointEditor.array[10 * 6 * j + 1], CGlobalPointEditor.array[10 * 6 * j + 2]);
					if(output[0] < min[0] || 
						output[0] > max[0] || 
						output[1] < min[1] || 
						output[1] > max[1])
						CGlobalPointEditor.array[10 * 6 * j + 10 * 0 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 1 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 2 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 3 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 4 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 5 + 5] = 0.0;  
					else
					{
						CGlobalPointEditor.selected_count++;
						CGlobalPointEditor.selection_center[0] += CGlobalPointEditor.array[10 * 6 * j + 0];
						CGlobalPointEditor.selection_center[1] += CGlobalPointEditor.array[10 * 6 * j + 1];
						CGlobalPointEditor.selection_center[2] += CGlobalPointEditor.array[10 * 6 * j + 2];
						CGlobalPointEditor.array[10 * 6 * j + 10 * 0 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 1 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 2 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 3 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 4 + 5] = CGlobalPointEditor.array[10 * 6 * j + 10 * 5 + 5] = 1.0;  
					}

				}
				if(CGlobalPointEditor.selected_count != 0)
				{
					CGlobalPointEditor.selection_center[0] /= CGlobalPointEditor.selected_count;
					CGlobalPointEditor.selection_center[1] /= CGlobalPointEditor.selected_count;
					CGlobalPointEditor.selection_center[2] /= CGlobalPointEditor.selected_count;
				}
				r_array_load_vertex(CGlobalPointEditor.pool, NULL, CGlobalPointEditor.array, 0, size * 6);
			}
		} 
	}
	if(betray_button_get(-1, BETRAY_BUTTON_DELETE) && CGlobalPointEditor.selected_count != 0)
	{
		uint i, j, k, size, stride, pos;
		float *p;
		size = entity->stack_types[CGlobalPointEditor.position].size;
		for(i = 0; i < entity->stack_type_count; i++)
		{
			if(CGlobalPointEditor.included[i])
			{
				p = &entity->stack[entity->stack_types[i].pos];
				stride = c_type_size_get(entity->stack_types[i].type);
				for(j = pos = 0; j < size; j++)
				{
					if(CGlobalPointEditor.array[10 * 6 * j + 10 * 0 + 5] < 0.1)
					{
						for(k = 0; k < stride; k++)
							p[pos++] = p[j * stride + k];
					}
				}
				c_entity_stack_resize(entity, i, pos / stride);
			}
		}
		confuse_point_editor_pool_generate(entity);
	}
}


