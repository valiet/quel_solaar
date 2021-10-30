
#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "c_internal.h"
#include "relinquish.h"

void c_execute_component_module_color_correct_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *color[3], *saturation[3], *input, low, mid, high, comp[4], sat, f;
	uint i, count = 1, color_count[3], color_stride[3], saturation_count[3], saturation_stride[3], input_count, input_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[7]];
	count = type->size;
	output = &stack[type->pos];
		
	input = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_COLOR, &input_stride, &input_count)];
	color[0] = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_COLOR, &color_stride[0], &color_count[0])];
	color[1] = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_COLOR, &color_stride[1], &color_count[1])];
	color[2] = &stack[c_entity_stack_get(entity, c->references[3], C_TYPE_COLOR, &color_stride[2], &color_count[2])];
	saturation[0] = &stack[c_entity_stack_get(entity, c->references[4], C_TYPE_FLOAT, &saturation_stride[0], &saturation_count[0])];
	saturation[1] = &stack[c_entity_stack_get(entity, c->references[5], C_TYPE_FLOAT, &saturation_stride[1], &saturation_count[1])];
	saturation[2] = &stack[c_entity_stack_get(entity, c->references[6], C_TYPE_FLOAT, &saturation_stride[2], &saturation_count[2])];

	for(i = 0; i < count; i++)
	{
		f = (input[i * 4 + 0] + input[i * 4 + 1] + input[i * 4 + 2]) * 0.333333;
		low = (1.0 - f);
		low *= low;
		high = f * f;
		mid = 1.0 - (low + high);
		sat = saturation[0][(i % saturation_count[0]) * saturation_stride[0]] * low + 
			saturation[1][(i % saturation_count[1]) * saturation_stride[1]] * mid + 
			saturation[2][(i % saturation_count[2]) * saturation_stride[2]] * high;
		sat *= 2.0;

		comp[0] = color[0][(i % color_count[0]) * color_stride[0] + 0] * low * input[(i % input_count) * input_stride + 0] + 
				color[1][(i % color_count[1]) * color_stride[1] + 0] * mid * input[(i % input_count) * input_stride + 0] + 
				color[2][(i % color_count[2]) * color_stride[2] + 0] * high * input[(i % input_count) * input_stride + 0];

		comp[1] = color[0][(i % color_count[0]) * color_stride[0] + 1] * low * input[(i % input_count) * input_stride + 1] + 
				color[1][(i % color_count[1]) * color_stride[1] + 1] * mid * input[(i % input_count) * input_stride + 1] + 
				color[2][(i % color_count[2]) * color_stride[2] + 1] * high * input[(i % input_count) * input_stride + 1];

		comp[2] = color[0][(i % color_count[0]) * color_stride[0] + 2] * low * input[(i % input_count) * input_stride + 2] + 
				color[1][(i % color_count[1]) * color_stride[1] + 2] * mid * input[(i % input_count) * input_stride + 2] + 
				color[2][(i % color_count[2]) * color_stride[2] + 2] * high * input[(i % input_count) * input_stride + 2];

		comp[3] = color[0][(i % color_count[0]) * color_stride[0] + 3] * low * input[(i % input_count) * input_stride + 3] + 
				color[1][(i % color_count[1]) * color_stride[1] + 3] * mid * input[(i % input_count) * input_stride + 3] + 
				color[2][(i % color_count[2]) * color_stride[2] + 3] * high * input[(i % input_count) * input_stride + 3];
		f = (comp[0] + comp[1] + comp[2]) * 0.333333 * (1.0 - sat);
		output[i * 4 + 0] = comp[0] * sat + f;
		output[i * 4 + 1] = comp[1] * sat + f;
		output[i * 4 + 2] = comp[2] * sat + f;
		output[i * 4 + 3] = comp[3];
	}
}



void c_execute_component_module_color_contrast_func(CSession *session, CInstance *instance, CComponent *c, uint primitive)
{
	float *output, *color[2],  *input, low, mid, high, comp[4], sat, f;
	uint i, count = 1, color_count[2], color_stride[2],  input_count, input_stride;
	CType *type;
	CEntity *entity;
	float *stack;
	entity = instance->entity;
	stack = instance->stack;

	type = &entity->stack_types[c->references[3]];
	count = type->size;
	output = &stack[type->pos];
		
	input = &stack[c_entity_stack_get(entity, c->references[0], C_TYPE_COLOR, &input_stride, &input_count)];
	color[0] = &stack[c_entity_stack_get(entity, c->references[1], C_TYPE_COLOR, &color_stride[0], &color_count[0])];
	color[1] = &stack[c_entity_stack_get(entity, c->references[2], C_TYPE_COLOR, &color_stride[1], &color_count[1])];

	for(i = 0; i < count; i++)
	{
		output[i * 4 + 0] = color[0][(i % color_count[0]) * color_stride[0] + 0] + color[1][(i % color_count[1]) * color_stride[1] + 0] * input[(i % input_count) * input_stride + 0] * 4.0;
		output[i * 4 + 1] = color[0][(i % color_count[0]) * color_stride[0] + 1] + color[1][(i % color_count[1]) * color_stride[1] + 1] * input[(i % input_count) * input_stride + 1] * 4.0;
		output[i * 4 + 2] = color[0][(i % color_count[0]) * color_stride[0] + 2] + color[1][(i % color_count[1]) * color_stride[1] + 2] * input[(i % input_count) * input_stride + 2] * 4.0;
		output[i * 4 + 3] = color[0][(i % color_count[0]) * color_stride[0] + 3] + color[1][(i % color_count[1]) * color_stride[1] + 3] * input[(i % input_count) * input_stride + 3] * 4.0;
	}
}



void c_execute_component_module_color_init(CSession *session)
{
	static CTypeType color_correct_param_types[7] = {C_TYPE_COLOR, C_TYPE_COLOR, C_TYPE_COLOR, C_TYPE_COLOR, C_TYPE_FLOAT, C_TYPE_FLOAT, C_TYPE_FLOAT};
	static char *color_correct_param_names[7] = {"input", "Low Color", "Mid Color", "High Color", "Low Saturation", "Mid Saturation", "High Saturation"};
	static CTypeType color_contrast_param_types[3] = {C_TYPE_COLOR, C_TYPE_COLOR, C_TYPE_COLOR};
	static char *color_contrast_param_names[3] = {"input", "Brighness", "Contrast"};

	confuse_component_add(session, "Color Correct", color_correct_param_types, color_correct_param_names, 7, C_TYPE_COLOR, -1, c_execute_component_module_color_correct_func, C_CC_PROCESS, -1, NULL, NULL);
	confuse_component_add(session, "Color Contrast", color_contrast_param_types, color_contrast_param_names, 3, C_TYPE_COLOR, -1, c_execute_component_module_color_contrast_func, C_CC_PROCESS, -1, NULL, NULL);
}