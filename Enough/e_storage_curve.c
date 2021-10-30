
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "forge.h"

#include "e_types.h"
#include "e_storage_node.h"

typedef float TBChanel;

typedef struct{
	real64 pre_value[4];
	uint32 pre_pos[4];
	real64 value[4];
	real64 pos;
	real64 post_value[4];
	uint32 post_pos[4];
	uint32 sorted_pos;
	uint32 sorted;
}ESCurvePoint;


typedef struct{
	uint16			curve_id;
	char			name[16];
	uint			version;
	uint			dimensions;
	ESCurvePoint	*points;
	uint32			length;
	uint32			allocated;
}ESCurve;

typedef struct{
	ENodeHead		head;
	EDynLookUpTable	curvetables;
}ESCurveNode;

extern void	e_ns_update_node_version_struct(ESCurveNode *node);
extern void	e_ns_update_node_version_data(ESCurveNode *node);

ESCurve *e_nsc_get_curve_by_name(ESCurveNode *node, const char *name)
{
	ESCurve *curve;
	for(curve =	e_dlut_next(&node->curvetables, 0); curve != NULL; curve = e_dlut_next(&node->curvetables, curve->curve_id + 1))
		if(strcmp(name, curve->name) == 0)
			return curve;
	return NULL;
}

ESCurve *e_nsc_get_curve_by_id(ESCurveNode *node, uint curve_id)
{
	return e_dlut_get(&node->curvetables, curve_id);
}

ESCurve *e_nsc_get_curve_next(ESCurveNode *node, uint curve_id)
{
	return e_dlut_next(&node->curvetables, curve_id);
}

uint e_nsc_get_curve_id(ESCurve *curve)
{
	return curve->curve_id;
}

char *e_nsc_get_curve_name(ESCurve *curve)
{
	return curve->name;
}

uint e_nsc_get_curve_version(ESCurve *curve)
{
	return curve->version;
}

uint e_nsc_get_curve_dimensions(ESCurve *curve)
{
	return curve->dimensions;
}

uint e_nsc_get_curve_point_count(ESCurve *curve)
{
	return curve->length;
}

uint e_nsc_get_point_order(ESCurve *curve, uint point_nr)
{
	return curve->points[point_nr].sorted;
}

uint e_nsc_get_point_pos(ESCurve *curve, uint point_id)
{
	return curve->points[point_id].sorted_pos;
}

ESCurveNode *e_create_c_node(VNodeID node_id, VNodeOwner owner)
{
	ESCurveNode	*node;
	if((node = (ESCurveNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		e_dlut_init(&node->curvetables); 
		e_ns_init_head((ENodeHead *)node, V_NT_CURVE, node_id, owner);
	}
	return node;
}


void delete_curve_curves_func(uint id, void *curve, void *user_data)
{
	ESCurve	*real_curve = curve;

	if(real_curve->points != NULL)
		free(real_curve->points);
	free(real_curve);
}

void delete_curve(ESCurveNode *node)
{
	e_dlut_remove_foreach(&node->curvetables, delete_curve_curves_func, NULL);
	free(node);
}


static void callback_send_c_curve_create(void *user, VNodeID node_id, VLayerID curve_id, const char *name, uint8 dimensions)
{
	ESCurveNode	*node;
	ESCurve *curve;
	uint i;
	node = e_create_c_node(node_id, 0);
	if((curve = e_dlut_get(&node->curvetables, curve_id)) != NULL)
	{
		if(curve->dimensions != dimensions)
		{
			if(curve->points)
				free(curve->points);
			curve->points = NULL;
			curve->allocated = 0;
			curve->length = 0;
		}
	}else
	{
		curve = malloc(sizeof *curve);
		curve->points = NULL;
		curve->allocated = 0;
		curve->length = 0;
		e_dlut_add(&node->curvetables, curve_id, curve);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		curve->name[i] = name[i];
	curve->name[i] = 0;
	curve->dimensions = dimensions;
	curve->curve_id = curve_id;
	curve->version++;
	verse_send_c_curve_subscribe(node_id, curve_id);
	e_ns_update_node_version_struct(node);
}

void callback_send_c_curve_destroy(void *user_data, VNodeID node_id, VLayerID curve_id)
{
	ESCurveNode	*node;
	ESCurve	*curve;
	node = e_create_c_node(node_id, 0);
	if((curve = e_dlut_get(&node->curvetables, curve_id)) != NULL)
	{
		e_dlut_remove(&node->curvetables, curve_id);
		if(curve->points != NULL)
			free(curve->points);
		free(curve);
	}
	e_ns_update_node_version_struct(node);
}
/*
}ESCurvePoint;

typedef struct{
	real64 *pre_value[4];
	uint32 *pre_pos[4];
	real64 *value[4];
	real64 pos;
	real64 *post_value[4];
	uint32 *post_pos[4];
	uint32 sorted_pos;
	uint32 sorted;
}ESCurvePoint;

typedef struct{
	uint16			curve_id;
	uint			version;
	uint			dimensions;
	ESCurvePoint	*points;
	uint32			length;
	uint32			allocated;
}ESCurve;
*/
void callback_send_c_key_set(void *user_data, VNodeID node_id, VLayerID curve_id, uint32 key_id, uint8 dimensions, real64 *pre_value, uint32 *pre_pos, real64 *value, real64 pos, real64 *post_value, uint32 *post_pos)
{
	ESCurveNode		*node;
	ESCurve			*curve;
	ESCurvePoint	*point;
	uint			i;

	node = e_create_c_node(node_id, 0);
	curve = e_dlut_get(&node->curvetables, curve_id);
	if(curve == NULL)
		return;
	if(key_id >= curve->allocated)
	{
		curve->points = realloc(curve->points, (sizeof *curve->points) * (key_id + 16));
		for(i = curve->allocated; i < (key_id + 16); i++)
			curve->points[i].sorted_pos = -1;
		curve->allocated = key_id + 16;
	}

	point = &curve->points[key_id];
	for(i = 0; i < dimensions; i++)
	{
		point->pre_value[i] = pre_value[i];
		point->pre_pos[i] = pre_pos[i];
		point->value[i] = value[i];
		point->post_value[i] = post_value[i];
		point->post_pos[i] = post_pos[i];
	}

	if(point->sorted_pos == -1)
	{
		for(i = curve->length; i != 0 && pos < curve->points[curve->points[i - 1].sorted].pos; i--)
		{
			curve->points[i].sorted = curve->points[i - 1].sorted;
			curve->points[curve->points[i].sorted].sorted_pos++;
		}
		curve->points[i].sorted = key_id;
		curve->points[key_id].sorted_pos = i;
		point->pos = pos;
		curve->length++;

	}else
	{
		if(point->pos < pos)
		{
			for(i = curve->points[key_id].sorted_pos; i < curve->length - 1 ; i++)
			{
				if(curve->points[curve->points[i + 1].sorted].pos > pos)
					break;
				curve->points[i].sorted = curve->points[i + 1].sorted;
				curve->points[curve->points[i].sorted].sorted_pos = i;
			}
		}else
		{
			for(i = curve->points[key_id].sorted_pos; i != 0; i--)
			{
				if(curve->points[curve->points[i - 1].sorted].pos < pos)
					break;
				curve->points[i].sorted = curve->points[i - 1].sorted;
				curve->points[curve->points[i].sorted].sorted_pos = i;
			}
		}
		curve->points[i].sorted = key_id;
		curve->points[key_id].sorted_pos = i;
		point->pos = pos;
	}

	curve->version++;
	e_ns_update_node_version_data(node);
}

void callback_send_c_key_destroy(void *user_data, VNodeID node_id, VLayerID curve_id, uint32 key_id)
{
	ESCurveNode		*node;
	ESCurve			*curve;
	ESCurvePoint	*point;
	uint			i;

	node = e_create_c_node(node_id, 0);
	curve = e_dlut_get(&node->curvetables, curve_id);
	if(curve == NULL)
		return;
	if(key_id >= curve->allocated)
		return;

	point = &curve->points[key_id];
	for(i = 0; i <  curve->allocated; i++)
		if(curve->points[i].sorted_pos != -1 && curve->points[i].sorted_pos > point->sorted_pos)
			curve->points[i].sorted_pos--;
	curve->length--;
	for(i = point->sorted_pos; i < curve->length; i++)
		curve->points[i].sorted = curve->points[i + 1].sorted;
	point->sorted_pos = -1;

	curve->version++;
	e_ns_update_node_version_data(node);
}



void es_curve_init(void)
{
	verse_callback_set(verse_send_c_curve_create,		callback_send_c_curve_create,		NULL);
	verse_callback_set(verse_send_c_curve_destroy,		callback_send_c_curve_destroy,		NULL);
	verse_callback_set(verse_send_c_key_set,			callback_send_c_key_set,		NULL);
	verse_callback_set(verse_send_c_key_destroy,		callback_send_c_key_destroy,	NULL);
}

uint e_nsc_get_point_next(ESCurve *curve, uint point_id)
{
	for(;point_id < curve->allocated && curve->points[point_id].sorted_pos == -1; point_id++);
	if(point_id == curve->allocated)
		return -1;
	return point_id;
}

void e_nsc_get_point(ESCurve *curve, uint point_id, real64 *pre_value, uint32 *pre_pos, real64 *value, real64 *pos, real64 *post_value, uint32 *post_pos)
{
	ESCurvePoint *point;
	uint i;
	point = &curve->points[point_id];
	if(pre_value != NULL)
		for(i = 0; i < curve->dimensions; i++)
			pre_value[i] = point->pre_value[i];
	if(pre_pos != NULL)
		for(i = 0; i < curve->dimensions; i++)
			pre_pos[i] = point->pre_pos[i];
	if(value != NULL)
		for(i = 0; i < curve->dimensions; i++)
			value[i] = point->value[i];
	if(pos != NULL)
		*pos = point->pos;
	if(post_value != NULL)
		for(i = 0; i < curve->dimensions; i++)
			post_value[i] = point->post_value[i];
	if(post_pos != NULL)
		for(i = 0; i < curve->dimensions; i++)
			post_pos[i] = point->post_pos[i];
}

void e_nsc_get_point_double(ESCurve *curve, uint point_id, real64 *pre_value, real64 *pre_pos, real64 *value, real64 *pos, real64 *post_value, real64 *post_pos)
{
	ESCurvePoint *point; 
	double dist[2] = {-1, 1};
	uint i;
	point = &curve->points[point_id];

	if(curve->points[point_id].sorted_pos > 0)
		dist[0] = curve->points[curve->points[curve->points[point_id].sorted_pos - 1].sorted].pos - point->pos; 

	if(curve->points[point_id].sorted_pos < curve->length - 1)
		dist[1] = curve->points[curve->points[curve->points[point_id].sorted_pos + 1].sorted].pos - point->pos; 


	if(pre_value != NULL)
		for(i = 0; i < curve->dimensions; i++)
			pre_value[i] = point->value[i] + point->pre_value[i] * -dist[0];
	if(pre_pos != NULL)
		for(i = 0; i < curve->dimensions; i++)
			pre_pos[i] = point->pos + point->pre_pos[i] / (double)(0xffffffff) * dist[0];
	if(value != NULL)
		for(i = 0; i < curve->dimensions; i++)
			value[i] = point->value[i];
	if(pos != NULL)
		*pos = point->pos;
	if(post_value != NULL)
		for(i = 0; i < curve->dimensions; i++)
			post_value[i] = point->value[i] + point->post_value[i] * dist[1];
	if(post_pos != NULL)
		for(i = 0; i < curve->dimensions; i++)
			post_pos[i] = point->pos + point->post_pos[i] / (double)(0xffffffff) * dist[1];
}

void e_nsc_send_c_key_set(ESCurveNode *node, ESCurve *curve, uint32 key_id, real64 *pre_value, real64 *pre_pos, real64 *value, real64 *pos, real64 *post_value, real64 *post_pos)
{
	ESCurvePoint	*point;
	double dist[2] = {-1, 1}, f, prev[4], postv[4];
	uint32 prep[4], postp[4];
	uint8 dimensions;
	uint i;

	dimensions = e_nsc_get_curve_dimensions(curve);

	point = &curve->points[key_id];

	if(pos == NULL)
		pos = &point->pos;

	if(curve->points[key_id].sorted_pos > 0)
		dist[0] = curve->points[curve->points[curve->points[key_id].sorted_pos - 1].sorted].pos - *pos; 

	if(curve->points[key_id].sorted_pos < curve->length - 1)
		dist[1] = curve->points[curve->points[curve->points[key_id].sorted_pos + 1].sorted].pos - *pos; 

	if(value == NULL)
		value = point->value;


	if(pre_value != NULL)
	{
		for(i = 0; i < curve->dimensions; i++)
			prev[i] = (pre_value[i] - value[i]) / -dist[0];
	}else
		for(i = 0; i < curve->dimensions; i++)
			prev[i] = point->pre_value[i];

	if(post_value != NULL)
	{
		for(i = 0; i < curve->dimensions; i++)
			postv[i] = (post_value[i] - value[i]) / dist[1];
	}else
		for(i = 0; i < curve->dimensions; i++)
			postv[i] = point->post_value[i];

	if(pre_pos != NULL)
	{
		for(i = 0; i < curve->dimensions; i++)
		{
			f = ((*pos - pre_pos[i]) / -dist[0]);
			if(f < 0)
				prep[i] = 0;
			else if(f > 1)
				prep[i] = 0xffffffff;
			else
				prep[i] = f * 0xffffffff;
		}
	}else
		for(i = 0; i < curve->dimensions; i++)
			prep[i] = point->pre_pos[i];

	if(post_pos != NULL)
	{
		for(i = 0; i < curve->dimensions; i++)
		{
			f = (*pos - post_pos[i]) / -dist[1];
			if(f < 0)
				postp[i] = 0;
			else if(f > 1)
				postp[i] = 0xffffffff;
			else
				postp[i] = f * 0xffffffff;
		}
	}else
		for(i = 0; i < curve->dimensions; i++)
			postp[i] = point->post_pos[i];

	verse_send_c_key_set(node->head.node_id, e_nsc_get_curve_id(curve), key_id, dimensions, prev, prep, value, *pos, postv, postp);
}


double e_nsc_evaluate_curve(ESCurve	*curve, double *output, double pos)
{
	uint32 before = 0, after, i;
	after = curve->length - 1;

	while(before + 1 != after)
	{
		if(curve->points[curve->points[(after + before) / 2].sorted_pos].pos > pos)
			after = (after + before) / 2;
		else
			before = (after + before) / 2;
	}
	before = curve->points[before].sorted_pos;
	after = curve->points[after].sorted_pos;
	pos = (pos - curve->points[before].pos) / (curve->points[after].pos - curve->points[before].pos);
	if(output != NULL)
	{
		if(curve->dimensions == 1)
		{
			output[0] = curve->points[before].value[0] * (1 - pos) + curve->points[before].value[0] * pos;
			output[3] =	output[2] =	output[1] =	output[0];
		}else
		{
			for(i = 0; i < curve->dimensions; i++)
				output[i] = curve->points[before].value[i] * (1 - pos) + curve->points[before].value[i] * pos;
			for(; i < 4; i++)
				output[i] = 0;
		}
		return output[0];
	}else
		return curve->points[before].value[0] * (1 - pos) + curve->points[before].value[0] * pos;
}

void e_nsc_get_segment(ESCurve *curve, uint segment_nr, uint axis, real64 *point_0, real64 *point_1, real64 *point_2, real64 *point_3)
{
	ESCurvePoint *point_a,  *point_b;
	double dist = 1;
	point_a = &curve->points[curve->points[segment_nr].sorted];
	point_b = &curve->points[curve->points[segment_nr + 1].sorted];

	dist = point_b->pos - point_a->pos;

	point_0[0] = point_a->pos;
	point_0[1] = point_a->value[axis];

	point_1[0] = point_a->pos + (double)point_a->post_pos[axis] / (double)(0xffffffff) * dist;
	point_1[1] = point_a->value[axis] + point_a->post_value[axis] * dist;

	point_2[0] = point_b->pos - (double)point_b->pre_pos[axis] / (double)(0xffffffff) * dist;
	point_2[1] = point_b->value[axis] + point_b->pre_value[axis] * dist;

	point_3[0] = point_b->pos;
	point_3[1] = point_b->value[axis];
}
