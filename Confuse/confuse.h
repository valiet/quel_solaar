typedef enum{
	C_CM_BASE,
	C_CM_LOOK,
	C_CM_BUTT,
	C_CM_BUTT2,
	C_CM_BUTT3,
	C_CM_BELLY,
	C_CM_TORSO,
	C_CM_HEAD,
	C_CM_ARM_LA_UP,
	C_CM_ARM_LA_DOWN,	
	C_CM_ARM_RA_UP,
	C_CM_ARM_RA_DOWN,	
	C_CM_ARM_LB_UP,
	C_CM_ARM_LB_DOWN,	
	C_CM_ARM_RB_UP,
	C_CM_ARM_RB_DOWN,
	C_CM_LEG_LA_UP,
	C_CM_LEG_LA_DOWN,
	C_CM_LEG_RA_UP,
	C_CM_LEG_RA_DOWN,
	C_CM_LEG_LB_UP,
	C_CM_LEG_LB_DOWN,
	C_CM_LEG_RB_UP,
	C_CM_LEG_RB_DOWN,
	C_CM_LEG_LC_UP,
	C_CM_LEG_LC_DOWN,
	C_CM_LEG_RC_UP,
	C_CM_LEG_RC_DOWN,
	C_CM_TOOL,
	C_CM_WORLD_MATRIX,
	C_CM_COUNT	
}CCharacterMatrix;

#define C_MAX_LEG_COUNT 6
#define C_MAX_ARM_COUNT 2
#define C_MAX_INVENTORY_COUNT 6

typedef enum{
	C_IT_RIFLE,
	C_IT_HANDGUN,
	C_IT_BIRD,
	C_IT_GEAR,
	C_IT_STABB,
	C_IT_UNDER_THROW,
	C_IT_OVER_THROW,
	C_IT_NO_EQUIPNENT_START,
	C_IT_IDLE,
	C_IT_BUILD,
	C_IT_ARM_GUN,
	C_IT_RIGHT_HIT,
	C_IT_LEFT_HIT,
	C_IT_COUNT
}CInventoryType;

typedef struct{
	float timer;
	float speed;
	float foot_pos[3]; 
	float foot_goal[3];
	float knee[3];
	boolean move;
	uint tmp;
}CIKLeg;

typedef enum{
	C_AS_WORLD,
	C_AS_BODY,
	C_AS_CENTERD,
	C_AS_TOOL,
	C_AS_NONE
}CSpace;

#define C_TOOL_COUNT 4

//#define C_ANIM_POINTS 4

typedef enum{
	C_APC_BASE,
	C_APC_SPINE_TOP,
	C_APC_SPINE_MID,
	C_APC_HAND_LEFT,
	C_APC_HAND_RIGHT,
	C_APC_ARM_LEFT,
	C_APC_ARM_RIGHT,
	C_APC_HEAD_UP,
	C_APC_HEAD_LOOK,
	C_APC_TOOL_POS,
	C_APC_TOOL_DIR,
	C_APC_TOOL_UP,
	C_APC_COUNT
}CAnimPointContorls;

typedef struct{
	float pos[3];
	uint space;
	float time;
}CAnimPoint;

typedef struct{
	CAnimPoint *points;
	uint point_count;
}CAnimPoints;

typedef struct{
	CAnimPoints anim[C_APC_COUNT];
	float speed;
	float start;
	float end;
	boolean loop;
	char name[32];
}CAnimEvent;

typedef struct{
	float hand[2][3];
	float hand_timers[2];
	float timer;
	CSpace snap[2];
	uint tool;
}CIKArms;

typedef struct{
	uint id;
	float timer;
	float influence;

	boolean stop;
}CCharacterEvents;

typedef struct{
	float matrix[C_CM_COUNT + C_MAX_INVENTORY_COUNT][16];
	float look_dir[3];
	CIKArms arms[C_MAX_ARM_COUNT];
	CIKLeg legs[C_MAX_LEG_COUNT];
	CAnimPoint points[C_APC_COUNT];
	CCharacterEvents event[16];
	uint event_count;
	uint tool_current;
	uint tool_last;
	float tool_timer;
}CCharacter;

typedef struct{
	float head_size;
	float neck_length;
	float neck_pos;
	float torso_size;
	float torso_width;
	float torso_lean;
	float stomach_size;
	float butt_size;
	float butt_width;
	float body_length;
	float arms_size;
	float legs_size;
	float legs_lean[2];
	float legs_dir;
	float scale;
	CAnimPoint default_pose[C_APC_COUNT];
	CAnimEvent *events;
	uint event_count;
}CCharDesc;

extern uint			c_space_base[C_APC_COUNT];
extern char			*c_point_name[C_APC_COUNT];

/* Character management */

extern void			c_character_init(CCharacter *c, float *pos);
extern void			c_character_load(CCharDesc *desc, float scale);
extern void			c_character_animate(CCharacter *c, CCharDesc *desc, float *pos, float *upp, float *look_dir, uint tool_count, uint *tools, float *tool_matrix, uint selected_tool, float delta);
extern void			c_character_world_matrix_set(CCharacter *c, float *matrix);

extern void			c_character_create_humanoid(CCharDesc *c, float size, uint seed, float alien);
extern void			c_character_plant_pos_func_set(boolean (*func)(float *goal, float *hip, float *vector, float *normal, float leg_length, void *user));

/* animation activation */

extern void			c_character_event_play(CCharacter *c, uint id);
extern void			c_character_event_stopp(CCharacter *c, uint id);
extern uint			c_character_event_get_id(CCharDesc *desc, char *name);

extern void			c_character_event_over_ride(uint id);
extern void			c_character_event_over_ride_time(float time);

/* animation editing functionality */

extern void			c_character_to_matrix(float *output, CCharacter *c, CAnimPoint *point, uint value_id);
extern void			c_character_from_matrix(float *output, CCharacter *c, CAnimPoint *point, uint value_id);

extern uint			c_character_desc_event_create(CCharDesc *desc);
extern void			c_character_desc_event_delete(CCharDesc *desc, uint event);

extern uint			c_character_desc_key_create(CCharDesc *desc, uint event, uint value_id, float time, uint space, float *pos);
extern void			c_character_desc_key_destroy(CCharDesc *desc, uint event, uint value_id, uint key);

/* util functionality */

extern float		c_compute_ik_matrix(float *start, float *end, float length, float *dir, float *knee, float *foot, float *leg_m, float *knee_m);
extern float		c_compute_ik(float *start, float *end, float length, float *dir, float *knee, float *foot);

extern void			c_character_desc_print(CCharDesc *c);
extern void			c_character_print(CCharacter *c);