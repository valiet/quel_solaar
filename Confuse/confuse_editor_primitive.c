#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "s_draw_3d.h"
#include "c_internal.h"

extern float c_editor_popup_animation_get();
void c_editor_popup_entity_create_replace(uint replaced_primitive, char *vertex_path, char *fragment_path);

void c_editor_component(BInputState *input, CSession *session, CEntity **entity, CEntity *draw_entity)
{
	static boolean active = TRUE;
	static float static_time = 0;
	static char *category_names[] = {"INITIALIZE", "PROCESS", "TIMER", "OBJECT", "DRAW", "PRIMITIVE", "PRIMITIVE STATE", "FORCE"};
	float pos = 0, name_size = 0.1, f, time;
	uint i, j, k, param_count, count, type_count[C_CC_COUNT], size;
	float color[4] = {1, 0, 0, 1}; 
	if(entity == NULL || *entity == NULL)
		return;
	seduce_animate(input, &static_time, active, 2.0);
	time = static_time * c_editor_popup_animation_get();
	time = 1.0;
	name_size = 0.0;
	
	for(i = 0; i < C_CC_COUNT; i++)
		type_count[i] = 0;
	for(i = 0; i < (*entity)->component_count; i++)
		type_count[session->component_list[(*entity)->components[i].type].type]++;

	count = (*entity)->component_count;
	for(i = 0; i < C_CC_COUNT; i++)
		if(type_count[i] != 0)
			count++;

	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < C_CC_COUNT; i++)
		{
			if(type_count[i] != 0)
			{
				f = seduce_text_line_length(SEDUCE_T_SIZE, SEDUCE_T_SPACE, category_names[i], -1);
				if(name_size < f)
					name_size = f;
			}
		}
		for(i = 0; i < (*entity)->component_count; i++)
		{
			f = seduce_text_line_length(SEDUCE_T_SIZE, SEDUCE_T_SPACE, session->component_list[(*entity)->components[i].type].name, -1);
			if(name_size < f)
				name_size = f;
		}
	//	r_matrix_push(NULL);
	/*	r_matrix_translate(NULL, 1.0, 0, 0);
		r_matrix_rotate(NULL, 60.0 * (c_editor_popup_animation_get() - 1.0), 0, 1.0, 0);
		r_matrix_translate(NULL, -1.0, 0, 0);*/
	}
//	if(input->mode == BAM_DRAW)
//		r_matrix_pop(NULL);


	pos = 0.01 * (float)count;


	if(*entity != draw_entity)
	{
		char name[128];
		sprintf(name, "Return to %s", draw_entity->name);
		if(seduce_text_button(input, &category_names, 0.96 - name_size, pos + 0.02, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, name, 0.7, 0.7, 0.7, 1, 1, 1, 1, 1))
			*entity = draw_entity;
	}else
		seduce_text_line_edit(input, (*entity)->name, (*entity)->name, 32, 0.96 - name_size, pos + 0.02, 0.24, SEDUCE_T_SIZE, "Name", TRUE, NULL, NULL, 0.8, 0.8, 0.8, 1, 1, 1, 1, 1);
	for(i = 0; i < C_CC_COUNT; i++)
	{
		if(type_count[i] != 0)
		{
			if(input->mode == BAM_DRAW)
				seduce_text_line_draw(0.94 - name_size, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, category_names[i], 0.5, 0.5, 0.5, 1, -1);
			pos -= 0.02;
			for(j = 0; j < (*entity)->component_count; j++)
			{
				if(session->component_list[(*entity)->components[j].type].type == i)
				{
					if(seduce_text_button(input, &(*entity)->components[j].special, 0.96 - name_size, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, session->component_list[(*entity)->components[j].type].name, 0.7, 0.7, 0.7, 1, 1, 1, 1, 1))
						(*entity)->component_selected = j;
					if(seduce_widget_icon_button(input, &(*entity)->components[j].type, SUI_3D_OBJECT_MINUS, 0.98, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, color))
					{	
						c_entity_components_remove(session, (*entity), j, TRUE);
						if((*entity)->component_selected == j)
							(*entity)->component_selected = -1;
						return;
					}

					if(i == C_CC_DRAW)
					{
						float color[4] = {0.2, 0.6, 1, 1}; 
						if(seduce_widget_icon_button(input, &session->primitive_settings.components[(*entity)->components[j].type], SUI_3D_OBJECT_BOOST, 0.945 - name_size, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, color))
						{
							session->primitive_settings.component_selected = -1;
							for(k = 0; k < session->primitive_settings.component_count &&
								session->component_list[session->primitive_settings.components[k].type].primitive != session->component_list[(*entity)->components[j].type].primitive; k++);
							if(k < session->primitive_settings.component_count)
							{
								session->primitive_settings.component_selected = k;
								*entity = &session->primitive_settings;
							}else
							{
								c_editor_popup_entity_create_replace(session->component_list[(*entity)->components[j].type].primitive, 
									session->prim[session->component_list[(*entity)->components[j].type].primitive].shader_paths[0], 
									session->prim[session->component_list[(*entity)->components[j].type].primitive].shader_paths[1]);
							}
						}
					}
					if(i == C_CC_PRIMITIVE_STATE)
					{
						float color[4] = {0.2, 0.6, 1, 1}; 
						if(seduce_widget_icon_button(input, &session->primitive_settings.components[(*entity)->components[j].type], SUI_3D_OBJECT_BOOST, 0.945 - name_size, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, color))
						{
							c_editor_popup_entity_create_replace(session->component_list[(*entity)->components[j].type].primitive, 
								session->prim[session->component_list[(*entity)->components[j].type].primitive].shader_paths[0], 
								session->prim[session->component_list[(*entity)->components[j].type].primitive].shader_paths[1]);
						}
					}
					pos -= 0.02;
				}
			}
		}
	}
	if(input->mode == BAM_DRAW)
	{
		if(seduce_text_button(input, &category_names, 0.96 - name_size, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "Add...", 0.7, 0.7, 0.7, 1, 1, 1, 1, 1))
			;
	}
	if(input->mode == BAM_DRAW)
	{
		pos = 0.01 * (float)count + 0.01 + SEDUCE_T_SIZE * 0.5;
		for(i = 0; i < count; i++)
		{
			r_primitive_line_2d(0.99, pos, 0.94 - name_size, pos, 0.25, 0.25, 0.25, 1);
			r_primitive_line_2d(0.97, pos - 0.005, 0.97, pos - 0.015, 0.25, 0.25, 0.25, 1);
			pos -= 0.02;
		}
		r_primitive_line_2d(0.99, pos, 0.96 - name_size, pos, 0.25, 0.25, 0.25, 1);
		r_primitive_line_flush();
		r_matrix_pop(NULL);
	}
}





//Name, Source, dest, Textures, buffer_size;

#ifdef FIX_THIS_SOME_DAY


void c_editor_primitive(BInputState *input)
{
	static uint requester_active = -1;
	static char *shader_paths[2] = {NULL, NULL};
	static boolean active = FALSE;
	static float time = 0;
	char string[128], *unit_names[2] = {"Vertex", "Fragment"};
	float pos = 0, name_size = 0.1, buffer_size, type_size, f;
	uint i, j, k, param_count, count;

	seduce_animate(input, &time, active, 2.0);

	if(requester_active != -1)
	{
		char *file, *copy;
		file = betray_requester_load_get();
		if(file != NULL)
		{
			if(shader_paths[requester_active] != NULL)
				free(shader_paths[requester_active]);
				shader_paths[requester_active] = f_text_copy_allocate(file);
			if(shader_paths[0] != NULL && shader_paths[1] != NULL)
			{
				CDrawState *p;
				p = c_primitive_add();
				p->shader_paths[0] = shader_paths[0];
				p->shader_paths[1] = shader_paths[1];
				c_primitive_compile(p);
				c_primitive_inject_uniform(p);
				shader_paths[0] = NULL;
				shader_paths[1] = NULL;
			}
			requester_active = -1;
		}
	}

	for(i = 0; i < c_primitive_count; i++)
	{
		seduce_text_line_edit(input, c_primitives[i].name, c_primitives[i].name, 32, 0.0, pos, 0.1, SEDUCE_T_SIZE, "Name", TRUE, NULL, NULL, 1, 1, 1, 1, 1, 1, 1, 1);
		j = c_primitives[i].buffer_size;
		if(seduce_text_uint_edit(input, &c_primitives[i].buffer_size, &c_primitives[i].buffer_size, 0.1, pos, 0.1, SEDUCE_T_SIZE, NULL, NULL, 1, 1, 1, 1, 1, 1, 1, 1) &&  j != c_primitives[i].buffer_size)
		{
			if(c_primitives[i].buffer != NULL)
				free(c_primitives[i].buffer);
			c_primitives[i].buffer = malloc(c_primitives[i].buffer_stride * c_primitives[i].buffer_size);
		}
	/*	
		for(j = 0; c_blend_mode_dest[j] != c_primitives[i].blend_destination; j++);
		seduce_text_button(input, &c_primitives[i].blend_destination, 0.215, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, c_blend_mode_dest_names[j], 0.5, 0.5, 0.5, 1, 1, 1, 1, 1);
		if(seduce_widget_select_radial(input, &c_primitives[i].blend_destination, &j, c_blend_mode_dest_names, 8, S_PUT_ANGLE, 0.2, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, 1, FALSE))
			c_primitives[i].blend_destination = c_blend_mode_dest[j];


		for(j = 0; c_blend_mode_source[j] != c_primitives[i].blend_source; j++);
		seduce_text_button(input, &c_primitives[i].blend_source, 0.315, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, c_blend_mode_source_names[j], 0.5, 0.5, 0.5, 1, 1, 1, 1, 1);
		if(seduce_widget_select_radial(input, &c_primitives[i].blend_source, &j, c_blend_mode_source_names, 8, S_PUT_ANGLE, 0.3, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, 1, FALSE))
			c_primitives[i].blend_source = c_blend_mode_source[j];

			*/

		seduce_widget_icon_toggle(input, &c_primitives[i].depth_test, &c_primitives[i].depth_test, SUI_3D_OBJECT_HIGHLIGHT, 0.4, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1);


	/*	if(entity->stack_types[i].type == C_TYPE_COLOR)
			seduce_text_line_draw(-0.93 + name_size + number_size, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "COLOR", entity->stack[entity->stack_types[i].pos], entity->stack[entity->stack_types[i].pos + 1], entity->stack[entity->stack_types[i].pos + 2], 1, -1);
		else
			seduce_text_line_draw(-0.93 + name_size + number_size, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, type_names[entity->stack_types[i].type], 1, 1, 1, 1, -1);
		count = 0;
		for(j = 0; j < entity->component_count; j++)
		{
			param_count = confuse_component_param_count_init_get(entity->components[j].type);
			for(k = 0; k < param_count; k++)
				if(entity->components[j].references[k] == i)
					count++;
		}
		
		if(count == 0)
		{
			float color[4] = {1, 0, 0, 1}; 
			if(seduce_widget_icon_button(input, &entity->stack_types[i].pos, SUI_3D_OBJECT_MINUS, -0.91 + name_size + number_size + type_size, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, color))
			{
				count = entity->stack_types[i].size * c_type_size_get(entity->stack_types[i].type);
				for(j = entity->stack_types[i].pos; j < entity->stack_size - count; j++)
					entity->stack[j] = entity->stack[j + count];

				entity->stack_type_count--;
				for(j = i; j < entity->stack_type_count; j++)
				{
					entity->stack_types[i] = entity->stack_types[i + 1];
					entity->stack_types[i].pos -= count;
				}
				for(j = 0; j < entity->component_count; j++)
				{
					param_count = confuse_primitive_param_count(entity->components[j].type);
					for(k = 0; k < param_count; k++)
						if(entity->components[j].references[k] > i)
								entity->components[j].references[k]--;
				}
			}
		}else if(input->mode == BAM_DRAW)
		{
			sprintf(string, "%u", count);
			seduce_text_line_draw(-0.91 + name_size + number_size + type_size, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, string, 1, 1, 1, 1, -1);
		}
		{
			float color[4] = {0.2, 0.6, 1, 1}; 
			seduce_widget_icon_button(input, &entity->stack_types[i].dependency, SUI_3D_OBJECT_BOOST, -0.89 + name_size + number_size + type_size, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, color);
		}
		*/
		pos -= 0.02;
	}
	if(input->mode == BAM_DRAW)
	{
		if(c_primitive_count == 0)
		{
			seduce_text_line_draw(-0.98, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "No primitives defined.", 1, 1, 1, 1, -1);
		}else
		{
			sprintf(string, "%u primitives", c_primitive_count);
			seduce_text_line_draw(-0.98, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, string, 1, 1, 1, 1, -1);
		}
	}
//	if(seduce_widget_icon_button(input, &c_primitive_count, SUI_3D_OBJECT_PLUS, 0.99, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, NULL))
		
	for(i = 0; i < 2; i++)
	{
		if((shader_paths[i] != NULL && seduce_text_button(input, &shader_paths[i], 0.92 - 0.1 * (float)i, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, unit_names[i], 0.5, 0.5, 0.5, 1, 1, 1, 1, 1)) ||
			(shader_paths[i] == NULL && seduce_text_button(input, &shader_paths[i], 0.92 - 0.1 * (float)i, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, unit_names[i], 0.2, 0.6, 1, 1, 1, 1, 1, 1)))
		{
			betray_requester_load(NULL, 0);
			requester_active = i;
		}
	}	

/*	if(input->mode == BAM_DRAW)
	{
		pos = 0 + 0.01 + SEDUCE_T_SIZE * 0.5;
		for(i = 0; i < entity->stack_type_count; i++)
		{
			r_primitive_line_2d(-0.99, pos, -0.88 + name_size + number_size + type_size, pos, 0.25, 0.25, 0.25, 1);
			
			r_primitive_line_2d(-0.96 + name_size, pos - 0.005, -0.96 + name_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
			r_primitive_line_2d(-0.94 + name_size + number_size, pos - 0.005, -0.94 + name_size + number_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
			r_primitive_line_2d(-0.92 + name_size + number_size + type_size, pos - 0.005, -0.92 + name_size + number_size + type_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
			r_primitive_line_2d(-0.90 + name_size + number_size + type_size, pos - 0.005, -0.90 + name_size + number_size + type_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
		
			pos -= 0.02;
		}
		r_primitive_line_2d(-0.99, pos, -0.88 + name_size + number_size + type_size, pos, 0.25, 0.25, 0.25, 1);
		r_primitive_line_flush();
	}*/
}

#endif