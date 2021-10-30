#include "betray.h"
#include "seduce.h"

char *sui_draw_shader_color_vertex = 
"attribute vec3 vertex;"
"attribute vec4 color;"
"uniform mat4 ModelViewProjectionMatrix;"
"varying vec4 col;"
"void main()"
"{"
"	col = color;"
"	gl_Position = ModelViewProjectionMatrix * vec4(vertex, 1.0);"
"}";

char *sui_draw_shader_color_fragment = 
"varying vec4 col;"
"void main()"
"{"
"	gl_FragColor = col;"
"}";

char *sui_draw_shader_surface_vertex = 
"attribute vec4 vertex;"
"attribute vec4 alpha;"
"uniform vec4 color;"
"uniform vec3 pos;"
"uniform vec2 size;"
"uniform vec2 pixel;"
"uniform mat4 ModelViewProjectionMatrix;"
"uniform mat4 NormalMatrix;"
"varying vec4 col;"
"varying vec3 normal;"
"varying vec4 v;"
"void main()"
"{"
"	vec3 expand;"
"	vec3 vec;"
"	col = color * alpha;"
"	normal = normalize(NormalMatrix * vec3(0.0, 0.0, -1.0));"
"	vec = pos + vec3(vertex.xy * size, 0.0);"
"	expand = vec3((ModelViewProjectionMatrix * vec4(vec, 1.0)).zz * vertex.ba * pixel, 0);"
//"	gl_Position = v = gl_ModelViewProjectionMatrix * vec4(vec + expand, 1.0);"
//"	expand = vec3((gl_NormalMatrix * vec).zz * vertex.ba * pixel, 0);"
"	gl_Position = v = ModelViewProjectionMatrix * vec4(vec + expand, 1.0) + vec4(expand, 0);"
"}";

char *sui_draw_shader_surface_fragment = 
"uniform sampler2D reflection;"
"varying vec4 col;"
"varying vec3 normal;"
"varying vec4 v;"
"void main()"
"{"
"	vec4 sample;"
"	vec3 ref;"
"	ref = normalize(reflect(normalize(v.xyz), normal.xyz)) * vec3(0.5) + vec3(0.5);"
"	gl_FragColor = col + vec4(texture2D(reflection, ref.xy).xyz * (vec3(1.0) + (normal.zzz)), 0.0);"
"}";

char *sui_draw_shader_image_vertex = 
"attribute vec4 vertex;"
"attribute vec4 alpha;"
"uniform vec4 uv;"
"uniform vec4 color;"
"uniform vec3 pos;"
"uniform vec2 size;"
"uniform vec2 pixel;"
"uniform mat4 ModelViewProjectionMatrix;"
"uniform mat4 NormalMatrix;"
"varying vec4 col;"
"varying vec3 normal;"
"varying vec4 v;"
"varying vec2 mapping;"
"void main()"
"{"
"	vec3 expand;"
"	vec3 vec;"
"	mapping = uv.rg + vertex.xy * uv.ba;"
"	col = color * alpha;"
"	normal = normalize(NormalMatrix * vec3(0.0, 0.0, -1.0));"
"	vec = pos + vec3(vertex.xy * size, 0.0);"
"	expand = vec3((ModelViewProjectionMatrix * vec4(vec, 1.0)).zz * vertex.ba * vec2(pixel), 0);"
"	gl_Position = v = ModelViewProjectionMatrix * vec4(vec + expand, 1.0);"
"}";

char *sui_draw_shader_image_fragment = 
"uniform sampler2D image;"
"uniform sampler2D reflection;"
"varying vec4 col;"
"varying vec3 normal;"
"varying vec4 v;"
"varying vec2 mapping;"
"void main()"
"{"
"	vec4 sample;"
"	vec3 ref;"
"	ref = normalize(reflect(normalize(v.xyz), normal.xyz)) * vec3(0.5) + vec3(0.5);"
"	sample = texture2D(image, mapping);"
"	gl_FragColor = col * sample + vec4(texture2D(reflection, ref.xy).xyz * (vec3(1.0) + (normal.zzz)), col.a) * (vec4(1.0) - sample);"
"}";


char *sui_draw_shader_sprite_vertex = 
"attribute vec3 vertex;"
"attribute vec4 color;"
"attribute vec4 size;"
"uniform mat4 camera_matrix;"
"uniform mat4 ModelViewProjectionMatrix;"
"varying vec2 uv;"
"varying vec4 col;"
"void main()"
"{"
"	uv = size.zw;"
"	col = color;"
"	gl_Position = ModelViewProjectionMatrix * (vec4(vertex.xyz, 1.0) + vec4(size.xy, 0.0, 0.0) * camera_matrix);"
"}";

char *sui_draw_shader_sprite_fragment = 
"uniform sampler2D image;"
"varying vec2 uv;"
"varying vec4 col;"
"void main()"
"{"
"	gl_FragColor = texture2D(image, uv.xy) * col;"
"}";


#define SUI_DRAW_SURFACE_BUFFER (128 * 6)
#define SUI_DRAW_IMAGE_BUFFER (6 * 5)
#define SUI_DRAW_LINE_BUFFER (1024 * 16)
#define SUI_DRAW_SPRITE_BUFFER (1024 * 16)

extern uint sui_3d_texture_shade;
extern uint sui_3d_texture_reflection;

void *sui_draw_surface_pool = NULL;
void *sui_draw_surface_section = NULL;
uint sui_draw_surface_count = 0;

void *sui_draw_line_pool = NULL;
void *sui_draw_line_section = NULL;
uint sui_draw_line_count = 0;

void *sui_draw_sprite_pool = NULL;
void *sui_draw_sprite_section = NULL;
uint sui_draw_sprite_count = 0;
uint sui_draw_sprite_texture_id = -1;

void *sui_draw_image_pool = NULL;
void *sui_draw_image_section = NULL;

uint sui_draw_color_shader;
uint sui_draw_image_shader;
uint sui_draw_image_location_uv;
uint sui_draw_image_location_color;
uint sui_draw_image_location_pos;
uint sui_draw_image_location_size;
uint sui_draw_image_location_pixel;
uint sui_draw_surface_shader;
uint sui_draw_surface_location_color;
uint sui_draw_surface_location_pos;
uint sui_draw_surface_location_size;
uint sui_draw_surface_location_pixel;
uint sui_draw_sprite_shader;
uint sui_draw_sprite_location_camera_matrix;
float sui_draw_sprite_matrix[16];

void sui_draw_primitive_image_aa_set(float *array, float pos_x, float pos_y, float alpha, float dist_x, float dist_y)
{
	array[0] = pos_x;
	array[1] = pos_y;
	array[2] = dist_x;
	array[3] = dist_y;
	array[4] = 1;
	array[5] = 1;
	array[6] = 1;
	array[7] = alpha;
}

void sui_draw_primitive_init()
{
	SUIFormats vertex_format_types[3] = {SUI_FLOAT, SUI_FLOAT, SUI_FLOAT};
	uint vertex_format_size[3] = {3, 4, 4};
	uint vertex_format_size_surface[2] = {4, 4};
	float array[8 * 30];
	char buffer[2048];

	sui_draw_line_pool = sui_draw_array_allocate(SUI_DRAW_LINE_BUFFER, vertex_format_types, vertex_format_size, 2);
	sui_draw_line_section = sui_draw_array_section_allocate(sui_draw_line_pool, SUI_DRAW_LINE_BUFFER);

	sui_draw_surface_pool = sui_draw_array_allocate(SUI_DRAW_SURFACE_BUFFER, vertex_format_types, vertex_format_size, 2);
	sui_draw_surface_section = sui_draw_array_section_allocate(sui_draw_surface_pool, SUI_DRAW_SURFACE_BUFFER);

	sui_draw_image_pool = sui_draw_array_allocate(SUI_DRAW_IMAGE_BUFFER, vertex_format_types, vertex_format_size_surface, 2);
	sui_draw_image_section = sui_draw_array_section_allocate(sui_draw_image_pool, SUI_DRAW_IMAGE_BUFFER);

	sui_draw_sprite_pool = sui_draw_array_allocate(SUI_DRAW_SPRITE_BUFFER, vertex_format_types, vertex_format_size, 3);
	sui_draw_sprite_section = sui_draw_array_section_allocate(sui_draw_sprite_pool, SUI_DRAW_SPRITE_BUFFER);


	sui_draw_primitive_image_aa_set(&array[8 * 0], 0, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 1], 1, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 2], 1, 1, 1, 0, 0);

	sui_draw_primitive_image_aa_set(&array[8 * 3], 0, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 4], 1, 1, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 5], 0, 1, 1, 0, 0);

	sui_draw_primitive_image_aa_set(&array[8 * 6], 0, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 7], 1, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 8], 1, 0, 0, 1, -1);

	sui_draw_primitive_image_aa_set(&array[8 * 9], 0, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 10], 1, 0, 0, 1, -1);
	sui_draw_primitive_image_aa_set(&array[8 * 11], 0, 0, 0, -1, -1);

	sui_draw_primitive_image_aa_set(&array[8 * 12], 1, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 13], 1, 0, 0, 1, -1);
	sui_draw_primitive_image_aa_set(&array[8 * 14], 1, 1, 1, 0, 0);

	sui_draw_primitive_image_aa_set(&array[8 * 15], 1, 1, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 16], 1, 0, 0, 1, -1);
	sui_draw_primitive_image_aa_set(&array[8 * 17], 1, 1, 0, 1, 1);

	sui_draw_primitive_image_aa_set(&array[8 * 18], 0, 1, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 19], 1, 1, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 20], 1, 1, 0, 1, 1);

	sui_draw_primitive_image_aa_set(&array[8 * 21], 0, 1, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 22], 1, 1, 0, 1, 1);
	sui_draw_primitive_image_aa_set(&array[8 * 23], 0, 1, 0, -1, 1);

	sui_draw_primitive_image_aa_set(&array[8 * 24], 0, 1, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 25], 0, 1, 0, -1, 1);
	sui_draw_primitive_image_aa_set(&array[8 * 26], 0, 0, 1, 0, 0);

	sui_draw_primitive_image_aa_set(&array[8 * 27], 0, 0, 1, 0, 0);
	sui_draw_primitive_image_aa_set(&array[8 * 28], 0, 1, 0, -1, 1);
	sui_draw_primitive_image_aa_set(&array[8 * 29], 0, 0, 0, -1, -1);
	sui_draw_array_load_vertex(sui_draw_image_pool, sui_draw_image_section, array, 0, 30);

	sui_draw_color_shader = sui_draw_shader_create(buffer, 2048, sui_draw_shader_color_vertex, sui_draw_shader_color_fragment, "color primitive");
	sui_draw_shader_attrib_bind(sui_draw_color_shader, 0, "vertex");
	sui_draw_shader_attrib_bind(sui_draw_color_shader, 1, "color");
/*	printf(buffer); */

	sui_draw_surface_shader = sui_draw_shader_create(buffer, 2048, sui_draw_shader_surface_vertex, sui_draw_shader_surface_fragment, "color primitive");
	sui_draw_shader_attrib_bind(sui_draw_surface_shader, 0, "vertex");
	sui_draw_shader_attrib_bind(sui_draw_surface_shader, 1, "alpha");
	sui_draw_shader_texture_unit_set(sui_draw_surface_shader, "reflection", 0);
	sui_draw_surface_location_color = sui_draw_shader_uniform_location(sui_draw_surface_shader, "color");
	sui_draw_surface_location_pos = sui_draw_shader_uniform_location(sui_draw_surface_shader, "pos");
	sui_draw_surface_location_size = sui_draw_shader_uniform_location(sui_draw_surface_shader, "size");
	sui_draw_surface_location_pixel = sui_draw_shader_uniform_location(sui_draw_surface_shader, "pixel");
/*	printf(buffer); */

	sui_draw_image_shader = sui_draw_shader_create(buffer, 2048, sui_draw_shader_image_vertex, sui_draw_shader_image_fragment, "image primitive");
	sui_draw_shader_attrib_bind(sui_draw_image_shader, 0, "vertex");
	sui_draw_shader_attrib_bind(sui_draw_image_shader, 1, "alpha");
	sui_draw_shader_texture_unit_set(sui_draw_image_shader, "image", 0);
	sui_draw_shader_texture_unit_set(sui_draw_image_shader, "reflection", 1);
	sui_draw_image_location_uv = sui_draw_shader_uniform_location(sui_draw_image_shader, "uv");
	sui_draw_image_location_color = sui_draw_shader_uniform_location(sui_draw_image_shader, "color");
	sui_draw_image_location_pos = sui_draw_shader_uniform_location(sui_draw_image_shader, "pos");
	sui_draw_image_location_size = sui_draw_shader_uniform_location(sui_draw_image_shader, "size");
	sui_draw_image_location_pixel = sui_draw_shader_uniform_location(sui_draw_image_shader, "pixel");

/*	printf(buffer); */

	sui_draw_sprite_shader = sui_draw_shader_create(buffer, 2048, sui_draw_shader_sprite_vertex, sui_draw_shader_sprite_fragment, "sprite primitive");
	sui_draw_shader_attrib_bind(sui_draw_sprite_shader, 0, "vertex");
	sui_draw_shader_attrib_bind(sui_draw_sprite_shader, 1, "color");
	sui_draw_shader_attrib_bind(sui_draw_sprite_shader, 2, "size");
	sui_draw_shader_texture_unit_set(sui_draw_sprite_shader, "image", 0);
	sui_draw_sprite_location_camera_matrix = sui_draw_shader_uniform_location(sui_draw_sprite_shader, "camera_matrix");
}

void sui_draw_primitive_image(float pos_x, float pos_y, float pos_z, float size_x, float size_y, float u_start, float v_start, float u_end, float v_end, uint texture_id, float red, float green, float blue, float alpha)
{
	if(sui_draw_shader_set(sui_draw_image_shader))
	{
		uint size;
		sui_draw_array_active_vertex_atrib(sui_draw_image_pool);	
		sui_draw_shader_active_texture(1);
		glBindTexture(GL_TEXTURE_2D, sui_3d_texture_reflection);
	//	glBindTexture(GL_TEXTURE_2D, texture_id);
		sui_draw_shader_active_texture(0);
		betray_screen_mode_get(&size, NULL, NULL);
		sui_draw_shader_vec2_set(sui_draw_image_location_pixel, 1.0 / (float)size, 1.0 / (float)size);
	}
	sui_draw_shader_vec4_set(sui_draw_image_location_uv, u_start, v_start, u_end - u_start, v_end - v_start);
	sui_draw_shader_vec4_set(sui_draw_image_location_color, red, green, blue, alpha);
	sui_draw_shader_vec3_set(sui_draw_image_location_pos, pos_x, pos_y, pos_z);
	sui_draw_shader_vec2_set(sui_draw_image_location_size, size_x, size_y);
	sui_draw_shader_active_texture(0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	sui_draw_array_section_draw(sui_draw_image_pool, sui_draw_image_section, GL_TRIANGLES, 0, 30);
}


void sui_draw_primitive_surface(float pos_x, float pos_y, float pos_z, float size_x, float size_y, float red, float green, float blue, float alpha)
{
	if(sui_draw_shader_set(sui_draw_surface_shader))
	{
		uint size;
		sui_draw_array_active_vertex_atrib(sui_draw_image_pool);	
		glBindTexture(GL_TEXTURE_2D, sui_3d_texture_reflection);
		betray_screen_mode_get(&size, NULL, NULL);
		sui_draw_shader_vec2_set(sui_draw_surface_location_pixel, 2.0 / (float)size, 2.0 / (float)size);
	}
	sui_draw_shader_vec4_set(sui_draw_surface_location_color, red, green, blue, alpha);
	sui_draw_shader_vec3_set(sui_draw_surface_location_pos, pos_x, pos_y, pos_z);
	sui_draw_shader_vec2_set(sui_draw_surface_location_size, size_x, size_y);
	sui_draw_array_section_draw(sui_draw_image_pool, sui_draw_image_section, GL_TRIANGLES, 0, 30);
}



void sui_draw_primitive_line_flush()
{
	if(sui_draw_shader_set(sui_draw_color_shader))
	{
	//	glEnable(GL_LINE_SMOOTH);
		sui_draw_array_active_vertex_atrib(sui_draw_line_pool);
	}
	sui_draw_array_section_draw(sui_draw_line_pool, sui_draw_line_section, GL_LINES, 0, sui_draw_line_count);
	sui_draw_line_count = 0;	
}

void sui_draw_primitive_line_3d(float start_x, float start_y, float start_z, float end_x, float end_y, float end_z, float red, float green, float blue, float alpha)
{
	float array[14];
	array[0] = start_x;
	array[1] = start_y;
	array[2] = start_z;
	array[3] = array[10] = red;
	array[4] = array[11] = green;
	array[5] = array[12] = blue;
	array[6] = array[13] = alpha;
	array[7] = end_x;
	array[8] = end_y;
	array[9] = end_z;
	sui_draw_array_load_vertex(sui_draw_line_pool, sui_draw_line_section, array, sui_draw_line_count, 2);
	sui_draw_line_count += 2;
	if(sui_draw_line_count == SUI_DRAW_LINE_BUFFER)
		sui_draw_primitive_line_flush();
}

void sui_draw_primitive_line_2d(float start_x, float start_y, float end_x, float end_y, float red, float green, float blue, float alpha)
{
	float array[14];
	array[0] = start_x;
	array[1] = start_y;
	array[2] = 0;
	array[3] = array[10] = red;
	array[4] = array[11] = green;
	array[5] = array[12] = blue;
	array[6] = array[13] = alpha;
	array[7] = end_x;
	array[8] = end_y;
	array[9] = 0;
	sui_draw_array_load_vertex(sui_draw_line_pool, sui_draw_line_section, array, sui_draw_line_count, 2);
	sui_draw_line_count += 2;
	if(sui_draw_line_count == SUI_DRAW_LINE_BUFFER)
		sui_draw_primitive_line_flush();
}


void sui_draw_primitive_line_fade_3d(float start_x, float start_y, float start_z, float end_x, float end_y, float end_z, float start_red, float start_green, float start_blue, float start_alpha, float end_red, float end_green, float end_blue, float end_alpha)
{
	float array[14];
	array[0] = start_x;
	array[1] = start_y;
	array[2] = start_z;
	array[3] = start_red;
	array[4] = start_green;
	array[5] = start_blue;
	array[6] = start_alpha;
	array[7] = end_x;
	array[8] = end_y;
	array[9] = end_z;
	array[10] = end_red;
	array[11] = end_green;
	array[12] = end_blue;
	array[13] = end_alpha;
	sui_draw_array_load_vertex(sui_draw_line_pool, sui_draw_line_section, array, sui_draw_line_count, 2);
	sui_draw_line_count += 2;
	if(sui_draw_line_count == SUI_DRAW_LINE_BUFFER)
		sui_draw_primitive_line_flush();
}

void sui_draw_primitive_line_fade_2d(float start_x, float start_y, float end_x, float end_y, float start_red, float start_green, float start_blue, float start_alpha, float end_red, float end_green, float end_blue, float end_alpha)
{
	float array[14];
	array[0] = start_x;
	array[1] = start_y;
	array[2] = 0;
	array[3] = start_red;
	array[4] = start_green;
	array[5] = start_blue;
	array[6] = start_alpha;
	array[7] = end_x;
	array[8] = end_y;
	array[9] = 0;
	array[10] = end_red;
	array[11] = end_green;
	array[12] = end_blue;
	array[13] = end_alpha;
	sui_draw_array_load_vertex(sui_draw_line_pool, sui_draw_line_section, array, sui_draw_line_count, 2);
	sui_draw_line_count += 2;
	if(sui_draw_line_count == SUI_DRAW_LINE_BUFFER)
		sui_draw_primitive_line_flush();
}

void sui_draw_primitive_sprite_flush()
{
	if(sui_draw_sprite_count == 0)
		return;
//	printf("sui_draw_sprite_count %u\n", sui_draw_sprite_count);
	if(sui_draw_shader_set(sui_draw_sprite_shader))
	{
		sui_draw_array_active_vertex_atrib(sui_draw_sprite_pool);
		glBindTexture(GL_TEXTURE_2D, sui_draw_sprite_texture_id);
	}
	sui_draw_shader_mat4v_set(sui_draw_sprite_location_camera_matrix, sui_draw_sprite_matrix);
	sui_draw_array_section_draw(sui_draw_sprite_pool, sui_draw_sprite_section, GL_QUADS, 0, sui_draw_sprite_count);
	sui_draw_sprite_count = 0;
}

void sui_draw_primitive_sprite(float pos_x, float pos_y, float pos_z, uint texture_id, float size_x, float size_y, float red, float green, float blue, float alpha)
{
	float array[11 * 4];
	if(sui_draw_sprite_texture_id != texture_id)
	{
		sui_draw_primitive_sprite_flush();
		sui_draw_sprite_texture_id = texture_id;
	}
	array[0] = pos_x;
	array[1] = pos_y;
	array[2] = pos_z;
	array[3] = red;
	array[4] = green;
	array[5] = blue;
	array[6] = alpha;
	array[7] = -size_x;
	array[8] = -size_y;
	array[9] = 0;
	array[10] = 0;

	array[11] = pos_x;
	array[12] = pos_y;
	array[13] = pos_z;
	array[14] = red;
	array[15] = green;
	array[16] = blue;
	array[17] = alpha;
	array[18] = size_x;
	array[19] = -size_y;
	array[20] = 1;
	array[21] = 0;
	
	array[22] = pos_x;
	array[23] = pos_y;
	array[24] = pos_z;
	array[25] = red;
	array[26] = green;
	array[27] = blue;
	array[28] = alpha;
	array[29] = size_x;
	array[30] = size_y;
	array[31] = 1;
	array[32] = 1;

	array[33] = pos_x;
	array[34] = pos_y;
	array[35] = pos_z;
	array[36] = red;
	array[37] = green;
	array[38] = blue;
	array[39] = alpha;
	array[40] = -size_x;
	array[41] = size_y;
	array[42] = 0;
	array[43] = 1;

	sui_draw_array_load_vertex(sui_draw_sprite_pool, sui_draw_sprite_section, array, sui_draw_sprite_count, 4);
	sui_draw_sprite_count += 4;
	if(sui_draw_sprite_count == SUI_DRAW_SPRITE_BUFFER)
		sui_draw_primitive_sprite_flush();
}

void sui_draw_primitive_sprite_uv(float pos_x, float pos_y, float pos_z, uint texture_id, float size_x, float size_y, float u_start, float v_start, float u_end, float v_end, float red, float green, float blue, float alpha)
{
}

void sui_draw_primitive_sprite_matrix_set(float *matrix)
{
	uint i;
	for(i = 0; i < 16; i++)
		sui_draw_sprite_matrix[i] = matrix[i];
}
