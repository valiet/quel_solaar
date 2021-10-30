#include <math.h>
#include "forge.h"
#include "confuse.h"

extern void c_character_matrix_scaled(float *matrix, float *vec, float *normal, float *pos, float scale);
extern void c_character_matrix(float *matrix, float *vec, float *normal, float *pos);
extern void c_character_matrix_aim(float *matrix, float *aim, float *normal, float *pos, float scale);

void c_character_spine_hip(CCharacter *c, uint legs, float *base, float *pos, float *matrix, float leg_length, float scale, float width, float *rotate, float *base_lean, float *top_lean)
{
	float f, side, upp, step, vec[3], *m, anim[3];
	m = c->matrix[C_CM_LEG_RA_DOWN + legs * 4];
	vec[0] = m[12] - 0.5 * m[4];
	vec[1] = m[13] - 0.5 * m[5];
	vec[2] = m[14] - 0.5 * m[6];
	side = (base[8] * vec[0] + base[9] * vec[1] + base[10] * vec[2]) / leg_length;
	m = c->matrix[C_CM_LEG_LA_DOWN + legs * 4];
	vec[0] -= m[12] - 0.5 * m[4];
	vec[1] -= m[13] - 0.5 * m[5];
	vec[2] -= m[14] - 0.5 * m[6];
	side -= (base[8] * vec[0] + base[9] * vec[1] + base[10] * vec[2]) / leg_length;
	f = c->legs[0].timer;
	if(f > 1.0)
		f = 1.0;
	upp = (1.0 - f) * 0.21 * f * c->legs[0].speed;
	f = c->legs[1].timer;
	if(f > 1.0)
		f = 1.0;
	f = (1.0 - f) * 0.21 * f * c->legs[1].speed;
	step = upp + f;
	upp -= f;
	*base_lean -= upp * 0.5;
	*top_lean += upp * 0.5;
	upp *= 0.9;
	vec[0] = base[0] + base[8] * side + base[4] * upp;
	vec[1] = base[1] + base[9] * side + base[5] * upp;
	vec[2] = base[2] + base[10] * side + base[6] * upp;

	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	matrix[0] = vec[0] / f;
	matrix[1] = vec[1] / f;
	matrix[2] = vec[2] / f;
	f = matrix[0] * base[8] + matrix[1] * base[9] + matrix[2] * base[10];
	vec[0] = base[8] - matrix[0] * f;
	vec[1] = base[9] - matrix[1] * f;
	vec[2] = base[10] - matrix[2] * f;
	f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
	matrix[8] = vec[0] / f;
	matrix[9] = vec[1] / f;
	matrix[10] = vec[2] / f;
	matrix[4] = matrix[1] * matrix[10] - matrix[2] * matrix[9];
	matrix[5] = matrix[2] * matrix[8] - matrix[0] * matrix[10];
	matrix[6] = matrix[0] * matrix[9] - matrix[1] * matrix[8];
	matrix[0] *= width;
	matrix[1] *= width;
	matrix[2] *= width;
	matrix[4] *= scale;
	matrix[5] *= scale;
	matrix[6] *= scale;
	matrix[8] *= scale;
	matrix[9] *= scale;
	matrix[10] *= scale;
	f = step;
	f *= leg_length * 0.2;
	matrix[12] = c->points[C_APC_BASE].pos[0] + base[4] * f;
	matrix[13] = c->points[C_APC_BASE].pos[1] + base[5] * f;
	matrix[14] = c->points[C_APC_BASE].pos[2] + base[6] * f;
}

void c_character_spine_body(CCharacter *c, CCharDesc *desc, float *matrix, float bend, float base_lean, float top_lean, float rotate, float *look)
{
	float f, vec[3], normal[3], pos[3], add[3] = {0, 0, 0}, *m;
	vec[0] = c->points[C_APC_HAND_RIGHT].pos[0] - c->points[C_APC_SPINE_TOP].pos[0];
	vec[1] = c->points[C_APC_HAND_RIGHT].pos[1] - c->points[C_APC_SPINE_TOP].pos[1];
	vec[2] = c->points[C_APC_HAND_RIGHT].pos[2] - c->points[C_APC_SPINE_TOP].pos[2];
	rotate = 0.15 * (vec[0] * c->matrix[C_CM_BASE][8] + vec[1] * c->matrix[C_CM_BASE][9] + vec[2] * c->matrix[C_CM_BASE][10]) / desc->torso_width;
	rotate += 0.15 * (vec[0] * c->matrix[C_CM_BASE][0] + vec[1] * c->matrix[C_CM_BASE][1] + vec[2] * c->matrix[C_CM_BASE][2]) / desc->torso_width;
	if(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] > desc->arms_size * desc->arms_size)
	{
		f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
		vec[0] /= f;
		vec[1] /= f;
		vec[2] /= f;
		add[0] = vec[0] * (f - desc->arms_size);
		add[1] = vec[1] * (f - desc->arms_size);
		add[2] = vec[2] * (f - desc->arms_size);
	}

	vec[0] = c->points[C_APC_HAND_LEFT].pos[0] - c->points[C_APC_SPINE_TOP].pos[0];
	vec[1] = c->points[C_APC_HAND_LEFT].pos[1] - c->points[C_APC_SPINE_TOP].pos[1];
	vec[2] = c->points[C_APC_HAND_LEFT].pos[2] - c->points[C_APC_SPINE_TOP].pos[2];
	rotate -= 0.15 * (vec[0] * c->matrix[C_CM_BASE][8] + vec[1] * c->matrix[C_CM_BASE][9] + vec[2] * c->matrix[C_CM_BASE][10]) / desc->torso_width;
	rotate += 0.15 * (vec[0] * c->matrix[C_CM_BASE][0] + vec[1] * c->matrix[C_CM_BASE][1] + vec[2] * c->matrix[C_CM_BASE][2]) / desc->torso_width;
	if(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2] > desc->arms_size * desc->arms_size)
	{
		f = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
		vec[0] /= f;
		vec[1] /= f;
		vec[2] /= f;
		add[0] += vec[0] * (f - desc->arms_size);
		add[1] += vec[1] * (f - desc->arms_size);
		add[2] += vec[2] * (f - desc->arms_size);
	}
	if(rotate < -0.8)
		rotate = -0.8;
	if(rotate > 0.8)
		rotate = 0.8;


	vec[0] = c->points[C_APC_SPINE_MID].pos[0] - c->matrix[C_CM_BUTT][12] + add[0] * 0.3;
	vec[1] = c->points[C_APC_SPINE_MID].pos[1] - c->matrix[C_CM_BUTT][13] + add[1] * 0.3;
	vec[2] = c->points[C_APC_SPINE_MID].pos[2] - c->matrix[C_CM_BUTT][14] + add[2] * 0.3;
	normal[0] = matrix[8] + matrix[0] * rotate;
	normal[1] = matrix[9] + matrix[1] * rotate;
	normal[2] = matrix[10] + matrix[2] * rotate;
	m = c->matrix[C_CM_BELLY];
	c_character_matrix_scaled(m, vec, normal, &c->matrix[C_CM_BUTT][12], desc->stomach_size);
	f = desc->stomach_size;
	bend *= 2.0;
	vec[0] = c->points[C_APC_SPINE_TOP].pos[0] - c->points[C_APC_SPINE_MID].pos[0] + add[0] * 0.7;
	vec[1] = c->points[C_APC_SPINE_TOP].pos[1] - c->points[C_APC_SPINE_MID].pos[1] + add[1] * 0.7;
	vec[2] = c->points[C_APC_SPINE_TOP].pos[2] - c->points[C_APC_SPINE_MID].pos[2] + add[2] * 0.7;
	normal[0] = (m[8] + m[0] * rotate) / f;
	normal[1] = (m[9] + m[1] * rotate) / f;
	normal[2] = (m[10] + m[2] * rotate) / f;
	pos[0] = m[12] + m[4];
	pos[1] = m[13] + m[5];
	pos[2] = m[14] + m[6];
	m = c->matrix[C_CM_TORSO];
	c_character_matrix_scaled(m, vec, normal, pos, desc->torso_width);
	f = (desc->torso_size / desc->torso_width);
	m[12] += m[4] * f;
	m[13] += m[5] * f;
	m[14] += m[6] * f;
	f = desc->torso_size;
	bend *= 0.5;
}

void c_character_spine_head(CCharacter *c, CCharDesc *desc,  float neck, float forward, float scale, uint base, float *look)
{
	float f, vec[3], normal[3], pos[3], *m;
	uint i;
	m = c->matrix[base];
	forward /= scale;
	neck /= scale;
	c->matrix[C_CM_HEAD][12] = m[12] + m[4] * neck + m[8] * forward;
	c->matrix[C_CM_HEAD][13] = m[13] + m[5] * neck + m[9] * forward;
	c->matrix[C_CM_HEAD][14] = m[14] + m[6] * neck + m[10] * forward;
	m = c->matrix[C_CM_HEAD];


	m[8] = look[0] - c->matrix[C_CM_HEAD][12];
	m[9] = look[1] - c->matrix[C_CM_HEAD][13];
	m[10] = look[2] - c->matrix[C_CM_HEAD][14];	
	
	f = sqrt(m[8] * m[8] + m[9] * m[9] + m[10] * m[10]);
	m[8] = m[8] / f;
	m[9] = m[9] / f;
	m[10] = m[10] / f;

	if(base == C_CM_BUTT)
		scale = -scale;

	normal[0] = c->points[C_APC_HEAD_UP].pos[0] - c->matrix[C_CM_HEAD][12];
	normal[1] = c->points[C_APC_HEAD_UP].pos[1] - c->matrix[C_CM_HEAD][13];
	normal[2] = c->points[C_APC_HEAD_UP].pos[2] - c->matrix[C_CM_HEAD][14];
	f = m[8] * normal[0] + m[9] * normal[1] + m[10] * normal[2];
	normal[0] = normal[0] - m[8] * f;
	normal[1] = normal[1] - m[9] * f;
	normal[2] = normal[2] - m[10] * f;
	f = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	m[4] = normal[0] / f;
	m[5] = normal[1] / f;
	m[6] = normal[2] / f;
	m[0] = m[10] * m[5] - m[9] * m[6];
	m[1] = m[8] * m[6] - m[10] * m[4];
	m[2] = m[9] * m[4] - m[8] * m[5];

	for(i = 0; i< 12; i++)
		m[i] *= desc->head_size;
}

void c_character_spine_hands(float *pos, float *base, float *right_hand, float *left_hand, float length, float *bend, float *base_lean, float *top_lean, float *rotate)
{
	float vec[3], vec2[3], vec3[3], p[3];
	vec[0] = left_hand[0] - pos[0];
	vec[1] = left_hand[1] - pos[1];
	vec[2] = left_hand[2] - pos[2];
	vec2[0] = base[0] * vec[0] + base[1] * vec[1] + base[2] * vec[2];
	vec2[1] = base[4] * vec[0] + base[5] * vec[1] + base[6] * vec[2];
	vec2[2] = base[8] * vec[0] + base[9] * vec[1] + base[10] * vec[2];
	vec[0] = right_hand[0] - pos[0];
	vec[1] = right_hand[1] - pos[1];
	vec[2] = right_hand[2] - pos[2];
	vec3[0] = base[0] * vec[0] + base[1] * vec[1] + base[2] * vec[2];
	vec3[1] = base[4] * vec[0] + base[5] * vec[1] + base[6] * vec[2];
	vec3[2] = base[8] * vec[0] + base[9] * vec[1] + base[10] * vec[2];
	p[0] = (vec2[0] + vec3[0]) * 0.5;
	p[1] = (vec2[1] + vec3[1]) * 0.5;
	p[2] = (vec2[2] + vec3[2]) * 0.5;
	*bend += (-(p[1] - length) / length * 0.5) * p[2] / length;
	*base_lean += p[0] / length * 0.1 - (vec3[1] - vec2[1]) * 0.1 / length;
	*top_lean += p[0] / length * 0.1 + (vec3[1] - vec2[1]) * 0.25 / length;
	*rotate += p[0] / length * 0.4 + (vec3[2] - vec2[2]) * 0.4 / length;
}

float c_character_spine_base(CCharacter *c, float *base, float *look, float *pos, float *upp, float *walk, float *look_dir, float delta)
{
	float vec[3], f, zero[3] = {0, 0, 0};
	walk[0] = (pos[0] - base[12]) / delta;
	walk[1] = (pos[1] - base[13]) / delta;
	walk[2] = (pos[2] - base[14]) / delta;

	if(delta / 5.0 > 1.0)
		delta = 1.0 / 5.0;
	c->look_dir[0] = c->look_dir[0] * (1.0 - delta * 5.0) + look_dir[0] * delta * 5.0;
	c->look_dir[1] = c->look_dir[1] * (1.0 - delta * 5.0) + look_dir[1] * delta * 5.0;
	c->look_dir[2] = c->look_dir[2] * (1.0 - delta * 5.0) + look_dir[2] * delta * 5.0;

	delta *= 10;
	if(delta > 1.0)
		delta = 1.0;
	f = delta * 0.2;
	if(walk[0] * look_dir[0] + walk[1] * look_dir[1] + walk[2] * look_dir[2] < 0)
	{
		base[8] = base[8] * (1.0 - f * 1.0) - walk[0] * f * 0.8 + look_dir[0] * f * 0.2;
		base[9] = base[9] * (1.0 - f * 1.0) - walk[1] * f * 0.8 + look_dir[1] * f * 0.2;	
		base[10] = base[10] * (1.0 - f * 1.0) - walk[2] * f * 0.8 + look_dir[2] * f * 0.2;
	}else
	{
		base[8] = base[8] * (1.0 - f * 1.0) + walk[0] * f * 0.8 + look_dir[0] * f * 0.2;
		base[9] = base[9] * (1.0 - f * 1.0) + walk[1] * f * 0.8 + look_dir[1] * f * 0.2;
		base[10] = base[10] * (1.0 - f * 1.0) + walk[2] * f * 0.8 + look_dir[2] * f * 0.2;
	}


	f = base[8] * upp[0] + base[9] * upp[1] + base[10] * upp[2];
	c_character_matrix(base, upp, &base[8], pos);
	f = sqrt(look_dir[0] * look_dir[0] + look_dir[1] * look_dir[1] + look_dir[2] * look_dir[2]);
	look_dir[0] = look_dir[0] / f;
	look_dir[1] = look_dir[1] / f;
	look_dir[2] = look_dir[2] / f;
	c_character_matrix_aim(look, look_dir, upp, zero, 1.0);
	f = walk[0] * walk[0] + walk[1] * walk[1] + walk[2] * walk[2];
	if(f > 0)
	{
		f = sqrt(f);
		walk[0] /= f;
		walk[1] /= f;
		walk[2] /= f;
		return f;
	}
	return 0;
}
