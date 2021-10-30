/*
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "forge.h"
#include "e_types.h"
#include "e_storage_node.h"

typedef struct {
	VBufferID	buffer_id;
	char		name[16];
	char		*data;
	size_t		length;		/* Exact length of string. */
	size_t		allocated;	/* Allocated space, typically larger than the string held. */
	uint		version;
} ESTextBuffer;

typedef struct {
	ENodeHead	head;
	EDynLookUpTable	buffertables;
	char		language[512];
} ESTextNode;

#define	ALLOC_EXTRA	1024

extern void	e_ns_update_node_version_struct(ESTextNode *node);

char * e_nst_get_language(ESTextNode *t_node)
{
	return t_node->language;
}

ESTextBuffer * e_nst_get_buffer_by_name(ESTextNode *node, char *name)
{
	ESTextBuffer *buffer;
	for(buffer = e_dlut_next(&node->buffertables, 0); buffer != NULL; buffer = e_dlut_next(&node->buffertables, buffer->buffer_id + 1))
		if(strcmp(name, buffer->name) == 0)
			return buffer;
	return NULL;
}

ESTextBuffer *e_nst_get_buffer_by_id(ESTextNode *node,  uint buffer_id)
{
	return e_dlut_get(&node->buffertables, buffer_id);
}

ESTextBuffer *e_nst_get_buffer_next(ESTextNode *node, uint buffer_id)
{
	return e_dlut_next(&node->buffertables, buffer_id);
}

const char * e_nst_get_buffer_data(ESTextNode *node, ESTextBuffer *buffer)
{
	if(node != NULL && buffer != NULL)
	{
		if(buffer->data == NULL)
			verse_send_t_buffer_subscribe(node->head.node_id, buffer->buffer_id);
		return buffer->data;
	}
	return NULL;
}

uint e_nst_get_buffer_data_length(ESTextNode *node, ESTextBuffer *buffer)
{
	return buffer->length;
}

uint e_nst_get_buffer_id(ESTextBuffer *buffer)
{
	return buffer->buffer_id;
}

char * e_nst_get_buffer_name(ESTextBuffer *buffer)
{
	return buffer->name;
}

uint e_nst_get_buffer_version(ESTextBuffer *buffer)
{
	return buffer->version;
}

ESTextNode * e_create_t_node(VNodeID node_id, VNodeOwner owner)
{
	ESTextNode	*node;
	if((node = (ESTextNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		e_dlut_init(&node->buffertables); 
		node->language[0] = 0;
		e_ns_init_head((ENodeHead *)node, V_NT_TEXT, node_id, owner);
	}
	return node;
}

static void delete_text_buffer_func(uint id, void *buffer, void *user_data)
{
	ESTextBuffer	*real_buffer = buffer;

	if(real_buffer->data != NULL)
		free(real_buffer->data);
	free(real_buffer);
}

void delete_text(ESTextNode *node)
{
	e_dlut_remove_foreach(&node->buffertables, delete_text_buffer_func, NULL);
	free(node);
}

static void callback_send_t_language_set(void *user, VNodeID node_id, const char *language)
{
	ESTextNode	*node;
	uint i;
	node = e_create_t_node(node_id, 0);
	for(i = 0; i < 511 && language[i] != 0; i++)
		node->language[i] = language[i];
	node->language[i] = 0;
	e_ns_update_node_version_struct(node);
}

static void callback_send_t_buffer_create(void *user, VNodeID node_id, VBufferID buffer_id, const char *name)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	uint i;
	node = e_create_t_node(node_id, 0);
	if((buffer = e_dlut_get(&node->buffertables, buffer_id)) == NULL)
	{
		buffer = malloc(sizeof *buffer);
		buffer->data = NULL;
		buffer->length = 0;
		buffer->allocated = 0;
		buffer->version = 0;
		buffer->buffer_id = buffer_id;
		e_dlut_add(&node->buffertables, buffer_id, buffer);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		buffer->name[i] = name[i];
	buffer->name[i] = '\0';
	buffer->version++;
	e_ns_update_node_version_struct(node);
}

static void callback_send_t_buffer_destroy(void *user, VNodeID node_id, VBufferID buffer_id)
{
	ESTextNode	*node;
	ESTextBuffer *buffer;
	node = e_create_t_node(node_id, 0);
	if((buffer = e_dlut_get(&node->buffertables, buffer_id)) != NULL)
	{
		e_dlut_remove(&node->buffertables, buffer_id);
		if(buffer->data != NULL)
			free(buffer->data);
		free(buffer);
		e_ns_update_node_version_struct(node);
	}
}

static void text_delete(ESTextBuffer *buf, size_t offset, size_t length)
{
	if(offset > buf->length)
		return;
	if(offset + length > buf->length)
		length = buf->length - offset;
	if(length == 0)
		return;
	memmove(buf->data + offset, buf->data + offset + length, buf->length - (offset + length));
	buf->length -= length;
	buf->data[buf->length] = '\0';
	/* FIXME: There (sh|c)ould be code here to realloc() the buffer *down*, if it shrunk a lot. */
}

static void text_insert(ESTextBuffer *buf, size_t offset, const char *text)
{
	size_t	len;

	if(text == NULL)
		return;
	if(offset > buf->length)
		offset = buf->length;
	len = strlen(text);
	if(len == 0)
		return;

	if(buf->length + len + 1 > buf->allocated)
	{
		char	*nb;

		nb = realloc(buf->data, buf->length + len + ALLOC_EXTRA);
		if(nb == NULL)
		{
			fprintf(stderr, "Enough: Running out of memory in text storage\n");
			return;
		}
		buf->data = nb;
		buf->allocated = buf->length + len + ALLOC_EXTRA;
	}
	memmove(buf->data + offset + len, buf->data + offset, buf->length - offset);
	memcpy(buf->data + offset, text, len);
	buf->length += len;
	buf->data[buf->length] = '\0';
}

static void callback_send_t_text_set(void *user, VNodeID node_id, VBufferID buffer_id, uint32 pos, uint32 length, const char *text)
{
	ESTextNode	*node;
	ESTextBuffer	*buffer;

	node = e_create_t_node(node_id, 0);
	if((buffer = e_dlut_get(&node->buffertables, buffer_id)) != NULL)
	{
		/* This is not optimally efficient for replacement, and also does the delete()
		 * without regard for the subsequent insert(). It is simple, though.
		*/
		text_delete(buffer, pos, length);
		text_insert(buffer, pos, text);
	}
}

void es_text_init(void)
{
	verse_callback_set(verse_send_t_language_set,		callback_send_t_language_set,		NULL);
	verse_callback_set(verse_send_t_buffer_create,		callback_send_t_buffer_create,		NULL);
	verse_callback_set(verse_send_t_buffer_destroy,		callback_send_t_buffer_destroy,		NULL);
	verse_callback_set(verse_send_t_text_set,		callback_send_t_text_set,		NULL);
}
