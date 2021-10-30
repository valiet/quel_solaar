#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "r_include.h"


void *(*sui_gl_GetProcAddress)(const char* proc) = NULL;
GLubyte *(APIENTRY *r_glGetStringi)(GLenum name, GLuint index);

void r_extension_init(void *(*glGetProcAddr)(const char* proc))
{
	sui_gl_GetProcAddress = glGetProcAddr;
	r_glGetStringi = sui_gl_GetProcAddress("glGetStringi");
}

void *r_extension_get_address(const char* proc)
{
	if(sui_gl_GetProcAddress == NULL)
		return NULL;
	else
		return sui_gl_GetProcAddress(proc);
}

#define GL_NUM_EXTENSIONS 0x821d

boolean r_extension_test(const char *string)
{
	const char *extension, *a;
	uint i, j, count = 0;
/*	if(r_glGetStringi != NULL)
	{
		glGetIntegerv(GL_NUM_EXTENSIONS, &count);
		for(i = 0; i < count; i++)
		{
			extension = r_glGetStringi(GL_EXTENSIONS, i);
			for(j = 0; extension[j] == string[j] && string[j] != 0; j++);
			if(extension[j] == string[j])
				return TRUE;
		}
	}else*/
	{
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
	}
	return FALSE;
}

void r_primitive_init(void);
boolean r_shader_init(void);
void r_array_init(void);
boolean r_framebuffer_init(void);
void r_uniform_init();

boolean r_init(void *(*glGetProcAddr)(const char* proc))
{
	GLboolean (APIENTRY *SwapInterval)(int interval) = NULL;
	float fov = 9.0 / 16.0;
	uint i;

	r_matrix_set(NULL);
	r_matrix_identity(NULL);
	r_matrix_frustum(NULL, -0.05, 0.05, -0.05 * fov, 0.05 * fov, 0.05, 100.0f);
	r_extension_init(glGetProcAddr);
	r_uniform_init();
	SwapInterval = r_extension_get_address("wglSwapIntervalEXT");
	if(SwapInterval == NULL)
		SwapInterval = r_extension_get_address("glXSwapIntervalEXT");
//	if(SwapInterval != NULL)
//		SwapInterval(1); 
	if(!r_shader_init())
		return FALSE;
	if(!r_framebuffer_init())
		return FALSE;
	r_array_init();
	r_primitive_init();
	return TRUE;

}
