#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "betray.h"
#include "seduce.h"
#include "seduce_editor.h"
#include "s_draw_3d.h"

//seduce_popup_detect_icon(BInputState *input, void *id, uint icon, float pos_x, float pos_y,  float scale, float time, void (*func)(BInputState *input, float time, void *user), void *user, float *color);


void seduce_editor_text_panel(BInputState *input, SEditorWidget *w, float timer)
{
		/* Backgrounds */

	seduce_background_square_draw(input, NULL, 0.620, -0.020, 0.200, 0.340, 0.5, 0.1, timer);

		/* Widgets */

	seduce_text_line_edit(input, w->name, w->name, 64, 0.640, 0.280, 0.160, 0.007, "Text", TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);

	seduce_text_float_edit(input, &w->data.text.size, &w->data.text.size, 0.640, 0.240, 0.160, 0.007, TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);

	seduce_text_float_edit(input, &w->data.text.spacing, &w->data.text.spacing, 0.640, 0.200, 0.160, 0.007, TRUE, NULL, NULL,  1.000, 1.000, 1.000, 1.000,  0.200, 0.600, 1.000, 1.000);

	seduce_widget_slider_radial(input, &w->data.text.center, &w->data.text.center, 0.660, 0.160, 0.050, 0.5, timer, 0.000, 1.000, NULL);

	seduce_widget_slider_radial(input, &w->data.text.color[0], &w->data.text.color[0], 0.660, 0.100, 0.050, 0.5, timer, 0.000, 1.000, w->data.text.color);

	seduce_widget_slider_radial(input, &w->data.text.color[1], &w->data.text.color[1], 0.720, 0.100, 0.050, 0.5, timer, 0.000, 1.000, w->data.text.color);

	seduce_widget_slider_radial(input, &w->data.text.color[2], &w->data.text.color[2], 0.780, 0.100, 0.050, 0.5, timer, 0.000, 1.000, w->data.text.color);

	seduce_widget_slider_radial(input, &w->data.text.color[4], &w->data.text.color[4], 0.660, 0.040, 0.050, 0.5, timer, 0.000, 1.000, &w->data.text.color[4]);

	seduce_widget_slider_radial(input, &w->data.text.color[5], &w->data.text.color[5], 0.720, 0.040, 0.050, 0.5, timer, 0.000, 1.000, &w->data.text.color[4]);

	seduce_widget_slider_radial(input, &w->data.text.color[6], &w->data.text.color[6], 0.780, 0.040, 0.050, 0.5, timer, 0.000, 1.000, &w->data.text.color[4]);

}


void seduce_editor_icon_select_func(BInputState *input, float time, uint *user)
{
	static int id;
	float color[4] = {1, 1, 1, 1};
	int i;
	for(i = 0; i < SUI_3D_OBJECT_COUNT; i++)
	{
		if(*user == i)
			seduce_widget_icon_button(input, &id + i, i, (i % 19 - 9) * 0.1, (i / 19) * 0.1,  0.05, time, color);
		else if(seduce_widget_icon_button(input, &id + i, i, (i % 19 - 9) * 0.1, (i / 19) * 0.1,  0.05, time, NULL))
			*user = i;
	}
}

void seduce_editor_icon_panel(BInputState *input, SEditorWidget *w, float timer)
{
	float value;
	static boolean toggle_0 = TRUE;
	static float color[3];

		/* Backgrounds */

	seduce_background_square_draw(input, NULL, 0.740, -0.100, 0.080, 0.380, 0.5, 0.1, timer);

		/* Backgrounds */

	seduce_popup_detect_icon(input, seduce_editor_icon_select_func, w->data.icon.icon, 0.780, 0.240, 0.050, timer, seduce_editor_icon_select_func, &w->data.icon.icon, FALSE, NULL);

	seduce_widget_icon_toggle(input, &w->data.icon.color_active, &w->data.icon.color_active, 38, 0.780, 0.120, 0.050, timer);

	value = w->data.icon.scale * 4.0;
	if(seduce_widget_slider_radial(input, &w->data.icon.scale, &value, 0.780, 0.180, 0.050, 0.5, 0.0, 1.0, timer, NULL))
		w->data.icon.scale = value / 4.0;

	seduce_widget_slider_radial(input, &w->data.icon.color[0], &w->data.icon.color[0], 0.780, 0.060, 0.050, 0.5, 0.0, 1.0, timer, NULL);
	seduce_widget_slider_radial(input, &w->data.icon.color[1], &w->data.icon.color[1], 0.780, 0.000, 0.050, 0.5, 0.0, 1.0, timer, NULL);
	seduce_widget_slider_radial(input, &w->data.icon.color[2], &w->data.icon.color[2], 0.780, -0.060, 0.050, 0.5, 0.0, 1.0, timer, NULL);
}

void seduce_editor_panel_draw_old(BInputState *input, SEditorWidget *w)
{
	static float animation[S_EWT_COUNT] = {10000};
	static SEditorWidget *icon_panel = NULL;
	static SEditorWidget *text_panel = NULL;
	uint i;
	if(animation[0] > 100)
		for(i = 0; i < S_EWT_COUNT; i++)
			animation[i] = 0;
	if(input->mode == BAM_MAIN)
	{
		for(i = 0; i < S_EWT_COUNT; i++)
		{
			if(w != NULL && i == w->type)
			{
				animation[i] += input->delta_time * 2.0;
				if(animation[i] > 1.0)
					animation[i] = 1.0;
			}else
			{
				animation[i] -= input->delta_time * 2.0;
				if(animation[i] < 0.0)
					animation[i] = 0.0;
			}	
		}		
	}
	if(w != NULL)
	{
		switch(w->type)
		{
			case S_EWT_TEXT_LINE_DRAW :
			case S_EWT_TEXT_LINE_EDIT :
			case S_EWT_TEXT_PASSWORD_EDIT :
			case S_EWT_TEXT_DOUBLE_EDIT :
			case S_EWT_TEXT_INT_EDIT :
			case S_EWT_TEXT_UINT_EDIT :
			case S_EWT_TEXT_BUTTON :
				text_panel = w;
			break;
			case S_EWT_TEXT_BLOCK_DRAW :
			case S_EWT_TEXT_BUTTON_LIST :
			case S_EWT_WIDGET_SELECT_RADIAL :
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
			case S_EWT_WIDGET_ICON_TOGGLE :
			case S_EWT_WIDGET_ICON_POPUP :
			case S_EWT_WIDGET_SLIDER_RADIAL :
			case S_EWT_WIDGET_WHEEL_RADIAL :
				icon_panel = w;
			break;
			case S_EWT_BACKGROUND_SQUARE :
			break;
			case S_EWT_BACKGROUND_SHAPE :
			break;
			case S_EWT_BACKGROUND_IMAGE :
			break;
		}
	}
	
	if(text_panel != NULL && animation[text_panel->type] > 0.01)
		seduce_editor_text_panel(input, text_panel, animation[text_panel->type]);
	if(icon_panel != NULL && animation[icon_panel->type] > 0.01)
		seduce_editor_icon_panel(input, icon_panel, animation[icon_panel->type]);
}



/*

typedef enum{
	SEDUCE_PET_LABEL,
	SEDUCE_PET_BOOLEAN,
	SEDUCE_PET_INTEGER,
	SEDUCE_PET_UNSIGNED_INTEGER,
	SEDUCE_PET_REAL,
	SEDUCE_PET_SLIDER,
	SEDUCE_PET_TEXT,
	SEDUCE_PET_PASSWORD,
	SEDUCE_PET_TEXT_BOX,
	SEDUCE_PET_LIST,
	SEDUCE_PET_COLOR_RGB,
	SEDUCE_PET_COLOR_SLICES,
	SEDUCE_PET_COLOR_WHEEL, 
	SEDUCE_PET_COLOR_HUE, 
	SEDUCE_PET_COLOR_SATURATION,
	SEDUCE_PET_COLOR_VALUE,
	SEDUCE_PET_SELECT,
	SEDUCE_PET_CUSTOM,
	SEDUCE_PET_SECTION_START,
	SEDUCE_PET_SECTION_END
}SeducePanelElementType;

typedef struct{
	SeducePanelElementType type;
	char *text;
	union{
		boolean checkbox; 
		int		integer;
		uint	uinteger;
		double	real;
		float	slider;
		struct{
			char	*text;
			uint	length;
			uint	lines;
		}text;
		struct{
			char	**text;
			uint	count;
			uint	active;
		}list;
		struct{
			char	*text;
			uint	length;
		}password;
		float	*color;
		struct{
			char	**text;
			uint	count;
			uint	select;
		}select;
		struct{
			uint	current;
			uint	count;
			boolean	add;
			boolean	del;
		}split_multi;
		struct{
			void (*func)(BInputState *input, void *user, double x_pos, double y_pos, double width, double length);
			void *user;
			float length;
		}custom;
		struct{
			boolean open;
			float timer;
		}sections;
	}param;
}SeducePanelElement;
*/
void seduce_editor_panel_test(BInputState *input)
{
	static float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	char *lables[5] = {"One" , "Two", "Three", "Four", "Five"};
	static SeducePanelElement panel[SEDUCE_PET_SECTION_END + 1];
	static char text[32] = {65, 0};
	static float color[4] = {0.2, 0.6, 1.0, 1.0};
	RMatrix *matrix;
	static uint i = 0;
	if(i == 0)
	{
		panel[i].type = SEDUCE_PET_LABEL;
		panel[i++].text = "LABEL";

		panel[i].type = SEDUCE_PET_BUTTON;
		panel[i].param.button.active = FALSE;
		panel[i].param.button.icon = 0;
		panel[i++].text = "BUTTON";

		panel[i].type = SEDUCE_PET_BOOLEAN;
		panel[i].text = "BOOLEAN";
		panel[i++].param.checkbox = TRUE; 

		panel[i].type = SEDUCE_PET_INTEGER;
		panel[i].text = "INTEGER";
		panel[i++].param.integer = 42; 

		panel[i].type = SEDUCE_PET_UNSIGNED_INTEGER;
		panel[i].text = "UNSIGNED_INTEGER";
		panel[i++].param.uinteger = 42; 
	
		panel[i].type = SEDUCE_PET_REAL;
		panel[i].text = "REAL";
		panel[i++].param.real = 1.0;

		panel[i].type = SEDUCE_PET_SLIDER;
		panel[i].text = "SLIDER";
		panel[i++].param.slider = 0.5;

		panel[i].type = SEDUCE_PET_TEXT;
		panel[i].text = "TEXT";
		panel[i].param.text.text = text;
		panel[i].param.text.length = 32;
		panel[i++].param.text.lines = 4;

		panel[i].type = SEDUCE_PET_PASSWORD;
		panel[i].text = "PASSWORD";
		panel[i].param.password.text = text;
		panel[i++].param.password.length = 32;

	/*	panel[i].type = SEDUCE_PET_TEXT_BOX;
		panel[i].text = "TEXT_BOX";
		panel[i].param.text.text = text;
		panel[i].param.text.length = 32;
		panel[i++].param.text.lines = 4;
*/
		panel[i].type = SEDUCE_PET_SECTION_START;
		panel[i].param.sections.open = TRUE;
		panel[i].param.sections.timer = 0.5;
		panel[i++].text = "SECTION_START";
		
		panel[i].type = SEDUCE_PET_COLOR_RGB;
		panel[i].text = "COLOR_RGB";
		panel[i++].param.color = color;

		panel[i].type = SEDUCE_PET_COLOR_WHEEL;
		panel[i].text = "COLOR_WHEEL";
		panel[i++].param.color = color;

		panel[i].type = SEDUCE_PET_COLOR_HUE;
		panel[i].text = "COLOR_HUE";
		panel[i++].param.color = color;

		panel[i].type = SEDUCE_PET_COLOR_SATURATION;
		panel[i].text = "COLOR_SATURATION";
		panel[i++].param.color = color;

		panel[i].type = SEDUCE_PET_SECTION_END;
		panel[i++].text = "END"; 

		panel[i].type = SEDUCE_PET_COLOR_VALUE;
		panel[i].text = "COLOR_VALUE";
		panel[i++].param.color = color;


		panel[i].type = SEDUCE_PET_SELECT;
		panel[i].text = "SELECT";
		panel[i].param.select.text = lables;
		panel[i].param.select.active = 0;
		panel[i++].param.select.count = 5;

	}

	matrix = r_matrix_get();
	r_matrix_push(matrix);
	r_matrix_matrix_mult(matrix, m);
	seduce_settings_panel(input, m, panel, i, 0, 0, 0.4, 1, 1);
	seduce_background_shape_matrix_interact(input, m, m, TRUE, TRUE);
	r_matrix_pop(matrix);
}




void seduce_editor_panel_draw(BInputState *input, SEditorWidget *w)
{

	static float m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
	char *lables[5] = {"One" , "Two", "Three", "Four", "Five"};
	static SeducePanelElement panel[SEDUCE_PET_SECTION_END + 1];
	static char text[32] = {65, 0};
	static float color[4] = {0.2, 0.6, 1.0, 1.0};
	RMatrix *matrix;
	uint i = 0, j;
	if(w != NULL)
	{
		panel[i].type = SEDUCE_PET_TEXT;
		panel[i].text = "Name";
		panel[i].param.text.text = w->name;
		panel[i].param.text.length = 32;
		panel[i++].param.text.lines = 4;
		panel[i].type = SEDUCE_PET_TEXT;
		panel[i].text = "Comment";
		panel[i].param.text.text = w->comment;
		panel[i].param.text.length = 1024;
		panel[i++].param.text.lines = 4;
		switch(w->type)
		{
			case S_EWT_TEXT_LINE_DRAW :
			case S_EWT_TEXT_LINE_EDIT :
			case S_EWT_TEXT_PASSWORD_EDIT :
			case S_EWT_TEXT_DOUBLE_EDIT :
			case S_EWT_TEXT_INT_EDIT :
			case S_EWT_TEXT_UINT_EDIT :
			case S_EWT_TEXT_BUTTON :
			panel[i].type = SEDUCE_PET_REAL;
			panel[i].text = "Size";
			panel[i++].param.real = w->data.text.size;
			panel[i].type = SEDUCE_PET_REAL;
			panel[i].text = "Spacing";
			panel[i++].param.real = w->data.text.spacing;
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Center";
			panel[i++].param.slider = w->data.text.center;

			panel[i].type = SEDUCE_PET_COLOR_WHEEL;
			panel[i].text = "Color";
			panel[i++].param.color =  w->data.text.color;
			panel[i].type = SEDUCE_PET_COLOR_RGB;
			panel[i].text = "RGB";
			panel[i++].param.color =  w->data.text.color;
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Alpha";
			panel[i++].param.slider = w->data.text.color[3];

			panel[i].type = SEDUCE_PET_COLOR_WHEEL;
			panel[i].text = "Color";
			panel[i++].param.color =  &w->data.text.color[4];
			panel[i].type = SEDUCE_PET_COLOR_RGB;
			panel[i].text = "RGB";
			panel[i++].param.color =  &w->data.text.color[4];
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Alpha";
			panel[i++].param.slider = w->data.text.color[7];
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
			case S_EWT_WIDGET_ICON_TOGGLE :
			case S_EWT_WIDGET_ICON_POPUP :
			case S_EWT_WIDGET_SLIDER_RADIAL :
			case S_EWT_WIDGET_WHEEL_RADIAL :

			if(w->type != S_EWT_WIDGET_SLIDER_RADIAL &&
				w->type != S_EWT_WIDGET_WHEEL_RADIAL)
			{
				panel[i].type = SEDUCE_PET_POPUP;
				panel[i].text = "icon";
				panel[i].param.popup.displace = FALSE;
				panel[i].param.popup.func = seduce_editor_icon_select_func;
				panel[i].param.popup.user = &w->data.icon.icon;
				panel[i++].param.popup.icon = w->data.icon.icon;
			}
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Size";
			panel[i++].param.slider = w->data.icon.scale * 10.0;

			panel[i].type = SEDUCE_PET_SECTION_START;
			panel[i].param.sections.open = w->data.icon.color_active;
			panel[i].param.sections.timer = 0.5;
			panel[i++].text = "Color Active";

			panel[i].type = SEDUCE_PET_COLOR_WHEEL;
			panel[i].text = "Color";
			panel[i++].param.color =  w->data.icon.color;
			panel[i].type = SEDUCE_PET_COLOR_RGB;
			panel[i].text = "RGB";
			panel[i++].param.color =  w->data.icon.color;
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Alpha";
			panel[i++].param.slider = w->data.icon.color[3];

			panel[i++].type = SEDUCE_PET_SECTION_END;
			break;
			case S_EWT_TEXT_BUTTON_LIST :
			case S_EWT_WIDGET_SELECT_RADIAL :
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "scale";
			panel[i++].param.slider = w->data.options.scale;
			panel[i].type = SEDUCE_PET_UNSIGNED_INTEGER;
			panel[i].text = "Option Count";
			panel[i++].param.uinteger = w->data.options.array_length;
			for(j = 0; j < w->data.options.array_length && j  < 24; j++)
			{
				panel[i].type = SEDUCE_PET_TEXT;
				panel[i].text = "Option";
				panel[i].param.text.text = w->data.options.array[0];
				panel[i++].param.text.length = 32;
			}
			break;
			case S_EWT_BACKGROUND_SQUARE :
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Split";
			panel[i++].param.slider = w->data.window.split;
			break;
			case S_EWT_BACKGROUND_SHAPE :
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "X normal";
			panel[i++].param.slider = w->data.surface.normal[0] + 0.5;
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Y normal";
			panel[i++].param.slider = w->data.surface.normal[1] + 0.5;
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "Transparancy";
			panel[i++].param.slider = w->data.surface.transparancy;
			break;
			case S_EWT_BACKGROUND_IMAGE :
			panel[i].type = SEDUCE_PET_REAL;
			panel[i].text = "U Start";
			panel[i++].param.real = w->data.image.u_start;
			panel[i].type = SEDUCE_PET_REAL;
			panel[i].text = "V Start";
			panel[i++].param.real = w->data.image.v_start;
			panel[i].type = SEDUCE_PET_REAL;
			panel[i].text = "U End";
			panel[i++].param.real = w->data.image.u_end;
			panel[i].type = SEDUCE_PET_REAL;
			panel[i].text = "V End";
			panel[i++].param.real = w->data.image.v_end;
			break;
		}
	}
	if(i != 0)
	{
		matrix = r_matrix_get();
		r_matrix_push(matrix);
		r_matrix_matrix_mult(matrix, m);
		seduce_settings_panel(input, m, panel, i, 0, 0, 0.4, 1, 1);
		seduce_background_shape_matrix_interact(input, m, m, TRUE, TRUE);
		r_matrix_pop(matrix);
	}

	if(w != NULL)
	{
		i = 2;
		switch(w->type)
		{
			case S_EWT_TEXT_LINE_DRAW :
			case S_EWT_TEXT_LINE_EDIT :
			case S_EWT_TEXT_PASSWORD_EDIT :
			case S_EWT_TEXT_DOUBLE_EDIT :
			case S_EWT_TEXT_INT_EDIT :
			case S_EWT_TEXT_UINT_EDIT :
			case S_EWT_TEXT_BUTTON :
			w->data.text.size = panel[i++].param.real;
			w->data.text.spacing = panel[i++].param.real;
			w->data.text.center = panel[i++].param.slider;
			i += 2;
			w->data.text.color[3] = panel[i++].param.slider;
			i += 2;
			w->data.text.color[7] = panel[i++].param.slider;
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
			case S_EWT_WIDGET_ICON_TOGGLE :
			case S_EWT_WIDGET_ICON_POPUP :
			case S_EWT_WIDGET_SLIDER_RADIAL :
			case S_EWT_WIDGET_WHEEL_RADIAL :

			if(w->type != S_EWT_WIDGET_SLIDER_RADIAL &&
				w->type != S_EWT_WIDGET_WHEEL_RADIAL)
				i++;
			w->data.icon.scale = panel[i++].param.slider / 10.0;
			w->data.icon.color_active = panel[i++].param.sections.open;
			i += 2;
			w->data.icon.color[3] = panel[i++].param.slider;
			i++;
			break;
			case S_EWT_TEXT_BUTTON_LIST :
			case S_EWT_WIDGET_SELECT_RADIAL :
			panel[i].type = SEDUCE_PET_SLIDER;
			panel[i].text = "scale";
			 w->data.options.scale = panel[i++].param.slider;
			panel[i].type = SEDUCE_PET_UNSIGNED_INTEGER;
			panel[i].text = "Option Count";
			w->data.options.array_length = panel[i++].param.uinteger;
			for(j = 0; j < w->data.options.array_length && j  < 24; j++)
				i++;
			break;
			case S_EWT_BACKGROUND_SQUARE :
			w->data.window.split = panel[i++].param.slider;
			break;
			case S_EWT_BACKGROUND_SHAPE :
			w->data.surface.normal[0] = panel[i++].param.slider - 0.5;
			w->data.surface.normal[1] = panel[i++].param.slider - 0.5;
			w->data.surface.transparancy = panel[i++].param.slider;
			break;
			case S_EWT_BACKGROUND_IMAGE :
			w->data.image.u_start = panel[i++].param.real;
			w->data.image.v_start = panel[i++].param.real;
			w->data.image.u_end = panel[i++].param.real;
			w->data.image.v_end = panel[i++].param.real;
			break;
		}
	}
}

/*

	if(input->mode == BAM_MAIN)
	{
		for(i = 0; i < S_EWT_COUNT; i++)
		{
			if(w != NULL && i == w->type)
			{
				animation[i] += input->delta_time * 2.0;
				if(animation[i] > 1.0)
					animation[i] = 1.0;
			}else
			{
				animation[i] -= input->delta_time * 2.0;
				if(animation[i] < 0.0)
					animation[i] = 0.0;
			}	
		}		
	}
	if(w != NULL)
	{
		panel[i].type = SEDUCE_PET_TEXT;
		panel[i].text = "Name";
		panel[i].param.text.text = w->name;
		panel[i].param.text.length = 32;
		panel[i++].param.text.lines = 4;
		panel[i].type = SEDUCE_PET_TEXT;
		panel[i].text = "Comment";
		panel[i].param.text.comment = w->comment;
		panel[i].param.text.length = 1024;
		panel[i++].param.text.lines = 4;

		switch(w->type)
		{
			case S_EWT_TEXT_LINE_DRAW :
			case S_EWT_TEXT_LINE_EDIT :
			case S_EWT_TEXT_PASSWORD_EDIT :
			case S_EWT_TEXT_DOUBLE_EDIT :
			case S_EWT_TEXT_INT_EDIT :
			case S_EWT_TEXT_UINT_EDIT :
			case S_EWT_TEXT_BUTTON :
		//		text_panel = w;
			break;
			case S_EWT_TEXT_BLOCK_DRAW :
			case S_EWT_TEXT_BUTTON_LIST :
			case S_EWT_WIDGET_SELECT_RADIAL :
			break;
			case S_EWT_WIDGET_ICON_BUTTON :
			case S_EWT_WIDGET_ICON_TOGGLE :
			case S_EWT_WIDGET_ICON_POPUP :
			case S_EWT_WIDGET_SLIDER_RADIAL :
			case S_EWT_WIDGET_WHEEL_RADIAL :
		//		icon_panel = w;
			break;
			case S_EWT_BACKGROUND_SQUARE :
			break;
			case S_EWT_BACKGROUND_SHAPE :
			break;
			case S_EWT_BACKGROUND_IMAGE :
			break;
		}
	}
	
	if(text_panel != NULL && animation[text_panel->type] > 0.01)
		seduce_editor_text_panel(input, text_panel, animation[text_panel->type]);
	if(icon_panel != NULL && animation[icon_panel->type] > 0.01)
		seduce_editor_icon_panel(input, icon_panel, animation[icon_panel->type]);
}*/
