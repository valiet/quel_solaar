#include "e_includes.h"

#include "verse.h"
#include "forge.h"
#include "e_types.h"

#include "e_storage_node.h"


#define MAX_CONNECTIONS 16 /* this is the maximum number ocf conection that the enough libary can handle at once */
/* ------------------------------------------------------------------------------------------------------------------------- */

/*
** this code is designed to retrive and store node ids coming from the server
*/
typedef struct {
	boolean			connected;
	boolean			accepted;
	char			rejection[256];
	EDynLookUpTable	look_up_table;
	uint32			node_type_count[V_NT_NUM_TYPES];
	uint32			selected_node[V_NT_NUM_TYPES];
	uint32			global_version[V_NT_NUM_TYPES];
	VSession		*session;
	uint			avatar; 
} ENSConnection;

struct {
	ENSConnection	context[MAX_CONNECTIONS];
	uint			curent_net_connection;
	uint			active_connection;
	boolean			connected;
	uint			number_of_connected;
	boolean			auto_subscribe[V_NT_NUM_TYPES];
	void			(* node_create_func)(uint connection, uint id, VNodeType type, void *user);
	void			*node_create_data;
	void			(*e_custom_func[V_NT_NUM_TYPES][E_CDC_COUNT])(ENodeHead *node, ECustomDataCommand command);
} ENSGlobal;

extern void	delete_object(ENodeHead *node);
extern void	delete_geometry(ENodeHead *node);
extern void	delete_material(ENodeHead *node);
extern void	delete_bitmap(ENodeHead *node);
extern void	delete_text(ENodeHead *node);
extern void	delete_curve(ENodeHead *node);
extern void	delete_audio(ENodeHead *node);

extern void es_head_init(void);
extern void	es_object_init(void);
extern void	es_geometry_init(void);
extern void	es_material_init(void);
extern void	es_bitmap_init(void);
extern void es_text_init(void);
extern void	es_curve_init(void);
extern void	es_audio_init(void);
/* ------------------------------------------------------------------------------------------------------------------------- */

static void e_ns_del_node(uint key, void *value, void *user_data)
{
	switch(((ENodeHead *)value)->node_type)
	{
		case V_NT_OBJECT :
			delete_object((ENodeHead *)value);
			break;
		case V_NT_GEOMETRY :
			delete_geometry((ENodeHead *)value);
			break;
		case V_NT_MATERIAL :
			delete_material((ENodeHead *)value);
			break;
		case V_NT_BITMAP :
			delete_bitmap((ENodeHead *)value);
			break;
		case V_NT_TEXT :
			delete_text((ENodeHead *)value);
			break;
		case V_NT_CURVE :
			delete_curve((ENodeHead *)value);
			break;
		case V_NT_AUDIO :
			delete_audio((ENodeHead *)value);
			break;
	}
}
/*
typedef enum {
	VN_OWNER_OTHER = 0,
	VN_OWNER_MINE
} VNodeOwner;*/

extern ENodeHead *e_create_o_node(VNodeID node_id, VNodeOwner owner);
extern ENodeHead *e_create_g_node(VNodeID node_id, VNodeOwner owner);
extern ENodeHead *e_create_m_node(VNodeID node_id, VNodeOwner owner);
extern ENodeHead *e_create_b_node(VNodeID node_id, VNodeOwner owner);
extern ENodeHead *e_create_t_node(VNodeID node_id, VNodeOwner owner);
extern ENodeHead *e_create_c_node(VNodeID node_id, VNodeOwner owner);
extern ENodeHead *e_create_a_node(VNodeID node_id, VNodeOwner owner);

ENodeHead *e_create_node(VNodeID node_id, VNodeType type, VNodeOwner owner)
{
	ENodeHead	*node;

	if((node = (ENodeHead *)e_ns_get_node_networking(node_id)) == NULL)
	{
		switch(type)
		{
			case V_NT_OBJECT :
				node = e_create_o_node(node_id, owner);
			break;
			case V_NT_GEOMETRY :
				node = e_create_g_node(node_id, owner);
			break;
			case V_NT_MATERIAL :
				node = e_create_m_node(node_id, owner);
			break;
			case V_NT_BITMAP :
				node = e_create_b_node(node_id, owner);
			break;
			case V_NT_TEXT :
				node = e_create_t_node(node_id, owner);
			break;
			case V_NT_CURVE :
				node = e_create_c_node(node_id, owner);
			break;
			case V_NT_AUDIO :
				node = e_create_a_node(node_id, owner);
			break;
		}
	}
	return node;
}

void e_vc_disconnect(uint connection, const char *bye)
{
	if(ENSGlobal.context[connection].connected == TRUE)
	{
	//	verse_send_connect_terminate(bye);
		ENSGlobal.context[connection].connected = FALSE;
		e_dlut_remove_foreach(&ENSGlobal.context[connection].look_up_table, e_ns_del_node, NULL);
		e_dlut_init(&ENSGlobal.context[connection].look_up_table);
		verse_session_destroy(ENSGlobal.context[connection].session);
	}
}

void e_vc_disconnect_all(char *bye)
{
	uint i;
	for(i = 0; i < MAX_CONNECTIONS; i++)
		e_vc_disconnect(i, bye);
}

void e_vc_set_current_active_connection(uint connection)
{
	verse_session_set(ENSGlobal.context[connection].session);
}

void callback_send_node_create(void *user, VNodeID node_id, VNodeType type, VNodeOwner owner)
{
	verse_send_node_subscribe(node_id);
	e_create_node(node_id, type, owner);
	if(owner == VN_OWNER_MINE)
	{
		if(ENSGlobal.node_create_func != NULL)
			ENSGlobal.node_create_func(ENSGlobal.curent_net_connection, node_id, type, ENSGlobal.node_create_data);
		ENSGlobal.context[ENSGlobal.curent_net_connection].selected_node[type] = node_id;
	}
}

extern ENodeHead *e_ns_get_node_networking(uint node_id);

void callback_send_node_destroy(void *user_data, VNodeID node_id)
{
	ENodeHead *node;
	node = e_ns_get_node_networking(node_id);
	if(node != NULL)
	{
		ENSGlobal.context[ENSGlobal.curent_net_connection].node_type_count[node->node_type]--;
		e_dlut_remove(&ENSGlobal.context[ENSGlobal.curent_net_connection].look_up_table, node_id);
		e_ns_del_node(node_id, node, NULL);
	}
}

void e_vc_connect_internal(VSession *session, uint	connection)
{
	uint i;
	ENSGlobal.context[connection].session = session;
	for(i = 0; i < V_NT_NUM_TYPES; i++)
		ENSGlobal.context[connection].node_type_count[i] = 0;
	ENSGlobal.curent_net_connection = connection;
	if(session != NULL)
	{
		verse_session_set(ENSGlobal.context[connection].session);

	}
	ENSGlobal.number_of_connected++;
	ENSGlobal.context[connection].connected = TRUE;
}

void callback_send_connect_accept(void *user, uint32 avatar, void *address)
{
	uint i, mask = 0;
	ENSGlobal.connected = TRUE;
	for(i = 0; i < V_NT_NUM_TYPES; i++)
		if(ENSGlobal.auto_subscribe[i] == TRUE)
			mask = mask | (1 << i);
	ENSGlobal.context[ENSGlobal.curent_net_connection].avatar = avatar;
	ENSGlobal.context[ENSGlobal.curent_net_connection].accepted = TRUE;
	verse_send_node_index_subscribe(mask);
}

void callback_send_connect_terminate(const char *address, const char *bye)
{
	uint i;
	for(i = 0; bye[i] != 0; i++)
		ENSGlobal.context[ENSGlobal.curent_net_connection].rejection[i] = bye[i];
	ENSGlobal.context[ENSGlobal.curent_net_connection].rejection[i] = 0;
}

uint e_vc_connect(const char *server_address, const char *name, const char *pass, const uint8 *server_id)
{
	uint	connection;
	VSession *session;
	for(connection = 0; ENSGlobal.context[connection].connected == TRUE && connection < MAX_CONNECTIONS; connection++);
	if(connection == MAX_CONNECTIONS)
		return -1;
	ENSGlobal.context[connection].accepted = FALSE;
	ENSGlobal.context[connection].rejection[0] = 0;
	session = verse_send_connect(name, pass, server_address, (uint8 *) server_id);
	if(session  != NULL)
		e_vc_connect_internal(session, connection);
	return connection;
}


void e_vc_set_auto_subscribe(VNodeType type, boolean set)
{
	ENSGlobal.auto_subscribe[type] = set;
}

void st_set_current_active_connection(uint connection)
{
	ENSGlobal.active_connection = connection;
}

void e_vc_connection_update(uint connection, uint time)
{
	ENSGlobal.curent_net_connection = connection;
	verse_session_set(ENSGlobal.context[ENSGlobal.curent_net_connection].session);
	verse_callback_update(time);
}



boolean e_vc_check_connected(void)
{
	return ENSGlobal.connected;
}

boolean e_vc_check_connected_slot(uint connection)
{
	return ENSGlobal.context[connection].connected;
}

boolean e_vc_check_accepted_slot(uint connection)
{
	return ENSGlobal.context[connection].accepted;
}


char *e_vc_check_rejected_slot(uint connection)
{
	return ENSGlobal.context[connection].rejection;
}

void e_ns_init_head(ENodeHead *head, VNodeType type, VNodeID node_id, VNodeOwner owner)
{
	uint i;
	head->node_type = type;
	head->node_id = node_id;
	head->owner = owner;
	head->updated = TRUE;
	head->usercount = 0;
	head->structure_version = 0;
	head->data_version = 0;
	head->tag_groups = NULL;
	head->group_count = 0;
	for(i = 0; i < E_CDC_COUNT; i++)
		head->user_data[i] = NULL;
	head->group_count = 0;
	sprintf(head->node_name, "node_%i", node_id);
	head->session = ENSGlobal.curent_net_connection;
	ENSGlobal.context[head->session].node_type_count[type]++;
	e_dlut_add(&ENSGlobal.context[ENSGlobal.curent_net_connection].look_up_table, node_id, head);
	e_ns_execute(head, E_CDC_CREATE);
}

void e_ns_head_copy(ENodeHead *node_from, ENodeHead *node_to, VNodeType type)
{
	*node_to = *node_from;
	node_to->node_type = type;
	e_dlut_add(&ENSGlobal.context[ENSGlobal.curent_net_connection].look_up_table, node_to->node_id, node_to);
	free(node_from);
}

ENodeHead *	e_ns_get_node(uint connection, uint node_id)
{
	return e_dlut_get(&ENSGlobal.context[connection].look_up_table, node_id);
}

ENodeHead *	 e_ns_get_node_link(const ENodeHead *parent, uint node_id)
{
	return e_dlut_get(&ENSGlobal.context[parent->session].look_up_table, node_id);
}

ENodeHead *	e_ns_get_node_networking(uint node_id)
{
	return e_dlut_get(&ENSGlobal.context[ENSGlobal.curent_net_connection].look_up_table, node_id);
}

ENodeHead *e_ns_get_node_avatar(uint connection)
{
	return e_dlut_get(&ENSGlobal.context[connection].look_up_table, ENSGlobal.context[connection].avatar);
}


void enough_init(void)
{
	uint i, j;
	for(i = 0; i < MAX_CONNECTIONS; i++)
	{
		ENSGlobal.context[i].connected = FALSE;
		e_dlut_init(&ENSGlobal.context[i].look_up_table);
		for(j = 0; j < V_NT_NUM_TYPES; j++)
		{
			ENSGlobal.context[i].node_type_count[j] = 0;
			ENSGlobal.context[i].selected_node[j] = 0;
		}
		ENSGlobal.context[i].session = NULL;
		ENSGlobal.context[i].avatar = 0; 
	}
	ENSGlobal.curent_net_connection = 0;
	ENSGlobal.active_connection = 0;
	ENSGlobal.connected = FALSE;
	ENSGlobal.number_of_connected = 0;
	ENSGlobal.active_connection = 0;
	ENSGlobal.auto_subscribe[V_NT_OBJECT] = TRUE;
	for(i = 1; i < V_NT_NUM_TYPES; i++)
		ENSGlobal.auto_subscribe[i] = TRUE;
	ENSGlobal.auto_subscribe[V_NT_BITMAP] = TRUE;
	ENSGlobal.node_create_func = NULL;
	verse_callback_set(verse_send_connect_accept,		callback_send_connect_accept,		NULL);
	verse_callback_set(verse_send_connect_terminate,	callback_send_connect_terminate,	NULL);
	verse_callback_set(verse_send_node_create,			callback_send_node_create,			NULL);
	verse_callback_set(verse_send_node_destroy,			callback_send_node_destroy,			NULL);
	es_head_init();
	es_object_init();
	es_geometry_init();
	es_material_init();
	es_bitmap_init();
	es_text_init();
	es_curve_init();
	es_audio_init();
}

ENodeHead *e_ns_get_node_next(uint id, uint connection, VNodeType type)
{
	ENodeHead *node;
	for(node = e_dlut_next(&ENSGlobal.context[connection].look_up_table, id);
	    node != NULL && node->node_type != type;
	    node = e_dlut_next(&ENSGlobal.context[connection].look_up_table, node->node_id + 1));
	return node;
}

void e_ns_set_node_create_func(void (* func)(uint connection, uint id, VNodeType type, void *user), void *user)
{
	ENSGlobal.node_create_func = func;
	ENSGlobal.node_create_data = user;
}

ENodeHead *e_ns_get_node_selected(uint connection, VNodeType type)
{
	return e_dlut_next(&ENSGlobal.context[connection].look_up_table, ENSGlobal.context[connection].selected_node[type]);
}


uint e_ns_get_node_selected_id(uint connection, VNodeType type)
{
	return ENSGlobal.context[connection].selected_node[type];
}

void e_ns_set_node_selected(uint connection, uint id, VNodeType type)
{
	ENSGlobal.context[connection].selected_node[type] = id;
}

uint  e_ns_get_node_count(uint connection, VNodeType type)
{
	return ENSGlobal.context[connection].node_type_count[type];
}

uint  e_ns_get_node_id(const ENodeHead *node)
{
	if(node != NULL)	
		return node->node_id;
	return -1;
}

VNodeType  e_ns_get_node_type(const ENodeHead *node)
{
	if(node != NULL)
		return node->node_type;
	return 0;
}

char * e_ns_get_node_name(ENodeHead *node)
{
	if(node != NULL)
		return node->node_name;
	return NULL;
}

VNodeOwner e_ns_get_node_owner(const ENodeHead *node)
{
	if(node != NULL)
		return node->owner;
	return -1;
}

uint e_ns_get_node_connection(const ENodeHead *node)
{
	if(node != NULL)
		return node->session;
	return 0;
}

void e_ns_set_custom_data(ENodeHead *node, uint slot, void *data)
{
	node->user_data[slot] = data;
}


void * e_ns_get_custom_data(const ENodeHead *node, uint slot)
{
	return node != NULL ? node->user_data[slot] : NULL;
}


void e_ns_set_custom_func(uint slot, VNodeType type, void (*func)(ENodeHead *node, ECustomDataCommand command))
{
	ENSGlobal.e_custom_func[type][slot] = func;
}

void e_ns_execute(ENodeHead *node, ECustomDataCommand command)
{
	uint i;
	for(i = 0; i < E_CDC_COUNT; i++)
		if(ENSGlobal.e_custom_func[node->node_type][i] != NULL)
			ENSGlobal.e_custom_func[node->node_type][i](node, command);
}

uint e_ns_get_node_version_struct(const ENodeHead *node)
{
	return node->structure_version;
}

uint e_ns_get_node_version_data(const ENodeHead *node)
{
	return node->data_version;
}

void e_ns_update_node_version_struct(ENodeHead *node)
{
	node->structure_version++;
	node->data_version++;
	ENSGlobal.context[node->session].global_version[node->node_type]++;
	e_ns_execute(node, E_CDC_STRUCT);
}

void e_ns_update_node_version_data(ENodeHead *node)
{
	node->data_version++;
	ENSGlobal.context[node->session].global_version[node->node_type]++;
	e_ns_execute(node, E_CDC_DATA);
}

uint e_ns_get_global_version(uint connection, VNodeType type)
{
	return ENSGlobal.context[connection].global_version[type];
}
