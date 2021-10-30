#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "testify.h"
#include "s_draw_3d.h"
#include "c_internal.h"

typedef enum{
	CE_PM_EDIT_PRIMITIVES,
	CE_PM_ADD_COMPONENT,
	CE_PM_SELECT_COMPONENT,
	CE_PM_SELECT_ENTITY,
	CE_PM_ENTITY_CREATE,
	CE_PM_ADD_COMPONENT_TYPE,
	CE_PM_NONE,
	CE_PM_COUNT
}CEPopupMode;

CEPopupMode ce_popup_mode = CE_PM_NONE;


boolean c_editor_popup_copy_to_replace(char *from, char *to, char *remove, char *add)
{
	uint i, j, k, l;
	for(i = 0; i < 1010 && from[i] != 0; i++)
	{
		if(from[i] == remove[0])
		{
			for(j = 0; j + i < 1023 && from[i + j] == remove[j] && remove[j] != 0; j++);
			if(j + i < 1023 && remove[j] == 0)
			{
				for(k = l = 0; k < i; k++)
					to[l++] = from[k];
				for(k = 0; add[k] != 0; k++)
					to[l++] = add[k];
				for(k = 0; from[i + j + k] != 0; k++)
					to[l++] = from[i + j + k];
				to[l] = 0;
				return TRUE;
			}
		}
	}
	return FALSE;
}

static char c_editor_popup_vertex_path[1024] = {0};
static char c_editor_popup_fragment_path[1024] = {0};
uint ce_popup_create_replacement_primitive = -1;

void c_editor_popup_entity_create_replace(uint replaced_primitive, char *vertex_path, char *fragment_path)
{
	uint i;
	ce_popup_mode = CE_PM_ENTITY_CREATE;
	ce_popup_create_replacement_primitive = replaced_primitive; 
	if(vertex_path != NULL)
	{
		for(i = 0; vertex_path[i] != 0; i++)
			c_editor_popup_vertex_path[i] = vertex_path[i];
		c_editor_popup_vertex_path[i] = 0;
	}else
		c_editor_popup_vertex_path[0] = 0;
	if(vertex_path != NULL)
	{
		for(i = 0; fragment_path[i] != 0; i++)
			c_editor_popup_fragment_path[i] = fragment_path[i];
		c_editor_popup_fragment_path[i] = 0;
	}else
		c_editor_popup_fragment_path[0] = 0;
}

void c_editor_popup_entity_create(BInputState *input, CSession *session, float timer, uint replace)
{
	static float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -0.3, 1}, start_pos[2] = {0, 0}, ok_timer = 0, debug_timer = 0, error_color[4] = {0, 0, 0, 1};
	RMatrix *matrix;
	static boolean ids[3] = {TRUE, TRUE, TRUE};
	static char name[1024] = {0}, debug_output[2048] = {0};
	char *ok_text = "OK", *cancel_text ="Cancel";
	static boolean vertex_requester, attrib_toggle = FALSE, uniform_toggle = FALSE;
	char *path;
	static boolean toggle_0 = TRUE, ok_available;
	uint i;
		/* Backgrounds */

	matrix = r_matrix_get();
	r_matrix_push(matrix);
	r_matrix_matrix_mult(matrix, m);

	ok_available = (attrib_toggle || uniform_toggle) && (c_editor_popup_vertex_path[0] != 0 && c_editor_popup_fragment_path[0] != 0);
	seduce_animate(input, &ok_timer, ok_available, 2.0);
	seduce_animate(input, &debug_timer, debug_output[0] != 0, 2.0);
	if(debug_timer > timer)
		debug_timer = timer;
	error_color[3] = debug_timer;


/*	seduce_background_shape_draw(input, m, -0.120, 0.160, 0.120, 0.160, 0.120, 0.280, -0.120, 0.300, timer, 0.000, 0.000, NULL, 0.700);

	seduce_background_shape_draw(input, m, -0.120, 0.120, 0.120, 0.120, 0.120, 0.160, -0.120, 0.160, timer, 0.000, 0.000, NULL, 0.900);
	seduce_background_shape_draw(input, m, -0.120, 0.080, 0.120, 0.080, 0.120, 0.120, -0.120, 0.120, timer, 0.000, 0.000, NULL, 0.700);

	seduce_background_shape_draw(input, m, -0.120, 0.040, 0.120, 0.040, 0.120, 0.080, -0.120, 0.080, timer, 0.000, 0.000, NULL, 0.900);
	seduce_background_shape_draw(input, m, -0.120, 0.000, 0.120, 0.000, 0.120, 0.040, -0.120, 0.040, timer, 0.000, 0.000, NULL, 0.700);
	seduce_background_shape_draw(input, m, -0.120, -0.040, 0.120, -0.040, 0.120, 0.000, -0.120, 0.000, timer, 0.000, 0.000, NULL, 0.900);
	seduce_background_shape_draw(input, m, -0.120, -0.120, 0.120, -0.120, 0.120, -0.040, -0.120, -0.040, timer, 0.000, 0.000, NULL, 0.700);
	seduce_background_shape_draw(input, m, -0.020, -0.140, 0.020, -0.140, 0.120, -0.120, -0.120, -0.120, timer, 0.000, 0.000, NULL, 0.700);
	seduce_background_shape_draw(input, m, -0.020, -0.180, 0.020, -0.180, 0.020, -0.140, -0.020, -0.140, timer, 0.000, 0.000, NULL, 0.700);
	seduce_background_shape_draw(input, m, -0.120, -0.200, 0.120, -0.200, 0.020, -0.180, -0.020, -0.180, timer, 0.000, 0.000, NULL, 0.700);
	seduce_background_shape_draw(input, m, -0.120, -0.240, 0.120, -0.280, 0.120, -0.200, -0.120, -0.200, timer, 0.000, 0.000, NULL, 0.700);
	seduce_background_shape_draw(input, m, 0.020, -0.180, 0.120, -0.200, 0.120, -0.120, 0.020, -0.140, timer, 0.083, 0.000, NULL, 0.700);
	seduce_background_shape_draw(input, m, -0.120, -0.200, -0.020, -0.180, -0.020, -0.140, -0.120, -0.120, timer, -0.083, 0.000, NULL, 0.700);
	*/
	
	seduce_background_quad_draw(input, m,
									-0.120, 0.240, 0, 
									-0.120, 0.155, 0, 
									0.120, 0.155, 0, 
									0.120, 0.240, 0, 
									0, 0, 1,
									0.0, 0.0, 0.0, 0.9);


	seduce_background_quad_draw(input, name, 
									-0.120, 0.135, 0, 
									-0.120, 0.155, 0, 
									0.120, 0.155, 0, 
									0.120, 0.135, 0, 
									0, 0, 1,
									0.2, 0.6, 1.0, 0.9);
	seduce_background_quad_draw(input, m,
									-0.120, 0.135, 0, 
									-0.120, 0.075, 0, 
									0.120, 0.075, 0, 
									0.120, 0.135, 0, 
									0, 0, 1,
									0.0, 0.0, 0.0, 0.9);
	
	seduce_background_quad_draw(input, c_editor_popup_vertex_path,
									-0.120, 0.055, 0, 
									-0.120, 0.075, 0, 
									0.120, 0.075, 0, 
									0.120, 0.055, 0, 
									0, 0, 1,
									0.2, 0.6, 1.0, 0.9);


	seduce_background_quad_draw(input, m,
									-0.120, 0.055, 0, 
									-0.120, -0.005, 0, 
									0.120, -0.005, 0, 
									0.120, 0.055, 0, 
									0, 0, 1,
									0.0, 0.0, 0.0, 0.9);

	seduce_background_quad_draw(input, c_editor_popup_fragment_path,
									-0.120, -0.025, 0, 
									-0.120, -0.005, 0, 
									0.120, -0.005, 0, 
									0.120, -0.025, 0, 
									0, 0, 1,
									0.2, 0.6, 1.0, 0.9);


	seduce_background_quad_draw(input, m,
									-0.120, -0.145, 0, 
									-0.120, -0.025, 0, 
									0.120, -0.025, 0, 
									0.120, -0.145, 0, 
									0, 0, 1,
									0.0, 0.0, 0.0, 0.9);


	seduce_background_quad_draw(input, cancel_text,
									0.120 - 0.130 * ok_timer, -0.145, 0, 
									0.120 - 0.110 * ok_timer, -0.165, 0, 
									-0.120, -0.165, 0, 
									-0.120, -0.145, 0, 
									0, 0, 1,
									1.0, 0.0, 0.2, 0.9);
	seduce_background_quad_draw(input, ok_text,
								0.120 - 0.130 * ok_timer, -0.145, 0, 
								0.120 - 0.110 * ok_timer, -0.165, 0, 
								0.120, -0.165, 0, 
								0.120, -0.145, 0, 
								0, 0, 1,
								0.2, 0.6, 1.0, 0.9);


	
	seduce_background_quad_draw(input, m,
									-0.120, -0.240, 0, 
									-0.120, -0.165, 0, 
									0.120, -0.165, 0, 
									0.120, -0.240, 0, 
									0, 0, 1,
									0.0, 0.0, 0.0, 0.9);
	
	seduce_background_polygon_flush(input, start_pos, timer);
	seduce_background_quad_draw(input, m,
									-0.120, -1.340, 0, 
									-0.120, -0.240, 0, 
									0.120, -0.240, 0, 
									0.120, -1.340, 0, 
									0, 0, 1,
									1.0, 0.0, 0.0, 0.9);
	seduce_background_polygon_flush(input, start_pos, debug_timer);


//	if(seduce_text_button(input, t, -0.090, -0.160, -0.000, 0.007, 0.200, t, 1.000, 1.000, 1.000, 1.000, 0.200, 0.600, 1.000, 1.000))

/*	seduce_background_quad_draw(input, &m[1],
									-0.120, 0.240, 0, 
									-0.120, 0.0, 0, 
									0.120, 0.0, 0, 
									0.120, 0.240, 0, 
									0, 0, 1,
									0.2, 0.6, 1.0, 0.9);*/



	/* name */
//	seduce_text_line_edit(input, name, name, 1024, -0.100, 0.140, 0.160, 0.007, seduce_translate("Name"), TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);

	/* vertex */
//	seduce_text_line_edit(input, c_editor_popup_vertex_path, c_editor_popup_vertex_path, 1024, -0.100, 0.060, 0.160, 0.007, seduce_translate("vertex shader file"), TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);

	/* fragment */
//	seduce_text_line_edit(input, c_editor_popup_fragment_path, c_editor_popup_fragment_path, 1024, -0.100, -0.020, 0.160, 0.007, seduce_translate("Fragment shader file"), TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);
	



		/* Text */

	if(input->mode == BAM_DRAW)
	{
		seduce_text_line_draw(-0.053, 0.220, 0.007, 0.200, seduce_translate("Create Primitive"), 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(-0.040, -0.060, 0.007, 0.200, seduce_translate("Object shader"), 1.000, 1.000, 1.000, 1.000, -1);
		seduce_text_line_draw(-0.040, -0.100, 0.007, 0.200, seduce_translate("Sprite shader"), 1.000, 1.000, 1.000, 1.000, -1);
	}

	/* vertex load*/
	if(seduce_widget_icon_button(input, &ids[0], 88, 0.080, 0.060, 0.0125, timer, NULL))
		betray_requester_load(NULL, 0, &ids[0]);

	/* fragment load*/
	if(seduce_widget_icon_button(input, &ids[2], 88, 0.080, -0.020, 0.0125, timer, NULL))
		betray_requester_load(NULL, 0, &ids[2]);


		
	path = betray_requester_load_get(&ids[0]);
	if(path != NULL)
	{
		for(i = 0; i < 1023 && path[i] != 0; i++)
			c_editor_popup_vertex_path[i] = path[i];
		c_editor_popup_vertex_path[i] = 0;
		if(c_editor_popup_fragment_path[0] == 0)
		{
			if(c_editor_popup_copy_to_replace(c_editor_popup_vertex_path, c_editor_popup_fragment_path, "vertex", "fragment") ||
				c_editor_popup_copy_to_replace(c_editor_popup_vertex_path, c_editor_popup_fragment_path, "Vertex", "Fragment") ||
				c_editor_popup_copy_to_replace(c_editor_popup_vertex_path, c_editor_popup_fragment_path, "VERTEX", "FRAGMENT") ||
				c_editor_popup_copy_to_replace(c_editor_popup_vertex_path, c_editor_popup_fragment_path, "_v.", "_f."))
			{
				FILE *f;
				if((f = fopen(c_editor_popup_fragment_path, "r")) != NULL)
					fclose(f);
				else
					c_editor_popup_fragment_path[0] = 0;
			}
		}
	}
	path = betray_requester_load_get(&ids[2]);
	if(path != NULL)
	{
		for(i = 0; i < 1023 && path[i] != 0; i++)
			c_editor_popup_fragment_path[i] = path[i];
		c_editor_popup_fragment_path[i] = 0;
		if(c_editor_popup_vertex_path[0] == 0)
		{
			if(c_editor_popup_copy_to_replace(c_editor_popup_fragment_path, c_editor_popup_vertex_path, "pixel", "vertex") ||
				c_editor_popup_copy_to_replace(c_editor_popup_fragment_path, c_editor_popup_vertex_path, "Pixel", "Vertex") ||
				c_editor_popup_copy_to_replace(c_editor_popup_fragment_path, c_editor_popup_vertex_path, "PIXEL", "VERTEX") ||
				c_editor_popup_copy_to_replace(c_editor_popup_fragment_path, c_editor_popup_vertex_path, "fragment", "vertex") ||
				c_editor_popup_copy_to_replace(c_editor_popup_fragment_path, c_editor_popup_vertex_path, "Fragment", "Vertex") ||
				c_editor_popup_copy_to_replace(c_editor_popup_fragment_path, c_editor_popup_vertex_path, "FRAGMENT", "VERTEX") ||
				c_editor_popup_copy_to_replace(c_editor_popup_fragment_path, c_editor_popup_vertex_path, "_f.", "_v."))
			{
				FILE *f;
				if((f = fopen(c_editor_popup_vertex_path, "r")) != NULL)
					fclose(f);
				else
					c_editor_popup_vertex_path[0] = 0;
			}
		}
	}


	if(attrib_toggle)
		seduce_widget_icon_toggle(input, &attrib_toggle, &attrib_toggle, SUI_3D_OBJECT_CHECKBOXCHECKED, -0.080, -0.060, 0.0125, timer);
	else
		seduce_widget_icon_toggle(input, &attrib_toggle, &attrib_toggle, SUI_3D_OBJECT_CHECKBOXUNCHECKED, -0.080, -0.060, 0.0125, timer);

	if(uniform_toggle)
		seduce_widget_icon_toggle(input, &uniform_toggle, &uniform_toggle, SUI_3D_OBJECT_CHECKBOXCHECKED, -0.080, -0.100, 0.0125, timer);
	else
		seduce_widget_icon_toggle(input, &uniform_toggle, &uniform_toggle, SUI_3D_OBJECT_CHECKBOXUNCHECKED, -0.080, -0.100, 0.0125, timer);
	
	/* name */
	seduce_text_line_edit(input, name, name, 1024, -0.100, 0.140, 0.160, 0.007, seduce_translate("Name"), TRUE, NULL, NULL,  0.000, 0.000, 0.000, 1.000,  0.000, 0.000, 0.000, 1.000);

	/* vertex */
	seduce_text_line_edit(input, c_editor_popup_vertex_path, c_editor_popup_vertex_path, 1024, -0.100, 0.060, 0.160, 0.007, seduce_translate("vertex shader file"), TRUE, NULL, NULL, 0.000, 0.000, 0.000, 1.000,  0.000, 0.000, 0.000, 1.000);

	/* fragment */
	seduce_text_line_edit(input, c_editor_popup_fragment_path, c_editor_popup_fragment_path, 1024, -0.100, -0.020, 0.160, 0.007, seduce_translate("Fragment shader file"), TRUE, NULL, NULL, 0.000, 0.000, 0.000, 1.000,  0.000, 0.000, 0.000, 1.000);
	
	if(ok_available)
	{
		if(seduce_text_button(input, ok_text, 0.080, -0.160, -0.000, 0.007, 0.200, ok_text, 0.000, 0.000, 0.000, 1.000, 0.200, 0.600, 1.000, 1.000))
		{
			char *f, *v;
			boolean compiled;
			v = c_primitive_text_load(c_editor_popup_vertex_path);
			f = c_primitive_text_load(c_editor_popup_fragment_path);
			compiled = c_primitive_compile_test(v, f, name, debug_output, 2048);
			if(v != NULL)
				free(v);
			if(f != NULL)
				free(f);
			if(compiled)
			{
				if(ce_popup_create_replacement_primitive != -1)
				{
					CDrawState *state;
					state = &session->prim[ce_popup_create_replacement_primitive];
					free(state->shader_paths[0]);
					free(state->shader_paths[1]);
					state->shader_paths[0] = f_text_copy_allocate(c_editor_popup_vertex_path);
					state->shader_paths[1] = f_text_copy_allocate(c_editor_popup_fragment_path);
					c_primitive_reload(session, state);
				}else
					c_primitive_load(session, c_editor_popup_vertex_path, c_editor_popup_fragment_path, attrib_toggle, uniform_toggle, name);
				ce_popup_mode = CE_PM_NONE;
				c_editor_popup_vertex_path[0] = 0;
				c_editor_popup_fragment_path[0] = 0;
				debug_output[0] = 0;
				attrib_toggle = FALSE, 
				uniform_toggle = FALSE;
			}
		}
	}

	if(seduce_text_button(input, cancel_text, -0.090, -0.160, -0.000, 0.007, 0.200, cancel_text, 0.000, 0.000, 0.000, 1.000, 1.200, 0.000, 0.200, 1.000))
	{
		ce_popup_mode = CE_PM_NONE;
		debug_output[0] = 0;
	}
	if(input->mode == BAM_DRAW)
		seduce_text_block_draw(-0.1, -0.26, SEDUCE_T_SIZE, SEDUCE_T_SPACE, 0.2, SEDUCE_T_SPACEING, -1, SEDUCE_BS_LEFT, debug_output, NULL, error_color, NULL, NULL, NULL, NULL);

	seduce_background_shape_matrix_interact(input, m, m, TRUE, TRUE);
	r_matrix_pop(matrix);
}



void c_editor_popup_func(BInputState *input, float time, void *user)
{
	static SUIPUElement element[16];
	uint output, entry = 0;

	element[entry].type = S_PUT_ANGLE;
	element[entry].text = "Edit primitivee";
	element[entry].data.angle[0] = 45;
	element[entry].data.angle[1] = 45 + 90;
	entry++;

	element[entry].type = S_PUT_ANGLE;
	element[entry].text = "Add component";
	element[entry].data.angle[0] = 45 + 270;
	element[entry].data.angle[1] = 45 + 360;
	entry++;

	element[entry].type = S_PUT_ANGLE;
	element[entry].text = "Select entity";
	element[entry].data.angle[0] = 45 + 180;
	element[entry].data.angle[1] = 45 + 270;
	entry++;
	
	element[entry].type = S_PUT_BOTTOM;
	element[entry].text = "Create entity";
	entry++;

	element[entry].type = S_PUT_BOTTOM;
	element[entry].text = "Load";
	entry++;

	element[entry].type = S_PUT_BOTTOM;
	element[entry].text = "Save";
	entry++;
/*
	element[entry].text = "button B";
	element[entry].type = S_PUT_BUTTON;
	element[entry].data.button_pos[0] = 0.2;
	element[entry].data.button_pos[1] = -0.15;
	entry++;*/
	output = seduce_popup(input, element, entry, time);
	switch(output)
	{
		case 0 :
			ce_popup_mode = CE_PM_EDIT_PRIMITIVES;
		break;
		case 1 :
			ce_popup_mode = CE_PM_ADD_COMPONENT;
		break;
		case 2 :
			ce_popup_mode = CE_PM_SELECT_ENTITY;
		break;
		case 3 :
			ce_popup_mode = CE_PM_ENTITY_CREATE;
		break;
		case 4 :	
			betray_requester_load(NULL, 0, &ce_popup_mode);
		break;
		case 5 :
			betray_requester_save(NULL, 0, &ce_popup_mode);
		break;
	}
}

/*
{
	uint *istack;
	istack = (uint*)&stack[type->pos];
	seduce_background_image_draw(input, istack, x - 0.025, y - 0.025, 0.05, 0.05, 0, 0, 1, 1, 1, NULL, *istack);
	
	if(seduce_widget_icon_button_invisible(input, &stack[type->pos], x, y))
	{
		if(expanded == type)
			expanded = NULL;
		else
			expanded = type;
	}
	if(expanded == type)
	{
		char *file;
		file = betray_requester_load_get();
		if(file != NULL)
			c_load_texture(file);
		if(input->mode == BAM_DRAW)
		{
			float x_pos, y_pos;
			x_pos = (float)(0 % 10) * 0.12 - 0.6;
			y_pos = (float)(0 / 10) * 0.12 + y + 0.1;
			r_primitive_line_2d(x_pos, y_pos, x_pos, y_pos + 0.1, 0.2, 0.2, 0.2, time);
			r_primitive_line_2d(x_pos, y_pos + 0.1, x_pos + 0.1, y_pos + 0.1, 0.2, 0.2, 0.2, time);
			r_primitive_line_2d(x_pos + 0.1, y_pos + 0.1, x_pos + 0.1, y_pos, 0.2, 0.2, 0.2, time);
			r_primitive_line_2d(x_pos + 0.1, y_pos, x_pos, y_pos, 0.2, 0.2, 0.2, time);

			r_primitive_line_2d(x_pos, y_pos, x_pos + 0.1, y_pos + 0.1, 0.2, 0.2, 0.2, time);
			r_primitive_line_2d(x_pos, y_pos + 0.1, x_pos + 0.1, y_pos, 0.2, 0.2, 0.2, time);
			r_primitive_line_flush();
		}
		if(seduce_text_button(input, &c_textures[c_texture_count], (float)(0 % 10) * 0.12 - 0.6 + 0.05, (float)(0 / 10) * 0.12 + y + 0.15 - SEDUCE_T_SIZE * 0.5, 0.5, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "Load...", 1, 1, 1, 0.5, 0.2, 0.6, 1, 1))
			betray_requester_load(NULL, 0);
		for(i = 0; i < c_texture_count; i++)
		{
			if(input->mode == BAM_DRAW)
			{
				float aspect;
				aspect = (float)c_textures[i].size[0] / (float)c_textures[i].size[1];
				if(aspect < 1.0)
					seduce_background_image_draw(input, &c_textures[i], (float)((i + 1) % 10) * 0.12 - 0.6, (float)((i + 1) / 10) * 0.12 + y + 0.1 + 0.05 * (1.0 - aspect), 0.1, 0.1 * aspect, 0, 0, 1, 1, 1, NULL, c_textures[i].texture_id);
				else
					seduce_background_image_draw(input, &c_textures[i], (float)((i + 1) % 10) * 0.12 - 0.6 + 0.05 * (1.0 - 1.0 / aspect), (float)((i + 1) / 10) * 0.12 + y + 0.1, 0.1 / aspect, 0.1, 0, 0, 1, 1, 1, NULL, c_textures[i].texture_id);
		
			}
			else if(seduce_widget_icon_button_invisible(input, &c_textures[i], x, y))
				*istack = c_textures[i].texture_id;

		}


	}
}
*/

extern RShader *c_editor_verse_preview_shader_get();
/*
	CE_PM_EDIT_PRIMITIVES,
	CE_PM_ADD_COMPONENT,
	CE_PM_SELECT_COMPONENT,
	CE_PM_SELECT_ENTITY,


	C_CC_INITIALIZE, // runs once per instance
	C_CC_PROCESS, // computation only
	C_CC_OBJECT, // computation that can be created without existing output draw. (lights, sounds...)
	C_CC_DRAW, // object to draw
	C_CC_PRIMITIVE_STATE // global state for drawcalls.
	*/

boolean c_editor_popup_icon(BInputState *input, void *id, float y, float dir, uint count, uint nr, char *name, CPreviewGeometry *g, float time)
{	
	uint i, part;
	if(input->mode == BAM_DRAW)
	{
		float x_pos, y_pos, f, array[12] = {-0.05, 0.05, 0,
											0.05, 0.05, 0,
											0.05, -0.05, 0,
											-0.05, -0.05, 0};
		r_matrix_push(NULL);
		if((nr / 10) % 2)
			x_pos = (float)(nr % 10) * 0.12 - 4.5 * 0.12;
		else
			x_pos = (float)(nr % 10) * 0.12 - 0.6;
		y_pos = (float)(nr / 10) * 0.12 * dir + y;
		x_pos *= dir;


		f = 0.0 - sqrt(x_pos * x_pos + y_pos * y_pos);
		if(f > 1.0)
			f = 1.0;
		time = time * 2.0 + f;
		if(time > 1.0)
			time = 1.0;
		r_matrix_translate(NULL, x_pos, y_pos, (1.0 - time));

		for(i = 0; i < input->user_count; i++)
			if(id == seduce_element_selected_id(i, NULL, &part) && nr == part)		
				r_matrix_scale(NULL, 1.2, 1.2, 1.2);
		if(id == seduce_element_popup_action(FALSE))
			r_matrix_scale(NULL, 1.2, 1.2, 1.2);
		for(i = 0; i < input->pointer_count; i++)
			if(id == seduce_element_pointer_id(input, i, &part) && nr == part)
				r_matrix_scale(NULL, 1.2, 1.2, 1.2);

		r_matrix_rotate(NULL, (1.0 - time) * 90.0, 1, 1, 1);
		seduce_element_add_quad(input, id, nr, &array[0], &array[3], &array[6], &array[9]);
		r_primitive_line_2d(-0.05, -0.05, -0.05, 0.05, 0.2, 0.2, 0.9, time);
		r_primitive_line_2d(-0.05, 0.05, 0.05, 0.05, 0.2, 0.2, 0.9, time);
		r_primitive_line_2d(0.05, 0.05, 0.05, -0.05, 0.2, 0.2, 0.9, time);
		r_primitive_line_2d(0.05, -0.05, -0.05, -0.05, 0.2, 0.2, 0.9, time);
		r_primitive_line_2d(-0.05, -0.05, 0.05, 0.05, 0.2, 0.2, 0.9, time);
		r_primitive_line_2d(-0.05, 0.05, 0.05, -0.05, 0.2, 0.2, 0.9, time);
		r_primitive_line_flush();
		seduce_text_line_draw(-0.05, -0.05, SEDUCE_T_SIZE, SEDUCE_T_SPACE, name, 1, 1, 1, 1, -1);

		if(seduce_widget_icon_button(input, &id, SUI_3D_OBJECT_MINUS, 0.04, 0.04, 0.01, time, NULL))
			;

		if(input->mode == BAM_DRAW && g != NULL && g->render_array != NULL)
		{
			r_shader_set(c_editor_verse_preview_shader_get());
			r_matrix_push(NULL);

			r_matrix_scale(NULL, 0.1, 0.1, 0.1);
			r_matrix_rotate(NULL, input->minute_time * 360 * 10.0, 0, 1, 0);
			r_array_section_draw(g->render_array, NULL, GL_LINES, 0, -1);
			r_matrix_pop(NULL);
		} 
		r_matrix_pop(NULL);
	}
	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
			if(!input->pointers[i].button[0] && input->pointers[i].last_button[0])
			{
				if(id == seduce_element_pointer_id(input, i, &part) && nr == part)
				{
					ce_popup_mode = CE_PM_NONE;
					return TRUE;
				}
			}
			if(!input->pointers[i].button[1] && input->pointers[i].last_button[1])
				if(id == seduce_element_pointer_id(input, i, &part) && nr == part)
					return TRUE;
			if(input->pointers[i].button[1] && !input->pointers[i].last_button[1])
				if(id == seduce_element_pointer_id(input, i, &part) && nr == part)
					return TRUE;
		}
		if(id == seduce_element_popup_action(TRUE))
			return TRUE;
		for(i = 0; i < input->user_count; i++)
			if(id == seduce_element_selected_id(i, NULL, &part) && nr == part)
				if(betray_button_get(i, BETRAY_BUTTON_FACE_A))
					return TRUE;
	}
	return FALSE;
}


static float c_popup_animation[CE_PM_COUNT] = {0, 0, 0, 0, 0, 0};
static uint c_primitive_type_create = -1;

void c_editor_popup_list(BInputState *input, CSession *session, CEntity **entity)
{
	char *file_name;
	static uint popup = -1;
	uint i, j, count, output;
	for(i = 0; i < CE_PM_COUNT; i++)
		seduce_animate(input, &c_popup_animation[i], i == ce_popup_mode, 2.0);

	if(ce_popup_mode != CE_PM_ENTITY_CREATE && ce_popup_mode != CE_PM_NONE)
		for(i = 0; i < input->pointer_count; i++)
			if(!input->pointers[i].button[0] && input->pointers[i].last_button[0])
				if(NULL == seduce_element_pointer_id(input, i, NULL))
					ce_popup_mode = CE_PM_NONE;

	if(c_popup_animation[CE_PM_EDIT_PRIMITIVES] > 0.01) 
	{
		float f;
		f = (float)((session->primitive_settings.component_count + 1) / 10) * 0.12 * 0.5 - 0.05;
		for(i = 0; i < session->primitive_settings.component_count; i++)
		{
			if(c_editor_popup_icon(input, session, f + 0.1, 1, session->primitive_settings.component_count + 1, i, confuse_component_name_get(session, session->primitive_settings.components[i].type), NULL, c_popup_animation[CE_PM_EDIT_PRIMITIVES]))
			{
				session->primitive_settings.component_selected = i;
				*entity = &session->primitive_settings;
			}
		}
		if(c_editor_popup_icon(input, session, f + 0.1, 1, session->primitive_count, i, "Add...", NULL, c_popup_animation[CE_PM_EDIT_PRIMITIVES]))
			c_editor_popup_entity_create_replace(-1, NULL, NULL);
		r_primitive_line_2d(-0.7 * c_popup_animation[CE_PM_EDIT_PRIMITIVES], f, 0.7 * c_popup_animation[CE_PM_EDIT_PRIMITIVES], f, 0.2, 0.2, 0.9, 1.0);
		r_primitive_line_flush();
		seduce_text_line_draw(-0.7 * c_popup_animation[CE_PM_EDIT_PRIMITIVES], f + SEDUCE_T_SIZE, SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "Edit Primitive", 1, 1, 1, 1, -1);
	}


	if(c_popup_animation[CE_PM_ADD_COMPONENT] > 0.01) 
	{
		for(i = count = 0; i < confuse_component_count_get(session); i++)
			if(C_CC_DRAW == confuse_component_category_get(session, i))
				count++;
		count++;
		for(i = 0; i < session->primitive_count; i++)
		{
			if(c_editor_popup_icon(input, session, -0.1, -1, session->primitive_count + 1, i, session->prim[i].name, NULL, c_popup_animation[CE_PM_ADD_COMPONENT]))
			{
				if(session->prim[i].type == C_DPT_DRAW_CALL_UNIFORM)
				{
					for(j = 0; j < confuse_component_count_get(session); j++)
					{
						if(C_CC_DRAW == confuse_component_category_get(session, j) && i == confuse_component_primitive_get(session, j))
						{
							c_entity_components_add(session, *entity, j, -1);
							(*entity)->component_selected = (*entity)->component_count - 1;
						}
					}
				}
				if(session->prim[i].type == C_DPT_DRAW_CALL_ATTRIBUTE) // draw a single drawcall with dynamicaly generated vertexx data
				{
					c_primitive_type_create = i;
					ce_popup_mode = CE_PM_ADD_COMPONENT_TYPE;
				}
			}
		}
		if(c_editor_popup_icon(input, &((uint8 *)entity)[1], -0.1, -1, session->primitive_count + 1, i, "Add...", NULL, c_popup_animation[CE_PM_ADD_COMPONENT]))
			c_editor_popup_entity_create_replace(-1, NULL, NULL);
		for(i = count = 0; i < confuse_component_count_get(session); i++)
			if(C_CC_OBJECT == confuse_component_category_get(session, i) ||
				C_CC_FORCE == confuse_component_category_get(session, i))
				count++;
		count++;
		for(i = j = 0; i < confuse_component_count_get(session); i++)
		{
			if(C_CC_OBJECT == confuse_component_category_get(session, i) ||
				C_CC_FORCE == confuse_component_category_get(session, i))
			{
				if(c_editor_popup_icon(input, entity, 0.1, 1, count, j++, confuse_component_name_get(session, i), NULL, c_popup_animation[CE_PM_ADD_COMPONENT]))
				{
					output = confuse_component_output_get(session, i);
					c_entity_components_add(session, *entity, i, -1);
					(*entity)->component_selected = (*entity)->component_count - 1;
				}
			}
		}

		r_primitive_line_2d(-0.7 * c_popup_animation[CE_PM_ADD_COMPONENT], 0.0, 0.7 * c_popup_animation[CE_PM_ADD_COMPONENT], 0.0, 0.2, 0.2, 0.9, 1.0);
		r_primitive_line_flush();
		seduce_text_line_draw(-0.7 * c_popup_animation[CE_PM_ADD_COMPONENT], SEDUCE_T_SIZE, SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "Draw Primitive", 1, 1, 1, 1, -1);
		seduce_text_line_draw(0.7 * c_popup_animation[CE_PM_ADD_COMPONENT] - seduce_text_line_length(SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "Helper Objects", -1), SEDUCE_T_SIZE * -4.0, SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "NAME", 1, 1, 1, 1, -1);
	}


	if(c_popup_animation[CE_PM_ADD_COMPONENT_TYPE] > 0.01) 
	{
		for(i = count = 0; i < confuse_component_count_get(session); i++)
			if(C_CC_DRAW == confuse_component_category_get(session, i) && c_primitive_type_create == confuse_component_primitive_get(session, i))
				count++;

		for(i = j = 0; i < confuse_component_count_get(session); i++)
		{
			if(C_CC_DRAW == confuse_component_category_get(session, i) && c_primitive_type_create == confuse_component_primitive_get(session, i))
			{
				if(c_editor_popup_icon(input, session, -0.1, -1, count, j, confuse_component_name_get(session, i), NULL, c_popup_animation[CE_PM_ADD_COMPONENT_TYPE]))
				{
					c_entity_components_add(session, *entity, i, -1);
					(*entity)->component_selected = (*entity)->component_count - 1;
				}
				j++;
			}
		}
		r_primitive_line_2d(-0.7 * c_popup_animation[CE_PM_ADD_COMPONENT_TYPE], 0.0, 0.7 * c_popup_animation[CE_PM_ADD_COMPONENT_TYPE], 0.0, 0.2, 0.2, 0.9, 1.0);
		r_primitive_line_flush();
		seduce_text_line_draw(-0.7 * c_popup_animation[CE_PM_ADD_COMPONENT_TYPE], SEDUCE_T_SIZE, SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "Draw Primitive", 1, 1, 1, 1, -1);
	}

			
	if(c_popup_animation[CE_PM_SELECT_COMPONENT] > 0.01) 
	{
		CEntity *e;
		float f;
		e = *entity;
		f = (float)((e->component_count + 1) / 10) * 0.12 * 0.5 - 0.05;
		for(i = 0; i < e->component_count; i++)
		{
			if(c_editor_popup_icon(input, session, f + 0.1, 1, e->component_count + 1, i, confuse_component_name_get(session, e->components[i].type), NULL, c_popup_animation[CE_PM_SELECT_COMPONENT]))
			{
				e->component_selected = i;
			}
		}
		if(c_editor_popup_icon(input, session, f + 0.1, 1, e->component_count + 1, e->component_count, "New...", NULL, c_popup_animation[CE_PM_SELECT_COMPONENT]))
			ce_popup_mode = CE_PM_ADD_COMPONENT;
		r_primitive_line_2d(-0.7 * c_popup_animation[CE_PM_SELECT_COMPONENT], f, 0.7 * c_popup_animation[CE_PM_SELECT_COMPONENT], f, 0.2, 0.2, 0.9, 1.0);
		r_primitive_line_flush();
		seduce_text_line_draw(-0.7 * c_popup_animation[CE_PM_SELECT_COMPONENT], f + SEDUCE_T_SIZE, SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "Select Entity", 1, 1, 1, 1, -1);
	}

	if(c_popup_animation[CE_PM_SELECT_ENTITY] > 0.01) 
	{
		float f;
		f = (float)((session->entity_define_count + 1) / 10) * 0.12 * 0.5 - 0.05;
		f -= 0.5;
		for(i = 0; i < session->entity_define_count; i++)
			if(c_editor_popup_icon(input, session, f + 0.1, 1, session->entity_define_count + 1, i, session->entity_defines[i].name, NULL, c_popup_animation[CE_PM_SELECT_ENTITY]))
				*entity = &session->entity_defines[i];
		if(c_editor_popup_icon(input, session, f + 0.1, 1, session->entity_define_count + 2, i, "Clone...", NULL, c_popup_animation[CE_PM_SELECT_ENTITY]))
			*entity = confuse_session_entity_clone(session, *entity);
		if(c_editor_popup_icon(input, session, f + 0.1, 1, session->entity_define_count + 2, i + 1, "New...", NULL, c_popup_animation[CE_PM_SELECT_ENTITY]))
			*entity = confuse_session_entity_add(session, "unnamed");

		r_primitive_line_2d(-0.7 * c_popup_animation[CE_PM_SELECT_ENTITY], f, 0.7 * c_popup_animation[CE_PM_SELECT_ENTITY], f, 0.2, 0.2, 0.9, 1.0);
		r_primitive_line_flush();
		seduce_text_line_draw(-0.7 * c_popup_animation[CE_PM_SELECT_ENTITY], f + SEDUCE_T_SIZE, SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "Select Entity", 1, 1, 1, 1, -1);
	}
	if(c_popup_animation[CE_PM_ENTITY_CREATE] > 0.01)
	{
		c_editor_popup_entity_create(input, session, c_popup_animation[CE_PM_ENTITY_CREATE], ce_popup_create_replacement_primitive);
	}

	file_name = betray_requester_save_get(&ce_popup_mode);
	file_name = betray_requester_load_get(&ce_popup_mode);
	if(file_name != NULL)
	{
		THandle *h;
		h = testify_file_load(file_name);
		if(h != NULL)
		{
			confuse_session_load(session, h);
			testify_free(h);
		}
	}

	file_name = betray_requester_save_get(&ce_popup_mode);
	if(file_name != NULL)
	{
		THandle *h;
		h = testify_file_save(file_name);
		if(h != NULL)
		{
			confuse_session_save(session, h);
			testify_free(h);
		}
	}
	/*	break;
		case CE_PM_ADD_COMPONENT :
	
		break;
		case CE_PM_EDIT_COMPONENT :
		break;
		case CE_PM_SELECT_ENTITY:

		break;
	}*/
}

void c_editor_popup(BInputState *input, CSession *session, CEntity **entity)
{
	float *color[4] = {1, 0, 1, 0};
	c_editor_popup_list(input, session, entity);
	seduce_popup_detect_mouse(input, 2, c_editor_popup_func, *entity);
	seduce_popup_detect_multitouch(input, 5, c_editor_popup_func, *entity);
	seduce_popup_detect_axis(input, BETRAY_BUTTON_FACE_Y, c_editor_popup_func, *entity);
//	seduce_popup_detect_icon(input, c_editor_popup, 23, 0.15, 0, 0.01, 1, c_editor_popup_func, *entity, TRUE, color);
}

float c_editor_popup_animation_get()
{
	return c_popup_animation[CE_PM_NONE];
}