#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "testify.h"
#include "s_draw_3d.h"
#include "c_internal.h"

extern float c_editor_popup_animation_get();
extern void confuse_point_editor_set(CEntity *entity, uint param);
uint c_editor_type_overlay = -1;

uint c_editor_stack_particle_draw(BInputState *input, CSession *session, CEntity *entity)
{
	float *stack, f, f2;
	uint i, j;
	if(session == NULL || entity == NULL)
		return;
	for(i = 0; i < entity->stack_type_count; i++)
	{
		if(entity->stack_types[i].type == C_TYPE_PARTICLE && 0 == c_entity_stack_user_count(session, entity, i, FALSE))
		{
			stack = &entity->stack[entity->stack_types[i].pos];
			for(j = 0; j < entity->stack_types[i].size * 7; j += 7)
			{
				f2 = stack[j + 6];
				f = 1.0 - f2;
				r_primitive_line_fade_3d(stack[j + 0], stack[j + 1], stack[j + 2], 
										 stack[j + 0] - stack[j + 3] * 0.1, 
										 stack[j + 1] - stack[j + 4] * 0.1,
										 stack[j + 2] - stack[j + 5] * 0.1, f * f, f * f2 * 2.0, f2 * f2, 1.0, 0, 0.0, 0.0, 0.0);
			}
		}
	}
}

uint c_editor_stack_popup(BInputState *input, CSession *session, CEntity *entity, uint type)
{
	uint i, part;
	if(input->mode == BAM_DRAW)
		c_editor_type_overlay = type;
	if(input->mode == BAM_EVENT)
	{		
		for(i = 0; i < input->pointer_count; i++)
			if(!input->pointers[0].button[0] && input->pointers[0].last_button[0])
				if(entity->stack == seduce_element_pointer_id(input, i, &part))
					return part;
	}
	return -1;
}

void c_editor_stack(BInputState *input, CSession *session, CEntity *entity)
{
	static boolean active = TRUE,  show_hidden = FALSE;
	static float static_time = 0, scroll = 0;
	static char *type_names[] = {"FLOAT",
								"UV",
								"POS",
								"COLOR",
								"MATRIX",
								"AREA",
								"POS_QUATERNION_SIZE",
								"UV_SECTION",
								"SAME_AS_OUTPUT",
								"IMAGE_2D",
								"IMAGE_3D",
								"IMAGE_CUBE",
								"OBJECT",
								"STATE_DEPTH_TEST",
								"STATE_BLEND_SOURCE",
								"STATE_BLEND_DESTINATION",
								"PARTICLES",
								"BOOLEAN"};
	char string[128];
	float pos = 0, name_size = 0.1, number_size, type_size, f, time, brightness, a[3], b[3], c[3], d[3], aspect;
	uint i, j, k, param_count, count, size, part, stack_visible;
	if(entity == NULL)
		return;
	seduce_animate(input, &static_time, active, 2.0);
	time = static_time * c_editor_popup_animation_get();
	time = 1.0;
	name_size = number_size = type_size = 0.0;
	
	if(input->mode == BAM_DRAW)
	{
		for(i = 0; i < entity->stack_type_count; i++)
		{
			f = seduce_text_line_length(SEDUCE_T_SIZE, SEDUCE_T_SPACE, entity->stack_types[i].name, -1);
			if(name_size < f)
				name_size = f;
			sprintf(string, "%u", entity->stack_types[i].size);
			f = seduce_text_line_length(SEDUCE_T_SIZE, SEDUCE_T_SPACE, string, -1);
			if(number_size < f)
				number_size = f;
			f = seduce_text_line_length(SEDUCE_T_SIZE, SEDUCE_T_SPACE, type_names[entity->stack_types[i].type], -1);
			if(type_size < f)
				type_size = f;
		}
		r_matrix_push(NULL);
		r_matrix_translate(NULL, -1.0, 0, 0);
		r_matrix_rotate(NULL, 60.0 * (c_editor_popup_animation_get() - 1.0), 0, 1.0, 0);
		r_matrix_translate(NULL, 1.0, 0, 0);
	}


	aspect = betray_screen_mode_get(NULL, NULL, NULL); 
	stack_visible = 0;
	if(show_hidden)
		stack_visible = entity->stack_type_count;
	else for(i = 0; i < entity->stack_type_count; i++)
		if(entity->stack_types[i].visible)
			stack_visible++;




	if(stack_visible * 0.01 < aspect)
		scroll = 0.0;
	else
	{
		if(betray_button_get(-1, BETRAY_BUTTON_SCROLL_UP))
		{
			if(scroll - (stack_visible + 3) * 0.01 > -aspect)
				scroll = -aspect + (stack_visible + 3) * 0.01;
			scroll += 0.02;
		}
		if(betray_button_get(-1, BETRAY_BUTTON_SCROLL_DOWN))
		{
			if(scroll + (stack_visible + 2) * 0.01 < aspect)
				scroll = aspect - (stack_visible + 2) * 0.01;
			scroll -= 0.02;
		}
	}
	

	pos = scroll + 0.01 * (float)stack_visible;
	
	for(i = 0; i < entity->stack_type_count; i++)
	{
		if(entity->stack_types[i].visible || show_hidden)
		{
			part = -1;
			for(j = 0; j < input->pointer_count; j++)
				if(entity->stack == seduce_element_pointer_id(input, j, &part))
					break;
			if(j == input->pointer_count)
				part = -1;
			brightness = 0.2;
			if(c_editor_type_overlay == -1 || c_entity_types_compatible(entity->stack_types[i].type, c_editor_type_overlay))
				brightness = 0.6;
			if(part == i)
				brightness = 1.0;	

			for(j = 0; j < input->pointer_count; j++)
				if(&entity->stack_types[i] == seduce_element_pointer_id(input, j, &part))
					break;
			if(j != input->pointer_count)
				brightness = 1.0;	

			seduce_text_line_edit(input, entity->stack_types[i].name, entity->stack_types[i].name, 32, -0.98, pos, 0.24, SEDUCE_T_SIZE, "Name", TRUE, NULL, NULL, 1, 1, 1, brightness, 1, 1, 1, brightness);
			j = entity->stack_types[i].size;
			if(seduce_text_uint_edit(input, &entity->stack_types[i].size, &j, -0.95 + name_size, pos, 0.1, SEDUCE_T_SIZE, TRUE, NULL, NULL, 1, 1, 1, brightness, 1, 1, 1, brightness) && entity->stack_types[i].size != j)
				c_entity_stack_resize(entity, i, j);
		
			if(entity->stack_types[i].type == C_TYPE_COLOR)
				seduce_text_line_draw(-0.93 + name_size + number_size, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "COLOR", entity->stack[entity->stack_types[i].pos], entity->stack[entity->stack_types[i].pos + 1], entity->stack[entity->stack_types[i].pos + 2], brightness, -1);
			else
				seduce_text_line_draw(-0.93 + name_size + number_size, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, type_names[entity->stack_types[i].type], 1, 1, 1, brightness, -1);
		
			count = c_entity_stack_user_count(session, entity, i, FALSE);
		
			if(count == 0 && entity->stack_types[i].dependency == -1)
			{
				float color[4] = {1, 0, 0, 1}; 
				if(seduce_widget_icon_button(input, &entity->stack_types[i].pos, SUI_3D_OBJECT_MINUS, -0.91 + name_size + number_size + type_size, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, color))
				{
					c_entity_stack_remove(session, entity, i);
					return;
				}
			}else if(input->mode == BAM_DRAW)
			{
				sprintf(string, "%u", count);
				seduce_text_line_draw(-0.91 + name_size + number_size + type_size, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, string, 1, 1, 1, brightness, -1);
			}
			{
				float color[4] = {0.2, 0.6, 1, 1}; 
				if(seduce_widget_icon_button(input, &entity->stack_types[i].dependency, SUI_3D_OBJECT_BOOST, -0.89 + name_size + number_size + type_size, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1, color))
					confuse_point_editor_set(entity, i);
			}
			seduce_widget_icon_toggle(input, &entity->stack_types[i].visible, &entity->stack_types[i].visible, SUI_3D_OBJECT_CHECKBOXCHECKED, -0.87 + name_size + number_size + type_size, pos + 0.005 + SEDUCE_T_SIZE * 0.25, 0.015, 1.0);
			pos -= 0.02;
		}
	}
	if(input->mode == BAM_DRAW)
	{
		if(entity->stack_type_count == 0)
		{
			seduce_text_line_draw(-0.98, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, "stack is empty", 1, 1, 1, 1, -1);
		}else
		{
			sprintf(string, "%u stack entries taking up %u bytes", entity->stack_type_count, entity->stack_size * 4);
			seduce_text_line_draw(-0.98, pos, SEDUCE_T_SIZE, SEDUCE_T_SPACE, string, 1, 1, 1, 1, -1);
		}
	}
	{
		static char *texts[3] = {"Load", "Prune", NULL};
		char *path;
		uint output;
		pos -= 0.02;
		if(show_hidden)
			texts[2] = "Hide";
		else
			texts[2] = "Show hidden";

		output = seduce_text_button_list(input, input, -0.98, pos, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, texts, 3, 1, 1, 1, 0.5, 1, 1, 1, 1);
		if(output == 0)
			betray_requester_load(NULL, 0, texts); 
		if(output == 1)
		{
			for(i = 0; i < entity->stack_type_count; i++)
			{
				count = c_entity_stack_user_count(session, entity, i, FALSE);
				if(count == 0 && entity->stack_types[i].dependency == -1)
					c_entity_stack_remove(session, entity, i--);
			}
		}
		if(output == 2)
			show_hidden = !show_hidden;

		path = betray_requester_load_get(texts);
		if(path != NULL)
		{
			CTypeType types[5] = {0, C_TYPE_FLOAT, C_TYPE_UV, C_TYPE_POS, C_TYPE_COLOR};
			uint component_size[255];
			uint component_stack_pos[255];
			char string[128];
			uint vertex_count, component_count;
			float value;
			THandle *h;

			h = testify_file_load(path);

			vertex_count = testify_unpack_uint32(h, "vertex_length");
			component_count = (uint)testify_unpack_int8(h, "component_count");
			for(i = 0; i < component_count; i++)
			{
				testify_unpack_string(h, string, 128, "component_name");
				component_size[i] = testify_unpack_int8(h, "component_count");
				if(component_size[i] < 5)
					component_stack_pos[i] = c_entity_stack_add(entity, types[component_size[i]], string, vertex_count);
			}
			for(i = 0; i < vertex_count; i++)
				for(j = 0; j < component_count; j++)
					for(k = 0; k < component_size[j]; k++)
					{
						uint a, b, c;
						a = component_stack_pos[j];
						b = entity->stack_types[component_stack_pos[j]].pos;
						c = entity->stack_types[component_stack_pos[j]].pos + i * component_size[j] + k;
						value = testify_unpack_real32(h, "data");
						if(value == value)
							entity->stack[entity->stack_types[component_stack_pos[j]].pos + i * component_size[j] + k] = value;
						else
							entity->stack[entity->stack_types[component_stack_pos[j]].pos + i * component_size[j] + k] = 0;
					}
			testify_free(h);
		}
	}
	if(input->mode == BAM_DRAW)
	{
		pos = scroll + 0.01 * (float)stack_visible + 0.01 + SEDUCE_T_SIZE * 0.5;
		for(i = 0; i < stack_visible; i++)
		{
			if(c_editor_type_overlay != -1 && c_entity_types_compatible(entity->stack_types[i].type, c_editor_type_overlay))
			{
				a[2] = b[2] = c[2] = d[2] = 0.0;
				a[0] = d[0] = -1;
				a[1] = b[1] = pos - 0.02;
				b[0] = c[0] = -0.88 + name_size + number_size + type_size;
				c[1] = d[1] = pos;
				seduce_element_add_quad(input, entity->stack, i, a, b, c, d);
			}
			r_primitive_line_2d(-0.99, pos, -0.88 + name_size + number_size + type_size, pos, 0.25, 0.25, 0.25, 1);
			
			r_primitive_line_2d(-0.96 + name_size, pos - 0.005, -0.96 + name_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
			r_primitive_line_2d(-0.94 + name_size + number_size, pos - 0.005, -0.94 + name_size + number_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
			r_primitive_line_2d(-0.92 + name_size + number_size + type_size, pos - 0.005, -0.92 + name_size + number_size + type_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
			r_primitive_line_2d(-0.90 + name_size + number_size + type_size, pos - 0.005, -0.90 + name_size + number_size + type_size, pos - 0.015, 0.25, 0.25, 0.25, 1);
		
			pos -= 0.02;
		}
		r_primitive_line_2d(-0.99, pos, -0.88 + name_size + number_size + type_size, pos, 0.25, 0.25, 0.25, 1);
		r_primitive_line_flush();
		r_matrix_pop(NULL);
		c_editor_type_overlay = -1;
	}
}