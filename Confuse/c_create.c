#include <math.h>
#include <stdio.h>
#include "forge.h"
#include "confuse.h"
 

void c_character_create_humanoid(CCharDesc *c, float size, uint seed, float alien)
{
	seed *= 10; 
	c->head_size = size * 0.030156 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->neck_length = size * 0.020156 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->neck_pos = -0.2 + 0.8 * (f_randf(seed++) * alien * 2);
	c->torso_size = size * 0.064922 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->torso_width = size * 0.044453 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->stomach_size = size * 0.054219 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->butt_size = size * 0.0350453 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->butt_width = size * 0.03 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->body_length = 0; /* not used for humans */
	c->arms_size = size * 0.152344 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->legs_size = size * 0.182344 * (f_randf(seed++) * alien * 2 + 1.0 - alien);
	c->legs_lean[0] = 1; /* not used for humans */
	c->legs_lean[1] = 1; /* not used for humans */
	c->scale = size;
	c->legs_dir = 1;

	c->default_pose[C_APC_BASE].pos[0] = 0.0;
	c->default_pose[C_APC_BASE].pos[1] = 0.0;
	c->default_pose[C_APC_BASE].pos[2] = 0.0;
	c->default_pose[C_APC_BASE].space = C_CM_BASE;
	c->default_pose[C_APC_BASE].time = 0;

	c->default_pose[C_APC_SPINE_TOP].pos[0] = 0.0;
	c->default_pose[C_APC_SPINE_TOP].pos[1] = c->torso_size * 2.0;
	c->default_pose[C_APC_SPINE_TOP].pos[2] = 0.0;
	c->default_pose[C_APC_SPINE_TOP].space = C_CM_BASE;
	c->default_pose[C_APC_SPINE_TOP].time = 0;

	c->default_pose[C_APC_SPINE_MID].pos[0] = 0.0;
	c->default_pose[C_APC_SPINE_MID].pos[1] = c->torso_size;
	c->default_pose[C_APC_SPINE_MID].pos[2] = 0.0;
	c->default_pose[C_APC_SPINE_MID].space = C_CM_BASE;
	c->default_pose[C_APC_SPINE_MID].time = 0;

	c->default_pose[C_APC_HAND_LEFT].pos[0] = 1.2;
	c->default_pose[C_APC_HAND_LEFT].pos[1] = -0.5;
	c->default_pose[C_APC_HAND_LEFT].pos[2] = 0.0;
	c->default_pose[C_APC_HAND_LEFT].space = C_CM_BUTT;
	c->default_pose[C_APC_HAND_LEFT].time = 0;

	c->default_pose[C_APC_HAND_RIGHT].pos[0] = -1.2;
	c->default_pose[C_APC_HAND_RIGHT].pos[1] = -0.5;
	c->default_pose[C_APC_HAND_RIGHT].pos[2] = 0.0;
	c->default_pose[C_APC_HAND_RIGHT].space = C_CM_BUTT;
	c->default_pose[C_APC_HAND_RIGHT].time = 0;

	c->default_pose[C_APC_ARM_LEFT].pos[0] = 1.5;
	c->default_pose[C_APC_ARM_LEFT].pos[1] = -1.0;
	c->default_pose[C_APC_ARM_LEFT].pos[2] = -1.0;
	c->default_pose[C_APC_ARM_LEFT].space = C_CM_TORSO;
	c->default_pose[C_APC_ARM_LEFT].time = 0;

	c->default_pose[C_APC_ARM_RIGHT].pos[0] = -1.5;
	c->default_pose[C_APC_ARM_RIGHT].pos[1] = -1.0;
	c->default_pose[C_APC_ARM_RIGHT].pos[2] = -1.0;
	c->default_pose[C_APC_ARM_RIGHT].space = C_CM_TORSO;
	c->default_pose[C_APC_ARM_RIGHT].time = 0;

	c->default_pose[C_APC_HEAD_UP].pos[0] = 0.0;
	c->default_pose[C_APC_HEAD_UP].pos[1] = 1.0;
	c->default_pose[C_APC_HEAD_UP].pos[2] = 0.0;
	c->default_pose[C_APC_HEAD_UP].space = C_CM_LOOK;
	c->default_pose[C_APC_HEAD_UP].time = 0;

	c->default_pose[C_APC_HEAD_LOOK].pos[0] = 0.0;
	c->default_pose[C_APC_HEAD_LOOK].pos[1] = 0.0;
	c->default_pose[C_APC_HEAD_LOOK].pos[2] = 1.0;
	c->default_pose[C_APC_HEAD_LOOK].space = C_CM_LOOK;
	c->default_pose[C_APC_HEAD_LOOK].time = 0;


	c->default_pose[C_APC_TOOL_POS].pos[0] = 0.0;
	c->default_pose[C_APC_TOOL_POS].pos[1] = -1.4;
	c->default_pose[C_APC_TOOL_POS].pos[2] = 0.0;
	c->default_pose[C_APC_TOOL_POS].space = C_CM_BUTT;
	c->default_pose[C_APC_TOOL_POS].time = 0;

	c->default_pose[C_APC_TOOL_DIR].pos[0] = -0.0;
	c->default_pose[C_APC_TOOL_DIR].pos[1] = -1.4;
	c->default_pose[C_APC_TOOL_DIR].pos[2] = 1.5;
	c->default_pose[C_APC_TOOL_DIR].space = C_CM_BUTT;
	c->default_pose[C_APC_TOOL_DIR].time = 0;

	c->default_pose[C_APC_TOOL_UP].pos[0] = 0.0;
	c->default_pose[C_APC_TOOL_UP].pos[1] = -2.4;
	c->default_pose[C_APC_TOOL_UP].pos[2] = 0.0;
	c->default_pose[C_APC_TOOL_UP].space = C_CM_BUTT;
	c->default_pose[C_APC_TOOL_UP].time = 0;
}

void c_character_desc_print(CCharDesc *c)
{
	printf("head_size = %f\n", c->head_size);
	printf("neck_length = %f\n", c->neck_length);
	printf("neck_pos = %f\n", c->neck_pos);
	printf("torso_size = %f\n", c->torso_size);
	printf("torso_width = %f\n", c->torso_width);
	printf("torso_lean = %f\n", c->torso_lean);
	printf("stomach_size = %f\n", c->stomach_size);
	printf("butt_size = %f\n", c->butt_size);
	printf("butt_width = %f\n", c->butt_width);
	printf("body_length = %f\n", c->body_length);
	printf("arms_size = %f\n", c->arms_size);
	printf("legs_size = %f\n", c->legs_size);
	printf("legs_lean[0] = %f\n", c->legs_lean[0]);
	printf("legs_lean[1] = %f\n", c->legs_lean[1]);
	printf("legs_dir = %f\n", c->legs_dir);
	printf("scale = %f\n", c->scale);
}

void c_character_print(CCharacter *c)
{
	uint i;
	for(i = 0; i < C_CM_COUNT; i++)
		printf("m0 = %f %f %f %f\nm1 = %f %f %f %f\nm2 = %f %f %f %f\nm3 = %f %f %f %f\n\n",
		c->matrix[i][0], c->matrix[i][1], c->matrix[i][2], c->matrix[i][3],
		c->matrix[i][4], c->matrix[i][5], c->matrix[i][6], c->matrix[i][7],
		c->matrix[i][8], c->matrix[i][9], c->matrix[i][10], c->matrix[i][11],
		c->matrix[i][12], c->matrix[i][13], c->matrix[i][14], c->matrix[i][15]);
}