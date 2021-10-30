#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "confuse.h"

extern void c_character_event_anim(CCharDesc *desc, CCharacter *c, uint tool_count, uint *tools, float *tool_matrix, uint selected_tool, float delta);
extern void c_character_matrix_aim(float *matrix, float *aim, float *normal, float *pos, float scale);
extern float c_character_spine_base(CCharacter *c, float *base, float *look, float *pos, float *upp, float *walk, float *look_dir, float delta);
extern void c_character_spine_hip(CCharacter *c, uint legs, float *base, float *pos, float *matrix, float leg_length, float scale, float width, float *rotate, float *base_lean, float *top_lean);
extern void c_character_spine_hands(float *pos, float *base, float *right_hand, float *left_hand, float length, float *bend, float *base_lean, float *top_lean, float *rotate);
extern void c_character_spine_body(CCharacter *c, CCharDesc *desc, float *matrix, float bend, float base_lean, float top_lean, float rotate, float *look);
extern void c_character_spine_head(CCharacter *c, CCharDesc *desc, float neck, float forward, float scale, uint base, float *look);
extern void c_character_leg(CIKLeg *leg, float *pos, float *center, float *dir, float *normal, float speed,  float *knee, float length, float height, float *leg_matrix, float *knee_matrix, boolean step, float delta);
extern void c_character_arm(float *base, float *matrix, float sholder_dist, float *goal, float length, float *elbow, float *up_matrix, float *down_matrix, float *hand);

/* debug line draw */

/*
void c_draw_line(float x0, float y0, float z0, float x1, float y1, float z1, float red, float green, float blue)
{
#ifdef ME_CLIENT_SIDE

#endif
}

#define MATRIX_SIZE 1.0

void c_draw_matrix(float *matrix)
{
//	c_draw_line(matrix[12], matrix[13], matrix[14], matrix[12] + matrix[0] * MATRIX_SIZE, matrix[13] + matrix[1] * MATRIX_SIZE, matrix[14] + matrix[2] * MATRIX_SIZE, 0, 0.5, 0.5);
//	c_draw_line(matrix[12], matrix[13], matrix[14], matrix[12] + matrix[4] * MATRIX_SIZE, matrix[13] + matrix[5] * MATRIX_SIZE, matrix[14] + matrix[6] * MATRIX_SIZE, 0, 1.0, 0);
	c_draw_line(matrix[12], matrix[13], matrix[14], matrix[12] + matrix[8] * MATRIX_SIZE, matrix[13] + matrix[9] * MATRIX_SIZE, matrix[14] + matrix[10] * MATRIX_SIZE, 0, 0, 1.0);

}

void c_draw_pos(float *pos)
{
	c_draw_line(pos[0] + 0.002, pos[1], pos[2], pos[0] - 0.002, pos[1], pos[2], 1, 0, 0);
	c_draw_line(pos[0], pos[1] + 0.002, pos[2], pos[0], pos[1] - 0.002, pos[2], 0, 1, 0);
	c_draw_line(pos[0], pos[1], pos[2] + 0.002, pos[0], pos[1], pos[2] - 0.002, 0, 0, 1);
}
*/
void c_character_matrix(float *matrix, float *vec, float *normal, float *pos)
{
	float f, n[3];
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	matrix[4] = vec[0] / f;
	matrix[5] = vec[1] / f;
	matrix[6] = vec[2] / f;
	f = matrix[4] * normal[0] + matrix[5] * normal[1] + matrix[6] * normal[2];
	n[0] = normal[0] - matrix[4] * f;
	n[1] = normal[1] - matrix[5] * f;
	n[2] = normal[2] - matrix[6] * f;
	f = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	matrix[8] = n[0] / f;
	matrix[9] = n[1] / f;
	matrix[10] = n[2] / f;
	matrix[0] = matrix[5] * matrix[10] - matrix[6] * matrix[9];
	matrix[1] = matrix[6] * matrix[8] - matrix[4] * matrix[10];
	matrix[2] = matrix[4] * matrix[9] - matrix[5] * matrix[8];
	matrix[12] = pos[0];
	matrix[13] = pos[1];
	matrix[14] = pos[2];		
}

void c_character_matrix_scaled(float *matrix, float *vec, float *normal, float *pos, float scale)
{
	float f, n[3];
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	matrix[4] = vec[0] / f;
	matrix[5] = vec[1] / f;
	matrix[6] = vec[2] / f;
	f = matrix[4] * normal[0] + matrix[5] * normal[1] + matrix[6] * normal[2];
	n[0] = normal[0] - matrix[4] * f;
	n[1] = normal[1] - matrix[5] * f;
	n[2] = normal[2] - matrix[6] * f;
	f = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	matrix[8] = n[0] / f;
	matrix[9] = n[1] / f;
	matrix[10] = n[2] / f;
	matrix[0] = matrix[5] * matrix[10] - matrix[6] * matrix[9];
	matrix[1] = matrix[6] * matrix[8] - matrix[4] * matrix[10];
	matrix[2] = matrix[4] * matrix[9] - matrix[5] * matrix[8];
	matrix[0] *= scale;
	matrix[1] *= scale;
	matrix[2] *= scale;
	matrix[4] *= scale;
	matrix[5] *= scale;
	matrix[6] *= scale;
	matrix[8] *= scale;
	matrix[9] *= scale;
	matrix[10] *= scale;
	matrix[12] = pos[0];
	matrix[13] = pos[1];
	matrix[14] = pos[2];		
}

void c_character_init(CCharacter *c, float *pos)
{
	uint i, j;
	float matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	for(j = 0; j < C_CM_COUNT; j++)
		for(i = 0; i < 16; i++)
			c->matrix[j][i] = matrix[i];

	c->look_dir[0] = pos[0];
	c->look_dir[1] = pos[1];
	c->look_dir[2] = pos[1];
	for(i = 0; i < C_MAX_LEG_COUNT; i++)
	{
		c->legs[i].move = FALSE;
		c->legs[i].timer = 1.01;
		c->legs[i].speed = 1;
		c->legs[i].foot_pos[0] = pos[0];  
		c->legs[i].foot_pos[1] = pos[1];  
		c->legs[i].foot_pos[2] = pos[2];  
		c->legs[i].foot_goal[0] = pos[0];  
		c->legs[i].foot_goal[1] = pos[1];  
		c->legs[i].foot_goal[2] = pos[2];
	}
	for(i = 0; i < C_MAX_ARM_COUNT; i++)
	{
		for(j = 0; j < 2; j++)
		{
			c->arms[i].hand[j][0] = pos[0];
			c->arms[i].hand[j][1] = pos[1];
			c->arms[i].hand[j][2] = pos[2];
			c->arms[i].hand_timers[j] = 0;
			c->arms[i].snap[j] = C_AS_NONE;
		}
		c->arms[i].timer = 0;
		c->arms[i].tool = -1;
	}
	c->event_count = 0;
	c->tool_current = -1;
	c->tool_last = -1;
	c->tool_timer = 0;
	c->points[C_APC_BASE].pos[0] = 0.0;
	c->points[C_APC_BASE].pos[1] = 0.0;
	c->points[C_APC_BASE].pos[2] = 0.0;
	c->points[C_APC_BASE].space = -1;
	c->points[C_APC_BASE].time = 0;

	c->points[C_APC_HAND_LEFT].pos[0] = 0.0;
	c->points[C_APC_HAND_LEFT].pos[1] = -1.0;
	c->points[C_APC_HAND_LEFT].pos[2] = 0.0;
	c->points[C_APC_HAND_LEFT].space = C_CM_BELLY;
	c->points[C_APC_HAND_LEFT].time = 0;

	c->points[C_APC_HAND_RIGHT].pos[0] = 0.0;
	c->points[C_APC_HAND_RIGHT].pos[1] = 1.0;
	c->points[C_APC_HAND_RIGHT].pos[2] = 0.0;
	c->points[C_APC_HAND_RIGHT].space = C_CM_BELLY;
	c->points[C_APC_HAND_RIGHT].time = 0;

	c->points[C_APC_HEAD_UP].pos[0] = 0.0;
	c->points[C_APC_HEAD_UP].pos[1] = 0.0;
	c->points[C_APC_HEAD_UP].pos[2] = 0.0;
	c->points[C_APC_HEAD_UP].space = C_CM_BELLY;
	c->points[C_APC_HEAD_UP].time = 0;
}

void c_character_leg_spacing(float *output, float *pos, float *base, float leg_length, float forward, float sides)
{
	sides *= leg_length * 0.4;
	forward *= leg_length;
	output[0] = pos[0] + base[0] * sides - base[8] * forward;
	output[1] = pos[1] + base[1] * sides - base[9] * forward;
	output[2] = pos[2] + base[2] * sides - base[10] * forward;
}
/*
extern void sui_draw_3d_line_gl(float start_x, float start_y,  float start_z, float end_x, float end_y, float end_z, float red, float green, float blue, float alpha);
extern void me_fx_draw_debug_cross(float x, float y, float z, float r, float g, float b, float size);
*/
void c_character_legs(CCharacter *c, CCharDesc *desc, float *pos, float *walk, float speed, float delta)
{
	float joint[3], center[3], knee[3], legs[3], *base, tmp, /*fcount,*/ lean, dist = 0, /*clearance = 0.9, length = 0.01*/ forwards = 1, side = 0, f = 0.25;
	uint  j;
	base = c->matrix[C_CM_BASE];
	lean = desc->legs_lean[0] * desc->legs_size;
	legs[0] = legs[1] = legs[2] = desc->legs_size;

	joint[0] = pos[0] + c->matrix[C_CM_BUTT][0];
	joint[1] = pos[1] + c->matrix[C_CM_BUTT][1];
	joint[2] = pos[2] + c->matrix[C_CM_BUTT][2];

	center[0] = pos[0] + base[0] * (desc->butt_size) * 0.5/* * (0.9 - desc->clearance)*/;
	center[1] = pos[1] + base[1] * (desc->butt_size) * 0.5/* * (0.9 - desc->clearance)*/;
	center[2] = pos[2] + base[2] * (desc->butt_size) * 0.5/* * (0.9 - desc->clearance)*/;

	joint[0] = c->matrix[C_CM_BUTT][12] + c->matrix[C_CM_BUTT][0];
	joint[1] = c->matrix[C_CM_BUTT][13] + c->matrix[C_CM_BUTT][1];
	joint[2] = c->matrix[C_CM_BUTT][14] + c->matrix[C_CM_BUTT][2];

	forwards = 0.5 - f;
	side = f * (1.0 - f);

	knee[0] = base[8] * forwards + base[0] * side;
	knee[1] = base[9] * forwards + base[1] * side;
	knee[2] = base[10] * forwards + base[2] * side;


	c_character_leg(&c->legs[0], joint, center, walk, &c->matrix[C_CM_BASE][4], speed, knee, legs[0], 1.0, c->matrix[C_CM_LEG_LA_UP], c->matrix[C_CM_LEG_LA_DOWN], c->legs[1].timer > 1.0, delta);

	joint[0] = pos[0] - c->matrix[C_CM_BUTT][0];
	joint[1] = pos[1] - c->matrix[C_CM_BUTT][1];
	joint[2] = pos[2] - c->matrix[C_CM_BUTT][2];
	
	center[0] = pos[0] - base[0] * (desc->butt_size) * 0.5/* * (0.9 - desc->clearance)*/;
	center[1] = pos[1] - base[1] * (desc->butt_size) * 0.5/* * (0.9 - desc->clearance)*/;
	center[2] = pos[2] - base[2] * (desc->butt_size) * 0.5/* * (0.9 - desc->clearance)*/;	
	
	
	joint[0] = c->matrix[C_CM_BUTT][12] - c->matrix[C_CM_BUTT][0];
	joint[1] = c->matrix[C_CM_BUTT][13] - c->matrix[C_CM_BUTT][1];
	joint[2] = c->matrix[C_CM_BUTT][14] - c->matrix[C_CM_BUTT][2];
	
	knee[0] = base[8] * forwards - base[0] * side;
	knee[1] = base[9] * forwards - base[1] * side;
	knee[2] = base[10] * forwards - base[2] * side;
	c_character_leg(&c->legs[1], joint, center, walk, &c->matrix[C_CM_BASE][4], speed, knee, legs[0], 1.0, c->matrix[C_CM_LEG_RA_UP], c->matrix[C_CM_LEG_RA_DOWN], c->legs[0].timer > 1.0, delta);
}

float c_character_compute_lean(CCharDesc *desc, CCharacter *c)
{
	float f, tmp[3];
	f = desc->legs_size * 0.6;
	f = desc->legs_size;
	tmp[0] = -((c->legs[0].foot_pos[0] + c->legs[1].foot_pos[0]) * 0.5 - c->matrix[C_CM_BASE][12]) / f;
	tmp[1] = -((c->legs[0].foot_pos[1] + c->legs[1].foot_pos[1]) * 0.5 - c->matrix[C_CM_BASE][13]) / f;
	tmp[2] = -((c->legs[0].foot_pos[2] + c->legs[1].foot_pos[2]) * 0.5 - c->matrix[C_CM_BASE][14]) / f;
	f = c->matrix[C_CM_BASE][4] * tmp[0] + 
		c->matrix[C_CM_BASE][5] * tmp[1] +
		c->matrix[C_CM_BASE][6] * tmp[2];

	f = (1 - f) * 0.3;
	if(f > 0.3)
		f = 0.3;
	if(f < -0.1)
		f = 0.1;
	return f;
}



void c_character_world_matrix_set(CCharacter *c, float *matrix)
{
	uint i;
	for(i = 0; i < 16; i++)
		c->matrix[C_CM_WORLD_MATRIX][i] = matrix[i];
}



void c_character_tools(CCharacter *c, CCharDesc *desc, uint tool_count, uint *tools, float *matrix)
{
	CAnimEvent *event;
	float pos[3], dir[3], up[3], f;
	uint i;

	dir[0] = c->points[C_APC_TOOL_DIR].pos[0] - c->points[C_APC_TOOL_POS].pos[0];
	dir[1] = c->points[C_APC_TOOL_DIR].pos[1] - c->points[C_APC_TOOL_POS].pos[1];
	dir[2] = c->points[C_APC_TOOL_DIR].pos[2] - c->points[C_APC_TOOL_POS].pos[2];
	f = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
	dir[0] /= f;
	dir[1] /= f;
	dir[2] /= f;
	up[0] = c->points[C_APC_TOOL_UP].pos[0] - c->points[C_APC_TOOL_POS].pos[0];
	up[1] = c->points[C_APC_TOOL_UP].pos[1] - c->points[C_APC_TOOL_POS].pos[1];
	up[2] = c->points[C_APC_TOOL_UP].pos[2] - c->points[C_APC_TOOL_POS].pos[2];
	c_character_matrix_aim(c->matrix[C_CM_TOOL], dir, up, c->points[C_APC_TOOL_POS].pos, desc->scale);

	for(i = 0; i < tool_count; i++)
	{
		event = &desc->events[tools[i]];
		if(tools[i] != c->tool_current || tools[i] != c->tool_last)
		{
			if(event->anim[C_APC_TOOL_POS].point_count != 0)
				c_character_from_matrix(pos, c, &event->anim[C_APC_TOOL_POS].points[0], C_APC_TOOL_POS, FALSE);
			else
				c_character_from_matrix(pos, c, &desc->default_pose[C_APC_TOOL_POS], C_APC_TOOL_POS, FALSE);

			if(event->anim[C_APC_TOOL_DIR].point_count != 0)
				c_character_from_matrix(dir, c, &event->anim[C_APC_TOOL_DIR].points[0], C_APC_TOOL_DIR, FALSE);
			else
				c_character_from_matrix(dir, c, &desc->default_pose[C_APC_TOOL_DIR], C_APC_TOOL_DIR, FALSE);

			if(event->anim[C_APC_TOOL_UP].point_count != 0)
				c_character_from_matrix(up, c, &event->anim[C_APC_TOOL_UP].points[0], C_APC_TOOL_UP, FALSE);
			else
				c_character_from_matrix(up, c, &desc->default_pose[C_APC_TOOL_UP], C_APC_TOOL_UP, FALSE);
			up[0] -= pos[0];
			up[1] -= pos[1];
			up[2] -= pos[2];
			dir[0] -= pos[0];
			dir[1] -= pos[1];
			dir[2] -= pos[2];
		}else
		{
			dir[0] = c->points[C_APC_TOOL_DIR].pos[0] - c->points[C_APC_TOOL_POS].pos[0];
			dir[1] = c->points[C_APC_TOOL_DIR].pos[1] - c->points[C_APC_TOOL_POS].pos[1];
			dir[2] = c->points[C_APC_TOOL_DIR].pos[2] - c->points[C_APC_TOOL_POS].pos[2];
			up[0] = c->points[C_APC_TOOL_UP].pos[0] - c->points[C_APC_TOOL_POS].pos[0];
			up[1] = c->points[C_APC_TOOL_UP].pos[1] - c->points[C_APC_TOOL_POS].pos[1];
			up[2] = c->points[C_APC_TOOL_UP].pos[2] - c->points[C_APC_TOOL_POS].pos[2];
			pos[0] = c->points[C_APC_TOOL_POS].pos[0];
			pos[1] = c->points[C_APC_TOOL_POS].pos[1];
			pos[2] = c->points[C_APC_TOOL_POS].pos[2];
		}
		f = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
		dir[0] /= f;
		dir[1] /= f;
		dir[2] /= f;
		c_character_matrix_aim(&matrix[i * 16], dir, up, pos, desc->scale);
	}
}

void c_character_animate(CCharacter *c, CCharDesc *desc, float *pos, float *upp, float *look_dir, uint tool_count, uint *tools, float *tool_matrix, uint selected_tool, float delta)
{
	float vec[3], look[3] = {1, 0, 0}, walk[3], speed, f;
	float bend = 0.3, base_lean = 0, top_lean = 0, rotate = 0;
	float *base;
	uint arm_pos = C_CM_TORSO;
	base = c->matrix[C_CM_BASE];
	f = look_dir[0] * look_dir[0] + look_dir[1] * look_dir[1] + look_dir[2] * look_dir[2];
	if(f > 0.00001)
	{
		f = sqrt(f);
		look[0] = look_dir[0] / f;
		look[1] = look_dir[1] / f;
		look[2] = look_dir[2] / f;
	}
	c_character_event_anim(desc, c, tool_count, tools, tool_matrix, selected_tool, delta);
	speed = c_character_spine_base(c, base, c->matrix[C_CM_LOOK], pos, upp, walk, look, delta);
	c_character_spine_hip(c, 0, base, &base[12], c->matrix[C_CM_BUTT], desc->legs_size, desc->butt_size, desc->butt_size, &rotate, &base_lean, &top_lean);
/*	{
		float dir[3], up[3];
		dir[0] = c->points[C_APC_TOOL_DIR].pos[0] - c->points[C_APC_TOOL_POS].pos[0];
		dir[1] = c->points[C_APC_TOOL_DIR].pos[1] - c->points[C_APC_TOOL_POS].pos[1];
		dir[2] = c->points[C_APC_TOOL_DIR].pos[2] - c->points[C_APC_TOOL_POS].pos[2];
		f = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
		dir[0] /= f;
		dir[1] /= f;
		dir[2] /= f;
		up[0] = c->points[C_APC_TOOL_UP].pos[0] - c->points[C_APC_TOOL_POS].pos[0];
		up[1] = c->points[C_APC_TOOL_UP].pos[1] - c->points[C_APC_TOOL_POS].pos[1];
		up[2] = c->points[C_APC_TOOL_UP].pos[2] - c->points[C_APC_TOOL_POS].pos[2];
		c_character_matrix_aim(c->matrix[C_CM_TOOL], dir, up, c->points[C_APC_TOOL_POS].pos, 1.0);
	}*/
	c_character_tools(c, desc, tool_count, tools, tool_matrix);

	
	vec[0] = base[12] + base[4] * (desc->stomach_size + desc->torso_size);
	vec[1] = base[13] + base[5] * (desc->stomach_size + desc->torso_size);
	vec[2] = base[14] + base[6] * (desc->stomach_size + desc->torso_size);
	c_character_spine_hands(vec, base, c->arms[0].hand[0], c->arms[0].hand[1], desc->arms_size, &bend, &base_lean, &top_lean, &rotate);

	bend = c_character_compute_lean(desc, c);
	{
		static float time = 0;
		time += delta * 3.0;
		rotate = sin(time) * 0.5;
		c_character_spine_body(c, desc, base, bend, base_lean, top_lean, rotate, look);
	}

	c_character_spine_head(c, desc, desc->neck_length * ( 1.0 - desc->neck_pos), desc->neck_length * desc->neck_pos, desc->torso_width, C_CM_TORSO, c->points[C_APC_HEAD_LOOK].pos);

	c_character_arm(&c->matrix[arm_pos][12], c->matrix[arm_pos], -0.5, c->points[C_APC_HAND_RIGHT].pos, desc->arms_size, c->points[C_APC_ARM_RIGHT].pos, c->matrix[C_CM_ARM_RA_UP], c->matrix[C_CM_ARM_RA_DOWN], vec/*&c->matrix[C_CM_TOOL][12]*/);
	c_character_arm(&c->matrix[arm_pos][12], c->matrix[arm_pos], 0.5, c->points[C_APC_HAND_LEFT].pos, desc->arms_size, c->points[C_APC_ARM_LEFT].pos, c->matrix[C_CM_ARM_LA_UP], c->matrix[C_CM_ARM_LA_DOWN], vec);
	c_character_legs(c, desc, &c->matrix[C_CM_BASE][12], walk, speed, delta);
}
