#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "r_include.h"

GLuint (APIENTRY *r_glGetUniformBlockIndex)(GLuint program, const char *uniformBlockName);
void (APIENTRY *r_glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
void (APIENTRY *r_glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);
void (APIENTRY *r_glGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
GLuint (APIENTRY *r_glGetUniformIndices)(GLuint program, GLsizei uniformCount, const char **uniformNames, GLuint *uniformIndices);
void (APIENTRY *r_glGetActiveUniformsiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
void (APIENTRY *r_glDrawArraysInstanced)( GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
void (APIENTRY *r_glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
void  (APIENTRY *r_gluniform3fvARB)(GLint location, uint count, GLfloat *v);

GLuint (APIENTRY *r_glGetProgramResourceIndex)(GLuint program,GLenum programInterface, const char * name);
void (APIENTRY *r_glShaderStorageBlockBinding)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
void (APIENTRY *r_glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);

void (APIENTRY *r_glGetActiveUniformName)( GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, char *uniformName);




extern GLvoid		(APIENTRY *r_glBindBufferARB)(uint target, uint buffer);
extern GLvoid		(APIENTRY *r_glDeleteBuffersARB)(uint n, const uint *buffers);
extern GLvoid		(APIENTRY *r_glGenBuffersARB)(uint n, uint *buffers);
extern GLvoid		(APIENTRY *r_glBufferDataARB)(uint target, uint size, const void *data, uint usage);
extern GLvoid		(APIENTRY *r_glBufferSubDataARB)(uint target, uint offset, uint size, const void *data);
extern GLvoid		(APIENTRY *r_glGetBufferSubDataARB)(uint target, uint offset, uint size, void *data);


#define GL_DYNAMIC_DRAW_ARB                             0x88E8
#define GL_UNIFORM_BLOCK_DATA_SIZE 0x8A40
#define GL_UNIFORM_OFFSET 0x8A3B
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_SHADER_STORAGE_BLOCK 0x92E6

char *experimental_shader_vertex_test = 
"#version 450\n"
"attribute vec3 vertex;\n"
"struct MatrixBlock\n"
"{\n"
"vec4 pos;\n"
"vec4 c;\n"
"};\n"
"\n"
"layout (std140, binding = 0) buffer InstanceBlock {\n"
"  MatrixBlock my_block[];\n"
"}my_name;\n"
"varying vec4 color;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"void main()\n"
"{\n"
"	vec3 position;\n"
"	position = vertex + my_name.my_block[gl_InstanceID].pos.xyz;\n"
"	color = my_name.my_block[gl_InstanceID].c.rgba;\n"
"	gl_Position = ModelViewProjectionMatrix * vec4(position, 1.0);\n"
"}\n";

char *experimental_shader_fragment_test = 
"#version 450\n"
"varying vec4 color;\n"
"void main()\n"
"{\n"
"	gl_FragColor = color;\n"
"}\n";

	
char *experimental_shader_vertex_test2 = 
"#version 450 \n"
"attribute vec3 vertex;"

"struct MatrixBlock"
"{"
"vec4 pos;"
"vec4 c;"
"};"
""
"layout (std140, binding = 0) uniform InstanceBlock {"
"  MatrixBlock my_block[32];"
"}my_name;"
"varying vec4 color;"
"uniform mat4 ModelViewProjectionMatrix;"
"void main()"
"{"
"	vec3 position;"
"	position = vertex + my_name.my_block[gl_InstanceID].pos.xyz;"
"	color = my_name.my_block[gl_InstanceID].c.rgba;"
"	gl_Position = ModelViewProjectionMatrix * vec4(position, 1.0);"
"}";

char *experimental_shader_fragment_test2 = 
"#version 400 \n"
"varying vec4 color;"
"void main()"
"{"
"	gl_FragColor = color;"
"}";



char *experimental_shader_vertex_test3 = 
"attribute vec3 vertex;\n"
"varying vec4 col;\n"
"uniform InstanceBlock {\n"
"uniform vec4 hello;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"};\n"
"void main()\n"
"{\n"
"	col = vec4(vertex, 1) - hello;\n"
"	gl_Position = ModelViewProjectionMatrix * vec4(vertex - vec3(0, 0, 0), 1.0);\n"
"}\n";

char *experimental_shader_fragment_test3 = 
"uniform sampler2D tex;\n"
"varying vec4 col;\n"
"void main()\n"
"{\n"
//"	gl_FragColor = texture2D(tex, col.rg * vec2(100.0));\n"
"	gl_FragColor = texture2D(tex, col.rg) + col.rgba;\n"
"}\n";



static void *experimental_cube_pool = NULL, *experimental_vertex_section = NULL, *experimental_reference_section = NULL;
static void *experimental_ball = NULL;
static RShader *experimental_shader;
static RShader *experimental_shader2;
uint uniform_buffer_object = 0;
uint storage_buffer_object = 0;
uint experimental_texture_id = 0;
extern uint r_shader_experimental_program_get(RShader *shader);



extern GLvoid		(APIENTRY *r_glGetProgramivARB)(GLuint shader,  GLenum pname,  GLint *params);
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_BLOCKS 0x8A36

void r_experimental_print_uniforms(uint program)
{
	char buffer[64];
	uint i, length, count = 0;
	r_glGetProgramivARB(program, GL_ACTIVE_UNIFORMS, &count);
	for(i = 0; i < count; i++)
	{
		r_glGetActiveUniformName(program, i, 64, &length, buffer);
		printf("name[%u] -%s-\n", i, buffer);
	}
}

void r_experimental_init()
{
	char buffer[2048];
	SUIFormats vertex_format_types[1] = {SUI_FLOAT};
	uint i, j, index, size, vertex_format_size[1] = {3};
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
	uint8 *array;

	uint ref_data[24] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 2, 1, 3, 4, 6, 5, 7, 0, 4, 1, 5, 2, 6, 3, 7};

	if(r_extension_test("GL_ARB_uniform_buffer_object"))
	{
		r_glGetUniformBlockIndex = r_extension_get_address("glGetUniformBlockIndex");
		r_glGetUniformIndices = r_extension_get_address("glGetUniformIndices");
		r_glGetActiveUniformsiv = r_extension_get_address("glGetActiveUniformsiv");
		r_glDrawArraysInstanced = r_extension_get_address("glDrawArraysInstanced");
		r_glDrawElementsInstanced = r_extension_get_address("glDrawElementsInstanced");

		r_gluniform3fvARB = r_extension_get_address("glUniform3fv");

		r_glGetProgramResourceIndex = r_extension_get_address("glGetProgramResourceIndex");
		r_glShaderStorageBlockBinding = r_extension_get_address("glShaderStorageBlockBinding");
		r_glGetActiveUniformName = r_extension_get_address("glGetActiveUniformName");
	}
	experimental_ball = r_array_allocate(8, vertex_format_types, vertex_format_size, 1, 24000);
	{
		float *data;
		data = malloc(24000 * 3 * sizeof(float));
		for(i = 0; i < 24000 * 3; i++)
			data[i] = f_randnf(i) * 10.0;
		r_array_load_vertex(experimental_ball, NULL, data, 0, 24000);
		free(data);
	}
	/* create apool ov vertex memory that vcann fit 8 vertices and 24 reference values */
	experimental_cube_pool = r_array_allocate(8, vertex_format_types, vertex_format_size, 1, 24);
	/* Allocat a section of the pool to use for vertex data */
	experimental_vertex_section = r_array_section_allocate_vertex(experimental_cube_pool, 8);
	/* Load our vertex data in to the section */
	r_array_load_vertex(experimental_cube_pool, experimental_vertex_section, vertex_data, 0, 8);
	/* Allocat a section of the pool to use for reference data */
	experimental_reference_section = r_array_section_allocate_reference(experimental_cube_pool, 24);
	/* Load our reference data in to the section */
	r_array_load_reference(experimental_cube_pool, experimental_reference_section, experimental_vertex_section, ref_data, 24);

	/* Compile a shader form source code*/

	experimental_texture_id = r_texture_allocate(R_IF_RGB_UINT8, 256, 256, 1, FALSE, TRUE, NULL);
//	exit(0);


/*	{
		char *names[] = {"my_name.pos", "pos", "ModelViewProjectionMatrix", "my_name", "InstanceBlock.c"};
		uint locations[5], offset[5];
		float *pos;
		r_glGetUniformIndices(r_shader_experimental_program_get(experimental_shader2), 5, names, locations);
		r_glGetActiveUniformsiv(r_shader_experimental_program_get(experimental_shader2), 5, locations, GL_UNIFORM_OFFSET, offset);

		for(i = 0; i < 1; i++)
		{
			pos = (float *)(&array[i * size + offset[0]]);
			pos[0] = (float)i / 0.1; 
			pos = (float *)(&array[i * size + offset[1]]);
			for(j  = 0; j < 16; j++)
			{
				pos[j * 4 + 0] = (float)0.0 + (float)j * 0.01; 
				pos[j * 4 + 1] = (float)0.0 + (float)j * 0.01; 
				pos[j * 4 + 2] = (float)0.0; 
				pos[j * 4 + 3] = (float)0.0; 
			}
		}
	}*/

	{
		char *shaders[2];
		uint *stages[2] = {GL_VERTEX_SHADER_ARB, GL_FRAGMENT_SHADER_ARB};
		shaders[0] = experimental_shader_vertex_test3;
		shaders[1] = experimental_shader_fragment_test3;
		experimental_shader = r_shader_create(buffer, 2048, shaders, stages, 2, "Relinquish experimental shader", NULL);
	}

	if(experimental_shader == NULL)
	{
		printf(buffer);
		exit(0);
	}
	return;
	GLuint ssbo = 0;
	r_glGenBuffersARB(1, &storage_buffer_object);
	r_glBindBufferARB(GL_SHADER_STORAGE_BUFFER, storage_buffer_object);
	{
		float pos[8 * 80];
		for(i = 0; i < 80; i++)
		{
			pos[i * 8 + 0] = f_randnf(i);
			pos[i * 8 + 1] = f_randnf(i + 3);
			pos[i * 8 + 2] = f_randnf(i + 8);
			pos[i * 8 + 3] = 0; 
			pos[i * 8 + 4] = f_randnf(i + 2);
			pos[i * 8 + 5] = f_randnf(i + 4);
			pos[i * 8 + 6] = f_randnf(i + 7);
			pos[i * 8 + 7] = 1; 
		}
		r_glBufferDataARB(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 80 * 8, pos, GL_DYNAMIC_DRAW_ARB);
	}
	r_glBindBufferARB(GL_SHADER_STORAGE_BUFFER, 0);
	/*
	index = r_glGetProgramResourceIndex(r_shader_experimental_program_get(experimental_shader), GL_SHADER_STORAGE_BLOCK, "MatrixBlock");
	r_glShaderStorageBlockBinding(r_shader_experimental_program_get(experimental_shader), index, 0);
	i = 0;*/

}

void r_experimental_draw(BInputState *input)
{
	uint location[4], place, i, size;
	float pos[3 * 16 * 12];
	uint8 *uniforms;
//	r_shader_set(experimental_shader2);		

/*	location[0] = r_shader_uniform_location(experimental_shader2, "MatrixBlock");
	location[1] = r_shader_uniform_location(experimental_shader2, "MatrixBlock.pos");
	location[2] = r_shader_uniform_location(experimental_shader2, "pos");*/

/*	r_shader_set(experimental_shader2);	
	location[3] = r_glGetUniformBlockIndex(r_shader_experimental_program_get(experimental_shader2), "InstanceBlock");
	r_glBindBufferARB(GL_UNIFORM_BUFFER, uniform_buffer_object);
	r_glBindBufferBase(GL_UNIFORM_BUFFER, location[3], uniform_buffer_object);
	r_array_section_draw(experimental_cube_pool, experimental_vertex_section, GL_LINES, 0, 8);
	r_glDrawArraysInstanced(GL_LINES, 0, 8, 16);*/

	/*
	r_shader_set(experimental_shader);
	r_glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffer_object);
	r_array_section_draw(experimental_cube_pool, experimental_vertex_section, GL_LINES, 0, 8);
	r_glDrawArraysInstanced(GL_LINES, 0, 8, 80);
	*/
	{
		uint64 data[4];
		data[0] = r_shader_uniform_texture_pointer_get(experimental_texture_id);
		r_shader_uniform_data_set(experimental_shader, data, 1); 
	}


	size = r_shader_uniform_block_size(experimental_shader, 0);
	uniforms = malloc(size * 160);
	for(i = 0; i < 160 * size; i++)
		uniforms[i] = 0;
	for(i = 0; i < 160; i++)
	{
		r_shader_uniform_matrix_set(experimental_shader, &uniforms[size * i], 0, NULL);
		r_matrix_translate(NULL, 0.01, 0.02, 0.03); 
	}
	r_shader_set(experimental_shader);
/*	{
		void *sections[160];
		for(i = 0; i < 160; i++)
			sections[i] = experimental_vertex_section;
		r_array_sections_draw(experimental_cube_pool, sections, GL_LINES, uniforms, 160);
	}*/

	{
		void *sections[160];
		for(i = 0; i < 160; i++)
			sections[i] = experimental_reference_section;
		r_array_references_draw(experimental_cube_pool, sections, GL_LINES, uniforms, NULL, 160);
	}
}



char *experimental_shader_vertex_draw_points = 
"attribute vec4 vertex;\n"
"uniform block{\n"
"	vec4 point;\n"
"	vec4 vector;\n"
"};\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"varying vec4 color;\n"
"void main()\n"
"{\n"
"	gl_Position = ModelViewProjectionMatrix * vec4(vertex.xyz * vec3(0.04) + point.xyz, 1.0);\n"
"	color = vec4(point.xyz, 1) * vec4(1.0 / length(point.xyz));"
"}\n";

char *experimental_shader_fragment_draw_points = 
"varying vec4 color;\n"
"void main()\n"
"{\n"
"	gl_FragColor = color;\n"
"}\n";

char *experimental_shader_vertex_normalize_points = 
"attribute vec4 vertex;\n"
"attribute vec4 dir;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"varying vec4 point;\n"
"varying vec4 vector;\n"
"void main()\n"
"{\n"
"	point = vertex + vec4(0.001) * dir;\n"
"	vector = dir + vec4(0, -0.01, 0, 0);\n"
"	gl_Position = vec4(vertex.xyz, 1.0);\n"
"}\n";

char *experimental_shader_fragment_normalize_points = 
"varying vec4 point;\n"
"varying vec4 vector;\n"
"void main()\n"
"{\n"
"	gl_FragColor = vec4(1.0);\n"
"}\n";


#define R_EXPERIMENTAL_TRANSFORM_DOT_COUNT 50000
#define R_EXPERIMENTAL_RESET_COUNT 50
#define R_EXPERIMENTAL_FORMAT_SIZE 8
void r_array_vertex_render(void *in_pool, void *out_pool);

void r_experimental_transform(BInputState *input)
{
	static RShader *draw, *transform;
	static void *array_in = NULL, *array_out, *tmp, **sections;
	static uint pos = 0, i, seed = 0;
	if(array_in == NULL)
	{
		SUIFormats vertex_format_types[2] = {SUI_FLOAT, SUI_FLOAT};
		uint vertex_format_size[2] = {4, 4};
		char buffer[2048];
		char *shaders[2];
		uint *stages[2] = {GL_VERTEX_SHADER_ARB, GL_FRAGMENT_SHADER_ARB};
		float *array;
		
		sections = malloc((sizeof *sections) * R_EXPERIMENTAL_TRANSFORM_DOT_COUNT);
		for(i = 0; i < R_EXPERIMENTAL_TRANSFORM_DOT_COUNT; i++)
			sections[i] = experimental_vertex_section;
		array = malloc((sizeof *array) * R_EXPERIMENTAL_FORMAT_SIZE * R_EXPERIMENTAL_TRANSFORM_DOT_COUNT);

		shaders[0] = experimental_shader_vertex_draw_points;
		shaders[1] = experimental_shader_fragment_draw_points;
		draw = r_shader_create(buffer, 2048, shaders, stages, 2, "Relinquish experimental shader", NULL);
		
		shaders[0] = experimental_shader_vertex_normalize_points;
		shaders[1] = experimental_shader_fragment_normalize_points;
		transform = r_shader_create(buffer, 2048, shaders, stages, 2, "Relinquish experimental shader", NULL);
		if(transform == NULL)
			transform = NULL;
		array = malloc((sizeof *array) * R_EXPERIMENTAL_FORMAT_SIZE * R_EXPERIMENTAL_TRANSFORM_DOT_COUNT);
		array_in = r_array_allocate(R_EXPERIMENTAL_TRANSFORM_DOT_COUNT, &vertex_format_types, &vertex_format_size, 2, 0);
		array_out = r_array_allocate(R_EXPERIMENTAL_TRANSFORM_DOT_COUNT, &vertex_format_types, &vertex_format_size, 2, 0);
		for(i = 0; i < R_EXPERIMENTAL_TRANSFORM_DOT_COUNT; i++)
		{
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 0] = f_randnf(seed++);
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 1] = f_randnf(seed++);
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 2] = 0;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 3] = 0;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 4] = f_randnf(seed++);
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 5] = f_randnf(seed++);
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 6] = f_randnf(seed++);
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 7] = f_randnf(seed++);
		}
		r_array_load_vertex(array_in, NULL, array, 0, R_EXPERIMENTAL_TRANSFORM_DOT_COUNT);
		free(array);
	}
	
	{
		float array[R_EXPERIMENTAL_RESET_COUNT * R_EXPERIMENTAL_FORMAT_SIZE];
		for(i = 0; i < R_EXPERIMENTAL_RESET_COUNT; i++)
		{
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 0] = 0;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 1] = 0;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 2] = 0;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 3] = 0;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 4] = f_randnf(seed++) + 0.7;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 5] = f_randnf(seed++) + 2.5;
			array[i * R_EXPERIMENTAL_FORMAT_SIZE + 6] = f_randnf(seed++);
		//	array[i * R_EXPERIMENTAL_FORMAT_SIZE + 7] = f_randnf(seed++);
		}	
		r_array_load_vertex(array_in, NULL, array, pos, R_EXPERIMENTAL_RESET_COUNT);
		pos = (pos + R_EXPERIMENTAL_RESET_COUNT) % R_EXPERIMENTAL_TRANSFORM_DOT_COUNT;
	}

	r_shader_set(transform);
	r_array_vertex_render(array_in, array_out);

	r_shader_set(draw);
//	r_array_sections_draw(experimental_cube_pool, NULL, R_PRIMITIVE_LINES, NULL, array_in, R_EXPERIMENTAL_TRANSFORM_DOT_COUNT);

	r_array_draw(experimental_cube_pool, experimental_reference_section, R_PRIMITIVE_LINES, 0, 24, NULL, array_in, R_EXPERIMENTAL_TRANSFORM_DOT_COUNT);


//	r_array_section_draw(array_out, NULL, GL_POINTS, 0, R_EXPERIMENTAL_TRANSFORM_DOT_COUNT);

	tmp = array_in;
	array_in = array_out;
	array_out = array_in;
}