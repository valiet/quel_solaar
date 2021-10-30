#include <math.h>

#include "seduce.h"

#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
	#include <GL/gl.h>
#else
#if defined(__APPLE__) || defined(MACOSX)
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif
#endif

#if defined(__APPLE__) || defined(MACOSX)
	#define APIENTRY
#endif

#define GL_FRAMEBUFFER_EXT                     0x8D40
#define GL_RENDERBUFFER_EXT                    0x8D41

#define GL_STENCIL_INDEX1_EXT                  0x8D46
#define GL_STENCIL_INDEX4_EXT                  0x8D47
#define GL_STENCIL_INDEX8_EXT                  0x8D48
#define GL_STENCIL_INDEX16_EXT                 0x8D49

#define GL_RENDERBUFFER_WIDTH_EXT              0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT             0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT    0x8D44
#define GL_RENDERBUFFER_RED_SIZE_EXT           0x8D50
#define GL_RENDERBUFFER_GREEN_SIZE_EXT         0x8D51
#define GL_RENDERBUFFER_BLUE_SIZE_EXT          0x8D52
#define GL_RENDERBUFFER_ALPHA_SIZE_EXT         0x8D53
#define GL_RENDERBUFFER_DEPTH_SIZE_EXT         0x8D54
#define GL_RENDERBUFFER_STENCIL_SIZE_EXT       0x8D55

#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT            0x8CD0
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT            0x8CD1
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_EXT          0x8CD2
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_EXT  0x8CD3
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT     0x8CD4

#define GL_COLOR_ATTACHMENT0_EXT                0x8CE0
#define GL_DEPTH_ATTACHMENT_EXT                 0x8D00
#define GL_STENCIL_ATTACHMENT_EXT               0x8D20

#define GL_FRAMEBUFFER_COMPLETE_EXT                          0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT             0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT     0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT             0x8CD9
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT                0x8CDA
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT            0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT            0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                       0x8CDD

#define GL_FRAMEBUFFER_BINDING_EXT             0x8CA6
#define GL_RENDERBUFFER_BINDING_EXT            0x8CA7
#define GL_MAX_COLOR_ATTACHMENTS_EXT           0x8CDF
#define GL_MAX_RENDERBUFFER_SIZE_EXT           0x84E8


#define GL_DEPTH_COMPONENT16				0x81A5
#define GL_DEPTH_COMPONENT24              0x81A6
#define GL_DEPTH_COMPONENT32              0x81A7
#define GL_STENCIL_INDEX8	             0x8D48
#define GL_CLAMP_TO_EDGE                  0x812F

#define GL_EXT_texture_cube_map             1 
#define GL_NORMAL_MAP_EXT                   0x8511 
#define GL_REFLECTION_MAP_EXT               0x8512 
#define GL_TEXTURE_CUBE_MAP_EXT             0x8513 
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A 
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B 
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C 

void (APIENTRY *sui_glBindFramebufferEXT)(GLenum target, GLuint framebuffer) = NULL;
void (APIENTRY *sui_glDeleteFramebuffersEXT)(GLsizei n, const GLuint *framebuffers) = NULL;
void (APIENTRY *sui_glGenFramebuffersEXT)(GLsizei n, GLuint *framebuffers) = NULL;

void (APIENTRY *sui_glBindRenderbufferEXT)(GLenum target, GLuint renderbuffer) = NULL;
void (APIENTRY *sui_glDeleteRenderbuffersEXT)(GLsizei n, const GLuint *renderbuffers) = NULL;
void (APIENTRY *sui_glGenRenderbuffersEXT)(GLsizei n, GLuint *renderbuffers) = NULL;
GLenum (APIENTRY *sui_glCheckFramebufferStatusEXT)(GLenum target) = NULL;
void (APIENTRY *sui_glRenderbufferStorageEXT)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height) = NULL;

void (APIENTRY *sui_glFramebufferTexture2DEXT)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) = NULL;
void (APIENTRY *sui_glFramebufferRenderbufferEXT)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer) = NULL;
void (APIENTRY *sui_glDrawBuffersARB)(GLuint, GLenum *buffers) = NULL;

#define       GL_RGB32F_ARB                       0x8815
#define       GL_RGB16F_ARB                       0x881B

void sui_draw_framebuffer_status(void)
{
    GLenum status;

    if (!sui_glBindFramebufferEXT)
        return;
    status = sui_glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
	{
        case GL_FRAMEBUFFER_COMPLETE_EXT:
			return;
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
            fprintf(stderr, "Unsupported framebuffer format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
            fprintf(stderr, "Framebuffer incomplete, missing attachment\n");
            break;
	/*	case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
            printf("Framebuffer incomplete, duplicate attachment\n");
            break;*/
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
            fprintf(stderr, "Framebuffer incomplete, attached images must have same dimensions\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
            fprintf(stderr, "Framebuffer incomplete, attached images must have same format\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
            fprintf(stderr, "Framebuffer incomplete, missing draw buffer\n");
			{
				uint *a = NULL;
				a[0] = 0;
			}
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
            fprintf(stderr, "Framebuffer incomplete, missing read buffer\n");
            break;
		default:
            fprintf(stderr, "Error id %x\n", status);
            break;
    }
}

uint sui_draw_framebuffer_depth_buffer_allocate(uint size_x, uint size_y)
{
	uint depth;
	sui_glGenRenderbuffersEXT(1, &depth);		
	sui_glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth);
	sui_glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, size_x, size_y);
	return depth;
}

uint sui_draw_framebuffer_allocate(boolean multilayer, uint *buffers, uint buffer_count, uint depth_buffer)
{
	uint i, fbo;
	sui_glGenFramebuffersEXT(1, &fbo);
	sui_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo); 
	for(i = 0; i < buffer_count; i++)
		sui_glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, buffers[i], 0);
	if(depth_buffer != -1)
		sui_glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_buffer);
	sui_draw_framebuffer_status();
	sui_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	return fbo;
}

void sui_draw_framebuffer_bind(uint id, uint buffers)
{
	GLenum attachements[8] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT0_EXT + 1, GL_COLOR_ATTACHMENT0_EXT + 2, GL_COLOR_ATTACHMENT0_EXT + 3, GL_COLOR_ATTACHMENT0_EXT + 4, GL_COLOR_ATTACHMENT0_EXT + 5, GL_COLOR_ATTACHMENT0_EXT + 6, GL_COLOR_ATTACHMENT0_EXT + 7};
//	sui_glDrawBuffersARB(buffers, attachements);
	sui_glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, id);	
}


boolean sui_draw_framebuffer_init(void)
{
	if(sui_draw_extension_test("GL_EXT_framebuffer_object"))
	{
		sui_glBindFramebufferEXT = sui_draw_extension_get_address("glBindFramebufferEXT");
		sui_glDeleteFramebuffersEXT = sui_draw_extension_get_address("glDeleteFramebuffersEXT");
		sui_glGenFramebuffersEXT = sui_draw_extension_get_address("glGenFramebuffersEXT");
		sui_glBindRenderbufferEXT = sui_draw_extension_get_address("glBindRenderbufferEXT");
		sui_glDeleteRenderbuffersEXT = sui_draw_extension_get_address("glDeleteRenderbuffersEXT");
		sui_glGenRenderbuffersEXT = sui_draw_extension_get_address("glGenRenderbuffersEXT");
		sui_glCheckFramebufferStatusEXT = sui_draw_extension_get_address("glCheckFramebufferStatusEXT");
		sui_glRenderbufferStorageEXT = sui_draw_extension_get_address("glRenderbufferStorageEXT");
		sui_glFramebufferTexture2DEXT = sui_draw_extension_get_address("glFramebufferTexture2DEXT");
		sui_glFramebufferRenderbufferEXT = sui_draw_extension_get_address("glFramebufferRenderbufferEXT");
		sui_glDrawBuffersARB = sui_draw_extension_get_address("glDrawBuffersARB");
		return TRUE;
	}
	return FALSE;
}

