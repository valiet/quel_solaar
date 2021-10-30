
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "verse.h"
#include "forge.h"

#include "e_types.h"
#include "e_storage_node.h"

typedef struct{
	uint			global_version;
	VNodeID			node_id;
	char			layers[3][16];
	void			*data[3];
	ebreal			(*func[3])(void *data, uint pixel);
	uint			size_x;
	ebreal			size_y;
	ebreal			size_z;
}EBMHandle;

typedef struct{
	VLayerID		layer_id;
	VNBLayerType	type;
	char			name[16];
	void			*data;
	uint			version;
}ESBitmapLayer;

typedef struct{
	ENodeHead		head;
	EDynLookUpTable	layertables;
	uint			size_x;
	uint			size_y;
	uint			size_z;
	uint			first;
}ESBitmapNode;


extern void	e_ns_update_node_version_struct(ESBitmapNode *node);
extern void	e_ns_update_node_version_data(ESBitmapNode *node);

uint global_version;
uint global_node_id;

EBMHandle def_handle;

ebreal e_nsb_get_aspect(ESBitmapNode *node)
{
	return (ebreal)node->size_x / (ebreal)node->size_y;
}

void e_nsb_get_size(ESBitmapNode *node, uint *x, uint *y, uint *z)
{
	if(x != NULL)
		*x = node->size_x;
	if(y != NULL)
		*y = node->size_y;
	if(z != NULL)
		*z = node->size_z;
}

ESBitmapLayer *e_nsb_get_layer_by_name(ESBitmapNode *node, const char *name)
{
	ESBitmapLayer *layer;
	for(layer =	e_dlut_next(&node->layertables, 0); layer != NULL; layer = e_dlut_next(&node->layertables, layer->layer_id + 1))
		if(strcmp(name, layer->name) == 0)
			return layer;
	return NULL;
}

ESBitmapLayer *e_nsb_get_layer_by_id(ESBitmapNode *node, uint layer_id)
{
	return e_dlut_get(&node->layertables, layer_id);
}

ESBitmapLayer *e_nsb_get_layer_by_type(ESBitmapNode *node, VNBLayerType type, const char *name)
{
	ESBitmapLayer *layer;
	for(layer =	e_dlut_next(&node->layertables, 0); layer != NULL; layer = e_dlut_next(&node->layertables, layer->layer_id + 1))
		if(layer->type == type && strcmp(name, layer->name) == 0)
			return layer;
	return NULL;
}

ESBitmapLayer *e_nsb_get_layer_next(ESBitmapNode *node, uint layer_id)
{
	return e_dlut_next(&node->layertables, layer_id);
}

void *e_nsb_get_layer_data(ESBitmapNode *node, ESBitmapLayer *layer)
{
	uint i;
	if(layer->data == NULL)
	{
		if(layer->type == VN_B_LAYER_UINT1)
		{
			layer->data = malloc(sizeof(uint8) * (node->size_x + 7) / 8 * node->size_y * node->size_z);
		}else if(layer->type == VN_B_LAYER_UINT8)
		{
			layer->data = malloc(sizeof(uint8) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((uint8 *)layer->data)[i] = 0;
//				((uint8 *)layer->data)[i] = ((layer->layer_id + i) % 3) * 127;
		}
		else if(layer->type == VN_B_LAYER_UINT16)
		{
			layer->data = malloc(sizeof(uint16) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((uint16 *)layer->data)[i] = 0;
//				((uint16 *)layer->data)[i] = ((layer->layer_id + i) % 3) * 127 * 256;
		}
		else if(layer->type == VN_B_LAYER_REAL32)
		{
			layer->data = malloc(sizeof(float) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((float *)layer->data)[i] = 0;
//				((float *)layer->data)[i] = (float)((layer->layer_id + i) % 3) * 0.5;
		}
		else
		{
			layer->data = malloc(sizeof(double) * node->size_x * node->size_y * node->size_z);
			for(i = 0; i < node->size_x * node->size_y * node->size_z; i++)
				((double *)layer->data)[i] = 0;
//				((double *)layer->data)[i] = (double)((layer->layer_id + i) % 3) * 0.5;
		}
		verse_send_b_layer_subscribe(node->head.node_id, layer->layer_id, 0);
	}
	return layer->data;
}

uint e_nsb_get_layer_id(ESBitmapLayer *layer)
{
	return layer->layer_id;
}

char *e_nsb_get_layer_name(ESBitmapLayer *layer)
{
	return layer->name;
}
VNBLayerType e_nsb_get_layer_type(ESBitmapLayer *layer)
{
	return layer->type;
}

uint e_nsb_get_layer_version(ESBitmapLayer *layer)
{
	return layer->version;
}

EBMHandle *e_nsb_get_empty_handle(void)
{
	def_handle.node_id = 0;
	return &def_handle;
}

ESBitmapNode *e_create_b_node(VNodeID node_id, VNodeOwner owner)
{
	ESBitmapNode	*node;
	if((node = (ESBitmapNode *) e_ns_get_node_networking(node_id)) == NULL)
	{
		node = malloc(sizeof *node);
		e_dlut_init(&node->layertables); 
		node->size_x = 0;
		node->size_y = 0;
		node->size_z = 0;
		e_ns_init_head((ENodeHead *)node, V_NT_BITMAP, node_id, owner);
	}
	return node;
}

void callback_send_b_layer_create(void *user_data, VNodeID node_id, VLayerID layer_id, const char *name, VNBLayerType type)
{
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	uint i;
	node = e_create_b_node(node_id, 0);
	if((layer = e_dlut_get(&node->layertables, layer_id)) != NULL)
	{
		if(layer->type != type)
		{
			if(layer->data)
				free(layer->data);
			layer->data = NULL;
		}
	}else
	{
		layer = malloc(sizeof *layer);
		layer->data = NULL;
		e_dlut_add(&node->layertables, layer_id, layer);
	}
	for(i = 0; i < 15 && name[i] != 0; i++)
		layer->name[i] = name[i];
	layer->name[i] = name[i];
	layer->layer_id = layer_id;
	layer->type = type;
	layer->version++;
	global_version++;
	e_ns_update_node_version_struct(node);
}

void callback_send_b_layer_destroy(void *user_data, VNodeID node_id, VLayerID layer_id)
{
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	node = e_create_b_node(node_id, 0);
	if((layer = e_dlut_get(&node->layertables, layer_id)) != NULL)
	{
		e_dlut_remove(&node->layertables, layer_id);
		if(layer->data != NULL)
			free(layer->data);
		free(layer);
		global_version++;
	}
	e_ns_update_node_version_struct(node);
}

void callback_send_b_dimensions_set(void *user_data, VNodeID node_id, uint16 width, uint16 height, uint16 depth)
{
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	float			*data;
	uint i, j, k;
	node = e_create_b_node(node_id, 0);

	for(layer = e_dlut_next(&node->layertables, 0); layer != NULL; layer = e_dlut_next(&node->layertables, layer->layer_id + 1))
	{
		if(layer->data != NULL)
		{
			switch(layer->type)
			{
				case VN_B_LAYER_UINT1 :
				{
				}
				break;
				case VN_B_LAYER_UINT8 :
				{
					uint8 *buf;
					buf = malloc((sizeof *buf) * width * height * depth);
					for(i = 0 ; i < depth && i < node->size_z; i++)
					{
						for(j = 0 ; j < height && j < node->size_y; j++)
						{
							for(k = 0 ; k < height && k < node->size_x; k++)
								buf[i * height * width + j * width + k] = ((uint8*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
							for(; k < width; k++)
								buf[i * height * width + j * width + k] = 0;
						}
						for(j *= width; j < height * width; j++)
							buf[i * height * width + j] = 0;
					}
					for(i *= height * width; i < depth * height * width; i++)
						buf[i] = 0;
					free(layer->data);
					layer->data = buf;
				}
				break;
				case VN_B_LAYER_UINT16 :
				{
					uint16 *buf;
					buf = malloc((sizeof *buf) * width * height * depth);
					for(i = 0 ; i < depth && i < node->size_z; i++)
					{
						for(j = 0 ; j < height && j < node->size_y; j++)
						{
							for(k = 0 ; k < height && k < node->size_x; k++)
								buf[i * height * width + j * width + k] = ((uint16*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
							for(; k < width; k++)
								buf[i * height * width + j * width + k] = 0;
						}
						for(j *= width; j < height * width; j++)
							buf[i * height * width + j] = 0;
					}
					for(i *= height * width; i < depth * height * width; i++)
						buf[i] = 0;
					free(layer->data);
					layer->data = buf;
				}
				break;
				case VN_B_LAYER_REAL32 :
				{
					real32 *buf;
					buf = malloc((sizeof *buf) * width * height * depth);
					for(i = 0 ; i < depth && i < node->size_z; i++)
					{
						for(j = 0 ; j < height && j < node->size_y; j++)
						{
							for(k = 0 ; k < height && k < node->size_x; k++)
								buf[i * height * width + j * width + k] = ((float*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
							for(; k < width; k++)
								buf[i * height * width + j * width + k] = 0;
						}
						for(j *= width; j < height * width; j++)
							buf[i * height * width + j] = 0;
					}
					for(i *= height * width; i < depth * height * width; i++)
						buf[i] = 0;
					free(layer->data);
					layer->data = buf;
				}
				break;
				case VN_B_LAYER_REAL64 :
				{
					real64 *buf;
					buf = malloc((sizeof *buf) * width * height * depth);
					for(i = 0 ; i < depth && i < node->size_z; i++)
					{
						for(j = 0 ; j < height && j < node->size_y; j++)
						{
							for(k = 0 ; k < height && k < node->size_x; k++)
								buf[i * height * width + j * width + k] = ((double*)layer->data)[i * node->size_y * node->size_x + j * node->size_x + k];
							for(; k < width; k++)
								buf[i * height * width + j * width + k] = 0;
						}
						for(j *= width; j < height * width; j++)
							buf[i * height * width + j] = 0;
					}
					for(i *= height * width; i < depth * height * width; i++)
						buf[i] = 0;
					free(layer->data);
					layer->data = buf;
				}
				break;
			}
		}
	}
	node->size_x = width;
	node->size_y = height;
	node->size_z = depth;
	e_ns_update_node_version_struct(node);
}

ebreal read_pixel_int_eight_func(void *data, uint pixel)
{
	return (ebreal)((uint8 *)data)[pixel] / 255.0;
}
ebreal read_pixel_int_sixteen_func(void *data, uint pixel)
{
	return (ebreal)((uint16 *)data)[pixel] / (255.0 * 255.0 - 1);
}
ebreal read_pixel_float_func(void *data, uint pixel)
{
	return (ebreal)((float *)data)[pixel];
}
ebreal read_pixel_double_func(void *data, uint pixel)
{
	return (ebreal)((double *)data)[pixel];
}
ebreal read_pixel_empty_func(void *data, uint pixel)
{
	return 0.5;
}

void uppdate_bitmap_image_handle(EBMHandle *handle)
{
	uint i, j;
	ESBitmapNode	*node;
	ESBitmapLayer	*layer;
	node = (ESBitmapNode *) e_ns_get_node_networking(handle->node_id);
	handle->func[0] = read_pixel_empty_func;
	handle->func[1] = read_pixel_empty_func;
	handle->func[2] = read_pixel_empty_func;
	if(node == NULL || node->size_x == 0 || node->size_y == 0 || node->size_z == 0)
		return;
	handle->size_x = node->size_x;
	handle->size_y = node->size_y;
	handle->size_z = node->size_z;
	handle->global_version = global_version;
	for(layer = e_dlut_next(&node->layertables, 0); layer != NULL; layer = e_dlut_next(&node->layertables, layer->layer_id + 1))
	{
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 16 && handle->layers[i][j] != 0 && handle->layers[i][j] == layer->name[j]; j++);
			if(j < 16 && layer->name[j] == 0)
			{
				e_nsb_get_layer_data(node, layer);
				handle->data[i] = layer->data;
				if(layer->type == VN_B_LAYER_UINT1)
					handle->func[i] = read_pixel_empty_func;
				else if(layer->type == VN_B_LAYER_UINT8)
					handle->func[i] = read_pixel_int_eight_func;
				else if(layer->type == VN_B_LAYER_UINT16)
					handle->func[i] = read_pixel_int_sixteen_func;
				else if(layer->type == VN_B_LAYER_REAL32)
					handle->func[i] = read_pixel_float_func;
				else if(layer->type == VN_B_LAYER_REAL64)
					handle->func[i] = read_pixel_double_func;
			}
		}
	}

}

EBMHandle *e_nsb_get_image_handle(VNodeID node_id, const char *layer_r, const char *layer_g, const char *layer_b)
{
	EBMHandle *handle;
	handle = malloc(sizeof *handle);
	handle->global_version = 127538;
	handle->node_id = node_id;
	handle->size_x = 1;
	handle->size_y = 1;
	handle->size_z = 1;
	sprintf(handle->layers[0], "%s", layer_r);
	sprintf(handle->layers[1], "%s", layer_g);
	sprintf(handle->layers[2], "%s", layer_b);
	uppdate_bitmap_image_handle(handle);
	return handle;
}



void update_bitmap_image_handle_time(EBMHandle *handle, ESBitmapNode *node, uint channel, uint ofset_x, uint ofset_y, uint ofset_z, float *data)
{
	uint x, y;
	ESBitmapLayer	*layer;
	VNBTile		tile;

	for(layer = e_dlut_next(&node->layertables, 0); layer != NULL && strcmp(handle->layers[channel], layer->name); layer = e_dlut_next(&node->layertables, layer->layer_id + 1))
		;
	if(layer == NULL)
		return;
	ofset_x = ofset_z * node->size_x * node->size_y;
	if(layer->type == VN_B_LAYER_UINT1)
	{
	}
	else if(layer->type == VN_B_LAYER_UINT8)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vuint8[y * VN_B_TILE_SIZE + x] = (uint8)(data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel] * 255.0);
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_UINT8, &tile);
	}
	else if(layer->type == VN_B_LAYER_UINT16)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vuint16[y * VN_B_TILE_SIZE + x] = (uint16)(data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel] * ((256.0 * 256.0) - 1));
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_UINT16, &tile);
	}
	else if(layer->type == VN_B_LAYER_REAL32)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vreal32[y * VN_B_TILE_SIZE + x] = data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel];
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_REAL32, &tile);
	}
	else if(layer->type == VN_B_LAYER_REAL64)
	{
		for(y = 0; y < VN_B_TILE_SIZE; y++)
			for(x = 0; x < VN_B_TILE_SIZE; x++)
				tile.vreal64[y * VN_B_TILE_SIZE + x] = data[((y + ofset_y) * node->size_x + x + ofset_x) * 3 + channel];
		verse_send_b_tile_set(node->head.node_id, layer->layer_id, ofset_x / VN_B_TILE_SIZE, ofset_y / VN_B_TILE_SIZE, ofset_z, VN_B_LAYER_REAL64, &tile);
	}
}

void update_bitmap_image_handle_data(EBMHandle *handle, uint ofset_x, uint ofset_y, uint size_x, uint size_y, float *data)
{
	uint i, x, y, z;
	ESBitmapNode	*node;
	node = (ESBitmapNode *) e_ns_get_node_networking(handle->node_id);
	if(node == NULL)
		return;
	for(z = 0; z < node->size_x; z++)
		for(y = 0; y < node->size_y; y += VN_B_TILE_SIZE)
			for(x = 0; x < node->size_x; x += VN_B_TILE_SIZE)
				for(i = 0; i < 3; i++)
					update_bitmap_image_handle_time(handle, node, i, x, y, z, data);
}


void e_nsb_destroy_image_handle(EBMHandle *handle)
{
	free(handle);
}

void e_nsb_evaluate_image_handle_clamp(EBMHandle *handle, ebreal *pixel, ebreal x, ebreal y, ebreal z)
{
	uint i;
	if(handle->global_version != global_version)
		uppdate_bitmap_image_handle(handle);
	if(x > 1);
		x = 1;
	if(x < 0);
		x = 0;
	if(y > 1);
		y = 1;
	if(y < 0);
		y = 0;
	if(z > 1);
		z = 1;
	if(z < 0);
		z = 0;
	i = (uint)(x * (ebreal)handle->size_x) + (uint)(y * handle->size_y) * handle->size_x + (uint)(z * handle->size_z) * handle->size_x * handle->size_y;
	pixel[0] = handle->func[0](handle->data[0], i);
	pixel[1] = handle->func[1](handle->data[1], i);
	pixel[2] = handle->func[2](handle->data[2], i);
}

void e_nsb_evaluate_image_handle_tile(EBMHandle *handle, ebreal *output, ebreal x, ebreal y, ebreal z)
{
	uint i;
	if(handle->global_version != global_version)
		uppdate_bitmap_image_handle(handle);
	x += 100;
	y += 100;
	z += 100;
	x -= (int)x;
	y -= (int)y;
	z -= (int)z;
	i = (uint)(x * (float)handle->size_x) + (uint)(y * handle->size_y) * handle->size_x + (uint)(z * handle->size_z) * handle->size_x * handle->size_y;
	output[0] = handle->func[0](handle->data[0], i);
	output[1] = handle->func[1](handle->data[1], i);
	output[2] = handle->func[2](handle->data[2], i);
}

void image_scale(float *in, float *out, uint in_chanels, uint out_chanels, uint in_size_x, uint in_size_y, uint out_size_x, uint out_size_y)
{
	uint i, j, k, xi, yi, pixel, chanels;
	float xf, yf, f;
	chanels = in_chanels;
	if(in_chanels > out_chanels)
		chanels = out_chanels;
	for(i = 0; i < out_size_y; i++)
	{
		for(j = 0; j < out_size_x; j++)
		{
			xf = (float)in_size_x * (float)j / (float)out_size_x;
			yf = (float)in_size_y * (float)i / (float)out_size_y;
			xi = (uint)xf; 
			yi = (uint)yf;
			xf = f_smooth_stepf(xf - (float)xi);
			yf = f_smooth_stepf(yf - (float)yi);
			for(k = 0; k < chanels; k++)
			{
				f = (in[(yi * in_size_x + xi) * in_chanels + k] * (1.0 - xf) + in[(yi * in_size_x + (xi + 1)) * in_chanels + k] * xf) * (1.0 - yf);
				f += (in[((yi + 1) * in_size_x + xi) * in_chanels + k] * (1.0 - xf) + in[((yi + 1) * in_size_x + (xi + 1)) * in_chanels + k] * xf) * yf;
				out[(i * out_size_x + j) * out_chanels + k] = f;
			}
		}
	}
}

void e_nsb_fill_buffer_uint8(ESBitmapNode *node, ESBitmapLayer *layer, uint8 *buffer, uint stride)
{
	uint i, j, k, start, next, size;
	size = node->size_x * node->size_y * node->size_z;
	switch(layer->type)
	{
		case VN_B_LAYER_UINT1 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = ((array[i] >> (i % 8)) & 1) * 255;
		}
		break;
		case VN_B_LAYER_UINT8 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = array[i];
		}
		break;
		case VN_B_LAYER_UINT16 :
		{
			uint16 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = array[i] / 256;
		}
		break;
		case VN_B_LAYER_REAL32 :
		{
			float *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (uint8)(array[i] * 255.0);
		}
		case VN_B_LAYER_REAL64 :
		{
			double *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (uint8)(array[i] * 255.0);
		}
		break;
	}
}



void e_nsb_fill_buffer_uint16(ESBitmapNode *node, ESBitmapLayer *layer, uint16 *buffer, uint stride)
{
	uint i, j, k, start, next, size;
	size = node->size_x * node->size_y * node->size_z;
	switch(layer->type)
	{
		case VN_B_LAYER_UINT1 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = ((array[i] >> (i % 8)) & 1) * (256 * 256 - 1);
		}
		break;
		case VN_B_LAYER_UINT8 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = array[i] * 256;
		}
		break;
		case VN_B_LAYER_UINT16 :
		{
			uint16 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = array[i];
		}
		break;
		case VN_B_LAYER_REAL32 :
		{
			float *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (uint8)(array[i] * (256.0 * 256.0 - 1.0));
		}
		case VN_B_LAYER_REAL64 :
		{
			double *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (uint8)(array[i] * (256.0 * 256.0 - 1.0));
		}
		break;
	}
}


void e_nsb_fill_buffer_float(ESBitmapNode *node, ESBitmapLayer *layer, float *buffer, uint stride)
{
	uint i, j, k, start, next, size;
	size = node->size_x * node->size_y * node->size_z;
	switch(layer->type)
	{
		case VN_B_LAYER_UINT1 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (float)((array[i] >> (i % 8)) & 1);
		}
		break;
		case VN_B_LAYER_UINT8 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (float)array[i] / 255.0f;
		}
		break;
		case VN_B_LAYER_UINT16 :
		{
			uint16 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (float)array[i] / (256.0 * 256.0 - 1.0);
		}
		break;
		case VN_B_LAYER_REAL32 :
		{
			float *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = array[i];
		}
		case VN_B_LAYER_REAL64 :
		{
			double *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (float)array[i];
		}
		break;
	}
}

void e_nsb_fill_buffer_double(ESBitmapNode *node, ESBitmapLayer *layer, double *buffer, uint stride)
{
	uint i, j, k, start, next, size;
	size = node->size_x * node->size_y * node->size_z;
	switch(layer->type)
	{
		case VN_B_LAYER_UINT1 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (double)((array[i] >> (i % 8)) & 1);
		}
		break;
		case VN_B_LAYER_UINT8 :
		{
			uint8 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (double)array[i] / 255.0f;
		}
		break;
		case VN_B_LAYER_UINT16 :
		{
			uint16 *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (double)array[i] / (256.0 * 256.0 - 1.0);
		}
		break;
		case VN_B_LAYER_REAL32 :
		{
			float *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = (double)array[i];
		}
		case VN_B_LAYER_REAL64 :
		{
			double *array;
			array = layer->data;
			for(i = 0; i < size; i++)
				buffer[i * stride] = array[i];
		}
		break;
	}
}



/*
#define VN_B_TILE_SIZE 8

typedef union{
	uint8 vuint1[8];
	uint8 vuint8[64];
	uint16 vuint16[64];
	real32 vreal32[64];
	real64 vreal64[64];
} VNBTile;*/

void e_nsb_image_set_float(ESBitmapNode *node, ESBitmapLayer *layer, float *buffer, uint stride)
{
	VNBTile tile;
	float f;
	uint i, j, k, l, m, start, next, x_size, y_size, z_size, size;
	x_size = node->size_x;
	y_size = node->size_y;
	z_size = node->size_z;
	size = x_size * y_size * z_size;

	switch(layer->type)
	{
		case VN_B_LAYER_UINT1 :
		{
		}
		break;
		case VN_B_LAYER_UINT8 :
		{
			for(i = 0; i < z_size; i++)
			{
				for(j = 0; j < y_size; j += VN_B_TILE_SIZE)
				{
					for(k = 0; k < x_size; k += VN_B_TILE_SIZE)
					{
						start = i * x_size * y_size + j * x_size + k;
						for(l = 0; l < VN_B_TILE_SIZE; l++)
						{
							for(m = 0; m < VN_B_TILE_SIZE; m++)
							{
								f =	buffer[((start + l * x_size + m) % size) * stride];
								if(f < 0.0)
									tile.vuint8[l * VN_B_TILE_SIZE + m] = 0;
								else if(f > 1.0)
									tile.vuint8[l * VN_B_TILE_SIZE + m] = 255;
								else
									tile.vuint8[l * VN_B_TILE_SIZE + m] = (uint8)(f * 255.0);
							}
						}
						verse_send_b_tile_set(e_ns_get_node_id(node), layer->layer_id, k / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, i, layer->type, &tile);
					}
				}
			}
		}
		break;
		case VN_B_LAYER_UINT16 :
		{
			for(i = 0; i < z_size; i++)
			{
				for(j = 0; j < y_size; j += VN_B_TILE_SIZE)
				{
					for(k = 0; k < x_size; k += VN_B_TILE_SIZE)
					{
						start = i * x_size * y_size + j * x_size + k;
						for(l = 0; l < VN_B_TILE_SIZE; l++)
						{
							for(m = 0; m < VN_B_TILE_SIZE; m++)
							{
								f =	buffer[((start + l * x_size + m) % size) * stride];
								if(f < 0.0)
									tile.vuint8[l * VN_B_TILE_SIZE + m] = 0;
								else if(f > 1.0)
									tile.vuint8[l * VN_B_TILE_SIZE + m] = 256 * 256 - 1;
								else
									tile.vuint8[l * VN_B_TILE_SIZE + m] = (uint8)(f * (256.0 * 256.0 - 1.0));
							}
						}
						verse_send_b_tile_set(e_ns_get_node_id(node), layer->layer_id, k / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, i, layer->type, &tile);
					}
				}
			}
		}
		break;
		case VN_B_LAYER_REAL32 :
		{
			for(i = 0; i < z_size; i++)
			{
				for(j = 0; j < y_size; j += VN_B_TILE_SIZE)
				{
					for(k = 0; k < x_size; k += VN_B_TILE_SIZE)
					{
						start = i * x_size * y_size + j * x_size + k;
						for(l = 0; l < VN_B_TILE_SIZE; l++)
							for(m = 0; m < VN_B_TILE_SIZE; m++)
								tile.vreal32[l * VN_B_TILE_SIZE + m] = buffer[((start + l * x_size + m) % size) * stride];
						verse_send_b_tile_set(e_ns_get_node_id(node), layer->layer_id, k / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, i, layer->type, &tile);
					}
				}
			}
		}
		break;
		case VN_B_LAYER_REAL64 :
		{
			for(i = 0; i < z_size; i++)
			{
				for(j = 0; j < y_size; j += VN_B_TILE_SIZE)
				{
					for(k = 0; k < x_size; k += VN_B_TILE_SIZE)
					{
						start = i * x_size * y_size + j * x_size + k;
						for(l = 0; l < VN_B_TILE_SIZE; l++)
							for(m = 0; m < VN_B_TILE_SIZE; m++)
								tile.vreal64[l * VN_B_TILE_SIZE + m] = buffer[((start + l * x_size + m) % size) * stride];
						verse_send_b_tile_set(e_ns_get_node_id(node), layer->layer_id, k / VN_B_TILE_SIZE, j / VN_B_TILE_SIZE, i, layer->type, &tile);
					}
				}
			}
		}
		break;
	}
}


void callback_send_b_tile_set(void *user_data, VNodeID node_id, VLayerID layer_id, uint16 tile_x, uint16 tile_y, uint16 z, VNBLayerType type, VNBTile *tile)
{
	ESBitmapNode		*node;
	ESBitmapLayer		*layer;
	float			*buf;
	uint32			i, x, y, x_offset, y_offset, z_offset, pixel_id, x_tilesize, y_tilesize, tw, th;

	node = e_create_b_node(node_id, 0);
	layer = e_dlut_get(&node->layertables, layer_id);
	if(layer == NULL)
		return;

	if(layer->type != type)
		return;
	x_offset = tile_x * VN_B_TILE_SIZE;
	y_offset = tile_y * VN_B_TILE_SIZE;
	z_offset = node->size_x * node->size_y * z;
	x_tilesize = (node->size_x + VN_B_TILE_SIZE - 1) / VN_B_TILE_SIZE;
	y_tilesize = (node->size_y + VN_B_TILE_SIZE - 1) / VN_B_TILE_SIZE;
	tw = (tile_x == x_tilesize - 1) && (node->size_x % VN_B_TILE_SIZE) != 0 ? node->size_x % VN_B_TILE_SIZE : VN_B_TILE_SIZE;
	th = (tile_y == y_tilesize - 1) && (node->size_y % VN_B_TILE_SIZE) != 0 ? node->size_y % VN_B_TILE_SIZE : VN_B_TILE_SIZE;
	switch(type)
	{
		case VN_B_LAYER_UINT1 :
		{
			uint8	*array = layer->data;
			uint32	lw = (node->size_x + 7) / 8;

			for(y = 0; y < th; y++)
				array[z * lw * node->size_y + (y_offset + y) * lw + tile_x] = tile->vuint1[y];
		}
		break;
		case VN_B_LAYER_UINT8 :
		{
			uint8	*array = layer->data;
			for(y = 0; y < th; y++)
			{
				for(x = 0; x < tw; x++)
				{
					pixel_id = (y_offset + y) * node->size_x + x_offset + x;
					array[pixel_id + z_offset] = tile->vuint8[y * VN_B_TILE_SIZE + x];
				}
			}
		}
		break;
		case VN_B_LAYER_UINT16 :
		{
			uint16	*array = layer->data;
			for(y = 0; y < th; y++)
			{
				for(x = 0; x < tw; x++)
				{
					pixel_id = (y_offset + y) * node->size_x + x_offset + x;
					array[pixel_id + z_offset] = tile->vuint16[y * VN_B_TILE_SIZE + x];
				}
			}
		}
		break;
		case VN_B_LAYER_REAL32 :
		{
			real32	*array = layer->data;
			for(y = 0; y < th; y++)
			{
				for(x = 0; x < tw; x++)
				{
					pixel_id = (y_offset + y) * node->size_x + x_offset + x;
					array[pixel_id + z_offset] = tile->vreal32[y * VN_B_TILE_SIZE + x];
				}
			}
		}
		break;
		case VN_B_LAYER_REAL64 :
		{
			real64	*array = layer->data;
			for(y = 0; y < th; y++)
			{
				for(x = 0; x < tw; x++)
				{
					pixel_id = (y_offset + y) * node->size_x + x_offset + x;
					array[pixel_id + z_offset] = tile->vreal64[y * VN_B_TILE_SIZE + x];
				}
			}
		}
		break;
	}
	layer->version++;
	e_ns_update_node_version_data(node);
}


void es_bitmap_init(void)
{
	verse_callback_set(verse_send_b_layer_create,		callback_send_b_layer_create,		NULL);
	verse_callback_set(verse_send_b_layer_destroy,		callback_send_b_layer_destroy,		NULL);
	verse_callback_set(verse_send_b_dimensions_set,		callback_send_b_dimensions_set,		NULL);
	verse_callback_set(verse_send_b_tile_set,			callback_send_b_tile_set,			NULL);
}

static void delete_bitmap_layer_func(uint id, void *pointer, void *user_data)
{
	ESBitmapLayer	*layer = pointer;

	if(layer->data != NULL)
		free(layer->data);
	free(layer);
}

void delete_bitmap(ESBitmapNode	*node)
{
	e_dlut_remove_foreach(&node->layertables, delete_bitmap_layer_func, NULL);
	free(node);
}
