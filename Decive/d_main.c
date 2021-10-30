#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "betray.h"
#include "enough.h"
#include "seduce.h"
#include "s_draw_3d.h"
#include "gather.h"

typedef struct{
	char address[48];
	char name[48];
	char pass[48];
}DLoginLink;

DLoginLink *d_login_list = NULL;
uint d_login_list_length = 0;

void deceve_add_login(char *address, char *name, char *pass)
{
	uint i, j;
	for(i = 0; i < d_login_list_length; i++)
	{
		for(j = 0; d_login_list[i].address[j] == address[j] && address[j] != 0; j++);
		if(d_login_list[i].address[j] == address[j])
			return;
	}
	if(d_login_list_length % 16 == 0)
		d_login_list = malloc((sizeof *d_login_list) * (d_login_list_length + 16));
	for(i = 0; i < 47 && address[i] != 0; i++)
		d_login_list[d_login_list_length].address[i] = address[i];
	d_login_list[d_login_list_length].address[i] = 0;

	for(i = 0; i < 47 && name[i] != 0; i++)
		d_login_list[d_login_list_length].name[i] = name[i];
	d_login_list[d_login_list_length].name[i] = 0;

	for(i = 0; i < 47 && pass[i] != 0; i++)
		d_login_list[d_login_list_length].pass[i] = pass[i];
	d_login_list[d_login_list_length].pass[i] = 0;
	d_login_list_length++;
}

char deceive_select_node[64] = {0};

void deceive_set_arg(int argc, char **argv)
{
	char address[64] = {0}, name[64] = {0}, pass[64] = {0}, node[64] = {0}, *input;
	uint i, j, k;
	if(argc > 1)
	{
		printf("Command line usage:\n");
		printf("%s <address>\n", argv[0]);
		printf("Alt:\n");
		printf("%s <name>:<password>@<address>/<selected_node_name>\n", argv[0]);
		printf("Alt:\n");
		printf("-a <adress>\n");
		printf("-n <name>\n");
		printf("-p <pass>\n");
		printf("-s <selected_node_name>\n");
	}
	for(k = 1; k < argc; k++)
	{
		input = argv[k];
		i = 0;
		if(input[0] == '-' && input[1] == 'a' && input[2] == 0)
		{
			if(++k < argc)
				for(; argv[k][i] != 0; i++)
					address[i] = argv[k][i];
			address[i] = 0;

		}else if(input[0] == '-' && input[1] == 'n' && input[2] == 0)
		{
			if(++k < argc)
				for(; argv[k][i] != 0; i++)
					name[i] = argv[k][i];
			name[i] = 0;
		}else if(input[0] == '-' && input[1] == 'p' && input[2] == 0)
		{
			if(++k < argc)
				for(; argv[k][i] != 0; i++)
					pass[i] = argv[k][i];
			pass[i] = 0;
		}else if(input[0] == '-' && input[1] == 's' && input[2] == 0)
		{
			if(++k < argc)
				for(; argv[k][i] != 0; i++)
					deceive_select_node[i] = argv[k][i];
			deceive_select_node[i] = 0;
		}else
		{
			for(; input[i] != '@' && input[i] != 0; i++);
			if(input[i] == '@')
			{
				for(i = 0; input[i] != '@' && input[i] != ':' && input[i] != 0; i++)
					name[i] = input[i];
				name[i] = 0;
				if(input[i] == ':')
				{
					j = 0;
					for(i++; input[i] != '@'; i++)
						pass[j++] = input[i];
					pass[j] = 0;
				}
				i++;
			}else
				i = 0;
			for(j = 0; input[i] != '/' && input[i] != '\\' && input[i] != 0; i++)
				address[j++] = input[i];
			address[j] = 0;
			j = 0;
			if(input[i] != 0)
				for(i++; input[i] != 0; i++)
					deceive_select_node[j++] = input[i];
			deceive_select_node[j] = 0;
		}
	}
	deceve_add_login(address, name, pass);
	printf("Address %s Name %s Pass %s Node %s\n", address, name, pass, deceive_select_node);
	if(address[0] != 0)
		e_vc_connect(address, name, pass, NULL);
}


void deceive_save_bookmarks(char *file_name)
{
	FILE *bookmarks;
	uint i;
	if(file_name == NULL)
		file_name = "bookmarks.dbm";
	bookmarks = fopen(file_name, "w");
	if(bookmarks == NULL)
		return;
	for(i = 0; i < d_login_list_length; i++)
		fprintf(bookmarks, "%s %s %s\n", 
				d_login_list[i].address,
				d_login_list[i].name,
				d_login_list[i].pass);
	fclose(bookmarks);
}

void deceive_load_bookmarks(char *file_name)
{
	char line[256], address[256], name[256], pass[256];
	FILE *bookmarks;
	if(file_name == NULL)
		file_name = "bookmarks.dbm";
	if((bookmarks = fopen(file_name, "r")) != NULL)
	{
		while((fgets(line, sizeof line, bookmarks)) != NULL)
			if(sscanf(line, "%s %s %s", address, name, pass) == 3)
				deceve_add_login(address, name, pass);
		fclose(bookmarks);
	}
}

ENode *deceive_get_edit_node(void)
{
	ENode *node;
	char *name;
	uint i, j;
	if(deceive_select_node[0] == 0)
		return NULL;
	for(i = 0; i < V_NT_NUM_TYPES; i++)
	{
		for(node = e_ns_get_node_next(0, 0, i); node != NULL; node = e_ns_get_node_next(e_ns_get_node_id(node) + 1, 0, i))
		{
			name = e_ns_get_node_name(node);
			for(j = 0; name[j] == deceive_select_node[j] && deceive_select_node[j] != 0; j++);
			if(name[j] == deceive_select_node[j])
			{
				deceive_select_node[0] = 0;
				return node;
			}
		}
	}
	return NULL;
}

void deceive_draw_symb_close(float pos_x, float pos_y)
{
	static float array[] = {-0.002500, -0.002500, -0.010000, -0.010000, -0.000000, -0.017500, 0.005408, -0.016643, 0.005408, -0.016643, 0.010286, -0.014158, 0.010286, -0.014158, 0.014158, -0.010286, 0.014158, -0.010286, 0.016643, -0.005408, 0.016643, -0.005408, 0.017500, 0.000000, -0.000000, -0.017500, -0.005408, -0.016643, -0.005408, -0.016643, -0.010286, -0.014158, -0.010286, -0.014158, -0.014158, -0.010286, -0.014158, -0.010286, -0.016643, -0.005408, -0.016643, -0.005408, -0.017500, 0.000000, -0.000000, 0.017500, -0.005408, 0.016643, -0.005408, 0.016643, -0.010286, 0.014158, -0.010286, 0.014158, -0.014158, 0.010286, -0.014158, 0.010286, -0.016643, 0.005408, -0.016643, 0.005408, -0.017500, 0.000000, 0.002500, 0.002500, 0.010000, 0.010000, -0.000000, 0.017500, 0.005408, 0.016643, 0.005408, 0.016643, 0.010286, 0.014158, 0.010286, 0.014158, 0.014158, 0.010286, 0.014158, 0.010286, 0.016643, 0.005408, 0.016643, 0.005408, 0.017500, 0.000000, 0.002500, -0.002500, 0.010000, -0.010000, -0.002500, 0.002500, -0.010000, 0.010000};
/*	glPushMatrix();
	glTranslatef(pos_x, pos_y, 0);
	glScalef(0.5, 0.5, 0.5);
	r_gl(GL_LINES, array, 48, 2, 0, 0, 0, 0);
	glPopMatrix();*/
}

void hidden_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, float red, float green, float blue)
{
/*	static uint cursor;
	char zeros[256];
	int i;
	float pos;
	
	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < 255 && text[i] != 0; i++)
			zeros[i] = '0';
		zeros[i] = 0;
		r_text(pos_x, pos_y, size, SEDUCE_T_SPACE, zeros, red, green, blue, 0.5);
	}
	else
	{
		if(input->mouse_button[0] == TRUE && input->last_mouse_button[0] == FALSE)
		{
			if(sui_box_click_test(pos_x, pos_y - size, length, size * 3.0))
			{
				text[0] = 0;
				betray_start_type_in(text, buffer_size, NULL, &cursor, NULL);
			}
		}
	}*/
}

void (*deceive_draw_func)(void *user_pointer) = NULL;
void *deceive_user_pointer = NULL;

void d_draw_login(BInputState *input);
void d_init_master(void);

void deceive_set_intro_draw_func(void (*draw_func)(void *user_pointer), void *user_pointer)
{
	deceive_draw_func = draw_func;
	deceive_user_pointer = user_pointer;
}

void deceive_draw_box(float x_pos, float y_pos, float length, float size)
{
	float y_size;
	x_pos -= size * 0.5;
	length += size;
	y_pos -= size * 0.5;
	y_size = size * 2.5;
	r_primitive_line_2d(x_pos, y_pos, x_pos + length, y_pos, 0.2, 0.6, 1.0, 0.2);
	r_primitive_line_2d(x_pos, y_pos + y_size, x_pos + length, y_pos + y_size, 0.2, 0.6, 1.0, 0.2);
	r_primitive_line_2d(x_pos, y_pos, x_pos, y_pos + y_size, 0.2, 0.6, 1.0, 0.2);
	r_primitive_line_2d(x_pos + length, y_pos, x_pos + length, y_pos + y_size, 0.2, 0.6, 1.0, 0.2);
}

extern void seduce_widget_list_element_test(BInputState *input);
extern void seduce_background_particle(BInputState *input);
extern boolean seduce_background_shape_draw2(BInputState *input, void *id, float a_x, float a_y, float b_x, float b_y, float c_x, float c_y, float d_x, float d_y, float timer, float normal_x, float normal_y, float *center);

void deceive_intro_handler(BInputState *input, void *application_handler_func)
{
	DLoginLink *link = NULL;
	static boolean active = FALSE, init = FALSE;
	static char connect_type_in[48];
	static float spin = 0, panel_spin, time = 0;
	uint i, j;
	if(init == FALSE)
	{
		d_init_master();
		deceive_load_bookmarks(NULL);
		init = TRUE;
	}
	connect_type_in[0] = 0;

	if(e_vc_check_connected() && e_vc_check_accepted_slot(0))
	{
		betray_action_func_set(application_handler_func, NULL);
		return;
	}

	if(input->mode == BAM_MAIN)
	{
		spin += input->delta_time;
		if(active)
		{
			panel_spin += input->delta_time * 4.0;
			if(panel_spin > 1.0)
				panel_spin = 1.0;
		}
		verse_callback_update(3000);
	//	return;
	}

	if(input->mode == BAM_DRAW)
	{
		float view[3] = {0, 0, 1}, matrix[16], aspect;
		double dist;
		uint x, y;
		dist = seduce_view_distance_camera_get(NULL);
		betray_view_vantage(view);
		aspect = betray_screen_mode_get(&x, &y, NULL);
		r_viewport(0, 0, x, y);
		r_matrix_set(NULL);
		r_matrix_identity(NULL);
		r_matrix_frustum(NULL, -0.001 - view[0] * 0.001, 0.001 - view[0] * 0.001, -0.001 * aspect - view[1] * 0.001, 0.001 * aspect - view[1] * 0.001, 0.001 * view[2], 10.0); /* set frustum */
		r_matrix_translate(NULL, -view[0], -view[1], -view[2]);
		betray_view_direction(matrix); 
		r_matrix_matrix_mult(NULL, matrix);
		
		glClearColor(1, 1, 1, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}
	if(input->mode == BAM_EVENT)
		for(i = 0; i < input->pointer_count; i++)
			if((input->pointers[i].button[0] == FALSE && input->pointers[i].last_button[0] == TRUE) || 
				(input->pointers[i].button[1] == FALSE && input->pointers[i].last_button[1] == TRUE) || 
				(input->pointers[i].button[2] == FALSE && input->pointers[i].last_button[2] == TRUE))
			active = TRUE;
	if(deceive_draw_func != NULL)
		deceive_draw_func(deceive_user_pointer);

	seduce_animate(input, &time, active, 1.0);
	if(active)
	{
		
		if(e_vc_check_connected_slot(0))
		{
			static char *cancel;
			cancel = seduce_translate("CANCEL");
			if(input->mode == BAM_DRAW)
			{
				seduce_text_line_draw(-seduce_text_line_length(SEDUCE_T_SIZE, SEDUCE_T_SPACE, seduce_translate("CONNECTING"), -1) - 0.1,  -0.5 * SEDUCE_T_SIZE, SEDUCE_T_SIZE, SEDUCE_T_SPACE, seduce_translate("CONNECTING"), 0, 0, 0, 1.0, -1);
			}
		//	if(seduce_text_button(input, button[0], -0.23, SEDUCE_T_SIZE * -7.0, 0.0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, button[0], 1, 1, 1, 1, 0.2, 0.6, 1.0, 1.0))
			if(seduce_text_button(input, cancel, 0.1, -SEDUCE_T_SIZE, 0, SEDUCE_T_SIZE , SEDUCE_T_SPACE, cancel, 0, 0, 0, 1, 1, 0, 0, 1.0))
				e_vc_disconnect(0);
		}
		else
		{
			float center[3] = {0, 0, 0};
			static char address[128] = {0, 0}, name[64] = {0, 0}, pass[64] = {0, 0}, hidden[64] = {0};
			char *inputs[3];
			char *button[2];

			inputs[0] = seduce_translate("ADDRESS:");
			inputs[1] = seduce_translate("NAME:");
			inputs[2] = seduce_translate("PASSWORD:");
			button[0] = seduce_translate("Connect");
			button[1] = seduce_translate("Clear");

			if(input->mode == BAM_DRAW)
			{
				seduce_background_quad_draw(input, NULL,
										1, SEDUCE_T_SIZE * 6.0, 0.0, 
										1, -SEDUCE_T_SIZE * 8.0, 0.0, 
										-1, -SEDUCE_T_SIZE * 8.0, 0.0, 
										-1, SEDUCE_T_SIZE * 6.0, 0.0, 
										0, 0, 1,
										0.0, 0.0, 0.0, 0.7);
				seduce_background_polygon_flush(input, center, time);
			}


			seduce_text_line_edit(input, address, address, 128, SEDUCE_T_SIZE, SEDUCE_T_SIZE * 2.0, 0.3, SEDUCE_T_SIZE,  NULL, TRUE, NULL, NULL, 1, 1, 1, 1, 0.2, 0.6, 1.0, 1.0);
			seduce_text_line_edit(input, name, name, 64, SEDUCE_T_SIZE, -SEDUCE_T_SIZE * 1.0, 0.3, SEDUCE_T_SIZE, NULL, TRUE, NULL, NULL, 1, 1, 1, 1, 0.2, 0.6, 1.0, 1.0);
			seduce_text_password_edit(input, pass, pass, 64, SEDUCE_T_SIZE, -SEDUCE_T_SIZE * 0.0, 0.3, SEDUCE_T_SIZE, NULL, TRUE, NULL, NULL, 1, 1, 1, 1, 0.2, 0.6, 1.0, 1.0);
			if(input->mode == BAM_DRAW)
			{		
				seduce_text_line_draw(-seduce_text_line_length(SEDUCE_T_SIZE * 0.7, SEDUCE_T_SPACE, inputs[0], -1) - SEDUCE_T_SIZE,  SEDUCE_T_SIZE * 2.15, SEDUCE_T_SIZE * 0.7, SEDUCE_T_SPACE, inputs[0], 1, 1, 1, 0.5, -1);
				seduce_text_line_draw(-seduce_text_line_length(SEDUCE_T_SIZE * 0.7, SEDUCE_T_SPACE, inputs[1], -1) - SEDUCE_T_SIZE, SEDUCE_T_SIZE * -0.85, SEDUCE_T_SIZE * 0.7, SEDUCE_T_SPACE, inputs[1], 1, 1, 1, 0.5, -1);
				seduce_text_line_draw(-seduce_text_line_length(SEDUCE_T_SIZE * 0.7, SEDUCE_T_SPACE, inputs[2], -1) - SEDUCE_T_SIZE, SEDUCE_T_SIZE * -3.85, SEDUCE_T_SIZE * 0.7, SEDUCE_T_SPACE, inputs[2], 1, 1, 1, 0.5, -1);
			}
			if(seduce_text_button(input, button[0], -0.23, SEDUCE_T_SIZE * -7.0, 0.0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, button[0], 1, 1, 1, 1, 0.2, 0.6, 1.0, 1.0))
				e_vc_connect(address, name, pass, NULL);
			if(seduce_text_button(input, button[1], 0.23, SEDUCE_T_SIZE * -7.0, 1.0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, button[1], 0, 0, 0, 1, 10, 0.0, 0.0, 1.0))
			{
				address[0] = 0;
				name[0] = 0;
				pass[0] = 0;
			}
 		}
	}
	if(input->mode == BAM_DRAW)
	{
		glDisable(GL_DEPTH_TEST);
		r_primitive_surface(-1, 0.45, 0, 2, 10, 0, 0, 0, 1.0);
		r_primitive_surface(-1, -10.45, 0, 2, 10, 0, 0, 0, 1.0);
	}
	
	seduce_element_endframe(input, FALSE);
	//	if(active)
//		d_draw_login(input);

}
