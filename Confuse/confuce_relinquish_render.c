#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "betray.h"
#include "relinquish.h"
#include "confuse2.h"

struct{
	RShader **shaders;
	void **pools;
	uint count;
}CRStorage;

uint r_confuse_default_texture_id = 0;

/*
typedef enum{
	C_TYPE_FLOAT,
	C_TYPE_AREA,
	C_TYPE_POS,
	C_TYPE_COLOR,
	C_TYPE_MATRIX,
	C_TYPE_IMAGE_2D,
	C_TYPE_IMAGE_3D,
	C_TYPE_IMAGE_CUBE,
	C_TYPE_POS_QUATERNION_SIZE,
	C_TYPE_UV_SECTION,
	C_TYPE_SAME_FLOAT_AS_OUTPUT,
	C_TYPE_COUNT
}CTypeType;

extern uint					confuse_primitive_count();
extern char					*confuse_primitive_name(uint id);
extern CDrawStateType	confuse_primitive_type(uint id);
extern CTypeType			confuse_primitive_param_type(uint id, uint param);
extern char					*confuse_primitive_param_name(uint id, uint param);
extern void					confuse_primitive_blend_func(uint id, uint *source, uint *dist);
extern uint					confuse_primitive_buffer_allocated(uint id);
extern uint					confuse_primitive_buffer_count(uint id);
extern void					*confuse_primitive_buffer_get(uint id);
extern char					*confuse_primitive_shader_get(uint id, uint source);
*/

uint confuse_relinquish_image_2d_func(char *name, uint8 *data, uint x, uint y, void *user)
{
	return r_texture_allocate(R_IF_RGBA_UINT8, x, y, 1, TRUE, TRUE, data);
}

uint confuse_relinquish_image_3d_func(char *name, uint8 *data, uint x, uint y, uint z, void *user)
{
	return r_texture_allocate(R_IF_RGBA_UINT8, x, y, z, TRUE, TRUE, data);
}

uint confuse_relinquish_image_cube_func(char *name, uint8 *data, uint x, uint y, void *user)
{
	return r_texture_allocate(R_IF_RGBA_UINT8, x, y, R_IB_IMAGE_CUBE, TRUE, TRUE, data);
}

void *confuse_relinquish_shader_func(char *name, char *vertex, char *fragment, char *debug_buffer, uint buffer_size, void *user)
{
	return r_shader_create_simple(debug_buffer, buffer_size, vertex, fragment, name);
}

uint confuse_relinquish_shader_localtion_func(void *shader, char *name, void *user)
{
	return r_shader_uniform_location(shader, name);
}


#define CONFUCE_RELINQUISH_PARAM_MAX 64

void *confuse_relinquish_array_func(char *name, uint8 *data, uint *format_types, uint format_count, uint vertex_count, void *user)
{
	SUIFormats vertex_format_type[CONFUCE_RELINQUISH_PARAM_MAX];
	uint vertex_format_size[CONFUCE_RELINQUISH_PARAM_MAX];
	void *pool;
	uint i; 
	if(format_count > CONFUCE_RELINQUISH_PARAM_MAX)
	{
		printf("FATAL ERROR: format_count == %u > CONFUCE_RELINQUISH_PARAM_MAX", format_count);
		exit(0);
	}

	for(i = 0; i < format_count && i < CONFUCE_RELINQUISH_PARAM_MAX; i++)
	{
		switch(format_types[i])
		{
			case C_TYPE_FLOAT :
				vertex_format_type[i] = SUI_FLOAT;
				vertex_format_size[i] = 1;
			break;
			case C_TYPE_UV :
				vertex_format_type[i] = SUI_FLOAT;
				vertex_format_size[i] = 2;
			break;
			case C_TYPE_POS :
				vertex_format_type[i] = SUI_FLOAT;
				vertex_format_size[i] = 3;
			break;
			case C_TYPE_COLOR :
				vertex_format_type[i] = SUI_FLOAT;
				vertex_format_size[i] = 4;
			break;
		}
	}
	pool = r_array_allocate(vertex_count, vertex_format_type, vertex_format_size, format_count, 0);
	if(data != NULL)
		r_array_load_vertex(pool, NULL, data, 0, vertex_count);
	return pool;
}

void confuse_relinquish_array_free_func(void *array, void *user)
{
	r_array_free(array);
}

void confuse_relinquish_init()
{
	confuce_asset_load_image_2d_func(confuse_relinquish_image_2d_func, NULL);
	confuce_asset_load_image_3d_func(confuse_relinquish_image_3d_func, NULL);
	confuce_asset_load_image_cube_func(confuse_relinquish_image_cube_func, NULL);
	confuce_asset_load_shader_func(confuse_relinquish_shader_func, NULL);
	confuce_asset_load_shader_localtion_func(confuse_relinquish_shader_localtion_func, NULL);
	confuce_asset_load_array_func(confuse_relinquish_array_func, NULL);
	confuce_asset_load_array_free_func(confuse_relinquish_array_free_func, NULL);
	r_confuse_default_texture_id = r_texture_allocate(R_IF_RGBA_UINT8, 128, 128, 1, TRUE, TRUE, NULL);
} 

/*
void confuse_relinquish_draw()
{
	uint length, i, src, dest;
	if(c_component_list_primitive_updated)
	{
		confuse_relinquish_exit();
		confuse_relinquish_init();
		c_component_list_primitive_updated = FALSE;
	}
	for(i = 0; i < CRStorage.count; i++)
	{
		r_shader_set(CRStorage.shaders[i]);
		length = confuse_primitive_buffer_count(i);
		r_array_load_vertex(CRStorage.pools[i], NULL, confuse_primitive_buffer_get(i), 0, length);
		confuse_primitive_blend_func(i, &src, &dest);
		if(confuse_primitive_depth_test(i))
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(TRUE);
		}else
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(FALSE);
		}
		glBlendFunc(src, dest);
		r_array_section_draw(CRStorage.pools[i], NULL, GL_TRIANGLES, 0, length);
		confuse_primitive_buffer_clear(i);
	}
	glEnable(GL_DEPTH_TEST);
	glDepthMask(TRUE);
}
*/

void confuse_relinquish_draw(CSession *session, float delta_time)
{
	RShader *shader;
	CTypeType type;
	CDrawState *state;
	void *array, *pool;
	float *uniform_data;
	uint i, j, /*k, */size, call_count, uniform_count, location, texture_id, src, dest, start, end;
	boolean depth_test = TRUE;
	glDisable(GL_CULL_FACE);
	confuce_draw_begin(session, delta_time);
//	for(k = 0; k < 2; k++)
	{
		for(state = confuce_draw_state_get_next(session, NULL); state != NULL; state = confuce_draw_state_get_next(session, state))
		{
			shader = confuce_draw_state_shader_get(state);
			r_shader_set(shader);
			pool = confuce_draw_state_array_get(state);
			array = confuce_draw_state_array_load(state, &size);
			if(array != NULL)	
				r_array_load_vertex(pool, NULL, array, 0, size);

			
			call_count = confuce_draw_state_call_count(state);
		
			for(i = 0; i < call_count; i++)
			{
			//	if(depth_test == confuse_draw_call_depth_test(state, i))
				{
					if(confuse_draw_call_depth_test(state, i))
					{
						glEnable(GL_DEPTH_TEST);
						glDepthMask(TRUE);
					}else
					{
						glDisable(GL_DEPTH_TEST);
						glDepthMask(FALSE);
					}

					uniform_count = confuce_draw_call_uniform_count(state, i);
					for(j = 0; j < uniform_count; j++)
					{
						uniform_data = confuce_draw_call_uniform_data(state, i, j, &type, &location);
						switch(type)
						{
							case C_TYPE_FLOAT :
								r_shader_float_set(shader, location, *uniform_data);
							break;
							case C_TYPE_UV :
								r_shader_vec2_set(shader, location, uniform_data[0], uniform_data[1]);
							break;
							case C_TYPE_POS :
								r_shader_vec3_set(NULL, location, uniform_data[0], uniform_data[1], uniform_data[2]);
							break;
							case C_TYPE_COLOR :
								r_shader_vec4_set(shader, location, uniform_data[0], uniform_data[1], uniform_data[2], uniform_data[3]);
							break;
							case C_TYPE_MATRIX :
								r_shader_mat4v_set(shader, location, uniform_data);
							break;
							case C_TYPE_IMAGE_2D :
							case C_TYPE_IMAGE_3D :
							case C_TYPE_IMAGE_CUBE :
								texture_id = *(uint *)uniform_data;
								if(texture_id == -1)
									r_shader_uniform_texture_set(shader, location, r_confuse_default_texture_id);
								else
									r_shader_uniform_texture_set(shader, location, texture_id);
							break;
						}
					}
					confuse_draw_call_blend_func(state, i, &src, &dest);
					glBlendFunc(src, dest);
					r_matrix_push(NULL);
					r_matrix_matrix_mult(NULL, confuse_draw_call_matrix(state, i));
					confuse_draw_call_draw_range(state, i, &start, &end);
					r_array_section_draw(pool, NULL, GL_TRIANGLES, start, end - start);
					r_matrix_pop(NULL);
				}
			}
		}
		depth_test = FALSE;
	}
	confuce_draw_end(session);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(TRUE);
}
