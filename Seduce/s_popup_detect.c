#include <math.h>
#include <stdlib.h>

#include "seduce.h"
#include "s_draw_3d.h"

extern void seduce_background_circle_draw(BInputState *input, float pos_x, float pos_y, uint splits, float timer, uint selected);
extern void seduce_background_angle(BInputState *input, void *id, float pos_x, float pos_y, float angle_a, float angle_b, float timer);

extern void seduce_widget_overlay_matrix(RMatrix *matrix);
extern void seduce_object_3d_draw(BInputState *input, float pos_x, float pos_y, float pos_z, float scale, uint id, float fade,  float *color);



void seduce_popup_execute(BInputState *input, float pointer_x, float pointer_y, boolean button, boolean last_button, float time, void (*func)(BInputState *input, float time, void *user), void *user)
{
	BInputState i;
	BInputPointerState pointer;
	i = *input;
	pointer.pointer_x = pointer_x;
	pointer.pointer_y = pointer_y;
	pointer.click_pointer_x[0] = pointer_x;
	pointer.click_pointer_y[0] = pointer_y;
	pointer.delta_pointer_x = 0;
	pointer.delta_pointer_y = 0;
	pointer.button[0] = button;
	pointer.last_button[0] = last_button;
	pointer.button_count = 1; 
	pointer.name[0] = 0;
	pointer.user_id  = -1;
	i.pointers = &pointer;
	i.pointer_count = 1;
	i.axis_count = 0;
	func(&i, time, user);
}

typedef struct{
	void *id;
	float timer;
	float pos[2];
	void (*func)(BInputState *input, float time, void *user);
	uint user_id;
	void *user;
	boolean sticky;
	boolean active;
}SeducePopupGrab;
/*
extern void seduce_element_popup_action_begin(float pos_x, float pos_y, boolean active);
extern void seduce_element_popup_action_end(input);
*/
boolean seduce_popup_detect_mouse(BInputState *input, uint button, void (*func)(BInputState *input, float time, void *user), void *user)
{
	static SeducePopupGrab *grab = NULL;
	float f, vec[2];
	uint i, j, k;

	if(grab == NULL)
	{
		uint count;
		count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
			grab[i].func = NULL;
	}

	if(input->mode == BAM_MAIN)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].active)
			{
				grab[i].timer += input->delta_time * 1.0;
				if(grab[i].timer > 1.0)
					grab[i].timer = 1.0;
			}else
			{
				grab[i].timer -= input->delta_time * 1.0;
				if(grab[i].timer < 0.0)
					grab[i].timer = 0.0;
			}
			if(grab[i].func != NULL && grab[i].timer > 0.001)
				grab[i].func(input, grab[i].timer, grab[i].user);
		}
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(input->pointers[i].button[button] && !input->pointers[i].last_button[button])
			{
				grab[i].timer = 0.05;	
				grab[i].func = func;
				grab[i].user = user;
				grab[i].user_id = input->pointers[i].user_id;
				grab[i].sticky = FALSE;
				grab[i].active = TRUE;
				grab[i].pos[0] = input->pointers[i].pointer_x;
				grab[i].pos[1] = input->pointers[i].pointer_y;
				seduce_background_particle_burst(input, grab[i].pos[0], grab[i].pos[1], 50, 1.0, S_PT_SURFACE);
			}

			if(grab[i].func != NULL && grab[i].timer > 0.001)
			{
				if(grab[i].sticky)			
					seduce_popup_execute(input, input->pointers[i].pointer_x, input->pointers[i].pointer_y, input->pointers[i].button[0], input->pointers[i].last_button[0], grab[i].timer, func, user);
				else
					seduce_popup_execute(input, input->pointers[i].pointer_x, input->pointers[i].pointer_y, !input->pointers[i].button[button], !input->pointers[i].last_button[button], grab[i].timer, func, user);
			}
			if(!input->pointers[i].button[button] && input->pointers[i].last_button[button])
			{
				if(grab[i].timer < 0.99 &&
					0.01 * 0.01 > (input->pointers[i].pointer_x - input->pointers[i].click_pointer_x[button]) *
								(input->pointers[i].pointer_x - input->pointers[i].click_pointer_x[button]) + 
								(input->pointers[i].pointer_y - input->pointers[i].click_pointer_y[button]) * 
								(input->pointers[i].pointer_y - input->pointers[i].click_pointer_y[button]))
					grab[i].sticky = TRUE;
				else
					grab[i].active = FALSE;
			}
		}
		for(i = 0; i < input->pointer_count; i++)
			if(grab[i].sticky)
				for(j = 0; j < input->pointer_count; j++)
					if(!input->pointers[j].button[0] && input->pointers[j].last_button[0])
						grab[i].active = FALSE;
	}
	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].func != NULL && grab[i].timer > 0.001)
			{
				RMatrix overlay, *reset;
				reset = r_matrix_get();
				seduce_widget_overlay_matrix(&overlay);
				r_matrix_push(&overlay);
				r_matrix_translate(&overlay, grab[i].pos[0], grab[i].pos[1], 0);
			//	seduce_element_popup_action_begin(input, input->pointers[i].user_id);
	
				if(grab[i].sticky)			
					seduce_popup_execute(input, input->pointers[i].pointer_x, input->pointers[i].pointer_y, input->pointers[i].button[button], input->pointers[i].button[button], grab[i].timer, func, user);
				else
					seduce_popup_execute(input, input->pointers[i].pointer_x, input->pointers[i].pointer_y, !input->pointers[i].button[button], !input->pointers[i].button[button], grab[i].timer, func, user);

			//	seduce_element_popup_action_end(input);
				r_matrix_set(reset);
			}
		}
	}
	for(i = 0; i < input->pointer_count; i++)
		if(grab[i].active)
			return TRUE;
	return FALSE;
}

boolean seduce_popup_detect_axis(BInputState *input, uint button, void (*func)(BInputState *input, float time, void *user), void *user)
{
	static SeducePopupGrab *users = NULL;
	float f, vec[2];
	uint i, j, k, count, user_count;

	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);
	if(users == NULL)
	{
		users = malloc((sizeof *users) * user_count);
		for(i = 0; i < user_count; i++)
		{
			users[i].func = NULL;
			users[i].active = FALSE;
		}
	}

	if(input->mode == BAM_MAIN)
	{
		for(i = 0; i < user_count; i++)
		{
			if(users[i].active)
			{
				users[i].timer += input->delta_time * 4.0;
				if(users[i].timer > 1.0)
					users[i].timer = 1.0;
			}else
			{
				users[i].timer -= input->delta_time * 4.0;
				if(users[i].timer < 0.0)
					users[i].timer = 0.0;
			}
			if(users[i].func != NULL && users[i].timer > 0.001)
				users[i].func(input, users[i].timer, users[i].user);
		}
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < user_count; i++)
		{
			if(!users[i].active)
			{	
				for(j = 0; j < input->button_event_count; j++)
				{
					if(input->button_event[j].user_id == i && input->button_event[j].state && input->button_event[j].button == button)
					{
						float pos[3];
						users[i].timer = 0.05;	
						users[i].func = func;
						users[i].user = user;
						users[i].user_id = input->button_event[j].user_id;
						users[i].sticky = FALSE;
						users[i].active = TRUE;
						seduce_element_selected_id(i, pos, NULL);
						users[i].pos[0] = pos[0];
						users[i].pos[1] = pos[1];
					}
				}
			}
			if(users[i].func != NULL)
				for(j = 0; j < input->button_event_count; j++)
					if(input->button_event[j].user_id == i && !input->button_event[j].state && input->button_event[j].button == button)
						users[i].active = FALSE;
		}
	}

	for(i = 0; i < user_count; i++)
	{
		if(users[i].func != NULL && users[i].timer > 0.001)
		{
			BInputAxisState *axis = NULL;
			BInputState fake_input;
			RMatrix overlay, *reset;
			uint primary;
			if(input->mode == BAM_DRAW)
			{
				reset = r_matrix_get();
				seduce_widget_overlay_matrix(&overlay);
				r_matrix_push(&overlay);
				r_matrix_translate(&overlay, users[i].pos[0], users[i].pos[1], 0);
			}
			if(input->axis_count != 0)
				axis = malloc((sizeof *axis) * input->axis_count);
			fake_input = *input;
			fake_input.pointer_count = 0;
			fake_input.axis = axis;
			fake_input.button_event_count = 0;
			
			primary = seduce_element_primary_axis(input, i);
			for(j = 0; j < input->axis_count; j++)
			{
				axis[j] = input->axis[j];
				if(j != primary)
					axis[j].axis[0] = axis[j].axis[1] = axis[j].axis[2] = 0;
			}

			for(j = 0; j < input->button_event_count; j++)
			{
				fake_input.button_event[fake_input.button_event_count] = input->button_event[j];
				if(input->button_event[j].user_id == i && input->button_event[j].button == button && !input->button_event[j].state)
				{
					fake_input.button_event[fake_input.button_event_count].state = TRUE;
					fake_input.button_event[fake_input.button_event_count].button = BETRAY_BUTTON_FACE_A;
				}
				if(input->button_event[j].user_id == i)
					fake_input.button_event_count++;
			}


			users[i].func(&fake_input, users[i].timer, users[i].user);
			if(input->mode == BAM_DRAW)
				r_matrix_set(reset);
			if(input->axis_count != 0)
				free(axis);
		}
		if(input->mode == BAM_EVENT)
			if(users[i].func != NULL)
				for(j = 0; j < input->button_event_count; j++)
					if(input->button_event[j].user_id == i && !input->button_event[j].state && input->button_event[j].button == button)
						users[i].active = FALSE;
	}
	for(i = 0; i < user_count; i++)
		if(users[i].active)
			return TRUE;
	return FALSE;
}

/*
typedef struct{
	float timer;
	float pos[2];
	void (*func)(BInputState *input, float time, void *user);
	void *user;
	boolean sticky;
	boolean active;
}SeducePopupGrab;
*/

boolean seduce_popup_detect_icon(BInputState *input, void *id, uint icon, float pos_x, float pos_y,  float scale, float time, void (*func)(BInputState *input, float time, void *user), void *user, boolean displace, float *color)
{
	static boolean *a_button, *a_button_last;
	static SeducePopupGrab *grab = NULL;
	float f, vec[2];
	uint i, j, k, count, user_count, pointer_count;

	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);
	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	count = user_count + pointer_count;

	if(grab == NULL)
	{
		grab = malloc((sizeof *grab) * count);
		for(i = 0; i < count; i++)
		{
			grab[i].id = NULL;
			grab[i].func = NULL;
			grab[i].active = FALSE;
			grab[i].timer = 0.0;
		}
		a_button = malloc((sizeof *a_button) * user_count);
		a_button_last = malloc((sizeof *a_button_last) * user_count);
		for(i = 0; i < user_count; i++)
			a_button[i] = a_button_last[i] = FALSE;
	}

	if(input->mode == BAM_DRAW)
	{
		float on[3] = {0.2, 0.6, 1.0}, *p = NULL, pos[3];
		for(i = 0; i < input->pointer_count; i++)
			if(id == seduce_element_pointer_id(input, i, NULL))
				break;
		if(i < input->pointer_count)
		{
			p = on;
			if(color != NULL)
				p = color;
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(id == seduce_element_selected_id(i, NULL, NULL))
			{
				if(betray_button_get(i, BETRAY_BUTTON_FACE_A))
				{
					p = on;
					if(color != NULL)
						p = color;
				}
			}
		}
		pos[0] = pos_x;
		pos[1] = pos_y;
		pos[2] = 0;
		seduce_element_add_point(input, id, 0, pos, scale);
		if(time < 0.999)
		{	
			RMatrix *matrix;
			matrix = r_matrix_get();
			r_matrix_push(matrix);
			r_matrix_translate(matrix, pos_x, pos_y, 0);
			r_matrix_rotate(matrix, 360.0 - time * 360.0, 0, 1, 0);
			r_matrix_scale(matrix, time, 2 - time, time);
			seduce_object_3d_draw(input, 0, 0, 0, scale, icon, FALSE, p);
			r_matrix_pop(matrix);
		}else
			seduce_object_3d_draw(input, pos_x, pos_y, 0, scale, icon, FALSE, p);
	}

	for(i = 0; i < count; i++)
	{
		RMatrix overlay, *reset;
		BInputPointerState *pointers = NULL;
		BInputState fake_input;
		if(input->mode == BAM_MAIN)
		{
			if(grab[i].active)
			{
				grab[i].timer += input->delta_time * 4.0;
				if(grab[i].timer > 1.0)
					grab[i].timer = 1.0;
			}else
			{
				grab[i].timer -= input->delta_time * 4.0;
				if(grab[i].timer < 0.0)
					grab[i].timer = 0.0;
			}
		}
		if(grab[i].func != NULL && grab[i].timer > 0.001 && id == grab[i].id)
		{
			BInputAxisState *axis = NULL;
			uint primary;

			if(input->mode == BAM_DRAW)
			{
				float pos[3];
				reset = r_matrix_get();
				r_matrix_projection_screenf(reset, pos, pos_x, pos_y, 0);
				seduce_widget_overlay_matrix(&overlay);
				r_matrix_push(&overlay);
				if(displace)
					r_matrix_translate(&overlay, pos[0], pos[1], 0);
			}
			fake_input = *input;
			if(input->pointer_count != 0)
				pointers = malloc((sizeof *pointers) * input->pointer_count);
			if(input->axis_count != 0)
				axis = malloc((sizeof *axis) * input->axis_count);

			fake_input.pointers = pointers;
			fake_input.pointer_count = 0;
			if(i < pointer_count)
			{
				pointers[fake_input.pointer_count] = input->pointers[i];		
				if(!grab[i].sticky)
				{
					for(k = 0; k < pointers[fake_input.pointer_count].button_count; k++)
					{
						pointers[fake_input.pointer_count].button[k] = !pointers[fake_input.pointer_count].button[k];
						pointers[fake_input.pointer_count].last_button[k] = !pointers[fake_input.pointer_count].last_button[k];
					}
				}
				fake_input.pointer_count++;
			}
			
			fake_input.axis = axis;
			primary = seduce_element_primary_axis(input, grab[i].user_id);
			for(j = 0; j < input->axis_count; j++)
			{
				axis[j] = input->axis[j];
				if(j != primary)
					axis[j].axis[0] = axis[j].axis[1] = axis[j].axis[2] = 0;
			}

			fake_input.button_event_count = 0;		
			for(j = 0; j < input->button_event_count; j++)
			{
				fake_input.button_event[fake_input.button_event_count] = input->button_event[j];
			//	if(!grab[i].sticky)
					if(input->button_event[j].user_id == grab[i].user_id && input->button_event[j].button == BETRAY_BUTTON_FACE_A && !input->button_event[j].state)
						fake_input.button_event[fake_input.button_event_count].state = TRUE;
				if(input->button_event[j].user_id == grab[i].user_id)
					fake_input.button_event_count++;
			}

			grab[i].func(&fake_input, grab[i].timer, grab[i].user);
			if(input->mode == BAM_DRAW)
				r_matrix_set(reset);
			if(pointers != NULL)
				free(pointers);
			if(axis != NULL)
				free(axis);


			if(input->mode == BAM_DRAW)
				r_matrix_set(reset);
		}
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(grab[i].active && id == grab[i].id)
			{
				if(!input->pointers[i].button[0] && input->pointers[i].last_button[0])
				{

					if(!grab[i].sticky &&
						(input->pointers[i].click_pointer_x[0] - input->pointers[i].pointer_x) *
						(input->pointers[i].click_pointer_x[0] - input->pointers[i].pointer_x) + 
						(input->pointers[i].click_pointer_y[0] - input->pointers[i].pointer_y) *
						(input->pointers[i].click_pointer_y[0] - input->pointers[i].pointer_y) < 0.01 * 0.01)
						grab[i].sticky = TRUE;
					else
						grab[i].active = FALSE;
				}
			}
			if(!grab[i].active)
			{
				if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				{
					if(id == seduce_element_pointer_id(input, i, NULL))
					{
						grab[i].active = TRUE;
						grab[i].sticky = FALSE;
						grab[i].id = id;
						grab[i].timer = 0.1;
						grab[i].pos[0] = input->pointers[i].pointer_x;
						grab[i].pos[1] = input->pointers[i].pointer_y;
						grab[i].func = func;
						grab[i].user = user;
						grab[i].user_id = input->pointers[i].user_id;
					}
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			uint part;
			if(id == seduce_element_selected_id(i, NULL, &part))
			{
				for(j = 0; j < input->button_event_count; j++)
					if(input->button_event[j].button == BETRAY_BUTTON_FACE_A && input->button_event[j].user_id == i)
						break;
				if(j < input->button_event_count)
				{
					if(!input->button_event[j].state)
					{
						uint axis;
						axis = seduce_element_primary_axis(input, i);
						if(grab[i + pointer_count].sticky)
							grab[i + pointer_count].active = FALSE;
						else if(axis == -1 || input->axis[axis].axis[0] * input->axis[axis].axis[0] + input->axis[axis].axis[1] * input->axis[axis].axis[1] > 0.01)
							grab[i + pointer_count].active = FALSE;
						else
							grab[i + pointer_count].sticky = TRUE;
					}

					
					if(input->button_event[j].state && !grab[i + pointer_count].active)
					{
						grab[i + pointer_count].active = TRUE;
						grab[i + pointer_count].sticky = FALSE;
						grab[i + pointer_count].id = id;
						grab[i + pointer_count].timer = 0.1;
						grab[i + pointer_count].pos[0] = input->pointers[i].pointer_x;
						grab[i + pointer_count].pos[1] = input->pointers[i].pointer_y;
						grab[i + pointer_count].func = func;
						grab[i + pointer_count].user = user;
						grab[i + pointer_count].user_id = i;
					}		
				}
			}
		}
	}



	for(i = 0; i < count; i++)
		if(grab[i].id == id && grab[i].active)
			return TRUE;
	return FALSE;
}

typedef struct{
	float timer;
	float pos[2];
	void (*func)(BInputState *input, float time, void *user);
	void *user;
	uint user_id;
	boolean sticky;
	boolean active;
	uint reject[16];
	uint reject_count;
}SeducePopupMultiTouch;

boolean seduce_popup_detect_multitouch(BInputState *input, uint finger_count, void (*func)(BInputState *input, float time, void *user), void *user)
{
	static float *timers = NULL;
	static SeducePopupMultiTouch *users = NULL;
	static BInputPointerState *fake_pointers = NULL;
	float f, vec[2];
	uint i, j, k, count, pointer_count, user_count;

	pointer_count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
	user_count = betray_support_functionality(B_SF_USER_COUNT_MAX);
	if(users == NULL)
	{
		users = malloc((sizeof *users) * user_count);
		for(i = 0; i < user_count; i++)
		{
			users[i].func = NULL;
			users[i].active = FALSE;
		}
		timers = malloc((sizeof *timers) * pointer_count);
		for(i = 0; i < pointer_count; i++)
			timers[i] = 0;
		fake_pointers = malloc((sizeof *fake_pointers) * user_count); 
	}

	if(input->mode == BAM_MAIN)
	{
		for(i = 0; i < pointer_count; i++)
			timers[i] += input->delta_time;
		for(i = 0; i < user_count; i++)
		{
			if(users[i].active)
			{
				users[i].timer += input->delta_time * 4.0;
				if(users[i].timer > 1.0)
					users[i].timer = 1.0;
			}else
			{
				users[i].timer -= input->delta_time * 4.0;
				if(users[i].timer < 0.0)
					users[i].timer = 0.0;
			}
		}
	}

	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < user_count; i++)
		{
			if(!users[i].active)
			{
				float pos[2] = {0, 0};
				for(j = users[i].reject_count = 0; j < input->pointer_count && users[i].reject_count < finger_count; j++)
				{
					if(input->pointers[j].button[0] && !input->pointers[j].last_button[0])
						timers[j] = 0.0;

					if(input->pointers[j].user_id == i && input->pointers[j].button[0] && timers[j] < 0.5)
					{
						users[i].reject[users[i].reject_count++] = j;
						pos[0] += input->pointers[j].pointer_x;
						pos[1] += input->pointers[j].pointer_y;
					}
				}		
				if(users[i].reject_count == finger_count)
				{
					users[i].timer = 0.05;	
					users[i].func = func;
					users[i].user = user;
					users[i].sticky = FALSE;
					users[i].active = TRUE;
					users[i].pos[0] = pos[0] / (float)users[i].reject_count;
					users[i].pos[1] = pos[1] / (float)users[i].reject_count;
				}
			}
		}
	}
	for(i = 0; i < user_count; i++)
	{
		if(users[i].func != NULL && users[i].timer > 0.001)
		{
			RMatrix overlay, *reset;
			BInputPointerState *pointers;
			BInputState fake_input;
			if(input->mode == BAM_DRAW)
			{
				BInputPointerState *save;
				reset = r_matrix_get();
				seduce_widget_overlay_matrix(&overlay);
				r_matrix_push(&overlay);
				r_matrix_translate(&overlay, users[i].pos[0], users[i].pos[1], 0);
			}
			for(j = 0; j < users[i].reject_count; j++)
			{
				for(k = 0; k < input->pointers[users[i].reject[j]].button_count && !input->pointers[users[i].reject[j]].button[0]  && !input->pointers[users[i].reject[j]].last_button[0]; k++);
				if(k == input->pointers[users[i].reject[j]].button_count)
					users[i].reject[j--] = users[i].reject[--users[i].reject_count];
			}

			fake_input = *input;
			pointers = malloc((sizeof *pointers) * input->pointer_count);
			fake_input.pointers = pointers;
			fake_input.pointer_count = 0;
			for(j = 0; j < input->pointer_count; j++)
			{
				for(k = 0; k < users[i].reject_count && users[i].reject[k] == j; k++);
				if(k == users[i].reject_count && pointers[j].user_id == i)
				{
					pointers[fake_input.pointer_count++] = input->pointers[j];
				}
			}
			fake_input.axis_count = 0;
			users[i].func(&fake_input, users[i].timer, users[i].user);
			if(input->mode == BAM_DRAW)
				r_matrix_set(reset);
			free(pointers);
		}
	}
	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < user_count; i++)
		{
			if(users[i].func != NULL)
			{
				for(j = 0; j < input->pointer_count; j++)
				{
					if(!input->pointers[j].button[0] && !input->pointers[j].last_button[0])
						for(k = 0; k < users[i].reject_count; k++)
							if(users[i].reject[k] == j)
								users[i].reject[k] = users[i].reject[--users[i].reject_count];

					if(i == input->pointers[j].user_id && !input->pointers[j].button[0] && input->pointers[j].last_button[0])
					{
						for(k = 0; k < users[i].reject_count; k++)
							if(users[i].reject[k] == j)
								break;
						if(k == users[i].reject_count)
							users[i].active = FALSE;
					}
				}
			}

		}
	}

	for(i = 0; i < user_count; i++)
		if(users[i].active)
			return TRUE;
	return FALSE;
}

