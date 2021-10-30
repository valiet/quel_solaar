#include <stdio.h>
#include <stdlib.h>
#include "seduce.h"
#include "s_text.h"

extern void		sui_text_line_edit_mouse(float size, float spacing, float length, char *text, uint *scroll_start, uint *select_start, uint *select_end, boolean mouse_button, boolean mouse_button_last, float pointer_x);
extern void		sui_text_line_edit_draw(float pos_x, float pos_y, float size, float spacing, float length, char *text, uint *scroll_start, uint select_start, uint select_end, float red, float green, float blue, float alpha);

extern boolean  sui_util_edit_insert_character(char *text, uint size, uint curser_start, uint curser_end, char *insert);
extern void		sui_text_edit_forward(char *text, uint *select_start, uint *select_end);
extern void		sui_text_edit_back(char *text, uint *select_start, uint *select_end);
extern void		sui_text_edit_delete(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_end(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_home(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_backspace(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_insert_character(char *text, uint length, uint *select_start, uint *select_end, char character);
extern void		sui_text_edit_paste(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_cut(char *text, uint length, uint *select_start, uint *select_end);
extern void		sui_text_edit_copy(char *text, uint length, uint *select_start, uint *select_end);

typedef struct{
	void *id;
	uint scroll_start;
	uint select_start; 
	uint select_end;
	char *copy;
	void *done_func;
}STextEditState;




static STextEditState *seduce_text_state;

static uint sui_type_in_cursor = 0;
static char *sui_type_in_text = 0;
static char *sui_type_in_copy = 0;
static char *sui_return_text = 0;
static void (* sui_type_in_done_func)(void *user, char *text); 
/*
boolean seduce_text_button(BInputState *input, float pos_x, float pos_y, float center, float size, float spacing, const char *text, float red, float green, float blue, float alpha, float red_select, float green_select, float blue_select, float alpha_select)
{
	if(input->mode == BAM_DRAW)
	{
		float f, length, brightness = 0;
		uint i;
		length = seduce_text_line_length(size, spacing, text, -1);
		center *= length;
		for(i = 0; i < input->pointer_count; i++)
		{
			if(sui_box_click_test(input->pointers[i].pointer_x, 
				input->pointers[i].pointer_y, 
				pos_x - center, 
				pos_y - size * 0.5, 
				length, size * 2.5))
			{
				if(brightness < 0.3)
					brightness = 0.5;
				if(input->pointers[i].button[0] || input->pointers[i].last_button[0])
					if(sui_box_click_test(input->pointers[i].click_pointer_x[0], input->pointers[i].click_pointer_y[0], pos_x - center, pos_y - size * 0.5, length, size * 2.5))
						brightness = 1.0;
			}
		}
		seduce_text_line_draw(pos_x - center, pos_y, size, spacing, text, 
			red + (red_select - red) * brightness, 
			green + (green_select - green) * brightness, 
			blue + (blue_select - blue) * brightness, 
			alpha + (alpha_select - alpha) * brightness, -1);
	}else if(input->mode == BAM_EVENT)
	{
		float length;
		uint i;
		length = seduce_text_line_length(size, spacing, text, -1);
		center *= length;
		for(i = 0; i < input->pointer_count; i++)
			if(input->pointers[i].button[0] == TRUE && input->pointers[i].last_button[0] == FALSE)
				if(sui_box_click_test(input->pointers[i].pointer_x, input->pointers[i].pointer_y, pos_x - center, pos_y - size * 0.5, length, size * 2.5) && 
				   sui_box_click_test(input->pointers[i].click_pointer_x[0], input->pointers[i].click_pointer_y[0], pos_x - center, pos_y - size * 0.5, length, size * 2.5))
					return TRUE;
	}
	return FALSE;
}
*/
boolean seduce_text_button(BInputState *input, void *id, float pos_x, float pos_y, float center, float size, float spacing, const char *text, float red, float green, float blue, float alpha, float red_select, float green_select, float blue_select, float alpha_select)
{
	uint i;
	if(input->mode == BAM_DRAW)
	{
		boolean acticve = FALSE;
		float a[3], b[3], length;
		for(i = 0; i < input->pointer_count; i++)
			if(id == seduce_element_pointer_id(input, i, NULL))
				break;
		acticve = i < input->pointer_count;

		for(i = 0; i < input->user_count; i++)
			if(id == seduce_element_selected_id(i, NULL, NULL))
				if(betray_button_get(i, BETRAY_BUTTON_FACE_A))
				acticve = TRUE;

		if(center < 0.001)
			a[0] = pos_x;
		else
		 {
			 length = seduce_text_line_length(size, spacing, text, -1);
			 a[0] = pos_x - length * center;
		}
		a[1] = b[1] = pos_y;
		a[2] = b[2] = 0;
		
		if(acticve)
			length = seduce_text_line_draw(a[0], a[1], size, spacing, text, red_select, green_select, blue_select, alpha_select, -1);
		else
			length = seduce_text_line_draw(a[0], a[1], size, spacing, text, red, green, blue, alpha, -1);
		b[0] = a[0] + length;
		a[1] += size * 0.5;
		b[1] += size * 0.5;
		seduce_element_add_line(input, id, 0, a, b, size * 8.0);
	}
	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
			if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
				if(id == seduce_element_pointer_id(input, i, NULL))
					return TRUE;
		if(id == seduce_element_popup_action(TRUE))
			return TRUE;
		for(i = 0; i < input->user_count; i++)
			if(id == seduce_element_selected_id(i, NULL, NULL))
				if(betray_button_get(i, BETRAY_BUTTON_FACE_A))
					return TRUE;
	}
	return FALSE;
}

uint seduce_text_button_list(BInputState *input, void *id, float pos_x, float pos_y, float center, float size, float spacing, const char **texts, uint text_count, float red, float green, float blue, float alpha, float red_select, float green_select, float blue_select, float alpha_select)
{
	uint i, part;
	if(input->mode == BAM_DRAW)
	{
		float length[64], sum = 0;
		boolean acticve[64] = {FALSE};
		float x, a[3], b[3];
		center *= 2.0;
		
		for(i = 0; i < input->pointer_count; i++)
			acticve[i] = FALSE;

		for(i = 0; i < input->pointer_count; i++)
			if(id == seduce_element_pointer_id(input, i, &part))
				acticve[part] = TRUE;

		for(i = 0; i < input->user_count; i++)
			if(id == seduce_element_selected_id(i, NULL, &part))
				if(betray_button_get(i, BETRAY_BUTTON_FACE_A))
					acticve[part] = TRUE;

		for(i = 0; i < text_count; i++)
		{
			length[i] = seduce_text_line_length(size, spacing, texts[i], -1);
			sum += length[i];
		}
		
		x = pos_x - sum * center + size * 4.0 * (float)(text_count - 1);
		for(i = 0; i < text_count; i++)
		{
			a[0] = x;
			a[1] = b[1] = pos_y;
			a[2] = b[2] = 0;
			if(acticve[i])
				seduce_text_line_draw(x, pos_y, size, spacing, texts[i], red, green, blue, alpha, -1);
			else
				seduce_text_line_draw(x, pos_y, size, spacing, texts[i], red_select, green_select, blue_select, alpha_select, -1);
			b[0] = a[0] + length[i];
			seduce_element_add_line(input, id, i, a, b, size * 4.0);
			x += length[i] + size * 4;
		}
		x = pos_x - sum * center + size * 4.0 * (float)(text_count - 1) - size * 2.0 ;
		for(i = 0; i < text_count - 1; i++)
		{
			x += length[i] + size * 4;
			r_primitive_line_2d(x, pos_y - size * 0.25, x, pos_y + size * 1.5, red_select, green_select, blue_select, alpha_select);
		}
		r_primitive_line_flush();
	}
	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
			if(!input->pointers[i].button[0] && input->pointers[i].last_button[0])
				if(id == seduce_element_pointer_id(input, i, &part))
					return part;
		if(id == seduce_element_popup_action(TRUE))
			return TRUE;
		for(i = 0; i < input->user_count; i++)
			if(id == seduce_element_selected_id(i, NULL, &part))
				if(betray_button_get(i, BETRAY_BUTTON_FACE_A))
					return part;
	}
	return -1;
}

void seduce_text_edit_init()
{
	uint i, count;
	count = betray_support_functionality(B_SF_USER_COUNT_MAX);
	seduce_text_state = malloc((sizeof *seduce_text_state) * count);
	for(i = 0; i < count; i++)
	{
		seduce_text_state[i].id = NULL;
		seduce_text_state[i].scroll_start = 0;
		seduce_text_state[i].select_start = 0;
		seduce_text_state[i].select_end = 0;
		seduce_text_state[i].done_func = NULL;
		seduce_text_state[i].copy = NULL;
	}
}

void seduce_text_activate(uint user_id, void *id,  char *text, uint buffer_size, void *done_func, void *user, boolean copy)
{
	char *t;
	uint i;
	if(user_id >= betray_support_functionality(B_SF_USER_COUNT_MAX))
		return;
	seduce_text_state[user_id].id = id;
	seduce_text_state[user_id].scroll_start = 0;
	seduce_text_state[user_id].select_start = 0;
	seduce_text_state[user_id].select_end = 0;
	seduce_text_state[user_id].done_func = done_func;
	if(copy)
	{
		t = seduce_text_state[user_id].copy = malloc((sizeof *seduce_text_state[user_id].copy) * buffer_size);
		for(i = 0; i < buffer_size - 1 && text[i] != 0; i++)
			seduce_text_state[user_id].copy[i] = text[i];
		seduce_text_state[user_id].copy[i] = 0;
	}else
		seduce_text_state[user_id].copy = NULL;
}


void seduce_text_deactivate(uint user_id)
{
	if(user_id >= betray_support_functionality(B_SF_USER_COUNT_MAX))
		return;
	seduce_text_state[user_id].id = NULL;
	if(seduce_text_state[user_id].copy != NULL)
		free(seduce_text_state[user_id].copy);
	seduce_text_state[user_id].copy = NULL;
}

STypeInState seduce_text_line_edit(BInputState *input, void *id, char *text, uint buffer_size, float pos_x, float pos_y, float length, float size, char *label, boolean left, void (*done_func)(void *user, char *text), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha)
{
	float array[6], output[2];
	char *t;
	uint i, j, user_count;
	if(input->mode == BAM_DRAW)
	{
		array[0] = pos_x;
		array[1] = pos_y + size * 0.5;
		array[2] = 0;
		array[3] = pos_x + length;
		array[4] = pos_y + size * 0.5;
		array[5] = 0;
		seduce_element_add_line(input, id, 0, array, &array[3], size * 2.0);
		seduce_element_add_surface(input, id);
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				if(seduce_text_state[i].done_func != NULL)
					t = seduce_text_state[i].copy;
				else
					t = text;
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], length) + seduce_text_state[i].scroll_start);
				sui_text_line_edit_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, length, t, &seduce_text_state[i].scroll_start, seduce_text_state[i].select_start, seduce_text_state[i].select_end, active_red, active_green, active_blue, active_alpha);
				break;
			}
		}
		if(i == input->user_count)
		{
			if(label != NULL && text[0] == 0)
				t = label;
			else
				t = text;

			for(i = 0; i < input->user_count && id != seduce_element_pointer_id(input, i, NULL); i++);

			if(i < input->user_count)
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, t, active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, t, length));
			else
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, t, red, green, blue, alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, t, length));
		}

	}else if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
			if(input->pointers[i].button[0] == TRUE && input->pointers[i].last_button[0] == FALSE)
				if(seduce_text_state[input->pointers[i].user_id].id != id && id == seduce_element_pointer_id(input, i, NULL))
					seduce_text_activate(input->pointers[i].user_id, id,  text, buffer_size, done_func, user, done_func != NULL);

		
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				if(seduce_text_state[i].done_func != NULL)
					t = seduce_text_state[i].copy;
				else
					t = text;

				for(j = 0; j < input->pointer_count; j++)
					if(input->pointers[j].user_id == i)
						if(seduce_element_surface_project(input, id, output, input->pointers[j].pointer_x, input->pointers[j].pointer_y))
							sui_text_line_edit_mouse(size, SEDUCE_T_SPACE, length, t, &seduce_text_state[i].scroll_start, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->pointers[j].button[0], input->pointers[j].last_button[0], output[0] - pos_x);
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				for(j = 0; j < input->button_event_count; j++)
				{
					if(input->button_event[j].state == TRUE && input->button_event[j].user_id == i)
					{
						if(seduce_text_state[i].done_func != NULL)
							t = seduce_text_state[i].copy;
						else
							t = text;
						if(input->button_event[j].button == BETRAY_BUTTON_PASTE)
							sui_text_edit_paste(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_CUT)
							sui_text_edit_cut(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_COPY)
							sui_text_edit_copy(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RIGHT)
							sui_text_edit_forward(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_LEFT)
							sui_text_edit_back(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_DELETE)
							sui_text_edit_delete(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_END)
							sui_text_edit_end(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_HOME)
							sui_text_edit_home(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_BACKSPACE)
							sui_text_edit_backspace(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RETURN) 
						{
							if(seduce_text_state[i].done_func != NULL)
							{
								void (* done_func)(void *user, char *text);
								done_func = seduce_text_state[i].done_func;
								done_func(user, t);
								free(seduce_text_state[i].copy);
							}
							seduce_text_state[i].id = NULL;
							return S_TIS_DONE;
						}
						else if(input->button_event[j].character > 31 && input->button_event[j].character < 256)
							sui_text_edit_insert_character(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->button_event[j].character);
					}
				}
			}
		}
	}
	for(i = 0; i < input->user_count; i++)
		if(seduce_text_state[i].id == id)
			return S_TIS_ACTIVE;
	return S_TIS_IDLE;
}

STypeInState seduce_text_password_edit(BInputState *input, void *id, char *text, uint buffer_size, float pos_x, float pos_y, float length, float size, char *label, boolean left, void (*done_func)(void *user, char *text), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha)
{
	float array[6], output[2];
	char *t, *password;
	uint i, j, user_count;
	if(input->mode == BAM_DRAW)
	{
		array[0] = pos_x;
		array[1] = pos_y + size * 0.5;
		array[2] = 0;
		array[3] = pos_x + length;
		array[4] = pos_y + size * 0.5;
		array[5] = 0;
		seduce_element_add_line(input, id, 0, array, &array[3], size * 2.0);
		seduce_element_add_surface(input, id);
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				if(seduce_text_state[i].done_func != NULL)
					t = seduce_text_state[i].copy;
				else
					t = text;
				for(j = 0; t[j] != 0; j++);
				password = malloc(sizeof *password * ++j);
				for(j = 0; t[j] != 0; j++)
					password[j] = '*';
				password[j] = 0;
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, &password[seduce_text_state[i].scroll_start], active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], length) + seduce_text_state[i].scroll_start);
				sui_text_line_edit_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, length, password, &seduce_text_state[i].scroll_start, seduce_text_state[i].select_start, seduce_text_state[i].select_end, active_red, active_green, active_blue, active_alpha);
				free(password);
				break;
			}
		}
		if(i == input->user_count)
		{
			password = NULL;
			if(label != NULL && text[0] == 0)
				t = label;
			else
			{
				for(j = 0; text[j] != 0; j++);
				password = malloc(sizeof *password * ++j);
				for(j = 0; text[j] != 0; j++)
					password[j] = '*';
				password[j] = 0;
				t = password;
			}
			for(i = 0; i < input->user_count && id != seduce_element_pointer_id(input, i, NULL); i++);
			if(i < input->user_count)
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, t, active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, t, length));
			else
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, t, red, green, blue, alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, t, length));
			if(password != NULL)
				free(password);
		}

	}else if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
			if(input->pointers[i].button[0] == TRUE && input->pointers[i].last_button[0] == FALSE)
				if(seduce_text_state[input->pointers[i].user_id].id != id && id == seduce_element_pointer_id(input, i, NULL))
					seduce_text_activate(input->pointers[i].user_id, id,  text, buffer_size, done_func, user, done_func != NULL);
		
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				if(seduce_text_state[i].done_func != NULL)
					t = seduce_text_state[i].copy;
				else
					t = text;
				for(j = 0; t[j] != 0; j++);
				password = malloc(sizeof *password * ++j);
				for(j = 0; t[j] != 0; j++)
					password[j] = '*';
				password[j] = 0;
				for(j = 0; j < input->pointer_count; j++)
					if(input->pointers[j].user_id == i)
						if(seduce_element_surface_project(input, id, output, input->pointers[j].pointer_x, input->pointers[j].pointer_y))
							sui_text_line_edit_mouse(size, SEDUCE_T_SPACE, length, password, &seduce_text_state[i].scroll_start, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->pointers[j].button[0], input->pointers[j].last_button[0], output[0] - pos_x);
				free(password);
			}
		}
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				for(j = 0; j < input->button_event_count; j++)
				{
					if(input->button_event[j].state == TRUE && input->button_event[j].user_id == i)
					{
						if(seduce_text_state[i].done_func != NULL)
							t = seduce_text_state[i].copy;
						else
							t = text;
						if(input->button_event[j].button == BETRAY_BUTTON_PASTE)
							sui_text_edit_paste(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_CUT)
							sui_text_edit_cut(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_COPY)
							sui_text_edit_copy(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RIGHT)
							sui_text_edit_forward(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_LEFT)
							sui_text_edit_back(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_DELETE)
							sui_text_edit_delete(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_END)
							sui_text_edit_end(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_HOME)
							sui_text_edit_home(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_BACKSPACE)
							sui_text_edit_backspace(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RETURN) 
						{
							if(seduce_text_state[i].done_func != NULL)
							{
								void (* done_func)(void *user, char *text);
								done_func = seduce_text_state[i].done_func;
								done_func(user, t);
								free(seduce_text_state[i].copy);
							}
							seduce_text_state[i].id = NULL;
							return S_TIS_DONE;
						}
						else if(input->button_event[j].character > 31 && input->button_event[j].character < 256)
							sui_text_edit_insert_character(t, buffer_size, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->button_event[j].character);
					}
				}
			}
		}
	}
	for(i = 0; i < input->user_count; i++)
		if(seduce_text_state[i].id == id)
			return S_TIS_ACTIVE;
	return S_TIS_IDLE;
}

STypeInState seduce_text_double_edit(BInputState *input, void *id, double *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, double value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha)
{
	float array[6], output[2];
	char *t, buffer[256];
	uint i, j, k, l, user_count;
	if(input->mode == BAM_DRAW)
	{
		array[0] = pos_x;
		array[1] = pos_y + size * 0.5;
		array[2] = 0;
		array[3] = pos_x + length;
		array[4] = pos_y + size * 0.5;
		array[5] = 0;
		seduce_element_add_line(input, id, 0, array, &array[3], size * 2.0);
		seduce_element_add_surface(input, id);
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				t = seduce_text_state[i].copy;
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], length) + seduce_text_state[i].scroll_start);
				sui_text_line_edit_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, length, t, &seduce_text_state[i].scroll_start, seduce_text_state[i].select_start, seduce_text_state[i].select_end, active_red, active_green, active_blue, active_alpha);
				break;
			}
		}
		if(i == input->user_count)
		{
			sprintf(buffer, "%f", *value);
			for(j = 0; 0 != buffer[j] && 46 != buffer[j]; j++);
			if(0 != buffer[j])
			{
				int pos;
				pos = -1;
				for(j++; 0 != buffer[j]; j++)
				{
					if(48 == buffer[j])
						pos++;
					else
						pos = 0;
				}
				if(pos > 0)
					buffer[j - pos] = 0;
			}
			for(i = 0; i < input->user_count && id != seduce_element_pointer_id(input, i, NULL); i++);
			if(i < input->user_count)
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
			else
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, red, green, blue, alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
		}

	}else if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(input->pointers[i].button[0] == TRUE && input->pointers[i].last_button[0] == FALSE)
			{
				if(seduce_text_state[input->pointers[i].user_id].id != id && id == seduce_element_pointer_id(input, i, NULL))
				{
					sprintf(buffer, "%f", *value);
					for(j = 0; 0 != buffer[j] && 46 != buffer[j]; j++);
					if(0 != buffer[j])
					{
						int pos;
						pos = -1;
						for(j++; 0 != buffer[j]; j++)
						{
							if(48 == buffer[j])
								pos++;
							else
								pos = 0;
						}
						if(pos > 0)
							buffer[j - pos] = 0;
					}
					seduce_text_activate(input->pointers[i].user_id, id, buffer, 256, done_func, user, TRUE);
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
			if(seduce_text_state[i].id == id)
				for(j = 0; j < input->pointer_count; j++)
					if(input->pointers[j].user_id == i)
						if(seduce_element_surface_project(input, id, output, input->pointers[j].pointer_x, input->pointers[j].pointer_y))
							sui_text_line_edit_mouse(size, SEDUCE_T_SPACE, length, seduce_text_state[i].copy, &seduce_text_state[i].scroll_start, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->pointers[j].button[0], input->pointers[j].last_button[0], output[0] - pos_x);

		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				for(j = 0; j < input->button_event_count; j++)
				{
					if(input->button_event[j].state == TRUE && input->button_event[j].user_id == i)
					{
						t = seduce_text_state[i].copy;
						if(input->button_event[j].button == BETRAY_BUTTON_PASTE)
						{
							boolean negative = FALSE;
							boolean period = FALSE;

							sui_text_edit_paste(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
							for(k = 0; t[k] != 0; k++)
							{
								if(t[k] == '-')
									negative = TRUE;
								if((t[k] <= 47 || t[k] >= 58) && (t[k] != '.' || period))
								{
									l = k;
									sui_text_edit_delete(t, 256, &k, &l);
									k--;
									seduce_text_state[i].select_end--;
								}
								if(t[k] != '.')
									period = TRUE;							
							}
							if(negative && t[0] != '-')
							{
								uint s = 0, e = 0;
								sui_text_edit_insert_character(t, 256, &s, &e, '-');
								seduce_text_state[i].select_start++;
								seduce_text_state[i].select_end++;
							}
						}else if(input->button_event[j].button == BETRAY_BUTTON_CUT)
							sui_text_edit_cut(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_COPY)
							sui_text_edit_copy(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RIGHT)
							sui_text_edit_forward(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_LEFT)
							sui_text_edit_back(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_DELETE)
							sui_text_edit_delete(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_END)
							sui_text_edit_end(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_HOME)
							sui_text_edit_home(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_BACKSPACE)
							sui_text_edit_backspace(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RETURN) 
						{
							double decimal = 0.1, v;
							v = 0;
							if(t[0] == '-')
								k = 1;
							else
								k = 0;
							for(; t[k] != 0 && t[k] != '.'; k++)
								if(t[k] > 47 && t[k] < 58)
									v = (t[k] - 48) + v * 10.0;
							if(t[k] == '.')
							{
								for(; t[k] != 0; k++)
								{
									if(t[k] > 47 && t[k] < 58)
									{
										v += decimal * (t[k] - 48);
										decimal /= 10.0;
									}
								}
							}
							if(t[0] == '-')
								v = -v;
							if(seduce_text_state[i].done_func != NULL)
							{
								void (* done_func)(void *user, double value);
								done_func = seduce_text_state[i].done_func;
								done_func(user, v);
								free(seduce_text_state[i].copy);
							}else
								*value = v;
							seduce_text_state[i].id = NULL;
							return S_TIS_DONE;
						}
						else if((input->button_event[j].character > 47 && 
								input->button_event[j].character < 58))
							sui_text_edit_insert_character(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->button_event[j].character);
						else if(input->button_event[j].character == '-' && t[0] != '-')
						{
							uint s = 0, e = 0;
							sui_text_edit_insert_character(t, 256, &s, &e, '-');
							seduce_text_state[i].select_start++;
							seduce_text_state[i].select_end++;
						}else if(input->button_event[j].character == '+' && t[0] == '-')
						{
							uint s = 0, e = 0;
							sui_text_edit_delete(t, 256, &s, &e);
							if(seduce_text_state[i].select_start > 0)
								seduce_text_state[i].select_start--;
							if(seduce_text_state[i].select_end > 0)
								seduce_text_state[i].select_end--;
						}
						else if(input->button_event[j].character == '.' ||
							input->button_event[j].character == ',')
						{
							uint l;
							for(k = 0; t[k] != 0 && t[k] != '.'; k++);
							if(t[k] == '.')
							{
								l = k;
								sui_text_edit_delete(t, 256, &l, &k);
								if(seduce_text_state[i].select_start >= l)
									seduce_text_state[i].select_start--;
								if(seduce_text_state[i].select_end >= l)
									seduce_text_state[i].select_end--;
							}
							sui_text_edit_insert_character(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, '.');
						}
					}
				}
			}
		}
	}
	for(i = 0; i < input->user_count; i++)
		if(seduce_text_state[i].id == id)
			return S_TIS_ACTIVE;
	return S_TIS_IDLE;
}


STypeInState seduce_text_float_edit(BInputState *input, void *id, float *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, double value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha)
{
	float array[6], output[2];
	char *t, buffer[256];
	uint i, j, k, l, user_count;
	if(input->mode == BAM_DRAW)
	{
		array[0] = pos_x;
		array[1] = pos_y + size * 0.5;
		array[2] = 0;
		array[3] = pos_x + length;
		array[4] = pos_y + size * 0.5;
		array[5] = 0;
		seduce_element_add_line(input, id, 0, array, &array[3], size * 2.0);
		seduce_element_add_surface(input, id);
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				t = seduce_text_state[i].copy;
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], length) + seduce_text_state[i].scroll_start);
				sui_text_line_edit_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, length, t, &seduce_text_state[i].scroll_start, seduce_text_state[i].select_start, seduce_text_state[i].select_end, active_red, active_green, active_blue, active_alpha);
				break;
			}
		}
		if(i == input->user_count)
		{
			sprintf(buffer, "%f", *value);
			for(j = 0; 0 != buffer[j] && 46 != buffer[j]; j++);
			if(0 != buffer[j])
			{
				int pos;
				pos = -1;
				for(j++; 0 != buffer[j]; j++)
				{
					if(48 == buffer[j])
						pos++;
					else
						pos = 0;
				}
				if(pos > 0)
					buffer[j - pos] = 0;
			}
			for(i = 0; i < input->user_count && id != seduce_element_pointer_id(input, i, NULL); i++);
			if(i < input->user_count)
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
			else
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, red, green, blue, alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
		}

	}else if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(input->pointers[i].button[0] == TRUE && input->pointers[i].last_button[0] == FALSE)
			{
				if(seduce_text_state[input->pointers[i].user_id].id != id && id == seduce_element_pointer_id(input, i, NULL))
				{
					sprintf(buffer, "%f", *value);
					for(j = 0; 0 != buffer[j] && 46 != buffer[j]; j++);
					if(0 != buffer[j])
					{
						int pos;
						pos = -1;
						for(j++; 0 != buffer[j]; j++)
						{
							if(48 == buffer[j])
								pos++;
							else
								pos = 0;
						}
						if(pos > 0)
							buffer[j - pos] = 0;
					}
					seduce_text_activate(input->pointers[i].user_id, id, buffer, 256, done_func, user, TRUE);
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
			if(seduce_text_state[i].id == id)
				for(j = 0; j < input->pointer_count; j++)
					if(input->pointers[j].user_id == i)
						if(seduce_element_surface_project(input, id, output, input->pointers[j].pointer_x, input->pointers[j].pointer_y))
							sui_text_line_edit_mouse(size, SEDUCE_T_SPACE, length, seduce_text_state[i].copy, &seduce_text_state[i].scroll_start, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->pointers[j].button[0], input->pointers[j].last_button[0], output[0] - pos_x);

		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				for(j = 0; j < input->button_event_count; j++)
				{
					if(input->button_event[j].state == TRUE && input->button_event[j].user_id == i)
					{
						t = seduce_text_state[i].copy;
						if(input->button_event[j].button == BETRAY_BUTTON_PASTE)
						{
							boolean negative = FALSE;
							boolean period = FALSE;

							sui_text_edit_paste(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
							for(k = 0; t[k] != 0; k++)
							{
								if(t[k] == '-')
									negative = TRUE;
								if((t[k] <= 47 || t[k] >= 58) && (t[k] != '.' || period))
								{
									l = k;
									sui_text_edit_delete(t, 256, &k, &l);
									k--;
									seduce_text_state[i].select_end--;
								}
								if(t[k] != '.')
									period = TRUE;							
							}
							if(negative && t[0] != '-')
							{
								uint s = 0, e = 0;
								sui_text_edit_insert_character(t, 256, &s, &e, '-');
								seduce_text_state[i].select_start++;
								seduce_text_state[i].select_end++;
							}
						}else if(input->button_event[j].button == BETRAY_BUTTON_CUT)
							sui_text_edit_cut(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_COPY)
							sui_text_edit_copy(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RIGHT)
							sui_text_edit_forward(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_LEFT)
							sui_text_edit_back(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_DELETE)
							sui_text_edit_delete(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_END)
							sui_text_edit_end(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_HOME)
							sui_text_edit_home(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_BACKSPACE)
							sui_text_edit_backspace(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RETURN) 
						{
							float decimal = 0.1, v;
							v = 0;
							if(t[0] == '-')
								k = 1;
							else
								k = 0;
							for(; t[k] != 0 && t[k] != '.'; k++)
								if(t[k] > 47 && t[k] < 58)
									v = (t[k] - 48) + v * 10.0;
							if(t[k] == '.')
							{
								for(; t[k] != 0; k++)
								{
									if(t[k] > 47 && t[k] < 58)
									{
										v += decimal * (t[k] - 48);
										decimal /= 10.0;
									}
								}
							}
							if(t[0] == '-')
								v = -v;
							if(seduce_text_state[i].done_func != NULL)
							{
								void (* done_func)(void *user, double value);
								done_func = seduce_text_state[i].done_func;
								done_func(user, v);
								free(seduce_text_state[i].copy);
							}else
								*value = v;
							seduce_text_state[i].id = NULL;
							return S_TIS_DONE;
						}
						else if((input->button_event[j].character > 47 && 
								input->button_event[j].character < 58))
							sui_text_edit_insert_character(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->button_event[j].character);
						else if(input->button_event[j].character == '-' && t[0] != '-')
						{
							uint s = 0, e = 0;
							sui_text_edit_insert_character(t, 256, &s, &e, '-');
							seduce_text_state[i].select_start++;
							seduce_text_state[i].select_end++;
						}else if(input->button_event[j].character == '+' && t[0] == '-')
						{
							uint s = 0, e = 0;
							sui_text_edit_delete(t, 256, &s, &e);
							if(seduce_text_state[i].select_start > 0)
								seduce_text_state[i].select_start--;
							if(seduce_text_state[i].select_end > 0)
								seduce_text_state[i].select_end--;
						}
						else if(input->button_event[j].character == '.' ||
							input->button_event[j].character == ',')
						{
							uint l;
							for(k = 0; t[k] != 0 && t[k] != '.'; k++);
							if(t[k] == '.')
							{
								l = k;
								sui_text_edit_delete(t, 256, &l, &k);
								if(seduce_text_state[i].select_start >= l)
									seduce_text_state[i].select_start--;
								if(seduce_text_state[i].select_end >= l)
									seduce_text_state[i].select_end--;
							}
							sui_text_edit_insert_character(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, '.');
						}
					}
				}
			}
		}
	}
	for(i = 0; i < input->user_count; i++)
		if(seduce_text_state[i].id == id)
			return S_TIS_ACTIVE;
	return S_TIS_IDLE;
}

STypeInState seduce_text_int_edit(BInputState *input, void *id, int *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, int value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha)
{
	float array[6], output[2];
	char *t, buffer[256];
	uint i, j, k, l, user_count;
	if(input->mode == BAM_DRAW)
	{
		array[0] = pos_x;
		array[1] = pos_y + size * 0.5;
		array[2] = 0;
		array[3] = pos_x + length;
		array[4] = pos_y + size * 0.5;
		array[5] = 0;
		seduce_element_add_line(input, id, 0, array, &array[3], size * 2.0);
		seduce_element_add_surface(input, id);

		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				t = seduce_text_state[i].copy;
				if(!left)
					pos_x += length - seduce_text_line_length(size, SEDUCE_T_SPACE, t, -1);
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], length) + seduce_text_state[i].scroll_start);
				sui_text_line_edit_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, length, t, &seduce_text_state[i].scroll_start, seduce_text_state[i].select_start, seduce_text_state[i].select_end, active_red, active_green, active_blue, active_alpha);
				break;
			}
		}
		if(i == input->user_count)
		{
			sprintf(buffer, "%i", *value);
			if(!left)
				pos_x += length - seduce_text_line_length(size, SEDUCE_T_SPACE, buffer, -1);
			for(i = 0; i < input->user_count && id != seduce_element_pointer_id(input, i, NULL); i++);
			if(i < input->user_count)
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
			else
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, red, green, blue, alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
		}
	}else if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(input->pointers[i].button[0] == TRUE && input->pointers[i].last_button[0] == FALSE)
			{
				if(seduce_text_state[input->pointers[i].user_id].id != id && id == seduce_element_pointer_id(input, i, NULL))
				{
					sprintf(buffer, "%i", *value);
					seduce_text_activate(input->pointers[i].user_id, id, buffer, 256, done_func, user, TRUE);
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
			if(seduce_text_state[i].id == id)
				for(j = 0; j < input->pointer_count; j++)
					if(input->pointers[j].user_id == i)
						if(seduce_element_surface_project(input, id, output, input->pointers[j].pointer_x, input->pointers[j].pointer_y))
							sui_text_line_edit_mouse(size, SEDUCE_T_SPACE, length, seduce_text_state[i].copy, &seduce_text_state[i].scroll_start, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->pointers[j].button[0], input->pointers[j].last_button[0], output[0] - pos_x);
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				for(j = 0; j < input->button_event_count; j++)
				{
					if(input->button_event[j].state == TRUE && input->button_event[j].user_id == i)
					{
						t = seduce_text_state[i].copy;
						if(input->button_event[j].button == BETRAY_BUTTON_PASTE)
						{
							boolean negative = FALSE;
							sui_text_edit_paste(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
							for(k = 0; t[k] != 0; k++)
							{
								if(t[k] == '-')
									negative = TRUE;
								if(t[k] <= 47 || t[k] >= 58)
								{
									l = k;
									sui_text_edit_delete(t, 256, &k, &l);
									k--;
									seduce_text_state[i].select_end--;
								}						
							}
							if(negative && t[0] != '-')
							{
								uint s = 0, e = 0;
								sui_text_edit_insert_character(t, 256, &s, &e, '-');
								seduce_text_state[i].select_start++;
								seduce_text_state[i].select_end++;
							}
						}else if(input->button_event[j].button == BETRAY_BUTTON_CUT)
							sui_text_edit_cut(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_COPY)
							sui_text_edit_copy(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RIGHT)
							sui_text_edit_forward(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_LEFT)
							sui_text_edit_back(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_DELETE)
							sui_text_edit_delete(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_END)
							sui_text_edit_end(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_HOME)
							sui_text_edit_home(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_BACKSPACE)
							sui_text_edit_backspace(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RETURN) 
						{
							int decimal = 0.1, v;
							v = 0;
							if(t[0] == '-')
								k = 1;
							else
								k = 0;
							for(; t[k] != 0; k++)
								if(t[k] > 47 && t[k] < 58)
									v = (t[k] - 48) + v * 10;
							if(t[0] == '-')
								v = -v;
							if(seduce_text_state[i].done_func != NULL)
							{
								void (* done_func)(void *user, int value);
								done_func = seduce_text_state[i].done_func;
								done_func(user, v);
								free(seduce_text_state[i].copy);
							}else
								*value = v;
							seduce_text_state[i].id = NULL;
							return S_TIS_DONE;
						}
						else if((input->button_event[j].character > 47 && input->button_event[j].character < 58))
							sui_text_edit_insert_character(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->button_event[j].character);
						else if(input->button_event[j].character == '-' && t[0] != '-')
						{
							uint s = 0, e = 0;
							sui_text_edit_insert_character(t, 256, &s, &e, '-');
							seduce_text_state[i].select_start++;
							seduce_text_state[i].select_end++;
						}else if(input->button_event[j].character == '+' && t[0] == '-')
						{
							uint s = 0, e = 0;
							sui_text_edit_delete(t, 256, &s, &e);
							if(seduce_text_state[i].select_start > 0)
								seduce_text_state[i].select_start--;
							if(seduce_text_state[i].select_end > 0)
								seduce_text_state[i].select_end--;
						}
					}
				}
			}
		}
	}
	for(i = 0; i < input->user_count; i++)
		if(seduce_text_state[i].id == id)
			return S_TIS_ACTIVE;
	return S_TIS_IDLE;
}


STypeInState seduce_text_uint_edit(BInputState *input, void *id, uint *value, float pos_x, float pos_y, float length, float size, boolean left, void (*done_func)(void *user, uint value), void *user, float red, float green, float blue, float alpha, float active_red, float active_green, float active_blue, float active_alpha)
{
	float array[6], output[2];
	char *t, buffer[256];
	uint i, j, k, l, user_count;
	if(input->mode == BAM_DRAW)
	{
		array[0] = pos_x;
		array[1] = pos_y + size * 0.5;
		array[2] = 0;
		array[3] = pos_x + length;
		array[4] = pos_y + size * 0.5;
		array[5] = 0;
		seduce_element_add_line(input, id, 0, array, &array[3], size * 2.0);
		seduce_element_add_surface(input, id);
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				t = seduce_text_state[i].copy;

				if(!left)
					pos_x += length - seduce_text_line_length(size, SEDUCE_T_SPACE, t, -1);
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, &t[seduce_text_state[i].scroll_start], length) + seduce_text_state[i].scroll_start);
				sui_text_line_edit_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, length, t, &seduce_text_state[i].scroll_start, seduce_text_state[i].select_start, seduce_text_state[i].select_end, active_red, active_green, active_blue, active_alpha);
				break;
			}
		}
		if(i == input->user_count)
		{
			sprintf(buffer, "%u", *value);
			if(!left)
				pos_x += length - seduce_text_line_length(size, SEDUCE_T_SPACE, buffer, -1);
			for(i = 0; i < input->user_count && id != seduce_element_pointer_id(input, i, NULL); i++);
			if(i < input->user_count)
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, active_red, active_green, active_blue, active_alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
			else
				seduce_text_line_draw(pos_x, pos_y, size, SEDUCE_T_SPACE, buffer, red, green, blue, alpha, seduce_text_line_hit_test(size, SEDUCE_T_SPACE, buffer, length));
		}

	}else if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(input->pointers[i].button[0] == TRUE && input->pointers[i].last_button[0] == FALSE)
			{
				if(seduce_text_state[input->pointers[i].user_id].id != id && id == seduce_element_pointer_id(input, i, NULL))
				{
					sprintf(buffer, "%u", *value);
					seduce_text_activate(input->pointers[i].user_id, id, buffer, 256, done_func, user, TRUE);
				}
			}
		}
		for(i = 0; i < input->user_count; i++)
			if(seduce_text_state[i].id == id)
				for(j = 0; j < input->pointer_count; j++)
					if(input->pointers[j].user_id == i)
						if(seduce_element_surface_project(input, id, output, input->pointers[j].pointer_x, input->pointers[j].pointer_y))
							sui_text_line_edit_mouse(size, SEDUCE_T_SPACE, length, seduce_text_state[i].copy, &seduce_text_state[i].scroll_start, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->pointers[j].button[0], input->pointers[j].last_button[0], output[0] - pos_x);
		for(i = 0; i < input->user_count; i++)
		{
			if(seduce_text_state[i].id == id)
			{
				for(j = 0; j < input->button_event_count; j++)
				{
					if(input->button_event[j].state == TRUE && input->button_event[j].user_id == i)
					{
						t = seduce_text_state[i].copy;
						if(input->button_event[j].button == BETRAY_BUTTON_PASTE)
						{
							sui_text_edit_paste(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
							for(k = 0; t[k] != 0; k++)
							{
								if(t[k] <= 47 || t[k] >= 58)
								{
									l = k;
									sui_text_edit_delete(t, 256, &k, &l);
									k--;
									seduce_text_state[i].select_end--;
								}						
							}
						}else if(input->button_event[j].button == BETRAY_BUTTON_CUT)
							sui_text_edit_cut(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_COPY)
							sui_text_edit_copy(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RIGHT)
							sui_text_edit_forward(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_LEFT)
							sui_text_edit_back(t, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_DELETE)
							sui_text_edit_delete(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_END)
							sui_text_edit_end(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_HOME)
							sui_text_edit_home(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_BACKSPACE)
							sui_text_edit_backspace(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end);
						else if(input->button_event[j].button == BETRAY_BUTTON_RETURN) 
						{
							uint v;
							v = 0;
							k = 0;
							for(; t[k] != 0; k++)
								if(t[k] > 47 && t[k] < 58)
									v = (t[k] - 48) + v * 10;
							if(seduce_text_state[i].done_func != NULL)
							{
								void (* done_func)(void *user, uint value);
								done_func = seduce_text_state[i].done_func;
								done_func(user, v);
								free(seduce_text_state[i].copy);
							}else
								*value = v;
							seduce_text_state[i].id = NULL;
							return S_TIS_DONE;
						}
						else if((input->button_event[j].character > 47 && input->button_event[j].character < 58))
							sui_text_edit_insert_character(t, 256, &seduce_text_state[i].select_start, &seduce_text_state[i].select_end, input->button_event[j].character);
					}
				}
			}
		}
	}
	for(i = 0; i < input->user_count; i++)
		if(seduce_text_state[i].id == id)
			return S_TIS_ACTIVE;
	return S_TIS_IDLE;
}


void sui_text_double_edit_print(double value, char *buffer)
{
	uint i;
	sprintf(buffer, "%f", value);
	for(i = 0; buffer[i] != 0; i++);
	for(i--; i > 0 && buffer[i] == '0'; i--);
	if(buffer[i] == '.')
	{
		buffer[i + 1] = '0';
		buffer[i + 2] = 0;
	}else
		buffer[i + 1] = 0;
}

