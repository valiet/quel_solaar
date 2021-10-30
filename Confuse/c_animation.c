#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "confuse.h"

uint c_character_event_anim_over_ride = -1;
float c_character_event_anim_over_ride_time = 0;
uint c_space_base[C_APC_COUNT] = {C_CM_BASE, C_CM_BASE, C_CM_BASE, C_CM_BASE, C_CM_BASE, C_CM_BASE, C_CM_BASE, C_CM_HEAD, C_CM_HEAD, -1, C_CM_TOOL, C_CM_TOOL};
char *c_point_name[C_APC_COUNT] = {"Base spine", "Top spine", "Mid spine", "Left hand", "Right hand", "Left elbow", "Right elbow", "Head up", "Head forward", "Tool pos", "Tool forward", "Tool up"};


extern boolean ca_character_stroke_rec;
extern float *ca_character_stroke;
extern uint ca_character_stroke_length;

void c_character_to_matrix(float *output, CCharacter *c, CAnimPoint *point, uint value_id)
{
	float size, tmp[3], *matrix;
	if(point->space >= C_CM_COUNT)
	{
		output[0] = point->pos[0];
		output[1] = point->pos[1];
		output[2] = point->pos[2];
		return;
	}
	matrix = c->matrix[point->space];
	if((point->space == C_CM_LOOK /*|| point->space == C_CM_BASE*/) && c_space_base[value_id] != -1)
	{
		uint space;
		space = c_space_base[value_id];
		if(space == C_CM_TORSO/* && point->space == C_CM_BASE*/)
			space = C_CM_BASE;
		tmp[0] = point->pos[0] - c->matrix[space][12];
		tmp[1] = point->pos[1] - c->matrix[space][13];
		tmp[2] = point->pos[2] - c->matrix[space][14];
	}else
	{
		tmp[0] = point->pos[0];
		tmp[1] = point->pos[1];
		tmp[2] = point->pos[2];
	}
	size = matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2];
	tmp[0] = (tmp[0] - matrix[12]) / size;
	tmp[1] = (tmp[1] - matrix[13]) / size;
	tmp[2] = (tmp[2] - matrix[14]) / size;
	output[0] = tmp[0] * matrix[0] + tmp[1] * matrix[1] + tmp[2] * matrix[2];
	output[1] = tmp[0] * matrix[4] + tmp[1] * matrix[5] + tmp[2] * matrix[6];
	output[2] = tmp[0] * matrix[8] + tmp[1] * matrix[9] + tmp[2] * matrix[10];
}

void c_character_from_matrix(float *output, CCharacter *c, CAnimPoint *point, uint value_id, boolean wiggle)
{
	static float point_scale[C_APC_COUNT] = {0.1, 0.3, 0.2, -0.4, -0.4, -0.4, -0.4, -0.4, 0.5, 0, 0, -0.3};
	float tmp[3], tmp2[3], *matrix;
	if(point->space >= C_CM_COUNT)
	{
	/*int *a = NULL;
		a[0] = 0;*/
		output[0] = point->pos[0];
		output[1] = point->pos[1];
		output[2] = point->pos[2];

		return;
	}
	matrix = c->matrix[point->space];
	tmp[0] = point->pos[0];
	tmp[1] = point->pos[1];
	tmp[2] = point->pos[2];
	output[0] = tmp[0] * matrix[0] + tmp[1] * matrix[4] + tmp[2] * matrix[8] + matrix[12];
	output[1] = tmp[0] * matrix[1] + tmp[1] * matrix[5] + tmp[2] * matrix[9] + matrix[13];
	output[2] = tmp[0] * matrix[2] + tmp[1] * matrix[6] + tmp[2] * matrix[10] + matrix[14];
	if((point->space == C_CM_LOOK /*|| point->space == C_CM_BASE*/) && c_space_base[value_id] != -1)
	{
		output[0] += c->matrix[c_space_base[value_id]][12];
		output[1] += c->matrix[c_space_base[value_id]][13];
		output[2] += c->matrix[c_space_base[value_id]][14];
	}
	;
	if(point->space == C_CM_BASE && wiggle)
	{
		f_wiggle3df(tmp, c->wiggle_speed, c->wiggle);
		f_wiggle3df(tmp2, c->wiggle_speed + (float)value_id * 32.1, c->wiggle);
		output[0] += (tmp[0] + tmp2[0]) * point_scale[value_id];
		output[1] += (tmp[1] + tmp2[1]) * point_scale[value_id];
		output[2] += (tmp[2] + tmp2[2]) * point_scale[value_id];
	}
}


uint c_character_desc_key_create(CCharDesc *desc, uint event, uint value_id, float time, uint space, float *pos)
{
	uint i, found;
	if(desc->events[event].anim[value_id].point_count % 16 == 0)
		desc->events[event].anim[value_id].points = realloc(desc->events[event].anim[value_id].points, (sizeof *desc->events[event].anim[value_id].points) * (desc->events[event].anim[value_id].point_count + 16));

	for(i = 0; i < desc->events[event].anim[value_id].point_count && 
		desc->events[event].anim[value_id].points[i].time < time; i++);
	found = i;
	for(i = desc->events[event].anim[value_id].point_count; i != found; i--)
		desc->events[event].anim[value_id].points[i] = desc->events[event].anim[value_id].points[i - 1];
	desc->events[event].anim[value_id].points[found].space = space;
	desc->events[event].anim[value_id].points[found].time = time;
	desc->events[event].anim[value_id].points[found].pos[0] = pos[0];
	desc->events[event].anim[value_id].points[found].pos[1] = pos[1];
	desc->events[event].anim[value_id].points[found].pos[2] = pos[2];
	desc->events[event].anim[value_id].point_count++;
	return found;
}


void c_character_desc_key_destroy(CCharDesc *desc, uint event, uint value_id, uint key)
{
	uint i;
	if(desc->events[event].anim[value_id].point_count <= 1)
		return;
	desc->events[event].anim[value_id].point_count--;
	for(i = key; i < desc->events[event].anim[value_id].point_count; i++)
		desc->events[event].anim[value_id].points[i] = desc->events[event].anim[value_id].points[i + 1];
}

uint c_character_desc_event_create(CCharDesc *desc)
{
	uint i;
	char *name = "clip";
	if(desc->event_count % 16 == 0)
		desc->events = realloc(desc->events, (sizeof *desc->events) * (desc->event_count + 16));
	for(i = 0; i < C_APC_COUNT; i++)
	{
		desc->events[desc->event_count].anim[i].points = NULL;
		desc->events[desc->event_count].anim[i].point_count = 0; 
	}
	desc->events[desc->event_count].speed = 3.0;
	desc->events[desc->event_count].start= 0.2;
	desc->events[desc->event_count].end = 0.8;
	desc->events[desc->event_count].loop = FALSE;
	for(i = 0; i < 32 && name[i] != 0; i++)
		desc->events[desc->event_count].name[i] = name[i];
	desc->events[desc->event_count].name[i] = 0;

	desc->event_count++;
	return desc->event_count - 1;
}

void c_character_desc_event_delete(CCharDesc *desc, uint event)
{
	uint i;
	desc->event_count--;
	for(i = event; i < desc->event_count; i++)
		desc->events[i] = desc->events[i + 1];

}


void c_character_from_matrix_animation(float *output, CCharacter *c, CAnimEvent *e, uint value_id, float time)
{
	CAnimPoints *points;
	points = &e->anim[value_id];

	if(points->point_count < 2)
		c_character_from_matrix(output, c, points->points, value_id, TRUE);
	else
	{
		float t0, t1, p0[3], p1[3];
		uint i;
		for(i = 0; i < points->point_count && points->points[i].time < time; i++);
		if(e->loop)
		{
			if(i != points->point_count)
				t1 = points->points[i].time;
			else
				t1 = points->points[0].time + 1.0;

			if(i != 0)
				t0 = points->points[i - 1].time;
			else
				t0 = points->points[points->point_count - 1].time - 1.0;
			time = (time - t0) / (t1 - t0);
			c_character_from_matrix(p0, c, &points->points[(i + points->point_count - 1) % points->point_count], value_id, TRUE);
			c_character_from_matrix(p1, c, &points->points[i % points->point_count], value_id, TRUE);
			output[0] = p0[0] + time * (p1[0] - p0[0]);
			output[1] = p0[1] + time * (p1[1] - p0[1]);
			output[2] = p0[2] + time * (p1[2] - p0[2]);
		}else
		{
			if(i != points->point_count)
				t1 = points->points[i].time;
			else
			{
				c_character_from_matrix(output, c, &points->points[points->point_count - 1], value_id, TRUE);
				return;
			}
			if(i != 0)
				t0 = points->points[i - 1].time;
			else
			{
				c_character_from_matrix(output, c, &points->points[0], value_id, TRUE);
				return;
			}
			time = (time - t0) / (t1 - t0);
			c_character_from_matrix(p0, c, &points->points[(i + points->point_count - 1) % points->point_count], value_id, TRUE);
			c_character_from_matrix(p1, c, &points->points[i % points->point_count], value_id, TRUE);
			output[0] = p0[0] + time * (p1[0] - p0[0]);
			output[1] = p0[1] + time * (p1[1] - p0[1]);
			output[2] = p0[2] + time * (p1[2] - p0[2]);
		}

	}
}


float c_compute_spline(float f, float v0, float v1, float v2, float v3)
{
	return ((v0 * f + v1 * (1 - f)) * f + (v1 * f + v2 * (1 - f)) * (1 - f)) * f +	((v1 * f + v2 * (1 - f)) * f + (v2 * f + v3 * (1 - f)) * (1 - f)) * (1 - f);
}

void c_character_event_anim(CCharDesc *desc, CCharacter *c, uint tool_count, uint *tools, float *tool_matrix, uint selected_tool, float delta)
{
	static float time = 0, timer = 0;
	CAnimEvent *e;
	float t, default_pos[C_APC_COUNT][3], zero[3] = {0, 0, 0}, pos[3], wiggle[3];
	uint i, j;

	c->wiggle = desc->wiggle * desc->scale * 0.5;
	c->wiggle_time += delta;
	t = 0.5 + 0.5 * f_noisef(c->wiggle_time);
	t = 0.01 + 0.5 * t * t;
	c->wiggle_speed += delta * t;

	if(c_character_event_anim_over_ride == -1)
	{
		if(c->tool_last == -1 && c->tool_current == -1)
		{
			for(i = 0; i < C_APC_COUNT; i++)
			{
				c_character_from_matrix(default_pos[i], c, &desc->default_pose[i], i, TRUE);
				c->points[i].pos[0] = default_pos[i][0];
				c->points[i].pos[1] = default_pos[i][1];
				c->points[i].pos[2] = default_pos[i][2];
			}
			if(selected_tool == -1)
				c->tool_current = -1;
			else
				c->tool_current = tools[selected_tool];
		}else if(c->tool_last == c->tool_current) // holding and rising the tool
		{
			if(c->tool_current == tools[selected_tool])
			{
				c->tool_timer += delta;
				if(c->tool_timer > 1.0)
					c->tool_timer = 1.0;
			}else
			{
				c->tool_timer -= delta;
				if(c->tool_timer < 0.0)
				{
					if(selected_tool == -1)
						c->tool_current = -1;
					else
						c->tool_current = tools[selected_tool];
					c->tool_timer = 0.0;
				}
			}
			e = &desc->events[c->tool_last];
			for(i = 0; i < C_APC_COUNT; i++)
			{
				if(e->anim[i].point_count > 0)
					c_character_from_matrix_animation(default_pos[i], c, e, i, c->tool_timer);
				else
					c_character_from_matrix(default_pos[i], c, &desc->default_pose[i], i, TRUE);
				c->points[i].pos[0] = default_pos[i][0];
				c->points[i].pos[1] = default_pos[i][1];
				c->points[i].pos[2] = default_pos[i][2];
			}
		}else // moving towards the next tool
		{
			for(i = 0; i < C_APC_COUNT; i++)
			{
				if(c->tool_last != -1 && desc->events[c->tool_last].anim[i].point_count > 0)
					c_character_from_matrix_animation(default_pos[i], c, &desc->events[c->tool_last], i, 0.0/*c->tool_timer*/);
				else
					c_character_from_matrix(default_pos[i], c, &desc->default_pose[i], i, TRUE);

				if(c->tool_current != -1 && desc->events[c->tool_current].anim[i].point_count > 0)
					c_character_from_matrix_animation(pos, c, &desc->events[c->tool_current], i, 0.0/*c->tool_timer*/);
				else
					c_character_from_matrix(pos, c, &desc->default_pose[i], i, TRUE);

				c->points[i].pos[0] = default_pos[i][0] * (1.0 - c->tool_timer) + pos[0] * c->tool_timer;
				c->points[i].pos[1] = default_pos[i][1] * (1.0 - c->tool_timer) + pos[1] * c->tool_timer;
				c->points[i].pos[2] = default_pos[i][2] * (1.0 - c->tool_timer) + pos[2] * c->tool_timer;
			}

			c->tool_timer += delta;
			if(c->tool_timer > 1.0)				
			{
				c->tool_last = c->tool_current;
				c->tool_timer = 0.0;
			}
		}


		for(i = 0; i < c->event_count; i++)
		{
			e = &desc->events[c->event[i].id];
			c->event[i].timer = c->event[i].timer + delta / e->speed;

			if(e->loop)
			{
				if(c->event[i].stop)
					c->event[i].influence -= delta;
				else
					c->event[i].influence += delta;
				if(c->event[i].influence > 1.0)
					c->event[i].influence = 1.0;
				t = c->event[i].influence;
				if(c->event[i].timer > 1.0)
					c->event[i].timer -= 1.0;
			}else
			{
				if(c->event[i].timer < e->start)
					t = c_compute_spline(c->event[i].timer / e->start, 1, 1, 0, 0);
				else if(c->event[i].timer < e->end)
					t = 1.0;
				else 
					t = c_compute_spline(1.0 - (c->event[i].timer - e->end) / (1.0 - e->end), 1, 1, 0, 0);
			}

			if((c->event[i].timer > 1.0 && !e->loop) ||
				(c->event[i].influence <= 0.0 && e->loop))
			{
				c->event_count--;
				for(j = i; j < c->event_count; j++)
					c->event[j] = c->event[j + 1];
				i--;
			}else
			{
				for(j = 0; j < C_APC_COUNT; j++)
				{
					if(e->anim[j].point_count > 0)
					{
						c_character_from_matrix_animation(pos, c, e, j, c->event[i].timer);
						c->points[j].pos[0] += (pos[0] - default_pos[j][0]) * t;
						c->points[j].pos[1] += (pos[1] - default_pos[j][1]) * t;
						c->points[j].pos[2] += (pos[2] - default_pos[j][2]) * t;
					}
				}
			}
		}
	}else
	{
		for(i = 0; i < C_APC_COUNT; i++)
		{
			c_character_from_matrix(c->points[i].pos, c, &desc->default_pose[i], i, TRUE);
		}
		e = &desc->events[c_character_event_anim_over_ride];
		for(i = 0; i < C_APC_COUNT; i++)
		{
			if(e->anim[i].point_count == 0)
			{
				c_character_from_matrix(default_pos[i], c, &desc->default_pose[i], i, TRUE);
				c->points[i].pos[0] = default_pos[i][0];
				c->points[i].pos[1] = default_pos[i][1];
				c->points[i].pos[2] = default_pos[i][2];
			}else
				c_character_from_matrix_animation(c->points[i].pos, c, e, i, c_character_event_anim_over_ride_time);
		}

	}
}

void c_character_event_play(CCharacter *c, uint id)
{
	uint i;
	for(i = 0; i < c->event_count && c->event[i].id != id; i++);
	if(i == c->event_count && c->event_count < 16)
	{
		c->event[c->event_count].id = id;
		c->event[c->event_count].timer = 0.0;
		c->event[c->event_count].influence = 0.0;
		c->event[c->event_count].stop = FALSE;
		c->event_count++;
	}
}

void c_character_event_stopp(CCharacter *c, uint id)
{
	uint i;
	for(i = 0; i < c->event_count; i++)
	{
		if(c->event[i].id == id)
		{
			c->event[i].stop = TRUE;
			return;
		}
	}
}

uint c_character_event_get_id(CCharDesc *desc, char *name)
{
	uint i, j;
	for(i = 0; i < desc->event_count; i++)
	{
		for(j = 0; desc->events[i].name[j] == name[j] && name[j] != 0; j++);
		if(desc->events[i].name[j] == name[j])
			return i;
	}
	return -1;
}

void c_character_event_over_ride(uint id)
{
	c_character_event_anim_over_ride = id;
}


void c_character_event_over_ride_time(float time)
{
	c_character_event_anim_over_ride_time = time;
}
