#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "s_draw_3d.h"
#include "c_internal.h"

extern void c_editor_type(BInputState *input, CSession *session, CEntity *entity, CType *type, CTypeType base_type, uint *ref, float *stack, float x, float y, RMatrix *matrix, float *center, float scale, float time, uint recursions);
extern float c_editor_popup_animation_get();

extern uint c_editor_display_stack[64];
extern float c_editor_display_stack_timer[64];
extern uint c_editor_display_stack_last[64];
extern float c_editor_display_stack_timer_last[64];

void c_editor_draw_braces(float x, float y, float size, float time)
{
	time *= 0.25;
	r_primitive_line_2d(x - size, y + 0.11, x - 0.01, y + 0.11, 1, 1, 1, time);
	r_primitive_line_2d(x + size, y + 0.11, x + 0.01, y + 0.11, 1, 1, 1, time);
	r_primitive_line_2d(x - size, y + 0.11, x - size - 0.01, y + 0.12, 1, 1, 1, time);
	r_primitive_line_2d(x + size, y + 0.11, x + size + 0.01, y + 0.12, 1, 1, 1, time);
	r_primitive_line_2d(x + 0.01, y + 0.11, x, y + 0.1, 1, 1, 1, time);
	r_primitive_line_2d(x - 0.01, y + 0.11, x, y + 0.1, 1, 1, 1, time);
	r_primitive_line_flush();
}

void c_editor_special_animation(BInputState *input, CSession *session, CEntity *entity,  RMatrix *matrix, float x, float y, uint component, uint recursions, float time)
{
	static float bar_size_static = 0.1;
	float bar_size = 0.1;
	float handle[3];
	CComponent *c;
	c = &entity->components[component];
//	bar_size = 0.05 + 0.04 * sin(input->minute_time * 500);
	if(input->mode == BAM_DRAW)
	{
		char *n;
	//	r_primitive_surface(x - 0.5, y + 0.16, 0, 1.0, bar_size, 1, 0, 0, 1);
		n = confuse_component_name_get(session, c->type);
		seduce_text_line_draw(x + seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE, n, -1), y + 0.12, SEDUCE_T_SIZE, SEDUCE_T_SPACE, n, 1, 1, 1, 0.5 * time, -1);
		c_editor_draw_braces(x, y, 0.5, time);
	}

	handle[0] = x + 0.55;
	handle[1] = y + 0.16 + bar_size_static;
	handle[2] = 0;
	if(seduce_manipulator_point_axis(input, NULL, handle, &bar_size_static, NULL, FALSE, 1, 0.3))
	{
		bar_size_static = handle[1] - (y + 0.16);
		if(bar_size_static < 0.02)
			bar_size_static = 0.02;
	}
	bar_size = bar_size_static * time;
	if(c->special != NULL)
	{
		CSpecialAnimation *anim;
		uint i, j, k;
		float line_x, line_y, last_x, last_y;
		anim = c->special;
		for(i = 0; i < input->pointer_count; i++)
		{
			if(input->pointers[i].pointer_x > x - 0.5 && input->pointers[i].pointer_x < x + 0.5)
			{
				for(j = 0; j < anim->key_count; j++)
					if(anim->keys[j].key_time + 0.01 > input->pointers[i].pointer_x + 0.5 - x &&
						anim->keys[j].key_time - 0.01 < input->pointers[i].pointer_x + 0.5 - x)
						break;
				if(j == anim->key_count)
				{
					if(seduce_widget_icon_button(input, anim, SUI_3D_OBJECT_PLUS, input->pointers[0].pointer_x, y + 0.14, 0.01, time, NULL))
					{
						if(anim->key_allocated == anim->key_count)
						{
							anim->key_allocated += 16; 
							anim->keys = realloc(anim->keys, (sizeof *anim->keys) * anim->key_allocated);
							c->references = realloc(c->references, (sizeof *anim->keys) * (anim->key_allocated + 2));
						}
						for(j = 0; j < anim->key_count && anim->keys[j].key_time < input->pointers[i].pointer_x + 0.5 - x; j++);

						c->references[anim->key_count + 2] = c->references[anim->key_count + 1];
						for(k = anim->key_count; k != j; k--)
						{
							anim->keys[k] = anim->keys[k -1];
							c->references[k + 1] = c->references[k + 0];
						}
						
						c->references[j + 1] = c_entity_stack_add(entity, entity->stack_types[c->references[1]].type, "Key", 1);
						anim->keys[j].key_time = input->pointers[i].pointer_x + 0.5 - x;
						anim->keys[j].key_ease[0] = 2.0 / 3.0;
						anim->keys[j].key_ease[1] = 1.0 / 3.0;
						anim->key_count++;

					}
				}
			}
		}

		
		c_editor_type(input, session, entity, &entity->stack_types[c->references[0]], C_TYPE_FLOAT, &c->references[0], entity->stack, x - 0.55, y + 0.16 + bar_size * 0.5, matrix, &entity->stack[entity->stack_types[c->references[0]].pos], 1, time, recursions);
		for(i = 0; i < anim->key_count; i++)
		{
			handle[0] = x - 0.5 + anim->keys[i].key_time;
			handle[1] = y + 0.16 + bar_size * 0.5;
			handle[2] = 0.0;
			if(seduce_manipulator_point_axis(input, NULL, handle, &c->references[i + 1], NULL, FALSE, 0, 0.6))
			{
				anim->keys[i].key_time = handle[0] - x + 0.5;
				if(i == 0)
				{
					if(anim->keys[i].key_time < 0.0)
						anim->keys[i].key_time = 0.0;
				}else
					if(anim->keys[i].key_time < anim->keys[i - 1].key_time + 0.005)
						anim->keys[i].key_time = anim->keys[i - 1].key_time + 0.005;

				if(anim->key_count == i + 1)
				{
					if(anim->keys[i].key_time > 1.0)
						anim->keys[i].key_time = 1.0;
				}else
					if(anim->keys[i].key_time > anim->keys[i + 1].key_time - 0.005)
						anim->keys[i].key_time = anim->keys[i + 1].key_time - 0.005;
			}

			c_editor_type(input, session, entity, &entity->stack_types[c->references[i + 1]], entity->stack_types[c->references[i + 1]].type, &c->references[i + 1], entity->stack, x - 0.5 + anim->keys[i].key_time, y + 0.19 + bar_size, matrix, &entity->stack[entity->stack_types[c->references[0]].pos], 1, time, recursions);
			
			if(anim->key_count > 1 && seduce_widget_icon_button(input, &anim->keys[i].key_time, SUI_3D_OBJECT_MINUS, x - 0.5 + anim->keys[i].key_time, y + 0.14, 0.01, 1, NULL))
			{
				anim->key_count--;
				for(k = i; k < anim->key_count - 1; k++)
				{
					anim->keys[k] = anim->keys[k + 1];
					c->references[k + 1] = c->references[k + 2];
				}
				c->references[k + 1] = c->references[k + 2];
				return;
			}

			handle[0] = x - 0.5 + anim->keys[i].key_time * 2.0 / 3.0 + anim->keys[i + 1].key_time * 1.0 / 3.0;
			handle[1] = y + 0.16 + bar_size * anim->keys[i].key_ease[1];
			handle[2] = 0;

			if(seduce_manipulator_point_axis(input, NULL, handle, &anim->keys[i + 0].key_ease[1], NULL, FALSE, 1, 0.6))
			{
				anim->keys[i + 0].key_ease[1] = (handle[1] - (y + 0.16)) / bar_size;
				if(anim->keys[i + 0].key_ease[1] > 1.0)
					anim->keys[i + 0].key_ease[1] = 1.0;
				if(anim->keys[i + 0].key_ease[1] < 0.0)
					anim->keys[i + 0].key_ease[1] = 0.0;
			}
		//	r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16, handle[0], handle[1], 1, 1, 1, 0.25);

			handle[0] = x - 0.5 + anim->keys[i].key_time * 1.0 / 3.0 + anim->keys[i + 1].key_time * 2.0 / 3.0;
			handle[1] = y + 0.16 + bar_size * anim->keys[i + 1].key_ease[0];
			if(seduce_manipulator_point_axis(input, NULL, handle, &anim->keys[i + 1].key_ease[0], NULL, FALSE, 1, 0.6))
			{
				anim->keys[i + 1].key_ease[0] = (handle[1] - (y + 0.16)) / bar_size;
				if(anim->keys[i + 1].key_ease[0] > 1.0)
					anim->keys[i + 1].key_ease[0] = 1.0;
				if(anim->keys[i + 1].key_ease[0] < 0.0)
					anim->keys[i + 1].key_ease[0] = 0.0;
			}
		}

		if(input->mode == BAM_DRAW)
		{
			float f;
			f = entity->stack[entity->stack_types[c->references[0]].pos];
			r_primitive_line_2d(x - 0.5 + f, y + 0.16, x - 0.5 + f, y + 0.15 + bar_size, 1, 1, 1, 1 - time);
	
			for(i = 0; i < anim->key_count; i++)
			{
				for(j = 0; j < input->pointer_count && &c->references[i + 1] != seduce_element_pointer_id(input, j, NULL); j++);

				if(entity->stack_types[c->references[anim->key_count + 1]].type == C_TYPE_COLOR)
				{
					float *rgb;
					rgb = &entity->stack[entity->stack_types[c->references[i + 1]].pos];
					if(j < input->pointer_count)
					{
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16, x - 0.5 + anim->keys[i].key_time, y + 0.15 + bar_size * 0.5, rgb[0], rgb[1], rgb[2], time);
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16 + bar_size, x - 0.5 + anim->keys[i].key_time, y + 0.17 + bar_size * 0.5, rgb[0], rgb[1], rgb[2], time);
					}else
					{
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16, x - 0.5 + anim->keys[i].key_time, y + 0.15 + bar_size * 0.5, 0.3 + rgb[0], 0.3 + rgb[1], 0.3 + rgb[2], time);
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16 + bar_size, x - 0.5 + anim->keys[i].key_time, y + 0.17 + bar_size * 0.5, 0.3 + rgb[0], 0.3 + rgb[1], 0.3 + rgb[2], time);
					}
				}else
				{
					if(j < input->pointer_count)
					{
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16, x - 0.5 + anim->keys[i].key_time, y + 0.15 + bar_size * 0.5, 1, 1, 1, time);
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16 + bar_size, x - 0.5 + anim->keys[i].key_time, y + 0.17 + bar_size * 0.5, 1, 1, 1, time);
					}else
					{
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16, x - 0.5 + anim->keys[i].key_time, y + 0.15 + bar_size * 0.5, 0.5, 0.5, 0.5, time);
						r_primitive_line_2d(x - 0.5 + anim->keys[i].key_time, y + 0.16 + bar_size, x - 0.5 + anim->keys[i].key_time, y + 0.17 + bar_size * 0.5, 0.5, 0.5, 0.5, time);
					}
				}
				if(i + 1 < anim->key_count)
				{

				//	r_primitive_line_2d(x - 0.5 + anim->keys[i + 1].key_time, y + 0.16 + bar_size, handle[0], handle[1], 1, 1, 1, 0.25);

					if(entity->stack_types[c->references[anim->key_count + 1]].type == C_TYPE_COLOR)
					{
						float rgb_last[3], rgb_line[3], *a, *b, a_handle[3], b_handle[3];
						last_x = x - 0.5 + anim->keys[i].key_time;
						last_y = y + 0.16;
						a = &entity->stack[entity->stack_types[c->references[i + 1]].pos];
						b = &entity->stack[entity->stack_types[c->references[i + 2]].pos];
						rgb_last[0] = a[0];
						rgb_last[1] = a[1];
						rgb_last[2] = a[2];
						a_handle[0] = a[0] + (b[0] - a[0]) * anim->keys[i + 0].key_ease[1];
						a_handle[1] = a[1] + (b[1] - a[1]) * anim->keys[i + 0].key_ease[1];
						a_handle[2] = a[2] + (b[2] - a[2]) * anim->keys[i + 0].key_ease[1];
						b_handle[0] = a[0] + (b[0] - a[0]) * anim->keys[i + 1].key_ease[0];
						b_handle[1] = a[1] + (b[1] - a[1]) * anim->keys[i + 1].key_ease[0];
						b_handle[2] = a[2] + (b[2] - a[2]) * anim->keys[i + 1].key_ease[0];
						for(j = 1; j < 21; j++)
						{
							line_x = x - 0.5 + anim->keys[i].key_time + (anim->keys[i + 1].key_time - anim->keys[i].key_time) * (float)j / 20.0;
							line_y = y + f_splinef((float)j / 20.0, 0.16, 0.16 + bar_size * anim->keys[i].key_ease[1], 0.16 + bar_size * anim->keys[i + 1].key_ease[0], 0.16 + bar_size);						
							f_spline3df(rgb_line, (float)j / 20.0, a, a_handle, b_handle, b);
							r_primitive_line_fade_2d(line_x, line_y, last_x, last_y, rgb_line[0], rgb_line[1], rgb_line[2], time, rgb_last[0], rgb_last[1], rgb_last[2], time);
							last_x = line_x;
							last_y = line_y;
							rgb_last[0] = rgb_line[0];
							rgb_last[1] = rgb_line[1];
							rgb_last[2] = rgb_line[2];
						}
					}else
					{
						last_x = x - 0.5 + anim->keys[i].key_time;
						last_y = y + 0.16;
						for(j = 1; j < 21; j++)
						{
							line_x = x - 0.5 + anim->keys[i].key_time + (anim->keys[i + 1].key_time - anim->keys[i].key_time) * (float)j / 20.0;
							line_y = y + f_splinef((float)j / 20.0, 0.16, 0.16 + bar_size * anim->keys[i].key_ease[1], 0.16 + bar_size * anim->keys[i + 1].key_ease[0], 0.16 + bar_size);
							r_primitive_line_2d(line_x, line_y, last_x, last_y, 0.5, 0.5, 0.5, time);
							last_x = line_x;
							last_y = line_y;
						}
					}

					line_x = x - 0.5 + anim->keys[i].key_time * 2.0 / 3.0 + anim->keys[i + 1].key_time * 1.0 / 3.0;
					line_y = y + 0.16 + bar_size * anim->keys[i].key_ease[1];
					last_x = x - 0.5 + anim->keys[i].key_time;
					last_y = y + 0.16;
					r_primitive_line_2d(last_x, last_y, line_x, line_y, 0.3, 0.3, 0.3, 0.25 * time);

					line_x = x - 0.5 + anim->keys[i].key_time * 1.0 / 3.0 + anim->keys[i + 1].key_time * 2.0 / 3.0;
					line_y = y + 0.16 + bar_size * anim->keys[i + 1].key_ease[0];
					last_x = x - 0.5 + anim->keys[i + 1].key_time;
					last_y = y + 0.16 + bar_size;
					r_primitive_line_2d(last_x, last_y, line_x, line_y, 0.3, 0.3, 0.3, 0.25 * time);
				}
			}
			r_primitive_line_flush();

			if(entity->stack_types[c->references[anim->key_count + 1]].type == C_TYPE_POS)
			{
				RMatrix *reset;
				float *a, *b;
				reset = r_matrix_get();
				r_matrix_set(matrix);
				for(i = 1; i < anim->key_count; i++)
				{
					a = &entity->stack[entity->stack_types[c->references[i]].pos];
					b = &entity->stack[entity->stack_types[c->references[i + 1]].pos];
					
					r_primitive_line_3d(a[0], a[1], a[2], b[0], b[1], b[2], 0.3, 0.3, 0.3, 0.25 * time);
				}
				r_primitive_line_flush();
				r_matrix_set(reset);
			}
		}
	}
}

/*
typedef struct{
	float key_time;
	float key_ease[2];
}CSpecialKey;

typedef struct{
	CSpecialKey *keys;
	uint key_count;
	uint key_allocated;
}CSpecialAnimation;
*/
void c_editor_constraint(BInputState *input, CSession *session, CEntity *entity,  RMatrix *matrix, float x, float y, uint component, uint recursions, float timer)
{
	CComponent *c;
	float pos_x, pos_y = -0.3;
	uint i, count;
	char *n;


	c = &entity->components[component];

	if(seduce_widget_icon_button(input, &c->type, SUI_3D_OBJECT_MINUS, x + 0.02, y + 0.08, 0.01, 1, NULL))
	{
		c_entity_components_remove(session, entity, component, TRUE);
		return;
	}
	if(seduce_widget_icon_button(input, &c->special, SUI_3D_OBJECT_BOOST, x - 0.02, y + 0.08, 0.01, 1, NULL))
		entity->component_selected = component;

	if(c->type == 0) 
	{
		c_editor_special_animation(input, session, entity,  matrix, x, y, component, recursions, timer);
	}else
	{
		count = confuse_component_param_count_extend_get(session, c);
		pos_x = -0.05 * (count - 1);
		pos_y = 0.2;
		if(input->mode == BAM_DRAW)
		{
			r_matrix_push(NULL);
			r_matrix_translate(NULL, x, y, 0);
			r_matrix_rotate(NULL, (1 - timer) * 90, 0, 1, 0);
			n = confuse_component_name_get(session, c->type);
			seduce_text_line_draw(seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE, n, -1), 0.12, SEDUCE_T_SIZE, SEDUCE_T_SPACE, n, 1, 1, 1, 0.5, -1);
			c_editor_draw_braces(0, 0, 0.05 * ((float)count - 0.5), timer);
		}

		for(i = 0; i < count; i++)
		{
			c_editor_type(input, session, entity, &entity->stack_types[c->references[i]], session->component_list[c->type].param_types[i], &c->references[i], entity->stack, pos_x, pos_y, matrix, &entity->stack[entity->stack_types[c->references[0]].pos], 1, timer, recursions);
			n = confuse_component_param_name_get(session, c->type, i);
			if(entity->stack_types[c->references[i]].size > 1)
			{
				char select_nr[128];
				sprintf(select_nr, "<%u/", entity->stack_types[c->references[i]].selected % entity->stack_types[c->references[i]].size);
				if(seduce_text_button(input, &entity->stack_types[c->references[i]].selected, pos_x, pos_y - 0.04, 1, SEDUCE_T_SIZE, SEDUCE_T_SPACE, select_nr, timer, timer, timer, timer, timer, timer, timer, timer))
					entity->stack_types[c->references[i]].selected = (entity->stack_types[c->references[i]].selected + entity->stack_types[c->references[i]].size - 1) % entity->stack_types[c->references[i]].size;

				sprintf(select_nr, "%u>", entity->stack_types[c->references[i]].size);
				if(seduce_text_button(input, &entity->stack_types[c->references[i]].name[1], pos_x, pos_y - 0.04, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, select_nr, timer, timer, timer, timer, timer, timer, timer, timer))
					entity->stack_types[c->references[i]].selected = (entity->stack_types[c->references[i]].selected + 1) % entity->stack_types[c->references[i]].size;
				seduce_text_line_draw(seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE * timer, n, -1) + pos_x, pos_y - 0.06, SEDUCE_T_SIZE, SEDUCE_T_SPACE * timer, n, timer, timer, timer, timer, -1);
			}else
				seduce_text_line_draw(seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE * timer, n, -1) + pos_x, pos_y - 0.05, SEDUCE_T_SIZE, SEDUCE_T_SPACE * timer, n, timer, timer, timer, timer, -1);
			pos_x += 0.1;
		}
		if(input->mode == BAM_DRAW)
			r_matrix_pop(NULL);
	}
}

extern void c_editor_bar_rename(BInputState *input, void *user_pointer);

void c_editor_bar(BInputState *input, CSession *session, CEntity *entity,  RMatrix *matrix)
{
	static float scale = 0.5;
	CComponent *c;
	uint i, count;
	CTypeType *types;
	char **names;
	float timer, aspect;

	for(i = 0; i < 64; i++)
	{
		seduce_animate(input, &c_editor_display_stack_timer[i], c_editor_display_stack[i] != -1, 2.0);
		seduce_animate(input, &c_editor_display_stack_timer_last[i], FALSE, 2.0);
		if(c_editor_display_stack_timer_last[i] < 0.01)
			c_editor_display_stack_last[i] = -1;

	}

	if(entity->component_selected >= entity->component_count)
		return;
	c = &entity->components[entity->component_selected];

	timer = c_editor_popup_animation_get();
	if(input->mode == BAM_DRAW)
	{
		r_matrix_push(NULL);
		r_matrix_translate(NULL, 0, 0.05 - betray_screen_mode_get(NULL, NULL, NULL), 0);
	}
	seduce_widget_slider_radial(input, &scale, &scale, 0, 0.0, 0.05, scale * 0.25, 0, 1, timer, NULL);

	if(input->mode == BAM_DRAW)
		r_matrix_scale(NULL, scale * 2, scale * 2, scale * 2);


/*
	{
		static float pos[3] = {0, 0.4, 0},  pos2[3] = {0.4, 0.0, 0}, tmp[3];	
	//	seduce_manipulator_point_plane(input, NULL, pos, pos, NULL, FALSE, 2, 1);
		seduce_manipulator_point(input, NULL, pos, pos, 1);
		seduce_manipulator_pos_xyz(input, NULL, pos2, pos2, NULL, FALSE, TRUE, TRUE, FALSE, 2, 1);

		if(input->mode == BAM_DRAW)
		{
			r_matrix_projection_surfacef(NULL, tmp, pos, 2, input->pointers[0].pointer_x, input->pointers[0].pointer_y);
			r_primitive_line_3d(tmp[0] + 1, tmp[1], tmp[2], tmp[0] - 1, tmp[1], tmp[2], 1, 0, 0, 1);
			r_primitive_line_3d(tmp[0], tmp[1] + 1, tmp[2], tmp[0], tmp[1] - 1, tmp[2], 0, 2, 0, 1);
			r_primitive_line_3d(tmp[0], tmp[1], tmp[2] + 1, tmp[0], tmp[1], tmp[2] - 1, 0, 0, 3, 1);
			r_primitive_line_flush();	
		}
	}*/						

	c_editor_constraint(input, session, entity,  matrix, 0, 0, entity->component_selected, 0, timer);
	if(input->mode == BAM_DRAW)
		r_matrix_pop(NULL);
	c_editor_bar_rename(input, NULL);

}