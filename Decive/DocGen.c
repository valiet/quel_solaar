#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include "forge.h"

#define DG_NAME_SIZE 64

typedef enum{
	BD_IT_DEFINE,
	BG_IT_STRUCT,
	BG_IT_ENUM,
	BG_IT_FUNCTION,
	BG_IT_GROUP,
	BG_IT_MEMBER,
	BG_IT_COUNT
}DGItemType;

typedef struct{
	char *file;
	char *source;
	DGItemType type;
	char *comment;
	char name[DG_NAME_SIZE];
}DGItem;


void dg_generate_html_link(char *link, DGItem *file)
{
	char *html = ".html", *text;
	uint i, j;
	for(i = 0; file->file[i] != 0 && file->file[i] != '.'; i++)
		link[i] = file->file[i];
	link[i++] = '_'; 

	if(file->name[0] != 0)
		text = file->name;
	else
		text = file->source;

	for(j = 0; text[j] != 0 && j < 32; j++)
	{
		if(text[j] <= 32)
			link[i + j] = '_';
		else
			link[i + j] = text[j];
	}
	i += j;
	for(j = 0; html[j] != 0; j++)
		link[i + j] = html[j];
	link[i + j] = 0;
}

boolean dg_is_next(char *text, char *search)
{
	uint i;
	for(i = 0; text[i] == search[i] && search[i] != 0; i++);
	return search[i] == 0 && text[i] <= 32;
}

void dg_print_text(FILE *f, char *text, DGItem *items, uint item_count)
{
	uint i, j, k;
	char link[256]; 
	for(i = 0; text[i] != 0; i++)
	{
		for(j = 0; j < item_count; j++)
		{
			if(items[j].name[0] != 0 && dg_is_next(&text[i], items[j].name))	
			{
				for(k = j; k != 0 && items[k].type != BG_IT_GROUP; k--);
				dg_generate_html_link(link, &items[k]);
				fprintf(f, "<a href=\"%s#%s\">", link, items[j].name);
				for(; text[i] > 32; i++)
					fputc(text[i], f);
				fprintf(f, "</a>");
			}
		}
		if(dg_is_next(&text[i], "http://"))	
		{
			fprintf(f, "<a href=\"");
			for(j = 0; text[j + i] > 32; j++)
				fputc(text[j + i], f);
			i += 7;
			fprintf(f, "\">");
			for(j = 0; text[j + i] > 32; j++)
				fputc(text[j + i], f);
			fprintf(f, "</a>");
			i += j;
		}
		if(text[i] > 32)
		{
			for(j = 0; text[i + j] > 32 && text[i + j] != '.'; j++);
			if(text[i + j] == '.' && (text[i + j + 1] == 'c' || text[i + j + 1] == 'h') && text[i + j + 2] <= 32)
			{
				fprintf(f, "<a href=\"");
				for(j = 0; text[j + i] > 32; j++)
					fputc(text[j + i], f);
				fprintf(f, "\">");
				for(j = 0; text[j + i] > 32; j++)
					fputc(text[j + i], f);
				fprintf(f, "</a>");
				i += j;
			}
		}
		if(text[i] == '\n')
			fprintf(f, "<BR>\n");
		else
			fputc(text[i], f);

	}
}

uint dg_find_in(char *text, char *search)
{
	uint i, j;
	for(i = 0; text[i] != 0; i++)
	{
		for(j = 0; text[i + j] == search[j] && search[j] != 0; j++);
		if(search[j] == 0)
			return i;
	}
	return -1;
}

char *dg_parce_section(char *text, uint *pos, char *start, char *end)
{
	uint i, j, k;
	for(i = 0; text[i + *pos] == start[i] && start[i] != 0; i++);
	if(start[i] != 0)
		return NULL;
	for(j = 0; text[i + j + *pos] != 0; j++)
	{
		for(k = 0; text[i + j + k + *pos] == end[k] && end[k] != 0; k++);
		if(end[k] == 0)
		{
			text[i + j + *pos] = 0;
			while(text[*pos + i] <= 32)
				i++;
			text = &text[*pos + i];
			*pos = i + j + k + *pos - 2;
			return text;
		}
	}
	return NULL;
}
/*
void dg_item_print(DGItem *items, uint item, FILE *html)
{
	fprintf("");
}*/



void dg_extract_comment(char *text, DGItem *items)
{
	char *p;
	uint i, j;
	items->comment = NULL;
	for(i = 0; text[i] != 0; i++)
	{
		j = i;
		if((p = dg_parce_section(text, &i, "/*", "*/")) != NULL)
		{
			text[j] = 0;
			items->comment = p; 
			return;
		}
		if((p = dg_parce_section(text, &i, "//", "\n")) != NULL)
		{
			text[j] = 0;
			items->comment = p; 
			return;
		}
	}
}


char *dg_load_file(char *path, uint *size)
{
	char *text, merge_source[1024];
	FILE *f;
	if((f = fopen(path, "r")) == NULL)
	{
		sprintf(merge_source, "../../Mergesource/%s", path);
		f = fopen(merge_source, "r");

		if(f == NULL)
		{
			sprintf(merge_source, "../Mergesource/%s", path);
			f = fopen(merge_source, "r");
		}
	}
	if(f != NULL)
	{
		fseek(f, 0, SEEK_END);
		*size = ftell(f);
		rewind(f);
		text = malloc(size + 1);
		*size = fread(text, 1, *size, f);
		text[*size] = 0;
		fclose(f);
		return text;
/*
		struct stat st;
		if(stat(path, &st) == 0)
        {

			*size = st.st_size;
			printf("Size = %u %s\n", *size, path);
			text = malloc((sizeof *text) * (*size + 1));
			*size = fread(text, (sizeof *text), *size, f);
			text[*size - 1] = 0;
			fclose(f);
			return text;
		}*/
	}
	printf("DocGen ERROR: file %s not found\n", path);
	return NULL;
}

void dg_html_header(FILE *f, char *name, DGItem *items, uint item_count, char *path, char **pathes, uint path_count)
{
	static char *intro = NULL;
	static char *midtro = NULL;
	uint i, j, group, size;
	float color[3];
	char file[256];
	fprintf(f, "<!DOCTYPE html>\n"
				"<html>\n"
				"<head>\n"
				"<meta charset=\"UTF-8\">\n"
				"<title>%s</title>\n"
				"<style type=\"text/css\">\n", name);
	for(i = 0; i < path_count && pathes[i] != path; i++);
	if(i < path_count)
	{
		f_hsv_to_rgb(color, ((float)i + 0.5)  / (float)path_count, 1.0, 0.85);
		fprintf(f, "font.header {color:rgb(%u,%u,%u);}\n", (uint)(color[0] * 255.0), (uint)(color[1] * 255.0), (uint)(color[2] * 255.0));
		fprintf(f, "h2 {color:rgb(%u,%u,%u);}\n", (uint)(color[0] * 255.0), (uint)(color[1] * 255.0), (uint)(color[2] * 255.0));
	}
	if(intro == NULL)
		intro = dg_load_file("DocGen_intro.html", &size);

	if(midtro == NULL)
		midtro = dg_load_file("DocGen_midtro.html", &size);
	fprintf(f, "%s", intro);
	fprintf(f, "<dl>\n");
	fprintf(f, "<dt><a href=\"index.html\">Home</a></dt>\n");
	for(i = 0; i < path_count; i++)
	{
//		dg_generate_html_link(file, &items[group]);
		for(j = 0;  pathes[i][j] != 0 && pathes[i][j] != '.'; j++)
			file[j] = pathes[i][j];
		file[j] = 0;
		f_hsv_to_rgb(color, ((float)i + 0.5)  / (float)path_count, 1.0, 0.85);
		fprintf(f, "<dt><a href=\"%s.html\" style=\"color:rgb(%u,%u,%u);\">%s</a></dt>", file, (uint)(color[0] * 255.0), (uint)(color[1] * 255.0), (uint)(color[2] * 255.0), file);
		if(pathes[i] == path)
		{
			group = -1;
			for(j = 0; j < item_count; j++)
			{
				if(items[j].file == path)
				{
					if(items[j].type == BG_IT_GROUP)
						group = j;
					if(items[j].type == BG_IT_FUNCTION && group != -1)
					{
						dg_generate_html_link(file, &items[group]);
						if(items[group].name[0] != 0)
							fprintf(f, "<dd><a href=\"%s\">%s</a></dd>", file, items[group].name);
						else
							fprintf(f, "<dd><a href=\"%s\">%s</a></dd>", file, items[group].source);
						group = -1;
					}
				}
			}
			if(path != NULL)
			{
				for(j = 0; path[j] != '.' && path[j] > 32 && j < 16; j++)
					file[j] = path[j];
				file[j] = 0;
				fprintf(f, "<dd><a href=\"%s_defines.html\">Defines</a></dd>", file);
			}
		}
	}
	fprintf(f, "</dl>\n");

	fprintf(f, midtro);
}

void dg_html_footer(FILE *f)
{
	static char *text = NULL;
	uint size;
	if(text == NULL)
		text = dg_load_file("DocGen_outro.html", &size);
	if(text != NULL)
		fprintf(f, text);
}

uint dg_parce_file(char *path, DGItem *items)
{
	char *text, *p;
	uint i, j, k, size, item_count = 0;
	FILE *f;
	printf("loading %s\n", path);
	if((text = dg_load_file(path, &size)) != NULL)
	{
		for(i = 0; i < size; i++)
		{	
			if((p = dg_parce_section(text, &i, "#define", "\n")) != NULL)
			{
				items[item_count].file = path;
				items[item_count].type = BD_IT_DEFINE;
				items[item_count].source = p;
				dg_extract_comment(p, &items[item_count]);
				for(j = 0; p[j] > 32 && j < DG_NAME_SIZE - 1; j++)
					items[item_count].name[j] = p[j];
				items[item_count].name[j] = 0;
				item_count++;
			}
			if((p = dg_parce_section(text, &i, "typedef struct{", "}")) != NULL)
			{
				items[item_count].file = path;
				items[item_count].type = BG_IT_STRUCT;
				items[item_count].source = p;
				items[item_count].comment = NULL;
				for(j = 0; text[i] != 0 && j < DG_NAME_SIZE - 1 && text[i] != ';'; i++)
					items[item_count].name[j++] = text[i];
				items[item_count].name[j] = 0;
				item_count++;
			}
			if((p = dg_parce_section(text, &i, "typedef enum{", ";")) != NULL)
			{
				items[item_count].file = path;
				items[item_count].type = BG_IT_ENUM;
				items[item_count].source = p;
				items[item_count].comment = NULL;
				for(j = k = 0; p[j] != '}' && p[j] != 0; j++);
				if(p[j] == '}')
				{
					for(j++; p[j + k] != 0 && k < DG_NAME_SIZE - 1; k++)
						items[item_count].name[k] = p[j + k];
				}
				items[item_count].name[k] = 0;
				item_count++;
			}
			if((p = dg_parce_section(text, &i, "extern", "\n")) != NULL)
			{
				items[item_count].file = path;
				items[item_count].type = BG_IT_FUNCTION;
				items[item_count].source = p;
				items[item_count].name[0] = 0;
				dg_extract_comment(p, &items[item_count]);
				for(j = 0; p[j] > 32; j++);
				for(; p[j] <= 32 && p[j] != 0; j++);
				for(; p[j] == '*'; j++);
				for(k = 0; p[j + k] != '(' && p[j + k] != 0 && k < DG_NAME_SIZE - 1; k++)
					items[item_count].name[k] = p[j + k];
				items[item_count].name[k] = 0;
				if(p[j + k] != 0 && k < DG_NAME_SIZE - 1)
					item_count++;
			}
			if((p = dg_parce_section(text, &i, "/*", "*/")) != NULL || 
				(p = dg_parce_section(text, &i, "//", "\n")) != NULL)
			{
				items[item_count].file = path;
				items[item_count].type = BG_IT_GROUP;
				items[item_count].name[0] = 0;
				if(p[1] == '-')
				{
					for(j = 2; p[j] == '-'; j++);
					for(; p[j] <= 32 && p[j] != 0; j++);
					if(p[j] != 0)
					{
						for(k = 0; p[j] != '-' && p[j] != 0 && k < DG_NAME_SIZE - 1; j++)
							items[item_count].name[k++] = p[j];
						items[item_count].name[k] = 0;
						for(; (p[j] == '-' || p[j] <= 32) && p[j] != 0; j++);
						p = &p[j];
					}
				}
				items[item_count].source = p;
				items[item_count].comment = NULL;
				item_count++;
			}
		}
	}

	return item_count;

}

void dg_generate_html_enum_struct(FILE *f, DGItem *item)
{
	char buffer[4096];
	uint i, j;
	boolean comment = FALSE;
	if(item->type == BG_IT_STRUCT)
		fprintf(f, "<p class=\"header\">Struct:</p> <p class=\"code\">%s<p>\n", item->name);
	else
		fprintf(f, "<p class=\"header\">Enum:</p> <p class=\"code\">%s<p>\n", item->name);

	fprintf(f, "<table border=\"1\">\n");
	comment = TRUE;

	if(item->type == BG_IT_STRUCT)
		printf(item->source);
	for(i = 0; item->source[i] != 0; i++)
	{
		while(item->source[i] <= 32 && item->source[i] != 0)
			i++;
		if(item->source[i] == '}')
			break;
		if(item->source[i] == '/')
		{
			i++;
			if(item->source[i] == '/')
			{
				i++;
				for(j = 0; j < 4095 && item->source[i + j] != 0 && item->source[i + j] != '\n'; j++)
					buffer[j] = item->source[i + j];
				buffer[j] = 0;
				i += j;
				fprintf(f, "<td valign=top>\n");
				fprintf(f, "<font class=\"header\">Description:</font>\n");
				fprintf(f, "<font class=\"description\">%s</font>\n", buffer);
				fprintf(f, "</td></tr>\n");
				comment = TRUE;
			}
			if(item->source[i] == '*')
			{
				i++;
				for(j = 0; j < 4095 && item->source[i + j] != 0 && item->source[i + j] != '*'; j++)
					buffer[j] = item->source[i + j];
				buffer[j] = 0;
				i += j + 1;
				fprintf(f, "<td valign=top>\n");
				fprintf(f, "<font class=\"header\">Description:</font>\n");
				fprintf(f, "<font class=\"description\">%s</font>\n", buffer);
				fprintf(f, "</td></tr>\n");
				comment = TRUE;
			}
		}else if(item->source[i] != 0)
		{
			for(j = 0; item->source[i + j] != 0 && j < 4095 && item->source[i + j] != ';' && item->source[i + j] != ',' && item->source[i + j] != '/' && item->source[i + j] != '}' && (item->source[i + j] > 32 || item->type == BG_IT_STRUCT); j++)
				buffer[j] = item->source[i + j];
			buffer[j] = 0;
			if(!comment)
				fprintf(f, "</tr>\n");
			fprintf(f, "<tr><td valign=top><p class=\"code\">%s</p></td>\n", buffer);
			i += j;
		}
	}
	fprintf(f, "</table>\n");
}


void dg_generate_html(char *path, DGItem *items, uint count, char **pathes, uint path_count)
{
	FILE *f;
	char name[64], buffer[32], file[128] = {0};
	uint i, j, group = -1;
	boolean first;
	for(i = 0; path[i] != 0 && path[i] != '.' && i < 32; i++)
		buffer[i] = path[i];
	buffer[i] = 0;
	sprintf(name, "./docs/%s.html", buffer);
	if((f = fopen(name, "w")) != NULL)
	{
		dg_html_header(f, buffer, items, count, path, pathes, path_count);
		for(i = 0; i < count && (items[i].type != BG_IT_GROUP || items[i].file != path); i++);
		if(i < count)
		{
			fprintf(f, "<p><H2>%s</H2></p>\n", buffer);
			fprintf(f, "<p class=\"group\">\n");
			dg_print_text(f, items[i].source, items, count);
			fprintf(f, "</p>\n");
			i++;
		}else
			i = 0;
		fprintf(f, "<p class=\"header\">Contents:</p>", items[i].name);
		fprintf(f, "<dl>\n");
		for(; i < count; i++)
		{
			if(items[i].file == path)
			{
				if(items[i].type == BG_IT_GROUP)
					group = i;
				else if(items[i].name[0] != 0 && items[i].type == BG_IT_FUNCTION)
				{
					if(group != -1)
					{
						dg_generate_html_link(file, &items[group]);
						if(items[group].name[0] != 0)
							fprintf(f, "<dt><a href=\"%s\">%s</a></dt>\n", file, items[group].name);
						else
							fprintf(f, "<dt><a href=\"%s\">%s</a></dt>\n", file, items[group].source);
						group = -1;
					}
					fprintf(f, "<dd><a href=\"%s#%s\">%s</a></dd>\n", file, items[i].name, items[i].name);
				}
			}
		}
		fprintf(f, "<dt><a href=\"%s_defines.html\">Defines</a></dt>\n", buffer);
		fprintf(f, "</dl>\n");
		dg_html_footer(f);
		fclose(f);
	}
	f = NULL;
	for(i = 0; i < count; i++)
	{
		if(items[i].file == path)
		{
			if(items[i].type == BG_IT_GROUP)
				group = i;
			else if(items[i].name[0] != 0 && items[i].type == BG_IT_FUNCTION)
			{
				if(group != -1)
				{
					dg_generate_html_link(file, &items[group]);
					sprintf(name, "./docs/%s", file);
					if(f != NULL)
					{
						dg_html_footer(f);
						fclose(f);
					}
					if((f = fopen(name, "w")) != NULL)
					{
						dg_html_header(f, buffer, items, count, path, pathes, path_count);
						if(items[group].name[0] != 0)
							fprintf(f, "<h1><p>%s</p></h1>\n", items[group].name);
						fprintf(f, "<p class=\"group\">", items[group].source);
						dg_print_text(f, items[group].source, items, count);
						fprintf(f, "</p>\n");
						group = -1;
					}
				}
				if(f != NULL)
				{
					fprintf(f, "<H3><p id=\"%s\">%s</p></H3>\n", items[i].name, items[i].name);
					fprintf(f, "<p class=\"code\">%s</p>\n", items[i].source);
					if(items[i].comment != NULL)
					{
						fprintf(f, "<font class=\"header\">Description:</font>\n");
						fprintf(f, "<font class=\"description\">%s</font>\n", items[i].comment);
					}
					first = TRUE;
					for(j = 0; j < count; j++)
					{
						if(items[j].file == path && items[j].name[0] != 0 && (items[j].type == BG_IT_ENUM))
						{
							if(-1 != dg_find_in(items[i].source, items[j].name))
							{
								if(first)
									fprintf(f, "<p>Types:<p>\n");
								dg_generate_html_enum_struct(f, &items[j]);
								first = FALSE;
							}
						}
					}
				}
			}	
		}
	}
	if(f != NULL)
	{
		dg_html_footer(f);
		fclose(f);
	}
	sprintf(file, "docs/%s_defines.html", buffer);

	if((f = fopen(file, "w")) != NULL)
	{
		dg_html_header(f, buffer, items, count, path, pathes, path_count);
		for(i = 0; i < count; i++)
		{
			if(items[i].file == path)
			{
				if(items[i].type == BG_IT_GROUP)
					group = i;
				else if(items[i].name[0] != 0 && (items[i].type == BD_IT_DEFINE || items[i].type == BG_IT_STRUCT || items[i].type == BG_IT_ENUM))
				{
					if(group != -1)
					{
						dg_generate_html_link(file, &items[group]);
						if(items[group].name[0] != 0)
							fprintf(f, "<p class=\"code\">%s</p>\n", file, items[group].name);
						else
							fprintf(f, "<p class=\"code\">%s</p>\n", file, items[group].source);
						group = -1;
					}
					if(items[i].type == BD_IT_DEFINE)
					{
						if(items[i].comment == NULL)
						{
							fprintf(f, "<p class=\"code\">%s</p>\n", items[i].source);
						}else
						{
							fprintf(f, "<H3><p id=\"%s\">%s<p></H3>\n", items[i].name, items[i].name);
							fprintf(f, "<p class=\"code\">%s</p>\n", items[i].source);
							fprintf(f, "<font class=\"header\">Description:</font>\n");
							fprintf(f, "<font class=\"description\">%s</font>\n", items[i].comment);
						}
					}else
						dg_generate_html_enum_struct(f, &items[i]);
				}
			}
		}

		group = -1;
		dg_html_footer(f);
		fclose(f);
	}
}



void main()
{
	FILE *f;
	DGItem *items;
	char *pathes[] = {"forge.h", "imagine.h", "betray.h", "betray_plugin_api.h", "relinquish.h", "testify.h", "seduce.h"};
	uint i, count = 0, file_count = 6;

	items = malloc((sizeof *items) * 1024);
	for(i = 0; i < file_count; i++)
	{
		count += dg_parce_file(pathes[i], &items[count]);
	}
	for(i = 0; i < file_count; i++)
		dg_generate_html(pathes[i], items, count, pathes, file_count);
	
	if((f = fopen("docs/index.html", "w")) != NULL)
	{
		char *text;
		dg_html_header(f, "Quel Solaar Source Code", items, count, NULL, pathes, file_count);
		text = dg_load_file("DocGen_index.html", &i);
		dg_print_text(f, text, items, count);
	//	fprintf(f, text);
		dg_html_footer(f);
		fclose(f);
	}
	exit(0);
/*	{
		uint *X = NULL;
		X[0] = 0;
	}*/
}