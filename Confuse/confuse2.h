#include "c_types.h"

/* session management */

typedef void CSession;
typedef void CInstance;
typedef void CType;
typedef void CDrawState;


CSession					*confuse_session_create(); /* create an empty session*/
boolean						confuse_session_save(CSession *session, void *handle); /* load a session from a unravel handle*/
boolean						confuse_session_load(CSession *session, void *handle);
void						confuse_session_free(CSession *session); /* destroy a session */


void						confuse_execute_threads_launch(CSession *session, uint thread_count);
void						confuse_execute_frame_advance(CSession *session, float delta_time);

uint						confuse_entity_count(CSession *session); /* how many entity prototypes exist in the session */
char						*confuse_entity_name_get(CSession *session, uint entity_id); /* get the name of a specific entity prototypes */
uint						confuse_entity_name_lookup(CSession *session, char *name, boolean ignore_empty); /* lookup a specific entity prototyp,es using its name */

uint						confuse_entity_type_count(CSession *session, uint entity_id); /* get the number of members of the specific entity prototypes stack */
CType						*confuse_entity_type_get(CSession *session, uint entity_id, uint type_id); /* get the stack members by id */
CType						*confuse_entity_type_lookup(CSession *session, uint entity_id, char *name); /* get the stack members by name */
CTypeType					confuse_entity_type_get_type(CSession *session, CType *type); /* get the stack data type of a member */
CType						*confuse_entity_type_specific_lookup(CSession *session, uint entity_id, char *name, CTypeType type); /* get the stack members by name and type */

void						confuse_entity_template_create(CSession *session, char *name, uint param_count, CTypeType *param_type, char **param_name);

CInstance					*confuse_entity_instance_create(CSession *session, uint entity_id); /* create an instance of a entity */
void						confuse_entity_instance_destroy(CSession *session, CInstance *instance); /* destroy an instance of a entity */
CInstance					*confuse_entity_instance_global_get(CSession *session); 

void						confuse_entity_instance_compute(CSession *session, CInstance *instance);
void						confuse_entity_instance_compute_fire_and_forget(CSession *session, CInstance *instance);

void						confuse_entity_instance_set_depth_test(CSession *session, CInstance *instance, CType *type, uint id, boolean enable);
void						confuse_entity_instance_set_boolean(CSession *session, CInstance *instance, CType *type, uint id, boolean value);
void						confuse_entity_instance_set_float(CSession *session, CInstance *instance, CType *type, uint id, float value);
void						confuse_entity_instance_set_uv(CSession *session, CInstance *instance, CType *type, uint id, float u, float v);
void						confuse_entity_instance_set_pos(CSession *session, CInstance *instance, CType *type, uint id, float x, float y, float z);
void						confuse_entity_instance_set_color(CSession *session, CInstance *instance, CType *type, uint id, float red, float green, float blue, float alpha);
void						confuse_entity_instance_set_matrix(CSession *session, CInstance *instance, CType *type, uint id, float *matrix);
void						confuse_entity_instance_set_area(CSession *session, CInstance *instance, CType *type, uint id, float x, float y);
void						confuse_entity_instance_set_quaternion(CSession *session, CInstance *instance, CType *type, uint id, float *pos, float quaternion, float *size);
void						confuse_entity_instance_set_uv_section(CSession *session, CInstance *instance, CType *type, uint id, float *uv0, float *uv1, float *uv2, float *uv3);
void						confuse_entity_instance_set_image_id(CSession *session, CInstance *instance, CType *type, uint id, uint32 image_id);
void						confuse_entity_instance_set_object_id(CSession *session, CInstance *instance, CType *type, uint id, uint32 object_id);

void						confuse_entity_instance_set_blend_source(CSession *session, CInstance *instance, uint source_factor);
void						confuse_entity_instance_set_blend_dest(CSession *session, CInstance *instance, uint dest_factor);


extern void					confuce_asset_load_image_2d_func(uint (*func)(char *name, uint8 *data, uint x, uint y, void *user), void *user);
extern void					confuce_asset_load_image_3d_func(uint (*func)(char *name, uint8 *data, uint x, uint y, uint z, void *user), void *user);
extern void					confuce_asset_load_image_cube_func(uint (*func)(char *name, uint8 *data, uint x, uint y, void *user), void *user);

extern void					confuce_asset_load_shader_func(void *(*func)(char *name, char *vertex, char *fragment, void *user), void *user);
extern void					confuce_asset_load_shader_localtion_func(uint (*func)(void *shader, char *name, void *user), void *user);

extern void					confuce_asset_load_array_func(void *(*func)(char *name, uint8 *data, uint *format_types, uint format_count, uint vertex_count, void *user), void *user);

/* draw API */

//State, calls.....

extern void					confuce_draw_begin(CSession *session, float delta_time);
extern void					confuce_draw_end(CSession *session);

extern CDrawState			*confuce_draw_state_get_next(CSession *session, CDrawState *previous);
extern void					*confuce_draw_state_shader_get(CDrawState *state);
extern void					*confuce_draw_state_array_get(CDrawState *state);
extern void					*confuce_draw_state_array_load(CDrawState *state, uint *size); // returns NULL if no array load is needed.

extern uint					confuce_draw_state_call_count(CDrawState *state);
extern uint					confuce_draw_call_uniform_count(CDrawState *state, uint call_id);
extern void					*confuce_draw_call_uniform_data(CDrawState *state, uint call_id, uint uniform_id, CTypeType *type_output, uint *location);
extern void					confuse_draw_call_blend_func(CDrawState *state, uint call_id, uint *source, uint *destination);
extern boolean				confuse_draw_call_depth_test(CDrawState *state, uint call_id);
extern float				*confuse_draw_call_matrix(CDrawState *state, uint call_id);
extern void					confuse_draw_call_draw_range(CDrawState *state, uint call_id, uint *start, uint *end);
extern void					confuse_view_matrix_set(float *matrix);

/*
extern uint					confuse_primitive_count();
extern char					*confuse_primitive_name(uint id);
extern CDrawStateType	confuse_primitive_type(uint id);
extern uint					confuse_primitive_param_count(uint id);
extern CTypeType			confuse_primitive_param_type(uint id, uint param);
extern char					*confuse_primitive_param_name(uint id, uint param);
extern void					confuse_primitive_blend_func(uint id, uint *source, uint *dist);
extern boolean				confuse_primitive_depth_test(uint id);
extern uint					confuse_primitive_buffer_allocated(uint id);
extern uint					confuse_primitive_buffer_count(uint id);
extern void					*confuse_primitive_buffer_get(uint id);
extern void					confuse_primitive_buffer_clear(uint id);
extern char					*confuse_primitive_shader_get(uint id, uint source);
*/

/* Functions only available when using confuse_relinquish_render.c for rendering */

extern void confuse_relinquish_init();
extern void confuse_relinquish_draw(CSession *session, float delta_time);

