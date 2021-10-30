#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "seduce_editor.h"
#include "s_draw_3d.h"

char *widget_name[S_EWT_COUNT] = {"text line draw",
									"text line edit",
									"text password edit",
									"text double edit",
									"text float edit",
									"text int edit",
									"text uint edit",
									"text block draw",
									"text button",
									"text button list",
									"widget icon button",
									"widget icon toggle",
									"widget icon popup",
									"widget slider radial",
									"widget wheel radial",
									"widget select radial",
									"background square",
									"background shape",
									"background image"};


SEditorWidget *s_editor_widgets = NULL;
SEditorWidget *s_editor_selected = NULL; 
uint s_editor_widget_count = 0;
float s_editor_widget_timer = 1.0;

void s_editor_widget_option_add(SEditorWidget *w, SEditorWidgetType type, char *name, float x, float y)
{
}

void s_editor_widget_clear(SEditorWidget *w, SEditorWidgetType type, float x, float y)
{
	char *option = "option", *t;
	uint i, j;
	w->type = type;
	for(i = 0; i < 31 && widget_name[type][0] != 0; i++)
		w->name[i] = widget_name[type][i];
	w->name[i] = 0;
	w->comment[0] = 0;

	for(i = 0; i < SE_POS_COUNT * 2; i += 2)
	{
		w->pos[i] = x + 0.1 * (float)i;
		w->pos[i + 1] = y + 0.1 * (float)i;
	}
	switch(type)
	{
		case S_EWT_TEXT_LINE_DRAW :
		case S_EWT_TEXT_LINE_EDIT :
		case S_EWT_TEXT_PASSWORD_EDIT :
		case S_EWT_TEXT_DOUBLE_EDIT :
		case S_EWT_TEXT_INT_EDIT :
		case S_EWT_TEXT_UINT_EDIT :
		case S_EWT_TEXT_BUTTON :
			w->data.text.size = SEDUCE_T_SIZE;
			w->data.text.spacing = SEDUCE_T_SPACE;
			w->data.text.color[0] = 1.0; 
			w->data.text.color[1] = 1.0; 
			w->data.text.color[2] = 1.0; 
			w->data.text.color[3] = 1.0; 
			w->data.text.color[4] = 0.2; 
			w->data.text.color[5] = 0.6; 
			w->data.text.color[6] = 1.0; 
			w->data.text.color[7] = 1.0; 
		break;
		case S_EWT_WIDGET_ICON_BUTTON :
		case S_EWT_WIDGET_ICON_TOGGLE :
		case S_EWT_WIDGET_ICON_POPUP :
		case S_EWT_WIDGET_SLIDER_RADIAL :
		case S_EWT_WIDGET_WHEEL_RADIAL :
			w->data.icon.icon = SUI_3D_OBJECT_PLAY;
			w->data.icon.scale = 0.05;
			w->data.icon.color_active = FALSE;
			w->data.icon.color[0] = 1.0;
			w->data.icon.color[1] = 1.0;
			w->data.icon.color[2] = 1.0;
			w->data.icon.color[3] = 1.0;
		break;
		case S_EWT_WIDGET_SELECT_RADIAL :
		case S_EWT_TEXT_BUTTON_LIST :
			t = malloc((sizeof *t) * 33 * 32);
			for(i = 0; i < 32; i++)
			{
				w->data.options.array[i] = &t[i * 33];
				for(j = 0; option[j] != 0; j++)
					w->data.options.array[i][j] = option[j];
				w->data.options.array[i][j] = 0;
			}
			w->data.options.array_length = 4;
			w->data.options.scale = 0.05;
		break;
		case S_EWT_BACKGROUND_SQUARE :
			w->pos[2] = w->pos[0] + 0.1;
			w->pos[3] = w->pos[1] + 0.1;
			w->data.window.split = 0.5;
		break;
		case S_EWT_BACKGROUND_SHAPE :
			w->pos[2] = w->pos[0] + 0.1;
			w->pos[3] = w->pos[1];
			w->pos[4] = w->pos[0] + 0.1;
			w->pos[5] = w->pos[1] + 0.1;
			w->pos[6] = w->pos[0];
			w->pos[7] = w->pos[1] + 0.1;
			w->data.surface.normal[0] = 0.0;
			w->data.surface.normal[1] = 0.0;
			w->data.surface.transparancy = 0.5;
		break;
		case S_EWT_BACKGROUND_IMAGE :
			w->pos[2] = w->pos[0] + 0.1;
			w->pos[3] = w->pos[1] + 0.1;
			w->data.image.u_start = 0.0;
			w->data.image.v_start = 0.0;
			w->data.image.u_end = 1.0;
			w->data.image.v_end = 1.0;
		break;
	}
}

uint s_editor_widget_add(SEditorWidgetType type, float x, float y)
{
	if(s_editor_widget_count % 256 == 0)
		s_editor_widgets = malloc((sizeof *s_editor_widgets) * (s_editor_widget_count + 256));
	s_editor_selected = &s_editor_widgets[s_editor_widget_count];
	s_editor_widget_clear(&s_editor_widgets[s_editor_widget_count++], type, x, y);
	return s_editor_widget_count - 1;
}


void s_editor_widget_remove()
{
	*s_editor_selected = s_editor_widgets[--s_editor_widget_count];
	s_editor_selected = NULL;
}

float *s_editor_widget_closest(float pointer_x, float pointer_y)
{
	float vec[2], best = 1000000.0f, f, *found = NULL;
	uint i;
	for(i = 0; i < s_editor_widget_count; i++)
	{
		vec[0] = s_editor_widgets[i].pos[0] - pointer_x;
		vec[1] = s_editor_widgets[i].pos[1] - pointer_y;
		f = vec[0] * vec[0] + vec[1] * vec[1];
		if(f < best)
		{
			best = f;
			found = s_editor_widgets[i].pos;
		}
		switch(s_editor_widgets[i].type)
		{
			case S_EWT_TEXT_LINE_DRAW :
			break;
			case S_EWT_TEXT_LINE_EDIT :
			break;
			case S_EWT_TEXT_PASSWORD_EDIT :
			break;
			case S_EWT_TEXT_DOUBLE_EDIT :
			break;
			case S_EWT_TEXT_INT_EDIT :
			break;
			case S_EWT_TEXT_UINT_EDIT :
			break;
			case S_EWT_TEXT_BLOCK_DRAW :
			break;
			case S_EWT_TEXT_BUTTON :
			break;
			case S_EWT_TEXT_BUTTON_LIST :
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
			break;
			case S_EWT_WIDGET_ICON_TOGGLE :
			break;
			case S_EWT_WIDGET_SLIDER_RADIAL :
			break;
			case S_EWT_WIDGET_WHEEL_RADIAL :
			break;
			case S_EWT_WIDGET_SELECT_RADIAL :
			break;
			case S_EWT_BACKGROUND_SQUARE :
			break;
			case S_EWT_BACKGROUND_SHAPE :
			break;
			case S_EWT_BACKGROUND_IMAGE :
			break;
		}
	}
	return found;
}

void s_editor_widget_pos(float *pos, float *closest)
{
	r_primitive_line_2d(pos[0] + 0.01, pos[1], pos[0] - 0.01, pos[1], 1, 1, 1, 1);
	r_primitive_line_2d(pos[0], pos[1] + 0.01, pos[0], pos[1] - 0.01, 1, 1, 1, 1);
}



void s_editor_widget_draw(BInputState *input, boolean overlay, float timer)
{
	boolean b_value = TRUE;
	float f_value = 0.5;
	static float vec[2], pos[6];
	uint i, j, count;
	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < s_editor_widget_count; i++)
		{
			switch(s_editor_widgets[i].type)
			{
				case S_EWT_BACKGROUND_SQUARE :
				seduce_background_square_draw(input, s_editor_widgets[i].pos, 
												s_editor_widgets[i].pos[0], 
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0], 
												s_editor_widgets[i].pos[3] - s_editor_widgets[i].pos[1], 
												0.5, 0.1, timer);
				break;
				case S_EWT_BACKGROUND_SHAPE :
					
				printf("s_editor_widgets[i].data.surface.transparancy %f\n", s_editor_widgets[i].data.surface.transparancy);
				seduce_background_shape_draw(input, s_editor_widgets[i].pos, 
												s_editor_widgets[i].pos[0],
												s_editor_widgets[i].pos[1],

												s_editor_widgets[i].pos[2],
												s_editor_widgets[i].pos[3],

												s_editor_widgets[i].pos[4],
												s_editor_widgets[i].pos[5], 

												s_editor_widgets[i].pos[6],
												s_editor_widgets[i].pos[7], 
												timer, 
												s_editor_widgets[i].data.surface.normal[0],
												s_editor_widgets[i].data.surface.normal[1], 
												NULL,
												s_editor_widgets[i].data.surface.transparancy);
				printf("s_editor_widgets[i].data.surface.transparancy %f\n", s_editor_widgets[i].data.surface.transparancy);
				break;
				case S_EWT_BACKGROUND_IMAGE :
				seduce_background_image_draw(input, s_editor_widgets[i].pos, 
												s_editor_widgets[i].pos[0], 
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0], 
												s_editor_widgets[i].pos[3] - s_editor_widgets[i].pos[1], 
												s_editor_widgets[i].data.image.u_start,
												s_editor_widgets[i].data.image.v_start,
												s_editor_widgets[i].data.image.u_end,
												s_editor_widgets[i].data.image.v_end,
												timer, NULL,
												-1);
				break;
			}
		}
		seduce_background_polygon_flush(input, NULL, timer);
		for(i = 0; i < s_editor_widget_count; i++)
		{
			switch(s_editor_widgets[i].type)
			{
				case S_EWT_TEXT_LINE_DRAW :
					seduce_text_line_draw(s_editor_widgets[i].pos[0] - seduce_text_line_length(s_editor_widgets[i].data.text.size, s_editor_widgets[i].data.text.spacing, s_editor_widgets[i].name, -1) * s_editor_widgets[i].data.text.center, 
									   s_editor_widgets[i].pos[1], 
									   s_editor_widgets[i].data.text.size, 
									   s_editor_widgets[i].data.text.spacing, 
									   s_editor_widgets[i].name, 
									   s_editor_widgets[i].data.text.color[0], 
									   s_editor_widgets[i].data.text.color[1], 
									   s_editor_widgets[i].data.text.color[2], 
									   s_editor_widgets[i].data.text.color[3], -1);
					break;
				case S_EWT_TEXT_LINE_EDIT :
				case S_EWT_TEXT_PASSWORD_EDIT :
				case S_EWT_TEXT_DOUBLE_EDIT :
				case S_EWT_TEXT_INT_EDIT :
				case S_EWT_TEXT_UINT_EDIT :
					seduce_text_line_draw(s_editor_widgets[i].pos[0], 
									   s_editor_widgets[i].pos[1], 
									   s_editor_widgets[i].data.text.size, 
									   s_editor_widgets[i].data.text.spacing, 
									   s_editor_widgets[i].name, 
									   s_editor_widgets[i].data.text.color[0], 
									   s_editor_widgets[i].data.text.color[1], 
									   s_editor_widgets[i].data.text.color[2], 
									   s_editor_widgets[i].data.text.color[3], -1);

					r_primitive_line_2d(s_editor_widgets[i].pos[0], 
									   s_editor_widgets[i].pos[1],
									   s_editor_widgets[i].pos[2], 
									   s_editor_widgets[i].pos[1], 1, 1, 1, 0.3);
					r_primitive_line_flush();
				break;
				case S_EWT_TEXT_BLOCK_DRAW :
				break;
				case S_EWT_TEXT_BUTTON :
					seduce_text_button(input, NULL, 
										s_editor_widgets[i].pos[0], 
										s_editor_widgets[i].pos[1], 
										s_editor_widgets[i].data.text.center, 
										s_editor_widgets[i].data.text.size, 
										s_editor_widgets[i].data.text.spacing, 
										s_editor_widgets[i].name, 
										s_editor_widgets[i].data.text.color[0], 
										s_editor_widgets[i].data.text.color[1], 
										s_editor_widgets[i].data.text.color[2], 
										s_editor_widgets[i].data.text.color[3], 
										s_editor_widgets[i].data.text.color[4], 
										s_editor_widgets[i].data.text.color[5], 
										s_editor_widgets[i].data.text.color[6],
										s_editor_widgets[i].data.text.color[7]);

				break;
				case S_EWT_TEXT_BUTTON_LIST :
				break;
				case S_EWT_WIDGET_ICON_BUTTON :
				case S_EWT_WIDGET_ICON_POPUP :
				seduce_widget_icon_button(input, NULL, 
								s_editor_widgets[i].data.icon.icon,
								s_editor_widgets[i].pos[0],
								s_editor_widgets[i].pos[1], 
								s_editor_widgets[i].data.icon.scale, timer, s_editor_widgets[i].data.icon.color);
				break;
				case S_EWT_WIDGET_ICON_TOGGLE :
				seduce_widget_icon_toggle(input, NULL, NULL, 
								s_editor_widgets[i].data.icon.icon,
								s_editor_widgets[i].pos[0],
								s_editor_widgets[i].pos[1],
								s_editor_widgets[i].data.icon.scale, timer);
				break;
				case S_EWT_WIDGET_SLIDER_RADIAL :
				seduce_widget_slider_radial(input,
								&f_value, 
								&f_value,
								s_editor_widgets[i].pos[0], 
								s_editor_widgets[i].pos[1], 
								s_editor_widgets[i].data.icon.scale, 
								0.5, 
								0.0,
								1.0,
								timer,
								NULL);

				break;
				case S_EWT_WIDGET_WHEEL_RADIAL :
					seduce_widget_wheel_radial(input, NULL, NULL, s_editor_widgets[i].pos[0], s_editor_widgets[i].pos[1], s_editor_widgets[i].data.icon.scale, 0.5, timer);
				break;
				case S_EWT_WIDGET_SELECT_RADIAL :
					seduce_widget_select_radial(input, NULL, NULL, s_editor_widgets[i].data.options.array, s_editor_widgets[i].data.options.array_length, S_PUT_ANGLE, s_editor_widgets[i].pos[0], s_editor_widgets[i].pos[1], s_editor_widgets[i].data.icon.scale, 0.5, timer, TRUE);
				break;
			}
		}
	}

	if(overlay)
	{
		for(i = 0; i < s_editor_widget_count; i++)
		{

			count = 1;
			if(s_editor_widgets[i].type == S_EWT_TEXT_LINE_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_PASSWORD_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_DOUBLE_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_INT_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_UINT_EDIT)
				count = 2;
			if(s_editor_widgets[i].type == S_EWT_BACKGROUND_SHAPE)
				count = 4;

			if(s_editor_widgets[i].type == S_EWT_BACKGROUND_SQUARE ||
				s_editor_widgets[i].type == S_EWT_BACKGROUND_IMAGE)
			{
				pos[0] = s_editor_widgets[i].pos[0];
				pos[1] = s_editor_widgets[i].pos[1];
				pos[2] = 0;
				pos[3] = s_editor_widgets[i].pos[2];
				pos[4] = s_editor_widgets[i].pos[3];
				pos[5] = 0;
				seduce_manipulator_square_cornered(input, NULL, 
					pos, &pos[3], 
					s_editor_widgets[i].pos, NULL, FALSE, FALSE, 0.3, timer);
				s_editor_widgets[i].pos[0] = pos[0];
				s_editor_widgets[i].pos[1] = pos[1];
				s_editor_widgets[i].pos[2] = pos[3];
				s_editor_widgets[i].pos[3] = pos[4];
				count = 0;
			}
			pos[0] = pos[1] = pos[2] = 0;
			for(j = 0; j < 4; j++)
			{
				pos[0] += s_editor_widgets[i].pos[j * 2 + 0] * 0.25;
				pos[1] += s_editor_widgets[i].pos[j * 2 + 1] * 0.25;
			}
			if(seduce_manipulator_point_plane(input, NULL, pos, s_editor_widgets[i].pos, NULL, FALSE, 2, 0.3))
			{
				pos[0] = f_snapf(pos[0], 0.02);
				pos[1] = f_snapf(pos[1], 0.02);
				for(j = 0; j < 4; j++)
				{
					pos[0] -= s_editor_widgets[i].pos[j * 2 + 0] * 0.25;
					pos[1] -= s_editor_widgets[i].pos[j * 2 + 1] * 0.25;
				}
				for(j = 0; j < 4; j++)
				{
					s_editor_widgets[i].pos[j * 2 + 0] += pos[0];
					s_editor_widgets[i].pos[j * 2 + 1] += pos[1];
				}
			}
			for(j = 0; j < count; j++)
			{
				pos[0] = s_editor_widgets[i].pos[j * 2 + 0];
				pos[1] = s_editor_widgets[i].pos[j * 2 + 1];
				pos[2] = 0;
				if(seduce_manipulator_point_plane(input, NULL, pos, &s_editor_widgets[i].pos[j + 1], NULL, FALSE, 2, 0.3))
				{
					s_editor_selected = &s_editor_widgets[i];
					s_editor_widgets[i].pos[j * 2 + 0] = pos[0];
					s_editor_widgets[i].pos[j * 2 + 1] = pos[1];
				}
			}
		/*	if(input->)
			for(j = 0; j < count; j++)
			{
				pos[0] = s_editor_widgets[i].pos[j * 2 + 0];
				pos[1] = s_editor_widgets[i].pos[j * 2 + 1];
				pos[2] = 0;
				if(seduce_manipulator_point_plane(input, NULL, pos, &s_editor_widgets[i].pos[j], NULL, FALSE, 2, 0.3))
					s_editor_selected = &s_editor_widgets[i];
				s_editor_widgets[i].pos[j * 2 + 0] = pos[0];
				s_editor_widgets[i].pos[j * 2 + 1] = pos[1];
			}*/

			if(s_editor_widgets[i].type == S_EWT_TEXT_LINE_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_PASSWORD_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_DOUBLE_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_INT_EDIT ||
				s_editor_widgets[i].type == S_EWT_TEXT_UINT_EDIT)
					s_editor_widgets[i].pos[3] = s_editor_widgets[i].pos[1];


			for(j = 0; j < 8; j++)
				s_editor_widgets[i].pos[j] = f_snapf(s_editor_widgets[i].pos[j], 0.02);
		}
	}
	if(overlay && input->mode == BAM_DRAW)
		for(i = 0; i < s_editor_widget_count; i++)
			seduce_text_line_draw(s_editor_widgets[i].pos[0], 
							   s_editor_widgets[i].pos[1], 
							   SEDUCE_T_SIZE, 
							   SEDUCE_T_SPACE, 
							   s_editor_widgets[i].name, 
							   1, 1, 1, 1, -1);
}

void s_editor_context_update(void)
{
	glMatrixMode(GL_MODELVIEW);	
	glClearColor(0.2, 0.2, 0.2, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
}

void s_editor_popup_handler(BInputState *input)
{
	static float pos_x = 0.5, pos_y = 0, timer = 0;
	SUIPUElement element[12];
	uint output;

	if(input->pointers[0].button[1] && !input->pointers[0].last_button[1])
	{
		pos_x = input->pointers[0].pointer_x;		
		pos_y = input->pointers[0].pointer_y;
	}
	output = seduce_popup_simple(input, 0, pos_x, pos_y, widget_name, S_EWT_COUNT, &timer, input->pointers[0].button[1], 0.9, 0.0, 0.4, 0.9, 0.0, 0.4);

	if(!input->pointers[0].button[1] && input->pointers[0].last_button[1] && output < S_EWT_COUNT)
		s_editor_widget_add(output, pos_x, pos_y);
}

void s_background_arc_draw(float center_x, float center_y, float expand_x, float expand_y, float color_r, float color_g, float color_b, float alpha);

void s_editor_menu(BInputState *input)
{
	static boolean export_save;  
	float timer = 1.0;
	static float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, -0.5, 0, 1}, anim = 10.0;
	RMatrix *matrix;
	static boolean ids[7] = {TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE};
	char *path;

		/* Backgrounds */

	matrix = r_matrix_get();
	r_matrix_push(matrix);
	r_matrix_matrix_mult(matrix, m);
	seduce_background_shape_draw(input, m, -0.240, -0.060, 0.240, -0.060, 0.200, 0.000, -0.200, 0.000, timer, 0.000, -0.0400, NULL, 0.5);
	seduce_background_shape_draw(input, m, -0.220, 0.000, 0.220, 0.000, 0.240, 0.040, -0.240, 0.040, timer, 0.000, 0.0400, NULL, 0.5);
	seduce_background_polygon_flush(input, NULL, timer);
	/* Text */

	if(input->mode == BAM_DRAW)
	{
		seduce_text_line_draw(-0.193, -0.040, 0.007, 0.200, "Add", 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(-0.076, -0.040, 0.007, 0.200, "Save", 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(-0.015, -0.040, 0.007, 0.200, "Load", 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(0.038, -0.040, 0.007, 0.200, "Export", 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(0.094, -0.040, 0.007, 0.200, "Settings", 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(0.167, -0.040, 0.007, 0.200, "Play", 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(-0.141, -0.040, 0.007, 0.200, "Delete", 1.000, 1.000, 1.000, 1.000, -1);
	}

		/* Widgets */


	if(seduce_widget_icon_button(input, &ids[0], 38, 0.180, 0.000, 0.031, timer, NULL))
	{
		anim = 0;
	}


	if(seduce_widget_icon_button(input, &ids[1], 28, -0.120, 0.000, 0.031, timer, NULL))
	{
	}


	if(seduce_widget_icon_button(input, &ids[2], 49, -0.060, 0.000, 0.031, timer, NULL))
	{
		betray_requester_save("*.bin", 1, NULL); /* lounge a requester to save something */
		export_save = FALSE;
	}


	if(seduce_widget_icon_button(input, &ids[3], 50, 0.000, 0.000, 0.031, timer, NULL))
	{
		betray_requester_load(&"*.bin", 1, NULL); /* lounge arequester to load something*/
	}


	if(seduce_widget_icon_button(input, &ids[4], 49, 0.060, 0.000, 0.031, timer, NULL))
	{
		betray_requester_save(&"*.bin", 1, NULL); /* lounge a requester to save something */
		export_save = TRUE;
	}


	if(seduce_widget_icon_button(input, &ids[5], 33, 0.120, 0.000, 0.031, timer, NULL))
	{
	}


	if(seduce_widget_icon_button(input, &ids[6], 22, -0.180, 0.000, 0.031, timer, NULL))
	{

	}
	if(input->mode == BAM_MAIN)
	{
		anim += input->delta_time * 2.0;
		if(anim < 1.0)
			s_editor_widget_timer = 1.0 - anim;
		else if(anim < 2.0)
			s_editor_widget_timer = 0.0;
		else if(anim < 3.0)
			s_editor_widget_timer = anim - 2.0;
		else
			s_editor_widget_timer = 1.0;
	}

	path = betray_requester_save_get(NULL);
	if(path != NULL)
	{
		if(export_save)
		{
			FILE *f;
			if((f = fopen(path, "w")) != NULL)
			{
				seduce_editor_save_function(f, "test_function");
				fclose(f);
			}
		}else
			seduce_editor_save_data(path);
	}

	path = betray_requester_load_get(NULL);
	if(path != NULL)
		seduce_editor_load_data(path);





	seduce_background_shape_matrix_interact(input, m, m, TRUE, TRUE);
	r_matrix_pop(matrix);
}

void seduce_color_settings(BInputState *input, boolean active);
void seduce_editor_panel_test(BInputState *input);

void s_editor_input_handler(BInputState *input, void *user_pointer)
{
	RMatrix matrix, color[4] = {1, 1, 1, 1}, *closest;
	static float view[3] = {0, 0, 1},  m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, a[3] = {-1, 1, 0}, b[3] = {1, 1, 0}, c[3] = {1, -1, 0}, d[3] = {-1, -1, 0};
	FILE *f;
	uint x, y, i;
	float aspect, pos;

	aspect = betray_screen_mode_get(&x, &y, NULL);
	if(input->mode == BAM_DRAW)
	{
		glClearColor(0.2, 0.2, 0.2, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		r_viewport(0, 0, x, y);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

	}
	r_matrix_identity(&matrix);
	betray_view_vantage(view);
	r_matrix_frustum(&matrix, -0.01 - view[0] * 0.01, 0.01 - view[0] * 0.01, -0.01 * aspect - view[1] * 0.01, 0.01 * aspect - view[1] * 0.01, 0.01 * view[2], 100.0); /* set frustum */
	r_matrix_translate(&matrix, -view[0], -view[1], -view[2]);
	r_matrix_set(&matrix);
	
	r_matrix_push(&matrix);
	r_matrix_matrix_mult(&matrix, m);
	if(input->mode == BAM_DRAW && !input->pointers[0].button[2])
	{
		glDepthMask(FALSE);
		i = 0;
		for(pos = -1.0; pos < 1.01; pos += 0.02)
		{
			if(i % 4 == 0)
			{
				r_primitive_line_2d(pos, 1, pos, -1, 0.0, 0.0, 0.0, 0.15);
				r_primitive_line_2d(1, pos, -1, pos, 0.0, 0.0, 0.0, 0.15);
			}else
			{
				r_primitive_line_2d(pos, 1, pos, -1, 0, 0, 0, 0.05);
				r_primitive_line_2d(1, pos, -1, pos, 0, 0, 0, 0.05);
			}
			i++;
		}
		r_primitive_line_2d(0, 1, 0, -1, 1, 1, 1, 0.15);
		r_primitive_line_2d(1, 0, -1, 0, 1, 1, 1, 0.15);
		r_primitive_line_flush();
		glDepthMask(TRUE);
		seduce_element_add_quad(input, m, 0, a, b, c, d);
	}

	s_editor_widget_draw(input, !input->pointers[0].button[2], s_editor_widget_timer);
	
	r_matrix_pop(&matrix);
//	seduce_editor_panel_test(input);

//	seduce_color_settings_panel(input, TRUE);
	seduce_editor_panel_draw(input, s_editor_selected);

	s_editor_menu(input);

	if(s_editor_selected != NULL)
		if(input->mode == BAM_EVENT)
			if(betray_button_get(-1, BETRAY_BUTTON_DELETE))
				s_editor_widget_remove();
	
	if(!input->pointers[0].button[2] && input->pointers[0].last_button[2])
	{
		if((f = fopen("output.txt", "w")) != NULL)
		{
			seduce_editor_save_function(f, "test_function");
			fclose(f);
		}
	}
	s_editor_popup_handler(input);
	seduce_element_draw(input);
	seduce_background_shape_matrix_interact(input, m, m, TRUE, TRUE);
	if(input->mode == BAM_EVENT)
	{
		void *id;
		id = seduce_element_pointer_id(input, 0, NULL);
		seduce_element_pointer(input, id, NULL);
	}
}

extern void seduce_element_test(BInputState *input, void *user_pointer);

extern void sui_3d_make(/*char *name*/);

int main(int argc, char **argv)
{
	float f;
	int16 *buffer;
	uint i;
	char *clip_buffer;

	if(!betray_support_context(B_CT_OPENGL))
	{
		printf("s_test.c Requires B_CT_OPENGL to be available, Program exit.\n");
		exit(0);
	}
//	
	seduce_translate_load("seduce_language_translation.txt");
	/* initialize betray by opening a screen */
//	betray_init(B_CT_OPENGL, argc, argv, 0, 0, 16, TRUE, "Seduce Editor");
	betray_init(B_CT_OPENGL, argc, argv, 1200, 800, 16, FALSE, "Seduce Editor");

//	sui_3d_make();
	r_init(betray_gl_proc_address_get());
	seduce_init();

//	s_editor_widget_add(S_EWT_TEXT);

	betray_gl_context_update_func_set(s_editor_context_update);
	betray_action_func_set(s_editor_input_handler, NULL);
//	betray_action_func_set(seduce_element_test, NULL);
	betray_launch_main_loop();
	return TRUE;
}
