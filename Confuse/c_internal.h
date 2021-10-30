
#define CONFUSE_INTERNAL

#include "c_types.h"

typedef enum{
	C_CC_INITIALIZE, // runs once per instance
	C_CC_PROCESS, // computation only
	C_CC_TIMER, // Dependent on time
	C_CC_OBJECT, // computation that can be created without existing  draw. (lights, sounds...)
	C_CC_DRAW, // object to draw
	C_CC_PRIMITIVE_STATE, // global state for drawcalls.
	C_CC_FORCE, // Physical force
	C_CC_COUNT
}CComponentCategory;

//#edfine 

typedef struct{
	char name[32];
	uint texture_id;
	uint8 *buffer;
	uint size[3];
	CTypeType type;
}CTexture;

typedef struct{
	void *render_array;
	uint32 node_id;
	char name[32];
	uint version;
}CPreviewGeometry;

typedef enum{
	C_BUL_DRAW,
	C_BUL_DEPTH_TEST,
	C_BUL_BLEND_SOURCE,
	C_BUL_BLEND_DEST,
	C_BUL_OBJECT,
	C_BUL_TRANSFORMATION_MATRIX,
	C_BUL_COUNT
}CBaseUniformsLocations;

typedef struct{
	char name[32];
	CTypeType type;
	uint id;
	uint offset;
}CDrawStateInput;

typedef struct{
	CDrawStateType type;
	uint attrib_param_count;
	CDrawStateInput *attrib_params;
	uint uniform_param_count;
	CDrawStateInput *uniform_params;
	uint blend_destination;
	uint blend_source;
	boolean depth_test;
	void *attrib_buffer;
	void *uniform_buffer;
	uint attrib_stride;
	uint uniform_stride;
	uint buffer_size;
	uint attrib_used;
	uint uniform_used;
	float *sort_distance;
	float *sort_buffer;
	char name[16];
	char *shaders[2];
	char *shader_paths[2];
	void *state_shader;
	void *state_array;
	boolean injected;
	uint geometry_checksum;
}CDrawState; /* a primitive */
/*
typedef struct{
	CDrawState *prim;
	uint primitive_count;
}CDrawStateStorage;

extern CDrawStateStorage *c_primitive_storage;
*/
typedef struct{
	float key_time;
	float key_ease[2];
}CSpecialKey;

typedef struct{
	CSpecialKey *keys;
	uint key_count;
	uint key_allocated;
}CSpecialAnimation;

typedef struct{
	CTypeType type;
	char name[32];
	uint pos;
	uint size;
	uint dependency;
	uint selected;
	uint number;
}CType;

typedef enum{
	C_SCT_ANIMATION,
	C_SCT_COUNT
}CSpecialComponentType;

typedef struct{
	uint type;
	uint *references;
	void *special;
}CComponent;

typedef struct{
	char name[32];
	CComponent *components;
	uint component_count;
	uint component_selected;
	float *stack;
	uint stack_size;
	uint stack_alloc;
	CType *stack_types;
	uint stack_type_count;
	uint stack_type_alloc;
	float time;
}CEntity;


/*
CComponentDef *c_component_list = NULL;
uint c_component_list_count = 0;
*/
typedef enum{
	C_CS_READY,
	C_CS_COMPUTED,
	C_CS_LOCKED
}CComputeState;

typedef enum{
	C_CU_UNCHANGED,
	C_CU_UPDATED,
	C_CU_COMPUTED
}CComputeUpdate;

typedef struct{
	void *mutex;
	CEntity *entity;
	void *stack;
	float time;
	uint seed;
	boolean ended;
	boolean fire_and_forget;
	CComputeUpdate *updates;
	CComputeState compute;
}CInstance;

typedef struct{
	char name[32];
	uint type;
	CTypeType *param_types;
	char **param_names;
	uint param_count;
	CTypeType output;
	uint primitive;
	void (*exe_func)(void *session, CInstance *instance, CComponent *c, uint primitive);
	void (*draw_func)(void *session, CInstance *instance, CComponent *c);
	uint special;
}CComponentDef;

typedef struct{
	CEntity *entity_defines;
	uint entity_define_count;

	CInstance **instances;
	uint instance_count;
	uint instance_allocated;

	CEntity primitive_settings;
	CDrawState *prim;
	uint primitive_count;

	CComponentDef *component_list;
	uint component_list_count;

	uint thread_count;
	void *master_mutex;
	void *gate_mutex;
	uint gate_counter;
	boolean delete_me;
	float delta_time;
}CSession;

extern CEntity *confuse_session_entity_add(CSession *session, char *name);
extern void		confuse_session_entity_delete(CSession *session, uint entity_id);
extern CEntity *confuse_session_entity_clone(CSession *session, CEntity *entity);

extern uint		confuse_component_add(CSession *session, char *name, CTypeType *param_types, char **param_names, uint param_count, CTypeType output, uint primitive, void (*func)(CSession *session, CInstance *instance, CComponent *c, uint primitive), CComponentCategory type, CSpecialComponentType special);
extern void 	confuse_component_remove(CSession *session, uint id);




extern uint		confuse_component_count_get(CSession *session);
extern char		*confuse_component_name_get(CSession *session, uint component);
extern CComponentCategory confuse_component_category_get(CSession *session, uint component);
extern CTypeType confuse_component_output_get(CSession *session, uint component);
extern uint		confuse_component_output_pos_get(CSession *session, CComponent *component);
extern uint		confuse_component_primitive_get(CSession *session, uint component);
extern uint		confuse_component_param_count_init_get(CSession *session, uint component);
extern uint		confuse_component_param_count_extend_get(CSession *session, CComponent *component);
extern CTypeType confuse_component_param_type_get(CSession *session, uint component, uint id);
extern char		*confuse_component_param_name_get(CSession *session, uint component, uint id);


extern CSpecialComponentType confuse_component_special_get(CSession *session, uint component);
extern boolean confuse_component_output_available(CSession *session, uint component, CTypeType type);

extern uint c_type_size_get(CTypeType type);

extern void c_entity_stack_clear(float *stack, CTypeType type, uint size, uint pos);
extern uint c_entity_stack_add(CEntity *e, CTypeType type, char *name, uint size);
extern boolean c_entity_stack_remove(CSession *session, CEntity *entity, uint stack_id);
extern void c_entity_stack_resize(CEntity *entity, uint id, uint new_size);
extern uint c_entity_stack_get(CEntity *e, uint id, CTypeType type, uint *stride, uint *size);
extern uint c_entity_stack_user_count(CSession *session, CEntity *entity, uint stack_id, boolean output);
extern boolean c_entity_types_compatible(CTypeType read, CTypeType write);
extern void c_entity_components_add(CSession *session, CEntity *e, uint type, uint output);
extern void		c_entity_components_remove(CSession *session, CEntity *e, uint component_id, boolean stack);

extern void confuse_execute(CSession *session, CEntity *entity, float delta_time, boolean reset_time);

extern CDrawState *c_primitive_add();
extern boolean c_primitive_compile(CDrawState *p);
extern boolean c_primitive_inject_attribute(CSession *session, CDrawState *p);
extern uint c_primitive_inject_uniform(CSession *session, CDrawState *p, CComponentCategory type, boolean add_component);
extern void c_update_primitive_array(CDrawState *state, uint8 *data, uint vertex_count);
extern void c_primitive_load(CSession *session, char *vertex_shader, char *fragment_shader, boolean attribute, boolean uniform, char *name);
extern void c_primitive_reload(CSession *session, CDrawState *primitive);
