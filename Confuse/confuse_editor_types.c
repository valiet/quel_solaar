#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "s_draw_3d.h"
#include "c_internal.h"


extern uint c_texture_count;
extern CTexture *c_textures;
extern boolean c_load_texture(char *file_name);
extern CPreviewGeometry *c_editor_verse_preview_next(CPreviewGeometry *data);
extern void c_editor_verse_preview_update();
extern RShader *c_editor_verse_preview_shader_get();
extern void c_editor_verse_vertex_fill(float *array, uint array_length, uint node_id);

uint c_editor_display_stack[64] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float c_editor_display_stack_timer[64] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

uint c_editor_display_stack_last[64] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
float c_editor_display_stack_timer_last[64] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

void c_editor_constraint(BInputState *input, CSession *session, CEntity *entity,  RMatrix *matrix, float x, float y, uint component, uint recursions, float timer);
boolean c_editor_popup_icon(BInputState *input, void *id, float y, float dir, uint count, uint nr, char *name, CPreviewGeometry *g, float time);
extern void seduce_widget_overlay_matrix(RMatrix *matrix);

boolean	confuse_editor_cube_draw(BInputState *input, float pos_a_x, float pos_a_y, float pos_a_z, float pos_b_x, float pos_b_y, float pos_b_z, float time)
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

typedef struct{
	CEntity *entity;
	uint *ref;
	CType *type;
	CTypeType base_type;
	CSession *session;
}CBarPopupParam;

static CBarPopupParam rename_param = {NULL};
static char rename_name[64] = {0};

void c_editor_bar_rename_activate(CBarPopupParam *param)
{
	if(param == NULL)
		rename_param.entity = NULL;
	else
		rename_param = *param;
}

void c_editor_bar_rename(BInputState *input, void *user_pointer)
{
	static float timer = 0.0, start_pos[2] = {0, 0};
	static float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	RMatrix *matrix;
	char name;
	static uint uvalue_0 = 1;
	uint i;
	char *t;

	seduce_animate(input, &timer, rename_param.entity != NULL, 1);
	if(timer < 0.01)
		return;
		/* Backgrounds */

	matrix = r_matrix_get();
	r_matrix_push(matrix);
	r_matrix_matrix_mult(matrix, m);
	seduce_background_shape_draw(input, m, 0.000, 0.040, 0.000, -0.120, 0.080, -0.080, 0.080, 0.020, timer, 0.100, 0.000, NULL, 0.5);
	seduce_background_shape_draw(input, m, 0.000, -0.120, 0.000, 0.040, -0.080, 0.020, -0.080, -0.080, timer, -0.100, 0.000, NULL, 0.5);
	seduce_background_polygon_flush(input, start_pos, timer);
		/* Widgets */


	seduce_text_line_edit(input, rename_name, rename_name, 64, -0.060, 0.000, 0.120, 0.007, seduce_translate("Name"), TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);
	seduce_text_uint_edit(input, &uvalue_0, &uvalue_0, -0.060, -0.020, 0.120, 0.007, TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);


	t = "OK";
	if(seduce_text_button(input, t, -0.060, -0.060, 0, 0.007, 0.200, t, 1.000, 1.000, 1.000, 1.000, 0.200, 0.600, 1.000, 1.000))
	{
		*rename_param.ref = c_entity_stack_add(rename_param.entity, rename_param.type->type, rename_name, uvalue_0);
		c_editor_bar_rename_activate(&rename_param);
		rename_param.entity = NULL;
	}


	t = "Cancel";
	if(seduce_text_button(input, t, 0.020, -0.060, 0, 0.007, 0.200, t, 1.000, 1.000, 1.000, 1.000, 0.200, 0.600, 1.000, 1.000))
		rename_param.entity = NULL;
	
	seduce_background_shape_matrix_interact(input, m, m, TRUE, TRUE);
	r_matrix_pop(matrix);
}

extern uint c_editor_stack_popup(BInputState *input, CSession *session, CEntity *entity, uint type);

void c_editor_bar_popup_func(BInputState *input, float time, void *user)
{
	CBarPopupParam *param;
	uint constraint_count, i, j;
	float f;

	param = user;
	

	for(constraint_count = i = 0; i < confuse_component_count_get(param->session); i++)
		if(confuse_component_output_available(param->session, i, /*param->type->type*/param->base_type))
			constraint_count++;

//	param->session->component_list[param->entity].param_types[param->type->number]
	i = c_editor_stack_popup(input, param->session, param->entity, /*param->type->type*/param->base_type);
	if(i != -1)
	{
		*param->ref = i;
		return;
	}


	f = (float)((constraint_count) / 10) * 0.12 * 0.5 - 0.05;
	for(i = j = 0; i < confuse_component_count_get(param->session); i++)
	{	
		if(confuse_component_output_available(param->session, i, /*param->type->type*/param->base_type))
		{
			if(c_editor_popup_icon(input, param->session, f + 0.1, 1, constraint_count, j++, confuse_component_name_get(param->session, i), NULL, time))
			{
				uint dependency, stack_pos;
				dependency = param->entity->stack_types[*param->ref].dependency;

				if(dependency != -1)
				{
					uint a, b;
					stack_pos = c_entity_stack_add(param->entity, param->entity->stack_types[*param->ref].type, "placeholder", 1);
					a = confuse_component_output_pos_get(param->session, &param->entity->components[dependency]);
					param->entity->components[dependency].references[a] = stack_pos;


					param->entity->stack_types[*param->ref].dependency = stack_pos;
				}
				c_entity_components_add(param->session, param->entity, i, *param->ref);
			}
		}
	}
	if(input->mode == BAM_DRAW)
	{
		r_primitive_line_2d(-0.7 * time, f, 0.7 * time, f, 0.2, 0.2, 0.9, 1.0);
		r_primitive_line_flush();
		seduce_text_line_draw(-0.7 * time, f + SEDUCE_T_SIZE, SEDUCE_T_SIZE * 2.0, SEDUCE_T_SPACE, "Select Entity", 1, 1, 1, 1, -1);
	}
/*	static SUIPUElement *element = NULL;
	static uint element_count = 0;
	uint *costraints;
	CBarPopupParam *param;
	uint i, j, id = 0, type_count = 0, constraint_count;
	param = user;


	for(constraint_count = i = 0; i < confuse_component_count_get(param->session); i++)
		if(confuse_component_output_available(param->session, i, param->type->type))
			constraint_count++;

	for(i = 0; i < param->entity->stack_type_count; i++)
		if(param->entity->stack_types[i].type == param->type->type)
			type_count++;

	if(constraint_count + type_count + 2 > element_count)
	{
		element_count = constraint_count + type_count + 2 + 16;
		element = realloc(element, (sizeof *element) * element_count);
	}

	element[id].type = S_PUT_ANGLE;
	element[id].text = "New";
	element[id].data.angle[0] = 60;
	element[id].data.angle[1] = 120;
	id++;
	element[id].type = S_PUT_ANGLE;
	element[id].text = "Reset";
	element[id].data.angle[0] =180 + 60;
	element[id].data.angle[1] = 180 + 120;
	id++;

	for(i = 0; i < param->entity->stack_type_count; i++)
	{
		if(param->entity->stack_types[i].type == param->type->type)
		{
			element[id].type = S_PUT_TOP;
			element[id].text = param->entity->stack_types[i].name;
			id++;
		}
	}

	for(constraint_count = i = 0; i < confuse_component_count_get(param->session); i++)
	{
		if(confuse_component_output_available(param->session, i, param->type->type))
		{
			element[id].type = S_PUT_BOTTOM;
			element[id].text = confuse_component_name_get(param->session, i);
			id++;
		}
	}

	id = seduce_popup(input, element, id, time);
	if(id != -1)
	{
		if(id == 0)
		{
			c_editor_bar_rename_activate(param);
		}else if(id == 1)
		{
			i = *param->ref;
			c_entity_stack_clear(param->entity->stack, param->type->type, param->entity->stack_types[i].size, param->entity->stack_types[i].pos);
		}else if(id < type_count + 2)
		{	
			type_count = 2;
			for(i = 0; i < param->entity->stack_type_count; i++)
			{
				if(param->entity->stack_types[i].type == param->type->type)
				{
					if(id == type_count)
						*param->ref = i;
					type_count++;
				}
			}
		}else
		{
			j = type_count + 2;
			for(constraint_count = i = 0; i < confuse_component_count_get(param->session); i++)
				if(confuse_component_output_available(param->session, i, param->type->type))
					if(j++ == id)	
						c_entity_components_add(param->session, param->entity, i, *param->ref);
		}
	}*/
}
/*
void c_editor_cloud_gen(BInputState *input, CEntity *entity, CType *type, float x, float y, RMatrix *matrix, float scale, float time)
{
	static float size = 0.2;
	static char *lables[2] = {"Center", "Shell", "Sphere", "Cloud"};
	static selected = 0;
	float origo[3] = {0, 0, 0};
	seduce_widget_select_radial(input, &lables, &selected, lables, 2, S_PUT_ANGLE, x, y,  0.05, scale, time, FALSE);
	boolean	seduce_manipulator_radius(BInputState *input, matrix, origo, float *radius, void *id, float time)
}*/

uint c_editor_type_object_list(BInputState *input, void *id, float time, float y)
{
	uint i, count;
	CPreviewGeometry *g;
	c_editor_verse_preview_update();
	g = c_editor_verse_preview_next(NULL);
	for(count = 0; g != NULL; count++)
		g = c_editor_verse_preview_next(g);
	g = c_editor_verse_preview_next(NULL);
	for(i = 0; g != NULL; i++)
	{
		if(c_editor_popup_icon(input, id, y + 0.1, 1,  count, i, g->name, g, time))
			return g->node_id;
		g = c_editor_verse_preview_next(g);
	}
	return 0;
}

void c_editor_type(BInputState *input, CSession *session, CEntity *entity, CType *type, CTypeType base_type, uint *ref, float *stack, float x, float y, RMatrix *matrix, float *center, float scale, float time, uint recursions)
{
	static CType *expanded = NULL;
	CBarPopupParam param;
	uint i, j;

	recursions++;
	if(recursions == 64)
			return;
//	 seduce_widget_icon_button(input, &type->data.real[1], SUI_3D_OBJECT_LOCK, x, y - 0.05, 0.025, time, NULL);
/*	if(type->size > 10)
	{
		if(expanded != type)
		{
			if(seduce_widget_icon_button(input, &stack[type->pos], SUI_3D_OBJECT_SNAPLOCK, x, y + 0.05, 0.025, time, NULL))
				expanded = type;
			return;
		}
	}*/
	

	if(type->dependency != -1)
	{
		boolean toggle;
		toggle = c_editor_display_stack[recursions] == type->dependency;
		seduce_widget_icon_toggle(input, &((uint8 *)(&type->dependency))[1], &toggle, SUI_3D_OBJECT_BOOST, x, y, 0.05, time);
		if(toggle != (c_editor_display_stack[recursions] == type->dependency))
		{
			c_editor_display_stack_last[recursions] = c_editor_display_stack[recursions];
			c_editor_display_stack_timer_last[recursions] = c_editor_display_stack_timer[recursions];
			c_editor_display_stack_timer[recursions] = 0.0;
			if(toggle)
				c_editor_display_stack[recursions] = type->dependency;
			else
				c_editor_display_stack[recursions] = -1;
		}
		if(c_editor_display_stack[recursions] == type->dependency)
			c_editor_constraint(input, session, entity,  matrix, x, y, type->dependency, recursions, c_editor_display_stack_timer[recursions] * time);
		else if(c_editor_display_stack_last[recursions] == type->dependency)
			c_editor_constraint(input, session, entity,  matrix, x, y, type->dependency, recursions, c_editor_display_stack_timer_last[recursions] * time);
	}else
	{
		switch(type->type)
		{
			case C_TYPE_FLOAT :
				seduce_widget_slider_radial(input, &stack[type->pos + type->selected % type->size], &stack[type->pos + type->selected % type->size], x, y, 0.05, scale * 0.25, 0, 1, time, NULL);
			break;

			case C_TYPE_AREA :
				{
					static boolean track = TRUE;
					char *lables[3] = {"Collapse", "Square", "Center"};
					RMatrix draw_matrix, *reset;
					float tmp[3], scaled[2], tmp2;
					uint output;
					output = -1;
					if(stack[type->pos + (type->selected % type->size) * 2] < 0.00001 &&
						stack[type->pos + (type->selected % type->size) * 2 + 1] < 0.00001)
						lables[0] = "Expand";
					if(!track)
						lables[2] = "Track";
					seduce_widget_select_radial(input, &stack[type->pos + 1 + (type->selected % type->size) * 2], &output, lables, 3, S_PUT_ANGLE, x, y,  0.05, scale, time, TRUE);
					if(output == 0)
					{
						if(stack[type->pos + (type->selected % type->size) * 2] < 0.00001 &&
							stack[type->pos + (type->selected % type->size) * 2 + 1] < 0.00001)
							stack[type->pos + (type->selected % type->size) * 2] = stack[type->pos + (type->selected % type->size) * 2 + 1] = 0.1;
						else
							stack[type->pos + (type->selected % type->size) * 2] = stack[type->pos + (type->selected % type->size) * 2 + 1] = 0.0;
					}
					if(output == 1)
					{
						float f;
						f = (stack[type->pos + (type->selected % type->size) * 2] + stack[type->pos + 1 + (type->selected % type->size) * 2]) * 0.5;
						stack[type->pos + (type->selected % type->size) * 2] = f;
						stack[type->pos + (type->selected % type->size) * 2 + 1] = f;
					}
					if(output == 2)
						track = !track;
					reset = r_matrix_get();
					seduce_widget_overlay_matrix(&draw_matrix);
					if(track)
						r_matrix_projection_screenf(matrix, tmp, center[0], center[1], center[2]);
					else
						r_matrix_projection_screenf(matrix, tmp, 0, 0, 0);
					scaled[0] = stack[type->pos + (type->selected % type->size) * 2] / -tmp[2];
					scaled[1] = stack[type->pos + (type->selected % type->size) * 2 + 1] / -tmp[2];
					tmp2 = -tmp[2];
					tmp[2] = 0;
					if(!track)
					{
						tmp[0] = x;
						tmp[1] = y + 0.1 * scale;
						r_matrix_set(reset);
					}
					if(seduce_manipulator_square_centered(input, NULL, tmp, scaled, &stack[type->pos + (type->selected % type->size) * 2], NULL, FALSE, FALSE, scale,  time))
					{
						stack[type->pos + (type->selected % type->size) * 2] = scaled[0] * tmp2;
						stack[type->pos + (type->selected % type->size) * 2 + 1] = scaled[1] * tmp2;
					}
					r_matrix_set(reset);
				}
			break;
			case C_TYPE_POS :
				{
					static boolean multi_move = FALSE;
					static uint selected = 1, output; 
					char *lables[8] = {"XYZ", "point", "Center", "Move All", "Shell", "Sphere", "Cloud", "Fill Geometry"};
					float center[3], size;
					if(expanded == type)
					{
						output = c_editor_type_object_list(input, &stack[type->pos + 2], time, y);
						if(output != 0)	
						{
							c_editor_verse_vertex_fill(&stack[type->pos], type->size, output);
							expanded = NULL;
						}
					}

					output = -1; 
					if(type->size == 1)
					{
						center[0] = stack[type->pos + 0];
						center[1] = stack[type->pos + 1];
						center[2] = stack[type->pos + 2];
						seduce_widget_select_radial(input, &stack[type->pos + 1], &output, lables, 3, S_PUT_ANGLE, x, y,  0.05, scale, time, TRUE);
					}else
					{
						float max[3], min[3];
						i = output;
						seduce_widget_select_radial(input, &stack[type->pos + 1], &output, lables, 8, S_PUT_ANGLE, x, y,  0.05, scale, time, TRUE);
						if(output == 7)
						{
							expanded = type;
							output = i;
						}else if(i != output && expanded == type)
							expanded = NULL;

						max[0] = min[0] = stack[type->pos];
						max[1] = min[1] = stack[type->pos + 1];
						max[2] = min[2] = stack[type->pos + 2];
						for(i = 1; i < type->size; i++)
						{
							if(min[0] > stack[type->pos + i * 3 + 0])
								min[0] = stack[type->pos + i * 3 + 0];
							if(min[1] > stack[type->pos + i * 3 + 1])
								min[1] = stack[type->pos + i * 3 + 1];
							if(min[2] > stack[type->pos + i * 3 + 2])
								min[2] = stack[type->pos + i * 3 + 2];
							if(max[0] < stack[type->pos + i * 3 + 0])
								max[0] = stack[type->pos + i * 3 + 0];
							if(max[1] < stack[type->pos + i * 3 + 1])
								max[1] = stack[type->pos + i * 3 + 1];
							if(max[2] < stack[type->pos + i * 3 + 2])
								max[2] = stack[type->pos + i * 3 + 2];
						}
						center[0] = (min[0] + max[0]) / 2.0;
						center[1] = (min[1] + max[1]) / 2.0;
						center[2] = (min[2] + max[2]) / 2.0;
						size = max[0] - min[0];
						if(size < max[1] - min[1])
							size = max[1] - min[1];
						if(size < max[2] - min[2])
							size = max[2] - min[2];
						size /= 2;
						if(size < 0.1)
							size = 0.1;
						if(!multi_move)
							lables[3] = "Move Individual";
					}
					if(output != -1)
					{
						float vec[3], f;
						switch(output)
						{
							case 0 :
							case 1 :
								selected = output;
							break;
							case 2 :
								for(i = 0; i < type->size * 3; i++)
									stack[type->pos + i] = 0;
							break;
							case 3 :
								multi_move = !multi_move;
							break;
							case 4 :
							{
								for(i = j = 0; i < type->size; i++)
								{
									f = 10;
									while(f > 1.0)
									{
										vec[0] = f_randnf(j++);
										vec[1] = f_randnf(j++);
										vec[2] = f_randnf(j++);
										f = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
									}
									f = sqrt(f);
									stack[type->pos + i * 3 + 0] = vec[0] * size / f;
									stack[type->pos + i * 3 + 1] = vec[1] * size / f;
									stack[type->pos + i * 3 + 2] = vec[2] * size / f;
								}
							}
							break;
							case 5 :
							{
								for(i = j = 0; i < type->size; i++)
								{
									f = 10;
									while(f > 1.0)
									{
										vec[0] = f_randnf(j++);
										vec[1] = f_randnf(j++);
										vec[2] = f_randnf(j++);
										f = vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2];
									}
									stack[type->pos + i * 3 + 0] = vec[0] * size;
									stack[type->pos + i * 3 + 1] = vec[1] * size;
									stack[type->pos + i * 3 + 2] = vec[2] * size;
								}
							}
							break;
							case 6 :
							{
								float max[3], min[3], f;
								vec[0] = center[0];
								vec[1] = center[1];
								vec[2] = center[2];
								for(i = j = 0; i < type->size; i++)
								{
									f = 10;
									if(i % (type->size / 10) == (type->size / 10) -1)
									{
										if(f_randf(j++) > 0)
										{
											vec[0] = center[0];
											vec[1] = center[1];
											vec[2] = center[2];
										}
									}
									vec[0] += f_randnf(j++) * 0.03 * size;
									vec[1] += f_randnf(j++) * 0.03 * size;
									vec[2] += f_randnf(j++) * 0.03 * size;
									stack[type->pos + i * 3 + 0] = vec[0];
									stack[type->pos + i * 3 + 1] = vec[1];
									stack[type->pos + i * 3 + 2] = vec[2];
								}
								max[0] = min[0] = stack[type->pos];
								max[1] = min[1] = stack[type->pos + 1];
								max[2] = min[2] = stack[type->pos + 2];
								for(i = 1; i < type->size; i++)
								{
									if(min[0] > stack[type->pos + i * 3 + 0])
										min[0] = stack[type->pos + i * 3 + 0];
									if(min[1] > stack[type->pos + i * 3 + 1])
										min[1] = stack[type->pos + i * 3 + 1];
									if(min[2] > stack[type->pos + i * 3 + 2])
										min[2] = stack[type->pos + i * 3 + 2];
									if(max[0] < stack[type->pos + i * 3 + 0])
										max[0] = stack[type->pos + i * 3 + 0];
									if(max[1] < stack[type->pos + i * 3 + 1])
										max[1] = stack[type->pos + i * 3 + 1];
									if(max[2] < stack[type->pos + i * 3 + 2])
										max[2] = stack[type->pos + i * 3 + 2];
								}
								center[0] -= (min[0] + max[0]) / 2.0;
								center[1] -= (min[1] + max[1]) / 2.0;
								center[2] -= (min[2] + max[2]) / 2.0;
								f = max[0] - min[0];
								if(f < max[1] - min[1])
									f = max[1] - min[1];
								if(f < max[2] - min[2])
									f = max[2] - min[2];
								f /= 2;
								size = size / f;

								for(i = j = 0; i < type->size; i++)
								{
									stack[type->pos + i * 3 + 0] = stack[type->pos + i * 3 + 0] / size + center[0];
									stack[type->pos + i * 3 + 1] = stack[type->pos + i * 3 + 1] / size + center[1];
									stack[type->pos + i * 3 + 2] = stack[type->pos + i * 3 + 2] / size + center[2];
								}
							}
							break;
						}
					}
					if(multi_move)
					{
						if(selected)
						{
							for(i = 0; i < type->size; i++)
								seduce_manipulator_point(input, matrix, &stack[type->pos + i * 3], &stack[type->pos + i * 3], 0.5);
						}else for(i = 0; i < type->size; i++)
							seduce_manipulator_pos_xyz(input, matrix, &stack[type->pos + i * 3], &stack[type->pos + i * 3], NULL, FALSE, TRUE, TRUE, TRUE, scale, time);
					}else
					{
						float change[3];
						change[0] = center[0];
						change[1] = center[1];
						change[2] = center[2];
						if((selected && seduce_manipulator_point(input, matrix, change, &stack[type->pos], 1)) ||
							(!selected && seduce_manipulator_pos_xyz(input, matrix, change, &stack[type->pos], NULL, FALSE, TRUE, TRUE, TRUE, scale, time)))
						{
							change[0] -= center[0];
							change[1] -= center[1];
							change[2] -= center[2];
							for(i = 0; i < type->size; i++)
							{
								stack[type->pos + i * 3] += change[0];
								stack[type->pos + i * 3 + 1] += change[1];
								stack[type->pos + i * 3 + 2] += change[2];
							}
						}
					}
					if(type->size > 1)
					{
						float previus;
						previus = size;
						if(seduce_manipulator_radius(input, matrix, center, &size, &stack[type->pos + 2], time))
						{
							size /= previus;
							for(i = 0; i < type->size * 3; i++)
								stack[type->pos + i] *= size;
						}
					}
				}
			break;
			case C_TYPE_COLOR :
				{
					float hsv[3];
					seduce_widget_wheel_radial(input, &stack[type->pos + (type->selected % type->size) * 4], &stack[type->pos + (type->selected % type->size) * 4], x, y - 0.03 * 0.6, 0.035, scale * 0.25, time);
					f_rgb_to_hsv(hsv, stack[type->pos + (type->selected % type->size) * 4], stack[type->pos + (type->selected % type->size) * 4 + 1], stack[type->pos + (type->selected % type->size) * 4 + 2]);
					if(seduce_widget_slider_radial(input, &stack[type->pos + (type->selected % type->size) * 4 + 1], &hsv[2], x - 0.025 * 0.6, y + 0.02 * 0.6, 0.035, scale * 0.25, 0, 1, time, &stack[type->pos + (type->selected % type->size) * 4]))
						f_hsv_to_rgb(&stack[type->pos + (type->selected % type->size) * 4], hsv[0], hsv[1], hsv[2]);
					seduce_widget_slider_radial(input, &stack[type->pos + (type->selected % type->size) * 4 + 3], &stack[type->pos + (type->selected % type->size) * 4 + 3], x + 0.025 * 0.6, y + 0.02 * 0.6, 0.035, scale * 0.25, 0, 1, time, NULL);
				}
			break;
			case C_TYPE_MATRIX :
			case C_TYPE_POS_QUATERNION_SIZE :
				{
					static char *lables[9] = {"XYZ", "point", "Rotate", "Cube",  "Scale", "All", "Reset Pos", "Reset Rotation", "Reset Scale"};
					float rotation_matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, origo[3] = {0, 0, 0}, object_scale[3], pos_quaternion_scale[10];
					static uint selected = 1;
					uint output, size = 10;
					boolean updated = FALSE;
					RMatrix *reset;
					output = selected;
					if(type->type == C_TYPE_MATRIX)
						size = 16;
					else
						size = 10;

					if(type->type == C_TYPE_MATRIX)
						f_matrix_to_pos_quaternion_scalef(&stack[type->pos + (type->selected % type->size) * 16], &pos_quaternion_scale[0], &pos_quaternion_scale[3], &pos_quaternion_scale[7]);
					else
						for(i = 0; i < 10; i++)
							pos_quaternion_scale[i] = stack[type->pos + (type->selected % type->size) * size + i];
					if(type->size == 1)
						seduce_widget_select_radial(input, &stack[type->pos + (type->selected % type->size) * size + 1], &output, lables, 9, S_PUT_ANGLE, x, y,  0.05, scale, time, TRUE);
					else
						seduce_widget_select_radial(input, &stack[type->pos + (type->selected % type->size) * size + 1], &output, lables, 6, S_PUT_ANGLE, x, y,  0.05, scale, time, TRUE);
					if(output  < 6)
						selected = output;
					if(output == 6)
					{	
						pos_quaternion_scale[0] = pos_quaternion_scale[1] = pos_quaternion_scale[2] = 0;
						updated = TRUE;
					}
					if(output == 7)
					{
						pos_quaternion_scale[3] = pos_quaternion_scale[4] = pos_quaternion_scale[5] = 0;
						pos_quaternion_scale[6] = 1;
						updated = TRUE;
					}
					if(output == 8)
					{
						pos_quaternion_scale[6] = 1;
						pos_quaternion_scale[8] = 1;
						pos_quaternion_scale[9] = 1;
						updated = TRUE;
					}
					for(i = 0; i < type->size; i++)
					{
						if(selected == 0)
							if(seduce_manipulator_pos_xyz(input, matrix, pos_quaternion_scale, &stack[type->pos + (type->selected % type->size) * size + i * 10], NULL, FALSE, TRUE, TRUE, TRUE, scale, time))
								updated = TRUE;
						if(selected == 1 || selected == 5)
							if(seduce_manipulator_point(input, matrix, pos_quaternion_scale, &stack[type->pos + (type->selected % type->size) * size + i * 10], scale))
								updated = TRUE;

						if(selected == 2 || selected == 5)
						{
							f_quaternion_to_matrixf(rotation_matrix, pos_quaternion_scale[3], pos_quaternion_scale[4], pos_quaternion_scale[5], pos_quaternion_scale[6]);
							if(seduce_manipulator_rotate(input, matrix, pos_quaternion_scale, rotation_matrix, &stack[type->pos + i * 10 + 2], NULL, FALSE, scale, time))
							{
								f_matrix_to_quaternionf(&pos_quaternion_scale[3], rotation_matrix);
								updated = TRUE;
							}
						}
						r_matrix_push(matrix);
						f_quaternion_to_matrixf(rotation_matrix, pos_quaternion_scale[3], pos_quaternion_scale[4], pos_quaternion_scale[5], pos_quaternion_scale[6]);
						r_matrix_translate(matrix, pos_quaternion_scale[0], pos_quaternion_scale[1], pos_quaternion_scale[2]);
						r_matrix_matrix_mult(matrix, rotation_matrix);
						if(selected == 3 || selected == 5)
							if(seduce_manipulator_cube_centered(input, matrix,  origo,  &pos_quaternion_scale[7],  &stack[type->pos + i * 10 + 3], NULL, FALSE, scale * 0.5, time))
								updated = TRUE;
						if(selected == 4)
							if(seduce_manipulator_scale(input, matrix, pos_quaternion_scale, &pos_quaternion_scale[7], &stack[type->pos + i * 10 + 4], NULL, FALSE, TRUE, TRUE, TRUE, scale, time))
								updated = TRUE;
						reset = r_matrix_get();
						r_matrix_set(matrix);
						confuse_editor_cube_draw(input, -pos_quaternion_scale[7], -pos_quaternion_scale[8], -pos_quaternion_scale[9], pos_quaternion_scale[7], pos_quaternion_scale[8], pos_quaternion_scale[9], time);
						r_matrix_set(reset);
						r_matrix_pop(matrix);
						if(updated)
						{
							if(type->type == C_TYPE_POS_QUATERNION_SIZE)
							{
								for(i = 0; i < 10; i++)
									stack[type->pos + (type->selected % type->size) * size + i] = pos_quaternion_scale[i];
							}else
								f_pos_quaternion_scale_to_matrixf(&pos_quaternion_scale[0], &pos_quaternion_scale[3], &pos_quaternion_scale[7], &stack[type->pos + (type->selected % type->size) * 16]);
						}
						
					}
				}
			break;
			case C_TYPE_UV :
			case C_TYPE_UV_SECTION :
				{
					static uint image_id = 0;
					static boolean image_expanded = FALSE;
					if(seduce_widget_icon_button(input, &stack[type->pos], SUI_3D_OBJECT_SNAPLOCK, x, y, 0.05, time, NULL))
					{
						if(expanded == type)
							expanded = NULL;
						else
							expanded = type;
					}
					if(expanded == type)
					{
						static int grid = 3;
						float f, down_left[3], up_right[3];
						uint add;
						glEnable(GL_BLEND);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						r_primitive_line_2d(x - 0.025, y + 0.075, x - 0.05, y + 0.1, 1, 1, 1, 0.5);
						r_primitive_line_2d(x + 0.025, y + 0.075, x + 0.05, y + 0.1, 1, 1, 1, 0.5);

						r_primitive_line_2d(x + 0.15, y + 0.1, x + 0.05, y + 0.1, 1, 1, 1, 0.5);
						r_primitive_line_2d(x - 0.15, y + 0.1, x - 0.05, y + 0.1, 1, 1, 1, 0.5);
						r_primitive_line_2d(x - 0.15, y + 0.4, x + 0.15, y + 0.4, 1, 1, 1, 0.5);

						r_primitive_line_2d(x - 0.15, y + 0.1, x - 0.15, y + 0.4, 1, 1, 1, 0.5);
						r_primitive_line_2d(x + 0.15, y + 0.1, x + 0.15, y + 0.4, 1, 1, 1, 0.5);
						if(c_textures != NULL)
						{

							seduce_background_image_draw(input, NULL, x + 0.2 - 0.025, y + 0.2 - 0.025, 0, 0.05, 0.05, 0, 0, 1, 1, time, NULL, c_textures[image_id].texture_id);
							if(seduce_widget_icon_button_invisible(input, &image_id, x + 0.2, y + 0.2, 0.01))
								image_expanded = !image_expanded;

							if(image_expanded)
							{
								for(i = 0; i < c_texture_count; i++)
								{
									if(input->mode == BAM_DRAW)
									{
										float aspect;
										aspect = (float)c_textures[i].size[0] / (float)c_textures[i].size[1];
										if(aspect < 1.0)
											seduce_background_image_draw(input, &c_textures[i], (float)((i + 1) % 10) * 0.12 - 0.6, (float)((i + 1) / 10) * 0.12 + y + 0.25 + 0.05 * (1.0 - aspect), 0, 0.1, 0.1 * aspect, 0, 0, 1, 1, time, NULL, c_textures[i].texture_id);
										else
											seduce_background_image_draw(input, &c_textures[i], (float)((i + 1) % 10) * 0.12 - 0.6 + 0.05 * (1.0 - 1.0 / aspect), (float)((i + 1) / 10) * 0.12 + y + 0.25, 0, 0.1 / aspect, 0.1, 0, 0, 1, 1, time, NULL, c_textures[i].texture_id);
		
									}
									else if(seduce_widget_icon_button_invisible(input, &c_textures[i], x, y, 0.01))
									{	
										image_expanded = FALSE;
										image_id = i;
									}
								}
							}
						}
					//	if(c_textures != NULL)
					//		seduce_background_image_draw(input, NULL, x - 0.15, y + 0.1, 0.3, 0.3, 0, 0, 1, 1, time, NULL, c_textures[image_id].texture_id);

						add = 1;
						for(i = 1; i < grid; i++)
							add *= 2;
						if(grid != 0)
						{
							for(i = 0; i < 32 * 3 + 1; i += add)
							{
								f = 0.005;
								if(i % 4 == 0)
									f = 0.01;
								if(i % 16 == 0)
									f = 0.02;
								r_primitive_line_2d(x - 0.45 + (float)i * 0.9 / 96.0, y + 0.7, x - 0.45 + (float)i * 0.9 / 96.0, y + 0.7 + f, 0.5, 0.5, 0.5, 0.5);
								r_primitive_line_2d(x - 0.45 + (float)i * 0.9 / 96.0, y - 0.2, x - 0.45 + (float)i * 0.9 / 96.0, y - 0.2 - f, 0.5, 0.5, 0.5, 0.5);
								r_primitive_line_2d(x - 0.45, y - 0.2 + (float)i * 0.9 / 96.0, x - 0.45 - f, y - 0.2 + (float)i * 0.9 / 96.0, 0.5, 0.5, 0.5, 0.5);
								r_primitive_line_2d(x + 0.45, y - 0.2 + (float)i * 0.9 / 96.0, x + 0.45 + f, y - 0.2 + (float)i * 0.9 / 96.0, 0.5, 0.5, 0.5, 0.5);
							}
						}

						if(type->type == C_TYPE_UV_SECTION)
						{
							r_primitive_line_2d(x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 0] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 1] * 0.3,
												x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 2] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 3] * 0.3, 1, 1, 1, 0.25);
							r_primitive_line_2d(x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 2] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 3] * 0.3,
												x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 4] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 5] * 0.3, 1, 1, 1, 0.25);
							r_primitive_line_2d(x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 4] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 5] * 0.3,
												x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 6] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 7] * 0.3, 1, 1, 1, 0.25);
							r_primitive_line_2d(x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 6] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 7] * 0.3,
												x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + 0] * 0.3,
												y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + 1] * 0.3, 1, 1, 1, 0.25);

							for(i = 0; i < 8; i += 2)
							{
								down_left[0] = x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + i] * 0.3;
								down_left[1] = y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + i + 1] * 0.3;
								down_left[2] = 0;

								if(seduce_manipulator_point_plane(input, NULL, down_left, &stack[type->pos + (type->selected % type->size) * 8 + 1 + i], NULL, FALSE, 2, 1))
								{
									stack[type->pos + (type->selected % type->size) * 8 + i] = (down_left[0] - x + 0.15) / 0.3;
									stack[type->pos + (type->selected % type->size) * 8 + i + 1] = (down_left[1] - y - 0.10) / 0.3;
									if(grid != 0)
									{
										stack[type->pos + (type->selected % type->size) * 8 + i] = f_snapf(stack[type->pos + (type->selected % type->size) * 8 + i], 1.0 / 32.0 * add);
										stack[type->pos + (type->selected % type->size) * 8 + i + 1] = f_snapf(stack[type->pos + (type->selected % type->size) * 8 + i + 1], 1.0 / 32.0 * add);						
										down_left[0] = x - 0.15 + stack[type->pos + (type->selected % type->size) * 8 + i] * 0.3;
										down_left[1] = y + 0.1 + stack[type->pos + (type->selected % type->size) * 8 + i + 1] * 0.3;
									}
									r_primitive_line_2d(down_left[0], y - 0.2, down_left[0], y + 0.7, 1, 1, 1, 0.25);
									r_primitive_line_2d(x - 0.45,  down_left[1], x + 0.45, down_left[1], 1, 1, 1, 0.25);
								}
							}
						}else
						{
							down_left[0] = x - 0.15 + stack[type->pos + (type->selected % type->size) * 2] * 0.3;
							down_left[1] = y + 0.1 + stack[type->pos + (type->selected % type->size) * 2 + 1] * 0.3;
							down_left[2] = 0;

							if(seduce_manipulator_point_plane(input, NULL, down_left, &stack[type->pos + (type->selected % type->size) * 2 + 1], NULL, FALSE, 2, 1))
							{
								stack[type->pos + (type->selected % type->size) * 2] = (down_left[0] - x + 0.15) / 0.3;
								stack[type->pos + (type->selected % type->size) * 2 + 1] = (down_left[1] - y - 0.10) / 0.3;
								if(grid != 0)
								{
									stack[type->pos + (type->selected % type->size) * 2] = f_snapf(stack[type->pos + (type->selected % type->size) * 2], 1.0 / 32.0 * add);
									stack[type->pos + (type->selected % type->size) * 2 + 1] = f_snapf(stack[type->pos + (type->selected % type->size) * 2 + 1], 1.0 / 32.0 * add);		
								}
							}
						}
						r_primitive_line_flush();



						f = ((float)grid) / 5.99;
						if(seduce_widget_slider_radial(input, &grid, &f, x + 0.2, y + 0.15, 0.025, scale * 0.25, 0, 1, time, NULL))
							grid = f * 6.0;
					}
				}
			break;
			case C_TYPE_IMAGE_2D :
			case C_TYPE_IMAGE_3D :
			case C_TYPE_IMAGE_CUBE :
				{
					uint *istack;
					istack = (uint*)&stack[type->pos + (type->selected % type->size)];
					seduce_background_image_draw(input, istack, x - 0.025, y - 0.025, 0, 0.05, 0.05, 0, 0, 1, 1, time, NULL, *istack);
	
					if(seduce_widget_icon_button_invisible(input, &stack[type->pos + (type->selected % type->size)], x, y, 0.025))
					{
						if(expanded == type)
							expanded = NULL;
						else
							expanded = type;
					}
					if(expanded == type)
					{
						char *file;
						file = betray_requester_load_get(c_load_texture);
						if(file != NULL)
							c_load_texture(file);
						if(input->mode == BAM_DRAW)
						{
							float x_pos, y_pos;
							x_pos = (float)(0 % 5) * 0.12 - 0.3;
							y_pos = (float)(0 / 5) * 0.12 + y + 0.1;
							r_primitive_line_2d(x_pos, y_pos, x_pos, y_pos + 0.1, 0.2, 0.2, 0.2, time);
							r_primitive_line_2d(x_pos, y_pos + 0.1, x_pos + 0.1, y_pos + 0.1, 0.2, 0.2, 0.2, time);
							r_primitive_line_2d(x_pos + 0.1, y_pos + 0.1, x_pos + 0.1, y_pos, 0.2, 0.2, 0.2, time);
							r_primitive_line_2d(x_pos + 0.1, y_pos, x_pos, y_pos, 0.2, 0.2, 0.2, time);

							r_primitive_line_2d(x_pos, y_pos, x_pos + 0.1, y_pos + 0.1, 0.2, 0.2, 0.2, time);
							r_primitive_line_2d(x_pos, y_pos + 0.1, x_pos + 0.1, y_pos, 0.2, 0.2, 0.2, time);
							r_primitive_line_flush();
						}
						if(seduce_text_button(input, c_load_texture, (float)(0 % 5) * 0.12 - 0.3, (float)(0 / 5) * 0.12 + y + 0.1, 0.5, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "Load...", 1, 1, 1, 0.5, 0.2, 0.6, 1, 1))
						{
							betray_requester_load(NULL, 0, c_load_texture);
						}
						for(i = 0; i < c_texture_count; i++)
						{
							if(input->mode == BAM_DRAW)
							{
								float aspect;
								aspect = (float)c_textures[i].size[1] / (float)c_textures[i].size[0];
								if(aspect < 1.0)
									seduce_background_image_draw(input, &c_textures[i], (float)((i + 1) % 5) * 0.12 - 0.3, (float)((i + 1) / 5) * 0.12 + y + 0.1 + 0.05 * (1.0 - aspect), 0, 0.1, 0.1 * aspect, 0, 0, 1, 1, time, NULL, c_textures[i].texture_id);
								else
									seduce_background_image_draw(input, &c_textures[i], (float)((i + 1) % 5) * 0.12 - 0.3 + 0.05 * (1.0 - 1.0 / aspect), (float)((i + 1) / 5) * 0.12 + y + 0.1, 0, 0.1 / aspect, 0.1, 0, 0, 1, 1, time, NULL, c_textures[i].texture_id);
							//	seduce_element_add_point(BInputState *input, void *id, uint part, float *pos, float size);
							}
							else if(seduce_widget_icon_button_invisible(input, &c_textures[i], x, y, 0.01))
							{
								*istack = c_textures[i].texture_id;
								for(j = 0; j < 31 && c_textures[i].name[j]; j++)
									type->name[j] = c_textures[i].name[j];
								type->name[j] = 0;
								expanded = NULL;
							}
						}
					}
				}
			break;
			case C_TYPE_OBJECT :
				{
					uint *istack;
					istack = (uint*)&stack[type->pos + (type->selected % type->size) * 3];
					if(input->mode == BAM_DRAW)
					{
						float pos_a_x = 0.015, pos_a_y = 0.015, pos_a_z = 0.015;
						float pos_b_x = -0.015, pos_b_y = -0.015, pos_b_z = -0.015;
						r_matrix_push(NULL);
						r_matrix_translate(NULL, x, y, 0);
						r_matrix_rotate(NULL, input->minute_time * 360 * 10.0, 1, 1, 1);
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
						r_matrix_pop(NULL);
					}
					if(seduce_widget_icon_button_invisible(input, istack, x, y, 0.01))
					{
						if(expanded == type)
							expanded = NULL;
						else
							expanded = type;
					}
					if(expanded == type)
					{
						uint count;
						CPreviewGeometry *g;
						c_editor_verse_preview_update();
						g = c_editor_verse_preview_next(NULL);
						for(count = 0; g != NULL; count++)
							g = c_editor_verse_preview_next(g);
						g = c_editor_verse_preview_next(NULL);
						for(i = 0; g != NULL; i++)
						{
							if(c_editor_popup_icon(input, &istack[1], y + 0.1, 1,  count, i, g->name, g, time))
							{
								expanded = NULL;
								for(j = 0; j < 31 && g->name[j]; j++)
									type->name[j] = g->name[j];
								type->name[j] = 0;
								istack[0] = g->node_id;
								istack[1] = 0;
								istack[2] = 0;
							}
							g = c_editor_verse_preview_next(g);
						}
						/*
						for(i = 0; g != NULL; i++)
						{

							if(seduce_widget_icon_button_invisible(input, g, x, y))
							{
								exit(0);
								istack[0] = g->node_id;
								istack[1] = 0; // FIX ME
								istack[2] = 3; 
							}
							g = c_editor_verse_preview_next(g);
						}*/
					}
				}
			break;
			case C_TYPE_STATE_DEPTH_TEST :
			{
				boolean toggle;
				uint *istack;
				istack = (uint*)&stack[type->pos + (type->selected % type->size)];
				toggle = *istack;
				if(seduce_widget_icon_toggle(input, istack, &toggle, SUI_3D_OBJECT_SELECT, x, y, 0.05, time))
					*istack = toggle;
			}
			break;
			case C_TYPE_STATE_BLEND_SOURCE :
			{
				static uint blend_mode_source[] = {GL_ZERO, GL_ONE, GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA_SATURATE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA};
				static char *blend_mode_source_names[] = {"ZERO", "ONE", "DST_COLOR", "ONE_MINUS_DST_COLOR", "SRC_ALPHA_SATURATE", "SRC_ALPHA", "ONE_MINUS_SRC_ALPHA", "DST_ALPHA", "ONE_MINUS_DST_ALPHA"};
				uint *istack;
				istack = (uint*)&stack[type->pos + (type->selected % type->size)];
				for(i = 0; blend_mode_source[i] != *istack && i < 7; i++);
			//	seduce_text_button(input, &c_primitives[i].blend_source, 0.315, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, c_blend_mode_source_names[j], 0.5, 0.5, 0.5, 1, 1, 1, 1, 1);
				if(seduce_widget_select_radial(input, istack, &i, blend_mode_source_names, 8, S_PUT_ANGLE, x, y, 0.05, 1, time, FALSE))
					*istack = blend_mode_source[i];
			}
			break;
			case C_TYPE_STATE_BLEND_DESTINATION :
			{
				static uint blend_mode_dest[] = {GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA};
				static char *blend_mode_dest_names[] = {"ZERO", "ONE", "SRC_COLOR", "ONE_MINUS_SRC_COLOR", "SRC_ALPHA", "ONE_MINUS_SRC_ALPHA", "DST_ALPHA", "ONE_MINUS_DST_ALPHA"};
				uint *istack;
				istack = (uint*)&stack[type->pos + (type->selected % type->size)];
				for(i = 0; blend_mode_dest[i] != *istack && i < 7; i++);
				if(seduce_widget_select_radial(input, istack, &i, blend_mode_dest_names, 8, S_PUT_ANGLE, x, y, 0.05, 1, time, FALSE))
					*istack = blend_mode_dest[i];
			}
			break;
			case C_TYPE_BOOLEAN :
			{
				boolean toggle;
				toggle = stack[type->pos + (type->selected % type->size)] <= 1.0 &&
						stack[type->pos + (type->selected % type->size)] >= 0.0;
				if(seduce_widget_icon_toggle(input, &stack[type->pos + (type->selected % type->size)], &toggle, SUI_3D_OBJECT_SELECT, x, y, 0.05, time))
				{
					if(toggle)
						stack[type->pos + (type->selected % type->size)] = 0.5;
					else
						stack[type->pos + (type->selected % type->size)] = 1.5;
				}
			}
			break;

		}
	}
	param.entity = entity;
	param.ref = ref;
	param.type = type;
	param.base_type = base_type;
	param.session = session;
	seduce_popup_detect_icon(input, ref, SUI_3D_OBJECT_LOCK, x, y + 0.05, 0.025, time, c_editor_bar_popup_func, &param, TRUE, NULL);
}
