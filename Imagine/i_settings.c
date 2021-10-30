#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "forge.h"

typedef enum{
	IMAGINE_ST_UNDEFINED,
	IMAGINE_ST_BOOLEAN,
	IMAGINE_ST_INTEGER,
	IMAGINE_ST_DOUBLE,
	IMAGINE_ST_TEXT,
	IMAGINE_ST_COUNT
}ImagineSettingType;

typedef struct{
	char	name[32];
	ImagineSettingType	type;
	char	*comment;
	union{
		boolean toggle;
		char	*text;
		double	real;
		int	integer;
	}data;
}ImagineSetting;

struct{
	ImagineSetting	*array;
	uint			count;
	uint			version;
}ImagineGlobalSettings;

static void init_setting(void)
{
	static boolean init = FALSE;
	if(init)
		return;
	init = TRUE;
	ImagineGlobalSettings.array = malloc((sizeof *ImagineGlobalSettings.array) * 128);
	ImagineGlobalSettings.count = 0;
	ImagineGlobalSettings.version = 345;
}


static ImagineSetting * add_setting(const char *name, const char *comment)
{
	uint i;
	init_setting();
	for(i = 0; i < 31 && name[i] != 0; i++)
		ImagineGlobalSettings.array[ImagineGlobalSettings.count].name[i] = name[i];
	ImagineGlobalSettings.array[ImagineGlobalSettings.count].name[i] = 0;
	ImagineGlobalSettings.array[ImagineGlobalSettings.count].comment = comment;
	ImagineGlobalSettings.array[ImagineGlobalSettings.count].type = IMAGINE_ST_UNDEFINED;
	ImagineGlobalSettings.array[ImagineGlobalSettings.count].data.integer = 1;
	return &ImagineGlobalSettings.array[ImagineGlobalSettings.count++];
}

static ImagineSetting * get_setting(const char *name)
{
	uint i;
	init_setting();
	for(i = 0; i < ImagineGlobalSettings.count; i++)
		if(strcmp(ImagineGlobalSettings.array[i].name, name) == 0)
			return &ImagineGlobalSettings.array[i];
	return NULL;
}

boolean imagine_setting_boolean_get(const char *setting, boolean default_value, const char *comment)
{
	ImagineSetting	 *s;
	char *text;
	uint i;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
	{
		s = add_setting(setting, comment);
		s->data.toggle = default_value;
		s->type = IMAGINE_ST_BOOLEAN;

	}else if(s->type != IMAGINE_ST_BOOLEAN)
	{
		text = s->data.text;
		s->data.toggle = default_value;
		for(i = 0; text[i] != 0 && text[i] <= 32; i++);
		if(text[i] != 0)
		{
			if(text[i] == 'T' || text[i] == 't')
				s->data.toggle = TRUE;
			if(text[i] == 'F' || text[i] == 'f' || (text[i] > '0' && text[i] <= '9'))
				s->data.toggle = FALSE;
		}
		free(text);
		s->type = IMAGINE_ST_BOOLEAN;
	}
	if(comment != NULL)
		s->comment = comment;
	return s->data.toggle;
}


void imagine_setting_boolean_set(const char *setting, boolean value, const char *comment)
{
	ImagineSetting	 *s;
	char *text;
	uint i;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
		s = add_setting(setting, comment);
	s->data.toggle = value;
	if(comment != NULL)
		s->comment = comment;
	s->type = IMAGINE_ST_BOOLEAN;
}

int imagine_setting_integer_get(const char *setting, int default_value, const char *comment)
{
	ImagineSetting	 *s;
	char *text;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
	{
		s = add_setting(setting, comment);
		s->data.integer = default_value;
		s->type = IMAGINE_ST_INTEGER;
	}else if(s->type != IMAGINE_ST_INTEGER)
	{
		text = s->data.text;
		s->data.integer = default_value;
		if(sscanf(text, "%u", &s->data.integer) == 1)
			free(text);
		s->type = IMAGINE_ST_INTEGER;
	}
	if(comment != NULL)
		s->comment = comment;
	return s->data.integer;
}


void imagine_setting_integer_set(const char *setting, int value, const char *comment)
{
	ImagineSetting	 *s;
	char *text;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
		s = add_setting(setting, comment);
	s->data.integer = value;
	s->type = IMAGINE_ST_INTEGER;
	if(comment != NULL)
		s->comment = comment;
}


double imagine_setting_double_get(const char *setting, double default_value, const char *comment)
{
	ImagineSetting	 *s;
	char *text;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
	{
		s = add_setting(setting, comment);
		s->data.real = default_value;
		s->type = IMAGINE_ST_DOUBLE;

	}else if(s->type != IMAGINE_ST_DOUBLE)
	{
		text = s->data.text;
		s->data.real = default_value;
		if(sscanf(text, "%f", &s->data.real) == 1)
			free(text);
		s->type = IMAGINE_ST_DOUBLE;
	}
	if(comment != NULL)
		s->comment = comment;
	return s->data.real;
}


void imagine_setting_double_set(const char *setting, double value, const char *comment)
{
	ImagineSetting	 *s;
	char *text;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
		s = add_setting(setting, comment);
	s->data.real = value;
	s->type = IMAGINE_ST_DOUBLE;
	if(comment != NULL)
		s->comment = comment;
}


char *imagine_setting_text_get(const char *setting, char *default_text, const char *comment)
{
	ImagineSetting	 *s;
	char *text;
	uint i;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
	{
		s = add_setting(setting, comment);
		for(i = 0; default_text[i] != 0; i++);
		text = malloc((sizeof *text) * (i + 1));
		for(i = 0; default_text[i] != 0; i++)
			text[i] = default_text[i];
		text[i] = 0;
		s->data.text = text;
		s->type = IMAGINE_ST_TEXT;
	}
	if(comment != NULL)
		s->comment = comment;
	return s->data.text;
}

void imagine_setting_text_set(const char *setting, char *text, const char *comment)
{
	ImagineSetting	 *s;
	char *t;
	uint i;
	init_setting();
	s = get_setting(setting);
	if(s == NULL)
		s = add_setting(setting, comment);
	for(i = 0; text[i] != 0; i++);
	t = malloc((sizeof *t) * (i + 1));
	for(i = 0; text[i] != 0; i++)
		t[i] = text[i];
	t[i] = 0;
	s->data.text = t;
	s->type = IMAGINE_ST_TEXT;
	if(comment != NULL)
		s->comment = comment;
}


boolean imagine_setting_test(const char *setting)
{
	return NULL != get_setting(setting);
}

void imagine_settings_save(const char *file_name)
{
	uint i;
	FILE *settings;
	init_setting();
	settings = fopen(file_name, "w");
	fprintf(settings, "<?xml version=\"1.0\"?>\n<ROOT>\n");


	for(i = 0; i < ImagineGlobalSettings.count; i++)
	{
		switch(ImagineGlobalSettings.array[i].type)
		{
			case IMAGINE_ST_UNDEFINED :
				fprintf(settings, "\t<%s>%s</%s>", ImagineGlobalSettings.array[i].name, ImagineGlobalSettings.array[i].data.text, ImagineGlobalSettings.array[i].name);
			break;
			case IMAGINE_ST_BOOLEAN :
				if(ImagineGlobalSettings.array[i].data.toggle)
					fprintf(settings, "\t<%s>TRUE</%s>", ImagineGlobalSettings.array[i].name, ImagineGlobalSettings.array[i].name);
				else
					fprintf(settings, "\t<%s>FALSE</%s>", ImagineGlobalSettings.array[i].name, ImagineGlobalSettings.array[i].name);
			break;
			case IMAGINE_ST_INTEGER :
				fprintf(settings, "\t<%s>%i</%s>", ImagineGlobalSettings.array[i].name, ImagineGlobalSettings.array[i].data.integer, ImagineGlobalSettings.array[i].name);
			break;
			case IMAGINE_ST_DOUBLE :
				fprintf(settings, "\t<%s>%f</%s>", ImagineGlobalSettings.array[i].name, ImagineGlobalSettings.array[i].data.real, ImagineGlobalSettings.array[i].name);
			break;
			case IMAGINE_ST_TEXT :
				fprintf(settings, "\t<%s>%s</%s>", ImagineGlobalSettings.array[i].name, ImagineGlobalSettings.array[i].data.text, ImagineGlobalSettings.array[i].name);
			break;
		}
		if(ImagineGlobalSettings.array[i].comment == NULL)
			fprintf(settings, "\n");
		else
			fprintf(settings, " <!-- %s -->\n", ImagineGlobalSettings.array[i].comment);
	}
	fprintf(settings, "</ROOT>\n");
	fclose(settings);
}

void imagine_settings_load(const char *file_name)
{
	char	*buffer, type[256], *text;
	uint	i, j, size;
	FILE	*settings;
	ImagineSetting *s;
	init_setting();

	if((settings = fopen(file_name, "r")) != NULL)
	{
		fseek(settings , 0, SEEK_END);
		size = ftell(settings) + 1;
		fseek(settings , 0, SEEK_SET);
		buffer = malloc(size + 1);
		fread(buffer, sizeof(char), size, settings);
		buffer[size] = 0;
		for(i = 0; buffer[i] != 0; i++)
		{
			if(buffer[i] == '<')
			{
				i++;
				if(buffer[i] != '/' && buffer[i] != '!')
				{
					for(j = 0; buffer[i] != 0 && buffer[i] != '>'; i++)
						type[j++] = buffer[i];
					type[j] = 0;
					i++;
					for(j = 0; buffer[i + j] != 0 && buffer[i + j] != '<'; j++);

					if(buffer[i + j] != 0 && j != 0)
					{
						j++;
						if(buffer[i + j] == '/')
						{
							text = malloc(sizeof(char) * j);
							for(j = 0; buffer[i + j] != 0 && buffer[i + j] != '<'; j++)
								text[j] = buffer[i + j];
							text[j] = 0;
							s = add_setting(type, NULL);
							s->data.text = text;
						}
					}
				}
			}
		}
		fclose(settings);
		free(buffer);
	}
}


void imagine_settings_test()
{
/*	sui_setting_boolean("BOOLEAN", TRUE, "This is a switch.");
	sui_setting_integer("IINTEGER", 1138, "This is an integer number.");
	sui_setting_double("DOUBLE", 3.14, "This is a double precision floating point value.");
	sui_setting_text("TEXT","hello world", "This is some text.");*/

	imagine_settings_load("setings.xml");
	imagine_settings_save("setings2.xml");
}
