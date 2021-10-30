#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "betray.h"
#include "seduce.h"
#include "vetk.h"

char *confuse_object_shader_vertex = 
"attribute vec3 vertex;\n"
"varying vec3 n;\n"
"uniform mat4 ModelViewMatrix;\n"
"uniform mat4 ModelViewProjectionMatrix;\n"
"uniform mat4 NormalMatrix;\n"
"void main()\n"
"{\n"
"	n = vertex.xyz + vec3(0.5);"
"	gl_Position = ModelViewProjectionMatrix * vec4(vertex.xyz, 1.0);\n"
"}\n";

char *confuse_object_shader_fragment = 
"varying vec3 n;"
"void main()"
"{"
//"	gl_FragColor = vec4(log(vec3(dot(n, vec3(3.0))) * vec3(0.2, 0.6, 1.0)) + vec3(0.1) + vec3(0.1) * n.yyy, 1.0);"
//"	gl_FragColor = vec4(vec3(dot(n, vec3(1.0))) * vec3(0.04, 0.12, 0.2) + vec3(0.3) + vec3(0.3) * n.yyy, 1.0);"
"	gl_FragColor = vec4(n, 1.0);"
"}";

RShader *c_editor_verse_preview_shader_get()
{
	static RShader *shader = NULL;
	char buffer[2048];
	if(shader == NULL)
		shader = r_shader_create_simple(buffer, 2048, confuse_object_shader_vertex, confuse_object_shader_fragment, "verse_preview");
	return shader;
}

/*
ERROR: 0:6: 'gl_FragColor' : undeclared identifier 
ERROR: 0:6: 'assign' :  cannot convert from '4-component vector of float' to 'float'
ERROR: 0:7: ';' : syntax error syntax error

ERROR: 0:1: 'attribute' :  supported in vertex shaders only  
ERROR: 0:2: 'attribute' :  supported in vertex shaders only  
ERROR: 0:8: 'assign' :  l-value required "mapping" (can't modify a varying)
ERROR: 0:9: 'gl_Position' : syntax error syntax error
*/

void c_editor_verse_vertex_fill(float *array, uint array_length, uint node_id)
{
	static uint seed = 0;
	char *vertex_params[] = {VETK_VP_VERTEX_X, VETK_VP_VERTEX_Y, VETK_VP_VERTEX_Z};
	VETKQuadTri *mesh;
	SUIFormats vertex_format_types[1] = {SUI_FLOAT};
	uint vertex_format_size[1] = {3};
	uint i, j, count;
	float min[3], max[3], pos[3], dir[3] = {1000, 0, 0}, tuv[3];
	mesh = vetk_create_mesh(e_ns_get_node(0, node_id), vertex_params, 3, FALSE, FALSE);
	if(mesh == NULL)
		return;
	for(i = 0; i < 3; i++)
		max[i] = min[i] = mesh->vertex[i];
	for(i = 0; i < mesh->vertex_length * 3; i += 3)
	{
		for(j = 0; j < 3; j++)
		{
			if(mesh->vertex[i + j] > max[j])
				max[j] = mesh->vertex[i + j];
			if(mesh->vertex[i + j] < min[j])
				min[j] = mesh->vertex[i + j];
		}
	}

	for(i = 0; i < array_length; i++)
	{
		count = 0;
		while(count % 2 != 1)
		{
			count = 0;
			for(j = 0; j < 3; j++)
				pos[j] = min[j] + (max[j] - min[j]) * f_randf(seed++); 
			for(j = 0; j < mesh->vertex_length * 3; j += 9)
				if(f_raycast_trif(pos, dir, &mesh->vertex[j], &mesh->vertex[j + 3], &mesh->vertex[j + 6], &tuv[0], &tuv[1], &tuv[2]) && tuv[0] > 0)
					count++;

		}
		array[i * 3 + 0] = pos[0];
		array[i * 3 + 1] = pos[1];
		array[i * 3 + 2] = pos[2];
	}
}

void c_editor_verse_preview_convert(CPreviewGeometry *data, ENode *node)
{
	char *vertex_params[] = {VETK_VP_VERTEX_X, VETK_VP_VERTEX_Y, VETK_VP_VERTEX_Z};
	VETKQuadTri *mesh;
	SUIFormats vertex_format_types[1] = {SUI_FLOAT};
	uint vertex_format_size[1] = {3};
	uint i, j, line_count = 0;
	char *name;
	void *array;
	float min[3], max[3], n1[3], n2[3], size = 0, *line_array;

	data->version = e_ns_get_node_version_data(node);
	name = e_ns_get_node_name(node);
	for(i = 0; name[i] != 0 && i < 32; i++)
		data->name[i] = name[i];
	data->name[i] = 0;
	data->node_id = e_ns_get_node_id(node);

	mesh = vetk_create_mesh(node, vertex_params, 3, FALSE, FALSE);

	if(mesh == NULL)
	{
		data->render_array = NULL;
		return;
	}
	if(mesh->vertex_length == 0)
	{
		data->render_array = NULL;
		return;
	}
	for(i = 0; i < 3; i++)
		max[i] = min[i] = mesh->vertex[i];
	for(i = 0; i < mesh->vertex_length * 3; i += 3)
	{
		for(j = 0; j < 3; j++)
		{
			if(mesh->vertex[i + j] > max[j])
				max[j] = mesh->vertex[i + j];
			if(mesh->vertex[i + j] < min[j])
				min[j] = mesh->vertex[i + j];
		}
	}
	for(i = 0; i < 3; i++)
	{
		if(max[i] - min[i] > size)
			size = max[i] - min[i];
		max[i] = (max[i] + min[i]) * 0.5;
	}
	for(i = 0; i < mesh->vertex_length * 3; i += 3)
		for(j = 0; j < 3; j++)
			mesh->vertex[i + j] = (mesh->vertex[i + j] - max[j]) / size;

	line_array = malloc((sizeof *line_array) * 6 * mesh->vertex_length);
	for(i = 0; i < mesh->vertex_length; i++)
	{
		if(mesh->neighbor[i] == -1)
		{
			line_array[line_count++] = mesh->vertex[i * 3 + 0];
			line_array[line_count++] = mesh->vertex[i * 3 + 1];
			line_array[line_count++] = mesh->vertex[i * 3 + 2];
			j = (i + 1) % 3 + (i / 3) * 3;
			line_array[line_count++] = mesh->vertex[j * 3 + 0];
			line_array[line_count++] = mesh->vertex[j * 3 + 1];
			line_array[line_count++] = mesh->vertex[j * 3 + 2];
		}else if(mesh->neighbor[i] < i)
		{
			j = (i / 3) * 9;
			min[0] = mesh->vertex[j + 3] - mesh->vertex[j + 0];
			min[1] = mesh->vertex[j + 4] - mesh->vertex[j + 1];
			min[2] = mesh->vertex[j + 5] - mesh->vertex[j + 2];
			max[0] = mesh->vertex[j + 6] - mesh->vertex[j + 0];
			max[1] = mesh->vertex[j + 7] - mesh->vertex[j + 1];
			max[2] = mesh->vertex[j + 8] - mesh->vertex[j + 2];
			f_cross3f(n1, min, max);
			f_normalize3f(n1);
			j = (mesh->neighbor[i] / 3) * 9;
			min[0] = mesh->vertex[j + 3] - mesh->vertex[j + 0];
			min[1] = mesh->vertex[j + 4] - mesh->vertex[j + 1];
			min[2] = mesh->vertex[j + 5] - mesh->vertex[j + 2];
			max[0] = mesh->vertex[j + 6] - mesh->vertex[j + 0];
			max[1] = mesh->vertex[j + 7] - mesh->vertex[j + 1];
			max[2] = mesh->vertex[j + 8] - mesh->vertex[j + 2];
			f_cross3f(n2, min, max);
			f_normalize3f(n2);
			if(0.7 > n1[0] * n2[0] + n1[1] * n2[1] + n1[2] * n2[2])
			{
				line_array[line_count++] = mesh->vertex[i * 3 + 0];
				line_array[line_count++] = mesh->vertex[i * 3 + 1];
				line_array[line_count++] = mesh->vertex[i * 3 + 2];
				j = (i + 1) % 3 + (i / 3) * 3;
				line_array[line_count++] = mesh->vertex[j * 3 + 0];
				line_array[line_count++] = mesh->vertex[j * 3 + 1];
				line_array[line_count++] = mesh->vertex[j * 3 + 2];
			}
		}
	}
	if(line_count != NULL)
	{
		data->render_array = r_array_allocate(line_count / 3, vertex_format_types, vertex_format_size, 1, 0);
		r_array_load_vertex(data->render_array, NULL, line_array, 0, line_count / 3);
	}else
		data->render_array = NULL;
	if(line_array != NULL)
		free(line_array);
	vetk_destroy_mesh(mesh);
}

void c_editor_verse_preview_update()
{
	static uint32 node_id = 0;
	CPreviewGeometry *data;
	ENode *node;
	node = e_ns_get_node_next(node_id, 0, V_NT_GEOMETRY);
	if(node != NULL)
	{
		node_id = e_ns_get_node_id(node) + 1;
		data = e_ns_get_custom_data(node, 0);
		if(data == NULL)
		{
			data = malloc(sizeof *data);
			e_ns_set_custom_data(node, 0, data);
		}else
		{
			if(data->version == e_ns_get_node_version_data(node))
				return;
			if(data->render_array != NULL)
				r_array_free(data->render_array);
		}
		c_editor_verse_preview_convert(data, node);
	}else
		node_id = 0;
}


CPreviewGeometry *c_editor_verse_preview_next(CPreviewGeometry *data)
{
	ENode *node;
	if(data == NULL)
		node = e_ns_get_node_next(0, 0, V_NT_GEOMETRY);
	else
		node = e_ns_get_node_next(data->node_id + 1, 0, V_NT_GEOMETRY);
	if(node == NULL)
		return NULL;
	return e_ns_get_custom_data(node, 0);
}

void c_update_primitive_array(CDrawState *state, void *data, uint vertex_count);


extern uint		confuse_component_param_count_init_get(uint component);
extern uint		confuse_component_param_count_extend_get(CComponent *component);
extern CTypeType confuse_component_param_type_get(uint component, uint id);
extern char		*confuse_component_param_name_get(uint component, uint id);

uint **c_editor_verse_convert_find_objects(CSession *session, CEntity *entity, uint component_id, uint **pointer_list, uint *list_used, uint *allocated_used, boolean colapsed)
{
	uint *ibuf, ref, i, j;
	for(i = 0; i < confuse_component_param_count_extend_get(session, &entity->components[component_id]); i++)
	{
		if(C_TYPE_OBJECT == confuse_component_param_type_get(session, entity->components[component_id].type, i))
		{
			ref = entity->components[component_id].references[i];

			if(entity->stack_types[ref].dependency != -1)
				pointer_list = c_editor_verse_convert_find_objects(session, entity, component_id, pointer_list, list_used, allocated_used, colapsed);

			ref = entity->stack_types[ref].pos;
			ibuf = &entity->stack[ref];
			for(j = 0; j < *list_used && *pointer_list[j] != *ibuf; j++);
			if(!colapsed || j == *list_used)
			{
				if(*list_used == *allocated_used)
				{
					if(*allocated_used == 0)
						*allocated_used = 8;
					*allocated_used *= 2;
					pointer_list = realloc(pointer_list, (sizeof *pointer_list) * *allocated_used);
				}
				pointer_list[(*list_used)++] = ibuf; 
			}
		}
	}
	return pointer_list;
}


void c_editor_verse_convert(CSession *session, uint component)
{
	char **vertex_params, *name_buffer;
	uint *node_list, node_list_length, node_list_used, vertex_param_count, vertex_array_length;
	float *vertex_array;
	CDrawState *state;
	VETKQuadTri **meshes;
	uint i, j, k;

	printf("component %u\n", component);

	state = &session->prim[confuse_component_primitive_get(session, component)];
	node_list_length = 128;
	node_list_used = 0;
	node_list = malloc((sizeof *node_list) * node_list_length * 3);

	for(i = 0; i < session->entity_define_count; i++)
	{
		for(j = 0; j < session->entity_defines[i].component_count; j++)
		{
			if(session->entity_defines[i].components[j].type == component)
			{
				uint *ibuf, ref;
				ref = session->entity_defines[i].components[j].references[C_BUL_OBJECT];
				ref = session->entity_defines[i].stack_types[ref].pos;
				ibuf = &session->entity_defines[i].stack[ref];
				printf("-Found need form Node %u to be included", *ibuf);
				for(k = 0; k < node_list_used && node_list[k * 3] != *ibuf; k++);
				if(k == node_list_used)
				{
					if(node_list_used == node_list_length)
					{
						node_list_length *= 2;
						node_list = realloc(node_list, (sizeof *node_list) * node_list_length * 3);
					}
					node_list[node_list_used++ * 3] = *ibuf;
					printf("ADDED\n");
				}else
					printf("NOT ADDED\n");
			}
		}
	}

	meshes = malloc((sizeof *meshes) * node_list_used);

	vertex_param_count = 0;
	vertex_params = malloc((sizeof *vertex_params) * state->attrib_param_count * 4);
	name_buffer = malloc((sizeof *name_buffer) * state->attrib_param_count * 4 * 32);
	for(i = 0; i < state->attrib_param_count * 4; i++)
		vertex_params[i] = &name_buffer[i * 32];

	for(i = j = 0; i < state->attrib_param_count; i++)
	{
		switch(state->attrib_params[i].type)
		{
			case C_TYPE_FLOAT :
				sprintf(vertex_params[vertex_param_count++], state->attrib_params[i].name);
			break;
			case C_TYPE_UV :
				sprintf(vertex_params[vertex_param_count++], "%s_u", state->attrib_params[i].name);
				sprintf(vertex_params[vertex_param_count++], "%s_v", state->attrib_params[i].name);
			break;
			case C_TYPE_POS :
				sprintf(vertex_params[vertex_param_count++], "x.%s", state->attrib_params[i].name);
				sprintf(vertex_params[vertex_param_count++], "y.%s", state->attrib_params[i].name);
				sprintf(vertex_params[vertex_param_count++], "z.%s", state->attrib_params[i].name);
			break;
			case C_TYPE_COLOR :
				sprintf(vertex_params[vertex_param_count++], "%s_r", state->attrib_params[i].name);
				sprintf(vertex_params[vertex_param_count++], "%s_g", state->attrib_params[i].name);
				sprintf(vertex_params[vertex_param_count++], "%s_b", state->attrib_params[i].name);
				sprintf(vertex_params[vertex_param_count++], "%s_a", state->attrib_params[i].name);
			break;
		}
	}

	for(i = vertex_array_length = 0; i < node_list_used; i++)
	{
		ENode *node;
		char *name;
		node = e_ns_get_node(0, node_list[i * 3]);

		if(node != NULL)
		{
			for(j = 0; j < vertex_param_count; j++)
				printf("param[%u] -%s- %s\n", i, vertex_params[j],  e_nsg_get_layer_by_name(node, vertex_params[j]) == NULL ? "NOT FOUND" : "FOUND");

			name = e_ns_get_node_name(node);
			meshes[i] = vetk_create_mesh(node, vertex_params, vertex_param_count, FALSE, FALSE);
		}
		else
			meshes[i] = NULL;
		if(meshes[i] != NULL)
			vertex_array_length += meshes[i]->vertex_length;
	}

	free(vertex_params);
	free(name_buffer);

	vertex_array = malloc((sizeof *vertex_array) * vertex_array_length * vertex_param_count);
	for(i = k = 0; i < node_list_used; i++)
	{
		node_list[i * 3 + 1] = k / vertex_param_count;
		if(meshes[i] != NULL)
		{
			for(j = 0; j < meshes[i]->vertex_length * vertex_param_count; j++)
				vertex_array[k++] = meshes[i]->vertex[j];
			printf("vertex[%u]length= %u\n", i, meshes[i]->vertex_length);
		}
		node_list[i * 3 + 2] = k / vertex_param_count;
	}
	
	for(i = 0; i < session->entity_define_count; i++)
	{
		for(j = 0; j < session->entity_defines[i].component_count; j++)
		{
			if(session->entity_defines[i].components[j].type == component)
			{
				uint *ibuf, ref;
				ref = session->entity_defines[i].components[j].references[C_BUL_OBJECT];
				ref = session->entity_defines[i].stack_types[ref].pos;
				ibuf = &session->entity_defines[i].stack[ref];
				for(k = 0; node_list[k * 3] != *ibuf && k < node_list_used; k++);
				if(k == node_list_used)
				{
					uint *a = NULL;
					a[0] = 0;
				}
				ibuf[1] = node_list[k * 3 + 1];
				ibuf[2] = node_list[k * 3 + 2];
			}
		}
	}
	for(i = 0; i < node_list_used; i++)
		if(meshes[i] != NULL)
			vetk_destroy_mesh(meshes[i]);
	free(meshes);
	free(node_list);
	printf("node_list_used = %u\n", node_list_used);
	printf("vertex_array_length = %u\n", vertex_array_length);
	c_update_primitive_array(state, vertex_array, vertex_array_length);
}



void c_editor_verse_convert_test(CSession *session)
{
	uint **pointer_list = NULL, list_used = 0, allocated_used = 0;
	uint i, j, k, version;
	CDrawState *state;
	ENode *node;

	for(i = 0; i < confuse_component_count_get(session); i++)
	{
		if(C_CC_DRAW == confuse_component_category_get(session, i))
		{
			state = &session->prim[confuse_component_primitive_get(session, i)];
			if(state->type == C_DPT_DRAW_CALL_UNIFORM)
			{
				list_used = version = 0;
				for(j = 0; j < session->entity_define_count; j++)
					for(k = 0; k < session->entity_defines[j].component_count; k++)
						if(session->entity_defines[j].components[k].type == i)
							pointer_list = c_editor_verse_convert_find_objects(session, &session->entity_defines[j], k, pointer_list, &list_used, &allocated_used, TRUE);
	
				for(j = 0; j < list_used; j++)
				{
					node = e_ns_get_node(0, *pointer_list[j]);
					if(node != NULL)
						version += e_ns_get_node_version_data(node);
				}
				if(state->geometry_checksum != version)
				{
					c_editor_verse_convert(session, i);
					state->geometry_checksum = version;
				}
			}
		}
	}
	if(pointer_list != NULL)
		free(pointer_list);
}