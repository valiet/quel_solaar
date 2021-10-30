#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "r_include.h"



boolean r_buffer_extension = FALSE;

GLvoid		(APIENTRY *r_glBindBufferARB)(uint target, uint buffer);
GLvoid		(APIENTRY *r_glDeleteBuffersARB)(uint n, const uint *buffers);
GLvoid		(APIENTRY *r_glGenBuffersARB)(uint n, uint *buffers);
GLvoid		(APIENTRY *r_glBufferDataARB)(uint target, uint size, const void *data, uint usage);
GLvoid		(APIENTRY *r_glBufferSubDataARB)(uint target, uint offset, uint size, const void *data);
GLvoid		(APIENTRY *r_glGetBufferSubDataARB)(uint target, uint offset, uint size, void *data);
GLvoid		(APIENTRY *r_glCopyBufferSubData)(GLenum readtarget, GLenum writetarget, void *readoffset, void *writeoffset, void *size);
GLvoid		(APIENTRY *r_glEnableVertexAttribArrayARB)(GLuint  index);
GLvoid		(APIENTRY *r_glDisableVertexAttribArrayARB)(GLuint  index);
GLvoid		(APIENTRY *r_glVertexAttribPointerARB)(GLuint  index,  GLint  size,  GLenum  type,  GLboolean  normalized,  GLsizei  stride,  const GLvoid *  pointer);


GLvoid		(APIENTRY *r_glDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, void *indices, GLint basevertex);
GLvoid		(APIENTRY *r_glDrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, GLvoid *indices, GLsizei primcount, GLint basevertex);


void		(APIENTRY *r_glDrawArraysInstanced)( GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
GLvoid		(APIENTRY *r_glMultiDrawArraysIndirect)(GLenum mode, const void *indirect, GLsizei drawcount, GLsizei stride);
GLvoid		(APIENTRY *r_glMultiDrawElementsIndirect)(GLenum mode, GLenum type, const void *indirect, GLsizei drawcount, GLsizei stride);

GLvoid		(APIENTRY *r_glBindVertexArray)(uint array);
GLvoid		(APIENTRY *r_glDeleteVertexArrays)(GLsizei n, const uint *arrays);
GLvoid		(APIENTRY *r_glGenVertexArrays)(GLsizei n, uint *arrays);

GLuint		(APIENTRY *r_glGetUniformBlockIndex)(GLuint program, const char *uniformBlockName);
void		(APIENTRY *r_glBindBufferBase)(GLenum target, GLuint index, GLuint buffer);

void		(APIENTRY *r_glBeginTransformFeedback)(GLenum primitiveMode);
void		(APIENTRY *r_glEndTransformFeedback)(void);


typedef struct{
	uint count;
	uint instanceCount;
	uint first;
	uint baseInstance;
}RDrawArraysIndirectCommand;

typedef struct{
	uint count;
	uint instanceCount;
	uint firstIndex;
	uint baseVertex;
	uint baseInstance;
}RDrawElementsIndirectCommand;



boolean r_array_attrib_mode[64] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
									FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
									FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
									FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
									FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
									FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
									FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
									FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};
uint r_array_bound = 0;

typedef struct{
	uint vertex_start;
	uint vertex_count;
	uint reference_count;
	void *prev;
	void *next;
	void *referencing;
}SUIArraySection;

typedef struct{
	uint gl_buffer;
	uint gl_array;
	uint8 *data;
	SUIArraySection *sections_start;
	SUIArraySection *sections_end;
	uint array_length;
	uint array_used;
	uint vertex_size;
	SUIFormats vertex_format_types[64];
	uint vertex_format_size[64];
	uint vertex_format_count;
	boolean defragged;
}SUIArrayPool;

SUIArrayPool *r_array_pool_bound = NULL;

uint r_array_vertex_size(SUIFormats *vertex_format_types, uint *vertex_format_size, uint vertex_format_count)
{
	uint type_sizes[] = {sizeof(int8), sizeof(uint8), sizeof(int16), sizeof(uint16), sizeof(int32), sizeof(uint32), sizeof(float), sizeof(double)}, i, size = 0;
	for(i = 0; i < vertex_format_count; i++)
	{
/*		printf("vertex_format_types[%u] = %u\n", i, type_sizes[vertex_format_types[i]]);
		printf("vertex_format_size[%u] = %u\n", i, vertex_format_size[i]);
		printf("size %u\n", (type_sizes[vertex_format_types[i]] * vertex_format_size[i] + 3));
		printf("div %u\n", (type_sizes[vertex_format_types[i]] * vertex_format_size[i] + 3) / 4);
*/		size +=	(type_sizes[vertex_format_types[i]] * vertex_format_size[i] + 3) / 4; 
	}
	return size * 4;
}


void r_array_debug_print_out(void *pool)
{
	char *names[8] = {"INT8", "UINT8", "INT16", "UINT16", "INT32", "UINT32", "FLOAT", "DOUBLE"};
	SUIArrayPool *p;
	SUIArraySection *s;
	uint i;
	p = pool;
	printf("Reliquish POOL:\n");
	printf("gl_buffer %u\n", p->gl_buffer);
	printf("data %p\n", p->data);
	printf("sections_start %p\n", p->sections_start);
	printf("sections_end %p\n", p->sections_end);
	printf("array_length %u\n", p->array_length);
	printf("array_used %u\n", p->array_used);
	printf("vertex_size %u\n", p->vertex_size);
	if(p->defragged)
		printf("Defragged TRUE\n");
	else
		printf("Defragged FALSE\n");
	for(i = 0; i < p->vertex_format_count; i++)
	{
		if(p->vertex_format_types[i] < 8)
			printf("Member[%u] Type: %s Count %u\n", i, names[p->vertex_format_types[i]], p->vertex_format_size[i]);
		else
			printf("Member[%u] Type: BROKEN Count %u\n", i, p->vertex_format_size[i]);
	}
	i = 0;
	for(s = p->sections_start; s != NULL; s = s->next)
	{
		printf("SECTION[%u]: %p\n", i++, s);
		printf("vertex_start %u\n", s->vertex_start);
		printf("vertex_count %u\n", s->vertex_count);
		printf("prev %p\n", s->prev);	
		printf("next %p\n", s->next);
	}
}


void *r_array_allocate(uint vertex_count, SUIFormats *vertex_format_types, uint *vertex_format_size, uint vertex_format_count, uint reference_count)
{
	uint type_sizes[] = {1, 1, 2, 2, 4, 4, 4, 8}, i, size = 0;
	SUIArrayPool *p;
	p = malloc(sizeof *p);
	p->vertex_format_count = vertex_format_count;
	for(i = 0; i < vertex_format_count; i++)
	{
		p->vertex_format_types[i] = vertex_format_types[i];
		p->vertex_format_size[i] = vertex_format_size[i];
	}
	for(; i < 64; i++)
		p->vertex_format_size[i] = 0;

	size = r_array_vertex_size(vertex_format_types, vertex_format_size, vertex_format_count);
	p->vertex_size = size;
	p->array_length = vertex_count + (reference_count * 4 + size - 1) / size;
	p->gl_array = 0;
	if(r_buffer_extension)
	{
		r_glGenBuffersARB(1, &p->gl_buffer);
		r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);
		r_glBufferDataARB(GL_ARRAY_BUFFER_ARB, p->array_length * size, NULL, GL_DYNAMIC_DRAW_ARB);
		p->data = NULL;
		if(r_glBindVertexArray != NULL)
		{
			static uint types[] = {GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE};
			uint type_sizes[] = {1, 1, 2, 2, 4, 4, 4, 8}, i, size = 0;
			r_glGenVertexArrays(1, &p->gl_array);
			r_glBindVertexArray(p->gl_array);
			r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);
			r_glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, p->gl_buffer);
			for(i = 0; i < p->vertex_format_count; i++)
			{
				r_glEnableVertexAttribArrayARB(i);
				r_glVertexAttribPointerARB(i, p->vertex_format_size[i], types[p->vertex_format_types[i]], FALSE, p->vertex_size, &p->data[size * 4]);
				size += (type_sizes[p->vertex_format_types[i]] * p->vertex_format_size[i] + 3) / 4; 
			}
			r_glBindVertexArray(0);
		}
		r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, r_array_bound);
	}else
	{
		p->gl_buffer = 0;
		p->data = malloc(p->array_length * size);
	}
	p->sections_start = NULL;
	p->sections_end = NULL;
	p->array_used = 0;
	p->defragged = TRUE;
	return p;
}



void r_array_free(void *pool)
{
	SUIArraySection *sections, *next;
	SUIArrayPool *p;
	if(pool == NULL)
		return;
	
	p = pool;

	if(p->data != NULL)
		free(p->data);

	if(p->gl_buffer != 0)
		r_glDeleteBuffersARB(1, &p->gl_buffer);

	for(sections = p->sections_start; sections != NULL; sections = next)
	{
		next = sections->next;
		free(sections);
	}
	free(p);
}

void r_array_section_move(SUIArrayPool *pool, uint last_pos, uint new_pos, uint size)
{
	uint *from, *to, i;
	if(pool->data != NULL)
	{
		from = (uint *)&pool->data[last_pos / 4];
		to = (uint *)&pool->data[new_pos / 4];
		size /= 4;
		for(i = 0; i < size; i++)
			to[i] = from[i];
	}
	if(pool->gl_buffer != 0)
	{
		void *copy_array;
		copy_array = malloc(size);
		r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, pool->gl_buffer);
		r_glGetBufferSubDataARB(GL_ARRAY_BUFFER_ARB, last_pos, size, copy_array);
		r_glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, new_pos, size, copy_array);
		free(copy_array);
	}
}


void r_array_section_rebase(SUIArrayPool *pool, SUIArraySection *sections, uint move)
{
	uint *index, i, length;
	length = sections->vertex_count * pool->vertex_size / sizeof(uint32);
	if(pool->data != NULL)
	{
		index = (uint *)&pool->data[sections->vertex_start * pool->vertex_size];
		for(i = 0; i < length; i++)
			index[i] -= move;
	}
	if(pool->gl_buffer != 0)
	{
		index = malloc(length * sizeof(uint32));
		r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, pool->gl_buffer);
		r_glGetBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sections->vertex_start * pool->vertex_size, length * sizeof(uint32), index);
		for(i = 0; i < length; i++)
			index[i] -= move;
		r_glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sections->vertex_start * pool->vertex_size, length * sizeof(uint32), index);
		free(index);
	}
}

boolean r_array_defrag(void *pool)
{
	SUIArrayPool *p;
	SUIArraySection *sections, *next, *ref;
	uint move;
	p = pool;
	if(p->defragged)
		return FALSE;
//	fprintf(stderr, "Working\n");

	if(r_array_bound != p->gl_buffer && p->gl_buffer != 0)
		r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);

	for(sections = p->sections_start; sections != NULL; sections = sections->next)
	{
		next = sections->next;
		if(next == NULL)
			break;
		if(sections->vertex_start + sections->vertex_count != next->vertex_start)
		{
			/* found a gap so we will move a block down to close the gap */
			move = next->vertex_start - (sections->vertex_start + sections->vertex_count);
			r_array_section_move(p, next->vertex_start * p->vertex_size, (sections->vertex_start + sections->vertex_count) * p->vertex_size, sections->vertex_count * p->vertex_size);
			next->vertex_start = sections->vertex_start + sections->vertex_count;
			if(sections->referencing != NULL && r_glDrawElementsBaseVertex == NULL)
				for(ref = p->sections_start; ref != NULL; ref = ref->next)
					if(ref == sections->referencing)
						r_array_section_rebase(pool, sections, move);
			if(r_array_bound != p->gl_buffer && p->gl_buffer != 0)
				r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, r_array_bound);
			return TRUE;
		}
	}
	if(r_array_bound != p->gl_buffer && p->gl_buffer != 0)
		r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, r_array_bound);
	p->defragged = TRUE;
	return FALSE;
}

void *r_array_section_allocate_vertex(void *pool, uint size)
{
	SUIArrayPool *p;
	SUIArraySection *section, *s;
	uint pos;
	p = pool;

	if(p->array_used + size > p->array_length) /* there is no space no matter */
	{
		printf("there is no space no matter %u %u %u\n", p->array_used, size, p->array_length);
		exit(0);
		return NULL;
	}
	section = malloc(sizeof *section);
	section->vertex_count = size;
	section->reference_count = 0;
	section->referencing = NULL;
	p->array_used += size;

	if(p->sections_start == NULL) // no section exists
	{
		p->sections_start = section;
		p->sections_end = section;
		section->next = section->prev = NULL;
		section->vertex_start = 0;
		return section;
	}else
	{
		s = p->sections_end;
		if(p->array_length >= s->vertex_start + s->vertex_count + size) // add new section to end of array
		{
			s->next = section;
			p->sections_end = section;
			section->next = NULL;
			section->prev = s;
			section->vertex_start = s->vertex_start + s->vertex_count;
			return section;
		}
		/* try to find a hole in the array of sections where the new section fits */
		pos = 0;
		for(s = p->sections_start; s != NULL; s = s->next)
		{
			if(pos + size <= s->vertex_start) /* Found a hole */
			{
				if(s->prev == NULL)
					p->sections_start = section;
				else
					((SUIArraySection *)s->prev)->next = section;
				section->prev = s->prev;
				section->next = s;
				s->prev = section;
				section->vertex_start = pos;
				return section;
			}
			pos = s->vertex_start + s->vertex_count;
		}
		/* lets do it again, but this time we start to defrag the segments to eventiualy find a gap big enough */
		pos = 0;
		if(r_array_bound != p->gl_buffer && p->gl_buffer != 0)
			r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);
		for(s = p->sections_start; s != NULL; s = s->next)
		{
			if(pos + size <= s->vertex_start) /* Found a hole big enough */
			{
				if(s->prev == NULL)
					p->sections_start = section;
				else
					((SUIArraySection *)s->prev)->next = section;
				section->prev = s->prev;
				section->next = s;
				s->prev = section;
				section->vertex_start = pos;
				return section;
			}
			if(pos != s->vertex_start) /* move back a section to close the gap. */
			{
				r_array_section_move(p, pos * p->vertex_size, s->vertex_start * p->vertex_size, s->vertex_count * p->vertex_size);
				s->vertex_start = pos;
			}
			pos = s->vertex_start + s->vertex_count;
		}
		if(r_array_bound != p->gl_buffer && p->gl_buffer != 0)
			r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, r_array_bound);
		/* add to the end */
		s = p->sections_end;
		s->next = section;
		p->sections_end = section;
		section->next = NULL;
		section->prev = s;
		section->vertex_start = s->vertex_start + s->vertex_count;
		return section;
	}
	return NULL;
}

void *r_array_section_allocate_reference(void *pool, uint size)
{
	SUIArraySection *section;
	section = r_array_section_allocate_vertex(pool, (size * sizeof(uint32) + ((SUIArrayPool * )pool)->vertex_size - 1) / ((SUIArrayPool * )pool)->vertex_size);
	section->reference_count = size;
	return section;
}

void r_array_section_free(void *pool, void *section)
{
	SUIArrayPool *p;
	SUIArraySection *s;
	uint pos;
	p = pool;
	for(s = p->sections_start; s != NULL; s = s->next)
	{
		if(s->referencing == section)
		{
			r_array_section_free(pool, s);
		}
	}
	s = section;
	p->array_used -= s->vertex_count;

	if(s->next == NULL)
		p->sections_end = s->prev;
	else
		((SUIArraySection *)s->next)->prev = s->prev;

	if(s->prev == NULL)
		p->sections_start = s->next;
	else
		((SUIArraySection *)s->prev)->next = s->next;
	free(s);
}


void  r_array_init(void)
{
	uint i;
#ifdef RELINQUISH_CONTEXT_OPENGLES
	r_glBindBufferARB = glBindBuffer;
	r_glDeleteBuffersARB = glDeleteBuffers;
	r_glGenBuffersARB = glGenBuffers;
	r_glBufferDataARB = glBufferData;
	r_glBufferSubDataARB = glBufferSubData;
//	r_glGetBufferSubDataARB = glGetBufferSubData;

	r_glVertexAttribPointerARB = glVertexAttribPointer;
	r_glEnableVertexAttribArrayARB = glEnableVertexAttribArray;
	r_glDisableVertexAttribArrayARB = glDisableVertexAttribArray;
	r_buffer_extension = TRUE;
#endif 
#ifdef RELINQUISH_CONTEXT_OPENGL
	r_buffer_extension = r_extension_test("GL_ARB_vertex_buffer_object");
	if(r_buffer_extension)
	{
		r_glBindBufferARB = r_extension_get_address("glBindBufferARB");
		r_glDeleteBuffersARB = r_extension_get_address("glDeleteBuffersARB");
		r_glGenBuffersARB = r_extension_get_address("glGenBuffersARB");
		r_glBufferDataARB = r_extension_get_address("glBufferDataARB");
		r_glBufferSubDataARB = r_extension_get_address("glBufferSubDataARB");
		r_glGetBufferSubDataARB = r_extension_get_address("glGetBufferSubDataARB");
		if(r_extension_test("GL_ARB_vertex_array_object"))
		{
			r_glBindVertexArray = r_extension_get_address("glBindVertexArray");
			r_glDeleteVertexArrays = r_extension_get_address("glDeleteVertexArrays");
			r_glGenVertexArrays = r_extension_get_address("glGenVertexArrays");
		}
	}

	r_glGetUniformBlockIndex = r_extension_get_address("glGetUniformBlockIndex");
	r_glBeginTransformFeedback = r_extension_get_address("glBeginTransformFeedback");
	r_glEndTransformFeedback = r_extension_get_address("glEndTransformFeedback");
	r_glVertexAttribPointerARB = r_extension_get_address("glVertexAttribPointerARB");
	r_glEnableVertexAttribArrayARB = r_extension_get_address("glEnableVertexAttribArrayARB");
	r_glDisableVertexAttribArrayARB = r_extension_get_address("glDisableVertexAttribArrayARB");
	r_glDrawElementsBaseVertex = r_extension_get_address("glDrawElementsBaseVertex");
	r_glDrawElementsInstancedBaseVertex = r_extension_get_address("glDrawElementsInstancedBaseVertex");
	r_glMultiDrawArraysIndirect = r_extension_get_address("glMultiDrawArraysIndirect");
	r_glMultiDrawElementsIndirect = r_extension_get_address("glMultiDrawElementsIndirect");
	r_glCopyBufferSubData = r_extension_get_address("glCopyBufferSubData");

#endif
	r_array_bound = 0;
	for(i = 0; i < 64; i++)
		r_array_attrib_mode[i] = FALSE;
}

float *sui_vertex_tmp = NULL;

void r_array_vertex_atrib_set(void *pool, boolean reference)
{
	static uint types[] = {GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE};
	uint type_sizes[] = {1, 1, 2, 2, 4, 4, 4, 8}, i, size = 0;
	SUIArrayPool *p;
	if(r_array_pool_bound == pool)
		return;
	p = pool;
	r_array_pool_bound = pool;

	if(r_glBindVertexArray != NULL)
	{
		r_glBindVertexArray(p->gl_array);
		if(reference)
			r_glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, p->gl_buffer);
	}else
	{
		if(r_array_bound != p->gl_buffer && p->gl_buffer != 0)
		{
			r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);
			if(reference)
				r_glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, p->gl_buffer);
			r_array_bound = p->gl_buffer;
		}
		for(i = 0; i < p->vertex_format_count; i++)
		{
			r_glVertexAttribPointerARB(i, p->vertex_format_size[i], types[p->vertex_format_types[i]], FALSE, p->vertex_size, &p->data[size * 4]);
			if(!r_array_attrib_mode[i])
				r_glEnableVertexAttribArrayARB(i);
			r_array_attrib_mode[i] = TRUE;
			size += (type_sizes[p->vertex_format_types[i]] * p->vertex_format_size[i] + 3) / 4; 
		}
		for(; i < 64; i++)
		{
			if(r_array_attrib_mode[i])
			{
				r_array_attrib_mode[i] = FALSE;
				r_glDisableVertexAttribArrayARB(i);
			}
		}
	}
}

void r_array_deactivate()
{
	uint i;
	r_array_pool_bound = NULL;
	if(r_array_bound != 0)
	{
		r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		r_array_bound = 0;
	}
	if(r_glBindVertexArray != NULL)
		r_glBindVertexArray(0);
	else
		for(i = 0; i < 64; i++)
			r_glDisableVertexAttribArrayARB(i);
}

void r_array_load_vertex(void *pool, SUIArraySection *section, void *data, uint start, uint length)
{
	SUIArrayPool *p;
	p = pool;
	if(section != NULL)
	{
		if(((SUIArraySection *)section)->vertex_count < length)
			length = ((SUIArraySection *)section)->vertex_count;

		if(p->data != NULL)
			memcpy(&p->data[(((SUIArraySection *)section)->vertex_start + start) * p->vertex_size], 
			data, 
			length * p->vertex_size);

		if(p->gl_buffer != 0)
		{
			if(r_array_bound != p->gl_buffer)
				r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);
			r_glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, (((SUIArraySection *)section)->vertex_start + start) * p->vertex_size, length * p->vertex_size, data);
			if(r_array_bound != p->gl_buffer)
				r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, r_array_bound);
		}
	}else
	{
		if(p->array_length + start < length)
		{
			length = p->array_length - start;
		}
		if(p->data != NULL)
		{
			memcpy(&p->data[start * p->vertex_size], 
			data, 
			length * p->vertex_size);
		}
		if(p->gl_buffer != 0)
		{
			if(r_array_bound != p->gl_buffer)
				r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);
			r_glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, start * p->vertex_size, length * p->vertex_size, data);
			if(r_array_bound != p->gl_buffer)
				r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, r_array_bound);
		}
	}
}

void r_array_load_reference(void *pool, void *section, void *referencing_section, uint *reference, uint length)
{
	SUIArrayPool *p;
	uint size;
	p = pool;

	r_array_vertex_atrib_set(pool, TRUE);
	((SUIArraySection *)section)->referencing = referencing_section;
	size = ((SUIArraySection *)section)->vertex_start;
	if(p->data != NULL)
	{
		if(((SUIArraySection *)referencing_section)->vertex_start == 0 || r_glDrawElementsBaseVertex != NULL)
			memcpy(&p->data[((SUIArraySection *)section)->vertex_start * p->vertex_size], reference, length * sizeof(uint32));
		else
		{
			uint32 *temp, *to, i, start;
			start = ((SUIArraySection *)referencing_section)->vertex_start;
			to = (uint32 *)&p->data[((SUIArraySection *)section)->vertex_start * p->vertex_size];
			for(i = 0; i < length; i++)
				to[i] = reference[i] + start;
		}
	}
	if(p->gl_buffer != 0)
	{
		if(r_array_bound != p->gl_buffer)
			r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, p->gl_buffer);
		if(((SUIArraySection *)referencing_section)->vertex_start == 0 || r_glDrawElementsBaseVertex != NULL)
			r_glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, (((SUIArraySection *)section)->vertex_start) * p->vertex_size, length * sizeof(uint32), reference);
		else
		{
			uint32 *temp, *to, i, start;
			start = ((SUIArraySection *)referencing_section)->vertex_start;
			temp = malloc((sizeof *temp) * length);
			to = (uint32 *)&p->data[((SUIArraySection *)section)->vertex_start * p->vertex_size];
			for(i = 0; i < length; i++)
				to[i] = reference[i] + start;
			r_glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, (((SUIArraySection *)section)->vertex_start) * p->vertex_size, length * sizeof(uint32), temp);
			free(temp);
		}
		if(r_array_bound != p->gl_buffer)
			r_glBindBufferARB(GL_ARRAY_BUFFER_ARB, r_array_bound);
	}
	((SUIArraySection *)section)->referencing = referencing_section;
}/*
ern void		r_array_load_vertex(void *pool, void *section, void *data, uint start, uint length);
extern void		r_array_load_reference(void *pool, void *section, uint *data, void *vertex_section, uint length);

*/
uint r_array_get_size(void *pool)
{
	return ((SUIArrayPool *)pool)->array_length;
}

uint r_array_get_used(void *pool)
{
	return ((SUIArrayPool *)pool)->array_used;
}

uint r_array_get_left(void *pool)
{
	return ((SUIArrayPool *)pool)->array_length - ((SUIArrayPool *)pool)->array_used;
}

uint r_array_get_vertex_size(void *pool)
{
	return ((SUIArrayPool *)pool)->vertex_size;
}

uint r_array_section_get_vertex_count(void *section)
{
	return ((SUIArraySection *)section)->vertex_count;
}

uint r_array_section_get_vertex_start(void *section)
{
	return ((SUIArraySection *)section)->vertex_start;
}

extern void r_shader_uniform_matrix_set(RMatrix *matrix, RShader *shader, uint block_id, uint8 *uniform_data);

/*
void r_array_section_draw(void *pool, void *section, uint primitive_type, uint vertex_start, uint vertex_count)
{
	uint i, offset;
	if(r_current_shader == NULL)
	{
		printf("Relinquish ERROR: no Shader set\n");
		return;
	}
	r_array_vertex_atrib_set(pool, FALSE);
	for(i = 0; i < r_current_shader->block_count; i++)
		r_shader_uniform_matrix_set(r_current_shader, r_current_shader->uniform_data, i, (RMatrix *)r_matrix_state);
	r_shader_unifrom_data_set_all(r_current_shader, r_current_shader->uniform_data, -1);

	if(section != NULL)
	{	
		SUIArraySection *s;
		s = (SUIArraySection *)section;
		if(vertex_count > s->vertex_count)
			vertex_count = s->vertex_count;
		glDrawArrays(primitive_type, s->vertex_start + vertex_start, vertex_count);
	}else
	{
		SUIArrayPool *p;
		p = pool;
		if(vertex_count > p->array_length)
			vertex_count = p->array_length;
		glDrawArrays(primitive_type, vertex_start, vertex_count);
	}
}*/

void r_array_draw(SUIArrayPool *pool, SUIArraySection *section, RPrimitive primitive_type, uint vertex_start, uint vertex_count, uint8 *uniforms, SUIArrayPool *array_pool, uint count)
{
	uint i, bind_point, add;
	uint progress, offset, calls, j;
	uint8 *p;

	r_array_vertex_atrib_set(pool, section != NULL && section->referencing != NULL);
	for(i = 0; i < r_current_shader->block_count; i++)
		r_shader_uniform_matrix_set(r_current_shader, r_current_shader->uniform_data, i, (RMatrix *)r_matrix_state);
	if(uniforms == NULL && array_pool == NULL)
	{
		r_shader_unifrom_data_set_all(r_current_shader, r_current_shader->uniform_data, -1);
		count = 1;
	}else
		r_shader_unifrom_data_set_all(r_current_shader, r_current_shader->uniform_data, r_current_shader->instance_block);
	if(section == NULL)
	{
		if(vertex_start >= pool->array_length)
			return;
		if(vertex_start + vertex_count > pool->array_length)
			vertex_count = pool->array_length - vertex_start;
	}else
	{
		if(section->referencing != NULL)
		{
			if(vertex_count > section->reference_count)
				vertex_count = section->reference_count;
			p = (uint *)(pool->data);
			p = &p[(section->vertex_start + vertex_start) * ((SUIArrayPool *)pool)->vertex_size];
		}else
		{

			if(vertex_start >= section->vertex_count)
				return;
			if(vertex_count > section->vertex_count - vertex_start)
				vertex_count = section->vertex_count - vertex_start;
			vertex_start += section->vertex_start;
		}
	}

/*
	if(r_glMultiDrawArraysIndirect != NULL)
	{
		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
			add = count;
		else
		{
			add = r_current_shader->blocks[r_current_shader->instance_block].array_length;
			if(add > count)
				add = count;
		}
	}else
		add = 1;*/
	if(r_current_shader->blocks[r_current_shader->instance_block].object == -1)
	{
//			uniforms = &r_current_shader->uniform_data[r_current_shader->blocks[r_current_shader->instance_block].offset];
		if(array_pool != NULL)
		{
			uniforms = malloc(r_current_shader->blocks[r_current_shader->instance_block].size * count);
			r_glBindBufferARB(GL_COPY_READ_BUFFER, array_pool->gl_buffer);
			r_glGetBufferSubDataARB(GL_COPY_READ_BUFFER, 0, r_current_shader->blocks[r_current_shader->instance_block].size * count, uniforms);
		}

		for(i = 0; i < count; i++)
		{	
			r_current_shader->blocks[r_current_shader->instance_block].updated = FALSE;
			if(uniforms != NULL)
				r_shader_unifrom_data_set_block(r_current_shader, &(((uint8 *)uniforms)[r_current_shader->blocks[r_current_shader->instance_block].size * i]), r_current_shader->instance_block);
			if(section != NULL)
			{
				if(section->referencing == NULL)
					glDrawArrays(primitive_type, vertex_start, vertex_count);
				else
				{
					if(r_glDrawElementsBaseVertex != NULL)
						r_glDrawElementsBaseVertex(primitive_type, vertex_count, GL_UNSIGNED_INT, p, ((SUIArraySection *)section->referencing)->vertex_start);
					else
						glDrawElements(primitive_type, vertex_count, GL_UNSIGNED_INT, p);
				}
			}else
				glDrawArrays(primitive_type, vertex_start, vertex_count);
		}
		if(array_pool != NULL)
			free(uniforms);
	}else
	{
		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT)
			bind_point = GL_SHADER_STORAGE_BUFFER;
		else
			bind_point = GL_UNIFORM_BUFFER;
		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
			r_glBindBufferBase(bind_point, r_current_shader->blocks[r_current_shader->instance_block].id, array_pool->gl_buffer);
		else
			r_glBindBufferBase(bind_point, r_current_shader->blocks[r_current_shader->instance_block].id, r_current_shader->blocks[r_current_shader->instance_block].object);

		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
			r_glBindBufferARB(bind_point, array_pool->gl_buffer);
		else
		{
			if(array_pool != NULL)
				r_glBindBufferARB(GL_COPY_READ_BUFFER, array_pool->gl_buffer);
			r_glBindBufferARB(bind_point, r_current_shader->blocks[r_current_shader->instance_block].object);
		}	

		if(r_glDrawArraysInstanced != NULL)
		{
			if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
				add = count;
			else
				add = r_current_shader->blocks[r_current_shader->instance_block].array_length;
		}else
			add = 1;

		for(progress = 0; progress < count; progress += add)
		{
			if(add + progress > count)
				add = count - progress;
			if(array_pool != NULL)
			{
				if(r_current_shader->mode == R_SIM_UNIFORM_BLOCK)
					r_glCopyBufferSubData(GL_COPY_READ_BUFFER, bind_point, progress * r_current_shader->blocks[r_current_shader->instance_block].size, 0, add * r_current_shader->blocks[r_current_shader->instance_block].size);
			}else if(uniforms != NULL)
				r_glBufferDataARB(bind_point, 
								add * r_current_shader->blocks[r_current_shader->instance_block].size, 
								&uniforms[progress * r_current_shader->blocks[r_current_shader->instance_block].size], 
								GL_DYNAMIC_DRAW_ARB);
			if(add != 1)
			{
				if(section != NULL && section->referencing != NULL)
				{
					r_glDrawElementsInstancedBaseVertex(primitive_type, vertex_count, GL_UNSIGNED_INT, p, add, ((SUIArraySection *)section->referencing)->vertex_start);
				}else
					r_glDrawArraysInstanced(primitive_type, 0, vertex_count, add);
			}else
			{
				if(section != NULL && section->referencing != NULL)
				{
					if(r_glDrawElementsBaseVertex != NULL)
						r_glDrawElementsBaseVertex(primitive_type, vertex_count, GL_UNSIGNED_INT, p, ((SUIArraySection *)section->referencing)->vertex_start);
					else
						glDrawElements(primitive_type, vertex_count, GL_UNSIGNED_INT, p);
				}else
					glDrawArrays(primitive_type, vertex_start, vertex_count);
			}
		}
	}
}



void r_array_sections_draw(void *pool, void **sections, RPrimitive primitive_type, uint8 *uniforms, SUIArrayPool *array_pool, uint count)
{
	uint i, bind_point;
	if(r_current_shader == NULL)
	{
		printf("Relinquish ERROR: no Shader set\n");
		return;
	}
	if(uniforms == NULL && array_pool == NULL)
	{
		printf("RELINQUISH Error: r_array_sections_draw uniform is NULL.\n");
		exit(0);
	}
	if(array_pool != NULL)
	{
		if(count > array_pool->array_length)
			count = array_pool->array_length;
		if(array_pool->vertex_size != r_current_shader->blocks[r_current_shader->instance_block].size)
		{
			printf("RELINQUISH Error: r_array_sections_draw array_pool vertex format size does not match instance uniform block size.\n");
			return;
		}
	}
	r_array_vertex_atrib_set(pool, FALSE);
	for(i = 0; i < r_current_shader->block_count; i++)
		r_shader_uniform_matrix_set(r_current_shader, r_current_shader->uniform_data, i, (RMatrix *)r_matrix_state);
	r_shader_unifrom_data_set_all(r_current_shader, r_current_shader->uniform_data, r_current_shader->instance_block);

	if(r_current_shader->blocks[r_current_shader->instance_block].object == -1)
	{
		SUIArraySection *s;
		SUIArrayPool *p;
		p = pool;
		if(array_pool != NULL)
		{
			uniforms = malloc(r_current_shader->blocks[r_current_shader->instance_block].size * count);
			r_glBindBufferARB(GL_COPY_READ_BUFFER, array_pool->gl_buffer);
			r_glGetBufferSubDataARB(GL_COPY_READ_BUFFER, 0, r_current_shader->blocks[r_current_shader->instance_block].size * count, uniforms);
		}
		for(i = 0; i < count; i++)
		{	
			r_current_shader->blocks[r_current_shader->instance_block].updated = FALSE;
			r_shader_unifrom_data_set_block(r_current_shader, &(((uint8 *)uniforms)[r_current_shader->blocks[r_current_shader->instance_block].size * i]), r_current_shader->instance_block);
			if(sections != NULL)
			{
				s = (SUIArraySection *)sections[i];
				glDrawArrays(primitive_type, s->vertex_start, s->vertex_count);
			}else
				glDrawArrays(primitive_type, 0, p->array_length);
		}
		if(array_pool != NULL)
			free(uniforms);
	}else
	{
		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT)
			bind_point = GL_SHADER_STORAGE_BUFFER;
		else
			bind_point = GL_UNIFORM_BUFFER;
		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
			r_glBindBufferBase(bind_point, r_current_shader->blocks[r_current_shader->instance_block].id, array_pool->gl_buffer);
		else
			r_glBindBufferBase(bind_point, r_current_shader->blocks[r_current_shader->instance_block].id, r_current_shader->blocks[r_current_shader->instance_block].object);

		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
			r_glBindBufferARB(bind_point, array_pool->gl_buffer);
		else
		{
			if(array_pool != NULL)
				r_glBindBufferARB(GL_COPY_READ_BUFFER, array_pool->gl_buffer);
			r_glBindBufferARB(bind_point, r_current_shader->blocks[r_current_shader->instance_block].object);
		}
		if(sections != NULL)
		{	
			static command_count = 0;
			static RDrawArraysIndirectCommand *command =  NULL;
			uint progress, offset, add, calls, j;
			SUIArraySection *s;

			if(r_glMultiDrawArraysIndirect != NULL)
			{
				if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
					add = count;
				else
					add = r_current_shader->blocks[r_current_shader->instance_block].array_length;
			}else
				add = 1;

			if(add > command_count)
			{
				command = realloc(command, (sizeof *command) * add);
				command_count = add;
			}
	
			for(progress = 0; progress < count; progress += add)
			{
				if(add + progress > count)
					add = count - progress;
				if(array_pool != NULL)
				{
					if(r_current_shader->mode == R_SIM_UNIFORM_BLOCK)
						r_glCopyBufferSubData(GL_COPY_READ_BUFFER, bind_point, progress * r_current_shader->blocks[r_current_shader->instance_block].size, 0, add * r_current_shader->blocks[r_current_shader->instance_block].size);
				}else
					r_glBufferDataARB(bind_point, 
									add * r_current_shader->blocks[r_current_shader->instance_block].size, 
									&uniforms[progress * r_current_shader->blocks[r_current_shader->instance_block].size], 
									GL_DYNAMIC_DRAW_ARB);
				if(add != 1)
				{
					for(i = calls = 0; i < add; i += j)
					{
						s = (SUIArraySection *)sections[i + progress];
						command[calls].baseInstance = i;
						command[calls].first = s->vertex_start;
						command[calls].count = s->vertex_count;
					//	for(j = 1; sections[i + progress] == sections[i + progress + j] && i + j <= add; j++);
						j = 1;
						command[calls].instanceCount = i + j;
						calls++;
					}
					r_glMultiDrawArraysIndirect(primitive_type, command, calls, (sizeof *command));
				}else
					glDrawArrays(primitive_type, s->vertex_start, s->vertex_count);
			}
		}else
		{
			SUIArrayPool *p;
			uint progress, offset, add;
			p = pool;
			if(r_glDrawArraysInstanced != NULL)
			{
				if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
					add = count;
				else
					add = r_current_shader->blocks[r_current_shader->instance_block].array_length;
			}else
				add = 1;
					

			for(progress = 0; progress < count; progress += add)
			{
				if(add + progress > count)
					add = count - progress;
				if(array_pool != NULL)
				{
					if(r_current_shader->mode == R_SIM_UNIFORM_BLOCK)
						r_glCopyBufferSubData(GL_COPY_READ_BUFFER, bind_point, progress * r_current_shader->blocks[r_current_shader->instance_block].size, 0, add * r_current_shader->blocks[r_current_shader->instance_block].size);
				}else
					r_glBufferDataARB(bind_point, 
									add * r_current_shader->blocks[r_current_shader->instance_block].size, 
									&uniforms[progress * r_current_shader->blocks[r_current_shader->instance_block].size], 
									GL_DYNAMIC_DRAW_ARB);
				if(add != 1)
					r_glDrawArraysInstanced(primitive_type, 0, p->array_length, add);
				else
					glDrawArrays(primitive_type, 0, p->array_length);
			}
		}
	}
}

/*
void r_array_reference_draw(void *pool, void *section, uint primitive_type, uint vertex_start, uint vertex_count)
{
	uint i;
	r_array_vertex_atrib_set(pool, TRUE);
	if(r_current_shader != NULL)
	{
		for(i = 0; i < r_current_shader->block_count; i++)
			r_shader_uniform_matrix_set(r_current_shader, r_current_shader->uniform_data, r_current_shader->instance_block, (RMatrix *)r_matrix_state);
		r_shader_unifrom_data_set_all(r_current_shader, r_current_shader->uniform_data, -1);
	}
	if(section != NULL)
	{	
		uint i, *p;
		if(vertex_count > ((SUIArraySection *)section)->vertex_count * ((SUIArrayPool *)pool)->vertex_size / 4 - vertex_start)
			vertex_count = ((SUIArraySection *)section)->vertex_count * ((SUIArrayPool *)pool)->vertex_size / 4 - vertex_start;
		p = (uint *)((SUIArrayPool *)pool)->data;
		p = &p[(((SUIArraySection *)section)->vertex_start + vertex_start) * ((SUIArrayPool *)pool)->vertex_size / 4];
		if(r_glDrawElementsBaseVertex == NULL)
			glDrawElements(primitive_type, vertex_count, GL_UNSIGNED_INT, p);
		else
			r_glDrawElementsBaseVertex(primitive_type, vertex_count, GL_UNSIGNED_INT, p, ((SUIArraySection *)((SUIArraySection *)section)->referencing)->vertex_start);

	}
}*/

void r_array_references_draw(void *pool, void **sections, RPrimitive primitive_type, uint8 *uniforms, SUIArrayPool *array_pool, uint count)
{
	uint i, vertex_count, bind_point;
	uint8 *p;
	if(r_current_shader == NULL)
	{
		printf("Relinquish ERROR: no Shader set\n");
		return;
	}
	if(uniforms == NULL && array_pool == NULL)
	{
		printf("RELINQUISH Error: r_array_sections_draw uniform is NULL.\n");
		exit(0);
	}
	if(array_pool != NULL)
	{
		if(count > array_pool->array_length)
			count = array_pool->array_length;
		if(array_pool->vertex_size != r_current_shader->blocks[r_current_shader->instance_block].size)
		{
			printf("RELINQUISH Error: r_array_sections_draw array_pool vertex format size does not match instance uniform block size.\n");
			return;
		}
	}
	r_array_vertex_atrib_set(pool, TRUE);
	for(i = 0; i < r_current_shader->block_count; i++)
		if(r_current_shader->instance_block != i)
			r_shader_uniform_matrix_set(r_current_shader, r_current_shader->uniform_data, i, (RMatrix *)r_matrix_state);
	r_shader_unifrom_data_set_all(r_current_shader, r_current_shader->uniform_data, r_current_shader->instance_block);

	if(r_current_shader->blocks[r_current_shader->instance_block].object == -1)
	{
		if(sections != NULL)
		{	
			SUIArraySection *s;
			if(array_pool != NULL)
			{
				uniforms = malloc(r_current_shader->blocks[r_current_shader->instance_block].size * count);
				r_glBindBufferARB(GL_COPY_READ_BUFFER, array_pool->gl_buffer);
				r_glGetBufferSubDataARB(GL_COPY_READ_BUFFER, 0, r_current_shader->blocks[r_current_shader->instance_block].size * count, uniforms);
			}
			for(i = 0; i < count; i++)
			{	
				s = (SUIArraySection *)sections[i];
				r_current_shader->blocks[r_current_shader->instance_block].updated = FALSE;
				r_shader_unifrom_data_set_block(r_current_shader, &(((uint8 *)uniforms)[r_current_shader->blocks[r_current_shader->instance_block].size * i]), r_current_shader->instance_block);

				vertex_count = ((SUIArraySection *)sections[i])->reference_count;
				p = ((SUIArrayPool *)pool)->data;
				p = &p[((SUIArraySection *)sections[i])->vertex_start * ((SUIArrayPool *)pool)->vertex_size];
				if(r_glDrawElementsBaseVertex == NULL)
					glDrawElements(primitive_type, vertex_count, GL_UNSIGNED_INT, p);
				else
					r_glDrawElementsBaseVertex(primitive_type, vertex_count, GL_UNSIGNED_INT, p, ((SUIArraySection *)((SUIArraySection *)sections[i])->referencing)->vertex_start);
			}
			if(array_pool != NULL)
				free(uniforms);
		}
	}else
	{
		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT)
			bind_point = GL_SHADER_STORAGE_BUFFER;
		else
			bind_point = GL_UNIFORM_BUFFER;
		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
			r_glBindBufferBase(bind_point, r_current_shader->blocks[r_current_shader->instance_block].id, array_pool->gl_buffer);
		else
			r_glBindBufferBase(bind_point, r_current_shader->blocks[r_current_shader->instance_block].id, r_current_shader->blocks[r_current_shader->instance_block].object);

		if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
			r_glBindBufferARB(bind_point, array_pool->gl_buffer);
		else
		{
			if(array_pool != NULL)
				r_glBindBufferARB(GL_COPY_READ_BUFFER, array_pool->gl_buffer);
			r_glBindBufferARB(bind_point, r_current_shader->blocks[r_current_shader->instance_block].object);
		}
		if(sections != NULL)
		{	
			static command_count = 0;
			static RDrawElementsIndirectCommand *command =  NULL;
			uint progress, offset, add, calls, j;
			SUIArraySection *s;
			
			if(r_glMultiDrawElementsIndirect != NULL)
			{
				if(r_current_shader->mode == R_SIM_BUFFER_OBJECT && array_pool != NULL)
					add = count;
				else
					add = r_current_shader->blocks[r_current_shader->instance_block].array_length;
			}else
				add = 1;
			if(add > command_count)
			{
				command = realloc(command, (sizeof *command) * add);
				command_count = add;
			}
//			r_glBindBufferARB(bind_point, r_current_shader->blocks[0].object);
			for(progress = 0; progress < count; progress += add)
			{
				if(add + progress > count)
					add = count - progress;
				if(array_pool != NULL)
				{
					if(r_current_shader->mode == R_SIM_UNIFORM_BLOCK)
						r_glCopyBufferSubData(GL_COPY_READ_BUFFER, bind_point, progress * r_current_shader->blocks[r_current_shader->instance_block].size, 0, add * r_current_shader->blocks[r_current_shader->instance_block].size);
				}else
					r_glBufferDataARB(bind_point, 
									add * r_current_shader->blocks[r_current_shader->instance_block].size, 
									&uniforms[progress * r_current_shader->blocks[r_current_shader->instance_block].size], 
									GL_DYNAMIC_DRAW_ARB);
				if(add != 1)
				{
					for(i = calls = 0; i < add; i += j)
					{
						s = (SUIArraySection *)sections[i + progress];
						command[calls].baseInstance = i;							
						p = ((SUIArrayPool *)pool)->data;
						p = &p[((SUIArraySection *)sections[i])->vertex_start * ((SUIArrayPool *)pool)->vertex_size / sizeof(uint)];
						command[calls].firstIndex = p;
						command[calls].count = s->reference_count;
						command[calls].baseVertex = ((SUIArraySection *)((SUIArraySection *)sections[i])->referencing)->vertex_start;
						for(j = 1; sections[i + progress] == sections[i + progress + j] && i + j <= add; j++);
						command[calls].instanceCount = j;
						calls++;
					}
					r_glMultiDrawElementsIndirect(primitive_type, GL_UNSIGNED_INT, command, calls, (sizeof *command));
				}else
				{
					vertex_count = ((SUIArraySection *)sections[progress])->reference_count;
					p = ((SUIArrayPool *)pool)->data;
					p = &p[((SUIArraySection *)sections[progress])->vertex_start * ((SUIArrayPool *)pool)->vertex_size];
					if(r_glDrawElementsBaseVertex == NULL)
						glDrawElements(primitive_type, vertex_count, GL_UNSIGNED_INT, p);
					else
						r_glDrawElementsBaseVertex(primitive_type, vertex_count, GL_UNSIGNED_INT, p, ((SUIArraySection *)((SUIArraySection *)sections[progress])->referencing)->vertex_start);
				}
			}
		}
	}
	i = 0;
}


void sui_array_section_draw_element(void *vertex_section, uint primitive_type, uint ref_type, void *ref_section, uint ref_count)
{
	r_shader_uniform_matrix_set(r_current_shader, r_current_shader->uniform_data, r_current_shader->instance_block, (RMatrix *)r_matrix_state);
/*	(((SUIArraySection *)section)->vertex_start + vertex_start) * 
	glDrawElements(primitive_type, ref_count, ref_type, ref_section);*/
}

#define GL_RASTERIZER_DISCARD 0x8C89
#define GL_TRANSFORM_FEEDBACK_BUFFER 0x8C8E

void r_array_vertex_render(void *in_pool, void *out_pool)
{
	SUIArrayPool *in_p, *out_p;
	in_p = in_pool;
	out_p = out_pool;
	r_array_vertex_atrib_set(in_pool, FALSE);
	glEnable(GL_RASTERIZER_DISCARD);
	r_glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, out_p->gl_buffer);
	r_glBeginTransformFeedback(GL_POINTS);
	if(in_p->array_length < out_p->array_length)
		glDrawArrays(GL_POINTS, 0, in_p->array_length);
	else
		glDrawArrays(GL_POINTS, 0, out_p->array_length);
	r_glEndTransformFeedback();
	glFlush();
	glDisable(GL_RASTERIZER_DISCARD);
}



void r_array_section_draw(void *pool, void *section, RPrimitive primitive_type, uint vertex_start, uint vertex_count)
{
	r_array_draw(pool, section, primitive_type, vertex_start, vertex_count, NULL, NULL, 1);
}

void r_array_reference_draw(void *pool, void *section, RPrimitive primitive_type, uint vertex_start, uint vertex_count)
{
	r_array_draw(pool, section, primitive_type, vertex_start, vertex_count, NULL, NULL, 1); 
}