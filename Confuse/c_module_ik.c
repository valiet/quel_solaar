
#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"
/*
void c_execute_component_module_ik_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *pos, *goal, *direction, *upper, *lower, f, x, y, *tmp_uv, tmp[4];
	uint i, count = 1, tile, animate_count, animate_stride, uv_count, uv_stride, u_divide_count, u_divide_stride, v_divide_count, v_divide_stride, spread_stride, spread_count, u, v;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[5]];
	count = type->size;
	output = &stack[type->pos];
	
	pos = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_POS, &animate_stride, &animate_count)];
	goal = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_POS, &uv_stride, &uv_count)];
	direction = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_POS, &u_divide_stride, &u_divide_count)];
	upper = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_FLOAT, &v_divide_stride, &v_divide_count)];
	lower = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &spread_stride, &spread_count)];

	for(i = 0; i < count; i += 2)
	{
	




	}
}

void c_execute_component_module_ik_init(CSession *session)
{

	static CTypeType ik_param_types[5] = {C_TYPE_POS, C_TYPE_POS, C_TYPE_POS, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *ik_param_names[5] = {"Base", "Goal", "Direction", "Uppuer", "Lower"};
	confuse_component_add(session, "IK", ik_param_types, ik_param_names, 5, C_TYPE_MATRIX, -1, c_execute_component_module_ik_func, C_CC_PROCESS, -1, NULL);
}*/