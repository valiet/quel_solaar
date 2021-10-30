#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "s_draw_3d.h"
#include "confuse2.h"


#define CONFUSE_EDITOR_PREVIEW_LINE_POOL_SIZE 4096
#define CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE 8192

void *confuse_editor_preview_pool = NULL;
void *confuse_editor_preview_line_section = NULL;
void *confuse_editor_preview_sphere_section = NULL;
void *confuse_editor_preview_axis_section = NULL;

float *confuse_editor_preview_line_vertex_array = NULL;
uint confuse_editor_preview_line_vertex_array_used = 0;

RShader *confuse_editor_preview_line_shader = NULL;



char *confuse_editor_preview_line_shader_vertex = 
"attribute vec4 vertex;"
"uniform mat4 ModelViewProjectionMatrix;"
"uniform float time;"
"varying float dist;"
"varying float dist_inv;"
"varying float dist_anim;"
"void main()"
"{"
"	dist = vertex.a;"
"	dist_inv = 1.0 - vertex.a;"
"	dist_anim = vertex.a * 5.0 - time;"
"	gl_Position = ModelViewProjectionMatrix * vec4(vertex.xyz, 1.0);"
"}";

char *confuse_editor_preview_line_shader_fragment = 
"varying float dist;"
"varying float dist_inv;"
"varying float dist_anim;"
"uniform vec4 color_a;"
"uniform vec4 color_b;"
"void main()"
"{"
"	float f;"
"	f = mod(dist_anim, 1.0);"
"	f *= f;"
"	gl_FragColor = mix(color_a, color_b, f * f);"
"	if(f > 0.97)"
"		gl_FragColor += vec4(0.1);"
//"	gl_FragColor = mix(vec4(0.0, 0.0, 0.0, 1), vec4(0.5, 0.6, 0.8, 1), f * f);"
"}";

void confuse_editor_preview_draw_flush()
{
	BInputState *input;
	uint location;
	r_shader_set(confuse_editor_preview_line_shader);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "time");
	input = betray_get_input_state();
	glBlendFunc(1, 1);
	r_shader_float_set(confuse_editor_preview_line_shader, location, input->minute_time * 30.0);

	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "color_a");
	r_shader_vec4_set(confuse_editor_preview_line_shader, location, 0.2, 0.2, 0.2, 1);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "color_b");
	r_shader_vec4_set(confuse_editor_preview_line_shader, location, 0.5, 0.6, 0.8, 1);
	r_array_load_vertex(confuse_editor_preview_pool, confuse_editor_preview_line_section, confuse_editor_preview_line_vertex_array, 0, confuse_editor_preview_line_vertex_array_used / 4);
	r_array_section_draw(confuse_editor_preview_pool, confuse_editor_preview_line_section, R_PRIMITIVE_LINES, 0, confuse_editor_preview_line_vertex_array_used / 4);
	confuse_editor_preview_line_vertex_array_used = 0;
}

void confuse_editor_preview_draw_line(float x_a, float y_a, float z_a, float x_b, float y_b, float z_b)
{
	if(confuse_editor_preview_line_vertex_array >= CONFUSE_EDITOR_PREVIEW_LINE_POOL_SIZE * 4)
		confuse_editor_preview_draw_flush();
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = x_a;
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = y_a;
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = z_a;
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = 0;
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = x_b;
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = y_b;
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = z_b;
	confuse_editor_preview_line_vertex_array[confuse_editor_preview_line_vertex_array_used++] = 1;
}




void confuse_editor_preview_draw_sphere(float x, float y, float z, float radius)
{
	BInputState *input;
	uint location;
	r_shader_set(confuse_editor_preview_line_shader);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "time");
	input = betray_get_input_state();
	glBlendFunc(1, 1);
	r_shader_float_set(confuse_editor_preview_line_shader, location, input->minute_time * 30.0);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "color_a");
	r_shader_vec4_set(confuse_editor_preview_line_shader, location, 0.0, 0.0, 0.0, 1);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "color_b");
	r_shader_vec4_set(confuse_editor_preview_line_shader, location, 0.25, 0.3, 0.4, 1);
	r_matrix_push(NULL);
	r_matrix_translate(NULL, x, y, z);
	r_matrix_scale(NULL, radius, radius, radius);
	r_array_section_draw(confuse_editor_preview_pool, confuse_editor_preview_sphere_section, R_PRIMITIVE_LINES, 0, -1);
	r_matrix_pop(NULL);
}


void confuse_editor_preview_draw_axis(float x_a, float y_a, float z_a, float x_b, float y_b, float z_b)
{
	float matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, rand[3] = {0.3, 0.5, 0.7}, f;
	BInputState *input;
	uint location;
	r_shader_set(confuse_editor_preview_line_shader);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "time");
	input = betray_get_input_state();
	glBlendFunc(1, 1);
	r_shader_float_set(confuse_editor_preview_line_shader, location, input->minute_time * 30.0);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "color_a");
	r_shader_vec4_set(confuse_editor_preview_line_shader, location, 0.2, 0.2, 0.2, 1);
	location = r_shader_uniform_location(confuse_editor_preview_line_shader, "color_b");
	r_shader_vec4_set(confuse_editor_preview_line_shader, location, 0.25, 0.3, 0.4, 1);
	r_matrix_push(NULL);
	matrix[0] = x_b - x_a;
	matrix[1] = y_b - y_a;
	matrix[2] = z_b - z_a;
	f = f_length3f(matrix);
	f_cross3f(&matrix[4], matrix, rand);
	f_normalize3f(&matrix[4]);
	matrix[4] *= f;
	matrix[5] *= f;
	matrix[6] *= f;
	f_cross3f(&matrix[8], matrix, &matrix[4]);
	f_normalize3f(&matrix[8]);
	matrix[8] *= f;
	matrix[9] *= f;
	matrix[10] *= f;
	matrix[12] = x_a;
	matrix[13] = y_a;
	matrix[14] = z_a;
	r_matrix_matrix_mult(NULL, matrix);
	r_array_section_draw(confuse_editor_preview_pool, confuse_editor_preview_axis_section, R_PRIMITIVE_LINES, 0, -1);
	r_matrix_pop(NULL);
}



void confuse_editor_wiggle3df(float *out, float f, float size, uint period)
{
	float v0[3], v1[3], v2[3], v3[3];
	uint seed;
	period *= 3;
	seed = (float)f;
	f -= (float)seed;
	seed *= 6;
	size *= 2.0;
	seed %= period;
	v0[0] = (f_randf(seed) - 0.5) * size;
	v0[1] = (f_randf(seed + 1) - 0.5) * size;
	v0[2] = (f_randf(seed + 2) - 0.5) * size;
	seed = (seed + 3) % period;
	v1[0] = (f_randf(seed) - 0.5) * size;
	v1[1] = (f_randf(seed + 1) - 0.5) * size;
	v1[2] = (f_randf(seed + 2) - 0.5) * size;
	seed = (seed + 3) % period;
	v3[0] = (f_randf(seed) - 0.5) * size;
	v3[1] = (f_randf(seed + 1) - 0.5) * size;
	v3[2] = (f_randf(seed + 2) - 0.5) * size;
	seed = (seed + 3) % period;
	v2[0] = v3[0] * 2.0 - (f_randf(seed++) - 0.5) * size;
	v2[1] = v3[1] * 2.0 - (f_randf(seed++) - 0.5) * size;
	v2[2] = v3[2] * 2.0 - (f_randf(seed++) - 0.5) * size;
	f_normalize3f(v0);
	f_normalize3f(v1);
	f_normalize3f(v2);
	f_normalize3f(v3);
	f_spline3df(out, f, v0, v1, v2, v3);
}


void confuse_editor_preview_draw_init()
{
	char debug_output[2048];
	SUIFormats vertex_format_types[1] = {SUI_FLOAT};
	uint i, vertex_format_size[1] = {4};
	float *array, f;
	c_preview_draw_line_func_set(confuse_editor_preview_draw_line);
	c_preview_draw_sphere_func_set(confuse_editor_preview_draw_sphere);
	c_preview_draw_axis_func_set(confuse_editor_preview_draw_axis);


	confuse_editor_preview_pool = r_array_allocate(CONFUSE_EDITOR_PREVIEW_LINE_POOL_SIZE + 
													CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE + 
													4 + 64 * 4, vertex_format_types, vertex_format_size, 1, 0); 

	confuse_editor_preview_line_section = r_array_section_allocate_vertex(confuse_editor_preview_pool, CONFUSE_EDITOR_PREVIEW_LINE_POOL_SIZE);
	confuse_editor_preview_line_vertex_array = malloc((sizeof *confuse_editor_preview_line_vertex_array) * CONFUSE_EDITOR_PREVIEW_LINE_POOL_SIZE * 4);
	confuse_editor_preview_line_shader = r_shader_create_simple(debug_output, 2048, confuse_editor_preview_line_shader_vertex, confuse_editor_preview_line_shader_fragment, "Wireframe");
	if(confuse_editor_preview_line_shader == NULL)
		confuse_editor_preview_line_shader = NULL;

	r_shader_state_set_blend_mode(confuse_editor_preview_line_shader, 1, 1);

	confuse_editor_preview_sphere_section = r_array_section_allocate_vertex(confuse_editor_preview_pool, CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE);

	array = malloc((sizeof *confuse_editor_preview_line_vertex_array) * CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE * 4);

	for(i = 0; i < CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE; i += 2)
	{
		confuse_editor_wiggle3df(&array[i * 4], (float)i / CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE * 50.0, 1.0, CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE / 50);
		f_normalize3f(&array[i * 4]);
		array[i * 4 + 3] = (float)i / CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE * 20;
		confuse_editor_wiggle3df(&array[i * 4 + 4], (float)(i + 2) / CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE * 50.0, 1.0, CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE / 50);
		f_normalize3f(&array[i * 4 + 4]);
		array[i * 4 + 7] = (float)(i + 2) / CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE * 20;
	}
	r_array_load_vertex(confuse_editor_preview_pool, confuse_editor_preview_sphere_section, array, 0, CONFUSE_EDITOR_PREVIEW_SPHERE_POOL_SIZE);
	

	confuse_editor_preview_axis_section = r_array_section_allocate_vertex(confuse_editor_preview_pool, 4 + 64 * 4);
	array[0] = -1;
	array[1] = 0;
	array[2] = 0;
	array[3] = 0;

	array[4] = 0.5;
	array[5] = 0;
	array[6] = 0;
	array[7] = 1;

	array[8] = 2;
	array[9] = 0;
	array[10] = 0;
	array[11] = 0;

	array[12] = 0.5;
	array[13] = 0;
	array[14] = 0;
	array[15] = 1;
	r_array_load_vertex(confuse_editor_preview_pool, confuse_editor_preview_axis_section, array, 0, 4);

	for(i = 0; i < 64; i++)
	{
		f = (float)i / 64.0;
		array[i * 8 + 3] = f;
		f *= PI * 2.0;
		array[i * 8 + 0] = 0;
		array[i * 8 + 1] = sin(f) * 0.2;
		array[i * 8 + 2] = cos(f) * 0.2;
		f = (float)(i + 1) / 64.0;
		array[i * 8 + 7] = f;
		f *= PI * 2.0;
		array[i * 8 + 4] = 0;
		array[i * 8 + 5] = sin(f) * 0.2;
		array[i * 8 + 6] = cos(f) * 0.2;
	}
	r_array_load_vertex(confuse_editor_preview_pool, confuse_editor_preview_axis_section, array, 4, 64 * 2);

	for(i = 0; i < 64 * 2; i++)
	{
		array[i * 4 + 0] = 1;
		array[i * 4 + 1] = -array[i * 4 + 1];
	}
	r_array_load_vertex(confuse_editor_preview_pool, confuse_editor_preview_axis_section, array, 4 + 64 * 2, 64 * 2);

	free(array);
}