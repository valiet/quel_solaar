
#if !defined(ENOUGH_H)

#define E_CDC_COUNT 16

typedef enum{
	E_CDC_CREATE,
	E_CDC_STRUCT,
	E_CDC_DATA,
	E_CDC_DESTROY
}ECustomDataCommand;
#endif

typedef struct{
/*	GCond			*waiting;*/
	uint			usercount;
	uint32			node_id;
	VNodeOwner		owner;
	uint			node_type;
	char			node_name[48];
	uint			session;
	boolean			updated;
	uint			structure_version;
	uint			data_version;
	void			*tag_groups;
	uint			group_count;
	void			*user_data[E_CDC_COUNT];
}ENodeHead;



extern ENodeHead *e_ns_get_node_networking(uint node_id);
extern void e_ns_init_head(ENodeHead *head, VNodeType type, VNodeID node_id, VNodeOwner owner);
extern void e_ns_execute(ENodeHead *head, ECustomDataCommand command);
extern void e_ns_head_copy(ENodeHead *node_from, ENodeHead *node_to, VNodeType type);
