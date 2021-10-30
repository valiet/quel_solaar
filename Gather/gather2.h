
//#define GATHER_VERSE_CONNECTION
#define GATHER_TARGA_SIMPLE
#define GATHER_WAVEFRONT_OBJ
#define GATHER_WAV_AUDIO
#define GATHER_OPENGL

#define GATHER_DELETED_VERTEX         3.402823466e+38f

typedef enum{
	G_FORMAT_TARGA_SIMPLE,
	G_FORMAT_WAVEFRONT_OBJ,
	G_FORMAT_WAV,
	G_FORMAT_ANY
}GFormats;

typedef enum{
	G_NT_GEOMETRY,
	G_NT_IMAGE,
	G_NT_OBJECT,
	G_NT_AUDIO
}GNodeType;

typedef enum {
	G_NGLT_LAYER_VERTEX_XYZ = 0,
	G_NGLT_LAYER_VERTEX_UINT32,
	G_NGLT_LAYER_VERTEX_REAL,
	G_NGLT_LAYER_POLYGON_CORNER_UINT32 = 128,
	G_NGLT_LAYER_POLYGON_CORNER_REAL,
	G_NGLT_LAYER_POLYGON_FACE_UINT8,
	G_NGLT_LAYER_POLYGON_FACE_UINT32,
	G_NGLT_LAYER_POLYGON_FACE_REAL
}GNodeGeometryLayerType;

#ifndef GATHER_INTERNAL

typedef void GatherData;
typedef void GatherNode;

extern GatherData	*gather_load(char *file_name, uint format);
extern boolean		gather_save(GatherData *data, char *file_name, uint format);
extern GatherData	*gather_create();
extern void			gather_free(GatherData *data);

/* readout content of data */

extern uint			gather_node_load_count(GatherData *data, GNodeType type);
extern GatherNode	*gather_node_load_enumerate(GatherData *data, uint nr, GNodeType type);
extern GatherNode	*gather_node_create(GatherData *data, GNodeType type);
extern void			gather_node_save_name(GatherNode *node, char *name);
extern char			*gather_node_load_name(GatherNode *node);

/* readout the content of a geometry node */

extern uint			gather_node_load_geometry_layer_count(GatherNode *node);
extern uint			gather_node_load_geometry_layer_lookup(GatherNode *node, char *name);
extern uint			gather_node_load_geometry_layer_type(GatherNode *node, uint layer_id);
extern void			*gather_node_load_geometry_layer_name(GatherNode *node, uint layer_id);
extern void			*gather_node_load_geometry_layer_data(GatherNode *node, uint layer_id);

extern uint			gather_node_load_geometry_poly_count(GatherNode *node);
extern uint			gather_node_load_geometry_vertex_count(GatherNode *node);

extern uint			gather_node_save_geometry_size(GatherNode *node, uint vertex_count, uint polygon_count);
extern void			gather_node_save_geometry_layer_add(GatherNode *node, void *layer_data, uint type, char *name);

/* readout the content of a image node */

extern uint			gather_node_opengl_texture(GatherNode *node);
extern uint			gather_file_opengl_texture(char *file_name, uint format);

extern uint			gather_node_load_image_layer_count(GatherNode *node);
extern uint			gather_node_load_image_layer_lookup(GatherNode *node, char *name);
extern boolean		gather_node_load_image_layer_hdri(GatherNode *node, uint layer_id);
extern char			*gather_node_load_image_layer_name(GatherNode *node, uint layer_id);
extern void			*gather_node_load_image_layer_data(GatherNode *node, uint layer_id);
extern boolean		gather_node_load_image_hdri(GatherNode *node);
extern float		*gather_node_load_image_rgb_float(GatherNode *node);
extern float		*gather_node_load_image_rgba_float(GatherNode *node);
extern uint8		*gather_node_load_image_rgb_uint8(GatherNode *node);
extern uint8		*gather_node_load_image_rgba_uint8(GatherNode *node);

extern uint			gather_node_load_image_size_x(GatherNode *node);
extern uint			gather_node_load_image_size_y(GatherNode *node);
extern uint			gather_node_load_image_size_z(GatherNode *node);

extern uint			gather_node_save_image_size(GatherNode *node, uint size_x, uint size_y, uint size_z);
extern void			gather_node_save_image_layer_float_add(GatherNode *node, char *name, float *data);
extern void			gather_node_save_image_layer_uint8_add(GatherNode *node, char *name, uint8 *data);
extern void			gather_node_save_image_rgb_float_add(GatherNode *node, float *data);
extern void			gather_node_save_image_rgb_uint8_add(GatherNode *node, uint8 *data);
extern void			gather_node_save_image_rgba_float_add(GatherNode *node, float *data);
extern void			gather_node_save_image_rgba_uint8_add(GatherNode *node, uint8 *data);

/* readout the content of a audio node */

extern uint			gather_node_load_audio_channel_count(GatherNode *node);
extern float		*gather_node_load_audio_channel_float_get(GatherNode *node, uint channel_id);
extern uint			gather_node_load_audio_rate(GatherNode *node);
extern uint			gather_node_load_audio_size(GatherNode *node);

extern void			gather_node_save_audio_channel_int16_set(GatherNode *node, int16 *channel, uint stride, uint size, float vec_x, float vec_y, float vec_z);
extern void			gather_node_save_audio_channel_float_set(GatherNode *node, float *channel, uint stride, uint size, float vec_x, float vec_y, float vec_z);
extern void			gather_node_save_audio_rate_set(GatherNode *node, uint rate);

#ifdef GATHER_VERSE_CONNECTION

extern void			gather_node_load_verse_geometry(GatherData *data, uint32 node_id);
extern boolean		gather_node_save_verse_geometry(GatherData *data);

#endif
#endif