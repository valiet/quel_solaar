#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "betray.h"
#include "relinquish.h" /*little opengl wrapper i use, in the case only to draw lines*/

uint b_test_sound = 0;

void b_test_draw_box(float red, float green, float blue)
{
	float array[2 * 3 * 4 * 3] = {
		0.1, 0.1, 0.1, -0.1, 0.1, 0.1,
		0.1, -0.1, 0.1, -0.1, -0.1, 0.1,
		0.1, -0.1, -0.1, -0.1, -0.1, -0.1,
		0.1, 0.1, -0.1, -0.1, 0.1, -0.1,
		0.1, 0.1, 0.1, 0.1, -0.1, 0.1,
		0.1, 0.1, -0.1, 0.1, -0.1, -0.1,
		-0.1, 0.1, -0.1, -0.1, -0.1, -0.1,
		-0.1, 0.1, 0.1, -0.1, -0.1, 0.1,
		0.1, 0.1, 0.1, 0.1, 0.1, -0.1, 
		-0.1, 0.1, 0.1, -0.1, 0.1, -0.1,
		-0.1, -0.1, 0.1, -0.1, -0.1, -0.1,
		0.1, -0.1, 0.1, 0.1, -0.1, -0.1};
	uint i;
	for(i = 0; i < 2 * 3 * 4 * 3; i += 6)
		r_primitive_line_3d(array[i + 0], array[i + 1], array[i + 2], array[i + 3], array[i + 4], array[i + 5], red, green, blue, 1.0);
	r_primitive_line_flush();
}

/*
Set up a function that Betray can call if the context is lost
*/

void r_test_context_update(void)
{
//	glMatrixMode(GL_MODELVIEW);	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
}

char *r_shader_vertex_test = 
"attribute vec3 vertex;\n"
"uniform float time;\n"
"uniform vec3 pos;\n"
"varying vec4 color;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"void main()"
"{"
"	vec3 position, wave;\n"
//"	color = vec4(pos * vec3(0.1) + vec3(0.1), 1);\n"
"	position = vertex + pos;\n"
"	wave = vec3(sin(time + position.y * 0.5), sin(time + position.z * 0.5), sin(time + position.x * 0.5));\n"
"	color = vec4((pos) * vec3(0.05) + vec3(0.1) + wave * vec3(0.3), 1);\n"
"	position += wave * vec3(0.3);\n"
"	gl_Position = ModelViewProjectionMatrix * vec4(position, 1.0);\n"
"}\n";

char *r_shader_fragment_test = 
"varying vec4 color;\n"
"void main()\n"
"{\n"
"	gl_FragColor = color;\n"
"}\n";

	
void r_experimental_draw(BInputState *input);
void r_experimental_transform(BInputState *input);
float my_test_value = 0;

#define RELINQUISH_TEST_BOX_COUNT 20

void r_test_input_handler(BInputState *input, void *user_pointer)
{
	static boolean init = FALSE;
	static float time = 0, pos[3] = {0, 0, -1}, vec[3] = {0, 0, 0};
	static uint texture_id = -1;
	static void *cube_pool = NULL, *vertex_section = NULL, *reference_section = NULL, **instance_sections;
	static RShader *shader;
	static uint8 *uniform_buffer;
	uint i, j, k;
	float f, aspect,  matrix[16], x, y, z;

	/* initialize all the data we will need. */

	if(!init)
	{
		char buffer[2048];
		SUIFormats vertex_format_types[1] = {SUI_FLOAT};
		uint i, size, pos_location, time_location, vertex_format_size[1] = {3};
		float vertex_data[3 * 8] = {
			0.1, 0.1, 0.1, 
			-0.1, 0.1, 0.1,
			0.1, -0.1, 0.1, 
			-0.1, -0.1, 0.1,
			0.1, 0.1, -0.1, 
			-0.1, 0.1, -0.1,
			0.1, -0.1, -0.1, 
			-0.1, -0.1, -0.1};
		float vertex_data_old[3 * 8] = {
			0.3, 0.3, 0.3, 
			-0.3, 0.3, 0.3,
			0.3, -0.3, 0.3, 
			-0.3, -0.3, 0.3,
			0.3, 0.3, -0.3, 
			-0.3, 0.3, -0.3,
			0.3, -0.3, -0.3, 
			-0.3, -0.3, -0.3};
		float *data;
		uint ref_data[24] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 2, 1, 3, 4, 6, 5, 7, 0, 4, 1, 5, 2, 6, 3, 7};
		/* create apool ov vertex memory that vcann fit 8 vertices and 24 reference values */
		cube_pool = r_array_allocate(8, vertex_format_types, vertex_format_size, 1, 24);
		/* Allocat a section of the pool to use for vertex data */
		vertex_section = r_array_section_allocate_vertex(cube_pool, 8);
		/* Load our vertex data in to the section */
		r_array_load_vertex(cube_pool, vertex_section, vertex_data, 0, 8);
		/* Allocat a section of the pool to use for reference data */
		reference_section = r_array_section_allocate_reference(cube_pool, 24);
		/* Load our reference data in to the section */
		r_array_load_reference(cube_pool, reference_section, vertex_section, ref_data, 24);		
		/* Allocate a texture */
		texture_id = r_texture_allocate(R_IF_RGB_UINT8, 256, 256, 1, FALSE, TRUE, NULL);

		/* Compile a shader from source code*/
		shader = r_shader_create_simple(buffer, 2048, r_shader_vertex_test, r_shader_fragment_test, "Relinquish test shader");



		/* If the shader compilation failed print out error messages. */
		if(shader == NULL)
		{
			printf("Relinquish shader compile failed;\n");
			printf(buffer);
			exit(0);
		}

		size = r_shader_uniform_block_size(shader, 0);
		uniform_buffer = malloc(size * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT);
		
		time_location = r_shader_uniform_block_location(shader, "time");
		pos_location = r_shader_uniform_block_location(shader, "pos");
		i = 0;
		for(x = -RELINQUISH_TEST_BOX_COUNT / 2; x < RELINQUISH_TEST_BOX_COUNT / 2 - 0.5; x++)
		{
			for(y = -RELINQUISH_TEST_BOX_COUNT / 2; y < RELINQUISH_TEST_BOX_COUNT / 2 - 0.5; y++)
			{
				for(z = -RELINQUISH_TEST_BOX_COUNT / 2; z < RELINQUISH_TEST_BOX_COUNT / 2 - 0.5; z++)
				{
					data = (float *)&uniform_buffer[i * size + time_location];
					data[0] = 0;
					data = (float *)&uniform_buffer[i * size + pos_location];
					data[0] = x;
					data[1] = y;
					data[2] = z;
					i++;
				}
			}
		}
		instance_sections = malloc((sizeof *instance_sections) * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT);
		for(i = 0; i < RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT; i++)
			instance_sections[i] = reference_section;
		init = TRUE;
	}


	if(input->mode == BAM_DRAW)
	{
		static uint seed = 0;
		uint screen_x, screen_y, location, size;
		float aspect, view[3] = {0, 0, 1}, matrix[16];
		betray_view_vantage(view);
		aspect = betray_screen_mode_get(&screen_x, &screen_y, NULL);

		/* Define the area opf the screen we want to draw */
		r_viewport(0, 0, screen_x, screen_y);
		/* Clear currently bound framebuffer color, stencil and depth to black */
		r_framebuffer_clear(0, 0.2, 0.4, 0, TRUE, TRUE);
		seed++;
		/* Set the defailt transform matrix */
		r_matrix_set(NULL);
		/* Clear the matrix*/
		r_matrix_identity(NULL);
		/* Define the frustum */
		r_matrix_frustum(NULL, -0.01 - view[0] * 0.01, 0.01 - view[0] * 0.01, -0.01 * aspect - view[1] * 0.01, 0.01 * aspect - view[1] * 0.01, 0.01 * view[2], 100.0); /* set frustum */

		/* Add direction matrix so that its possible to implement different view direction*/
		betray_view_direction(matrix); 
		/* add a matrix to the transform stack */
		r_matrix_matrix_mult(NULL, matrix);
		/* translate the scene */
		r_matrix_translate(NULL, -view[0], -view[1], -view[2]); /* move the camera if betray wants us to. */

		/* Push the stak one level */
		r_matrix_push(NULL);
		
		/* Rotate the scene */
		r_matrix_rotate(NULL, input->minute_time * 360 * 2, 1, 0.7, 0.2);
		r_experimental_transform(input);
//		r_experimental_draw(input);
	//	return;
		/* Set the shader we want to draw with */
		r_shader_set(shader);
		/* Find the location of a uniform in the shader */
		location = r_shader_uniform_location(shader, "time");

		
		
		size = r_shader_uniform_block_size(shader, 0);
		for(i = 0; i < RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT; i++)
		{	
			*((float *)&uniform_buffer[size * i + location]) = input->minute_time * 60.0;
			r_shader_uniform_matrix_set(shader, &uniform_buffer[size * i], 0, NULL);
		}
		r_array_references_draw(cube_pool, instance_sections, R_PRIMITIVE_LINES, uniform_buffer, NULL, RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT);
//		r_array_sections_draw(cube_pool, NULL, R_PRIMITIVE_LINES, uniform_buffer, NULL, RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT * RELINQUISH_TEST_BOX_COUNT);


		/* set the value of the location*/
		r_shader_float_set(shader, location, input->minute_time * 60.0);
		
		/* Find the location of a uniform in the shader */
		location = r_shader_uniform_location(shader, "pos");

		for(x = -10; x < 10.5; x++)
		{
			for(y = -10; y < 10.5; y++)
			{
				for(z = -10; z < 10.5; z++)
				{
					/* set the value of the location*/
		//			r_shader_vec3_set(location, x, y, z);
					/* draw our reference_section */
		//			r_array_reference_draw(cube_pool, reference_section, R_PRIMITIVE_LINES, 0, 24);	
				}
			}
		}
		/* Pop the stak one level */
		r_matrix_pop(NULL);

		/* fer debugghing you can use convinient line functions to draw lines */
		for(i = 0; i < input->pointer_count; i++)
		{
			r_primitive_line_3d(input->pointers[i].pointer_x - 0.1, input->pointers[i].pointer_y, 0.0, input->pointers[i].pointer_x + 0.1, input->pointers[i].pointer_y, 0.0, 1, 0, 0, 1.0);
			r_primitive_line_3d(input->pointers[i].pointer_x, input->pointers[i].pointer_y - 0.1, 0.0, input->pointers[i].pointer_x, input->pointers[i].pointer_y + 0.1, 0.0, 0, 1, 0, 1.0);
			r_primitive_line_3d(input->pointers[i].pointer_x, input->pointers[i].pointer_y, 0.1, input->pointers[i].pointer_x, input->pointers[i].pointer_y, -0.1, 0, 0, 1, 1.0);
		}	
		r_primitive_line_flush(); /* after drawing lines you must always flush the lines!  */


		/* draw a surface with a unigform color */
		
		r_primitive_surface(0.5, -0.1, 0, 0.2, 0.2, 1, 0, 0, 0);

		/* draw a surface with a texture */

		r_primitive_image(-0.7, -0.1, 0, 0.2, 0.2, 0, 0, 1, 1, texture_id, 1, 1, 1, 1);
	}
	
}

void r_experimental_init();

int main(int argc, char **argv)
{
	float f;
	int16 *buffer;
	uint i;
	char *clip_buffer;
	/* Check if betray implementation supports OpenGL or OpenGLES, Relinqish runs on both, so any is fine*/
	if(!betray_support_context(B_CT_OPENGL_OR_ES))
	{
		printf("Relequish Requires B_CT_OPENGL or B_CT_OPENGLES2 to be available, Program exit.\n");
		exit(0);
	}
	/* initialize betray by opening a screen */
	betray_init(B_CT_OPENGL_OR_ES, argc, argv, 800, 600, 4, FALSE, "Relinquish Test Application");

	/* initalize Relinquish*/

	if(!r_init((void*(*)(const char*))betray_gl_proc_address_get()))
	{
		printf("Relequish Requires darwdare/driver features that are not available, Program exit.\n");
		exit(0);
	}
	r_experimental_init();
	betray_gl_context_update_func_set(r_test_context_update);


	betray_action_func_set(r_test_input_handler, NULL);
	betray_launch_main_loop();
	return TRUE;
}