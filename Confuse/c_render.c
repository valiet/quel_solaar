#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "c_internal.h"
//#include "confuse2.h"
#include "relinquish.h"
/*
typedef struct{
	CDrawStateType type;
	uint attrib_param_count;
	CDrawStateInput *attrib_params;
	uint uniform_param_count;
	CDrawStateInput *uniform_params;
	uint blend_destination;
	uint blend_source;
	boolean depth_test;
	void *buffer;
	uint buffer_stride;
	uint buffer_size;
	uint buffer_used;
	char name[16];
	char *shaders[2];
	char *shader_paths[2];
	boolean injected;
}CDrawState;*/

CDrawState *confuce_draw_state_get_next(CSession *session, CDrawState *previous)
{
	uint id = 0;

/*	{
		uint i;
		for(i = 0; i < session->primitive_count; i++)
			printf("[%u] %u %u %p\n", i, session->prim[i].uniform_used, session->prim[i].attrib_used, session->prim[i].state_array);
	}*/
	if(previous != 0)
		while(&session->prim[id++] != previous);
	while(id < session->primitive_count && (session->prim[id].uniform_used == 0 || session->prim[id].attrib_used == 0 || session->prim[id].state_array == NULL))
	{
		
		id++;
	}

	if(id < session->primitive_count)
		return &session->prim[id];
	return NULL;
}
void *confuce_draw_state_shader_get(CDrawState *state)
{
	return state->state_shader;
}

void *confuce_draw_state_array_get(CDrawState *state)
{
	return state->state_array;
}

void *confuce_draw_state_array_load(CDrawState *state, uint *size) // returns NULL if no array load is needed.
{
	if(state->type == C_DPT_DRAW_CALL_UNIFORM)
		return NULL;
	*size = state->attrib_used;
	return state->attrib_buffer;
}

uint confuce_draw_state_call_count(CDrawState *state)
{
	if(state->type == C_DPT_DRAW_CALL_ATTRIBUTE)
		return 1;
	return state->uniform_used;
}

uint confuce_draw_call_uniform_count(CDrawState *state, uint call_id)
{
	return state->uniform_param_count - C_BUL_COUNT;
}

void *confuce_draw_call_uniform_data(CDrawState *state, uint call_id, uint uniform_id, CTypeType *type_output, uint *location)
{
	float *buf;
	CDrawStateInput *i;
	buf = state->uniform_buffer;
	uniform_id += C_BUL_COUNT;
	i = &state->uniform_params[uniform_id];
	*type_output = i->type;
	*location = i->id;
	return &buf[call_id * state->uniform_stride + state->uniform_params[uniform_id].offset];
}

void  confuse_draw_call_blend_func(CDrawState *state, uint call_id, uint *source, uint *destination)
{
	uint *ibuf;
	ibuf = state->uniform_buffer;
	*source = ibuf[call_id * state->uniform_stride + state->uniform_params[C_BUL_BLEND_SOURCE].offset];
	*destination = ibuf[call_id * state->uniform_stride + state->uniform_params[C_BUL_BLEND_DEST].offset];
}

boolean confuse_draw_call_depth_test(CDrawState *state, uint call_id)
{
	uint *ibuf;
	ibuf = state->uniform_buffer;
	return ibuf[call_id * state->uniform_stride + state->uniform_params[C_BUL_DEPTH_TEST].offset];
}

float *confuse_draw_call_matrix(CDrawState *state, uint call_id)
{
	float *buf;
	buf = state->uniform_buffer;
	return &buf[call_id * state->uniform_stride + state->uniform_params[C_BUL_TRANSFORMATION_MATRIX].offset];
}

void confuse_draw_call_draw_range(CDrawState *state, uint call_id, uint *start, uint *end)
{
	if(state->type == C_DPT_DRAW_CALL_ATTRIBUTE)
	{
		*start = 0;
		*end = state->attrib_used;
		return;
	}else
	{
		uint *ibuf;
		ibuf = state->uniform_buffer;
		ibuf = &ibuf[call_id * state->uniform_stride + state->uniform_params[C_BUL_OBJECT].offset];
		*start = ibuf[1];
		*end = ibuf[2];
		return;
	}
	*start = 0;
	*end = 0;
}
