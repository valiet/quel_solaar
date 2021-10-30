#include <math.h>
#include <stdlib.h>
#include "seduce.h"
#include "s_draw_3d.h"

extern SViewData sui_default_view;

RMatrix *r_matrix_get();
extern void seduce_widget_overlay_matrix(RMatrix *matrix);
extern void seduce_object_3d_draw(BInputState *input, float pos_x, float pos_y, float pos_z, float scale, uint id, float fade,  float *color);
/*
void seduce_manipulator_matrix_clear_and_clone(RMatrix *matrix, RMatrix *clone)
{
	uint m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	uint i;
	if(clone == NULL)
		clone = r_matrix_get();
	for(i = 0; i < 16; i++)
	{
		matrix->matrix[0][i] = m[i];
		matrix->projection[i] = clone->projection[i];
	}
	matrix->current = 0;
}*/

typedef struct{
	void *id;
	boolean active;
	float timer;
	uint part;
	float grab_pos[3];
	RMatrix matrix;
}SeduceManipulatorGrab;

boolean	seduce_manipulator_point(BInputState *input, RMatrix *m, float *pos, void *id, float scale)
{
	static boolean *a_button, *a_button_last;
	static SeduceManipulatorGrab *grab = NULL;
	static uint frame_id = -1;
	RMatrix *reset;
	float tmp[3], f, matrix[16];
	uint i, count, active_pointer = -1, pointer_count, user_count;

	reset = r_matrix_get();
	if(m == NULL)
		m = reset;

	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);

	if(grab == NULL)
	{
		count = betray_support_functionality(B_SF_USER_COUNT_MAX);
		a_button = malloc((sizeof *a_button) * count);
		a_button_last = malloc((sizeof *a_button_last) * count);
		for(i = 0; i < count; i++)
			a_button[i] = a_button_last[i] = FALSE;
		count += betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
			grab[i].id = NULL;
	}

	r_matrix_projection_screenf(m, tmp, pos[0], pos[1], pos[2]);
	if(input->mode == BAM_DRAW && tmp[2] < 0)
	{
		float color[4] = {0.2, 0.6, 1.0, 1.0};
		static float time = 0;
		boolean active = FALSE;
		RMatrix draw_matrix;
		seduce_widget_overlay_matrix(&draw_matrix);

		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].id == id && grab[i].active)
				active = TRUE;
			else if(id == seduce_element_pointer_id(input, i, NULL))
			{
				grab[i].matrix = *m;
				active = TRUE;
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				active = TRUE;	
				grab[i + pointer_count].matrix = *m;
			}
		}
		if(active)
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.025, SUI_3D_OBJECT_MOVE_3D, 1, color);
		else
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.025, SUI_3D_OBJECT_MOVE_3D, 1, NULL);
		r_matrix_set(m);
		seduce_element_add_point(input, id, 0, pos, scale * 0.025);
		r_matrix_set(reset);
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(!input->pointers[i].button[0])
				grab[i].active = FALSE;
			if(grab[i].id == id && grab[i].active)
			{	
				r_matrix_projection_vertexf(&grab[i].matrix, tmp, pos, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
				pos[0] = tmp[0] + grab[i].grab_pos[0];
				pos[1] = tmp[1] + grab[i].grab_pos[1];
				pos[2] = tmp[2] + grab[i].grab_pos[2];
			}else if(!grab[i].active)
			{
				if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				{
					if(id == seduce_element_pointer_id(input, i, NULL))
					{
						grab[i].active = TRUE;
						grab[i].id = id;
						grab[i].timer = 0.1;
						r_matrix_projection_vertexf(&grab[i].matrix, tmp, pos, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
						grab[i].grab_pos[0] = pos[0] - tmp[0];
						grab[i].grab_pos[1] = pos[1] - tmp[1];
						grab[i].grab_pos[2] = pos[2] - tmp[2];
					}
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			float pos[3];
			if(id == seduce_element_selected_id(i, pos, NULL))
			{
				betray_button_get_up_down(i, &a_button[i], &a_button_last[i], BETRAY_BUTTON_FACE_A);
				if(!a_button[i])
					grab[i + pointer_count].active = FALSE;

				if(grab[i + pointer_count].id == id && grab[i + pointer_count].active)
				{
					uint axis;
					axis = seduce_element_primary_axis(input, i);
					if(axis != -1)
					{
						r_matrix_projection_screenf(&grab[i + pointer_count].matrix, tmp, pos[0], pos[1], pos[2]);
						tmp[0] += input->axis[axis].axis[0] * input->delta_time;
						tmp[1] += input->axis[axis].axis[1] * input->delta_time;
						r_matrix_projection_worldf(&grab[i + pointer_count].matrix, tmp, tmp[0], tmp[1], tmp[2]);
						pos[0] = tmp[0];
						pos[1] = tmp[1];
						pos[2] = tmp[2];
					}
				}else if(!grab[i + pointer_count].active)
				{
					if(a_button[i] && !a_button_last[i])
					{
						grab[i + pointer_count].active = TRUE;
						grab[i + pointer_count].id = id;
						grab[i + pointer_count].timer = 0.1;
					}		
				}
			}
		}
	}

	if(input->mode == BAM_MAIN && frame_id != input->frame_number)
	{
		frame_id = input->frame_number;
		for(i = 0; i < (pointer_count + user_count); i++)
		{
			if(grab[i].active)
			{
				grab[i].timer += input->delta_time * 2.5;
				if(grab[i].timer > 1.0)		
					grab[i].timer = 1.0;
			}else
				grab[i].timer -= input->delta_time * 2.5;
		}
	}

	for(i = 0; i < (pointer_count + user_count); i++)
		if(grab[i].id == id && grab[i].active)
			return TRUE;
	return FALSE;
}

boolean	seduce_manipulator_pos_xyz(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, boolean x, boolean y, boolean z, float scale, float time)
{
	static boolean *a_button, *a_button_last;
	static SeduceManipulatorGrab *grab = NULL;
	static uint frame_id = -1;
	float tmp[3], tmp2[3], f, matrix[16];
	uint i, count, active_pointer = -1, pointer_count, user_count;
	if(m == NULL)
		m = r_matrix_get();

	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);

	if(grab == NULL)
	{
		count = betray_support_functionality(B_SF_USER_COUNT_MAX);
		a_button = malloc((sizeof *a_button) * count);
		a_button_last = malloc((sizeof *a_button_last) * count);
		for(i = 0; i < count; i++)
			a_button[i] = a_button_last[i] = FALSE;
		count += betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
			grab[i].id = NULL;
	}

	r_matrix_projection_screenf(m, tmp, pos[0], pos[1], pos[2]);
	if(input->mode == BAM_DRAW && tmp[2] < 0)
	{
		float flip[3], a[3] = {-0.16, 0.0, 0.0}, b[3] = {-0.03, 0.0, 0.0}, color[5] = {0, 0.3, 1, 0, 0}, t;
		uint part = -1;
		boolean parts[3] = {FALSE, FALSE, FALSE};
		RMatrix *reset;
		reset = r_matrix_get();

		t = 1.0 - time;
		t = t * t * -2.0 + t; 
		r_matrix_projection_worldf(m, flip, 0, 0, 0);
		for(i = 0; i < 3; i++)
		{
			flip[i] -= pos[i];
			if(flip[i] > 0)
				flip[i] = -1.0;
			else
				flip[i] = 1.0;
		}
		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].id == id && grab[i].active)
				parts[grab[i].part] = TRUE;
			else if(id == seduce_element_pointer_id(input, i, &part))
				parts[part] = TRUE;
			if(id == seduce_element_pointer_id(input, i, NULL))
				 grab[i].matrix = *m;
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, &part))
			{
				parts[part] = TRUE;
				grab[i + pointer_count].matrix = *m;
			}
		}
		a[0] = scale * (0.04 * tmp[2] + 0.025 * -tmp[2]);
		b[0] = scale * (0.04 * tmp[2] - 0.025 * -tmp[2]);
		r_matrix_set(m);
		r_matrix_push(m);
		r_matrix_translate(m, pos[0], pos[1], pos[2]);
		r_matrix_normalize_scale(m);
		r_matrix_scale(m, flip[0], flip[1], flip[2]);
		if(x)
		{
			r_matrix_push(m);
			r_matrix_rotate(m, 3600.0 * input->minute_time + 180.0 * t, 1, 0, 0);
			if(parts[0])
				seduce_object_3d_draw(input, scale * 0.04 * tmp[2] + t, 0, 0, scale * 0.05 * -tmp[2], SUI_3D_OBJECT_AXIS_HANDLE, 1, &color[2]);
			else
				seduce_object_3d_draw(input, scale * 0.04 * tmp[2] + t, 0, 0, scale * 0.05 * -tmp[2], SUI_3D_OBJECT_AXIS_HANDLE, 1, NULL);
			seduce_element_add_line(input, id, 0, a, b, scale * 0.1);
			r_matrix_pop(m);
		}
		r_matrix_rotate(m, 90, 0, 0, 1);
		if(y)
		{
			r_matrix_push(m);
			r_matrix_rotate(m, 3600.0 * input->minute_time + 180.0 * t, 1, 0, 0);
			if(parts[1])
				seduce_object_3d_draw(input, scale * 0.04 * tmp[2] + t, 0, 0, scale * 0.05 * -tmp[2], SUI_3D_OBJECT_AXIS_HANDLE, 1, &color[1]);
			else
				seduce_object_3d_draw(input, scale * 0.04 * tmp[2] + t, 0, 0, scale * 0.05 * -tmp[2], SUI_3D_OBJECT_AXIS_HANDLE, 1, NULL);
			seduce_element_add_line(input, id, 1, a, b, scale * 0.1);
			r_matrix_pop(m);
		}
		if(z)
		{
			r_matrix_rotate(m, -90, 0, 1, 0);
			r_matrix_push(m);
			r_matrix_rotate(m, 3600.0 * input->minute_time + 180.0 * t, 1, 0, 0);
			if(parts[2])
				seduce_object_3d_draw(input, scale * 0.04 * tmp[2] + t, 0, 0, scale * 0.05 * -tmp[2], SUI_3D_OBJECT_AXIS_HANDLE, 1, &color[0]);
			else
				seduce_object_3d_draw(input, scale * 0.04 * tmp[2] + t, 0, 0, scale * 0.05 * -tmp[2], SUI_3D_OBJECT_AXIS_HANDLE, 1, NULL);
			seduce_element_add_line(input, id, 2, a, b, scale * 0.1);
			r_matrix_pop(m);
		}
		r_matrix_pop(m);
		r_matrix_set(reset);
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(!input->pointers[i].button[0])
				grab[i].active = FALSE;
			if(grab[i].id == id && grab[i].active)
			{
				if(snap != NULL && snap_active)
					pos[grab[i].part] = snap[grab[i].part];
				else if(r_matrix_projection_axisf(&grab[i].matrix, tmp, pos, grab[i].part, input->pointers[i].pointer_x, input->pointers[i].pointer_y))
				{
					pos[0] = tmp[0] + grab[i].grab_pos[0];
					pos[1] = tmp[1] + grab[i].grab_pos[1];
					pos[2] = tmp[2] + grab[i].grab_pos[2];
				}
			}else if(!grab[i].active)
			{
				if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				{
					if(id == seduce_element_pointer_id(input, i, &grab[i].part))
					{
						grab[i].active = TRUE;
						grab[i].id = id;
						grab[i].timer = 0.1;
						r_matrix_projection_axisf(&grab[i].matrix, tmp, pos, grab[i].part, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
						grab[i].grab_pos[0] = pos[0] - tmp[0];
						grab[i].grab_pos[1] = pos[1] - tmp[1];
						grab[i].grab_pos[2] = pos[2] - tmp[2];
						
					}
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			uint part;
			if(id == seduce_element_selected_id(i, NULL, &part))
			{
				betray_button_get_up_down(i, &a_button[i], &a_button_last[i], BETRAY_BUTTON_FACE_A);
				if(!a_button[i])
					grab[i + pointer_count].active = FALSE;

				if(grab[i + pointer_count].id == id && grab[i + pointer_count].active)
				{
					if(snap != NULL && snap_active)
						pos[grab[i + pointer_count].part] = snap[grab[i + pointer_count].part];
					else 
					{
						uint axis;
						axis = seduce_element_primary_axis(input, i);
						if(axis != -1)
						{
							float vec[3] = {0, 0, 0}, dist;
							dist = tmp[2];
							vec[grab[i + pointer_count].part] = -0.01 * dist;
							r_matrix_projection_screenf(&grab[i + pointer_count].matrix, tmp, pos[0] - vec[0], pos[1] - vec[1], pos[2] - vec[2]);
							r_matrix_projection_screenf(&grab[i + pointer_count].matrix, tmp2, pos[0] + vec[0], pos[1] + vec[1], pos[2] + vec[2]);
							tmp[0] -= tmp2[0];
							tmp[1] -= tmp2[1];
							f_normalize2f(tmp);
							f = tmp[0] * input->axis[axis].axis[0] + tmp[1] * input->axis[axis].axis[1];
							pos[grab[i + pointer_count].part] += f * input->delta_time * dist;
				
						}
					}
				}else if(!grab[i + pointer_count].active)
				{
					if(a_button[i] && !a_button_last[i])
					{
						grab[i + pointer_count].active = TRUE;
						grab[i + pointer_count].id = id;
						grab[i + pointer_count].part = part;
						grab[i + pointer_count].timer = 0.1;
					}		
				}
			}
		}
	}

	if(input->mode == BAM_MAIN && frame_id != input->frame_number)
	{
		frame_id = input->frame_number;
		for(i = 0; i < (pointer_count + user_count); i++)
		{
			if(grab[i].active)
			{
				grab[i].timer += input->delta_time * 2.5;
				if(grab[i].timer > 1.0)		
					grab[i].timer = 1.0;
			}else
				grab[i].timer -= input->delta_time * 2.5;
		}
	}

	for(i = 0; i < (pointer_count + user_count); i++)
		if(grab[i].id == id && grab[i].active)
			return TRUE;
	return FALSE;
}


boolean	seduce_manipulator_point_plane(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, uint axis, float scale)
{
	static boolean *a_button, *a_button_last;
	static SeduceManipulatorGrab *grab = NULL;
	static uint frame_id = -1;
	RMatrix *reset;
	float tmp[3], f, matrix[16];
	uint i, count, active_pointer = -1, pointer_count, user_count;

	reset = r_matrix_get();
	if(m == NULL)
		m = reset;

	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);

	if(grab == NULL)
	{
		count = betray_support_functionality(B_SF_USER_COUNT_MAX);
		a_button = malloc((sizeof *a_button) * count);
		a_button_last = malloc((sizeof *a_button_last) * count);
		for(i = 0; i < count; i++)
			a_button[i] = a_button_last[i] = FALSE;
		count += betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
			grab[i].id = NULL;
	}

	r_matrix_projection_screenf(m, tmp, pos[0], pos[1], pos[2]);
	if(input->mode == BAM_DRAW && tmp[2] < 0)
	{
		float color[4] = {0.2, 0.6, 1.0, 1.0};
		static float time = 0;
		boolean active = FALSE;
		RMatrix draw_matrix;
		reset = r_matrix_get();
		seduce_widget_overlay_matrix(&draw_matrix);

		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].id == id && grab[i].active)
				active = TRUE;
			else if(id == seduce_element_pointer_id(input, i, NULL))
				active = TRUE;
			if(id == seduce_element_pointer_id(input, i, NULL))
				grab[i].matrix = *m;
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				active = TRUE;
				grab[i + pointer_count].matrix = *m;
			}
		}
		if(active)
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.025, SUI_3D_OBJECT_MOVE_3D, 1, color);
		else
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.025, SUI_3D_OBJECT_MOVE_3D, 1, NULL);
		r_matrix_set(m);
		seduce_element_add_point(input, id, 0, pos, scale * 0.025);
		r_matrix_set(reset);
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(!input->pointers[i].button[0])
				grab[i].active = FALSE;
			if(grab[i].id == id && grab[i].active)
			{	
				if(snap != NULL && snap_active)
				{
					pos[(axis + 1) % 3] = snap[(axis + 1) % 3];
					pos[(axis + 2) % 3] = snap[(axis + 2) % 3];
				}else
				{
					if(r_matrix_projection_surfacef(&grab[i].matrix, tmp, pos, axis, input->pointers[i].pointer_x, input->pointers[i].pointer_y))
					{
						pos[0] = tmp[0] + grab[i].grab_pos[0];
						pos[1] = tmp[1] + grab[i].grab_pos[1];
						pos[2] = tmp[2] + grab[i].grab_pos[2];
						pos[0] = tmp[0];
						pos[1] = tmp[1];
						pos[2] = tmp[2];
					}
				}
			}else if(!grab[i].active)
			{
				if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				{
					if(id == seduce_element_pointer_id(input, i, NULL))
					{
						if(r_matrix_projection_surfacef(&grab[i].matrix, tmp, pos, axis, input->pointers[i].pointer_x, input->pointers[i].pointer_y))
						{
							grab[i].active = TRUE;
							grab[i].id = id;
							grab[i].timer = 0.1;
							grab[i].grab_pos[0] = pos[0] - tmp[0];
							grab[i].grab_pos[1] = pos[1] - tmp[1];
							grab[i].grab_pos[2] = pos[2] - tmp[2];
						}
					}
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				betray_button_get_up_down(i, &a_button[i], &a_button_last[i], BETRAY_BUTTON_FACE_A);
				if(!a_button[i])
					grab[i + pointer_count].active = FALSE;

				if(grab[i + pointer_count].id == id && grab[i + pointer_count].active)
				{
					uint axis;
					axis = seduce_element_primary_axis(input, i);
					if(axis != -1)
					{
						r_matrix_projection_screenf(&grab[i + pointer_count].matrix, tmp, pos[0], pos[1], pos[2]);
						tmp[0] += input->axis[axis].axis[0] * input->delta_time;
						tmp[1] += input->axis[axis].axis[1] * input->delta_time;
						if(r_matrix_projection_surfacef(&grab[i + pointer_count].matrix, tmp, pos, axis, tmp[0], tmp[1]))
						{
							pos[0] = tmp[0];
							pos[1] = tmp[1];
							pos[2] = tmp[2];
						}
					}
				}else if(!grab[i + pointer_count].active)
				{
					if(a_button[i] && !a_button_last[i])
					{
						grab[i + pointer_count].active = TRUE;
						grab[i + pointer_count].id = id;
						grab[i + pointer_count].timer = 0.1;
					}		
				}
			}
		}
	}

	if(input->mode == BAM_MAIN && frame_id != input->frame_number)
	{
		frame_id = input->frame_number;
		for(i = 0; i < (pointer_count + user_count); i++)
		{
			if(grab[i].active)
			{
				grab[i].timer += input->delta_time * 2.5;
				if(grab[i].timer > 1.0)		
					grab[i].timer = 1.0;
			}else
				grab[i].timer -= input->delta_time * 2.5;
		}
	}

	for(i = 0; i < (pointer_count + user_count); i++)
		if(grab[i].id == id && grab[i].active)
			return TRUE;
	return FALSE;
}

boolean seduce_manipulator_point_axis(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, uint axis, float scale)
{
	static boolean *a_button, *a_button_last;
	static SeduceManipulatorGrab *grab = NULL;
	static uint frame_id = -1;
	RMatrix *reset;
	float tmp[3], f;
	uint i, count, active_pointer = -1, pointer_count, user_count;

	reset = r_matrix_get();
	if(m == NULL)
		m = reset;

	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);

	if(grab == NULL)
	{
		count = betray_support_functionality(B_SF_USER_COUNT_MAX);
		a_button = malloc((sizeof *a_button) * count);
		a_button_last = malloc((sizeof *a_button_last) * count);
		for(i = 0; i < count; i++)
			a_button[i] = a_button_last[i] = FALSE;
		count += betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
			grab[i].id = NULL;
	}

	r_matrix_projection_screenf(m, tmp, pos[0], pos[1], pos[2]);
	if(input->mode == BAM_DRAW && tmp[2] < 0)
	{
		float color[4] = {0.2, 0.6, 1.0, 1.0};
		static float time = 0;
		boolean active = FALSE;
		RMatrix draw_matrix;
		reset = r_matrix_get();
		seduce_widget_overlay_matrix(&draw_matrix);

		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].id == id && grab[i].active)
				active = TRUE;
			else if(id == seduce_element_pointer_id(input, i, NULL))
				active = TRUE;
			if(id == seduce_element_pointer_id(input, i, NULL))
				grab[i].matrix = *m;
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				active = TRUE;
				grab[i + pointer_count].matrix = *m;
			}
		}
		if(active)
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.025, SUI_3D_OBJECT_MOVE_3D, 1, color);
		else
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.025, SUI_3D_OBJECT_MOVE_3D, 1, NULL);
		r_matrix_set(m);
		seduce_element_add_point(input, id, 0, pos, scale * 0.025);
		r_matrix_set(reset);

	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(!input->pointers[i].button[0])
				grab[i].active = FALSE;
			if(grab[i].id == id && grab[i].active)
			{	
				if(snap != NULL && snap_active)
				{
					pos[axis] = snap[axis];
				}else
				{
					r_matrix_projection_axisf(&grab[i].matrix, tmp, pos, axis, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
					pos[0] = tmp[0] + grab[i].grab_pos[0];
					pos[1] = tmp[1] + grab[i].grab_pos[1];
					pos[2] = tmp[2] + grab[i].grab_pos[2];
				}
			}else if(!grab[i].active)
			{
				if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				{
					if(id == seduce_element_pointer_id(input, i, NULL))
					{
						grab[i].active = TRUE;
						grab[i].id = id;
						grab[i].timer = 0.1;
						r_matrix_projection_axisf(&grab[i].matrix, tmp, pos, axis, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
						grab[i].grab_pos[0] = pos[0] - tmp[0];
						grab[i].grab_pos[1] = pos[1] - tmp[1];
						grab[i].grab_pos[2] = pos[2] - tmp[2];
					}
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				betray_button_get_up_down(i, &a_button[i], &a_button_last[i], BETRAY_BUTTON_FACE_A);
				if(!a_button[i])
					grab[i + pointer_count].active = FALSE;

				if(grab[i + pointer_count].id == id && grab[i + pointer_count].active)
				{
					uint axis;
					axis = seduce_element_primary_axis(input, i);
					if(axis != -1)
					{
						r_matrix_projection_screenf(&grab[i + pointer_count].matrix, tmp, pos[0], pos[1], pos[2]);
						tmp[0] += input->axis[axis].axis[0] * input->delta_time;
						tmp[1] += input->axis[axis].axis[1] * input->delta_time;
						r_matrix_projection_axisf(&grab[i + pointer_count].matrix, tmp, pos, axis, tmp[0], tmp[1]);
						pos[0] = tmp[0];
						pos[1] = tmp[1];
						pos[2] = tmp[2];
					}
				}else if(!grab[i + pointer_count].active)
				{
					if(a_button[i] && !a_button_last[i])
					{
						grab[i + pointer_count].active = TRUE;
						grab[i + pointer_count].id = id;
						grab[i + pointer_count].timer = 0.1;
					}		
				}
			}
		}
	}

	if(input->mode == BAM_MAIN && frame_id != input->frame_number)
	{
		frame_id = input->frame_number;
		for(i = 0; i < (pointer_count + user_count); i++)
		{
			if(grab[i].active)
			{
				grab[i].timer += input->delta_time * 2.5;
				if(grab[i].timer > 1.0)		
					grab[i].timer = 1.0;
			}else
				grab[i].timer -= input->delta_time * 2.5;
		}
	}

	for(i = 0; i < (pointer_count + user_count); i++)
		if(grab[i].id == id && grab[i].active)
			return TRUE;
	return FALSE;
}


boolean seduce_manipulator_point_vector(BInputState *input, RMatrix *m, float *pos, void *id, float *snap, boolean snap_active, float *vector, float scale)
{
	static boolean *a_button, *a_button_last;
	static SeduceManipulatorGrab *grab = NULL;
	static uint frame_id = -1;
	RMatrix *reset;
	float tmp[3], f;
	uint i, count, active_pointer = -1, pointer_count, user_count;

	reset = r_matrix_get();
	if(m == NULL)
		m = reset;

	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);

	if(grab == NULL)
	{
		count = betray_support_functionality(B_SF_USER_COUNT_MAX);
		a_button = malloc((sizeof *a_button) * count);
		a_button_last = malloc((sizeof *a_button_last) * count);
		for(i = 0; i < count; i++)
			a_button[i] = a_button_last[i] = FALSE;
		count += betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
			grab[i].id = NULL;
	}

	r_matrix_projection_screenf(m, tmp, pos[0], pos[1], pos[2]);
	if(input->mode == BAM_DRAW && tmp[2] < 0)
	{
		float color[4] = {0.2, 0.6, 1.0, 1.0};
		static float time = 0;
		boolean active = FALSE;
		RMatrix draw_matrix;
		reset = r_matrix_get();
		seduce_widget_overlay_matrix(&draw_matrix);

		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].id == id && grab[i].active)
				active = TRUE;
			else if(id == seduce_element_pointer_id(input, i, NULL))
				active = TRUE;
			if(id == seduce_element_pointer_id(input, i, NULL))
				grab[i].matrix = *m;
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				active = TRUE;
				grab[i + pointer_count].matrix = *m;
			}
		}
		if(active)
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.05, SUI_3D_OBJECT_MOVE_3D, 1, color);
		else
			seduce_object_3d_draw(input, tmp[0], tmp[1], 0, scale * 0.05, SUI_3D_OBJECT_MOVE_3D, 1, NULL);
		r_matrix_set(m);
		seduce_element_add_point(input, id, 0, pos, scale * 0.025);
		r_matrix_set(reset);
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(!input->pointers[i].button[0])
				grab[i].active = FALSE;
			if(grab[i].id == id && grab[i].active)
			{	
				if(snap != NULL && snap_active)
				{
					pos[0] = snap[0];
					pos[1] = snap[1];
					pos[2] = snap[2];
				}else
				{
					r_matrix_projection_vectorf(&grab[i].matrix, tmp, pos, vector, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
					pos[0] = tmp[0] + grab[i].grab_pos[0];
					pos[1] = tmp[1] + grab[i].grab_pos[1];
					pos[2] = tmp[2] + grab[i].grab_pos[2];
				}
			}else if(!grab[i].active)
			{
				if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				{
					if(id == seduce_element_pointer_id(input, i, NULL))
					{
						grab[i].active = TRUE;
						grab[i].id = id;
						grab[i].timer = 0.1;
						r_matrix_projection_vectorf(&grab[i + pointer_count].matrix, tmp, pos, vector, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
						grab[i].grab_pos[0] = 0;
						grab[i].grab_pos[1] = 0;
						grab[i].grab_pos[2] = 0;
					}
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				betray_button_get_up_down(i, &a_button[i], &a_button_last[i], BETRAY_BUTTON_FACE_A);
				if(!a_button[i])
					grab[i + pointer_count].active = FALSE;

				if(grab[i + pointer_count].id == id && grab[i + pointer_count].active)
				{
					uint axis;
					axis = seduce_element_primary_axis(input, i);
					if(axis != -1)
					{
						r_matrix_projection_screenf(&grab[i + pointer_count].matrix, tmp, pos[0], pos[1], pos[2]);
						tmp[0] += input->axis[axis].axis[0] * input->delta_time;
						tmp[1] += input->axis[axis].axis[1] * input->delta_time;
						r_matrix_projection_axisf(&grab[i + pointer_count].matrix, tmp, pos, axis, tmp[0], tmp[1]);
						pos[0] = tmp[0];
						pos[1] = tmp[1];
						pos[2] = tmp[2];
					}
				}else if(!grab[i + pointer_count].active)
				{
					if(a_button[i] && !a_button_last[i])
					{
						grab[i + pointer_count].active = TRUE;
						grab[i + pointer_count].id = id;
						grab[i + pointer_count].timer = 0.1;
					}		
				}
			}
		}
	}

	if(input->mode == BAM_MAIN && frame_id != input->frame_number)
	{
		frame_id = input->frame_number;
		for(i = 0; i < (pointer_count + user_count); i++)
		{
			if(grab[i].active)
			{
				grab[i].timer += input->delta_time * 2.5;
				if(grab[i].timer > 1.0)		
					grab[i].timer = 1.0;
			}else
				grab[i].timer -= input->delta_time * 2.5;
		}
	}

	for(i = 0; i < (pointer_count + user_count); i++)
		if(grab[i].id == id && grab[i].active)
			return TRUE;
	return FALSE;
}


void seduce_manipulator_normal_draw(BInputState *input, RMatrix *m, void *id, float normal_x, float normal_y, float normal_z, uint icon, float time, boolean active, uint part, float begin, float end)
{
	float vec[3] = {0, 0, 0}, normal[3], color[4] = {0.2, 0.6, 1.0, 1.0}, matrix[16], a[3] = {0, 0, 0}, b[3] = {0, 0, 0};
	normal[0] = normal_x;
	normal[1] = normal_y;
	normal[2] = normal_z;
	if(0.1 > normal[0] * normal[0])
		vec[0] = 1.0;
	else
		vec[1] = 1.0;
	f_matrixxyf(matrix, NULL, normal, vec);
	r_matrix_push(m);
	r_matrix_matrix_mult(m, matrix);
	if(active)
	{
		r_matrix_rotate(NULL, input->minute_time * 20.0 * 360.0, 1, 0, 0);
		seduce_object_3d_draw(input, time, 0, 0, 1, icon, 1, color);
	}else
		seduce_object_3d_draw(input, time, 0, 0, 1, icon, 1, NULL);
	a[0] = begin;
	b[0] = end;
	seduce_element_add_line(input, id, part, a, b, 0.01);
	r_matrix_pop(m);
}


boolean	seduce_manipulator_normal(BInputState *input, RMatrix *m, float *pos, float *normal, void *id, float scale, float time)
{
	static boolean *a_button, *a_button_last;
	static SeduceManipulatorGrab *grab = NULL;
	static uint frame_id = -1;
	RMatrix *reset;
	float tmp[3], f, matrix[16];
	uint i, count, active_pointer = -1, pointer_count, user_count, part;

	reset = r_matrix_get();
	if(m == NULL)
		m = reset;

	scale *= 0.4;
	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);

	if(grab == NULL)
	{
		count = betray_support_functionality(B_SF_USER_COUNT_MAX);
		a_button = malloc((sizeof *a_button) * count);
		a_button_last = malloc((sizeof *a_button_last) * count);
		for(i = 0; i < count; i++)
			a_button[i] = a_button_last[i] = FALSE;
		count += betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
			grab[i].id = NULL;
	}

	r_matrix_projection_screenf(m, tmp, pos[0], pos[1], pos[2]);
	scale *= -tmp[2];
	if(input->mode == BAM_DRAW && tmp[2] < 0)
	{
		boolean active;
		for(i = 0; i < input->pointer_count; i++)
			if(grab[i].id == id && grab[i].active)
				break;
		active = i < input->pointer_count;


		for(i = 0; i < input->pointer_count; i++)
			if(id == seduce_element_pointer_id(input, i, NULL))
				grab[i].matrix = *m;
		for(i = 0; i < input->user_count; i++)
			if(id == seduce_element_selected_id(i, NULL, NULL))
				grab[i + pointer_count].matrix = *m;

		r_matrix_set(m);
		r_matrix_push(m);
		r_matrix_translate(m, pos[0], pos[1], pos[2]);
		r_matrix_normalize_scale(m);
		r_matrix_scale(m, scale, scale, scale);
		time = 1.0 - time;
		seduce_manipulator_normal_draw(input, m, id, 1,  0, 0, SUI_3D_OBJECT_NORMALLOCK, time, normal[0] > 0.9999 && active, 0, 0.45, 0.5);
		seduce_manipulator_normal_draw(input, m, id, -1, 0, 0, SUI_3D_OBJECT_NORMALLOCK, time, normal[0] < -0.9999 && active, 1, 0.45, 0.5);
		seduce_manipulator_normal_draw(input, m, id, 0, 1 , 0, SUI_3D_OBJECT_NORMALLOCK, time, normal[1] > 0.9999 && active, 2, 0.45, 0.5);
		seduce_manipulator_normal_draw(input, m, id, 0, -1, 0, SUI_3D_OBJECT_NORMALLOCK, time, normal[1] < -0.9999 && active, 3, 0.45, 0.5);
		seduce_manipulator_normal_draw(input, m, id, 0, 0, 1,  SUI_3D_OBJECT_NORMALLOCK, time, normal[2] > 0.9999 && active, 4, 0.45, 0.5);
		seduce_manipulator_normal_draw(input, m, id, 0, 0, -1, SUI_3D_OBJECT_NORMALLOCK, time, normal[2] < -0.9999 && active, 5, 0.45, 0.5);
		time = 1.0 - time;
		r_matrix_scale(m, time, time, time);
		if(active)
			seduce_manipulator_normal_draw(input, m, NULL, normal[0], normal[1], normal[2], SUI_3D_OBJECT_NORMAL, 0, TRUE,  6, 0.0, 0.45);
		else
			seduce_manipulator_normal_draw(input, m, id, normal[0], normal[1], normal[2], SUI_3D_OBJECT_NORMAL, 0, FALSE,6, 0.0, 0.45);
		r_matrix_pop(m);
		r_matrix_set(reset);
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(!input->pointers[i].button[0])
				grab[i].active = FALSE;
			if(grab[i].id == id && grab[i].active)
			{	
				r_matrix_push(m);
				r_matrix_normalize_scale(m);

				if(id == seduce_element_pointer_id(input, i, &part) && part < 6)
				{
					if(part < 6)
					{
						normal[0] = normal[1] = normal[2] = 0.0;
						normal[part / 2] = (float)(part % 2) * -2.0 + 1.0;
					}
				}else
				{
					float tmp2[3];
					tmp2[0] = normal[0] * scale * 0.35 + pos[0];
					tmp2[1] = normal[1] * scale * 0.35 + pos[1];
					tmp2[2] = normal[2] * scale * 0.35 + pos[2];
					r_matrix_projection_vertexf(&grab[i].matrix, tmp, tmp2, input->pointers[i].pointer_x, input->pointers[i].pointer_y);
					normal[0] = tmp[0] - pos[0];
					normal[1] = tmp[1] - pos[1];
					normal[2] = tmp[2] - pos[2];
					f_normalize3f(normal);
				}
				r_matrix_pop(m);
			}else if(!grab[i].active)
			{
				if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				{
					if(id == seduce_element_pointer_id(input, i, NULL))
					{
						grab[i].active = TRUE;
						grab[i].id = id;
						grab[i].timer = 0.1;
					}
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			float pos[3];
			if(id == seduce_element_selected_id(i, pos, NULL))
			{
				betray_button_get_up_down(i, &a_button[i], &a_button_last[i], BETRAY_BUTTON_FACE_A);
				if(!a_button[i])
					grab[i + pointer_count].active = FALSE;

				if(grab[i + pointer_count].id == id && grab[i + pointer_count].active)
				{
					uint axis;
					axis = seduce_element_primary_axis(input, i);
					if(axis != -1)
					{
						r_matrix_projection_screenf(&grab[i + pointer_count].matrix, tmp, pos[0], pos[1], pos[2]);
						tmp[0] += input->axis[axis].axis[0] * input->delta_time;
						tmp[1] += input->axis[axis].axis[1] * input->delta_time;
						r_matrix_projection_worldf(&grab[i + pointer_count].matrix, tmp, tmp[0], tmp[1], tmp[2]);
						pos[0] = tmp[0];
						pos[1] = tmp[1];
						pos[2] = tmp[2];
					}
				}else if(!grab[i + pointer_count].active)
				{
					if(a_button[i] && !a_button_last[i])
					{
						grab[i + pointer_count].active = TRUE;
						grab[i + pointer_count].id = id;
						grab[i + pointer_count].timer = 0.1;
					}		
				}
			}
		}
	}
	for(i = 0; i < (pointer_count + user_count); i++)
		if(grab[i].id == id && grab[i].active)
			return TRUE;
	return FALSE;
}
