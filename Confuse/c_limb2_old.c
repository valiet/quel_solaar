

#include <math.h>
#include <stdio.h>
#include "seduce.h"
#include "testify.h"
#include "gather.h"
#include "vo_beat.h"

#define C_LOOP_BEATS 32
#define C_LOOP_COUNT (4 * C_LOOP_BEATS)
#define C_LOOP_OFFSET 4

typedef struct{
	float pos[C_LOOP_COUNT * 3];
	uint start;
}CAnimPoint;

typedef struct{
	float collar[3];
	float collar_distance;
	float sholder[3];
	float sholder_radius;
	float knee_dir[3];
	float heal[3];
	float foot_up[3];
	float length[2];
}CLimbDescription;

typedef struct{
	float collar[3];
	float shoulder[3];
	float knee[3];
	float heal[3];
	float foot_up[3];
	float knee_direction[3];
}CLimb;

float c_character_matrix[16 * 16];

void c_limb_snap_to_distance(float *base, float *extend, float distance)
{
	float f, vec[3];
	vec[0] = extend[0] - base[0];
	vec[1] = extend[1] - base[1];
	vec[2] = extend[2] - base[2];
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]) / distance;
	extend[0] = base[0] + vec[0] / f;
	extend[1] = base[1] + vec[1] / f;
	extend[2] = base[2] + vec[2] / f;
}

void c_limb_ik(float *sholder, float *knee, float *hand, float *goal, float length_a, float length_b)
{
	uint i;
	for(i = 0; i < 10; i++)
	{
		c_limb_snap_to_distance(goal, knee, length_b);
		c_limb_snap_to_distance(sholder, knee, length_a);
	}
	hand[0] = goal[0];
	hand[1] = goal[1];
	hand[2] = goal[2];
	c_limb_snap_to_distance(knee, hand, length_b);
}


void c_limb_snap_to_sholder(float *collar, float collar_distance, float *sholder, float *goal, float extend, float distance_a, float distance_b)
{
	float f, vec[3], max, min;
	max = distance_a + distance_b;
	min = (distance_a - distance_b) * 2.0;
	if(min < 0)
		min = -min;
	min += max * 0.4; 
	vec[0] = goal[0] - sholder[0];
	vec[1] = goal[1] - sholder[1];
	vec[2] = goal[2] - sholder[2];
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	if(f > max)
	{
		if(f - max < extend)
			extend = f - max;
		sholder[0] += vec[0] * extend / f;
		sholder[1] += vec[1] * extend / f;
		sholder[2] += vec[2] * extend / f;
		c_limb_snap_to_distance(collar, sholder, collar_distance);
	}else if(f < min * 1.0)
	{
		if((min * 1.0 - f) * 0.25 < extend)
			extend = (min * 1.0 - f) * 0.25;
		sholder[0] -= vec[0] * extend / f;
		sholder[1] -= vec[1] * extend / f;
		sholder[2] -= vec[2] * extend / f;
		c_limb_snap_to_distance(collar, sholder, collar_distance);
	}
}


void c_limb_expand(float *sholder, float *a, float *b, float length)
{
	float vec[2], f;
	vec[0] = a[0] - b[0];
	vec[1] = a[1] - b[1];
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	sholder[0] = a[0] + vec[1] * length / f;
	sholder[1] = a[1] - vec[0] * length / f;
}

void c_limb_compute(CLimbDescription *desc, CLimb *limb)
{

}


typedef enum{
	C_EP_HEAD,
	C_EP_SPINE_TOP,
	C_EP_SPINE_BOTTOM,
	C_EP_HAND_LEFT,
	C_EP_HAND_RIGHT,
	C_EP_FOOT_LEFT,
	C_EP_FOOT_RIGHT,
	C_EP_COUNT
}CEditPoints;

typedef enum{
	C_IP_SPINE_MIDDLE = C_EP_COUNT,
	C_IP_SHOLDER_LEFT,
	C_IP_SHOLDER_RIGHT,
	C_IP_ELBOW_LEFT,
	C_IP_ELBOW_RIGHT,
	C_IP_HIP_LEFT,
	C_IP_HIP_RIGHT,
	C_IP_KNEE_LEFT,
	C_IP_KNEE_RIGHT,
	C_IP_COUNT
}CImplicitPoints;

CAnimPoint global_anim_points[C_EP_COUNT];

float lines_debug[4 * 64];

void c_animation_point(float *points, uint start, uint length)
{
	uint i,	mod_i1, mod_i2, mod_i3, loop, add, debug_add = 0;
	float f, temp_pos[C_LOOP_COUNT * 3];



	for(i = 0; i < length - 4; i++)
	{
		temp_pos[0] = points[start * 3 + 0] - points[((start + 1 + i) % C_LOOP_COUNT) * 3 + 0];
		temp_pos[1] = points[start * 3 + 1] - points[((start + 1 + i) % C_LOOP_COUNT) * 3 + 1];
		if(0.01 * 0.01 < temp_pos[0] * temp_pos[0] + temp_pos[1] * temp_pos[1])
			break;
	}
	start = (start + i) % C_LOOP_COUNT;
	length -= i;

	for(i = length - 1; i > 3; i--)
	{
		temp_pos[0] = points[((start + i - 1) % C_LOOP_COUNT) * 3 + 0] - points[((start + length - 1) % C_LOOP_COUNT) * 3 + 0];
		temp_pos[1] = points[((start + i - 1) % C_LOOP_COUNT) * 3 + 1] - points[((start + length - 1) % C_LOOP_COUNT) * 3 + 0];
		if(0.01 * 0.01 < temp_pos[0] * temp_pos[0] + temp_pos[1] * temp_pos[1])
			break;
	}
	length = i + 1;



	for(loop = 1; loop + (loop * 3 / 4) < length; loop *= 2);
	for(i = 0; i < 4 * 64; i++)
		lines_debug[i] = 0.0;

	lines_debug[debug_add++] = points[start * 3 + 0];
	lines_debug[debug_add++] = points[start * 3 + 1];
	lines_debug[debug_add++] = points[((start + length) % C_LOOP_COUNT) * 3 + 0];
	lines_debug[debug_add++] = points[((start + length) % C_LOOP_COUNT) * 3 + 1];
//return;
	if(loop < length)
	{
		add = length - loop;
		printf("start %u start add %u (%u %u) loop %u length %u\n", start, start + add, start, add, loop, length);
		for(i = start; i < start + add; i++)
		{
			f = (float)(i - start) / (float)add;
			mod_i1 = i % C_LOOP_COUNT;
			mod_i2 = (i + loop) % C_LOOP_COUNT;
			printf("f %f - %u %u\n", f, mod_i1, mod_i2);

			lines_debug[debug_add++] = points[mod_i1 * 3 + 0];
			lines_debug[debug_add++] = points[mod_i1 * 3 + 1];
			lines_debug[debug_add++] = points[mod_i2 * 3 + 0];
			lines_debug[debug_add++] = points[mod_i2 * 3 + 1];
			points[mod_i1 * 3 + 0] = points[mod_i1 * 3 + 0] * f + points[mod_i2 * 3 + 0] * (1.0 - f);
			points[mod_i1 * 3 + 1] = points[mod_i1 * 3 + 1] * f + points[mod_i2 * 3 + 1] * (1.0 - f);
			points[mod_i1 * 3 + 2] = points[mod_i1 * 3 + 2] * f + points[mod_i2 * 3 + 2] * (1.0 - f);

			lines_debug[debug_add++] = points[mod_i1 * 3 + 0] + 0.01;
			lines_debug[debug_add++] = points[mod_i1 * 3 + 1];
			lines_debug[debug_add++] = points[mod_i1 * 3 + 0] - 0.01;
			lines_debug[debug_add++] = points[mod_i1 * 3 + 1];
		}
	}else
	{
		mod_i2 = (start + length + C_LOOP_COUNT) % C_LOOP_COUNT;
		mod_i3 = (start) % C_LOOP_COUNT;
	//	printf("loop %u length %u", loop, length);


	lines_debug[debug_add++] = points[mod_i2 * 3 + 0];
	lines_debug[debug_add++] = points[mod_i2 * 3 + 1];
	lines_debug[debug_add++] = points[mod_i3 * 3 + 0];
	lines_debug[debug_add++] = points[mod_i3 * 3 + 1];

		for(i = length; i < loop; i++)
		{
			f = (float)(i - length) / (float)(loop - length);

			mod_i1 = (start + i) % C_LOOP_COUNT;

			points[mod_i1 * 3 + 0] = points[mod_i2 * 3 + 0] * (1.0 - f) + points[mod_i3 * 3 + 0] * f;
			points[mod_i1 * 3 + 1] = points[mod_i2 * 3 + 1] * (1.0 - f) + points[mod_i3 * 3 + 1] * f;
			points[mod_i1 * 3 + 2] = points[mod_i2 * 3 + 2] * (1.0 - f) + points[mod_i3 * 3 + 2] * f;

			lines_debug[debug_add++] = points[mod_i1 * 3 + 0] + 0.01;
			lines_debug[debug_add++] = points[mod_i1 * 3 + 1];
			lines_debug[debug_add++] = points[mod_i1 * 3 + 0] - 0.01;
			lines_debug[debug_add++] = points[mod_i1 * 3 + 1];
		}
	}
	for(i = loop; i < C_LOOP_COUNT; i++)
	{
		points[((start + i) % C_LOOP_COUNT) * 3 + 0] = points[((start + (i % loop)) % C_LOOP_COUNT) * 3 + 0];
		points[((start + i) % C_LOOP_COUNT) * 3 + 1] = points[((start + (i % loop)) % C_LOOP_COUNT) * 3 + 1];
		points[((start + i) % C_LOOP_COUNT) * 3 + 2] = points[((start + (i % loop)) % C_LOOP_COUNT) * 3 + 2];
	}
	for(i = 0; i < C_LOOP_COUNT * 3; i++)
		temp_pos[i] = points[i];
	for(i = 0; i < C_LOOP_COUNT; i++)
	{
		points[((i + 1) % C_LOOP_COUNT) * 3 + 0] = (temp_pos[((i + 1) % C_LOOP_COUNT) * 3 + 0] * 2.0 + temp_pos[((i + 0) % C_LOOP_COUNT) * 3 + 0] + temp_pos[((i + 2) % C_LOOP_COUNT) * 3 + 0]) * 0.25;
		points[((i + 1) % C_LOOP_COUNT) * 3 + 1] = (temp_pos[((i + 1) % C_LOOP_COUNT) * 3 + 1] * 2.0 + temp_pos[((i + 0) % C_LOOP_COUNT) * 3 + 1] + temp_pos[((i + 2) % C_LOOP_COUNT) * 3 + 1]) * 0.25;
		points[((i + 1) % C_LOOP_COUNT) * 3 + 2] = (temp_pos[((i + 1) % C_LOOP_COUNT) * 3 + 2] * 2.0 + temp_pos[((i + 0) % C_LOOP_COUNT) * 3 + 2] + temp_pos[((i + 2) % C_LOOP_COUNT) * 3 + 2]) * 0.25;
	}
}

void c_walk_interact(float *points, BInputState *input, float timer)
{
	static uint start[10];
	static uint length[10];
	static uint grab[10];
	static float center[C_EP_COUNT * 2];
	static float radius[C_EP_COUNT];
	static float grab_ofset_x[10];
	static float grab_ofset_y[10];
	static boolean init = FALSE, grabbed_points[C_EP_COUNT];
	uint i, j, found, itimer;

	timer *= C_LOOP_COUNT;
	itimer = (uint)timer;
	timer -= (float)itimer;

	if(!init)
	{
		init = TRUE;

		for(i = 0; i < C_LOOP_COUNT; i++)
		{
			global_anim_points[C_EP_HEAD].pos[i * 3 + 0] = 0;
			global_anim_points[C_EP_HEAD].pos[i * 3 + 1] = 0.8;
			global_anim_points[C_EP_HEAD].pos[i * 3 + 2] = 0;

			global_anim_points[C_EP_SPINE_TOP].pos[i * 3 + 0] = 0;
			global_anim_points[C_EP_SPINE_TOP].pos[i * 3 + 1] = 0.4;
			global_anim_points[C_EP_SPINE_TOP].pos[i * 3 + 2] = 0;

			global_anim_points[C_EP_SPINE_BOTTOM].pos[i * 3 + 0] = 0;
			global_anim_points[C_EP_SPINE_BOTTOM].pos[i * 3 + 1] = -0.3;
			global_anim_points[C_EP_SPINE_BOTTOM].pos[i * 3 + 2] = 0;

			global_anim_points[C_EP_HAND_LEFT].pos[i * 3 + 0] = -0.5;
			global_anim_points[C_EP_HAND_LEFT].pos[i * 3 + 1] = -0.4;
			global_anim_points[C_EP_HAND_LEFT].pos[i * 3 + 2] = 0;

			global_anim_points[C_EP_HAND_RIGHT].pos[i * 3 + 0] = 0.5;
			global_anim_points[C_EP_HAND_RIGHT].pos[i * 3 + 1] = -0.4;
			global_anim_points[C_EP_HAND_RIGHT].pos[i * 3 + 2] = 0;

			global_anim_points[C_EP_FOOT_LEFT].pos[i * 3 + 0] = 0.4;
			global_anim_points[C_EP_FOOT_LEFT].pos[i * 3 + 1] = -1.3;
			global_anim_points[C_EP_FOOT_LEFT].pos[i * 3 + 2] = 0;

			global_anim_points[C_EP_FOOT_RIGHT].pos[i * 3 + 0] = -0.4;
			global_anim_points[C_EP_FOOT_RIGHT].pos[i * 3 + 1] = -1.3;
			global_anim_points[C_EP_FOOT_RIGHT].pos[i * 3 + 2] = 0;
		}

			center[C_EP_HEAD * 2 + 0] = 0;
			center[C_EP_HEAD * 2 + 1] = 0.8;
			radius[C_EP_HEAD] = 0.2;

			center[C_EP_SPINE_TOP * 2 + 0] = 0;
			center[C_EP_SPINE_TOP * 2 + 1] = 0.4;
			radius[C_EP_SPINE_TOP] = 0.2;

			center[C_EP_SPINE_BOTTOM * 2 + 0] = 0;
			center[C_EP_SPINE_BOTTOM * 2 + 1] = -0.5;
			radius[C_EP_SPINE_BOTTOM] = 0.4;

			center[C_EP_HAND_LEFT * 2 + 0] = -0.5;
			center[C_EP_HAND_LEFT * 2 + 1] = 0.4;
			radius[C_EP_HAND_LEFT] = 1.0;

			center[C_EP_HAND_RIGHT * 2 + 0] = 0.5;
			center[C_EP_HAND_RIGHT * 2 + 1] = 0.4;
			radius[C_EP_HAND_RIGHT] = 1.0;

			center[C_EP_FOOT_LEFT * 2 + 0] = 0.5;
			center[C_EP_FOOT_LEFT * 2 + 1] = -1.2;
			radius[C_EP_FOOT_LEFT] = 0.4;

			center[C_EP_FOOT_RIGHT * 2 + 0] = -0.5;
			center[C_EP_FOOT_RIGHT * 2 + 1] = -1.2;
			radius[C_EP_FOOT_RIGHT] = 0.4;

	/*	for(i = 0; i < C_LOOP_COUNT; i++)
		{
			for(j = 0; j < C_EP_COUNT; j++)
			{
				global_anim_points[j].pos[i * 3 + 0] += sin(j + (float)i * PI * 2.0 / C_LOOP_COUNT) * 0.1;
				global_anim_points[j].pos[i * 3 + 1] += cos(j + (float)i * PI * 2.0 / C_LOOP_COUNT) * 0.1;
			}
		}*/
		for(i = 0; i < 10; i++)
			grab[i] = -1;
		for(i = 0; i < C_EP_COUNT; i++)
			grabbed_points[i] = FALSE;
	}

	for(i = 0; i < C_EP_COUNT; i++)
	{
		points[i * 3 + 0] = global_anim_points[i].pos[itimer * 3 + 0] + (global_anim_points[i].pos[((itimer + 1) % C_LOOP_COUNT) * 3 + 0] - global_anim_points[i].pos[itimer * 3 + 0]) * timer;
		points[i * 3 + 1] = global_anim_points[i].pos[itimer * 3 + 1] + (global_anim_points[i].pos[((itimer + 1) % C_LOOP_COUNT) * 3 + 1] - global_anim_points[i].pos[itimer * 3 + 1]) * timer;
		points[i * 3 + 2] = global_anim_points[i].pos[itimer * 3 + 2] + (global_anim_points[i].pos[((itimer + 1) % C_LOOP_COUNT) * 3 + 2] - global_anim_points[i].pos[itimer * 3 + 2]) * timer;

/*		r_primitive_line_3d(points[i * 3 + 0] - 0.1, points[i * 3 + 1], points[i * 3 + 2],
							points[i * 3 + 0] + 0.1, points[i * 3 + 1], points[i * 3 + 2], 0.4, 0.2, 0.2, 1.0);
		r_primitive_line_3d(points[i * 3 + 0], points[i * 3 + 1] - 0.1, points[i * 3 + 2],
							points[i * 3 + 0], points[i * 3 + 1] + 0.1, points[i * 3 + 2], 0.4, 0.2, 0.2, 1.0);


		r_primitive_line_3d(center[i * 2 + 0] - radius[i], center[i * 2 + 1], 0,
							center[i * 2 + 0] + radius[i], center[i * 2 + 1], 0, 0.1, 0.1, 0.2, 1.0);
		r_primitive_line_3d(center[i * 2 + 0], center[i * 2 + 1] - radius[i], 0,
							center[i * 2 + 0], center[i * 2 + 1] + radius[i], 0, 0.1, 0.1, 0.2, 1.0);*/
	}

	for(i = 0; i < 10 && i < input->pointer_count; i++)
	{
		if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
		{
			float vec[2], f, best = 1.0;
			found = -1;
			for(j = 0; j < C_EP_COUNT; j++)
			{
				if(!grabbed_points[j])
				{
					vec[0] = points[j * 3 + 0] - input->pointers[i].pointer_x;
					vec[1] = points[j * 3 + 1] - input->pointers[i].pointer_y;
					f = vec[0] * vec[0] + vec[1] * vec[1];
					if(f < best)
					{
						found = j;
						best = f;
					}
				}
			}
			grabbed_points[found] = TRUE;
			grab[i] = found;
			start[i] = (itimer + C_LOOP_OFFSET) % C_LOOP_COUNT;
			length[i] = 0;
			grab_ofset_x[i] = input->pointers[i].pointer_x - points[grab[i] * 3 + 0];
			grab_ofset_y[i] = input->pointers[i].pointer_y - points[grab[i] * 3 + 1];
		}
		if(grab[i] != -1)
		{
			float vec[2], f;
			if((itimer + C_LOOP_OFFSET + C_LOOP_COUNT - start[i]) % C_LOOP_COUNT > length[i])
				length[i] = (itimer + C_LOOP_OFFSET + C_LOOP_COUNT - start[i]) % C_LOOP_COUNT;
			points[grab[i] * 3 + 0] = global_anim_points[grab[i]].pos[((itimer + C_LOOP_OFFSET) % C_LOOP_COUNT) * 3 + 0] = grab_ofset_x[i] + input->pointers[i].pointer_x;
			points[grab[i] * 3 + 1] = global_anim_points[grab[i]].pos[((itimer + C_LOOP_OFFSET) % C_LOOP_COUNT) * 3 + 1] = grab_ofset_y[i] + input->pointers[i].pointer_y;
			vec[0] = points[grab[i] * 3 + 0] - center[grab[i] * 2 + 0];
			vec[1] = points[grab[i] * 3 + 1] - center[grab[i] * 2 + 1];
			f = vec[0] * vec[0] + vec[1] * vec[1];
			if(f > radius[grab[i]] * radius[grab[i]])
			{
				f = sqrt(f);
				global_anim_points[grab[i]].pos[((itimer + C_LOOP_OFFSET) % C_LOOP_COUNT) * 3 + 0] = points[grab[i] * 3 + 0] = center[grab[i] * 2 + 0] + vec[0] * radius[grab[i]] / f;
				global_anim_points[grab[i]].pos[((itimer + C_LOOP_OFFSET) % C_LOOP_COUNT) * 3 + 1] = points[grab[i] * 3 + 1] = center[grab[i] * 2 + 1] + vec[1] * radius[grab[i]] / f;
			}
			timer = 0;
		}
		if(!input->pointers[i].button[0] && grab[i] != -1)
		{
			c_animation_point(global_anim_points[grab[i]].pos, start[i], length[i]);
			grabbed_points[grab[i]] = FALSE;
			grab[i] = -1;
		}

	}


/*	
	for(i = 0; i < 64; i++)
		r_primitive_line_3d(lines_debug[4 * i + 0], lines_debug[4 * i + 1], 0,
							lines_debug[4 * i + 2], lines_debug[4 * i + 3], 0, 0.8, 0.8, 0.2, 1.0);
*/

/*
	for(i = 0; i < C_EP_COUNT; i++)
	{
		for(j = 0; j < C_LOOP_COUNT; j++)
		{

			r_primitive_line_2d(global_anim_points[i].pos[j * 3 + 0], global_anim_points[i].pos[j * 3 + 1],
								global_anim_points[i].pos[((j + 1) % C_LOOP_COUNT) * 3 + 0], global_anim_points[i].pos[((j + 1) % C_LOOP_COUNT) * 3 + 1], 0.5, 0.4, 0.3, 1.0);
		}
	}
*/
	r_primitive_line_flush();
}

extern void vo_line_draw_3d(float *pos_a, float *pos_b, float *camera, float red, float green, float blue, float alpha);
extern void vo_matrixyzf(float *matrix, const float *origo, const float *point_a, const float *point_b);
extern void vo_line_draw_marices(float *martix, uint matrix_count, float red, float green, float blue, float alpha);

void c_draw_limb(float *points, uint a, uint b)
{
	float camera[3] = {0, 0, 1};
	r_primitive_line_3d(points[a * 3 + 0], points[a * 3 + 1], points[a * 3 + 2],
						points[b * 3 + 0], points[b * 3 + 1], points[b * 3 + 2], 0.4, 0.4, 0.4, 1.0);

	vo_line_draw_3d(&points[a * 3], &points[b * 3], camera, 1, 0, 0, 1);
}
void c_walk_test(BInputState *input, float timer, float *vector)
{
	static float edit_points[C_EP_COUNT * 3];
	static float draw_points[C_IP_COUNT * 3];
	static float camera[3] = {0, 0, 1};
	uint i;

	c_walk_interact(edit_points, input, timer);
/*C_EP_HEAD * 3 + 2] = 0;

			points[C_EP_SPINE_TOP * 3 + 0] = 0;
			points[C_EP_SPINE_TOP * 3 + 1] = 0.7;
			points[C_EP_SPINE_TOP * 3 + 2] = 0;

			points[C_EP_SPINE_BOTTOM*/

	for(i = 0; i < C_EP_COUNT * 3; i++)
		draw_points[i] = edit_points[i];

	draw_points[C_IP_SPINE_MIDDLE * 3 + 0] = (draw_points[C_EP_SPINE_BOTTOM * 3 + 0] + draw_points[C_EP_SPINE_TOP * 3 + 0]) * 0.2/* +
		(draw_points[C_EP_SPINE_TOP * 3 + 0] - draw_points[C_EP_HEAD * 3 + 0]) * 0.3*/;
	draw_points[C_IP_SPINE_MIDDLE * 3 + 1] = (draw_points[C_EP_SPINE_BOTTOM * 3 + 1] + draw_points[C_EP_SPINE_TOP * 3 + 1]) * 0.2;
	draw_points[C_IP_SPINE_MIDDLE * 3 + 2] = (draw_points[C_EP_SPINE_BOTTOM * 3 + 2] + draw_points[C_EP_SPINE_TOP * 3 + 2]) * 0.2 + 0.1;

	c_limb_snap_to_distance(&draw_points[C_EP_SPINE_BOTTOM * 3], &draw_points[C_IP_SPINE_MIDDLE * 3], 0.2);

	c_limb_snap_to_distance(&draw_points[C_IP_SPINE_MIDDLE * 3], &draw_points[C_EP_SPINE_TOP * 3], 0.5);

	c_limb_snap_to_distance(&draw_points[C_EP_SPINE_TOP * 3], &draw_points[C_EP_HEAD * 3], 0.3);


	c_limb_expand(&draw_points[C_IP_SHOLDER_LEFT * 3], &draw_points[C_EP_SPINE_TOP * 3], &draw_points[C_EP_SPINE_BOTTOM * 3], -0.3);
	c_limb_expand(&draw_points[C_IP_SHOLDER_RIGHT * 3], &draw_points[C_EP_SPINE_TOP * 3], &draw_points[C_EP_SPINE_BOTTOM * 3], 0.3);

	draw_points[C_IP_ELBOW_LEFT * 3 + 0] = -1;
	draw_points[C_IP_ELBOW_LEFT * 3 + 1] = 0;
	draw_points[C_IP_ELBOW_LEFT * 3 + 2] = -0.8;
	draw_points[C_IP_ELBOW_RIGHT * 3 + 0] = 1;
	draw_points[C_IP_ELBOW_RIGHT * 3 + 1] = 0;
	draw_points[C_IP_ELBOW_RIGHT * 3 + 2] = -0.8;

	c_limb_snap_to_sholder(&draw_points[C_EP_SPINE_TOP * 3], 0.3, &draw_points[C_IP_SHOLDER_LEFT * 3], &draw_points[C_EP_HAND_LEFT * 3], 0.1, 0.5, 0.5);
	c_limb_ik(&draw_points[C_IP_SHOLDER_LEFT * 3], &draw_points[C_IP_ELBOW_LEFT * 3], &draw_points[C_EP_HAND_LEFT * 3], &draw_points[C_EP_HAND_LEFT * 3], 0.5, 0.5);

	c_limb_snap_to_sholder(&draw_points[C_EP_SPINE_TOP * 3], 0.3, &draw_points[C_IP_SHOLDER_RIGHT * 3], &draw_points[C_EP_HAND_RIGHT * 3], 0.1, 0.5, 0.5);
	c_limb_ik(&draw_points[C_IP_SHOLDER_RIGHT * 3], &draw_points[C_IP_ELBOW_RIGHT * 3], &draw_points[C_EP_HAND_RIGHT * 3], &draw_points[C_EP_HAND_RIGHT * 3], 0.5, 0.5);



	c_limb_expand(&draw_points[C_IP_HIP_LEFT * 3], &draw_points[C_EP_SPINE_BOTTOM * 3], &draw_points[C_EP_SPINE_TOP * 3], -0.2);
	c_limb_expand(&draw_points[C_IP_HIP_RIGHT * 3], &draw_points[C_EP_SPINE_BOTTOM * 3], &draw_points[C_EP_SPINE_TOP * 3], 0.2);

	draw_points[C_IP_KNEE_LEFT * 3 + 0] = 1;
	draw_points[C_IP_KNEE_LEFT * 3 + 1] = 0;
	draw_points[C_IP_KNEE_LEFT * 3 + 2] = 1.3;
	draw_points[C_IP_KNEE_RIGHT * 3 + 0] = -1;
	draw_points[C_IP_KNEE_RIGHT * 3 + 1] = 0;
	draw_points[C_IP_KNEE_RIGHT * 3 + 2] = 1.3;

	c_limb_snap_to_sholder(&draw_points[C_EP_SPINE_BOTTOM * 3], 0.2, &draw_points[C_IP_HIP_LEFT * 3], &draw_points[C_EP_FOOT_LEFT * 3], 0.15, 0.5, 0.5);
	c_limb_ik(&draw_points[C_IP_HIP_LEFT * 3], &draw_points[C_IP_KNEE_LEFT * 3], &draw_points[C_EP_FOOT_LEFT * 3], &draw_points[C_EP_FOOT_LEFT * 3], 0.5, 0.5);

	c_limb_snap_to_sholder(&draw_points[C_EP_SPINE_BOTTOM * 3], 0.2, &draw_points[C_IP_HIP_RIGHT * 3], &draw_points[C_EP_FOOT_RIGHT * 3], 0.15, 0.5, 0.5);
	c_limb_ik(&draw_points[C_IP_HIP_RIGHT * 3], &draw_points[C_IP_KNEE_RIGHT * 3], &draw_points[C_EP_FOOT_RIGHT * 3], &draw_points[C_EP_FOOT_RIGHT * 3], 0.5, 0.5);

/*	for(i = 0; i < C_IP_COUNT; i++)
	{
		r_primitive_line_3d(draw_points[i * 3 + 0] - 0.01, draw_points[i * 3 + 1] + 0.01, draw_points[i * 3 + 2],
							draw_points[i * 3 + 0] + 0.01, draw_points[i * 3 + 1] - 0.01, draw_points[i * 3 + 2], 0.4, 0.0, 0.4, 1.0);
		r_primitive_line_3d(draw_points[i * 3 + 0] - 0.01, draw_points[i * 3 + 1] - 0.01, draw_points[i * 3 + 2],
							draw_points[i * 3 + 0] + 0.01, draw_points[i * 3 + 1] + 0.01, draw_points[i * 3 + 2], 0.4, 0.0, 0.4, 1.0);
	}

	r_primitive_line_3d(draw_points[i * 3 + 0], draw_points[i * 3 + 1], draw_points[i * 3 + 2],
						draw_points[i * 3 + 0], draw_points[i * 3 + 1], draw_points[i * 3 + 2], 0.4, 0.0, 0.4, 1.0);
	
*/
	{
		float camera[3] = {0, 0, 1};
		vo_line_set(0.03, 0.00, 0.1, vo_beat_divider(timer, 1), timer);
	//	vo_line_draw_3d(&draw_points[C_EP_FOOT_RIGHT * 3], &draw_points[C_IP_KNEE_RIGHT * 3], camera, 1, 0.5, 0, 1.0);
	}
/*	c_draw_limb(draw_points, C_EP_HEAD, C_EP_SPINE_TOP);
	c_draw_limb(draw_points, C_IP_SPINE_MIDDLE, C_EP_SPINE_TOP);
	c_draw_limb(draw_points, C_IP_SHOLDER_LEFT, C_EP_SPINE_TOP);
	c_draw_limb(draw_points, C_IP_SHOLDER_RIGHT, C_EP_SPINE_TOP);
	c_draw_limb(draw_points, C_IP_SHOLDER_LEFT, C_IP_ELBOW_LEFT);
	c_draw_limb(draw_points, C_IP_SHOLDER_RIGHT, C_IP_ELBOW_RIGHT);
	c_draw_limb(draw_points, C_EP_HAND_LEFT, C_IP_ELBOW_LEFT);
	c_draw_limb(draw_points, C_EP_HAND_RIGHT, C_IP_ELBOW_RIGHT);
	c_draw_limb(draw_points, C_IP_SPINE_MIDDLE, C_EP_SPINE_BOTTOM);
	c_draw_limb(draw_points, C_IP_HIP_LEFT, C_EP_SPINE_BOTTOM);
	c_draw_limb(draw_points, C_IP_HIP_RIGHT, C_EP_SPINE_BOTTOM);
	c_draw_limb(draw_points, C_IP_HIP_LEFT, C_IP_KNEE_LEFT);
	c_draw_limb(draw_points, C_IP_HIP_RIGHT, C_IP_KNEE_RIGHT);
	c_draw_limb(draw_points, C_EP_FOOT_LEFT, C_IP_KNEE_LEFT);
	c_draw_limb(draw_points, C_EP_FOOT_RIGHT, C_IP_KNEE_RIGHT);
r_primitive_line_flush();*/
	vo_matrixyzf(&c_character_matrix[0 * 16], &draw_points[C_EP_HEAD * 3], &draw_points[C_EP_SPINE_TOP * 3], vector);
	vo_matrixyzf(&c_character_matrix[1 * 16], &draw_points[C_IP_SPINE_MIDDLE * 3], &draw_points[C_EP_SPINE_TOP * 3], vector);
	vo_matrixyzf(&c_character_matrix[2 * 16], &draw_points[C_IP_SHOLDER_LEFT * 3], &draw_points[C_EP_SPINE_TOP * 3], vector);
	vo_matrixyzf(&c_character_matrix[3 * 16], &draw_points[C_IP_SHOLDER_RIGHT * 3], &draw_points[C_EP_SPINE_TOP * 3], vector);
	vo_matrixyzf(&c_character_matrix[4 * 16], &draw_points[C_IP_SHOLDER_LEFT * 3], &draw_points[C_IP_ELBOW_LEFT * 3], vector);
	vo_matrixyzf(&c_character_matrix[5 * 16], &draw_points[C_IP_SHOLDER_RIGHT * 3], &draw_points[C_IP_ELBOW_RIGHT * 3], vector);
	vo_matrixyzf(&c_character_matrix[6 * 16], &draw_points[C_EP_HAND_LEFT * 3], &draw_points[C_IP_ELBOW_LEFT * 3], vector);
	vo_matrixyzf(&c_character_matrix[7 * 16], &draw_points[C_EP_HAND_RIGHT * 3], &draw_points[C_IP_ELBOW_RIGHT * 3], vector);
	vo_matrixyzf(&c_character_matrix[8 * 16], &draw_points[C_IP_SPINE_MIDDLE * 3], &draw_points[C_EP_SPINE_BOTTOM * 3], vector);
	vo_matrixyzf(&c_character_matrix[9 * 16], &draw_points[C_IP_HIP_LEFT * 3], &draw_points[C_EP_SPINE_BOTTOM * 3], vector);
	vo_matrixyzf(&c_character_matrix[10 * 16], &draw_points[C_IP_HIP_RIGHT * 3], &draw_points[C_EP_SPINE_BOTTOM * 3], vector);
	vo_matrixyzf(&c_character_matrix[11 * 16], &draw_points[C_IP_HIP_LEFT * 3], &draw_points[C_IP_KNEE_LEFT * 3], vector);
	vo_matrixyzf(&c_character_matrix[12 * 16], &draw_points[C_IP_HIP_RIGHT * 3], &draw_points[C_IP_KNEE_RIGHT * 3], vector);
	vo_matrixyzf(&c_character_matrix[13 * 16], &draw_points[C_EP_FOOT_LEFT * 3], &draw_points[C_IP_KNEE_LEFT * 3], vector);
	vo_matrixyzf(&c_character_matrix[14 * 16], &draw_points[C_EP_FOOT_RIGHT * 3], &draw_points[C_IP_KNEE_RIGHT * 3], vector);
	
	vo_line_draw_marices(c_character_matrix, 15, 1, 1, 1, 1);
//	r_primitive_line_flush();
}
/*
	C_EP_HEAD,
	C_EP_SPINE_TOP,
	C_EP_SPINE_BOTTOM,
	C_EP_HAND_LEFT,
	C_EP_HAND_RIGHT,
	C_EP_FOOT_LEFT,
	C_EP_FOOT_RIGHT,
	C_EP_COUNT
}CEditPoints;

typedef enum{
	C_IP_SPINE_MIDDLE = C_EP_COUNT,
	C_IP_SHOLDER_LEFT,
	C_IP_SHOLDER_RIGHT,
	C_IP_ELBOW_LEFT,
	C_IP_ELBOW_RIGHT,
	C_IP_HIP_LEFT,
	C_IP_HIP_RIGHT,
	C_IP_KNEE_LEFT,
	C_IP_KNEE_RIGHT,*/

void c_walk_test3(BInputState *input)
{
	static boolean init = TRUE;
	static CLimbDescription desc;
	static float pos[3] = {0.4, 0, 0}, knee[3], sholder[3], hand[3];
	uint i;

	if(init)
	{
		desc.collar[0] = 0.0;
		desc.collar[1] = 0.5;
		desc.collar[2] = 0.0;
		desc.collar_distance = 0.3;
		desc.sholder[0] = 0.3;
		desc.sholder[1] = 0.5;
		desc.sholder[2] = 0.0;
		desc.sholder_radius = 0.3;
		desc.knee_dir[0] = 0.0;
		desc.knee_dir[1] = 0.5;
		desc.knee_dir[2] = 0.0;
		desc.heal[0];
		desc.foot_up[0];
		desc.length[0] = 0.4;
		desc.length[1] = 0.6;
		init = FALSE;
	}
/*	r_primitive_line_3d(desc.collar[0], desc.collar[1], desc.collar[2],
						desc.sholder[0], desc.sholder[1], desc.sholder[2], 0.2, 0.2, 0.2, 1.0);
*/

	r_primitive_line_3d(desc.sholder[0] + desc.sholder_radius, desc.sholder[1], desc.sholder[2],
						desc.sholder[0] - desc.sholder_radius, desc.sholder[1], desc.sholder[2], 0.4, 0.2, 0.2, 1.0);

	r_primitive_line_3d(desc.sholder[0], desc.sholder[1] + desc.sholder_radius, desc.sholder[2],
						desc.sholder[0], desc.sholder[1] - desc.sholder_radius, desc.sholder[2], 0.4, 0.2, 0.2, 1.0);


/*	r_primitive_line_3d(pos[0], pos[1], pos[2],
						desc.sholder[0], desc.sholder[1], desc.sholder[2], 0.2, 0.2, 0.2, 1.0);
*/
	if(input->pointers[0].button[0])
	{
		pos[0] += input->pointers[0].delta_pointer_x;
		pos[1] += input->pointers[0].delta_pointer_y;
	}
	knee[0] = desc.sholder[0] + desc.knee_dir[0];
	knee[1] = desc.sholder[1] + desc.knee_dir[1];
	knee[2] = desc.sholder[2] + desc.knee_dir[2];


	sholder[0] = desc.sholder[0];
	sholder[1] = desc.sholder[1];
	sholder[2] = desc.sholder[2];

	c_limb_snap_to_sholder(desc.collar, desc.collar_distance, sholder, pos, desc.sholder_radius, desc.length[0], desc.length[1]);


	for(i = 0; i < 10; i++)
	{
		c_limb_snap_to_distance(pos, knee, desc.length[1]);
		c_limb_snap_to_distance(sholder, knee, desc.length[0]);
	}
	hand[0] = pos[0];
	hand[1] = pos[1];
	hand[2] = pos[2];
	c_limb_snap_to_distance(knee, hand, desc.length[1]);

	r_primitive_line_3d(desc.collar[0], desc.collar[1], desc.collar[2],
						sholder[0], sholder[1], sholder[2], 0.2, 1.2, 0.2, 1.0);
	r_primitive_line_3d(knee[0], knee[1], knee[2],
						sholder[0], sholder[1], sholder[2], 0.2, 1.2, 0.2, 1.0);
	r_primitive_line_3d(hand[0], hand[1], hand[2],
						knee[0], knee[1], knee[2], 0.2, 1.2, 0.2, 1.0);

	r_primitive_line_3d(pos[0] + 0.01, pos[1] + 0.01, pos[2], pos[0] - 0.01, pos[1] - 0.01, pos[2], 0.2, 0.2, 1.2, 1.0);
	r_primitive_line_3d(pos[0] - 0.01, pos[1] + 0.01, pos[2], pos[0] + 0.01, pos[1] - 0.01, pos[2], 0.2, 0.2, 1.2, 1.0);


/*
	center[0] = 0.0;
	center[1] = 0.0;
	for(i = 0; i < C_LEG_COUNT; i++)
	{
		center[0] += feet[i].pos[0] / C_LEG_COUNT;
		center[1] += feet[i].pos[1] / C_LEG_COUNT;
	}
	vec[0] = center[0] - pos[0];
	vec[1] = center[1] - pos[1];

	count = 0;
	for(i = 0; i < C_LEG_COUNT; i++)
		if(feet[i].progress > 0.001)
			count++;
	if(count < 1)
	{
		best = 0;
		found = -1;
		for(i = 0; i < C_LEG_COUNT; i++)
		{
			if(feet[found].progress < 0.001)
			{
				vec2[0] = feet[i].pos[0] - (pos[0] + feet[i].hip_offset[0]);
				vec2[1] = feet[i].pos[1] - (pos[1] + feet[i].hip_offset[1]);
				f = vec2[0] * vec[0] + vec2[1] * vec[1];
				if(f > 0.0 && 0.1 * 0.1 < vec2[0] * vec2[0] + vec2[1] * vec2[1])
				{
					feet[i].progress = 0.001 + input->delta_time * 1.0;
					count++;
				}else
				{
					
					if(f > best)
					{
						best = f;
						found = i;
					}
				}
			}
		}
		if(found != -1)
		{
			feet[found].progress = 0.001 + input->delta_time * 1.0;
			count++;
		}
	}

	for(i = 0; i < C_LEG_COUNT; i++)
	{
		if(feet[i].progress > 0.001)
		{	
			feet[i].progress += input->delta_time * 1.0;

			feet[i].pos[0] -= vec[0] * C_LEG_COUNT / (float)count;
			feet[i].pos[1] -= vec[1] * C_LEG_COUNT / (float)count;

			if(feet[i].progress > 0.1)
				feet[i].progress = 0.0;
		}
	}



	r_primitive_line_2d(pos[0] + 0.1, pos[1], pos[0] - 0.1, pos[1], 1.0, 1.0, 1.0, 1.0);
	r_primitive_line_2d(pos[0], pos[1] + 0.1, pos[0], pos[1] - 0.1, 1.0, 1.0, 1.0, 1.0);
	for(i = 0; i < C_LEG_COUNT; i++)
	{
		r_primitive_line_2d(pos[0] + feet[i].hip_offset[0] + 0.01, pos[1] + feet[i].hip_offset[1], pos[0] + feet[i].hip_offset[0] - 0.01, pos[1] + feet[i].hip_offset[1], 1.0, 1.0, 0.0, 1.0);
		r_primitive_line_2d(pos[0] + feet[i].hip_offset[0], pos[1] + feet[i].hip_offset[1] + 0.01, pos[0] + feet[i].hip_offset[0], pos[1] + feet[i].hip_offset[1] - 0.01, 1.0, 1.0, 0.0, 1.0);

		r_primitive_line_2d(feet[i].pos[0] + 0.02, feet[i].pos[1] - 0.02, feet[i].pos[0] - 0.02, feet[i].pos[1] + 0.02, 1.0, 0.0, 1.0, 1.0);
		r_primitive_line_2d(feet[i].pos[0] + 0.02, feet[i].pos[1] + 0.02, feet[i].pos[0] - 0.02, feet[i].pos[1] - 0.02, 1.0, 0.0, 1.0, 1.0);


		r_primitive_line_2d(pos[0] + feet[i].hip_offset[0], pos[1] + feet[i].hip_offset[1], feet[i].pos[0], feet[i].pos[1], 0.3, 0.3, 0.3, 1.0);

	}*/
	r_primitive_line_flush();
}