#include "e_includes.h"

#include "verse.h"
#include "forge.h"
#include "e_storage_node.h"

typedef struct{
	VNTag		tag;
	VNTagType	type;
	char		tag_name[16];
}ETag;

typedef struct{
	ETag		*tags;
	uint		tag_count;
	char		group_name[16];
}ETagGroup;

extern void e_ns_update_node_version_data(ENodeHead *node);

void e_destroy_node_head(ENodeHead *node)
{
	uint i;
	if(node->tag_groups != NULL)
	{
		for(i = 0; i < node->group_count; i++)
			free(((ETagGroup *)node->tag_groups)[i].tags);
		free(node->tag_groups);
	}
}

extern ENodeHead *e_ns_get_node_networking(uint node_id);


void callback_send_tag_group_create(void *user, VNodeID node_id, uint16 group_id, const char *name)
{
	ENodeHead *node;
	uint i;
	if((node = (ENodeHead *)e_ns_get_node_networking(node_id)) == NULL)
		return;
	if(group_id >= node->group_count)
	{
		node->tag_groups = realloc(node->tag_groups, sizeof(ETagGroup) * (group_id + 16));
		for(i = node->group_count; i < group_id + 16; i++)
		{
			((ETagGroup *)node->tag_groups)[i].group_name[0] = 0;
 			((ETagGroup *)node->tag_groups)[i].tags = NULL;
			((ETagGroup *)node->tag_groups)[i].tag_count = 0;
		}
		node->group_count = group_id + 16;
	}
	for(i = 0; name[i] != 0 && i < 15; i++)
		((ETagGroup *)node->tag_groups)[group_id].group_name[i] = name[i];
	((ETagGroup *)node->tag_groups)[group_id].group_name[i] = 0;
	verse_send_tag_group_subscribe(node_id, group_id);
	e_ns_update_node_version_data(node);
}

void callback_send_tag_group_destroy(void *user, VNodeID node_id, uint16 group_id)
{
	ENodeHead *node;
	if((node = e_ns_get_node_networking(node_id)) == NULL)
		return;
	if(node->group_count < group_id || ((ETagGroup *)node->tag_groups)[group_id].group_name[0] == 0)
		return;
	if(((ETagGroup *)node->tag_groups)[group_id].tag_count != 0)
		free(((ETagGroup *)node->tag_groups)[group_id].tags);
	((ETagGroup *)node->tag_groups)[group_id].group_name[0] = 0;
	((ETagGroup *)node->tag_groups)[group_id].tags = NULL;
	((ETagGroup *)node->tag_groups)[group_id].tag_count = 0;
	e_ns_update_node_version_data(node);
}


void callback_send_tag_create(void *user, VNodeID node_id, uint16 group_id, uint16 tag_id, const char *name, VNTagType type, VNTag *tag)
{
	ENodeHead *node;
	ETag *t = NULL;
	uint i;
	if((node = (ENodeHead *)e_ns_get_node_networking(node_id)) == NULL)
		return;
	if(group_id >= node->group_count || ((ETagGroup *)node->tag_groups)[group_id].group_name[0] == 0)
		return;

	if(((ETagGroup *)node->tag_groups)[group_id].tag_count <= tag_id)
	{
		((ETagGroup *)node->tag_groups)[group_id].tags = realloc(((ETagGroup *)node->tag_groups)[group_id].tags, (sizeof *((ETagGroup *)node->tag_groups)[group_id].tags) * (tag_id + 8));
		i = 0;
		for(((ETagGroup *)node->tag_groups)[group_id].tag_count = 0; i < tag_id + 8; i++)
			((ETagGroup *)node->tag_groups)[group_id].tags[i].tag_name[0] = 0;
		((ETagGroup *)node->tag_groups)[group_id].tag_count = tag_id + 8;
	}	

	t = &((ETagGroup *)node->tag_groups)[group_id].tags[tag_id];
	t->type = type;
	
	for(i = 0; name[i] != 0 && i < 15; i++)
		t->tag_name[i] = name[i];
	t->tag_name[i] = 0;
	switch(type)
	{
		case VN_TAG_BOOLEAN :
			t->tag.vboolean = tag->vboolean;
		break;
		case VN_TAG_UINT32 :
			t->tag.vuint32 = tag->vuint32;
		break;
		case VN_TAG_REAL64 :
			t->tag.vreal64 = tag->vreal64;
		break;
		case VN_TAG_STRING :
			for(i = 0; tag->vstring[i] != 0; i++);
			t->tag.vstring = malloc((sizeof *tag->vstring) * (i + 1));
			for(i = 0; tag->vstring[i] != 0; i++)
				t->tag.vstring[i] = tag->vstring[i];
			t->tag.vstring[i] = 0;
		break;
		case VN_TAG_REAL64_VEC3 :
			t->tag.vreal64_vec3[0] = tag->vreal64_vec3[0];
			t->tag.vreal64_vec3[1] = tag->vreal64_vec3[1];
			t->tag.vreal64_vec3[2] = tag->vreal64_vec3[2];
		break;
		case VN_TAG_LINK :
			t->tag.vlink = tag->vlink;
		break;
		case VN_TAG_ANIMATION :
			t->tag.vanimation.curve = tag->vanimation.curve;
			t->tag.vanimation.start = tag->vanimation.start;
			t->tag.vanimation.end = tag->vanimation.end;
		break;
		case VN_TAG_BLOB :
			t->tag.vblob.blob = malloc(tag->vblob.size);
			t->tag.vblob.size = tag->vblob.size;
			for(i = 0; i < t->tag.vblob.size; i++)
				((char *)t->tag.vblob.blob)[i] = ((char *)tag->vblob.blob)[i];
		break;
	}
	e_ns_update_node_version_data(node);
}

/*
typedef union {
	boolean vboolean;
	uint32	vuint32;
	real64	vreal64;
	char	*vstring;
	real64	vreal64_vec3[3];
	VNodeID	vlink;
	struct {
		VNodeID curve;
		uint32 start;
		uint32 end;
	} vanimation;
	struct {
		uint16	size;
		void	*blob;
	} vblob;
} VNTag;
*/

void callback_send_tag_destroy(void *user, VNodeID node_id, uint16 group_id, uint16 tag_id)
{
	ENodeHead *node;
	if((node = e_ns_get_node_networking(node_id)) == NULL)
		return;
	if(node->group_count >= group_id && ((ETagGroup *)node->tag_groups)[group_id].group_name[0] != 0 && ((ETagGroup *)node->tag_groups)[group_id].tag_count > tag_id)
		((ETagGroup *)node->tag_groups)[group_id].tags[tag_id].tag_name[0] = 0;
	e_ns_update_node_version_data(node);
}

char *e_ns_get_tag_group(const ENodeHead *node, uint16 group_id)
{
	if(node->group_count > group_id)
		if(((ETagGroup *)node->tag_groups)[group_id].group_name[0] != 0)
			return ((ETagGroup *)node->tag_groups)[group_id].group_name;
	return NULL;
}

uint16 e_ns_get_next_tag_group(const ENodeHead *node, uint16 group_id)
{
	for(;node->group_count > group_id && ((ETagGroup *)node->tag_groups)[group_id].group_name[0] == 0; group_id++);
	if(node->group_count == group_id)
		return (uint16) ~0u;
	return group_id;
}

uint16 e_ns_get_next_tag(const ENodeHead *node, uint16 group_id, uint16 tag_id)
{	
	if(node->group_count <= group_id || ((ETagGroup *)node->tag_groups)[group_id].group_name[0] == 0)
		return -1;
	for(;((ETagGroup *)node->tag_groups)[group_id].tag_count > tag_id && ((ETagGroup *)node->tag_groups)[group_id].tags[tag_id].tag_name[0] == 0; tag_id++);
	if(((ETagGroup *)node->tag_groups)[group_id].tag_count == tag_id)
		return -1;
	return tag_id;
}

char *e_ns_get_tag_name(const ENodeHead *node, uint16 group_id, uint16 tag_id)
{
	if(node->group_count >= group_id && ((ETagGroup *)node->tag_groups)[group_id].group_name[0] != 0 && ((ETagGroup *)node->tag_groups)[group_id].tag_count > tag_id)
		return ((ETagGroup *)node->tag_groups)[group_id].tags[tag_id].tag_name;
	return NULL;
}

VNTagType e_ns_get_tag_type(const ENodeHead *node, uint16 group_id, uint16 tag_id)
{
	if(node->group_count >= group_id && ((ETagGroup *)node->tag_groups)[group_id].group_name[0] != 0 && ((ETagGroup *)node->tag_groups)[group_id].tag_count > tag_id)
		return ((ETagGroup *)node->tag_groups)[group_id].tags[tag_id].type;
	return 0;
}

VNTag *e_ns_get_tag(const ENodeHead *node, uint16 group_id, uint16 tag_id)
{
	if(node->group_count >= group_id && ((ETagGroup *)node->tag_groups)[group_id].group_name[0] != 0 && ((ETagGroup *)node->tag_groups)[group_id].tag_count > tag_id)
		return &((ETagGroup *)node->tag_groups)[group_id].tags[tag_id].tag;
	return NULL;
}

void e_ns_get_tag_by_name(const ENodeHead *node, char *name, uint16 *group_id, uint16 *tag_id)
{
	uint i;
	uint16 g, t;
	for(g = 0; g < node->group_count; g++)
	{
		if(((ETagGroup *)node->tag_groups)[g].group_name[0] != 0)
		{
			for(t = 0; t < ((ETagGroup *)node->tag_groups)[g].tag_count; t++)
			{
				for(i = 0; ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] != 0 && ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == name[i]; i++);
				if(((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == name[i])
				{
					if(group_id != NULL)
						*group_id = g;
					if(tag_id != NULL)
						*tag_id = t;
					return;
				}
			}
		}
	}
	if(group_id != NULL)
		*group_id = -1;
	if(tag_id != NULL)
		*tag_id = -1;
}

VNTag *e_ns_get_tag_by_type(const ENodeHead *node, char *name, VNTagType type, uint16 *group_id, uint16 *tag_id)
{
	uint i;
	uint16 g, t;
	for(g = 0; g < node->group_count; g++)
	{
		if(((ETagGroup *)node->tag_groups)[g].group_name[0] != 0)
		{
			for(t = 0; t < ((ETagGroup *)node->tag_groups)[g].tag_count; t++)
			{
				if(((ETagGroup *)node->tag_groups)[g].tags[t].type == type)
				{
					for(i = 0; ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] != 0 && ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == name[i]; i++);
					if(((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == name[i])
					{
						if(group_id != NULL)
							*group_id = g;
						if(tag_id != NULL)
							*tag_id = t;
						return &((ETagGroup *)node->tag_groups)[g].tags[t].tag;
					}
				}
			}
		}
	}
	if(group_id != NULL)
		*group_id = -1;
	if(tag_id != NULL)
		*tag_id = -1;
	return NULL;
}


uint16 e_ns_get_group_by_name(const ENodeHead *node, char *group_name)
{
	uint i;
	uint16 g;
	for(g = 0; g < node->group_count; g++)
	{
		for(i = 0; ((ETagGroup *)node->tag_groups)[g].group_name[i] != 0 && ((ETagGroup *)node->tag_groups)[g].group_name[i] == group_name[i]; i++);
		if(((ETagGroup *)node->tag_groups)[g].group_name[i] == group_name[i])
			return g;
	}
	return -1;
}

VNTag *e_ns_get_tag_by_name_and_group(const ENodeHead *node, char *group_name, char *tag_name, uint16 *group_id, uint16 *tag_id, VNTagType *tag_type)
{
	uint i;
	uint16 g, t;
	for(g = 0; g < node->group_count; g++)
	{
		for(i = 0; ((ETagGroup *)node->tag_groups)[g].group_name[i] != 0 && ((ETagGroup *)node->tag_groups)[g].group_name[i] == group_name[i]; i++);
		if(((ETagGroup *)node->tag_groups)[g].group_name[i] == group_name[i])
		{
			for(t = 0; t < ((ETagGroup *)node->tag_groups)[g].tag_count; t++)
			{
				for(i = 0; ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] != 0 && ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == tag_name[i]; i++);
				if(((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == tag_name[i])
				{
					if(group_id != NULL)
						*group_id = g;
					if(tag_id != NULL)
						*tag_id = t;
					if(tag_type != NULL)
						*tag_type = ((ETagGroup *)node->tag_groups)[g].tags[t].type;
					return &((ETagGroup *)node->tag_groups)[g].tags[t].tag;
				}
			}
		}
	}
	if(group_id != NULL)
		*group_id = -1;
	if(tag_id != NULL)
		*tag_id = -1;
	return NULL;
}

VNTag *e_ns_get_tag_by_type_and_group(const ENodeHead *node, char *group_name, char *tag_name, VNTagType type, uint16 *group_id, uint16 *tag_id)
{
	uint i;
	uint16 g, t;
	for(g = 0; g < node->group_count; g++)
	{
		for(i = 0; ((ETagGroup *)node->tag_groups)[g].group_name[i] != 0 && ((ETagGroup *)node->tag_groups)[g].group_name[i] == group_name[i]; i++);
		if(((ETagGroup *)node->tag_groups)[g].group_name[i] == group_name[i])
		{
			for(t = 0; t < ((ETagGroup *)node->tag_groups)[g].tag_count; t++)
			{
				if(((ETagGroup *)node->tag_groups)[g].tags[t].type == type)
				{
					for(i = 0; ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] != 0 && ((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == tag_name[i]; i++);
					if(((ETagGroup *)node->tag_groups)[g].tags[t].tag_name[i] == tag_name[i])
					{
						if(group_id != NULL)
							*group_id = g;
						if(tag_id != NULL)
							*tag_id = t;
						return &((ETagGroup *)node->tag_groups)[g].tags[t].tag;
					}
				}
			}
		}
	}
	if(group_id != NULL)
		*group_id = -1;
	if(tag_id != NULL)
		*tag_id = -1;
	return NULL;
}

void callback_send_node_name_set(void *user, VNodeID node_id, const char *name)
{
	ENodeHead *node;
	uint i;
	if((node = (ENodeHead *)e_ns_get_node_networking(node_id)) == NULL)
		return;
	if(node == 0)
		return;
	for(i = 0; name[i] != 0 && i < 48; i++)
		node->node_name[i] = name[i];
	node->node_name[i] = name[i];
	e_ns_update_node_version_data(node);
}

void es_head_init(void)
{
	verse_callback_set(verse_send_tag_group_create, callback_send_tag_group_create, NULL);
	verse_callback_set(verse_send_tag_group_destroy, callback_send_tag_group_destroy, NULL);
	verse_callback_set(verse_send_tag_create, callback_send_tag_create, NULL);
	verse_callback_set(verse_send_tag_destroy, callback_send_tag_destroy, NULL);
	verse_callback_set(verse_send_node_name_set, callback_send_node_name_set, NULL);
}
