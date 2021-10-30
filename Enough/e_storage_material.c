#include "e_includes.h"

#include "verse.h"
#include "forge.h"
#include "e_types.h"
#include "enough.h"
#include "e_storage_node.h"



/*
	this file receaves and stores the material node and it does allso convert it in to NGLAGeometry indypendent
*/
#define E_CDC_COUNT 16

void (*storage_material_func[E_CDC_COUNT])(ENode *node, VNMFragmentID frag, ECustomDataCommand command);

typedef struct {
	VNMFragmentType	type;
	uint			version;
	VMatFrag		frag;
	void			*user[E_CDC_COUNT];
	boolean			mutex;
}ESFragment;

typedef struct{
	ENodeHead		head;
	ESFragment		*fragments;
	uint			alocated;
	uint			count;
	VNMFragmentID	output_color_front;
	VNMFragmentID	output_color_back;
	VNMFragmentID	output_particles;
	VNMFragmentID	output_displacement;
}ESMaterialNode;

VNMFragmentID e_nsm_get_fragment_next(ENode *node, VNMFragmentID id)
{
	for(;id < ((ESMaterialNode *)node)->alocated && ((ESMaterialNode *)node)->fragments[id].type > VN_M_FT_OUTPUT; id++);
	if(id == ((ESMaterialNode *)node)->alocated)
		return -1;
	return id;
}

VMatFrag *e_nsm_get_fragment(ENode *node, VNMFragmentID id)
{
	if(id >= ((ESMaterialNode *)node)->alocated || ((ESMaterialNode *)node)->fragments[id].type > VN_M_FT_OUTPUT)
		return NULL;
	return &((ESMaterialNode *)node)->fragments[id].frag;
}

boolean e_nsm_enter_fragment(ENode *node, VNMFragmentID id)
{
	if(id >= ((ESMaterialNode *)node)->alocated || ((ESMaterialNode *)node)->fragments[id].type > VN_M_FT_OUTPUT)
		return FALSE;
	if(((ESMaterialNode *)node)->fragments[id].mutex == TRUE)
		return FALSE;
	((ESMaterialNode *)node)->fragments[id].mutex = TRUE;
	return TRUE;	
}

void e_nsm_leave_fragment(ENode *node, VNMFragmentID id)
{
	if(id >= ((ESMaterialNode *)node)->alocated || ((ESMaterialNode *)node)->fragments[id].type > VN_M_FT_OUTPUT)
		return;
	((ESMaterialNode *)node)->fragments[id].mutex = FALSE;	
}


uint e_nsm_get_fragment_version(ENode *node, VNMFragmentID id)
{
	if(id >= ((ESMaterialNode *)node)->alocated || ((ESMaterialNode *)node)->fragments[id].type > VN_M_FT_OUTPUT)
		return 0;
	return ((ESMaterialNode *)node)->fragments[id].version;
}

VNMFragmentType e_nsm_get_fragment_type(ENode *node, VNMFragmentID id)
{
	if(id >= ((ESMaterialNode *)node)->alocated)
		return VN_M_FT_OUTPUT + 1;
	return ((ESMaterialNode *)node)->fragments[id].type;
}

VNMFragmentID e_nsm_get_fragment_color_front(ENode *node)
{
	return ((ESMaterialNode *)node)->output_color_front;
}

VNMFragmentID e_nsm_get_fragment_color_back(ENode *node)
{
	return ((ESMaterialNode *)node)->output_color_back;
}

VNMFragmentID e_nsm_get_fragment_color_particles(ENode *node)
{
	return ((ESMaterialNode *)node)->output_particles;
}

VNMFragmentID e_nsm_get_fragment_color_displacement(ENode *node)
{
	return ((ESMaterialNode *)node)->output_displacement;
}

VNMFragmentID e_nsm_find_empty_slot(ENode *node, VNMFragmentID id)
{
	for(;id < ((ESMaterialNode *)node)->alocated && ((ESMaterialNode *)node)->fragments[id].type <= VN_M_FT_OUTPUT; id++);
	return id;
}

ENode *e_create_m_node(uint node_id, VNodeOwner owner)
{
	ESMaterialNode		*node;
	uint i;
	if((node = (ESMaterialNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		node->alocated = 16;
		node->fragments = malloc((sizeof *node->fragments) * node->alocated);
		for(i = 0; i < node->alocated; i++)
			node->fragments[i].type = VN_M_FT_OUTPUT + 1; 
		node->output_color_front = (uint16)-1; 
		node->output_color_back = (uint16)-1;
		node->output_displacement = (uint16)-1;
		node->output_particles = (uint16)-1;
		node->count = 0;
		e_ns_init_head((ENodeHead *)node, V_NT_MATERIAL, node_id, owner);
	}
	return (ENode *)node;
}

void callback_send_m_fragment_create(void *user_data, VNodeID node_id, VNMFragmentID frag_id, VNMFragmentType type, VMatFrag *fragment)
{
	ESMaterialNode		*node;
	uint i;
	boolean	create = FALSE, data = FALSE;
	node = e_create_m_node(node_id, 0);
	if(frag_id >= node->alocated)
	{
		node->fragments = realloc(node->fragments, (sizeof *node->fragments) * (frag_id + 16));
		for(i = node->alocated; i < frag_id + 16; i++)
			node->fragments[i].type = VN_M_FT_OUTPUT + 1;
		node->alocated = frag_id + 16;
	}
	if(node->fragments[frag_id].type == VN_M_FT_COLOR && type == VN_M_FT_COLOR)
		data = TRUE;
	if(node->fragments[frag_id].type == VN_M_FT_MATRIX && type == VN_M_FT_MATRIX && fragment->matrix.data == node->fragments[frag_id].frag.matrix.data)
		data = TRUE;
	if(node->fragments[frag_id].type != type)
	{
		for(i = 0; i < E_CDC_COUNT; i++)
		{
			if(storage_material_func[i] != NULL && node->fragments[frag_id].type <= VN_M_FT_OUTPUT)
				storage_material_func[i](node, frag_id, E_CDC_DESTROY);
			node->fragments[frag_id].user[i] = NULL;
		}
		create = TRUE;
		if(node->fragments[frag_id].type <= VN_M_FT_OUTPUT)
			node->count--;
	}
	node->fragments[frag_id].type = type;
	if(create)
		node->fragments[frag_id].version = 0;
	else
		node->fragments[frag_id].version++;
	node->fragments[frag_id].frag = *fragment;
	node->fragments[frag_id].mutex = FALSE;
	if(node->fragments[frag_id].type == VN_M_FT_OUTPUT)
	{
		node->output_color_front = (uint16)-1;
		node->output_color_back = (uint16)-1;
		node->output_particles = (uint16)-1;
		node->output_displacement = (uint16)-1;
		for(i = 0; i < node->alocated; i++)
		{
			if(node->fragments[i].type == VN_M_FT_OUTPUT)
			{
				if(strcmp(node->fragments[i].frag.output.label, "color") == 0)
				{
					node->output_color_front = node->fragments[i].frag.output.front;
					node->output_color_back = node->fragments[i].frag.output.back;
				}
				else if(strcmp(node->fragments[i].frag.output.label, "particles") == 0)
				{
					if(node->fragments[i].frag.output.front != (VNMFragmentID) ~0)
						node->output_particles = node->fragments[i].frag.output.front;
					else
						node->output_particles = node->fragments[i].frag.output.back;
				}
				else if(strcmp(node->fragments[i].frag.output.label, "displacement") == 0)
				{
					if(node->fragments[i].frag.output.front != (VNMFragmentID) ~0)
						node->output_displacement = node->fragments[i].frag.output.front;
					else
						node->output_displacement = node->fragments[i].frag.output.back;
				}
			}
		}
	}
	
	if(create)
	{
		node->count++;
		for(i = 0; i < E_CDC_COUNT; i++)
			if(storage_material_func[i] != NULL)
				storage_material_func[i](node, frag_id, E_CDC_CREATE);
	}else if(data)
	{
		for(i = 0; i < E_CDC_COUNT; i++)
			if(storage_material_func[i] != NULL)
				storage_material_func[i](node, frag_id, E_CDC_DATA);
		e_ns_update_node_version_data(node);
		return;
	}else
		for(i = 0; i < E_CDC_COUNT; i++)
			if(storage_material_func[i] != NULL)
				storage_material_func[i](node, frag_id, E_CDC_STRUCT);
	e_ns_update_node_version_struct(node);
}

void callback_send_m_fragment_destroy(void *user_data, VNodeID node_id, VNMFragmentID frag_id)
{
	ESMaterialNode *node;
	uint i;
	if((node = (ESMaterialNode *)e_ns_get_node_networking(node_id)) == NULL)
		return;
	if(node->alocated <= frag_id)
		return;
	for(i = 0; i < E_CDC_COUNT; i++)
		if(storage_material_func[i] != NULL)
			storage_material_func[i](node, frag_id, E_CDC_DESTROY);
//	e_ns_update_node_version_data(node);
	e_ns_update_node_version_struct(node);
	if(node->fragments[frag_id].type <= VN_M_FT_OUTPUT)
		node->count--;
	if(node->fragments[frag_id].type == VN_M_FT_OUTPUT)
	{
		node->fragments[frag_id].type = VN_M_FT_OUTPUT + 1;
		node->output_color_front = (uint16)-1;
		node->output_color_back = (uint16)-1;
		node->output_particles = (uint16)-1;
		node->output_displacement = (uint16)-1;
		for(i = 0; i < node->alocated; i++)
		{
			if(node->fragments[i].type == VN_M_FT_OUTPUT)
			{
				if(strcmp(node->fragments[i].frag.output.label, "color") == 0)
				{
					node->output_color_front = node->fragments[i].frag.output.front;
					node->output_color_back = node->fragments[i].frag.output.back;
				}
				else if(strcmp(node->fragments[i].frag.output.label, "particles") == 0)
				{
					if(node->fragments[i].frag.output.front != (VNMFragmentID) ~0)
						node->output_particles = node->fragments[i].frag.output.front;
					else
						node->output_particles = node->fragments[i].frag.output.back;
				}
				else if(strcmp(node->fragments[i].frag.output.label, "displacement") == 0)
				{
					if(node->fragments[i].frag.output.front != (VNMFragmentID) ~0)
						node->output_displacement = node->fragments[i].frag.output.front;
					else
						node->output_displacement = node->fragments[i].frag.output.back;
				}
			}
		}
	}else
		node->fragments[frag_id].type = VN_M_FT_OUTPUT + 1;
}

uint e_nsm_get_fragment_count(ENode *node)
{
	return ((ESMaterialNode *)node)->count;
}

void delete_material(ESMaterialNode *node)
{
	free(node->fragments);
	free(node);
}

void e_nsm_set_custom_data(ENode *node, VNMFragmentID frag, uint slot, void *data)
{
	if(((ESMaterialNode *)node)->alocated <= frag && ((ESMaterialNode *)node)->fragments[frag].type <= VN_M_FT_OUTPUT)
		return;
	((ESMaterialNode *)node)->fragments[frag].user[slot] = data;
}

void e_nsm_set_custom_func(uint slot, void (*func)(ENode *node, VNMFragmentID frag, ECustomDataCommand command))
{
	storage_material_func[slot] = func;
}

void *e_nsm_get_custom_data(ENode *node, VNMFragmentID frag, uint slot)
{
	if(((ESMaterialNode *)node)->alocated <= frag || ((ESMaterialNode *)node)->fragments[frag].type > VN_M_FT_OUTPUT)
		return NULL;
	return((ESMaterialNode *)node)->fragments[frag].user[slot];
}

void es_material_init(void)
{
	uint i;
	for(i = 0; i < E_CDC_COUNT; i++)
		storage_material_func[i] = NULL;
	verse_callback_set(verse_send_m_fragment_create,				callback_send_m_fragment_create,	NULL);
	verse_callback_set(verse_send_m_fragment_destroy,				callback_send_m_fragment_destroy,	NULL);
}
