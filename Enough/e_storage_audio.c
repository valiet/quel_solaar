#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "forge.h"

#include "e_types.h"
#include "e_storage_node.h"

typedef struct {
	VLayerID		buffer_id;
	char			name[16];
	char			*data;
	uint			length;
	uint			allocated;
	real64			frequency;
	uint			version;
	VNABlockType	type;
} ESAudioBuffer;

typedef struct {
	VBufferID		stream_id;
	char			name[16];
	uint			version;
} ESAudioStream;

typedef struct {
	ENodeHead		head;
	EDynLookUpTable	buffertables;
	EDynLookUpTable	streamtables;
} ESAudioNode;

extern void	e_ns_update_node_version_struct(ESAudioNode *node);

ESAudioBuffer * e_nsa_get_buffer_by_name(ESAudioNode *node, char *name)
{
	ESAudioBuffer *buffer;
	for(buffer = e_dlut_next(&node->buffertables, 0); buffer != NULL; buffer = e_dlut_next(&node->buffertables, buffer->buffer_id + 1))
		if(strcmp(name, buffer->name) == 0)
			return buffer;
	return NULL;
}

ESAudioBuffer * e_nsa_get_buffer_by_id(ESAudioNode *node,  uint buffer_id)
{
	return e_dlut_get(&node->buffertables, buffer_id);
}

ESAudioBuffer * e_nsa_get_buffer_next(ESAudioNode *node, uint buffer_id)
{
	return e_dlut_next(&node->buffertables, buffer_id);
}

char * e_nsa_get_buffer_data(ESAudioNode *node, ESAudioBuffer *buffer)
{
	return buffer->data;
}

uint e_nsa_get_buffer_data_length(ESAudioNode *node, ESAudioBuffer *buffer)
{
	return buffer->length;
}

uint e_nsa_get_buffer_id(ESAudioBuffer *buffer)
{
	return buffer->buffer_id;
}

char *e_nsa_get_buffer_name(ESAudioBuffer *buffer)
{
	return buffer->name;
}

VNABlockType e_nsa_get_buffer_type(ESAudioBuffer *buffer)
{
	return buffer->type;
}

double e_nsa_get_buffer_frequency(ESAudioBuffer *buffer)
{
	return buffer->frequency;
}

uint e_nsa_get_buffer_version(ESAudioBuffer *buffer)
{
	return buffer->version;
}

ESAudioStream * e_nsa_get_stream_by_name(ESAudioNode *node, char *name)
{
	ESAudioStream *stream;
	for(stream = e_dlut_next(&node->streamtables, 0); stream != NULL; stream = e_dlut_next(&node->streamtables, stream->stream_id + 1))
		if(strcmp(name, stream->name) == 0)
			return stream;
	return NULL;
}

ESAudioBuffer * e_nsa_get_stream_by_id(ESAudioNode *node,  uint stream_id)
{
	return e_dlut_get(&node->streamtables, stream_id);
}

ESAudioBuffer * e_nsa_get_stream_next(ESAudioNode *node, uint stream_id)
{
	return e_dlut_next(&node->streamtables, stream_id);
}


uint e_nsa_get_stream_id(ESAudioStream *stream)
{
	return stream->stream_id;
}

char * e_nsa_get_stream_name(ESAudioStream *stream)
{
	return stream->name;
}

static uint e_nsa_get_stream_version(ESAudioStream *stream)
{
	return stream->version;
}

ESAudioNode * e_create_a_node(VNodeID node_id, VNodeOwner owner)
{
	ESAudioNode *node;
	if((node = (ESAudioNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		e_dlut_init(&node->buffertables); 
		e_dlut_init(&node->streamtables); 
		e_ns_init_head((ENodeHead *)node, V_NT_AUDIO, node_id, owner);
	}
	return node;
}

static void delete_audio_buffer_func(uint id, void *buffer, void *user_data)
{
	ESAudioBuffer	*real_buffer = buffer;

	if(real_buffer->data != NULL)
		free(real_buffer->data);
	free(real_buffer);
}

static void delete_audio_stream_func(uint id, void *buffer, void *user_data)
{
	free(buffer);
}

void delete_audio(ESAudioNode *node)
{
	e_dlut_remove_foreach(&node->buffertables, delete_audio_buffer_func, NULL);
	e_dlut_remove_foreach(&node->streamtables, delete_audio_stream_func, NULL);
	free(node);
}

static void callback_send_a_buffer_create(void *user_data, VNodeID node_id, VLayerID buffer_id, const char *name, VNABlockType type, real64 frequency)
{
	ESAudioNode	*node;
	ESAudioBuffer	*buffer;
	uint i;
	node = e_create_a_node(node_id, 0);
	if((buffer = e_dlut_get(&node->buffertables, buffer_id)) != NULL)
	{
		if(buffer->type != type)
		{
			if(buffer->data)
				free(buffer->data);
			buffer->data = NULL;
		}
	}else
	{
		buffer = malloc(sizeof *buffer);
		buffer->data = NULL;
		e_dlut_add(&node->buffertables, buffer_id, buffer);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		buffer->name[i] = name[i];
	buffer->name[i] = '\0';
	buffer->buffer_id = buffer_id;
	buffer->frequency = frequency;
	buffer->type = type;
	buffer->version++;
	e_ns_update_node_version_struct(node);
}

static void callback_send_a_buffer_destroy(void *user_data, VNodeID node_id, VLayerID buffer_id)
{
	ESAudioNode		*node;
	ESAudioBuffer	*buffer;
	node = e_create_a_node(node_id, 0);
	if((buffer = e_dlut_get(&node->buffertables, buffer_id)) != NULL)
	{
		e_dlut_remove(&node->buffertables, buffer_id);
		if(buffer->data != NULL)
			free(buffer->data);
		free(buffer);
	}
	e_ns_update_node_version_struct(node);
}

static void callback_send_a_stream_create(void *user_data, VNodeID node_id, VLayerID stream_id, const char *name)
{
	ESAudioNode		*node;
	ESAudioStream	*stream;
	uint i;
	node = e_create_a_node(node_id, 0);
	if((stream = e_dlut_get(&node->streamtables, stream_id)) == NULL)
	{
		stream = malloc(sizeof *stream);
		e_dlut_add(&node->streamtables, stream_id, stream);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		stream->name[i] = name[i];
	stream->name[i] = '\0';
	stream->stream_id = stream_id;
	stream->version++;
	e_ns_update_node_version_struct(node);
}

static void callback_send_a_stream_destroy(void *user_data, VNodeID node_id, VLayerID stream_id)
{
	ESAudioNode		*node;
	ESAudioStream	*stream;
	node = e_create_a_node(node_id, 0);
	if((stream = e_dlut_get(&node->streamtables, stream_id)) != NULL)
	{
		e_dlut_remove(&node->streamtables, stream_id);
		free(stream);
	}
	e_ns_update_node_version_struct(node);
}


void es_audio_init(void)
{
	verse_callback_set(verse_send_a_buffer_destroy,		callback_send_a_buffer_destroy,		NULL);
	verse_callback_set(verse_send_a_buffer_create,		callback_send_a_buffer_create,		NULL);
	verse_callback_set(verse_send_a_stream_destroy,		callback_send_a_stream_destroy,		NULL);
	verse_callback_set(verse_send_a_stream_create,		callback_send_a_stream_create,		NULL);
}
