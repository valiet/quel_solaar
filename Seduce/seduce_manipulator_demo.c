#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "s_draw_3d.h"

typedef enum{
	SMT_POINT,
	SMT_POS_XYZ,
	SMT_POINT_PLANE,
	SMT_POINT_AXIS,
	SMT_POINT_VECTOR,
	SMT_NORMAL,
	SMT_RADIUS,
	SMT_SCALE,
	SMT_SQUARE_CENTERED,
	SMT_SQUARE_CORNERED,
	SMT_CUBE_CENTERED,
	SMT_CUBE_CORNERED,
	SMT_ROTATE,
	SMT_COUNT
}SeduceManipulatorType;

extern void seduce_widget_list_element_test(BInputState *input);
extern void seduce_background_particle(BInputState *input);

void seduce_element_make_sure();

boolean	seduce_manipulator_cube_draw(BInputState *input, float pos_a_x, float pos_a_y, float pos_a_z, float pos_b_x, float pos_b_y, float pos_b_z, float time)
{
	if(input->mode == BAM_DRAW)
	{
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		r_primitive_line_fade_3d(pos_a_x, pos_a_y, pos_a_z, pos_b_x, pos_a_y, pos_a_z, 0, 0, 0, time, 1, 0, 0, time);
		r_primitive_line_fade_3d(pos_a_x, pos_b_y, pos_a_z, pos_b_x, pos_b_y, pos_a_z, 0, 1, 0, time, 1, 1, 0, time);
		r_primitive_line_fade_3d(pos_a_x, pos_b_y, pos_b_z, pos_b_x, pos_b_y, pos_b_z, 0, 1, 1, time, 1, 1, 1, time);
		r_primitive_line_fade_3d(pos_a_x, pos_a_y, pos_b_z, pos_b_x, pos_a_y, pos_b_z, 0, 0, 1, time, 1, 0, 1, time);

		r_primitive_line_fade_3d(pos_a_x, pos_a_y, pos_a_z, pos_a_x, pos_b_y, pos_a_z, 0, 0, 0, time, 0, 1, 0, time);
		r_primitive_line_fade_3d(pos_b_x, pos_a_y, pos_a_z, pos_b_x, pos_b_y, pos_a_z, 1, 0, 0, time, 1, 1, 0, time);
		r_primitive_line_fade_3d(pos_b_x, pos_a_y, pos_b_z, pos_b_x, pos_b_y, pos_b_z, 1, 0, 1, time, 1, 1, 1, time);
		r_primitive_line_fade_3d(pos_a_x, pos_a_y, pos_b_z, pos_a_x, pos_b_y, pos_b_z, 0, 0, 1, time, 0, 1, 1, time);

		r_primitive_line_fade_3d(pos_a_x, pos_a_y, pos_a_z, pos_a_x, pos_a_y, pos_b_z, 0, 0, 0, time, 0, 0, 1, time);
		r_primitive_line_fade_3d(pos_b_x, pos_a_y, pos_a_z, pos_b_x, pos_a_y, pos_b_z, 1, 0, 0, time, 1, 0, 1, time);
		r_primitive_line_fade_3d(pos_b_x, pos_b_y, pos_a_z, pos_b_x, pos_b_y, pos_b_z, 1, 1, 0, time, 1, 1, 1, time);
		r_primitive_line_fade_3d(pos_a_x, pos_b_y, pos_a_z, pos_a_x, pos_b_y, pos_b_z, 0, 1, 0, time, 0, 1, 1, time);
		r_primitive_line_flush();
	}
}

void seduce_manipulator_demo_handler(BInputState *input, void *user_pointer)
{
	static float time[SMT_COUNT] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, scale = 1.0;
	static uint selected = SMT_POINT;
	static boolean toggle = TRUE;
	RMatrix	matrix;
	uint i, x, y;
	float view[3] = {0.0, 0.0, 1}, aspect, origo[3] = {0, 0, 0};
	char *lables[SMT_COUNT] = {"POINT",
								"POS XYZ",
								"POINT PLANE",
								"POINT AXIS",
								"POINT VECTOR",
								"NORMAL",
								"RADIUS",
								"SCALE",
								"SQUARE CENTERED",
								"SQUARE CORNERED",
								"CUBE CENTERED",
								"CUBE CORNERED",
								"ROTATE"};


	r_matrix_identity(&matrix); /* clear a matric*/
	aspect = betray_screen_mode_get(&x, &y, NULL); /* get resolution and aspect from Betray */
	betray_view_vantage(view); /* get vantage from Betray*/
	r_matrix_frustum(&matrix, -0.01 - view[0] * 0.01, 0.01 - view[0] * 0.01, -0.01 * aspect - view[1] * 0.01, 0.01 * aspect - view[1] * 0.01, 0.01 * view[2], 100.0); /* set frustum */
	r_matrix_translate(&matrix, -view[0], -view[1], -view[2]); /* move back to make xy plane visable*/
	r_matrix_set(&matrix); /* set this matrix as the active matrix */ 
	r_matrix_push(&matrix);
	r_matrix_rotate(&matrix, 30.0, 1, 0.1, 0);
	r_matrix_rotate(&matrix, input->minute_time * 360.0, 0, 1, 0);


	/* Set up OpenGL */
	if(input->mode == BAM_DRAW)
	{
		glClearColor(0.0, 0.0, 0.0, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		r_viewport(0, 0, x, y);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if(toggle)
		{
			for(i = 0; i < 41; i++)
			{
				r_primitive_line_3d((float)i / 20.0 - 1.0, -0.01, -1.0, (float)i / 20.0 - 1.0, -0.01, 1.0, 1, 1, 1, 0.1);
				r_primitive_line_3d(-1.0, -0.01, (float)i / 20.0 - 1.0, 1.0, -0.01, (float)i / 20.0 - 1.0, 1, 1, 1, 0.1);
			}
		}	
		r_primitive_line_flush();
	}
	/* quit if the user presses the Q button */
	if(betray_button_get(0, BETRAY_BUTTON_Q))
		exit(0);

	/* Animate to the selected manipulator */
	if(input->mode == BAM_MAIN)
	{
		for(i = 0; i < SMT_COUNT; i++)
		{
			if(selected == i)
			{
				time[i] += input->delta_time * 2.0;
				if(time[i] > 1.0)
					time[i] = 1.0;
			}else
			{
				time[i] -= input->delta_time * 2.0;
				if(time[i] < 0.0)
					time[i] = 0.0;
			}
		}
	}
	if(time[SMT_POINT] > 0.01)
	{
		static float pos[3] = {0, 0, 0}; /* a vector of floats to move */
		seduce_manipulator_point(input, &matrix, pos, pos, scale); /* draw and manage point manipulator */
		seduce_manipulator_cube_draw(input, 0, 0, 0, pos[0], pos[1], pos[2], time[SMT_POINT]);
	}

	if(time[SMT_POS_XYZ] > 0.01)
	{
		static float pos[3] = {0, 0, 0}; /* a vector of floats to move */
		seduce_manipulator_pos_xyz(input, &matrix, pos, pos, NULL, FALSE, TRUE, TRUE, TRUE, scale, time[SMT_POS_XYZ]); /* draw and manage pos xyz manipulator */
		seduce_manipulator_cube_draw(input, 0, 0, 0, pos[0], pos[1], pos[2], time[SMT_POS_XYZ]);
	}
	
	if(time[SMT_POINT_PLANE] > 0.01)
	{
		static float pos[3] = {0, 0, 0}; /* a vector of floats to move */
		seduce_manipulator_point_plane(input, &matrix, pos, pos, NULL, FALSE, 0, scale); /* draw and manage pos plane manipulator (In the X (0) Axis) */
		seduce_manipulator_cube_draw(input, -0.01, 0, 0, 0.01, pos[1], pos[2], time[SMT_POINT_PLANE]);
	}	

	if(time[SMT_POINT_AXIS] > 0.01)
	{
		static float pos[3] = {0, 0, 0}; /* a vector of floats to move */
		seduce_manipulator_point_axis(input, &matrix, pos, pos, NULL, FALSE, 2, scale); /* draw and manage pos axis manipulator (Around the Z (2) Axis) */
		seduce_manipulator_cube_draw(input, -0.01, -0.01, 0, 0.01, 0.01, pos[2], time[SMT_POINT_AXIS]);
	}
	if(time[SMT_POINT_VECTOR] > 0.01)
	{
		static float pos[3] = {0, 0, 0}; /* a vector of floats to move */
		float vector[3] = {-1, -1, 1}; /* a vector of floats to move */
		seduce_manipulator_point_vector(input, &matrix, pos, pos, NULL, FALSE, vector, scale); /* draw and manage pos axis manipulator (Around the Z (2) Axis) */
		seduce_manipulator_cube_draw(input, 0, 0, 0, pos[0], pos[1], pos[2], time[SMT_POINT_VECTOR]);
	}

	if(time[SMT_NORMAL] > 0.01)
	{
		float pos[3] = {0, 0, 0}; /* placement of the manipulator */
		static float normal[3] = {1, 0, 0}; /* the normal value */
		seduce_manipulator_normal(input, &matrix, pos, normal, normal, scale, time[SMT_NORMAL]); /* draw and manage pos axis manipulator (Around the Z (2) Axis) */
	}

	if(time[SMT_RADIUS] > 0.01)
	{
		float pos[3] = {0, 0, 0}; /* placement of the manipulator */
		static float radius = 0.5; /* the radius value */
		seduce_manipulator_radius(input, &matrix, pos, &radius, &radius, /*scale, */time[SMT_RADIUS]); /* draw and manage pos axis manipulator (Around the Z (2) Axis) */
	}

	if(time[SMT_SCALE] > 0.01)
	{
		float pos[3] = {0.0, 0.0, 0.0}; /* placement of the manipulator */
		static float size[3] = {0.5, 0.5, 0.5}; /* the radius value */
		seduce_manipulator_scale(input, &matrix, pos, size, size, NULL, FALSE, TRUE, TRUE, TRUE, scale, time[SMT_SCALE]); /* draw and manage pos axis manipulator (Around the Z (2) Axis) */
		seduce_manipulator_cube_draw(input, pos[0] - size[0], pos[1] - size[1], pos[2] - size[2], pos[0] + size[0], pos[1] + size[1], pos[2] + size[2], time[SMT_SCALE]);
	}

	if(time[SMT_SQUARE_CENTERED] > 0.01)
	{
		static float pos[3] = {0, 0, 0}; /* placement of the manipulator (3D!!!)*/
		static float size[2] = {0.25, 0.25}; /* size of the plane (2D) */
		seduce_manipulator_square_centered(input, &matrix, pos, size,  pos, NULL, FALSE, FALSE, scale, time[SMT_SQUARE_CENTERED]);

		seduce_manipulator_cube_draw(input, pos[0] - size[0], pos[1] - size[1], -0.01, pos[0] + size[0], pos[1] + size[1], 0.01, time[SMT_SQUARE_CENTERED]);
	}

	if(time[SMT_SQUARE_CORNERED] > 0.01)
	{
		static float down_left[3] = {-0.25, -0.25, 0}; /* placement of the manipulator (3D!!!)*/
		static float up_right[3] = {0.25, 0.25, 0}; /* size of the plane (3D!!!) */
		seduce_manipulator_square_cornered(input, &matrix, down_left, up_right,  down_left, NULL, FALSE, FALSE, scale, time[SMT_SQUARE_CORNERED]);
		seduce_manipulator_cube_draw(input, down_left[0], down_left[1], -0.01, up_right[0], up_right[1], 0.01, time[SMT_SQUARE_CORNERED]);
	}

	if(time[SMT_CUBE_CENTERED] > 0.01)
	{
		static float pos[3] = {0, 0, 0}; /* placement of the manipulator (3D!!!)*/
		static float size[3] = {0.25, 0.25, 0.25}; /* size of the plane (2D) */
		seduce_manipulator_cube_centered(input, &matrix, pos, size, pos, NULL, FALSE, scale, time[SMT_CUBE_CENTERED]);
		seduce_manipulator_cube_draw(input, pos[0] - size[0], pos[1] - size[1], pos[2] - size[2], pos[0] + size[0], pos[1] + size[1], pos[2] + size[2], time[SMT_CUBE_CENTERED]);
	}

	if(time[SMT_CUBE_CORNERED] > 0.01)
	{
		static float down_left[3] = {-0.25, -0.25, -0.25}; /* placement of the manipulator (3D!!!)*/
		static float up_right[3] = {0.25, 0.25, 0.25}; /* size of the plane (3D!!!) */
		seduce_manipulator_cube_cornered(input, &matrix, down_left, up_right, down_left, NULL, FALSE, scale, time[SMT_CUBE_CORNERED]);
		seduce_manipulator_cube_draw(input, down_left[0], down_left[1], down_left[2], up_right[0], up_right[1], up_right[2], time[SMT_CUBE_CORNERED]);
	}

	if(time[SMT_ROTATE] > 0.01)
	{
		static float pos[3] = {0, 0, 0};
		static float rotation_matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		seduce_manipulator_rotate(input, &matrix, pos, rotation_matrix, rotation_matrix, NULL, FALSE, scale, time[SMT_ROTATE]);
		r_matrix_push(&matrix);
		r_matrix_matrix_mult(&matrix, rotation_matrix);
		seduce_manipulator_cube_draw(input, -0.25, -0.25, -0.25, 0.25, 0.25, 0.25, time[SMT_ROTATE]);
		r_matrix_pop(&matrix);
	}
		r_matrix_set(&matrix); 
	r_matrix_pop(&matrix);

	if(input->mode == BAM_DRAW)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		seduce_text_line_draw(seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE, "Select", -1) - 0.15, -aspect * 0.5 - 0.05, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "Select", 1, 1, 1, 1, -1);
		seduce_text_line_draw(seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE, "Scale", -1) - 0.05, -aspect * 0.5 - 0.05, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "Scale", 1, 1, 1, 1, -1);
		seduce_text_line_draw(seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE, "Lines", -1) + 0.05, -aspect * 0.5 - 0.05, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "Lines", 1, 1, 1, 1, -1);
		seduce_text_line_draw(seduce_text_line_length(SEDUCE_T_SIZE * -0.5, SEDUCE_T_SPACE, "Exit", -1) + 0.15, -aspect * 0.5 - 0.05, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "Exit", 1, 1, 1, 1, -1);
	}
	seduce_widget_icon_toggle(input, &toggle, &toggle, SUI_3D_OBJECT_HIGHLIGHT, 0.05, -aspect * 0.5, 0.05, 1.0);
	if(seduce_widget_icon_button(input, input, SUI_3D_OBJECT_HIGHLIGHT, 0.15, -aspect * 0.5,  0.02, 1, NULL))
		exit(0);

	scale *= 0.5;
	seduce_widget_slider_radial(input, &scale, &scale, -0.05, -aspect * 0.5, 0.05, 0.5, 0, 1, 1, NULL);
	scale *= 2.0;
	seduce_widget_select_radial(input, &selected, &selected, lables, SMT_COUNT, S_PUT_ANGLE, -0.15, -aspect * 0.5, 0.05, 0.5, 0.5, FALSE);
	seduce_widget_list_element_test(input);
	{
		static float color[3] = {1, 0, 0};
		seduce_widget_wheel_radial(input, color, color, 0, 0, 0.05, 1, 1);
	}

//	seduce_element_make_sure();
//	seduce_element_draw(input);
	seduce_background_particle(input);

	if(input->mode == BAM_EVENT)
	{
		void *id;
		id = seduce_element_pointer_id(input, 0, NULL);
		i = seduce_element_pointer(input, id, NULL);
	}
}

extern void seduce_element_test(BInputState *input, void *user_pointer);
extern void sui_3d_make();

int main(int argc, char **argv)
{
	if(!betray_support_context(B_CT_OPENGL))
	{
		printf("s_test.c Requires B_CT_OPENGL to be available, Program exit.\n");
		exit(0);
	}
	seduce_translate_load("seduce_language_translation.txt");
//	betray_init(B_CT_OPENGL, argc, argv, 0, 0, 16, TRUE, "Seduce Manipulator Demo");
	betray_init(B_CT_OPENGL, argc, argv, 1024, 768, 16, FALSE, "Seduce Manipulator Demo");
	r_init(betray_gl_proc_address_get());
	seduce_init();
	betray_action_func_set(seduce_manipulator_demo_handler, NULL);
//	betray_action_func_set(seduce_element_test, NULL);
	betray_launch_main_loop();
	return TRUE;
}
