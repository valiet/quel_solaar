#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "enough.h"
#include "s_draw_3d.h"
#include "c_internal.h"
#include "testify.h"
#include "gather.h"

void c_editor_bar(BInputState *input, CSession *session, CEntity *entity, RMatrix *matrix);
void c_editor_popup(BInputState *input, CSession *session, CEntity **entity);
void c_editor_stack(BInputState *input, CSession *session, CEntity *entity);
void c_editor_component(BInputState *input, CSession *session, CEntity **entity, CEntity *draw_entity);
uint c_editor_stack_particle_draw(BInputState *input, CSession *session, CEntity *entity);
void c_editor_primitive(BInputState *input);
void c_editor_texture_select(BInputState *input);
CSession *confuse_session_create();
boolean confuse_session_save(CSession *session, void *handle);
boolean confuse_session_load(CSession *session, void *handle);

extern void seduce_element_make_sure();

extern void c_quaternion_to_matrix(float *matrix, float x, float y, float z, float w);
extern void c_matrix_to_quaternion(float *quaternion, float *matrix);
extern void	confuse_view_matrix_set(float *matrix);
extern void confuse_primitive_init_hack(CSession *session);
extern void c_editor_verse_convert(CSession *session, uint component);
extern void c_editor_verse_convert_test(CSession *session);
extern void confuse_relinquish_draw(CSession *session, float delta_time);

extern void seduce_betray_settings(BInputState *input, boolean active);
extern void confuse_point_editor(BInputState *input, CEntity *entity, RMatrix *interface_matrix, RMatrix *camera_matrix);
extern void confuse_editor_preview_draw_flush();

extern void seduce_object_3d_draw(BInputState *input, float pos_x, float pos_y, float pos_z, float scale, uint id, float fade,  float *color);

void confuse_editor_handler(BInputState *input, void *user_pointer)
{
	static boolean toggle = TRUE, init = FALSE;
	static float scale = 1;
	static CSession *session;
	static CEntity *entity_selected = NULL;
	static CEntity *draw_entity = NULL;
	static int component_id = 0;
	RMatrix	matrix, world;
	uint i, x, y;
	float view[3] = {0.0, 0.0, 1.0}, aspect, origo[3] = {0, 0, 0}, m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};



	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < betray_settings_count(); i++)
		{
			if(BETRAY_ST_SELECT == betray_settings_type(i))
			{
				uint j;
				//betray_settings_name(uint id);
		/*		for(j = 0; j < betray_settings_select_count_get(i); j++)
					if(betray_button_get(-1, BETRAY_BUTTON_F1 + j))
						betray_settings_select_set(i, j);*/
			}
		}
		if(betray_button_get(-1, BETRAY_BUTTON_Q))
			exit(0);
	}

//	if(input->mode != BAM_DRAW)
//		return;

	if(!init)
	{
		THandle *h;
		init = TRUE;
		session = confuse_session_create();
		h = testify_file_load("save_everything.bin");
//		h = testify_file_load("Confuse_export.con");
		if(!confuse_session_load(session, h))
		{
			confuse_primitive_init_hack(session);
			confuse_session_entity_add(session, "my entity");
		/*	h = testify_file_save("save_everything.bin");
			if(h != NULL)
			{
				confuse_session_save(session, h);
				testify_free(h);
			}*/
		}else
			testify_free(h);
		entity_selected = session->entity_defines;

	}

	if(input->mode == BAM_MAIN)
	{	
		e_vc_connection_update(0, 0);
		seduce_view_update(NULL, input->delta_time);
	}
	seduce_view_change_right_button(NULL, input);
	r_matrix_identity(&world); /* clear a matric*/
	aspect = betray_screen_mode_get(&x, &y, NULL); /* get resolution and aspect from Betray */
	betray_view_vantage(view); /* get vantage from Betray*/
	r_matrix_frustum(&world, -0.01 - view[0] * 0.01, 0.01 - view[0] * 0.01, -0.01 * aspect - view[1] * 0.01, 0.01 * aspect - view[1] * 0.01, 0.01 * view[2], 500.0); /* set frustum */
	betray_view_direction(m); 
	r_matrix_matrix_mult(&world, m);
	r_matrix_translate(&world, -view[0], -view[1], -view[2]); /* move back to make xy plane visable*/
	r_matrix_set(&world); /* set this matrix as the active matrix */ 
	r_matrix_push(&world);
	matrix = world;

	r_matrix_translate(&world, 0, 0, 1);
	seduce_view_set(NULL, &world);

	/* Set up OpenGL */
	if(input->mode == BAM_DRAW)
	{
		glClearColor(0.0, 0.0, 0.0, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		r_viewport(0, 0, x, y);
		glEnable(GL_BLEND);
		confuse_relinquish_draw(session, input->delta_time);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		if(toggle)
		{
			for(i = 0; i < 41; i++)
			{
				r_primitive_line_3d((float)i / 20.0 - 1.0, -0.01, -1.0, (float)i / 20.0 - 1.0, -0.01, 1.0, 1, 1, 1, 0.2);
				r_primitive_line_3d(-1.0, -0.01, (float)i / 20.0 - 1.0, 1.0, -0.01, (float)i / 20.0 - 1.0, 1, 1, 1, 0.2);
			}
		}
		c_editor_stack_particle_draw(input, session, entity_selected);
		r_primitive_line_flush();
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		confuse_view_matrix_set(world.matrix[world.current]);
		if(entity_selected != &session->primitive_settings || draw_entity == NULL)		
			draw_entity = entity_selected;
		confuse_execute(session, draw_entity, input->delta_time, betray_button_get(-1, BETRAY_BUTTON_SPACE));
		for(i = 0; i < session->entity_define_count; i++)
			if(&session->entity_defines[i] == entity_selected)
				confuse_execute_display(session, i);
		confuse_editor_preview_draw_flush();
	}
	confuse_point_editor(input, entity_selected, &matrix, &world);
	r_matrix_set(&matrix);
	/* quit if the user presses the Q button */
	if(betray_button_get(-1, BETRAY_BUTTON_Q))
		exit(0);
	if(input->mode == BAM_EVENT)
	{
		if(betray_button_get(-1, BETRAY_BUTTON_P))
		{
			THandle *h;
			h = testify_file_save("save_everything.bin");
			if(h != NULL)
			{
			//	testify_debug_mode_set(h, TRUE, NULL);
				confuse_session_save(session, h);
				testify_free(h);
			}else
				exit(0);

		}
	}
/*	if(input->mode == BAM_DRAW)
	{
		float color[4] = {0.2, 0.6, 1, 1};
		for(i = 0; i < 97; i++)
			seduce_object_3d_draw(input, (float)(i % 11) * 0.1 - 0.5, (float)(i / 11) * 0.1 - 0.5, 0, 0.05, i, 0, color);
	}*/
	c_editor_bar(input, session, entity_selected, &world);
	c_editor_stack(input, session, entity_selected);
	c_editor_component(input, session, &entity_selected, draw_entity);
	c_editor_texture_select(input);
	c_editor_popup(input, session, &entity_selected);



	 c_editor_verse_convert_test(session);
//	if(input->mode == BAM_DRAW)
//		seduce_element_make_sure();
//	seduce_betray_settings(input, TRUE);
	{
		uint i, j;
		for(i = 0; i < betray_settings_count(); i++)
		{
			if(BETRAY_ST_SELECT == betray_settings_type(i))
			{
				//betray_settings_name(uint id);
				for(j = 0; j < betray_settings_select_count_get(i); j++)
					if(betray_button_get(-1, BETRAY_BUTTON_F1 + j))
						betray_settings_select_set(i, j);
			}
		}
	}

	seduce_element_endframe(input, FALSE);
/*	if(input->mode == BAM_EVENT)
	{
		void *id;
		id = seduce_element_pointer_id(input, 0, NULL);
	//	i = seduce_element_pointer(input, id, NULL);
	}
	seduce_background_particle(input);*/
}

extern void seduce_element_test(BInputState *input, void *user_pointer);
extern void sui_3d_make();




void confuse_relinquish_init();



void gen_data()
{
	float f, rgb[3];
	FILE *file;
	uint i;
	file = fopen("jason_output.txt", "w");
	for(i = 0; i < 8; i++)
	{
		f_hsv_to_rgb(rgb, (float)i  / 8.0, 1.0, 1.0);
		f = (float)i * 2.0 * PI / 8.0;
		fprintf(file, "%.03f, ", sin(f));
		fprintf(file, "%.03f, ", cos(f));
		fprintf(file, "0.0, ");
		fprintf(file,  "%.03f, %.03f, %.03f, ", rgb[0], rgb[1], rgb[2]);

		fprintf(file, "%.03f, ", sin(f + 0.5));
		fprintf(file, "%.03f, ", cos(f + 0.5));
		fprintf(file, "0.0, ");
		fprintf(file, "%.03f, %.03f, %.03f, ", rgb[0], rgb[1], rgb[2]);

		fprintf(file, "%.03f, ", sin(f + 0.25) * 0.2);
		fprintf(file, "%.03f, ", cos(f + 0.25) * 0.2);
		fprintf(file, "0.3, ");
		fprintf(file, "%.03f, %.03f, %.03f, \n", rgb[0], rgb[1], rgb[2]);
	}
	fclose(file);

}

extern gen_installation_data();
extern void sui_3d_make();

extern void seduce_element_test(BInputState *input, void *user_pointer);
extern void confuse_editor_preview_draw_init();

int main(int argc, char **argv)
{
	if(!betray_support_context(B_CT_OPENGL))
	{
		printf("s_test.c Requires B_CT_OPENGL to be available, Program exit.\n");
		exit(0);
	}
	sui_3d_make();
/*	gen_data();
	exit(0);*/
	seduce_translate_load("seduce_language_translation.txt");

//	betray_init(B_CT_OPENGL, argc, argv, 0, 0, 4, TRUE, "Confuce FX editor");
	betray_init(B_CT_OPENGL, argc, argv, 1824, 968, 16, FALSE, "Confuce FX editor");

	r_init(betray_gl_proc_address_get());
	seduce_init();
	enough_init();
	confuse_relinquish_init();
	confuse_point_editor_init();
	confuse_editor_preview_draw_init();
	e_vc_connect("localhost", "name", "pass", NULL);
	gather_init();
	betray_action_func_set(confuse_editor_handler, NULL);
//	betray_action_func_set(seduce_element_test, NULL);
	betray_launch_main_loop();
	return TRUE;
}
