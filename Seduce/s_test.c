#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "s_draw_3d.h"

void s_test_context_update(void)
{
	glMatrixMode(GL_MODELVIEW);	
	glClearColor(0.2, 0.2, 0.2, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
}

/*
typedef enum{
	SUI_INT8,
	SUI_UINT8,
	SUI_INT16,
	SUI_UINT16,
	SUI_INT32,
	SUI_UINT32,
	SUI_FLOAT,
	SUI_DOUBLE
}SUIFormats;

extern uint 	r_array_vertex_size(SUIFormats *vertex_format_types, uint *vertex_format_size, uint vertex_format_count);

extern void 	*r_array_allocate(uint bytes, SUIFormats *vertex_format_types, uint *vertex_format_size, uint vertex_format_count);
extern void		r_array_free(void *pool);
extern boolean	r_array_defrag(void *pool);
extern void 	*r_array_section_allocate(void *pool, uint size);
extern void		r_array_section_free(void *pool, void *section);
extern void		r_array_active_vertex_normal_texcoord_color(void *pool);
extern void		r_array_active_vertex_atrib(void *pool);
extern void		r_array_deactivate();
extern void		r_array_section_draw(void *section, uint primitive_type, uint vertex_count);
extern uint		r_array_get_size(void *pool);
extern uint		r_array_get_used(void *pool);
extern uint		r_array_get_left(void *pool);
extern uint		r_array_get_vertex_size(void *pool);
extern uint		r_array_section_get_vertex_count(void *section);
extern uint		r_array_section_get_vertex_start(void *section);
*/


void sui_text_line_draw_sample(float size, float spacing, const char *text, char length);
//boolean sui_type_in(BInputState *input, float pos_x, float pos_y, float length, float size, char *text, uint buffer_size, void (*done_func)(void *user, char *text), void *user, float red, float green, float blue, float alpha);

void text_wrap_func(float pos_x, float pos_y, float *start, float *end, void *user)
{
	BInputState *input;
	float dist, f;
	input = user;
	if(input->pointer_count == 0)
		return;

	if(/*input->pointers[0].pointer_y*/0 > pos_y + 0.2 ||
		/*input->pointers[0].pointer_y*/0 < pos_y - 0.2)
		return;


	f = (pos_y - /*input->pointers[0].pointer_y*/0) / 0.2;
	f = sqrt(1 - f * f) * 0.2;


	if(*start + (*end - *start) * 0.5 < /*input->pointers[0].pointer_x*/0 - pos_x)
		*end = /*input->pointers[0].pointer_x*/0 - pos_x - f;
	else
		*start = /*input->pointers[0].pointer_x*/0 - pos_x + f;


//	*end = input->pointers[0].pointer_x;
	
}

void r_object_color(float col_a_r, float col_a_g, float col_a_b, float col_b_r, float col_b_g, float col_b_b, float reflect_r, float reflect_g, float reflect_b);
extern uint sui_3d_texture_reflection;
extern uint sui_3d_texture_shade;

uint sui_text_block_hit_detection(float pointer_x, float pointer_y, float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint *scroll, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user);
void sui_text_block_selection(BInputState *input, float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint text_start, uint *scroll, uint *selection, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user);


void s_test_input_handler(BInputState *input, void *user_pointer)
{
	static char *text = "The first thing I want to do is\n\nto explore more style weapons like\nthe occupation monolith.\nThe occupation monolith is great because you cant ignore it. Yesturday i added the first version a of a force field attack where the AI simply placses a forcefield next to the settlement, sets it to something nasty, like fire or corrosion, and then just starts to pipe in as much power as they can in to it to grow it. Pretty cool, and forceds the players to take down the settlement doing it. Im also thinking of changing the artilery unit to be something larger, much more rare and something that first needs chargeing and then once fully charges fires relentlesly on all settlements in the hemisphere. Rather then having just one power source it would have several. The idea with there is to have big threatning events that everyone can reccognice, and that forces the players to go out in to the world but do it together. The other tribes could with attacks like these could much more obviusly be on your side, or against you.";
	static float time = 0;

	if(input->mode == BAM_MAIN)
		time += input->delta_time;

	if(input->mode == BAM_DRAW)
	{
		static float time = 0;
		static uint scroll[2] = {0, 0}, color_switch[3] = {0, 0, -1}, pos;
		float color[12] = {0.2, 0.6, 1.0, 1.0, 0.6, 1.0, 0.2, 1.0, 0.2, 0.6, 1.0, 1.0};

		glClearColor(0.2, 0.2, 0.2, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		sui_frustum_set(0, 0, 100.0, 0.001, 1.0, 1.0);
		r_matrix_push(NULL);
		glTranslatef(0, 0, -1);

		{
			double matrix[16];
			uint i;
			glGetDoublev(GL_PROJECTION_MATRIX, matrix);
			for(i = 0; i < 4; i++)
				printf("Matrix P %f %f %f %f\n", matrix[i * 4 + 0], matrix[i * 4 + 1], matrix[i * 4 + 2], matrix[i * 4 + 3]);
			glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
			for(i = 0; i < 4; i++)
				printf("Matrix M %f %f %f %f\n", matrix[i * 4 + 0], matrix[i * 4 + 1], matrix[i * 4 + 2], matrix[i * 4 + 3]);
		}

		time += 0.1;
	//	glRotatef(time, 0, 1, 0);
		glColor4f(1.0, 1.0, 1.0, 1.0 / 8.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		r_matrix_push(NULL);
		glTranslatef(0.5 + 0.1 * sin(time * 0.1), 0.03, 0);
		r_matrix_pop(NULL);

	//	sui_text_block_draw(float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, float *color, uint *color_switch, uint *scroll, void (*func)(float *pos_x, float *pos_y, float *start, float *end, void *user), void *user);
	
	
/*		color_switch[0] = sui_text_block_hit_detection(input->pointers[0].pointer_x, input->pointers[0].pointer_y, -0.3, 0.3, SEDUCE_T_SIZE, SEDUCE_T_SPACE, 0.5, SEDUCE_T_SPACEING, 20, SEDUCE_BS_STRETCH, text, 
														scroll, 
														text_wrap_func, 
														input);
*/

		/*
sui_text_block_selection(input, -0.3, 0.3, SEDUCE_T_SIZE, SEDUCE_T_SPACE, 0.5, SEDUCE_T_SPACEING, 20, SEDUCE_BS_STRETCH, text, 0, scroll, &color_switch[0], text_wrap_func, input);
sui_text_block_selection(input, 0.3, 0.3, SEDUCE_T_SIZE, SEDUCE_T_SPACE, 0.5, SEDUCE_T_SPACEING, 20, SEDUCE_BS_STRETCH, text, pos, scroll, &color_switch[0], text_wrap_func, input);

*/

		{
			char tmp[256];
			sprintf(tmp, "Color %u %u Pos %u", color_switch[0], color_switch[1], pos);
			seduce_text_line_draw(0.4, 0.4, SEDUCE_T_SIZE, SEDUCE_T_SPACE, tmp, 1, 1, 0, 1, -1);
		}


  

		
		pos = seduce_text_block_draw(-0.3, 0.3, SEDUCE_T_SIZE, SEDUCE_T_SPACE, 0.5, SEDUCE_T_SPACEING, 20, SEDUCE_BS_STRETCH, text, 0,
			color, 
			color_switch, 
			scroll, 
			text_wrap_func, 
			input);

		if(text[pos] != 0)
		{
			seduce_text_block_draw(0.3, 0.3, SEDUCE_T_SIZE, SEDUCE_T_SPACE, 0.5, SEDUCE_T_SPACEING, 20, SEDUCE_BS_STRETCH, text, pos,
				color, 
				color_switch, 
				scroll, 
				text_wrap_func, 
				input);
		}
	//	seduce_text_line_draw(0.4, 0.4, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "hello world", 1, 1, 0, 1, -1);
	}

	
	{
		static float pointer2[2] = {0, 0};
		if(input->pointers[0].button[1] && !input->pointers[0].last_button[1])
		{
			pointer2[0] = input->pointers[0].pointer_x;
			pointer2[1] = input->pointers[0].pointer_y;
		}
	//	if(input->mode == BAM_DRAW)
	//		seduce_text_line_draw(pointer2[0], pointer2[1], SEDUCE_T_SIZE, SEDUCE_T_SPACE, "NISSE", 1, 1, 1, 1, -1);
		if(input->mode == BAM_EVENT)
		{
			seduce_view_change(NULL, input);
		}
	}

/*	{
		char *lables[10] = {"popup0", "popup1", "popup2", "popup3", "popup4", "popup5", "popup6", "popup7", "popup8", "popup9"};
		static float time = 0;
		seduce_popup_simple(input, 0, input->pointers[0].click_pointer_x[0], input->pointers[0].click_pointer_y[0], lables, 8, &time, input->pointers[0].button[0], 0.2, 0.6, 1.0, 1.0, 1.0, 1.0);
	}*/
	{
		SUIPUElement element[15];
		static float slider = 0.5, pos_x, pos_y;
		static float time = 0;
		uint i;
		element[0].text = "text0";
		element[0].type = S_PUT_TOP;
		element[1].text = "text1";
		element[1].type = S_PUT_TOP;
		element[2].text = "text2";
		element[2].type = S_PUT_TOP;
		element[3].text = "text3";
		element[3].type = S_PUT_TOP;
		element[4].text = "text4";
		element[4].type = S_PUT_TOP;
		element[5].text = "text5";
		element[5].type = S_PUT_TOP;
		element[6].text = "text6";
		element[6].type = S_PUT_BOTTOM;
		element[7].text = "text7";
		element[7].type = S_PUT_BOTTOM;

		element[8].text = "Angle0";
		element[8].type = S_PUT_ANGLE;
		element[8].data.slider_angle.angle[0] = 45;
		element[8].data.slider_angle.angle[1] = 90;

		element[9].text = "Angle1";
		element[9].type = S_PUT_ANGLE;
		element[9].data.slider_angle.angle[0] = 90;
		element[9].data.slider_angle.angle[1] = 90 + 45;

		element[10].text = "Angle2";
		element[10].type = S_PUT_ANGLE;
		element[10].data.slider_angle.angle[0] = 45 + 180;
		element[10].data.slider_angle.angle[1] = 90 + 180;

		element[11].text = "Slider";
		element[11].type = S_PUT_SLIDER;
		element[11].data.slider_angle.angle[0] = 90 + 180;
		element[11].data.slider_angle.angle[1] = 90 + 45 + 180;
		element[11].data.slider_angle.value = &slider;

		element[12].text = "Image";
		element[12].type = S_PUT_IMAGE;
		element[12].data.image.pos[0] = -0.1;
		element[12].data.image.pos[1] = -0.25;
		element[12].data.image.size[0] = 0.1;
		element[12].data.image.size[1] = 0.05;
		element[12].data.image.texture_id = sui_3d_texture_reflection;

		element[13].text = "button A";
		element[13].type = S_PUT_BUTTON;
		element[13].data.button_pos[0] = -0.2;
		element[13].data.button_pos[1] = -0.15;

		element[14].text = "button B";
		element[14].type = S_PUT_BUTTON;
		element[14].data.button_pos[0] = 0.2;
		element[14].data.button_pos[1] = -0.15;

		if(input->pointers[0].button[2] && !input->pointers[0].last_button[2])
		{
			pos_x = input->pointers[0].pointer_x;
			pos_y = input->pointers[0].pointer_y;
		}
		s_popup(input, pos_x, pos_y, element, 15, &time, input->pointers[0].button[2], 0.2, 0.6, 1.0, 1.0, 1.0, 1.0);


	}

	{
		static boolean toggle = FALSE;
		static float slider[5] = {0.6, 0.1, 0.5, 0.8, 0.3}, timer = 0, size = 0.5, scale = 1.0, time = 1, color[4] = {1.0, 0.2, 0.6, 0.5};
		static uint select = 0;
		static char *options[6] = {"One", "Two", "Three", "Four", "Five", "Six"}, text[64] = {65, 66, 67, 0};

		timer += 0.01;
	//	size = 0.7 + sin(timer) * 0.5;
	//	scale = 1.0 + sin(timer * 0.72) * 0.5;

	//	time = sin(timer) * 0.5 + 0.5;

		s_widget_select(input, 0, SEDUCE_T_SIZE * -3.0 * scale, size, scale, "Select", &select, 6, options, options, time, 1, 1, 1, 1, 1, 1);
	//	s_widget_text(input, 0, SEDUCE_T_SIZE * 18.0 * scale, size, scale, "Text", text, 64, time, 1, 1, 1);
	//	s_widget_color(input, 0, SEDUCE_T_SIZE * -6.0 * scale, size, scale, "Color", color, TRUE, TRUE, time, 1, 1, 1);

		s_widget_color(input, 0, 0, 0.1, 0.1, "Color", color, TRUE, TRUE, time, 1, 1, 1);
	}
	if(input->mode == BAM_DRAW)
	{
		static float time = 0;
		uint i;
		time += 0.8;

		
		glPopMatrix();
		r_matrix_push(NULL, NULL);
		seduce_view_set(NULL, NULL);



	//	r_primitive
	//	
		
		
/*		r_primitive_image(0, 0, 1.0, 5, 5.0, input->pointers[0].pointer_x, input->pointers[0].pointer_y, 1.0, 1.0, sui_3d_texture_reflection, 1, 1, 1, 1);



		r_primitive_surface(0, 5.05, 1.0, 5, 1.0, 0.2, 0.6, 1.0, 1.0);
	//	r_primitive_surface(0, 0, 1.0, 5, 5.0, 0.2, 0.6, 1.0, 0.6);
		r_primitive_surface(0, -0.25, 1.0, 5, 0.20, 0.2, 0.6, 1.0, 1.0);


		r_primitive_line_3d(0, 0, 0, 1, 0, 0, 0, 0, 0, 1);

		r_primitive_line_3d(1, 1, 0, 0, 1, 0, 0, 0, 0, 1);

		r_primitive_line_2d(1, 0, 0.1, 0.1, 0.2, 0.6, 1.0, 1);
		r_primitive_line_flush();

  
		r_array_deactivate();*/

	//	glScaled(0.2, 0.2, 0.2);

/*

		seduce_text_line_draw(0.02, 0.52, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "hello world", 0, 0, 0, 1, -1);*/
		glPopMatrix();

	}

	if(input->mode == BAM_DRAW)
	{
		r_matrix_push(NULL);
		seduce_view_set(NULL, NULL);
	}
	{
		static float pos[3] = {0.2, 0.2, 0.2}, radius = 2.3, normal[3] = {1.0, 0.02, 0.0}, matrix[16], scale, time = 0, pos_b[3] = {1, 1, 1}, value = 0.5, up[3] = {1, 1, 1}, size[2] = {0.8, 0.4};
		time += 0.001;
		scale = 1.0 + sin(time) * 0.0;
		value = 0.5 + sin(time * 0.9) * 0.4;
		seduce_manipulator_position(input, NULL, pos, pos, NULL, FALSE, scale, 0, 0, 0, 0, 0, 0);
		seduce_manipulator_point(input, NULL, pos, pos, scale);

		seduce_manipulator_normal(input, NULL, pos, normal, normal, NULL, FALSE, scale, 0, 0, 0, 0, 0, 0);
		seduce_manipulator_radius(input, NULL, pos, &radius, &radius, scale, 0, 0, 0, 0, 0, 0);
		seduce_manipulator_rotate(input, NULL, pos, matrix, matrix, NULL, FALSE, scale, 0, 0, 0, 0, 0, 0);
		seduce_manipulator_slider(input, NULL, pos, pos_b, &value, &value, scale);
	}


	if(input->mode == BAM_DRAW)
		glPopMatrix();

}



void s_test_input_handler_test(BInputState *input, void *user_pointer)
{
	if(input->mode == BAM_EVENT)
		seduce_view_change(NULL, input);

	if(input->mode == BAM_DRAW)
	{
		glClearColor(0.2, 0.2, 0.2, 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		sui_frustum_set(0, 0, 100.0, 0.001, 1.0, 1.0);
		r_matrix_push(NULL, NULL);
		seduce_view_set(NULL, NULL);
		{
			double matrix[16];
			uint i;
			glGetDoublev(GL_PROJECTION_MATRIX, matrix);
			for(i = 0; i < 4; i++)
				printf("Matrix P %f %f %f %f\n", matrix[i * 4 + 0], matrix[i * 4 + 1], matrix[i * 4 + 2], matrix[i * 4 + 3]);
			glGetDoublev(GL_MODELVIEW_MATRIX, matrix);
			for(i = 0; i < 4; i++)
				printf("Matrix M %f %f %f %f\n", matrix[i * 4 + 0], matrix[i * 4 + 1], matrix[i * 4 + 2], matrix[i * 4 + 3]);
		}
		glPopMatrix();
	}
}

int main_makle();

int main(int argc, char **argv)
{
	float f;
	int16 *buffer;
	uint i;
	char *clip_buffer;
	/* Check if betray implementation supports OpenGL */
	if(!betray_support_context(B_CT_OPENGL))
	{
		printf("s_test.c Requires B_CT_OPENGL to be available, Program exit.\n");
		exit(0);
	}

	for(i = 0; i < 7; i++)
	{
		f = ((float)i + 0.25) / 7.0;
		fprintf(stderr, "sample pos %f, %f\n", sin(f * PI * 2.0), cos(f * PI * 2.0));
	}
	/* initialize betray by opening a screen */
	betray_init(B_CT_OPENGL, argc, argv, 1500, 800, FALSE, 1, "Seduce Test Application");
	seduce_init();

	betray_gl_context_update_func_set(s_test_context_update);
	betray_action_func_set(s_test_input_handler, NULL);

	betray_launch_main_loop();
	return TRUE;
}
