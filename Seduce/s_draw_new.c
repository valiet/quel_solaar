#include "betray.h"
#include "seduce.h"
#include <stdio.h>

void *(*sui_gl_GetProcAddress)(const char* proc) = NULL;

void sui_draw_extension_init()
{
	const char *extension;
	sui_gl_GetProcAddress = betray_gl_proc_address_get();
	extension = glGetString(GL_EXTENSIONS);
}

void *sui_draw_extension_get_address(const char* proc)
{
	if(sui_gl_GetProcAddress == NULL)
		return NULL;
	else
		return sui_gl_GetProcAddress(proc);
}


boolean sui_draw_extension_test(const char *string)
{
	const char *extension, *a;
	uint i;
	extension = glGetString(GL_EXTENSIONS);
	if(extension != NULL)
	{
		for(a = extension; a[0] != 0; a++)
		{
			for(i = 0; string[i] != 0 && a[i] != 0 && string[i] == a[i]; i++);
			if(string[i] == 0)
				return TRUE;
		}
	}
	return FALSE;
}



#define GL_VERTEX_SHADER_ARB                        0x8B31
#define GL_FRAGMENT_SHADER_ARB                      0x8B30

typedef uint GLhandleARB;

GLhandleARB (APIENTRY *sui_glCreateShaderObjectARB)(GLenum  shaderType);
GLvoid      (APIENTRY *sui_glDeleteObjectARB)(GLhandleARB obj);
GLhandleARB (APIENTRY *sui_glCreateProgramObjectARB)(GLvoid);
GLvoid      (APIENTRY *sui_glAttachObjectARB)(GLhandleARB containerObj, GLhandleARB obj);
GLvoid      (APIENTRY *sui_glShaderSourceARB)(GLhandleARB shaderObj, GLsizei count, const char **string, const GLint *length);
GLvoid      (APIENTRY *sui_glCompileShaderARB)(GLhandleARB shaderObj);
GLvoid      (APIENTRY *sui_glLinkProgramARB)(GLhandleARB programObj);
GLvoid      (APIENTRY *sui_glUseProgramObjectARB)(GLhandleARB programObj);
GLint       (APIENTRY *sui_glGetUniformLocationARB)(GLhandleARB programObj, const char *name);
GLvoid      (APIENTRY *sui_glUniform4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLvoid      (APIENTRY *sui_glUniform3fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLvoid      (APIENTRY *sui_glUniform2fARB)(GLint location, GLfloat v0, GLfloat v1);
GLvoid      (APIENTRY *sui_glUniform1fARB)(GLint location, GLfloat v0);
GLvoid      (APIENTRY *sui_glUniform1iARB)(GLint location, GLint i);
GLvoid		(APIENTRY *sui_glUniformMatrix4fvARB)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
GLvoid		(APIENTRY *sui_glUniformMatrix3fvARB)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);
GLvoid		(APIENTRY *sui_glUniformMatrix2fvARB)(GLint location, GLsizei count, GLboolean transpose, GLfloat *value);

GLint       (APIENTRY *sui_glGetAttribLocationARB)(GLhandleARB programObj, const char *name);
GLvoid		(APIENTRY *sui_glVertexAttrib4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
GLvoid		(APIENTRY *sui_glActiveTextureARB)(GLenum texture);

GLvoid		(APIENTRY *sui_glBindAttribLocationARB)(GLhandleARB programObj, GLuint index, const char *name);
GLvoid		(APIENTRY *sui_glVertexAttribPointerARB)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
GLvoid		(APIENTRY *sui_glEnableVertexAttribArrayARB)(GLuint index);
GLvoid		(APIENTRY *sui_glDisableVertexAttribArrayARB)(GLuint index);

GLvoid		(APIENTRY *sui_glGetInfoLogARB)(GLhandleARB object, GLsizei maxLenght, GLsizei *length, char *infoLog);

boolean sui_programmable_shaders_supported;

void sui_draw_init(void)
{
	if(sui_programmable_shaders_supported = sui_draw_extension_test("GL_ARB_shading_language_100"))
	{
		sui_glCreateShaderObjectARB =		sui_draw_extension_get_address("glCreateShaderObjectARB");
		sui_glDeleteObjectARB =				sui_draw_extension_get_address("glDeleteObjectARB");
		sui_glCreateProgramObjectARB =		sui_draw_extension_get_address("glCreateProgramObjectARB");
		sui_glAttachObjectARB =				sui_draw_extension_get_address("glAttachObjectARB");
		sui_glShaderSourceARB =				sui_draw_extension_get_address("glShaderSourceARB");
		sui_glCompileShaderARB =			sui_draw_extension_get_address("glCompileShaderARB");
		sui_glLinkProgramARB =				sui_draw_extension_get_address("glLinkProgramARB");
		sui_glUseProgramObjectARB =			sui_draw_extension_get_address("glUseProgramObjectARB");
		sui_glGetUniformLocationARB =		sui_draw_extension_get_address("glGetUniformLocationARB");
		sui_glUniform4fARB =				sui_draw_extension_get_address("glUniform4fARB");
		sui_glUniform3fARB =				sui_draw_extension_get_address("glUniform3fARB");
		sui_glUniform2fARB =				sui_draw_extension_get_address("glUniform2fARB");
		sui_glUniform1fARB =				sui_draw_extension_get_address("glUniform1fARB");
		sui_glUniform1iARB =				sui_draw_extension_get_address("glUniform1iARB");
		sui_glUniformMatrix4fvARB =			sui_draw_extension_get_address("glUniformMatrix4fvARB");
		sui_glUniformMatrix3fvARB =			sui_draw_extension_get_address("glUniformMatrix3fvARB");
		sui_glUniformMatrix2fvARB =			sui_draw_extension_get_address("glUniformMatrix2fvARB");
		sui_glBindAttribLocationARB =		sui_draw_extension_get_address("glBindAttribLocationARB");
		sui_glGetAttribLocationARB =		sui_draw_extension_get_address("glGetAttribLocationARB");
		sui_glVertexAttrib4fARB =			sui_draw_extension_get_address("glVertexAttrib4fARB");
		sui_glVertexAttribPointerARB =		sui_draw_extension_get_address("glVertexAttribPointerARB");
		sui_glEnableVertexAttribArrayARB =	sui_draw_extension_get_address("glEnableVertexAttribArrayARB");
		sui_glDisableVertexAttribArrayARB =	sui_draw_extension_get_address("glDisableVertexAttribArrayARB");
		sui_glActiveTextureARB =			sui_draw_extension_get_address("glActiveTextureARB");
		sui_glGetInfoLogARB =				sui_draw_extension_get_address("glGetInfoLogARB");
	}

}

uint sui_draw_shader_create(char *debug_output, uint output_size, char *vertex, char *fragment, char *name)
{
	uint vertex_obj, fragment_obj, prog_obj;
	uint i, size = 0;
	char buf[20000];
//	debug_output[0] = 0;
	prog_obj = sui_glCreateProgramObjectARB();
	vertex_obj = sui_glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragment_obj = sui_glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	sui_glAttachObjectARB(prog_obj, vertex_obj);
	sui_glAttachObjectARB(prog_obj, fragment_obj);
	sui_glShaderSourceARB(vertex_obj, 1, (const char **)&vertex, NULL);
	sui_glCompileShaderARB(vertex_obj);
	sui_glGetInfoLogARB(vertex_obj, /*output_size - size*/20000, &size, buf);
	printf(buf);
	output_size -= size;
	sui_glShaderSourceARB(fragment_obj, 1, (const char **)&fragment, NULL);
	sui_glCompileShaderARB(fragment_obj);
	sui_glGetInfoLogARB(fragment_obj, /*output_size - size*/20000, &size, buf);
	printf(buf);
	output_size -= size;
	sui_glLinkProgramARB(prog_obj);
	sui_glGetInfoLogARB(prog_obj, /*output_size - size*/20000, &size, buf);
	printf(buf);
	return prog_obj;
}


uint sui_draw_shader_create_from_file(char *debug_output, uint output_size, char *vertex_file, char *fragment_file, char *name)
{
	uint i, size, program;
	char *vertex, *fragment;
	FILE *f;
	f = fopen(vertex_file, "r");
	if(f == NULL)
		return 0;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	vertex = malloc(size + 1);
	size = fread(vertex, 1, size, f);
	fclose(f);
	vertex[size] = 0;
	f = fopen(fragment_file, "r");
	if(f == NULL)
	{
		free(vertex);
		return 0;
	}
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	fragment = malloc(size + 1);
	size = fread(fragment, 1, size, f);
	fclose(f);
	fragment[size] = 0;
	printf(fragment);
		printf(vertex);
	program = sui_draw_shader_create(debug_output, output_size, vertex, fragment, name);
	free(fragment);
	free(vertex);
	return program;
}


boolean sui_draw_shader_set(uint program)
{
	static uint current_shader = 0;
	if(current_shader == program)
		return FALSE;
	sui_glUseProgramObjectARB(program);
	current_shader = program;
	return TRUE;
}

void sui_draw_shader_attrib_bind(uint program, uint id, char *name)
{
	sui_glBindAttribLocationARB(program, id, name);
	sui_glLinkProgramARB(program);
}

#define GL_TEXTURE0_ARB								0x84C0

void sui_draw_shader_active_texture(uint id)
{
	sui_glActiveTextureARB(GL_TEXTURE0_ARB + id);
}

uint sui_draw_texture_gen(void *data, uint type, uint x_size, uint y_size, boolean alpha)
{
	uint texture_id;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	if(alpha)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x_size, y_size, 0, GL_RGBA, type, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x_size, y_size, 0, GL_RGB, type, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);
	return texture_id;
}

void sui_draw_shader_texture_unit_set(uint program, char *name, uint id)
{
	sui_glUniform1iARB(sui_glGetUniformLocationARB(program, name), id);
}

uint sui_draw_shader_uniform_location(uint program, char *name)
{
	return sui_glGetUniformLocationARB(program, name);
}

void sui_draw_shader_mat4v_set(uint location, float *matrix)
{
	sui_glUniformMatrix4fvARB(location, 1, FALSE, matrix);
}

void sui_draw_shader_mat3v_set(uint location, float *matrix)
{
	sui_glUniformMatrix3fvARB(location, 1, FALSE, matrix);
}

void sui_draw_shader_mat2v_set(uint location, float *matrix)
{
	sui_glUniformMatrix2fvARB(location, 1, FALSE, matrix);
}

void sui_draw_shader_vec4_set(uint location, float v0, float v1, float v2, float v3)
{
	sui_glUniform4fARB(location, v0, v1, v2, v3);
}

void sui_draw_shader_vec3_set(uint location, float v0, float v1, float v2)
{
	sui_glUniform3fARB(location, v0, v1, v2);
}

void sui_draw_shader_vec2_set(uint location, float v0, float v1)
{
	sui_glUniform2fARB(location, v0, v1);
}

void sui_draw_shader_float_set(uint location, float f)
{
	sui_glUniform1fARB(location, f);
}
