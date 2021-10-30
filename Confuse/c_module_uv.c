
#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"

void c_execute_component_module_uv_rotate_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *angle, *uv, *tmp_uv, f, x, y, tmp[4];
	uint i, count = 1, angle_count, angle_stride, uv_count, uv_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[2]];
	count = type->size;
	output = &stack[type->pos];

	angle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_FLOAT, &angle_stride, &angle_count)];

	uv = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_UV_SECTION, &uv_stride, &uv_count)];

	for(i = 0; i < count; i++)
	{
		f = angle[(i % angle_count) * angle_stride] + 0.625;
		x = (sin(f * PI * 2.0) * 1.414 + 1.0) / 2;
		y = (cos(f * PI * 2.0) * 1.414 + 1.0) / 2;
		f = 1.0 - x;
		tmp_uv = &uv[(i % uv_count) * uv_stride];
		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;

		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 0] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 1] = tmp[1] * y + tmp[3] * f;

		f = angle[(i % angle_count) * angle_stride] + 0.25 + 0.625;
		x = (sin(f * PI * 2.0) * 1.414 + 1.0) / 2;
		y = (cos(f * PI * 2.0) * 1.414 + 1.0) / 2;
		f = 1.0 - x;

		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;

		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 2] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 3] = tmp[1] * y + tmp[3] * f;

		f = angle[(i % angle_count) * angle_stride] + 0.5 + 0.625;
		x = (sin(f * PI * 2.0) * 1.414 + 1.0) / 2;
		y = (cos(f * PI * 2.0) * 1.414 + 1.0) / 2;
		f = 1.0 - x;

		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;

		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 4] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 5] = tmp[1] * y + tmp[3] * f;

		f = angle[(i % angle_count) * angle_stride] + 0.75 + 0.625;
		x = (sin(f * PI * 2.0) * 1.414 + 1.0) / 2;
		y = (cos(f * PI * 2.0) * 1.414 + 1.0) / 2;
		f = 1.0 - x;

		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;

		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 6] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 7] = tmp[1] * y + tmp[3] * f;
	}
}


void c_execute_component_module_uv_tile_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *animate, *uv, *u_divide, *v_divide, *spread, f, x, y, *tmp_uv, tmp[4];
	uint i, count = 1, tile, animate_count, animate_stride, uv_count, uv_stride, u_divide_count, u_divide_stride, v_divide_count, v_divide_stride, spread_stride, spread_count, u, v;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[5]];
	count = type->size;
	output = &stack[type->pos];
	
	animate = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_FLOAT, &animate_stride, &animate_count)];
	uv = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_UV_SECTION, &uv_stride, &uv_count)];
	u_divide = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &u_divide_stride, &u_divide_count)];
	v_divide = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &v_divide_stride, &v_divide_count)];
	spread = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &spread_stride, &spread_count)];

	for(i = 0; i < count; i++)
	{
		f = u_divide[(i % u_divide_count) * u_divide_stride];
		if(f < 1.0 / 1024.0)
			u = 1024;
		else
			u = (uint)(1.0 / f);
		f = v_divide[(i % v_divide_count) * v_divide_stride];
		if(f < 1.0 / 1024.0)
			v = 1024;
		else
			v = (uint)(1.0 / f);
		tile = (uint)((animate[(i % animate_count) * animate_stride] + spread[(i % spread_count) * spread_stride] * (float)i / (float)count) * (float)(u * v)) % (u * v);

		x = (float)(tile % u) / (float)u;
		y = (float)(tile / u) / (float)v;
		tmp_uv = &uv[(i % uv_count) * uv_stride];
		f = 1.0 - x;
		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;
		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 0] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 1] = tmp[1] * y + tmp[3] * f;

		x = (float)(tile % u + 1) / (float)u;
		y = (float)(tile / u) / (float)v;
		tmp_uv = &uv[(i % uv_count) * uv_stride];
		f = 1.0 - x;
		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;
		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 2] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 3] = tmp[1] * y + tmp[3] * f;


		x = (float)(tile % u + 1) / (float)u;
		y = (float)(tile / u + 1) / (float)v;
		tmp_uv = &uv[(i % uv_count) * uv_stride];
		f = 1.0 - x;
		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;
		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 4] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 5] = tmp[1] * y + tmp[3] * f;


		x = (float)(tile % u) / (float)u;
		y = (float)(tile / u + 1) / (float)v;
		tmp_uv = &uv[(i % uv_count) * uv_stride];
		f = 1.0 - x;
		tmp[0] = tmp_uv[0] * x + tmp_uv[2] * f;
		tmp[1] = tmp_uv[1] * x + tmp_uv[3] * f;
		tmp[2] = tmp_uv[6] * x + tmp_uv[4] * f;
		tmp[3] = tmp_uv[7] * x + tmp_uv[5] * f;
		f = 1.0 - y;
		output[i * 8 + 6] = tmp[0] * y + tmp[2] * f;
		output[i * 8 + 7] = tmp[1] * y + tmp[3] * f;

	}
}

extern float c_primitives_matrix[9];

void c_execute_component_module_particle_uv_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *particle, *p, *uv, *speed, *size, f, x, y, *tmp, vec[2];
	uint i, count = 1, tile, particle_count, particle_stride, uv_count, uv_stride, speed_stride, speed_count, size_stride, size_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[4]];
	count = type->size;
	output = &stack[type->pos];
	
	particle = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_PARTICLE, &particle_stride, &particle_count)];
	uv = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_UV_SECTION, &uv_stride, &uv_count)];
	speed = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_FLOAT, &speed_stride, &speed_count)];
	size = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_AREA, &size_stride, &size_count)];

	for(i = 0; i < count; i++)
	{
		p = &particle[(i % particle_count) * particle_stride];
		if(p[6] < 0.001)
		{
			tmp = &uv[(i % uv_count) * uv_stride];
			output[i * 8] = tmp[0];
			output[i * 8 + 1] = tmp[1];
			output[i * 8 + 2] = tmp[2];
			output[i * 8 + 3] = tmp[3];
			output[i * 8 + 4] = tmp[4];
			output[i * 8 + 5] = tmp[5];
			output[i * 8 + 6] = tmp[6];
			output[i * 8 + 7] = tmp[7];
		}else
		{
			vec[0] = c_primitives_matrix[0] * p[3] + c_primitives_matrix[1] * p[4] + c_primitives_matrix[2] * p[5];
			vec[1] = c_primitives_matrix[3] * p[3] + c_primitives_matrix[4] * p[4] + c_primitives_matrix[5] * p[5];
			f = session->delta_time * speed[(i % speed_count) % speed_stride];
			tmp = &size[(i % size_count) * size_stride];
			vec[0] = vec[0] * f / tmp[0];
			vec[1] = vec[1] * f / tmp[1];
			output[i * 8 + 0] += vec[0];
			output[i * 8 + 1] += vec[1];
			output[i * 8 + 2] += vec[0];
			output[i * 8 + 3] += vec[1];
			output[i * 8 + 4] += vec[0];
			output[i * 8 + 5] += vec[1];
			output[i * 8 + 6] += vec[0];
			output[i * 8 + 7] += vec[1];
		}
	}
}


void c_execute_component_module_uv_turn_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *uv, f, x, y, *tmp_uv, tmp[4];
	uint i, count = 1, tile, u, v, uv_stride, uv_count;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[1]];
	count = type->size;
	output = &stack[type->pos];
	
	uv = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_UV_SECTION, &uv_stride, &uv_count)];
	for(i = 0; i < count; i++)
	{
		tmp_uv = &uv[(i % uv_count) * uv_stride];
		switch(i % 8)
		{
			case 0 :
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
			break;
			case 1 :
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
			break;
			case 2 :
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
			break;
			case 3 :
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
			break;
			case 4 :
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
			break;
			case 5 :
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
			break;
			case 6 :
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
			break;
			case 7 :
				*output++ = tmp_uv[2];
				*output++ = tmp_uv[3];
				*output++ = tmp_uv[0];
				*output++ = tmp_uv[1];
				*output++ = tmp_uv[6];
				*output++ = tmp_uv[7];
				*output++ = tmp_uv[4];
				*output++ = tmp_uv[5];
			break;
		}
	}
}

void c_execute_component_module_uv_init(CSession *session)
{
	static CTypeType uv_rotate_param_types[2] = {C_TYPE_FLOAT, C_TYPE_UV_SECTION};
	static char *uv_rotate_param_names[2] = {"Driver", "UV"};
	static CTypeType uv_tile_param_types[5] = {C_TYPE_FLOAT, C_TYPE_UV_SECTION, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *uv_tile_param_names[5] = {"Animate", "Section", "U Divide", "V Divide", "Spread"};
	static CTypeType uv_flip_param_types = C_TYPE_UV_SECTION;
	static char *uv_flip_param_names = "UV";
	static CTypeType uv_particle_param_types[4] = {C_TYPE_PARTICLE, C_TYPE_UV_SECTION, C_TYPE_FLOAT, C_TYPE_AREA};
	static char *uv_particle_param_names[4] = {"Particles", "Start", "Speed", "Scale"};
	confuse_component_add(session, "Rotate", uv_rotate_param_types, uv_rotate_param_names, 2, C_TYPE_UV_SECTION, -1, c_execute_component_module_uv_rotate_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "Tile", uv_tile_param_types, uv_tile_param_names, 5, C_TYPE_UV_SECTION, -1, c_execute_component_module_uv_tile_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "Flip", &uv_flip_param_types, &uv_flip_param_names, 1, C_TYPE_UV_SECTION, -1, c_execute_component_module_uv_turn_func, C_CC_PROCESS, -1, NULL);
	confuse_component_add(session, "Particle", &uv_particle_param_types, &uv_particle_param_names, 4, C_TYPE_UV_SECTION, -1, c_execute_component_module_particle_uv_func, C_CC_PROCESS, -1, NULL);
}