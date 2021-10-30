#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "seduce_editor.h"
#include "s_draw_3d.h"
#include "testify.h"

void seduce_editor_save_function(FILE *f, char *name)
{
	boolean added = FALSE;
	boolean matrix = TRUE;
	char *surface_id = "NULL";
	uint i, j, count, counters[S_EWT_COUNT];
	fprintf(f, "#include <math.h>\n");
	fprintf(f, "#include <stdlib.h>\n");
	fprintf(f, "#include <stdio.h>\n");
	fprintf(f, "#include \"betray.h\"\n");
	fprintf(f, "#include \"seduce.h\"\n\n");
	fprintf(f, "void %s(BInputState *input, void *user_pointer)\n{\n", name);
	fprintf(f, "\tfloat timer = 1.0;\n");

	if(matrix)
	{
		fprintf(f, "\tstatic float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -0.3, 1};\n");
		fprintf(f, "\tRMatrix *matrix;\n");
		surface_id = "m";
	}

// seduce_background_shape_matrix_interact(input, void *id, float *matrix, boolean scale, boolean rotate);

	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_WIDGET_ICON_BUTTON)
			count++;
	if(count != 0)
	{
		fprintf(f, "\tstatic boolean ids[%u] = {TRUE", count);
		for(i = 1; i < count; i++)
			fprintf(f, ", TRUE");
		fprintf(f, "};\n");
	}
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_TEXT_LINE_EDIT)
			fprintf(f, "\tstatic char text_%u[64] = {0};\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_TEXT_PASSWORD_EDIT)
			fprintf(f, "\tstatic char password_%u[64] = {0};\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_TEXT_DOUBLE_EDIT)
			fprintf(f, "\tstatic double dvalue_%u = 0.0;\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_TEXT_FLOAT_EDIT)
			fprintf(f, "\tstatic float fvalue_%u = 0.0;\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_TEXT_INT_EDIT)
			fprintf(f, "\tstatic int ivalue_%u = 0;\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_TEXT_UINT_EDIT)
			fprintf(f, "\tstatic uint uvalue_%u = 0;\n", count++);

	for(i = 0; i < s_editor_widget_count; i++)
	{	
		if(s_editor_widgets[i].type == S_EWT_TEXT_BUTTON)
		{	
			fprintf(f, "\tchar *t;\n");
			break;
		}		
	}

	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_WIDGET_ICON_BUTTON)
			count++;
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_WIDGET_ICON_TOGGLE)
			fprintf(f, "\tstatic boolean toggle_%u = TRUE;\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_WIDGET_SLIDER_RADIAL)
			fprintf(f, "\tstatic float slider_%u = 0.5;\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
		if(s_editor_widgets[i].type == S_EWT_WIDGET_WHEEL_RADIAL)
			fprintf(f, "\tstatic float color_%u[3] = {0.2, 0.6, 1.0};\n", count++);
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type == S_EWT_TEXT_BUTTON_LIST)
		{
			fprintf(f, "\tstatic char *buttons_%u[] = {NULL};\n", count++);
		}
	}


	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type == S_EWT_WIDGET_SELECT_RADIAL)
		{
			fprintf(f, "\tstatic uint selec_%u = 0;\n", count);
			fprintf(f, "\tstatic char *lable_%u[] = {NULL};\n", count++);
		}
	}
	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type == S_EWT_WIDGET_SELECT_RADIAL)
		{
			if(count == 0)
				fprintf(f, "\tif(lable_0[0] == NULL)\n\t{\n");
			for(j = 0; j < s_editor_widgets[i].data.options.array_length; j++)
				fprintf(f, "\t\tlable_%u[%u] = seduce_translate(\"%s\");\n", count, j, s_editor_widgets[i].data.options.array[j]);
			count++;
		}
	}
	if(count != 0)
		fprintf(f, "\t}\n");

	count = 0;
	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type == S_EWT_TEXT_BUTTON_LIST)
		{
			if(count == 0)
				fprintf(f, "\tif(buttons_0[0] == NULL)\n\t{\n");
			for(j = 0; j < s_editor_widgets[i].data.options.array_length; j++)
				fprintf(f, "\t\tbuttons_%u[%u] = seduce_translate(\"%s\");\n", count, j, s_editor_widgets[i].data.options.array[j]);
			count++;
		}
	}
	if(count != 0)
		fprintf(f, "\t}\n");

	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type == S_EWT_BACKGROUND_SQUARE ||
			s_editor_widgets[i].type == S_EWT_BACKGROUND_SHAPE ||
			s_editor_widgets[i].type == S_EWT_BACKGROUND_IMAGE)
		{
			fprintf(f, "\n\t\t/* Backgrounds */\n\n");
			break;
		}
	}
	if(matrix)
	{
		fprintf(f, "\tmatrix = r_matrix_get();\n");
		fprintf(f, "\tr_matrix_push(matrix);\n");
		fprintf(f, "\tr_matrix_matrix_mult(matrix, m);\n");
	}

	for(i = 0; i < s_editor_widget_count; i++)
	{
		switch(s_editor_widgets[i].type)
		{
			case S_EWT_BACKGROUND_SQUARE :
			fprintf(f, "\tseduce_background_square_draw(input, %s, %.3f, %.3f, %.3f, %.3f, 0.5, 0.1, timer);\n",
											surface_id,
											s_editor_widgets[i].pos[0], 
											s_editor_widgets[i].pos[1],
											s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0], 
											s_editor_widgets[i].pos[3] - s_editor_widgets[i].pos[1]);
			break;
			case S_EWT_BACKGROUND_SHAPE :
			fprintf(f, "\tseduce_background_shape_draw(input, %s, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, timer, %.3f, %.3f, NULL, %.3f);\n",
											surface_id,							
											s_editor_widgets[i].pos[0],
											s_editor_widgets[i].pos[1],
											s_editor_widgets[i].pos[2],
											s_editor_widgets[i].pos[3],
											s_editor_widgets[i].pos[4],
											s_editor_widgets[i].pos[5], 
											s_editor_widgets[i].pos[6],
											s_editor_widgets[i].pos[7], 
											s_editor_widgets[i].data.surface.normal[0],
											s_editor_widgets[i].data.surface.normal[1],
											s_editor_widgets[i].data.surface.transparancy);
			break;
			case S_EWT_BACKGROUND_IMAGE :
			fprintf(f, "\tseduce_background_image_draw(input, %s, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, timer, NULL, -1);\n",
											surface_id,
											s_editor_widgets[i].pos[0], 
											s_editor_widgets[i].pos[1],
											s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0], 
											s_editor_widgets[i].pos[3] - s_editor_widgets[i].pos[1], 
											s_editor_widgets[i].data.image.u_start,
											s_editor_widgets[i].data.image.v_start,
											s_editor_widgets[i].data.image.u_end,
											s_editor_widgets[i].data.image.v_end);
			break;
		}
	}
	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type == S_EWT_BACKGROUND_SHAPE)
		{
			fprintf(f, "\tseduce_background_polygon_flush(input, NULL, timer);\n");
			break;
		}
	}
	added = FALSE;
	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type == S_EWT_TEXT_LINE_DRAW)
		{
			if(!added)
				fprintf(f, "\n\t\t/* Text */\n\n\tif(input->mode == BAM_DRAW)\n\t{\n");
			fprintf(f, "\t\tseduce_text_line_draw(%.3f, %.3f, %.3f, %.3f, seduce_translate(\"%s\"), %.3f, %.3f, %.3f, %.3f, -1);\n", 
							   s_editor_widgets[i].pos[0] - seduce_text_line_length(s_editor_widgets[i].data.text.size, s_editor_widgets[i].data.text.spacing, s_editor_widgets[i].name, -1) * s_editor_widgets[i].data.text.center, 
							   s_editor_widgets[i].pos[1], 
							   s_editor_widgets[i].data.text.size, 
							   s_editor_widgets[i].data.text.spacing, 
							   s_editor_widgets[i].name, 
							   s_editor_widgets[i].data.text.color[0], 
							   s_editor_widgets[i].data.text.color[1], 
							   s_editor_widgets[i].data.text.color[2], 
							   s_editor_widgets[i].data.text.color[3]);
			added = TRUE;
		}
	}
	if(added)
		fprintf(f, "\t}\n");

	for(i = 0; i < s_editor_widget_count; i++)
	{
		if(s_editor_widgets[i].type != S_EWT_BACKGROUND_SQUARE &&
			s_editor_widgets[i].type != S_EWT_BACKGROUND_SHAPE &&
			s_editor_widgets[i].type != S_EWT_BACKGROUND_IMAGE &&
			s_editor_widgets[i].type != S_EWT_TEXT_LINE_DRAW)
		{
			fprintf(f, "\n\t\t/* Widgets */\n\n");
			break;
		}
	}

	for(i = 0; i < S_EWT_COUNT; i++)	
		counters[i] = 0;
	for(i = 0; i < s_editor_widget_count; i++)
	{
		fprintf(f, "\n");
		if(s_editor_widgets[i].comment[0] != 0)
			fprintf(f, "\t\t/* %s */\n", s_editor_widgets[i].comment);
	
		switch(s_editor_widgets[i].type)
		{
			case S_EWT_TEXT_LINE_EDIT :
				fprintf(f, "\tseduce_text_line_edit(input, text_%u, text_%u, 64, %.3f, %.3f, %.3f, %.3f, seduce_translate(\"%s\"), TRUE, NULL, NULL,  %.3f, %.3f, %.3f, %.3f,  %.3f, %.3f, %.3f, %.3f);\n",
												counters[S_EWT_TEXT_LINE_EDIT],
												counters[S_EWT_TEXT_LINE_EDIT],
												s_editor_widgets[i].pos[0],
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0],
												s_editor_widgets[i].data.text.size,
												s_editor_widgets[i].name,
												s_editor_widgets[i].data.text.color[0], 
												s_editor_widgets[i].data.text.color[1], 
												s_editor_widgets[i].data.text.color[2], 
												s_editor_widgets[i].data.text.color[3],
												s_editor_widgets[i].data.text.color[4], 
												s_editor_widgets[i].data.text.color[5], 
												s_editor_widgets[i].data.text.color[6], 
												s_editor_widgets[i].data.text.color[7]);
				counters[S_EWT_TEXT_LINE_EDIT]++;
			break;
			case S_EWT_TEXT_PASSWORD_EDIT :
				fprintf(f, "\tseduce_text_password_edit(input, password_%u, %.3f, %.3f, %.3f, %.3f, password_%u, 64, seduce_translate(\"%s\"), NULL, NULL,  %.3f, %.3f, %.3f, %.3f,  %.3f, %.3f, %.3f, %.3f);\n",
												counters[S_EWT_TEXT_PASSWORD_EDIT],
												s_editor_widgets[i].pos[0],
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0],
												s_editor_widgets[i].data.text.size,
												counters[S_EWT_TEXT_PASSWORD_EDIT],
												s_editor_widgets[i].name,
												s_editor_widgets[i].data.text.color[0], 
												s_editor_widgets[i].data.text.color[1], 
												s_editor_widgets[i].data.text.color[2], 
												s_editor_widgets[i].data.text.color[3],
												s_editor_widgets[i].data.text.color[4], 
												s_editor_widgets[i].data.text.color[5], 
												s_editor_widgets[i].data.text.color[6], 
												s_editor_widgets[i].data.text.color[7]);
				counters[S_EWT_TEXT_PASSWORD_EDIT]++;
			break;
			case S_EWT_TEXT_DOUBLE_EDIT :
				fprintf(f, "\tseduce_text_double_edit(input, &dvalue_%u, &dvalue_%u, %.3f, %.3f, %.3f, %.3f, NULL, NULL,  %.3f, %.3f, %.3f, %.3f,  %.3f, %.3f, %.3f, %.3f);\n",
												counters[S_EWT_TEXT_DOUBLE_EDIT],
												counters[S_EWT_TEXT_DOUBLE_EDIT],
												s_editor_widgets[i].pos[0],
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0],
												s_editor_widgets[i].data.text.size,
												s_editor_widgets[i].data.text.color[0], 
												s_editor_widgets[i].data.text.color[1], 
												s_editor_widgets[i].data.text.color[2], 
												s_editor_widgets[i].data.text.color[3],
												s_editor_widgets[i].data.text.color[4], 
												s_editor_widgets[i].data.text.color[5], 
												s_editor_widgets[i].data.text.color[6], 
												s_editor_widgets[i].data.text.color[7]);
				counters[S_EWT_TEXT_DOUBLE_EDIT]++;
			break;
			case S_EWT_TEXT_FLOAT_EDIT :
				fprintf(f, "\tseduce_text_float_edit(input, &fvalue_%u, &fvalue_%u, %.3f, %.3f, %.3f, %.3f, NULL, NULL,  %.3f, %.3f, %.3f, %.3f,  %.3f, %.3f, %.3f, %.3f);\n",
												counters[S_EWT_TEXT_FLOAT_EDIT],
												counters[S_EWT_TEXT_FLOAT_EDIT],
												s_editor_widgets[i].pos[0],
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0],
												s_editor_widgets[i].data.text.size,
												s_editor_widgets[i].data.text.color[0], 
												s_editor_widgets[i].data.text.color[1], 
												s_editor_widgets[i].data.text.color[2], 
												s_editor_widgets[i].data.text.color[3],
												s_editor_widgets[i].data.text.color[4], 
												s_editor_widgets[i].data.text.color[5], 
												s_editor_widgets[i].data.text.color[6], 
												s_editor_widgets[i].data.text.color[7]);
				counters[S_EWT_TEXT_FLOAT_EDIT]++;
			break;
			case S_EWT_TEXT_INT_EDIT :
				fprintf(f, "\tseduce_text_int_edit(input, &ivalue_%u, &ivalue_%u, %.3f, %.3f, %.3f, %.3f, NULL, NULL,  %.3f, %.3f, %.3f, %.3f,  %.3f, %.3f, %.3f, %.3f);\n",
												counters[S_EWT_TEXT_INT_EDIT],
												counters[S_EWT_TEXT_INT_EDIT],
												s_editor_widgets[i].pos[0],
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0],
												s_editor_widgets[i].data.text.size,
												s_editor_widgets[i].data.text.color[0], 
												s_editor_widgets[i].data.text.color[1], 
												s_editor_widgets[i].data.text.color[2], 
												s_editor_widgets[i].data.text.color[3],
												s_editor_widgets[i].data.text.color[4], 
												s_editor_widgets[i].data.text.color[5], 
												s_editor_widgets[i].data.text.color[6], 
												s_editor_widgets[i].data.text.color[7]);
				counters[S_EWT_TEXT_INT_EDIT]++;
			break;
			case S_EWT_TEXT_UINT_EDIT :
				fprintf(f, "\tseduce_text_uint_edit(input, &uvalue_%u, &uvalue_%u, %.3f, %.3f, %.3f, %.3f, NULL, NULL,  %.3f, %.3f, %.3f, %.3f,  %.3f, %.3f, %.3f, %.3f);\n",
												counters[S_EWT_TEXT_UINT_EDIT],
												counters[S_EWT_TEXT_UINT_EDIT],
												s_editor_widgets[i].pos[0],
												s_editor_widgets[i].pos[1],
												s_editor_widgets[i].pos[2] - s_editor_widgets[i].pos[0],
												s_editor_widgets[i].data.text.size,
												s_editor_widgets[i].data.text.color[0], 
												s_editor_widgets[i].data.text.color[1], 
												s_editor_widgets[i].data.text.color[2], 
												s_editor_widgets[i].data.text.color[3],
												s_editor_widgets[i].data.text.color[4], 
												s_editor_widgets[i].data.text.color[5], 
												s_editor_widgets[i].data.text.color[6], 
												s_editor_widgets[i].data.text.color[7]);
				counters[S_EWT_TEXT_UINT_EDIT]++;
			break;
			case S_EWT_TEXT_BLOCK_DRAW :
			break;
			case S_EWT_TEXT_BUTTON :
				fprintf(f, "\tt = \"%s\";\n", s_editor_widgets[i].name);
				fprintf(f, "\tif(seduce_text_button(input, t, %.3f, %.3f, %.3f, %.3f, %.3f, t, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f))\n\t{\n\t}\n\n",
									s_editor_widgets[i].pos[0], 
									s_editor_widgets[i].pos[1], 
									s_editor_widgets[i].data.text.center, 
									s_editor_widgets[i].data.text.size, 
									s_editor_widgets[i].data.text.spacing, 
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
				fprintf(f, "\tswitch(seduce_text_button_list(input, buttons_%y, %.3f, %.3f, 0, SEDUCE_T_SIZE, SEDUCE_T_SPACE, buttons_%y, %u, 1, 1, 1, 1, 1, 1, 1, 1))\n\t{\n",
						counters[S_EWT_WIDGET_WHEEL_RADIAL],
						s_editor_widgets[i].pos[0], 
						s_editor_widgets[i].pos[1],
						counters[S_EWT_WIDGET_WHEEL_RADIAL],
						s_editor_widgets[i].data.options.array_length);

				for(j = 0; j < s_editor_widgets[i].data.options.array_length; j++)
					fprintf(f, "\t\tcase %u : /* %s */\n\t\tbreak;\n", j, s_editor_widgets[i].data.options.array[j]);			
				fprintf(f, "\t}\n\n");
				counters[S_EWT_WIDGET_WHEEL_RADIAL]++;
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
				fprintf(f, "\tif(seduce_widget_icon_button(input, &ids[%u], %u, %.3f, %.3f, %.3f, timer, NULL))\n\t{\n\t}\n\n",
							counters[S_EWT_WIDGET_ICON_BUTTON],
							s_editor_widgets[i].data.icon.icon,
							s_editor_widgets[i].pos[0],
							s_editor_widgets[i].pos[1], 
							s_editor_widgets[i].data.icon.scale);
				counters[S_EWT_WIDGET_ICON_BUTTON]++;
			break;

			case S_EWT_WIDGET_ICON_TOGGLE :
			fprintf(f, "\tseduce_widget_icon_toggle(input, &toggle_%u, &toggle_%u, %u, %.3f, %.3f, %.3f, timer);\n",
							counters[S_EWT_WIDGET_ICON_TOGGLE],
							counters[S_EWT_WIDGET_ICON_TOGGLE],
							s_editor_widgets[i].data.icon.icon,
							s_editor_widgets[i].pos[0],
							s_editor_widgets[i].pos[1],
							s_editor_widgets[i].data.icon.scale);
				counters[S_EWT_WIDGET_ICON_TOGGLE]++;
			break;
			case S_EWT_WIDGET_ICON_POPUP :
				fprintf(f, "\tseduce_popup_detect_icon(input, NULL, %u, %.3f, %.3f, %.3f, timer, NULL, NULL, TRUE, NULL);\n",
							s_editor_widgets[i].data.icon.icon,
							s_editor_widgets[i].pos[0],
							s_editor_widgets[i].pos[1], 
							s_editor_widgets[i].data.icon.scale);
				counters[S_EWT_WIDGET_ICON_POPUP]++;
			break;
			case S_EWT_WIDGET_SLIDER_RADIAL :
			fprintf(f, "\tseduce_widget_slider_radial(input, &slider_%u, &slider_%u, %.3f, %.3f, %.3f, 0.5, 0, 1, timer, NULL);\n", 
							counters[S_EWT_WIDGET_SLIDER_RADIAL],
							counters[S_EWT_WIDGET_SLIDER_RADIAL],
							s_editor_widgets[i].pos[0], 
							s_editor_widgets[i].pos[1], 
							s_editor_widgets[i].data.icon.scale);
				counters[S_EWT_WIDGET_SLIDER_RADIAL]++;
			break;
			case S_EWT_WIDGET_WHEEL_RADIAL :
			fprintf(f, "\tseduce_widget_wheel_radial(input, color_%u, color_%u, %.3f, %.3f, %.3f, 0.5, timer);\n",
					counters[S_EWT_WIDGET_WHEEL_RADIAL],
					counters[S_EWT_WIDGET_WHEEL_RADIAL],
					s_editor_widgets[i].pos[0], 
					s_editor_widgets[i].pos[1], 
					s_editor_widgets[i].data.icon.scale);
				counters[S_EWT_WIDGET_WHEEL_RADIAL]++;
			break;
			case S_EWT_WIDGET_SELECT_RADIAL :
				fprintf(f, "\tswitch(seduce_widget_select_radial(input, select_%u, lable_%u, lable_%u, &u, S_PUT_ANGLE, %.3f, %.3f, %.3f, timer, TRUE))\n\t{\n",
						counters[S_EWT_WIDGET_WHEEL_RADIAL],
						counters[S_EWT_WIDGET_WHEEL_RADIAL],
						counters[S_EWT_WIDGET_WHEEL_RADIAL],
						s_editor_widgets[i].data.options.array_length,
						s_editor_widgets[i].pos[0], 
						s_editor_widgets[i].pos[1], 
						s_editor_widgets[i].data.options.scale);
				for(j = 0; j < s_editor_widgets[i].data.options.array_length; j++)
					fprintf(f, "\t\tcase %u : /* %s */\n\t\tbreak;\n", j, s_editor_widgets[i].data.options.array[j]);			
				fprintf(f, "\t}\n\n");
				counters[S_EWT_WIDGET_WHEEL_RADIAL]++;
			break;
		}
	}
	if(matrix)
	{
		fprintf(f, "\tseduce_background_shape_matrix_interact(input, m, m, TRUE, TRUE);\n");
		fprintf(f, "\tr_matrix_pop(matrix);\n");
	}
	fprintf(f, "}\n");
}

void seduce_editor_save_data(char *file_name)
{
	uint i, j;
	THandle *h;
	h = testify_file_save(file_name);
	if(h == NULL)
		return;
	testify_debug_mode_set(h, TRUE, "text_debug_copy.txt");
	testify_pack_uint32(h, s_editor_widget_count, "widget count");
	for(i = 0; i < s_editor_widget_count; i++)
	{
		testify_pack_uint8(h, s_editor_widgets[i].type, "type");
		testify_pack_string(h, s_editor_widgets[i].name, "name");
		
		for(j = 0; j < 2 * SE_POS_COUNT; j++)
			testify_pack_real32(h, s_editor_widgets[i].pos[j], "pos");
		testify_pack_string(h, s_editor_widgets[i].comment, "comment");
		switch(s_editor_widgets[i].type)
		{
			case S_EWT_TEXT_LINE_DRAW :
			case S_EWT_TEXT_LINE_EDIT :
			case S_EWT_TEXT_PASSWORD_EDIT :
			case S_EWT_TEXT_DOUBLE_EDIT :
			case S_EWT_TEXT_FLOAT_EDIT :
			case S_EWT_TEXT_INT_EDIT :
			case S_EWT_TEXT_UINT_EDIT :
			case S_EWT_TEXT_BLOCK_DRAW :
			case S_EWT_TEXT_BUTTON :
				testify_pack_real32(h, s_editor_widgets[i].data.text.size, "text.size");
				testify_pack_real32(h, s_editor_widgets[i].data.text.spacing, "text.spacing");
				testify_pack_real32(h, s_editor_widgets[i].data.text.center, "text.center");
				for(j = 0; j < 8; j++)
					testify_pack_real32(h, s_editor_widgets[i].data.text.color[j], "text.color");
			break;
			case S_EWT_TEXT_BUTTON_LIST :
			case S_EWT_WIDGET_SELECT_RADIAL :
				testify_pack_uint32(h, s_editor_widgets[i].data.options.array_length, "options.scale");
				for(j = 0; j < s_editor_widgets[i].data.options.array_length; j++)
					testify_pack_string(h, s_editor_widgets[i].data.options.array[j], "option.array");
				testify_pack_real32(h, s_editor_widgets[i].data.options.scale, "options.scale");
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
			case S_EWT_WIDGET_ICON_TOGGLE :
			case S_EWT_WIDGET_ICON_POPUP :
			case S_EWT_WIDGET_SLIDER_RADIAL :
			case S_EWT_WIDGET_WHEEL_RADIAL :
				testify_pack_uint32(h, s_editor_widgets[i].data.icon.icon, "icon.icon");
				testify_pack_real32(h, s_editor_widgets[i].data.icon.scale, "icon.scale");
				testify_pack_uint8(h, s_editor_widgets[i].data.icon.color_active, "icon.color_active");
				for(j = 0; j < 4; j++)
					testify_pack_real32(h, s_editor_widgets[i].data.icon.color[j], "icon.color");
			break;
			case S_EWT_BACKGROUND_SQUARE :
				testify_pack_real32(h, s_editor_widgets[i].data.window.split, "window.split");
			break;
			case S_EWT_BACKGROUND_SHAPE :
				testify_pack_real32(h, s_editor_widgets[i].data.surface.normal[0], "surface.normal[0]");
				testify_pack_real32(h, s_editor_widgets[i].data.surface.normal[1], "surface.normal[1]");
			break;
			case S_EWT_BACKGROUND_IMAGE :
				testify_pack_real32(h, s_editor_widgets[i].data.image.u_start, "image.u_start");
				testify_pack_real32(h, s_editor_widgets[i].data.image.v_start, "image.v_start");
				testify_pack_real32(h, s_editor_widgets[i].data.image.u_end, "image.u_end");
				testify_pack_real32(h, s_editor_widgets[i].data.image.v_end, "image.v_end");
			break;
		}
	}
	testify_free(h);
}


void seduce_editor_load_data(char *file_name)
{
	uint i, j, id, count;
	THandle *h;
	h = testify_file_load(file_name);
	if(h == NULL)
		return;
	testify_debug_mode_set(h, TRUE, NULL);
	count = testify_unpack_uint32(h, "widget count");
	for(i = 0; i < count; i++)
	{
		id = s_editor_widget_add(testify_unpack_uint8(h, "type"), 0, 0);
		testify_unpack_string(h, s_editor_widgets[id].name, 32,"name");
		for(j = 0; j < 2 * SE_POS_COUNT; j++)
			s_editor_widgets[id].pos[j] = testify_unpack_real32(h, "pos");
		testify_unpack_string(h, s_editor_widgets[id].comment, 1024, "comment");
		switch(s_editor_widgets[id].type)
		{
			case S_EWT_TEXT_LINE_DRAW :
			case S_EWT_TEXT_LINE_EDIT :
			case S_EWT_TEXT_PASSWORD_EDIT :
			case S_EWT_TEXT_DOUBLE_EDIT :
			case S_EWT_TEXT_FLOAT_EDIT :
			case S_EWT_TEXT_INT_EDIT :
			case S_EWT_TEXT_UINT_EDIT :
			case S_EWT_TEXT_BLOCK_DRAW :
			case S_EWT_TEXT_BUTTON :
				s_editor_widgets[id].data.text.size = testify_unpack_real32(h, "text.size");
				s_editor_widgets[id].data.text.spacing = testify_unpack_real32(h, "text.spacing");
				s_editor_widgets[id].data.text.center = testify_unpack_real32(h, "text.center");
				for(j = 0; j < 8; j++)
					s_editor_widgets[id].data.text.color[j] = testify_unpack_real32(h, "text.color");
			break;
			case S_EWT_TEXT_BUTTON_LIST :
			case S_EWT_WIDGET_SELECT_RADIAL :
				s_editor_widgets[id].data.options.array_length = testify_unpack_uint32(h, "options.scale");
				for(j = 0; j < s_editor_widgets[id].data.options.array_length; j++)
					testify_unpack_string(h, s_editor_widgets[id].data.options.array[j], 32, "option.array");
				s_editor_widgets[id].data.options.scale = testify_unpack_real32(h, "options.scale");
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
			case S_EWT_WIDGET_ICON_TOGGLE :
			case S_EWT_WIDGET_ICON_POPUP :
			case S_EWT_WIDGET_SLIDER_RADIAL :
			case S_EWT_WIDGET_WHEEL_RADIAL :
				s_editor_widgets[id].data.icon.icon = testify_unpack_uint32(h, "icon.icon");
				s_editor_widgets[id].data.icon.scale = testify_unpack_real32(h, "icon.scale");
				s_editor_widgets[id].data.icon.color_active = testify_unpack_uint8(h, "icon.color_active");
				for(j = 0; j < 4; j++)
					s_editor_widgets[id].data.icon.color[j] = testify_unpack_real32(h, "icon.color");
			break;
			case S_EWT_BACKGROUND_SQUARE :
				s_editor_widgets[id].data.window.split = testify_unpack_real32(h, "window.split");
			break;
			case S_EWT_BACKGROUND_SHAPE :
				s_editor_widgets[id].data.surface.normal[0] = testify_unpack_real32(h, "surface.normal[0]");
				s_editor_widgets[id].data.surface.normal[1] = testify_unpack_real32(h, "surface.normal[1]");
			break;
			case S_EWT_BACKGROUND_IMAGE :
				s_editor_widgets[id].data.image.u_start = testify_unpack_real32(h, "image.u_start");
				s_editor_widgets[id].data.image.v_start = testify_unpack_real32(h, "image.v_start");
				s_editor_widgets[id].data.image.u_end = testify_unpack_real32(h, "image.u_end");
				s_editor_widgets[id].data.image.v_end = testify_unpack_real32(h, "image.v_end");
			break;
		}
	}
	testify_free(h);
}