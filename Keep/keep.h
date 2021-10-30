
#ifndef KSession
typedef void KSession;
#endif

#define K_CLUSTER_NULL 0

extern KSession	*keep_session_create(uint64 memory_footprint, uint64 user_id);
extern boolean	keep_session_destroy(KSession *session);
extern boolean	keep_session_service(KSession *session);
extern void		*keep_get(KSession *session, uint64 address, uint *size);
extern void		keep_return(KSession *session, uint64 address, boolean modifyed);
extern boolean	keep_allocate_root(KSession *session, uint64 size, uint64 address);
extern uint64	keep_allocate(KSession *session, uint64 size);
extern void		keep_free(KSession *session, uint64 address);


extern void k_crypto_rand_init();
extern void k_crypto_rand_set(uint8 *input, uint size);
extern void k_crypto_rand(uint8 *output, uint size);

typedef uint64 KUser;

typedef enum{ /* Reduced list of Types */
	K_JM_NOT_JOURNALED,
	K_JM_JOURNALED_PRECISSION_LOW,
	K_JM_JOURNALED_PRECISSION_MID,
	K_JM_JOURNALED_PRECISSION_HIGH,
	K_JM_COUNT
}KJournalingMode;

typedef enum{ /* Reduced list of Types */
	K_VT_INT8,
	K_VT_UINT8,
	K_VT_INT16,
	K_VT_UINT16,
	K_VT_INT32,
	K_VT_UINT32,
	K_VT_INT64,
	K_VT_UINT64,
	K_VT_REAL32,
	K_VT_REAL64,
	K_VT_TIMESTAMP_LOW,
	K_VT_TIMESTAMP_MID,
	K_VT_TIMESTAMP_HIGH,
	K_VT_USER,
	K_VT_ID,
	K_VT_STRING,
	K_VT_ENUM,
	K_VT_STRUCT,
	K_VT_COUNT,
	K_VT_ANY_TYPE = 255,
}KValueType;

#ifndef K_RECORD_INTERNAL

typedef void KRecord;
typedef void KValue;

extern KRecord *k_record_create(KUser user, KJournalingMode journaling);
extern void k_record_free(KRecord *record);

extern void k_record_serialize(KRecord *record, void *handle, boolean journaling);
extern KRecord *k_record_unserialize(void *handle);

extern void k_record_timestamp_lock(KRecord *record);
extern void k_record_timestamp_lock_specific(KRecord *record, int64 seconds, uint64 fractions);
extern void k_record_timestamp_unlock(KRecord *record);

extern KValueType k_record_entry_type(KValue *handle);
extern uint64	k_record_entry_array(KValue *handle);
extern uint64	k_record_entry_array(KValue *handle);
extern uint64	k_record_entry_version(KValue *handle);
extern char		*k_record_entry_comment_get(KValue *handle);
extern void		k_record_entry_comment_set(KValue *handle, char *comment);
extern void		k_record_entry_delete(KValue *handle);
extern boolean	k_record_entry_deleted(KValue *handle);
extern boolean	k_record_entry_leaf(KValue *handle);

extern boolean	k_record_entry_array_set_size(KRecord *record, KValue *handle, uint64 size);

extern int8		k_record_entry_int8_name_get(KRecord *record, char *name, uint id, int8 default_value, boolean create);
extern boolean	k_record_entry_int8_name_set(KRecord *record, char *name, uint id, int8 value);
extern int8		k_record_entry_int8_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_int8_handle_set(KRecord *record, KValue *handle, uint id, int8 value);
extern uint8	k_record_entry_uint8_name_get(KRecord *record, char *name, uint id, uint8 default_value, boolean create);
extern boolean	k_record_entry_uint8_name_set(KRecord *record, char *name, uint id, uint8 value);
extern uint8	k_record_entry_uint8_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_uint8_handle_set(KRecord *record, KValue *handle, uint id, uint8 value);
extern int16	k_record_entry_int16_name_get(KRecord *record, char *name, uint id, int16 default_value, boolean create);
extern boolean	k_record_entry_int16_name_set(KRecord *record, char *name, uint id, int16 value);
extern int16	k_record_entry_int16_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_int16_handle_set(KRecord *record, KValue *handle, uint id, int16 value);
extern uint16	k_record_entry_uint16_name_get(KRecord *record, char *name, uint id, uint16 default_value, boolean create);
extern boolean	k_record_entry_uint16_name_set(KRecord *record, char *name, uint id, uint16 value);
extern uint16	k_record_entry_uint16_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_uint16_handle_set(KRecord *record, KValue *handle, uint id, uint16 value);
extern int32	k_record_entry_int32_name_get(KRecord *record, char *name, uint id, int32 default_value, boolean create);
extern boolean	k_record_entry_int32_name_set(KRecord *record, char *name, uint id, int32 value);
extern int32	k_record_entry_int32_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_int32_handle_set(KRecord *record, KValue *handle, uint id, int32 value);
extern uint32	k_record_entry_uint32_name_get(KRecord *record, char *name, uint id, uint32 default_value, boolean create);
extern boolean	k_record_entry_uint32_name_set(KRecord *record, char *name, uint id, uint32 value);
extern uint32	k_record_entry_uint32_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_uint32_handle_set(KRecord *record, KValue *handle, uint id, uint32 value);
extern int64	k_record_entry_int64_name_get(KRecord *record, char *name, uint id, int64 default_value, boolean create);
extern boolean	k_record_entry_int64_name_set(KRecord *record, char *name, uint id, int64 value);
extern int64	k_record_entry_int64_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_int64_handle_set(KRecord *record, KValue *handle, uint id, int64 value);
extern uint64	k_record_entry_uint64_name_get(KRecord *record, char *name, uint id, uint64 default_value, boolean create);
extern boolean	k_record_entry_uint64_name_set(KRecord *record, char *name, uint id, uint64 value);
extern uint64	k_record_entry_uint64_handle_get(KRecord *record, KValue *handle, uint id);

extern boolean	k_record_entry_uint64_handle_set(KRecord *record, KValue *handle, uint id, uint64 value);
extern float	k_record_entry_float_name_get(KRecord *record, char *name, uint id, float default_value, boolean create);
extern float	k_record_entry_float_handle_get(KRecord *record, KValue *handle, uint id);
extern boolean	k_record_entry_float_handle_set(KRecord *record, KValue *handle, uint id, float value);

extern double	k_record_entry_double_name_get(KRecord *record, char *name, uint id, double default_value, boolean create);
extern boolean	k_record_entry_double_name_set(KRecord *record, char *name, uint id, double value);
extern double	k_record_entry_double_handle_get(KRecord *record, KValue *handle, uint id);
extern boolean	k_record_entry_double_handle_set(KRecord *record, KValue *handle, uint id, double value);

extern char		*k_record_entry_string_name_get(KRecord *record, char *name, uint id, char *default_value, boolean create);
extern boolean	k_record_entry_string_name_set(KRecord *record, char *name, uint id, char *string);
extern char		*k_record_entry_string_handle_get(KRecord *record, KValue *handle, uint id);
extern boolean	k_record_entry_string_handle_set(KRecord *record, KValue *handle, uint id, char *string);

#endif /* K_RECORD_INTERNAL */ 

extern void k_type_enum_si_unit_name_print(char *buffer, uint type);