
#include <stdio.h>
#include "forge.h"
#include "imagine.h"

#define FCD_MAX_NAME_LENGTH 128
#define FCD_MAX_PARAM_COUNT 32

typedef struct{
	char name[FCD_MAX_NAME_LENGTH];
	char type[FCD_MAX_NAME_LENGTH];
	boolean pointer;
}FCDParam;

typedef struct{
	char name[FCD_MAX_NAME_LENGTH];
	char return_type[FCD_MAX_NAME_LENGTH];
	FCDParam params[FCD_MAX_PARAM_COUNT];
	uint param_count;
	uint line;
	char file[FCD_MAX_NAME_LENGTH];
}FCDFunction;

struct{
	FCDFunction *functions;
	uint function_count;
}FCDFunctionStorage;


void fcd_function_print(FILE *rapport, FCDFunction *func)
{
	uint i, j;
	fprintf(rapport, "%s %s(", func->return_type, func->name);
	for(j = 0; j < func->param_count; j++)
	{
		if(j == 0)
			fprintf(rapport, "%s ", func->params[j].type);
		else
			fprintf(rapport, ", %s ", func->params[j].type);
		for(i = 0; i < func->params[j].pointer; i++)
			fprintf(rapport, "*");
		fprintf(rapport, "%s", func->params[j].name);
	}
	fprintf(rapport, "); ------------------------- %s:%u\n", func->file, func->line);
}


void fcd_function_error_print(FILE *rapport, FCDFunction *func_a, FCDFunction *func_b, char *message, uint param)
{
	uint i;
	fprintf(rapport, "FCD Warning: %s\n\t", message);
	fcd_function_print(rapport, func_a);	
	fprintf(rapport, "\t");
	fcd_function_print(rapport, func_b);
	return;


	if(!f_text_compare(func_a->return_type, func_b->return_type))
		fprintf(rapport, "\t%s/%s %s\n", func_a->return_type, func_b->return_type, func_a->name);
	else
		fprintf(rapport, "\t%s %s\n", func_a->return_type, func_a->name);
	fprintf(rapport, "\t%s:%u\n",func_a->file, func_a->line);
	if(param != -1)
	{
		fprintf(rapport, "\t\t%s ", func_a->params[param].type);
		for(i = 0; i < func_a->params[param].pointer; i++)
			fprintf(rapport, "*", func_a->params[param].type);
		fprintf(rapport, "%s (param = %u)\n", func_a->params[param].name, param);	 
	}
	fprintf(rapport, "\t%s:%u\n",func_b->file, func_b->line);
	if(param != -1)
	{
		fprintf(rapport, "\t\t%s ", func_b->params[param].type);
		for(i = 0; i < func_b->params[param].pointer; i++)
			fprintf(rapport, "*", func_b->params[param].type);
		fprintf(rapport, "%s (param = %u)\n", func_b->params[param].name, param);	 
	}
}


void fcd_function_add(FILE *rapport, FCDFunction *func)
{
	uint i, j, k;
	if(f_text_compare(func->name, "main"))
		return;

	for(i = 0; i < FCDFunctionStorage.function_count; i++)
	{
		if(f_text_compare(func->name, FCDFunctionStorage.functions[i].name))
		{
			if(!f_text_compare(func->return_type, FCDFunctionStorage.functions[i].return_type))
			{
				fcd_function_error_print(rapport, &FCDFunctionStorage.functions[i], func, "return functions dont match!", -1);
				fprintf(rapport, "\treturning %s as compared %s\n",  FCDFunctionStorage.functions[i].return_type, func->return_type);
			}
			if(func->param_count == FCDFunctionStorage.functions[i].param_count)
			{
				for(j = 0; j < func->param_count; j++)
				{
					if(func->params[j].pointer != FCDFunctionStorage.functions[i].params[j].pointer)
						fcd_function_error_print(rapport, &FCDFunctionStorage.functions[i], func, "miss match in indirection!", j);
					else if(func->params[j].pointer == 0)
						if(!f_text_compare(func->params[j].type, FCDFunctionStorage.functions[i].params[j].type))
							fcd_function_error_print(rapport, &FCDFunctionStorage.functions[i], func, "miss match in param types!", j);
				/*	if(!f_text_compare(func->params[j].name, FCDFunctionStorage.functions[i].params[j].name))
						fcd_function_error_print(rapport, &FCDFunctionStorage.functions[i], func, "miss match in param names!", j);*/
					
				}
			}else
				fcd_function_error_print(rapport, &FCDFunctionStorage.functions[i], func, "functions have different number of params!", -1);
			return;
		}
	}
	if(FCDFunctionStorage.function_count % 64 == 0)
		FCDFunctionStorage.functions = realloc(FCDFunctionStorage.functions, (sizeof *FCDFunctionStorage.functions) * (FCDFunctionStorage.function_count + 64));
	FCDFunctionStorage.functions[FCDFunctionStorage.function_count++] = *func;
} 


boolean fcd_character_test(char character)
{
	return (character >= '0' && character <= '9') ||
		(character >= 'A' && character <= 'Z') ||
		(character >= 'a' && character <= 'z') ||
		character == '_';
}




uint fcd_parse_function_params(FILE *rapport, char *file, char *return_type, char *function_name, char *file_name, uint line_number)
{
	FCDFunction func;
	char word_a[3][1024];
	uint i, j, parenthesis = 1, word_pos = 0, word_count = 0;
	boolean pointer = FALSE;
	
	f_text_copy(FCD_MAX_NAME_LENGTH, func.name, function_name);
	f_text_copy(FCD_MAX_NAME_LENGTH, func.return_type, return_type);
	f_text_copy(FCD_MAX_NAME_LENGTH, func.file, file_name);
	func.line = line_number;
	func.param_count = 0;
//	printf("function found! %s returning %s on line %u in file %s\n", function_name, return_type, line_number, file_name);
	for(i = 0; parenthesis != 0; i++)
	{
		if(file[i] == '(')
			parenthesis++;
		if(file[i] == ')')
			parenthesis--;

		if(fcd_character_test(file[i]))
		{
			if(word_pos == 0)
				word_count++;
			word_a[word_count % 3][word_pos++] = file[i];
		}else
		{
			if(word_pos != 0)
			{
				word_a[word_count % 3][word_pos] = 0;
				word_pos = 0;
			}
			if(file[i] == '*')
				pointer++;
			if(file[i] == ',' || (file[i] == ')' && parenthesis == 0 && word_count > 1))
			{
				f_text_copy(FCD_MAX_NAME_LENGTH, func.params[func.param_count].name, word_a[word_count % 3]);
				f_text_copy(FCD_MAX_NAME_LENGTH, func.params[func.param_count].type, word_a[(word_count + 2) % 3]);
				func.params[func.param_count].pointer = pointer;
				func.param_count++;
				pointer = 0;
			}

		}
	}
	for(j = i; file[j] != 0 && file[j] != ';' && file[j] != '{' && file[j] != '('; j++);
	if(file[j] != '(')
		fcd_function_add(rapport, &func);
	else
	{
		parenthesis = 1;
		for(j++; parenthesis != 0; j++)
		{
			if(file[j] == '(')
				parenthesis++;
			if(file[j] == ')')
				parenthesis--;
		}
	}
	return j;
}


void fcd_parse_file(char *file_name, FILE *rapport)
{
	char *file, word_a[3][1024];
	FILE *f;
	uint size, i, word_pos, word_count, comment, brackets, line_number, returns, text;
	f = fopen(file_name, "r");
	if(f == NULL)
		return 0;
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	rewind(f);
	file = malloc(size + 1);
	size = fread(file, 1, size, f);
	file[size] = 0;
	fclose(f);
	text = 0;
	line_number = 0;
	word_count = 0;
	word_pos = 0;
	comment = 0;
	brackets = 0;
	word_a[0][0] = 0;
	word_a[1][0] = 0;
	word_a[2][0] = 0;
	for(i = 0; i < size; i++)
	{
		if(fcd_character_test(file[i]))
		{
			if(word_pos == 0)
				word_count++;
			word_a[word_count % 3][word_pos++] = file[i];
		}else
		{
			if(word_pos != 0)
				word_a[word_count % 3][word_pos] = 0;
			if(file[i] == '(')
				i += fcd_parse_function_params(rapport, &file[i + 1], word_a[(word_count + 2) % 3], word_a[word_count % 3], file_name, line_number);
			if(file[i] == '#')
			{
				returns = 1;
				for(i++; i < size && returns != 0; i++)
				{
					if(file[i] == '\n')
						returns--;
					if(file[i] == '\\')
						returns++;
				}
				i--;
			}
			if(file[i] == '/' && file[i + 1] == '/')
				for(i += 2; i < size && file[i] != '\n'; i++);
			if(file[i] == '\n')
				line_number++;
			
			if(file[i] == '{')
				brackets++;
			if(file[i] == '"')
				text = 1;
			if(file[i] == '/' && file[i + 1] == '*')
				comment++;
			if(brackets + comment + text != 0)
			{
				uint start;
				start = i;
				for(i++; i < size && (brackets != 0 || comment != 0 || text != 0); i++)
				{
					if(file[i] == '\n')
						line_number++;
					if(text == 0)
					{
						if(file[i] == '/' && file[i + 1] == '/')
							for(i += 2; i < size && file[i] != '\n'; i++);
						if(file[i] == '/' && file[i + 1] == '*')
						{
							i++;
							comment++;
						}
						if(file[i] == '*' && file[i + 1] == '/')
						{
							i++;
							comment--;
						}
						if(comment == 0)
						{
							if(file[i] == '{')
								brackets++;
							if(file[i] == '}')
								brackets--;
						}
					}

					if(file[i] == '\\' && file[i + 1] == '\\')
						i++;
					else if(file[i] == '\\' && file[i + 1] == '"')
					{
						char buffer[256];
						uint j;
						for(j = 0; j < 255  && file[i + j] != 0; j++)
							buffer[j] = file[i + j];
						buffer[j] = 0;
						i++;
					}
					else if(file[i] == '"')
						text = (text + 1) % 2;
				}			
			}
			word_pos = 0;
		}
	}
	free(file);
}

int main()
{
	char file_name_buffer[1024], path[1024];
	IDir *dir;
	FILE *f;
	uint i;
	FCDFunctionStorage.functions = NULL;
	FCDFunctionStorage.function_count = 0;
	my_buggy_function();

	f = fopen("FCD_Error_rapport.txt", "w");
	dir = imagine_path_open("../../Mergesource/");
	while(imagine_path_next(dir, file_name_buffer, 1024))
	{	
		if((f_text_filter(file_name_buffer, ".c") ||
			f_text_filter(file_name_buffer, ".h")) && 
			!f_text_filter(file_name_buffer, "old") &&
			!f_text_filter(file_name_buffer, ".cpp"))
		{
			printf("Parsing %s\n", file_name_buffer);
			sprintf(path,"../../Mergesource/%s", file_name_buffer);
			fcd_parse_file(path, f);
		}
	}
//	fcd_parse_file("test.c", f);
	fprintf(f, "\n\nListing all %u fuunctions:\n\n", FCDFunctionStorage.function_count);
	for(i = 0; i < FCDFunctionStorage.function_count; i++)
		fcd_function_print(f, &FCDFunctionStorage.functions[i]);
	printf("%u functions found", FCDFunctionStorage.function_count);
	imagine_path_close(dir);
}