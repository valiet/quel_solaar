
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "seduce.h"

extern float sui_font_vertex_array[13098];
extern unsigned int *sui_font_ref_array[256];
extern unsigned int sui_font_ref_size[256];
extern float sui_font_letter_size[2048];


#define SUI_TEXTURE_DRAW_CASH_SIZE 512
#define SUI_TEXTURE_LETTER_SIZE 2.0
#define SUI_TEXTURE_LETTER_BASE (0.5)


extern boolean	betray_set_screen_mode(uint x_size, uint y_size, boolean fullscreen);
extern double	betray_screen_mode_get(uint *x_size, uint *y_size, boolean *fullscreen);
extern void		betray_set_frustum_mode(double far, double near, double fov);
extern void		betray_get_frustum_mode(double *far, double *near, double *fov);

extern void seduce_font_init();

void *sui_font_array = NULL;
void *sui_font_section = NULL;
void *sui_font_reference[256];
void *sui_font_shader;
uint sui_font_shader_location_shader;

char *r_font_shader_vertex = 
"attribute vec2 vertex;\n"
"uniform block{\n"
"	vec2 pos;\n"
"};\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"void main()\n"
"{\n"
"	gl_Position = ModelViewProjectionMatrix * vec4(vertex + pos, 0.0, 1.0);\n"
"}\n";
char *r_font_shader_fragment = 
"uniform vec4 color;\n"
"void main()\n"
"{\n"
"	gl_FragColor = color;\n"
"}\n";


void seduce_text_init()
{
	SUIFormats vertex_format_types = SUI_FLOAT;
	uint vertex_format_size = 2, i, ref_count = 0;
	char buf[2000];
	for(i = 0; i < 256; i++)
		ref_count += (sui_font_ref_size[i] + 2)/* / 2*/;
	sui_font_array = r_array_allocate(13074 / 2 + 256, &vertex_format_types, &vertex_format_size, 1, ref_count);
	sui_font_section = r_array_section_allocate_vertex(sui_font_array, 13074 / 2);
	r_array_load_vertex(sui_font_array, sui_font_section, sui_font_vertex_array, 0, 13074 / 2);
	for(i = 0; i < 256; i++)
	{
		if(sui_font_ref_size[i] == 0)
		{
			sui_font_reference[i] = NULL;
		}else
		{
			sui_font_reference[i] = r_array_section_allocate_reference(sui_font_array, sui_font_ref_size[i]);
			r_array_load_reference(sui_font_array, sui_font_reference[i], sui_font_section, sui_font_ref_array[i], sui_font_ref_size[i]);
		}
	}
	sui_font_shader = r_shader_create_simple(buf, 2000, r_font_shader_vertex, r_font_shader_fragment, "color font");
	printf(buf);
	sui_font_shader_location_shader = r_shader_uniform_location(sui_font_shader, "color");
}

float sui_text_space(const char *text)
{
	float f, f2;
	f = sui_font_letter_size[text[0] * 8 + 4] - sui_font_letter_size[text[1] * 8 + 0];
	f2 = sui_font_letter_size[text[0] * 8 + 5] - sui_font_letter_size[text[1] * 8 + 1];
	if(f2 > f)
		f = f2;
	f2 = sui_font_letter_size[text[0] * 8 + 6] - sui_font_letter_size[text[1] * 8 + 2];
	if(f2 > f)
		f = f2;
	f2 = sui_font_letter_size[text[0] * 8 + 7] - sui_font_letter_size[text[1] * 8 + 3];
	if(f2 > f)
		f = f2;
	return f;
}

float sui_text_size(const char *text)
{
	float f, f2;
	f = sui_font_letter_size[text[0] * 8 + 4];
	f2 = sui_font_letter_size[text[0] * 8 + 5];
	if(f2 > f)
		f = f2;
	f2 = sui_font_letter_size[text[0] * 8 + 6];
	if(f2 > f)
		f = f2;
	f2 = sui_font_letter_size[text[0] * 8 + 7];
	if(f2 > f)
		f = f2;
	return f;
}

float sui_text_line_draw_sample(float size, float spacing, const char *text, char length)
{
	unsigned int i = 0, j = 0, k;
	float f, f2, pos = 0;
	RMatrix	*m;

	m = r_matrix_get();
	if(text[0] != 0)
	{
		static void **sections = NULL;
		static float *data;
		uint location, size, calls = 0;
		float *c;
		size = r_shader_uniform_block_size(sui_font_shader, 0);
		if(sections == NULL)
		{
			sections = malloc((sizeof *sections) * 1024);
			data = malloc(1024 * size);
		}
		size /= sizeof(float);
		data[0] = 0;
		data[1] = 0;

		while(i < length && text[i] != 0)
		{
			while(i < length && calls < 1024)
			{
				if(sui_font_reference[(uint8)text[i]] != NULL)	
				{
					data[calls * size] = pos;
					data[calls * size + 1] = 0;
					sections[calls] = sui_font_reference[(uint8)text[i]];
					calls++;
					if(calls == 1024)
						break;
				}
 				i++;
				if(text[i] == 0)
					break;
				f = sui_text_space(&text[i - 1]);
				pos += f + spacing;
			}
			r_array_references_draw(sui_font_array, sections, GL_TRIANGLES, data, NULL, calls);
			calls = 0;
		}
	}
	return pos;
}

float sui_text_line_draw_sample_old(float size, float spacing, const char *text, char length)
{
	unsigned int i = 0, j = 0, k;
	float f, f2, pos = 0;
	RMatrix	*m;

	m = r_matrix_get();
	if(text[0] != 0)
	{
		while(i < length)
		{
			if(sui_font_reference[(uint8)text[i]] != NULL)	
			{
				r_array_reference_draw(sui_font_array, sui_font_reference[(uint8)text[i]], GL_TRIANGLES, 0, sui_font_ref_size[(uint8)text[i]]);
			}
 			i++;
			if(text[i] == 0)
				break;
			f = sui_text_space(&text[i - 1]);
			pos += f;
			r_matrix_translate(m, f + spacing, 0, 0);
		}
	}
	return pos;
}


float seduce_text_line_draw(float pos_x, float pos_y, float size, float spacing, const char *text, float red, float green, float blue, float alpha, uint length)
{
	RShader	*s;
	RMatrix	*m;
	float f = 0;
	if(text == NULL || *text == '\0')
		return 0.0;
	m = r_matrix_get();
	r_matrix_push(m);
	r_matrix_translate(m, pos_x, pos_y, 0);
	r_matrix_scale(m, size, size, 1.0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	r_shader_set(sui_font_shader);
	r_shader_vec4_set(NULL, sui_font_shader_location_shader, red, green, blue, alpha);
	f = sui_text_line_draw_sample(size, spacing, text, length);
	r_matrix_pop(m);
	return f * size;
}

float sui_text_pos(float size, float spacing, const char *text, float left, float spaces, uint end)
{
	unsigned int i, j, k;
	float f, f2, length = 0;
	i = 0;
	if(text[0] != 0 && 0 != end)
	{
		while(TRUE)
		{
			i++;
			f = sui_text_space(&text[i - 1]);
			if(text[i] <= 32)
				length += f + spacing + left / (float)spaces;
			else
				length += f + spacing;
			if(text[i] == 0 || i == end)
				break;
		}
		return length;
	}
	return 0;
}

float seduce_text_line_length(float size, float spacing, const char *text, uint end)
{
	return sui_text_pos(size, spacing, text, 0.0, 1.0, end) * size;
}

uint sui_compute_find_size(const char *text, float spacing, float size, boolean cut_words, SUIBlockStyle style, uint *spaces, uint *add, float *left, float *start)
{
	unsigned int i, j, k, end = 0;
	float f, f2, length = 0;
	*spaces = 0;
	*start = *left = 0;
	if(SEDUCE_BS_CODE == style)
	{
		j = (uint)(size / (spacing * 3.5));
		for(i = 0; text[i] != 0 && text[i] != 10 && i < j; i++);
		for(j = i; text[j] != 0 && text[j] != 10; j++);
		*add = j;
		*spaces = 1;
		return i;
	}
	i = 0;
	if(text[0] != 0)
	{
		f = 0;
		while(TRUE)
		{
			
			if(text[i] < 33)
			{
				end = i;
				*spaces += 1;
				*left = length;
			}
			if(text[i] == 10)
			{
				*left = 0;
				*add = i;
				return i;
			}
			if(text[i] == 0 || length > size)
			{

				
			//	*spaces -= 1;
				if(*spaces == 0)
					end = i;
				break;
			}
			if(cut_words)
				end = i;
			i++;
			f = sui_text_space(&text[i - 1]);
			length += f + spacing;
		}
	}
	if(left != NULL)
		*left = size - *left;

	switch(style)
	{
		case SEDUCE_BS_LEFT :
			*left = *start = 0.0;
		break;
		case SEDUCE_BS_RIGHT :
			*start = *left;
			*left = 0.0;
		break;
		case SEDUCE_BS_CENTER :
			*start = *left * 0.5;
			*left = 0.0;
		break;
		case SEDUCE_BS_STRETCH :
			*start = 0.0;
			if(text[i] == 0)
				*left = 0;
		break;
		case SEDUCE_BS_CODE :
			*left = *start = 0.0;
		break;
	}
	*add = end;
	if(text[end] == 10)
	{
		*add = end + 1;
		
	}
	return end;
}


uint sui_text_block_line_end(const char *text, float spacing, float letter_size, float size, SUIBlockStyle style)
{
	uint spaces, add;
	float left, start;
	return sui_compute_find_size(text, spacing, size / letter_size, FALSE, style, &spaces, &add, &left, &start);
}

uint sui_text_block_draw_sample(float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint text_start, float *color, uint *color_switch, uint *scroll, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user)
{
	uint i, j, k = 0, length, add, pos, spaces, c_switch = 0;
	float left, f, f2, array[SUI_TEXTURE_DRAW_CASH_SIZE * 4 * 2], uv[SUI_TEXTURE_DRAW_CASH_SIZE * 4 * 2], start_x, end_x;

	pos = text_start;

	if(scroll != NULL)
	{
		for(i = 0; i < scroll[0]; i++)
		{
			sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, style, &spaces, &add, &left, &f);
			pos += add;
		}
	}
	if(color_switch != NULL)
	{
		for(i = 0; pos > color_switch[i]; i++)
		{
			r_shader_vec4_set(NULL, sui_font_shader_location_shader, color[4 * c_switch + 0], color[4 * c_switch + 1], color[4 * c_switch + 2], color[4 * c_switch + 3]);
			c_switch++;
		}
	}

	if(c_switch == 0)
		r_shader_vec4_set(NULL, sui_font_shader_location_shader, color[0], color[1], color[2], color[3]);

	start_x = 0;
	end_x = line_size;
	if(func != NULL)
	{
		func(pos_x, pos_y, &start_x, &end_x, user);
		if(start_x < 0)
			start_x = 0;
		if(end_x > line_size)
			end_x = line_size;
	}
	length = sui_compute_find_size(&text[pos], letter_spacing, (end_x - start_x) / letter_size, FALSE, style, &spaces, &add, &left, &f);
	start_x += f;
	for(i = 0; i < line_count; i++)
	{
		if(scroll != 0)
			for(; pos < scroll[1] && text[pos] != 10 && text[pos] != 0; pos++);
		r_matrix_push(NULL);

		k = pos;
		if(text[pos] != 0)
		{
			if(SEDUCE_BS_CODE == style)
			{
				f2 = start_x;
				for(j = 0; j < length; j++)
				{
					if(color != NULL && color_switch != NULL && color_switch[c_switch] == pos)
					{
						c_switch++;
						r_shader_vec4_set(NULL, sui_font_shader_location_shader, color[4 * c_switch + 0], color[4 * c_switch + 1], color[4 * c_switch + 2], color[4 * c_switch + 3]);
					}
					f = (sui_text_size(&text[pos]) - letter_spacing * 3.5) / 2.0;
					r_matrix_translate(NULL, letter_spacing * 3.5 - f + f2, 0, 0);
					if(sui_font_ref_array[text[pos]] != NULL)
						r_array_reference_draw(sui_font_array, sui_font_reference[text[pos]], GL_TRIANGLES, 0, sui_font_ref_size[text[pos]]);
					pos++;
					f2 = f;
				}
			}else
			{
				r_matrix_translate(NULL, start_x, 0, 0);
				for(j = 0; j < length; j++)
				{
					if(color != NULL && color_switch != NULL && color_switch[c_switch] == pos)
					{
						c_switch++;
						r_shader_vec4_set(NULL, sui_font_shader_location_shader, color[4 * c_switch + 0], color[4 * c_switch + 1], color[4 * c_switch + 2], color[4 * c_switch + 3]);
					}					
					if(sui_font_ref_array[text[pos]] != NULL)
						r_array_reference_draw(sui_font_array, sui_font_reference[text[pos]], GL_TRIANGLES, 0, sui_font_ref_size[text[pos]]);
					pos++;
					if(j + 1 == length)
						break;
					f = sui_text_space(&text[pos - 1]);
					if(sui_font_ref_array[text[pos]] != NULL || text[pos - j + length] == 0)
						f += letter_spacing;
					else
						f += letter_spacing + (float)left / ((float)spaces - 1.0);
					r_matrix_translate(NULL, f, 0, 0);
				}
			}
		}
		r_matrix_pop(NULL);
		r_matrix_translate(NULL, 0, -line_spacing, 0);
		pos = k + add;
		if(text[pos] == 0)
		{
			if(line_count == -1)
				return i + 1;
			else
				return pos;
		}
		if(color != NULL && color_switch != NULL && color_switch[c_switch] == pos)
		{
			c_switch++;
			r_shader_vec4_set(NULL, sui_font_shader_location_shader, color[4 * c_switch + 0], color[4 * c_switch + 1], color[4 * c_switch + 2], color[4 * c_switch + 3]);
		}
		pos += 1;
		start_x = 0;
		if(func != NULL)
		{
			start_x = 0;
			end_x = line_size;
			func(pos_x, pos_y + (float)(i + 1) * -line_spacing * letter_size, &start_x, &end_x, user);
			if(start_x < 0)
				start_x = 0;
			if(end_x > line_size)
				end_x = line_size;
		}
		length = sui_compute_find_size(&text[pos], letter_spacing, (end_x - start_x) / letter_size, FALSE, style, &spaces, &add, &left, &f);
		start_x += f;
	}
	if(line_count == -1)
		return i;
	else
		return pos;
}

uint seduce_text_block_draw(float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint text_start, float *color, uint *color_switch, uint *scroll, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user)
{
	uint out;

	if(text == NULL || *text == '\0')
		return 0;
	r_matrix_push(NULL);
	r_matrix_translate(NULL, pos_x, pos_y, 0);
	r_matrix_scale(NULL, letter_size, letter_size, 1.0);
	r_shader_set(sui_font_shader);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	out = sui_text_block_draw_sample(pos_x, pos_y, letter_size, letter_spacing, line_size, line_spacing, line_count, style, text, text_start, color, color_switch, scroll, func, user);
	r_matrix_pop(NULL);
	return out;
}

uint sui_text_hit_test(float letter_size, float letter_spacing, const char *text, float left, float spaces, float pos_x, uint end)
{
	float f, f2, dist = 0;
	uint j;
	if(pos_x < 0)
		return 0;
	for(j = 0; j < end; j++)
	{
		if(text[j] == 0)
			return j;
		if(j == 0)
			f = 0;
		else
			f = sui_text_space(&text[j - 1]);
		if(sui_font_ref_array[text[j]] != NULL/* || text[end] == 0*/)
			f = f + letter_spacing;
		else
			f = f + letter_spacing + left / spaces;
		if(pos_x > dist - 0.5 && pos_x < f + dist - 0.5)
			return j;

		dist += f;
	}
	return j;
}

uint seduce_text_block_hit_test(float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, const char *text, float pos_x, float pos_y)
{
/*	char buf[4096];
	uint i, j, add, pos = 0, spaces, line;
	float left, f, f2, dist;
	seduce_font_init();
	add = sui_compute_find_size(text, letter_spacing, line_size / letter_size, FALSE, &spaces, &left);

	line = (-pos_y + (letter_size * 2)) / (line_spacing * letter_size);
	if(line_count < line)
		return 0;
	for(i = 0; i < line_count && i < line; i++)
		pos += sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, &spaces, &left) + 1;
	if(text[pos] == 0)
		return pos - 1;
	add = sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, &spaces, &left) + 1;
	dist = 0;
	pos_x /= letter_size;
	return  sui_text_hit_test(letter_size, letter_spacing, &text[pos], left, spaces, pos_x, add) + pos;*/
	return 0;
}


uint sui_text_block_hit_detection(float pointer_x, float pointer_y, float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint *scroll, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user)
{
	uint i, j, k = 0, length, add, pos = 0, spaces, line;
	float left, f, f2, array[SUI_TEXTURE_DRAW_CASH_SIZE * 4 * 2], uv[SUI_TEXTURE_DRAW_CASH_SIZE * 4 * 2], start_x, end_x;


	if(pointer_x < pos_x ||
		pointer_y > pos_y ||
		pointer_x > pos_x  + line_size ||
		pointer_y < pos_y - letter_spacing * (float)line_count)
			return -1;

	line = (uint)((pos_y - pointer_y) / (letter_size * line_spacing));
	if(scroll != NULL)
		line += scroll[0];

/*	for(i = 0; i < line; i++)
	{
		start_x = 0;
		end_x = line_size;
		if(func != NULL)
		{
			func(pos_x, pos_y, &start_x, &end_x, user);
			if(start_x < 0)
				start_x = 0;
			if(end_x > line_size)
				end_x = line_size;
		}
		length = sui_compute_find_size(&text[pos], letter_spacing, (end_x - start_x) / letter_size, FALSE, style, &spaces, &add, &left, &f);
		pos += add + 1;
	}*/

	start_x = 0;
	end_x = line_size;
	if(func != NULL)
	{
		func(pos_x, pos_y, &start_x, &end_x, user);
		if(start_x < 0)
			start_x = 0;
		if(end_x > line_size)
			end_x = line_size;
	}
	length = sui_compute_find_size(text, letter_spacing, (end_x - start_x) / letter_size, FALSE, style, &spaces, &add, &left, &f);
	for(i = 0; i < line; i++)
	{
		for(; pos < scroll[1] && text[pos] != 10 && text[pos] != 0; pos++);
		k = pos;
		pos = k + add;
		if(text[pos] == 0)
			return pos;
		pos += 1;
		if(func != NULL)
		{
			start_x = 0;
			end_x = line_size;
			func(pos_x, pos_y + (float)(i + 1) * -line_spacing * letter_size, &start_x, &end_x, user);
			if(start_x < 0)
				start_x = 0;
			if(end_x > line_size)
				end_x = line_size;
		}
		length = sui_compute_find_size(&text[pos], letter_spacing, (end_x - start_x) / letter_size, FALSE, style, &spaces, &add, &left, &f);
	}


/*	if(SEDUCE_BS_CODE == style)
	{
		f2 = start_x;
		for(j = 0; j < length; j++)
		{
			f = (sui_text_size(&text[pos]) - letter_spacing * 3.5) / 2.0;
			r_matrix_translate(NULL, letter_spacing * 3.5 - f + f2, 0, 0);
			if(sui_font_ref_array[text[pos]] != NULL)
				r_array_reference_draw(sui_font_array, sui_font_reference[text[pos]], GL_TRIANGLES, 0, sui_font_ref_size[text[pos]]);
			pos++;
			f2 = f;
		}
	}else*/
	{
		f2 = pos_x;
		for(j = 0; j < length; j++)
		{
			pos++;
			if(j + 1 == length)
				break;
			f = sui_text_space(&text[pos - 1]);
			if(sui_font_ref_array[text[pos]] != NULL || text[pos - j + length] == 0)
				f += letter_spacing;
			else
				f += letter_spacing + (float)left / ((float)spaces - 1.0);
			if(pointer_x < f2)
				return pos;
			f2 += f * letter_size;
		}
	}

	return pos;
/*
	if(pointer_x < pos_x)
		return FALSE;

	start_x = 0;
	end_x = line_size;
	if(func != NULL)
	{
		func(pos_x, pos_y, &start_x, &end_x, user);
		if(start_x < 0)
			start_x = 0;
		if(end_x > line_size)
			end_x = line_size;
	}
	length = sui_compute_find_size(text, letter_spacing, (end_x - start_x) / letter_size, FALSE, style, &spaces, &add, &left, &f);
	for(i = 0; i < line_count; i++)
	{
		for(; pos < scroll[1] && text[pos] != 10 && text[pos] != 0; pos++);
		r_matrix_push(NULL);
		k = pos;
		pos = k + add;
		if(text[pos] == 0)
			return pos;
		pos += 1;
		if(func != NULL)
		{
			start_x = 0;
			end_x = line_size;
			func(pos_x, pos_y + (float)(i + 1) * -line_spacing * letter_size, &start_x, &end_x, user);
			if(start_x < 0)
				start_x = 0;
			if(end_x > line_size)
				end_x = line_size;
		}
		length = sui_compute_find_size(&text[pos], letter_spacing, (end_x - start_x) / letter_size, FALSE, style, &spaces, &add, &left, &f);
	}
	return pos;*/
}

typedef struct{
	char *text;
	uint start;
}STextBlockGrab;

void sui_text_block_selection(BInputState *input, float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint text_start, uint *scroll, uint *selection, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user)
{
	static STextBlockGrab *grab = NULL;
	float start, end;
	uint i, count, click;

	if(grab == NULL)
	{
		count = betray_support_functionality(B_SF_POINTER_COUNT_MAX);
		grab = malloc((sizeof *grab) * betray_support_functionality(B_SF_POINTER_COUNT_MAX));
		for(i = 0; i < count; i++)
		{
			grab[i].text = NULL;
			grab[i].start = -1;
		}
	}


//	if(input->mode == BAM_EVENT)
	{
		for(i = 0; i < input->pointer_count; i++)
		{
		//		exit(0);
			if(input->pointers[i].button[0] &&
				!input->pointers[i].last_button[0] &&
				input->pointers[i].pointer_x > pos_x &&
				input->pointers[i].pointer_y < pos_y &&
				input->pointers[i].pointer_x < pos_x  + line_size &&
				input->pointers[i].pointer_y > pos_y - letter_spacing * (float)line_count)
			{
			

				if(func != NULL)
				{
					start = pos_x;
					end = pos_x + line_size;
					func(input->pointers[i].pointer_x, input->pointers[i].pointer_y, &start, &end, user);
					if(input->pointers[i].pointer_x > start && input->pointers[i].pointer_x < end)
						grab[i].text = text;
				}else
					grab[i].text = text;
			}
		
			if(grab[i].text == text)
			{

				if(input->pointers[i].pointer_x > pos_x &&
					input->pointers[i].pointer_y < pos_y &&
					input->pointers[i].pointer_x < pos_x  + line_size &&
					input->pointers[i].pointer_y > pos_y - letter_spacing * (float)line_count)
				{
					click = text_start + sui_text_block_hit_detection(input->pointers[i].pointer_x, input->pointers[i].pointer_y, pos_x, pos_y, letter_size, letter_spacing, line_size, line_spacing, line_count, style, &text[text_start], scroll, func, user);
					if(input->pointers[i].button[0] && !input->pointers[i].last_button[0])
						grab[i].start = selection[0] = selection[1] = click;
					if(click > grab[i].start)
					{
						selection[0] = grab[i].start;
						selection[1] = click;
					}else
					{
						selection[0] = click;
						selection[1] = grab[i].start;
					}
				}
			}

			if(!input->pointers[i].button[0])
			{
				grab[i].text = NULL;
				grab[i].start = -1;
			}
		}
	///	uint sui_text_block_hit_detection(float pointer_x, float pointer_y, float pos_x, float pos_y, float letter_size, float letter_spacing, float line_size, float line_spacing, uint line_count, SUIBlockStyle style, const char *text, uint *scroll, void (*func)(float pos_x, float pos_y, float *start, float *end, void *user), void *user);
	}
}


void sui_text_block_pos(float letter_size, float letter_spacing, float line_size, float line_spacing, const char *text, uint curser, float *pos_x, float *pos_y)
{
	uint i, j, add, pos = 0, spaces;
	float left, length = 0, f, f2;
	for(i = 0; text[pos] != 0; i++)
	{
		pos++;
//		add = sui_compute_find_size(&text[pos], letter_spacing, line_size / letter_size, FALSE, &spaces, &left);
		if(pos + add > curser && pos < curser)
		{
			for(j = pos; j < curser; j++)
			{
				f = sui_text_space(&text[i - 1]);
				if(sui_font_ref_array[text[j + 1]] != NULL || text[pos + add] == 0)
					length += f + letter_spacing;
				else
					length += f + letter_spacing + left / (float)spaces;
			}
			if(pos_x != NULL)
				*pos_x = length * letter_size;
			if(pos_y != NULL)
				*pos_y = (float)i * -letter_size * line_spacing;
			return;
		}
		pos += add;
	}
}

uint seduce_text_line_hit_test(float letter_size, float letter_spacing, const char *text, float pos_x)
{
	return sui_text_hit_test(letter_size, letter_spacing, text, 0, 1, pos_x / letter_size, -1);
}
