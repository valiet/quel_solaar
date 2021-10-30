#include "e_includes.h"

#include "verse.h"
#include "forge.h"
#include "e_types.h"
#include "e_storage_node.h"

typedef struct{ 
	char	weight[16];
	char	reference[16];
	char	pos_label[16];
	char	rot_label[16];
	char	scale_label[16];
	uint32	parent;
	real64	pos_x;
	real64	pos_y;
	real64	pos_z;
}EGeoBone;

typedef struct{
	char			name[16];
	uint16			layer_id;
	uint32			def_integer;
	double			def_real;
	VNGLayerType	type;
	void			*data;
	uint			version;
}EGeoLayer;

typedef struct{
	ENodeHead				head;
	EGeoLayer				*layers;
	uint32					layer_allocated;
	uint32					vertex_length; 
	uint32					polygon_length;
	egreal					space[6];
	boolean					space_recompute;
	char					vertex_crease_layer[16];
	uint32					vertex_crease;
	char					edge_crease_layer[16];
	uint32					edge_crease;
	EGeoBone				*bones;
	uint32					bones_allocated;
}ESGeometryNode;


extern uint	e_ns_get_node_version_struct(ESGeometryNode *node);
extern uint	e_ns_get_node_version_data(ESGeometryNode *node);
extern void		e_ns_update_node_version_struct(ESGeometryNode *node);
extern void		e_ns_update_node_version_data(ESGeometryNode *node);
extern uint	e_ns_get_node_id(ESGeometryNode *node);


#define GEOMETRY_ARRAY_CHUNK_SIZE 640

void vertex_append_array(ESGeometryNode	*node, uint32 id)
{
	uint			i;
	EGeoLayer	*end, *layer;

	if(id < node->vertex_length)
		return;

	layer = node->layers;
	for(end = layer + node->layer_allocated; layer != end; layer++)
	{
		if(layer->data != NULL)
		{
			switch(layer->type)
			{
				case VN_G_LAYER_VERTEX_XYZ :
				{
					layer->data = realloc(layer->data, sizeof(egreal) * (id + GEOMETRY_ARRAY_CHUNK_SIZE) * 3);
					for(i = node->vertex_length * 3; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE) * 3; i++)
						((egreal *)layer->data)[i] = E_REAL_MAX;
				}
				break;
				case VN_G_LAYER_VERTEX_UINT32 :
				{
					layer->data  = realloc(layer->data, sizeof(uint32) *  (id + GEOMETRY_ARRAY_CHUNK_SIZE));
					for(i = node->vertex_length ; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE); i++)
						((uint32 *)layer->data)[i] = layer->def_integer;
				}
				break;
				case VN_G_LAYER_VERTEX_REAL :
				{
					layer->data  = realloc(layer->data, sizeof(egreal) *  (id + GEOMETRY_ARRAY_CHUNK_SIZE));
					for(i = node->vertex_length; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE); i++)
						((egreal *)layer->data)[i] = layer->def_real;
				}
				break;
			}
		}
	}
	node->vertex_length = id + GEOMETRY_ARRAY_CHUNK_SIZE;
}

void polygon_append_array(ESGeometryNode	*node, uint32 id)
{
	uint			i;
	EGeoLayer	*end, *layer;
	if(id < node->polygon_length)
		return;

	layer = node->layers;
	for(end = layer + node->layer_allocated; layer != end; layer++)
	{
		if(layer->data != NULL)
		{
			switch(layer->type)
			{
				case VN_G_LAYER_POLYGON_CORNER_UINT32 :
				{
					layer->data  = realloc(layer->data, sizeof(uint32) * 4 * (id + GEOMETRY_ARRAY_CHUNK_SIZE));
					for(i = node->polygon_length * 4; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE) * 4; i++)
						((uint32 *)layer->data)[i] = layer->def_integer;
				}
				break;
				case VN_G_LAYER_POLYGON_CORNER_REAL :
				{
					layer->data  = realloc(layer->data, sizeof(egreal) * 4 * (id + GEOMETRY_ARRAY_CHUNK_SIZE));
					for(i = node->polygon_length * 4; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE) * 4; i++)
						((egreal *)layer->data)[i] = layer->def_real;
				}
				break;
				case VN_G_LAYER_POLYGON_FACE_UINT32 :
				{
					layer->data  = realloc(layer->data, sizeof(uint32) * (id + GEOMETRY_ARRAY_CHUNK_SIZE));
					for(i =  node->polygon_length ; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE); i++)
						((uint32 *)layer->data)[i] = layer->def_integer;
				}
				break;
				case VN_G_LAYER_POLYGON_FACE_UINT8 :
				{
					layer->data = realloc(layer->data, sizeof(uint8) * (id + GEOMETRY_ARRAY_CHUNK_SIZE));
					for(i =  node->polygon_length ; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE); i++)
						((uint8 *)layer->data)[i] = layer->def_integer;
				}
				break;
				case VN_G_LAYER_POLYGON_FACE_REAL :
				{
					layer->data  = realloc(layer->data, sizeof(egreal) * (id + GEOMETRY_ARRAY_CHUNK_SIZE));
					for(i =  node->polygon_length ; i < (id + GEOMETRY_ARRAY_CHUNK_SIZE); i++)
						((egreal *)layer->data)[i] = layer->def_real;
				}
				break;
			}
		}
	}
	node->polygon_length = id + GEOMETRY_ARRAY_CHUNK_SIZE;
}


void * e_nsg_get_layer_data(ESGeometryNode *g_node, EGeoLayer *layer)
{
	uint	i;
	if(layer == NULL)
		return NULL;
	if(layer->data != NULL)
		return layer->data;
	verse_send_g_layer_subscribe(g_node->head.node_id, layer->layer_id, E_GEOMETRY_SUBSCRIBE);
	switch(layer->type)
	{
		case VN_G_LAYER_VERTEX_XYZ :
		{
			layer->data = malloc(sizeof(egreal) * g_node->vertex_length * 3);
			for(i = 0 ; i < g_node->vertex_length * 3; i++)
				((egreal *)layer->data)[i] = E_REAL_MAX;
		}
		break;
		case VN_G_LAYER_VERTEX_UINT32 :
		{
			layer->data = malloc(sizeof(uint32) * g_node->vertex_length);
			for(i = 0 ; i < g_node->vertex_length; i++)
				((uint32 *)layer->data)[i] = layer->def_integer;
		}
		break;
		case VN_G_LAYER_VERTEX_REAL :
		{
			layer->data = malloc(sizeof(egreal) * g_node->vertex_length);
			for(i = 0 ; i < g_node->vertex_length; i++)
				((egreal *)layer->data)[i] = layer->def_real;
		}
		break;
		case VN_G_LAYER_POLYGON_CORNER_UINT32 :
		{
			layer->data = malloc(sizeof(uint32) * g_node->polygon_length * 4);
			for(i = 0 ; i < g_node->polygon_length * 4; i++)
				((uint32 *)layer->data)[i] = layer->def_integer;
		}
		break;
		case VN_G_LAYER_POLYGON_CORNER_REAL :
		{
			layer->data = malloc(sizeof(egreal) * g_node->polygon_length * 4);
			for(i = 0 ; i < g_node->polygon_length * 4; i++)
				((egreal *)layer->data)[i] = layer->def_real;
		}
		break;
		case VN_G_LAYER_POLYGON_FACE_UINT32 :
		{
			layer->data = malloc(sizeof(uint32) * g_node->polygon_length);
			for(i = 0 ; i < g_node->polygon_length; i++)
				((uint32 *)layer->data)[i] = layer->def_integer;
		}
		break;
		case VN_G_LAYER_POLYGON_FACE_UINT8 :
		{
			layer->data = malloc(sizeof(uint8) * g_node->polygon_length);
			for(i = 0 ; i < g_node->polygon_length; i++)
				((uint8 *)layer->data)[i] = layer->def_integer;
		}
		break;
		case VN_G_LAYER_POLYGON_FACE_REAL :
		{
			layer->data = malloc(sizeof(egreal) * g_node->polygon_length);
			for(i = 0 ; i < g_node->polygon_length; i++)
				((egreal *)layer->data)[i] = layer->def_real;
		}
		break;
	}		
	return layer->data;
}

VNGLayerType e_nsg_get_layer_type(EGeoLayer *layer)
{
	return layer->type;
}

uint e_nsg_get_layer_id(EGeoLayer *layer)
{
	return layer->layer_id;
}

uint e_nsg_get_layer_version(EGeoLayer *layer)
{
	return layer->version;
}

char *e_nsg_get_layer_name(EGeoLayer *layer)
{
	return layer->name;
}

EGeoLayer *e_nsg_get_layer_by_name(ESGeometryNode *node, const char *name)
{
	EGeoLayer *data, *end;
	data = node->layers;
	for(end = data + node->layer_allocated; data != end; data++)
		if(data->name[0] != 0)
			if(strcmp(name, data->name) == 0)
				return data;			
	return NULL;
}

EGeoLayer *e_nsg_get_layer_by_id(ESGeometryNode *node, uint layer_id)
{
	if(layer_id < node->layer_allocated)
		return &node->layers[layer_id];
	return NULL;
}


EGeoLayer* e_nsg_get_layer_next(ESGeometryNode *node, uint layer_id)
{
	for(; layer_id < node->layer_allocated && node->layers[layer_id].name[0] == 0; layer_id++);
	if(layer_id < node->layer_allocated)
		return &node->layers[layer_id];
	return NULL;
}

uint e_nsg_get_vertex_length(ESGeometryNode *node)
{
	return node->vertex_length;
}

uint e_nsg_get_polygon_length(ESGeometryNode *node)
{
	return node->polygon_length;
}

EGeoLayer* e_nsg_get_layer_by_type(ESGeometryNode *node, VNGLayerType type, const char *name)
{
	EGeoLayer *data, *end;
	data = node->layers;
	for(end = data + node->layer_allocated; data != end; data++)
		if(data->name[0] != 0)
			if(data->type == type)
				if(strcmp(name, data->name) == 0)
					return data;			
	return NULL;
}


EGeoLayer *e_nsg_get_layer_crease_vertex_layer(ESGeometryNode *g_node)
{
	return e_nsg_get_layer_by_type(g_node, VN_G_LAYER_VERTEX_UINT32, g_node->vertex_crease_layer);
}

EGeoLayer *e_nsg_get_layer_crease_edge_layer(ESGeometryNode *g_node)
{
	return e_nsg_get_layer_by_type(g_node, VN_G_LAYER_POLYGON_CORNER_UINT32, g_node->edge_crease_layer);	
}

char *e_nsg_get_layer_crease_vertex_name(ESGeometryNode *g_node)
{
	return g_node->vertex_crease_layer;
}

char *e_nsg_get_layer_crease_edge_name(ESGeometryNode *g_node)
{
	return g_node->edge_crease_layer;	
}

uint32 e_nsg_get_layer_crease_vertex_value(ESGeometryNode *g_node)
{
	return g_node->vertex_crease;
}

uint32 e_nsg_get_layer_crease_edge_value(ESGeometryNode *g_node)
{
	return g_node->edge_crease;	
}


void callback_send_g_polygon_set_corner_uint32(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 polygon_id, uint32 v0, uint32 v1, uint32 v2, uint32 v3)
{
	ESGeometryNode	*node;
	uint32			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_POLYGON_CORNER_UINT32)
		return;
	layer = &node->layers[layer_id];
	layer->version++;
	polygon_append_array(node, polygon_id);
	write = e_nsg_get_layer_data(node, layer);
	write += polygon_id * 4;
	*write++ = v0;
	*write++ = v1;
	*write++ = v2;
	*write++ = v3;
	e_ns_update_node_version_data(node);
	if(layer_id == 1 || strcmp(node->edge_crease_layer, layer->name) == 0)
		e_ns_update_node_version_struct(node);
}

void callback_send_g_polygon_set_corner_real(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 polygon_id, egreal v0, egreal v1, egreal v2, egreal v3)
{
	ESGeometryNode	*node;
	egreal			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_POLYGON_CORNER_REAL)
		return;
	layer = &node->layers[layer_id];
	layer->version++;
	polygon_append_array(node, polygon_id);
	write = e_nsg_get_layer_data(node, layer);
	write += polygon_id * 4;
	*write++ = v0;
	*write++ = v1;
	*write++ = v2;
	*write++ = v3;
	e_ns_update_node_version_data(node);
}

void callback_send_g_polygon_set_face_uint32(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 polygon_id, uint32 value)
{
	ESGeometryNode	*node;
	uint32			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_POLYGON_FACE_UINT32)
		return;
	layer = &node->layers[layer_id];
	layer->version++;
	polygon_append_array(node, polygon_id);
	write = e_nsg_get_layer_data(node, layer);
	write += polygon_id;
	*write = value;
	e_ns_update_node_version_data(node);
}


void callback_send_g_polygon_set_face_uint8(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 polygon_id, uint8 value)
{
	ESGeometryNode	*node;
	uint8			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_POLYGON_FACE_UINT8)
		return;
	layer = &node->layers[layer_id];
	layer->version++;
	polygon_append_array(node, polygon_id);
	write = e_nsg_get_layer_data(node, layer);
	write += polygon_id;
	*write = value;
	e_ns_update_node_version_data(node);
}


void callback_send_g_polygon_set_face_real(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 polygon_id, egreal value)
{
	ESGeometryNode	*node;
	egreal			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_POLYGON_FACE_REAL)
		return;
	layer = &node->layers[layer_id];
	layer->version++;
	polygon_append_array(node, polygon_id);
	write = e_nsg_get_layer_data(node, layer);
	write += polygon_id;
	*write = value;
	e_ns_update_node_version_data(node);
}

extern void *p_sub_get_anim_data(void *mesh);
extern void p_geo_update_anim(void *data, uint layer_id, uint vertex_id, float x, float y, float z);

void callback_send_g_vertex_set_real_xyz(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 vertex_id, egreal x, egreal y, egreal z)
{
	ESGeometryNode	*node;
	egreal			*write, input[3];
	EGeoLayer		*layer;
	uint			i, j;
	boolean			update = FALSE;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_VERTEX_XYZ)
		return;
	layer = &node->layers[layer_id];
	layer->version++;	
	vertex_append_array(node, vertex_id);	
	write = e_nsg_get_layer_data(node, layer);

	input[0] = x;
	input[1] = y;
	input[2] = z;
	for(i = 0; i < 6; i++)
	{
		x = input[i / 2];
		if(x > node->space[i])
			node->space[i] = x;
		else if(node->space[i] == write[vertex_id * 3 + i / 2] || node->vertex_length < GEOMETRY_ARRAY_CHUNK_SIZE + 2)
			node->space_recompute = TRUE;
		i++;
		if(x < node->space[i])
			node->space[i] = x;
		else if(node->space[i] == write[vertex_id * 3 + i / 2] || node->vertex_length < GEOMETRY_ARRAY_CHUNK_SIZE + 2)
			node->space_recompute = TRUE;
	}

	write += vertex_id * 3;
	if(layer_id == 0 && *write == E_REAL_MAX)
		update = TRUE;
	*write++ = input[0];
	*write++ = input[1];
	*write++ = input[2];

	e_ns_update_node_version_data(node);
	if(update)
		e_ns_update_node_version_struct(node);
}

void callback_send_g_vertex_set_uint32(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 vertex_id, uint32 value)
{
	ESGeometryNode	*node;
	uint			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_VERTEX_UINT32)
		return;
	layer = &node->layers[layer_id];
	layer->version++;
	vertex_append_array(node, vertex_id);
	write = e_nsg_get_layer_data(node, layer);
	write += vertex_id;
	*write = value;
	e_ns_update_node_version_data(node);
	if(strcmp(node->vertex_crease_layer, layer->name) == 0)
		e_ns_update_node_version_struct(node);
}

void callback_send_g_vertex_set_real(void *user_data, VNodeID node_id, VLayerID layer_id, uint32 vertex_id, egreal value)
{
	ESGeometryNode	*node;
	egreal			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(layer_id >= node->layer_allocated || node->layers[layer_id].name[0] == 0 || node->layers[layer_id].type != VN_G_LAYER_VERTEX_REAL)
		return;
	layer = &node->layers[layer_id];
	layer->version++;
	vertex_append_array(node, vertex_id);
	write = e_nsg_get_layer_data(node, layer);
	write += vertex_id;
	*write = value;
	e_ns_update_node_version_data(node);
	if(strcmp(node->vertex_crease_layer, layer->name) == 0)
		e_ns_update_node_version_struct(node);
}

extern ESGeometryNode	*e_create_g_node(VNodeID node_id, VNodeOwner owner);

void callback_send_g_layer_create(void *user_data, VNodeID node_id, VLayerID layer_id, char *name, VNGLayerType type, uint32 def_integer, real64 def_real)
{
	ESGeometryNode	*node;
	EGeoLayer		*layer;
	uint			i;

	node = e_create_g_node(node_id, 0);

	if(layer_id >= node->layer_allocated)
	{
		node->layers = realloc(node->layers, (sizeof *node->layers) * (layer_id + 8));
		for(;node->layer_allocated < layer_id + 8; node->layer_allocated++)
		{
			node->layers[node->layer_allocated].name[0] = 0;
			node->layers[node->layer_allocated].type = -1;
			node->layers[node->layer_allocated].data = NULL;
		}
	}
	layer = &node->layers[layer_id];
	if(type != layer->type)
	{
		if(layer->data != NULL)
		{
			free(layer->data);
			layer->data = NULL;
		}
	}
	layer->layer_id = layer_id;
	layer->def_integer = def_integer;
	if(layer_id == 1)
		layer->def_integer = -1;
	layer->def_real = def_real;
	layer->type = type;
	layer->version = 0;
	if(layer->name[0] == 0 || layer->type != type)
	{
		if(layer->name[0] != 0)
			free(layer->data);
		layer->data = NULL;
		if(type == VN_G_LAYER_VERTEX_XYZ || layer->layer_id < 2)
			e_nsg_get_layer_data(node, layer);
		if(type == VN_G_LAYER_VERTEX_XYZ || type == VN_G_LAYER_VERTEX_UINT32 || type == VN_G_LAYER_VERTEX_REAL)
		{
			if(type == VN_G_LAYER_VERTEX_XYZ || layer->layer_id < 2)
				e_nsg_get_layer_data(node, layer);
			if(strcmp(node->vertex_crease_layer, name) == 0 || strcmp(node->edge_crease_layer, name) == 0)
				e_nsg_get_layer_data(node, layer);
		}
	}
	e_nsg_get_layer_data(node, layer);
	for(i = 0; name[i] != 0 && i < 15; i++)
		layer->name[i] = name[i];
	layer->name[i] = 0;
	e_ns_update_node_version_struct(node);
}

ESGeometryNode	*e_create_g_node(VNodeID node_id, VNodeOwner owner)
{
	ESGeometryNode	*node;
	if((node = (ESGeometryNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		uint i;
		node = malloc(sizeof *node);
		node->layer_allocated = 0;
		node->layers = NULL;
		node->vertex_length = 16;
		node->polygon_length = 16;
		node->vertex_crease_layer[0] = 0;
		node->vertex_crease = 0;
		node->edge_crease_layer[0] = 0;
		node->edge_crease = 0;
		node->bones = NULL;
		node->bones_allocated = 0;
		node->space_recompute = TRUE;
		for(i = 0; i < 6; i++)
			node->space[i] = 0;
		e_ns_init_head((ENodeHead *)node, V_NT_GEOMETRY, node_id, owner);
		callback_send_g_layer_create(NULL, node_id, 0, "vertex", VN_G_LAYER_VERTEX_XYZ, 0, 0);
		callback_send_g_layer_create(NULL, node_id, 1, "polygon", VN_G_LAYER_POLYGON_CORNER_UINT32, 0, 0);
	}
	return node;
}

void callback_send_g_layer_destroy(void *user_data, VNodeID node_id, VLayerID layer_id)
{
	ESGeometryNode	*node;
	node = (ESGeometryNode *) e_ns_get_node_networking(node_id);
	if(node == NULL || node->layer_allocated <= layer_id)
		return;
	if(node->layers[layer_id].data != NULL)
		free(node->layers[layer_id].data);
	node->layers[layer_id].name[0] = 0;
	e_ns_update_node_version_struct(node);
}

void callback_send_g_polygon_delete(void *user_data, VNodeID node_id, uint32 polygon_id)
{
	ESGeometryNode	*node;
	uint32			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(1 >= node->layer_allocated)
		return;
	layer = &node->layers[1];
	layer->version++;
	polygon_append_array(node, polygon_id);
	write = e_nsg_get_layer_data(node, layer);
	write += polygon_id * 4;
	*write++ = -1;
	*write++ = -1;
	*write++ = -1;
	*write++ = -1;
	e_ns_update_node_version_data(node);
	e_ns_update_node_version_struct(node);
}

void callback_send_g_vertex_delete(void *user_data, VNodeID node_id, uint32 vertex_id)
{
	ESGeometryNode	*node;
	egreal			*write;
	EGeoLayer		*layer;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(0 >= node->layer_allocated || node->layers[0].name[0] == 0 || node->layers[0].type != VN_G_LAYER_VERTEX_XYZ)
		return;
	layer = &node->layers[0];
	layer->version++;
	vertex_append_array(node, vertex_id);
	write = e_nsg_get_layer_data(node, layer);
	write += vertex_id * 3;
	*write++ = E_REAL_MAX;
	*write++ = E_REAL_MAX;
	*write++ = E_REAL_MAX;
	e_ns_update_node_version_data(node);
	e_ns_update_node_version_struct(node);
}

void delete_geometry(ESGeometryNode	*g_node)
{
	uint i;
	for(i = 0; i < g_node->layer_allocated; i++)
		if(g_node->layers[i].name[0] != 0 && g_node->layers[i].data != NULL)
			free(g_node->layers[i].data);
	free(g_node->layers);
	free(g_node);
}

void callback_send_g_crease_set_vertex(void *user_data, VNodeID node_id, char *layer, uint32 def_crease)
{
	ESGeometryNode	*node;
	EGeoLayer	*crease_layer;
	uint i;
	node = e_create_g_node(node_id, 0);
	for(i = 0; layer[i] != 0 && i < 15; i++)
		node->vertex_crease_layer[i] = layer[i];
	node->vertex_crease_layer[i] = layer[i];
	node->vertex_crease = def_crease;
	crease_layer = e_nsg_get_layer_by_type(node, VN_G_LAYER_VERTEX_UINT32, layer);
	if(crease_layer != NULL)
		e_nsg_get_layer_data(node, crease_layer);
	e_ns_update_node_version_struct(node);
}

void callback_send_g_crease_set_edge(void *user_data, VNodeID node_id, char *layer, uint32 def_crease)
{
	ESGeometryNode	*node;
	EGeoLayer	*crease_layer;
	uint i;
	node = e_create_g_node(node_id, 0);
	for(i = 0; layer[i] != 0 && i < 15; i++)
		node->edge_crease_layer[i] = layer[i];
	node->edge_crease_layer[i] = layer[i];
	node->edge_crease = def_crease;
	crease_layer = e_nsg_get_layer_by_type(node, VN_G_LAYER_POLYGON_CORNER_UINT32, layer);
	if(crease_layer != NULL)
		e_nsg_get_layer_data(node, crease_layer);
	e_ns_update_node_version_struct(node);
}

uint e_nsg_find_empty_vertex_slot(ESGeometryNode *node, uint start)
{
	int			i, count;
	EGeoLayer	*layer;
	egreal		*data;
	layer = e_nsg_get_layer_by_id(node, 0);
	count = node->vertex_length * 3;
	if(layer != NULL)
	{
		data = e_nsg_get_layer_data(node, layer);
		for(i = start * 3; i < count; i += 3)
		{
			if(data[i] == E_REAL_MAX)
				return i / 3;
		}
	}
	if(node->vertex_length > start)
		return node->vertex_length;
	else
		return start;
}


uint e_nsg_find_empty_polygon_slot(ESGeometryNode *node, uint start)
{
	int			i, count;
	EGeoLayer	*layer;
	uint32		*data;
	layer = e_nsg_get_layer_by_id(node, 1);
	count = node->polygon_length * 4;
	if(layer != NULL)
	{
		data = e_nsg_get_layer_data(node, layer);
		for(i = start * 4; i < count; i += 4)
			if(data[i] > node->vertex_length || data[i + 1] > node->vertex_length || data[i + 2] > node->vertex_length)
				return i / 4;
	}
	if(node->polygon_length > start)
		return node->polygon_length;
	else
		return start;
}

void e_nsg_re_compute_space(ESGeometryNode *node)
{
	uint		i;
	EGeoLayer	*end, *layer;
	if(node->space_recompute != TRUE)
		return;

	layer = node->layers;
	for(i = 0; i < node->vertex_length * 3 && ((egreal *)layer->data)[i] == E_REAL_MAX; i += 3)
		;
	if(i == node->vertex_length * 3)
		return;
	node->space_recompute = FALSE;
	node->space[0] = ((egreal *)layer->data)[i];
	node->space[1] = ((egreal *)layer->data)[i];
	i++;
	node->space[2] = ((egreal *)layer->data)[i];
	node->space[3] = ((egreal *)layer->data)[i];
	i++;
	node->space[4] = ((egreal *)layer->data)[i];
	node->space[5] = ((egreal *)layer->data)[i];


	for(end = layer + node->layer_allocated; layer != end; layer++)
	{
		if(layer->data != NULL)
		{
			if(layer->type == VN_G_LAYER_VERTEX_XYZ)
			{
				for(i = 0; i < node->vertex_length * 3;)
				{
					if(((egreal *)layer->data)[i] != E_REAL_MAX)
					{
						if(((egreal *)layer->data)[i] > node->space[0])
							node->space[0] = ((egreal *)layer->data)[i];
						if(((egreal *)layer->data)[i] < node->space[1])
							node->space[1] = ((egreal *)layer->data)[i];
						i++;
						if(((egreal *)layer->data)[i] > node->space[2])
							node->space[2] = ((egreal *)layer->data)[i];
						if(((egreal *)layer->data)[i] < node->space[3])
							node->space[3] = ((egreal *)layer->data)[i];
						i++;
						if(((egreal *)layer->data)[i] > node->space[4])
							node->space[4] = ((egreal *)layer->data)[i];
						if(((egreal *)layer->data)[i] < node->space[5])
							node->space[5] = ((egreal *)layer->data)[i];
						i++;
					}else
						i += 3;
				}
			}
		}
	}
}

void e_nsg_get_center(ESGeometryNode *node, egreal *center)
{
	e_nsg_re_compute_space(node);
	center[0] = node->space[1] + (node->space[0] - node->space[1]) * 0.5;
	center[1] = node->space[3] + (node->space[2] - node->space[3]) * 0.5;
	center[2] = node->space[5] + (node->space[4] - node->space[5]) * 0.5;
}

void e_nsg_get_bounding_box(ESGeometryNode *node, egreal *high_x, egreal *low_x, egreal *high_y, egreal *low_y, egreal *high_z, egreal *low_z)
{
	e_nsg_re_compute_space(node);
	if(high_x != NULL)
		*high_x = node->space[0];
	if(low_x != NULL)
		*low_x = node->space[1];
	if(high_y != NULL)
		*high_y = node->space[2];
	if(low_y != NULL)
		*low_y = node->space[3];
	if(high_z != NULL)
		*high_z = node->space[4];
	if(low_z != NULL)
		*low_z = node->space[5];
}

egreal e_nsg_get_size(ESGeometryNode *node)
{
	e_nsg_re_compute_space(node);
	return sqrt((node->space[0] - node->space[1]) * (node->space[0] - node->space[1]) +
	(node->space[2] - node->space[3]) * (node->space[2] - node->space[3]) +
	(node->space[4] - node->space[5]) * (node->space[4] - node->space[5]));
}



void callback_send_g_bone_create(void *user, VNodeID node_id, uint16 bone_id, const char *weight, const char *reference, uint16 parent,
				 real64 pos_x, real64 pos_y, real64 pos_z, char *pos_label, char *rot_label, char *scale_label)
{
	ESGeometryNode	*node;
	uint			i;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(node == NULL)
		return;

	if(node->bones_allocated <= bone_id)
	{
		i = node->bones_allocated;
		node->bones_allocated = bone_id + 16;
		node->bones = realloc(node->bones, (sizeof *node->bones) * node->bones_allocated);
		while(i < node->bones_allocated)
			node->bones[i++].weight[0] = 0;
	}

	for(i = 0; weight[i] != 0 && node->bones[bone_id].weight[i] != 0 && weight[i] == node->bones[bone_id].weight[i]; i++);

	if(weight[i] != node->bones[bone_id].weight[i] || node->bones[bone_id].parent != parent)
		e_ns_update_node_version_struct(node);
	e_ns_update_node_version_data(node);

	for(i = 0; weight[i] != 0 && i < 15; i++)
		node->bones[bone_id].weight[i] = weight[i];
	node->bones[bone_id].weight[i] = 0;	
	for(i = 0; reference[i] != 0 && i < 15; i++)
		node->bones[bone_id].reference[i] = reference[i];
	node->bones[bone_id].reference[i] = 0;

	for(i = 0; pos_label[i] != 0 && i < 15; i++)
		node->bones[bone_id].pos_label[i] = pos_label[i];
	node->bones[bone_id].pos_label[i] = 0;	
	for(i = 0; rot_label[i] != 0 && i < 15; i++)
		node->bones[bone_id].rot_label[i] = rot_label[i];
	node->bones[bone_id].rot_label[i] = 0;
	for(i = 0; scale_label[i] != 0 && i < 15; i++)
		node->bones[bone_id].scale_label[i] = scale_label[i];
	node->bones[bone_id].scale_label[i] = '\0';

	node->bones[bone_id].parent = parent,
	node->bones[bone_id].pos_x = pos_x;
	node->bones[bone_id].pos_y = pos_y;
	node->bones[bone_id].pos_z = pos_z;
}

void callback_send_g_bone_destroy(void *user, VNodeID node_id, uint16 bone_id)
{
	ESGeometryNode	*node;
	node = (ESGeometryNode *)e_ns_get_node_networking(node_id);
	if(node == NULL)
		return;

	if(bone_id >= node->bones_allocated || node->bones[bone_id].weight[0] == 0)
		return;

	node->bones[bone_id].weight[0] = 0;	
	e_ns_update_node_version_struct(node);
	e_ns_update_node_version_data(node);
}


uint16 e_nsg_get_bone_by_weight(ESGeometryNode *g_node, char *name)
{
	uint i, j;
	for(i = 0; i < g_node->bones_allocated; i++)
	{
		for(j = 0; name[j] == g_node->bones[i].weight[j] && g_node->bones[i].weight[j] != 0; j++)
		if(name[j] == g_node->bones[i].weight[j])
			return i;
	}
	return -1;
}

uint16 e_nsg_get_bone_next(ESGeometryNode	*g_node, uint16 bone_id)
{
	for(; bone_id < g_node->bones_allocated; bone_id++)
		if(g_node->bones[bone_id].weight[0] != 0)
			return bone_id;
	return (uint16) -1;
}

char *e_nsg_get_bone_weight(ESGeometryNode *g_node, uint16 bone_id)
{
	if(bone_id < g_node->bones_allocated)
		return g_node->bones[bone_id].weight;
	return NULL;
}

char *e_nsg_get_bone_reference(ESGeometryNode *g_node, uint16 bone_id)
{
	if(bone_id < g_node->bones_allocated)
		return g_node->bones[bone_id].reference;
	return NULL;
}

char *e_nsg_get_bone_pos_label(ESGeometryNode *g_node, uint16 bone_id)
{
	if(bone_id < g_node->bones_allocated)
		return g_node->bones[bone_id].pos_label;
	return NULL;
}

char *e_nsg_get_bone_rot_label(ESGeometryNode *g_node, uint16 bone_id)
{
	if(bone_id < g_node->bones_allocated)
		return g_node->bones[bone_id].rot_label;
	return NULL;
}

char *e_nsg_get_bone_scale_label(ESGeometryNode *g_node, uint16 bone_id)
{
	if(bone_id < g_node->bones_allocated)
		return g_node->bones[bone_id].scale_label;
	return NULL;
}

uint16 e_nsg_get_bone_parent(ESGeometryNode *g_node, uint16 bone_id)
{
	if(bone_id < g_node->bones_allocated)
		return g_node->bones[bone_id].parent;
	return -1;
}

void e_nsg_get_bone_pos32(ESGeometryNode *g_node, uint16 bone_id, float *pos)
{
	if(bone_id < g_node->bones_allocated)
	{
		pos[0] = g_node->bones[bone_id].pos_x;
		pos[1] = g_node->bones[bone_id].pos_y;
		pos[2] = g_node->bones[bone_id].pos_z;
	}
}

void e_nsg_get_bone_pos64(ESGeometryNode *g_node, uint16 bone_id, double *pos)
{
	if(bone_id < g_node->bones_allocated)
	{
		pos[0] = g_node->bones[bone_id].pos_x;
		pos[1] = g_node->bones[bone_id].pos_y;
		pos[2] = g_node->bones[bone_id].pos_z;
	}
}

void e_nsg_get_bone_rot32(ESGeometryNode *g_node, uint16 bone_id, VNQuat32 *rot)
{
/*	if(bone_id < g_node->bones_allocated && rot != NULL)
	{
		rot->x = g_node->bones[bone_id].rot.x;
		rot->y = g_node->bones[bone_id].rot.y;
		rot->z = g_node->bones[bone_id].rot.z;
		rot->w = g_node->bones[bone_id].rot.w;
	}
*/
}

void e_nsg_get_bone_rot64(ESGeometryNode *g_node, uint16 bone_id, VNQuat64 *rot)
{
/*	if(bone_id < g_node->bones_allocated && rot != NULL)
		*rot = g_node->bones[bone_id].rot;
*/
} 

void e_nsg_get_bone_matrix32(ESGeometryNode *o_node, ESGeometryNode *g_node, uint16 bone_id, float *matrix)
{
}

void e_nsg_get_bone_matrix64(ESGeometryNode *o_node, ESGeometryNode *g_node, uint16 bone_id, double *matrix)
{
}

//	Using the property of unit quaternions that w2 + x2 + y2 + z2 = 1, we can reduce the matrix to

double create_q_w(double x, double y, double z)
{
	return sqrt(x * x + y * y + z * z);
}

void create_q_nomalize(double *q)
{
	double r;
	r = sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
	q[0] /= r;
	q[1] /= r;
	q[2] /= r;
	q[3] /= r;
}

void create_matrix_from_q(double *matrix, double x, double y, double z, double w)
{
	matrix[0] = w * w + x * x - y * y - z * z;
	matrix[4] = 2 * x * y - 2 * w * z;
	matrix[8] = 2 * x * z + 2 * w * y;
	matrix[12] = 0; 

	matrix[1] = 2 * x * y + 2 * w * z;
	matrix[5] = w * w - x * x + y * y - z * z; 
	matrix[9] = 2 * y * z - 2 * w * x;
	matrix[13] = 0;

	matrix[2] = 2 * x * z - 2 * w * y;
	matrix[6] = 2 * y * z + 2 * w * x; 
	matrix[10] = w * w - x * x - y * y + z * z;
	matrix[14] = 0; 

	matrix[3] = 0;
	matrix[7] = 0; 
	matrix[11] = 0;
	matrix[15] = 1; 
}
/*
void create_matrix_from_q(double *matrix, double x, double y, double z, double w)
{
	matrix[0] = 1 - 2 * y * y - 2 * z * z;
	matrix[4] = 2 * x * y - 2 * w * z;
	matrix[8] = 2 * x * z + 2 * w * y;
	matrix[12] = 0; 

	matrix[1] = 2 * x * y + 2 * w * z;
	matrix[5] = 1 - 4 * x * x - 4 * z * z; 
	matrix[9] = 2 * y * z - 2 * w * x;
	matrix[13] = 0;

	matrix[2] = 2 * x * z - 2 * w * y;
	matrix[6] = 2 * y * z + 2 * w * x; 
	matrix[10] = 1 - 2 * x * x - 2 * y * y;
	matrix[14] = 0; 

	matrix[3] = 0;
	matrix[7] = 0; 
	matrix[11] = 0;
	matrix[15] = 1; 
}*/
void q_test(double *matrix, double t)
{
	double q_a[4] = {-0.1, 0.4, -0.1, 4};
	double q_b[4] = {8, -0.2, 9, 4};
	double q_c[4];

//	q_a[3] = create_q_w(q_a[0], q_a[1], q_a[2]);
//	q_b[3] = create_q_w(q_b[0], q_b[1], q_b[2]);
	create_q_nomalize(q_a);
	create_q_nomalize(q_b);

	t = sin(t) * 0.5 + 0.5;

	q_c[0] = q_a[0] * t + q_b[0] * (1 - t);
	q_c[1] = q_a[1] * t + q_b[1] * (1 - t);
	q_c[2] = q_a[2] * t + q_b[2] * (1 - t);
	q_c[3] = q_a[3] * t + q_b[3] * (1 - t);

//	q_c[3] = create_q_w(q_c[0], q_c[1], q_c[2]);
	create_q_nomalize(q_c);

	create_matrix_from_q(matrix, q_c[0], q_c[1], q_c[2], q_c[3]);
}

void es_geometry_init(void)
{
	verse_callback_set(verse_send_g_layer_create,					callback_send_g_layer_create,					NULL);
	verse_callback_set(verse_send_g_layer_destroy,					callback_send_g_layer_destroy,					NULL);

#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
	verse_callback_set(verse_send_g_vertex_set_xyz_real64,			callback_send_g_vertex_set_real_xyz,			NULL);
	verse_callback_set(verse_send_g_vertex_set_real64,				callback_send_g_vertex_set_real,				NULL);
	verse_callback_set(verse_send_g_polygon_set_corner_real64,		callback_send_g_polygon_set_corner_real,		NULL);
	verse_callback_set(verse_send_g_polygon_set_face_real64,		callback_send_g_polygon_set_face_real,			NULL);
#endif

#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
	verse_callback_set(verse_send_g_vertex_set_xyz_real32,			callback_send_g_vertex_set_real_xyz,			NULL);
	verse_callback_set(verse_send_g_vertex_set_real32,				callback_send_g_vertex_set_real,				NULL);
	verse_callback_set(verse_send_g_polygon_set_corner_real32,		callback_send_g_polygon_set_corner_real,		NULL);
	verse_callback_set(verse_send_g_polygon_set_face_real32,		callback_send_g_polygon_set_face_real,			NULL);
#endif
	
	verse_callback_set(verse_send_g_vertex_set_uint32,				callback_send_g_vertex_set_uint32,				NULL);
	verse_callback_set(verse_send_g_vertex_delete_real64,			callback_send_g_vertex_delete,					NULL);
	verse_callback_set(verse_send_g_vertex_delete_real32,			callback_send_g_vertex_delete,					NULL);

	verse_callback_set(verse_send_g_polygon_set_corner_uint32,		callback_send_g_polygon_set_corner_uint32,		NULL);
	verse_callback_set(verse_send_g_polygon_set_face_uint32,		callback_send_g_polygon_set_face_uint32,		NULL);
	verse_callback_set(verse_send_g_polygon_set_face_uint8,			callback_send_g_polygon_set_face_uint8, 		NULL);
	verse_callback_set(verse_send_g_polygon_delete,					callback_send_g_polygon_delete,					NULL);

	verse_callback_set(verse_send_g_crease_set_vertex,				callback_send_g_crease_set_vertex,				NULL);
	verse_callback_set(verse_send_g_crease_set_edge,				callback_send_g_crease_set_edge,				NULL);

	verse_callback_set(verse_send_g_bone_create,					callback_send_g_bone_create,					NULL);
	verse_callback_set(verse_send_g_bone_destroy,					callback_send_g_bone_destroy,					NULL);
}
