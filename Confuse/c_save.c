#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "forge.h"
#include "c_internal.h"
#include "testify.h"

uint c_texture_count = 0;
CTexture *c_textures = NULL;

uint (*c_load_image_2d_func)(char *name, uint8 *data, uint x, uint y, void *user) = NULL;
void *c_load_image_2d_user;
uint (*c_load_image_3d_func)(char *name, uint8 *data, uint x, uint y, uint z, void *user) = NULL;
void *c_load_image_3d_user;
uint (*c_load_image_cube_func)(char *name, uint8 *data, uint x, uint y, void *user) = NULL;
void *c_load_image_cube_user;

void confuce_asset_load_image_2d_func(uint (*func)(char *name, uint8 *data, uint x, uint y, void *user), void *user)
{
	c_load_image_2d_func = func;
	c_load_image_2d_user = user;
}

void confuce_asset_load_image_3d_func(uint (*func)(char *name, uint8 *data, uint x, uint y, uint z, void *user), void *user)
{
	c_load_image_3d_func = func;
	c_load_image_3d_user = user;
}

void confuce_asset_load_image_cube_func(uint (*func)(char *name, uint8 *data, uint x, uint y, void *user), void *user)
{
	c_load_image_cube_func = func;
	c_load_image_cube_user = user;
}



void confuse_session_save_textures_used(CEntity *e, uint *ids)
{
	uint i, j, *stack;
	stack = (uint *)e->stack;

	for(i = 0; i < e->stack_type_count; i++)
	{
		if(e->stack_types[i].type == C_TYPE_IMAGE_2D ||
			e->stack_types[i].type == C_TYPE_IMAGE_3D ||
			e->stack_types[i].type == C_TYPE_IMAGE_CUBE)
		{
			if(stack[e->stack_types[i].pos] != -1)
			{
				for(j = 0; ids[j] != stack[e->stack_types[i].pos] && ids[j] != -1; j++);
				if(ids[j] == -1)
					ids[j] = stack[e->stack_types[i].pos];
			}
		}
	}
}

uint *confuse_session_save_textures(CSession *session, THandle *handle)
{
	uint i, j, k, size, *ids;
	ids = malloc((sizeof *ids) * c_texture_count);
	for(i = 0; i < c_texture_count; i++)
		ids[i] = -1;	
	confuse_session_save_textures_used(&session->primitive_settings, ids);
	for(i = 0; i < session->entity_define_count; i++)
		confuse_session_save_textures_used(&session->entity_defines[i], ids);

	for(i = 0; i < c_texture_count && ids[i] != -1; i++);
	testify_pack_uint32(handle, i, "texture_count");

	for(i = 0; i < c_texture_count && ids[i] != -1; i++)
	{
		for(j = 0; j < c_texture_count && c_textures[j].texture_id != ids[i]; j++);
		if(j < c_texture_count)
		{
			testify_pack_uint8(handle, c_textures[j].type, "type");
			testify_pack_string(handle, c_textures[j].name, "name");
			testify_pack_uint32(handle, c_textures[j].size[0], "x");
			testify_pack_uint32(handle, c_textures[j].size[1], "y");
			testify_pack_uint32(handle, c_textures[j].size[2], "z");
			size = 4 * c_textures[j].size[0] * c_textures[j].size[1];
			if(c_textures[j].type == C_TYPE_IMAGE_3D)
				size *= c_textures[j].size[2];
			if(c_textures[j].type == C_TYPE_IMAGE_CUBE)
				size *= 6;		
			for(k = 0; k < size; k++)
				testify_pack_uint8(handle, c_textures[j].buffer[k], "pizel");
		}
	}
	return ids;
}


void confuse_session_load_textures(CSession *session, THandle *handle)
{
	uint i, j, size;
	c_texture_count = testify_unpack_uint32(handle, "texture_count");
	c_textures = realloc(c_textures, (sizeof *c_textures) * (c_texture_count + 16));
	for(i = 0; i < c_texture_count; i++)
	{

		c_textures[i].type = testify_unpack_uint8(handle, "type");
		testify_unpack_string(handle, c_textures[i].name, 32, "name");
		c_textures[i].size[0] = testify_unpack_uint32(handle, "x");
		c_textures[i].size[1] = testify_unpack_uint32(handle, "y");
		c_textures[i].size[2] = testify_unpack_uint32(handle, "z");
		size = 4 * c_textures[i].size[0] * c_textures[i].size[1];
		if(c_textures[i].type == C_TYPE_IMAGE_3D)
			size *= c_textures[i].size[2];
		if(c_textures[i].type == C_TYPE_IMAGE_CUBE)
			size *= 6;
		c_textures[i].buffer = malloc((sizeof *c_textures[i].buffer) * size);
		for(j = 0; j < size; j++)
			c_textures[i].buffer[j] = testify_unpack_uint8(handle,  "pizel");
		if(c_textures[i].type == C_TYPE_IMAGE_2D && c_load_image_2d_func != NULL)
			c_textures[i].texture_id = c_load_image_2d_func(c_textures[i].name, c_textures[i].buffer, c_textures[i].size[0], c_textures[i].size[1], c_load_image_2d_user);
		if(c_textures[i].type == C_TYPE_IMAGE_3D && c_load_image_3d_func != NULL)
			c_textures[i].texture_id = c_load_image_3d_func(c_textures[i].name, c_textures[i].buffer, c_textures[i].size[0], c_textures[i].size[1], c_textures[i].size[2], c_load_image_3d_user);
		if(c_textures[i].type == C_TYPE_IMAGE_CUBE && c_load_image_cube_func != NULL)
			c_textures[i].texture_id = c_load_image_cube_func(c_textures[i].name, c_textures[i].buffer, c_textures[i].size[0], c_textures[i].size[1], c_load_image_cube_user);
	}
}

void confuse_session_save_component_print(CComponentDef *component)
{
	uint i;
	printf("name %s\n", component->name);
	printf("type %u\n", component->type);
	printf("param_count %u\n", component->param_count);
	for(i = 0; i < component->param_count; i++)
		printf("param[%u] %u - %s\n", i, component->param_types[i], component->param_names[i]);
	printf("output %u\n", component->output);
	printf("primitive %u\n", component->primitive);
	printf("exe func %p\n", component->exe_func);
	printf("display func %p\n", component->display_func);
	printf("special %u\n", component->special);
}

void confuse_session_save_entity(CSession *session, CEntity *e, THandle *handle)
{
	uint i, j, k, size, *istack;
//	confuse_execute(session, e, 0.01, TRUE);
	testify_pack_string(handle, e->name, "name");

	testify_pack_uint32(handle, e->stack_size, "stack_size");
	testify_pack_uint32(handle, e->stack_alloc, "stack_alloc");
	testify_pack_uint32(handle, e->stack_type_count, "stack_type_count");
	testify_pack_uint32(handle, e->stack_type_alloc, "stack_type_alloc");
	for(i = 0; i < e->stack_type_count; i++)
	{
		testify_pack_uint8(handle, e->stack_types[i].type, "type");
		testify_pack_string(handle, e->stack_types[i].name, "name");
		testify_pack_uint32(handle, e->stack_types[i].pos, "pos");
		testify_pack_uint32(handle, e->stack_types[i].size, "size");
		testify_pack_uint32(handle, e->stack_types[i].dependency, "dependency");
		if(e->stack_types[i].type == C_TYPE_IMAGE_2D ||
			e->stack_types[i].type == C_TYPE_IMAGE_3D ||
			e->stack_types[i].type == C_TYPE_IMAGE_CUBE)
		{
			istack = (uint*)e->stack;
			for(j = 0; j < e->stack_types[i].size; j++)
			{
				for(k = 0; k < c_texture_count && c_textures[k].texture_id != istack[e->stack_types[i].pos + j];  k++);
				if(k < c_texture_count)
					testify_pack_string(handle, c_textures[k].name, "texture_name");
				else
					testify_pack_string(handle, "", "texture_name");
			}
		}else
		{
			size = e->stack_types[i].size * c_type_size_get(e->stack_types[i].type);
			for(k = 0; k < size; k++)
				testify_pack_real32(handle, e->stack[k + e->stack_types[i].pos], "stack");
		}
	}

	testify_pack_uint32(handle, e->component_count, "component_count");
	testify_pack_uint32(handle, e->component_selected, "component_selected");
	for(i = 0; i < e->component_count; i++)
	{
		testify_pack_string(handle, session->component_list[e->components[i].type].name, "name");
		k = session->component_list[e->components[i].type].param_count;

		if(session->component_list[e->components[i].type].special == C_SCT_ANIMATION)
		{
			CSpecialAnimation *anim;
			anim = e->components[i].special;
			testify_pack_uint32(handle, anim->key_count, "key_count");
			testify_pack_uint32(handle, anim->key_allocated, "key_allocated");

			for(j = 0; j < anim->key_count + 2; j++)
				testify_pack_uint32(handle, e->components[i].references[j], "references");


			for(j = 0; j < anim->key_count; j++)
			{
				testify_pack_real32(handle, anim->keys[j].key_time, "key_time");
				testify_pack_real32(handle, anim->keys[j].key_ease[0], "key_ease");
				testify_pack_real32(handle, anim->keys[j].key_ease[1], "key_ease");
			}
		}else
		{

			testify_pack_uint32(handle, session->component_list[e->components[i].type].param_count, "param_count");
			for(j = 0; j < session->component_list[e->components[i].type].param_count; j++)
			{
				testify_pack_string(handle, session->component_list[e->components[i].type].param_names[j], "name");
				testify_pack_uint8(handle, session->component_list[e->components[i].type].param_types[j], "type");
				testify_pack_uint32(handle, e->components[i].references[j], "references");
			}
			if(session->component_list[e->components[i].type].output != -1)
				testify_pack_uint32(handle, e->components[i].references[j], "output");
			else
				testify_pack_uint32(handle, -1, "output");
		}

	}
}


void confuse_session_load_entity(CSession *session, CEntity *e, THandle *handle)
{
	uint i, j, k, l, size, component, count, type, *istack, save;
	char name[32];
	testify_unpack_string(handle, e->name, 32, "name");

	e->stack_size = testify_unpack_uint32(handle, "stack_size");
	e->stack_alloc = testify_unpack_uint32(handle, "stack_alloc");
	e->stack_type_count = testify_unpack_uint32(handle, "stack_type_count");
	e->stack_type_alloc = testify_unpack_uint32(handle, "stack_type_alloc");
	e->stack_types = malloc((sizeof *e->stack_types) * e->stack_type_alloc);
	
	e->stack = malloc((sizeof *e->stack) * e->stack_alloc);
	for(i = 0; i < e->stack_type_count; i++)
	{
		e->stack_types[i].type = testify_unpack_uint8(handle, "type");
		testify_unpack_string(handle, e->stack_types[i].name, 32, "name");
		e->stack_types[i].pos = testify_unpack_uint32(handle, "pos");
		e->stack_types[i].size = testify_unpack_uint32(handle, "size");
		if(e->stack_types[i].size == 0)
			e->stack_types[i].size = 0;
		e->stack_types[i].dependency = testify_unpack_uint32(handle, "dependency");
		e->stack_types[i].number = i;
		if(e->stack_types[i].type == C_TYPE_IMAGE_2D ||
			e->stack_types[i].type == C_TYPE_IMAGE_3D ||
			e->stack_types[i].type == C_TYPE_IMAGE_CUBE)
		{
			char name[128];
			istack = (uint *)e->stack;
			for(j = 0; j < e->stack_types[i].size; j++)
			{
				testify_unpack_string(handle, name, 128, "texture_name");
				istack[e->stack_types[i].pos + j] = -1;
				for(k = 0; k < c_texture_count;  k++)
				{
					for(l = 0; name[l] != 0 && name[l] == c_textures[k].name[l]; l++);
					if(name[l] == c_textures[k].name[l])
					{
						istack[e->stack_types[i].pos + j] = c_textures[k].texture_id;
						break;
					}
				}
			}
			
		}else
		{
			size = e->stack_types[i].size * c_type_size_get(e->stack_types[i].type);
			for(k = 0; k < size; k++)
				e->stack[e->stack_types[i].pos + k] = testify_unpack_real32(handle, "stack");
		}
	}

	save = e->stack_type_count;
	for(j = 0; j < e->stack_type_count; j++)
	{
		if(e->stack_types[j].size == 0)
			k = 0;
	}

	e->component_count = testify_unpack_uint32(handle, "component_count");
	e->component_selected = testify_unpack_uint32(handle, "component_selected");

	e->components = malloc((sizeof *e->components) * (session->component_list_count + 16));
	for(i = 0; i < e->component_count; i++)
	{
		testify_unpack_string(handle, name, 32, "name");
		for(component = 0; component < session->component_list_count; component++)
		{
			for(k = 0; k < 31 && name[k] != 0 && name[k] == session->component_list[component].name[k]; k++);
			if(name[k] == session->component_list[component].name[k])
				break;
		}
		if(component < session->component_list_count)
		{
			e->components[i].type = component;
			e->components[i].special = NULL;

			if(session->component_list[e->components[i].type].special == C_SCT_ANIMATION)
			{
				CSpecialAnimation *anim;
				anim = e->components[i].special = malloc(sizeof * anim);
					
				anim->key_count = testify_unpack_uint32(handle, "key_count");
				anim->key_allocated = testify_unpack_uint32(handle, "key_allocated");
				anim->keys = malloc((sizeof *anim->keys) * anim->key_allocated);
				e->components[i].references = malloc((sizeof *e->components[i].references) * (anim->key_allocated + 2)); 

				for(j = 0; j < anim->key_count + 2; j++)
					e->components[i].references[j] = testify_unpack_uint32(handle, "references");

				for(j = 0; j < anim->key_count; j++)
				{
					anim->keys[j].key_time = testify_unpack_real32(handle, "key_time");
					anim->keys[j].key_ease[0] = testify_unpack_real32(handle, "key_ease");
					anim->keys[j].key_ease[1] = testify_unpack_real32(handle, "key_ease");
				}
			}else
			{

				count = testify_unpack_uint32(handle, "param_count");
				if(count > session->component_list[component].param_count)
				{
					e->components[i].references = malloc((sizeof *e->components[i].references) * (count + 1));
					for(j = 0; j < count; j++)
						e->components[i].references[j] = -1;
				}else 
				{
					e->components[i].references = malloc((sizeof *e->components[i].references) * (session->component_list[component].param_count + 1));
					for(j = 0; j < session->component_list[component].param_count; j++)
						e->components[i].references[j] = -1;
				}
				if(session->component_list[component].param_count != count)
					j = 0;
				for(j = 0; j < count; j++)
				{
					testify_unpack_string(handle, name, 32, "name");
					type = testify_unpack_uint8(handle, "type");

					for(k = 0; k < session->component_list[component].param_count; k++)
					{
						if(session->component_list[component].param_types[k] == type)
						{
							for(l = 0; l < 31 && name[l] != 0 && name[l] == session->component_list[component].param_names[k][l]; l++);
							if(name[l] == session->component_list[component].param_names[k][l])
								break;
						}
					}
					if(k < session->component_list[component].param_count)
						e->components[i].references[k] = testify_unpack_uint32(handle, "references");
					else
						testify_unpack_uint32(handle, "references");
				}
				e->components[i].references[j] = testify_unpack_uint32(handle, "output");
				for(j = 0; j < session->component_list[component].param_count; j++)
				{
					if(e->components[i].references[j] == -1)
						e->components[i].references[j] = c_entity_stack_add(e, session->component_list[component].param_types[j], session->component_list[component].param_names[j], 1);
				}
			}
		}else
		{
			count = testify_unpack_uint32(handle, "param_count");
			for(j = 0; j < count; j++)
			{
				testify_unpack_string(handle, name, 32, "name");
				testify_unpack_uint8(handle, "type");
				testify_unpack_uint32(handle, "references");
				/* void *special */
			}
			testify_unpack_uint32(handle, "output");
			i--;
			e->component_count--;
		}
	}

	for(j = 0; j < e->stack_type_count; j++)
	{
		if(e->stack_types[j].size == 0)
			k = 0;
	}
}


void confuse_session_save_primitives(CSession *session, THandle *handle)
{
	char *name;
	float *buffer;
	uint i, j, k;
	testify_pack_uint32(handle, session->primitive_count, "primitive_count");
	for(i = 0; i < session->primitive_count; i++)
	{
		testify_pack_string(handle, session->prim[i].name, "name");
		testify_pack_uint8(handle, session->prim[i].type, "type");
		testify_pack_string(handle, session->prim[i].shaders[0], "shaders");
		testify_pack_string(handle, session->prim[i].shader_paths[0], "shader_paths");
		testify_pack_string(handle, session->prim[i].shaders[1], "shaders");
		testify_pack_string(handle, session->prim[i].shader_paths[1], "shader_paths");

		if(session->prim[i].type == C_DPT_DRAW_CALL_UNIFORM)
		{
			buffer = session->prim[i].attrib_buffer;
			testify_pack_uint32(handle, session->prim[i].attrib_used, "buffer size");
			printf("used %u %u\n", session->prim[i].attrib_used, session->prim[i].attrib_stride);
			for(j = 0; j < session->prim[i].attrib_used * session->prim[i].attrib_stride; j++)
				testify_pack_real32(handle, buffer[j], "buffer");
		}

		if(session->prim[i].type == C_DPT_DRAW_CALL_ATTRIBUTE)
		{
			buffer = session->prim[i].uniform_buffer;		
			testify_pack_uint32(handle, session->prim[i].uniform_stride, "buffer size");
			for(j = 0; j < session->prim[i].uniform_stride; j++)
				testify_pack_real32(handle, buffer[j], "buffer");
		}
	}
}

uint *confuse_session_load_primitives(CSession *session, THandle *handle)
{
	CDrawState *p;
	float *buffer;
	char name[1024], *shader_names[2], *shader_source[2];
	uint i, j, k, primitive_count, type, *output, old_count;
	primitive_count = testify_unpack_uint32(handle, "primitive_count");
	output = malloc((sizeof *output) * primitive_count);
	old_count = session->primitive_count;
	for(i = 0; i < primitive_count; i++)
	{
		testify_unpack_string(handle, name, 1024, "name");
		type = testify_unpack_uint8(handle,  "type");

		shader_source[0] = testify_unpack_string_allocate(handle, "shaders");
		shader_names[0] = testify_unpack_string_allocate(handle, "shader_paths");
		shader_source[1] = testify_unpack_string_allocate(handle, "shaders");
		shader_names[1] = testify_unpack_string_allocate(handle, "shader_paths");
		for(j = 0; j < old_count; j++)
		{
			if(session->prim[j].type == type)
			{
				for(k = 0; session->prim[j].shaders[0][k] == shader_source[0][k] && shader_source[0][k] != 0; k++);
				if(session->prim[j].shaders[0][k] == shader_source[0][k])
				{
					for(k = 0; session->prim[j].shaders[1][k] == shader_source[1][k] && shader_source[1][k] != 0; k++);
					if(session->prim[j].shaders[1][k] == shader_source[1][k])
					{
						break;
					}
				}
			}
		}
		if(j < old_count)
		{
			output[i] = j;
			buffer = session->prim[j].uniform_buffer;
			j = testify_unpack_uint32(handle, "buffer size");
			for(k = 0; k < j; k++)
				testify_unpack_real32(handle, "buffer");
			printf("Error\n");
			exit(0);
		}
		else
		{
			output[i] = primitive_count + i;
			p = c_primitive_add(session, name);
			p->shader_paths[0] = shader_names[0];
			p->shader_paths[1] = shader_names[1];
			p->shaders[0] = shader_source[0];
			p->shaders[1] = shader_source[1];
			c_primitive_compile(p);
			if(p->attrib_param_count == 0)
				k = 0;
			if(type == C_DPT_DRAW_CALL_UNIFORM)
			{
				c_primitive_inject_uniform(session, p, C_CC_DRAW, FALSE);
			}else
			{
				c_primitive_inject_uniform(session, p, C_CC_PRIMITIVE_STATE, TRUE);
				c_primitive_inject_attribute(session, p);
			}
			j = testify_unpack_uint32(handle, "buffer size");
			
			printf("LOADING %s\n", name);
			printf("Buffer size %u - %u - %u %u\n", j, p->type, C_DPT_DRAW_CALL_UNIFORM, C_DPT_DRAW_CALL_ATTRIBUTE);
			printf("Buffer %p %p\n", p->uniform_buffer, p->attrib_buffer);
			
			if(p->type == C_DPT_DRAW_CALL_UNIFORM)
			{
				buffer = malloc(sizeof(float) * j * p->attrib_stride);
				p->attrib_used = j;
				for(k = 0; k < j * p->attrib_stride; k++)
					buffer[k] = testify_unpack_real32(handle, "buffer");
				c_update_primitive_array(p, (uint8 *)buffer, j);
			}
			
			buffer = p->uniform_buffer;
			if(p->type == C_DPT_DRAW_CALL_ATTRIBUTE)
				for(k = 0; k < j; k++)
					buffer[k] = testify_unpack_real32(handle, "buffer");
		}

	}


	return output;
}


boolean confuse_session_save(CSession *session, THandle *handle)
{
	uint i;
	
	if(handle == NULL)
		return FALSE;
//	testify_debug_mode_set(handle, TRUE, "save_everything.txt");
	confuse_session_save_textures(session, handle);
	confuse_session_save_primitives(session, handle);
	confuse_session_save_entity(session, &session->primitive_settings, handle);
	testify_pack_uint32(handle, session->entity_define_count, "entity_define_count");
	for(i = 0; i < session->entity_define_count; i++)
		confuse_session_save_entity(session, &session->entity_defines[i], handle);
	return TRUE;
}

boolean confuse_session_load(CSession *session, THandle *handle)
{
	uint *primitives, i, count;
	if(handle == NULL)
		return FALSE;
//	testify_debug_mode_set(handle, TRUE, NULL);
	confuse_session_load_textures(session, handle);
	primitives = confuse_session_load_primitives(session, handle);
	confuse_session_load_entity(session, &session->primitive_settings, handle);
//	for(i = 0; i < session->component_list_count; i++)
//		confuse_session_save_component_print(&session->component_list[i]);

	session->entity_define_count = testify_unpack_uint32(handle, "entity_define_count");
	count = (session->entity_define_count + 15 + 16) / 16;
	session->entity_defines = malloc((sizeof *session->entity_defines) * count * 16);

	for(i = 0; i < session->entity_define_count; i++)
		confuse_session_load_entity(session, &session->entity_defines[i], handle);
	free(primitives);

	return TRUE;
}