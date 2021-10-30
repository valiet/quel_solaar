#include <math.h>
#include <stdlib.h>
#include "forge.h"
#include "c_internal.h"
/*
void c_animate_type_blend(CTypeData *out_data, CTypeData *a_data, CTypeData *b_data, CTypeType type, float blend)
{
	switch(type)
	{
		case C_TYPE_FLOAT_ZERO_ONE :
		case C_TYPE_FLOAT_MINUS_ONE_ONE :
		case C_TYPE_FLOAT_INF :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			return;
		case C_TYPE_INT_COUNT :
			out_data->integer = (int)((float)a_data->integer + ((float)b_data->integer - (float)a_data->integer) * blend);
			return;
		case C_TYPE_INT_OPTION :
			if(blend < 0.5)
				out_data->integer = a_data->integer;
			else
				out_data->integer = b_data->integer;
			return;
		case C_TYPE_COLOR_RGB :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			out_data->real[1] = a_data->real[1] + (b_data->real[1] - a_data->real[1]) * blend;
			out_data->real[2] = a_data->real[2] + (b_data->real[2] - a_data->real[2]) * blend;
			return;
		case C_TYPE_COLOR_RGBA :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			out_data->real[1] = a_data->real[1] + (b_data->real[1] - a_data->real[1]) * blend;
			out_data->real[2] = a_data->real[2] + (b_data->real[2] - a_data->real[2]) * blend;
			out_data->real[3] = a_data->real[3] + (b_data->real[3] - a_data->real[3]) * blend;
			return;
		case C_TYPE_POS :
		case C_TYPE_VECTOR :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			out_data->real[1] = a_data->real[1] + (b_data->real[1] - a_data->real[1]) * blend;
			out_data->real[2] = a_data->real[2] + (b_data->real[2] - a_data->real[2]) * blend;
			return;
		case C_TYPE_NORMAL :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			out_data->real[1] = a_data->real[1] + (b_data->real[1] - a_data->real[1]) * blend;
			out_data->real[2] = a_data->real[2] + (b_data->real[2] - a_data->real[2]) * blend;
			f_normalize3f(out_data->real);
			return;
		case C_TYPE_POS_SCALE_QUATERNION :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			out_data->real[1] = a_data->real[1] + (b_data->real[1] - a_data->real[1]) * blend;
			out_data->real[2] = a_data->real[2] + (b_data->real[2] - a_data->real[2]) * blend;

			out_data->real[3] = a_data->real[3] + (b_data->real[3] - a_data->real[3]) * blend;
			out_data->real[4] = a_data->real[4] + (b_data->real[4] - a_data->real[4]) * blend;
			out_data->real[5] = a_data->real[5] + (b_data->real[5] - a_data->real[5]) * blend;

			out_data->real[6] = a_data->real[6] + (b_data->real[6] - a_data->real[6]) * blend;
			out_data->real[7] = a_data->real[7] + (b_data->real[7] - a_data->real[7]) * blend;
			out_data->real[8] = a_data->real[8] + (b_data->real[8] - a_data->real[8]) * blend;
			out_data->real[9] = a_data->real[9] + (b_data->real[9] - a_data->real[9]) * blend;
			f_normalize4f(&out_data->real[6]);
			return;
		case C_TYPE_MATRIX :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			out_data->real[1] = a_data->real[1] + (b_data->real[1] - a_data->real[1]) * blend;
			out_data->real[2] = a_data->real[2] + (b_data->real[2] - a_data->real[2]) * blend;
			out_data->real[3] = a_data->real[3] + (b_data->real[3] - a_data->real[3]) * blend;
			out_data->real[4] = a_data->real[4] + (b_data->real[4] - a_data->real[4]) * blend;
			out_data->real[5] = a_data->real[5] + (b_data->real[5] - a_data->real[5]) * blend;
			out_data->real[6] = a_data->real[6] + (b_data->real[6] - a_data->real[6]) * blend;
			out_data->real[7] = a_data->real[7] + (b_data->real[7] - a_data->real[7]) * blend;
			out_data->real[8] = a_data->real[8] + (b_data->real[8] - a_data->real[8]) * blend;
			out_data->real[9] = a_data->real[9] + (b_data->real[9] - a_data->real[9]) * blend;
			out_data->real[10] = a_data->real[10] + (b_data->real[10] - a_data->real[10]) * blend;
			out_data->real[11] = a_data->real[11] + (b_data->real[11] - a_data->real[11]) * blend;
			out_data->real[12] = a_data->real[12] + (b_data->real[12] - a_data->real[12]) * blend;
			out_data->real[13] = a_data->real[13] + (b_data->real[13] - a_data->real[13]) * blend;
			out_data->real[14] = a_data->real[14] + (b_data->real[14] - a_data->real[14]) * blend;
			out_data->real[15] = a_data->real[15] + (b_data->real[15] - a_data->real[15]) * blend;
			return;
		case C_TYPE_UV_SECTION :
			out_data->real[0] = a_data->real[0] + (b_data->real[0] - a_data->real[0]) * blend;
			out_data->real[1] = a_data->real[1] + (b_data->real[1] - a_data->real[1]) * blend;
			out_data->real[2] = a_data->real[2] + (b_data->real[2] - a_data->real[2]) * blend;
			out_data->real[3] = a_data->real[3] + (b_data->real[3] - a_data->real[3]) * blend;
			return;
		case C_TYPE_TEXTURE_ASSET :
		case C_TYPE_GEOMETRY_ASSET :
			if(blend < 0.5)
				out_data->integer = a_data->integer;
			else
				out_data->integer = b_data->integer;
			return;
	}
}*/

/*
void c_animate_evaluate_type(CType *t, CTypeData *out_data)
{
	CTypeData data, a, b;
	CType *keys;
	float time = 0;
	if(t->keys != NULL)
	{
		uint i;
		c_animate_evaluate_type(t->driver, &data);
		time = data.real[0];
		keys = t->keys;
		for(i = 0; i < t->key_count && keys[i].key_time < time; i++);
		if(i == 0)
			c_animate_evaluate_type(keys, &t->data);
		else if(i < t->key_count)
			c_animate_evaluate_type(&keys[t->key_count - 1], &t->data);
		else
		{
			float inv, in_ease, out_ease;
			c_animate_evaluate_type(&keys[i - 1], &a);
			out_ease = keys[i - 1].key_ease[1];
			c_animate_evaluate_type(&keys[i], &b);
			in_ease = keys[i].key_ease[0];
			time -= keys[i - 1].key_time;
			time /= keys[i].key_time - keys[i - 1].key_time;
			inv = 1.0f - time;
			time =  ((1 * time + out_ease * inv) *time + (out_ease * time + in_ease * inv) * inv) * time + ((out_ease * time + in_ease * inv) * time + (in_ease * time) * inv) * inv;
			c_animate_type_blend(&t->data, &a, &b, t->type, time);
		}
	}
	if(t->dependency_func != NULL)
		t->dependency_func(out_data, &t->data, t->type, t->dependency_user);
	else
		*out_data = t->data;
}
*/
