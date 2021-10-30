
#include "k_internal.h"


uint k_data_type_size[] = {sizeof(int8), /* K_VT_INT8 */
							sizeof(uint8), /* K_VT_UINT8 */
							sizeof(int16), /* K_VT_INT16 */
							sizeof(uint16), /* K_VT_UINT16 */
							sizeof(int32), /* K_VT_INT32 */
							sizeof(uint32), /* K_VT_UINT32 */
							sizeof(int64), /* K_VT_INT64 */
							sizeof(uint64), /* K_VT_UINT64 */
							sizeof(float), /* K_VT_REAL32 */
							sizeof(double), /* K_VT_REAL64 */
							sizeof(KTimestamp), /* K_VT_TIMESTAMP_LOW */
							sizeof(KTimestamp), /* K_VT_TIMESTAMP_MID */
							sizeof(KTimestamp), /* K_VT_TIMESTAMP_HIGH */
							sizeof(uint64), /* K_VT_USER */
							256, /* K_VT_ID */
							sizeof(char *), /* K_VT_STRING */
							sizeof(uint8), /* K_VT_ENUM */
							0}; /* K_VT_STRUCT */

uint k_data_type_alignment[] = {sizeof(int8), /* K_VT_INT8 */
							sizeof(uint8), /* K_VT_UINT8 */
							sizeof(int16), /* K_VT_INT16 */
							sizeof(uint16), /* K_VT_UINT16 */
							sizeof(int32), /* K_VT_INT32 */
							sizeof(uint32), /* K_VT_UINT32 */
							sizeof(int64), /* K_VT_INT64 */
							sizeof(uint64), /* K_VT_UINT64 */
							sizeof(float), /* K_VT_REAL32 */
							sizeof(double), /* K_VT_REAL64 */
							sizeof(uint64), /* K_VT_TIMESTAMP_LOW */
							sizeof(uint64), /* K_VT_TIMESTAMP_MID */
							sizeof(uint64), /* K_VT_TIMESTAMP_HIGH */
							sizeof(uint64), /* K_VT_USER */
							1, /* K_VT_ID */
							sizeof(char *), /* K_VT_STRING */
							sizeof(uint8), /* K_VT_ENUM */
							0}; /* K_VT_STRUCT */

uint k_data_foot_size[] = {0, /* K_VT_INT8 */
							0, /* K_VT_UINT8 */
							0, /* K_VT_INT16 */
							0, /* K_VT_UINT16 */
							0, /* K_VT_INT32 */
							0, /* K_VT_UINT32 */
							0, /* K_VT_INT64 */
							0, /* K_VT_UINT64 */
							0, /* K_VT_REAL32 */
							0, /* K_VT_REAL64 */
							0, /* K_VT_TIMESTAMP_LOW */
							0, /* K_VT_TIMESTAMP_MID */
							0, /* K_VT_TIMESTAMP_HIGH */
							0, /* K_VT_USER */
							0, /* K_VT_ID */
							0, /* K_VT_STRING */
							sizeof(KFootEnum), /* K_VT_ENUM */
							sizeof(KFootStruct)}; /* K_VT_STRUCT */


KRecord *k_record_unpack(void *array)
{

}

KRecord *k_record_pack(void *record)
{

}

KRecord *k_record_create(KUser user, KJournalingMode journaling)
{
	KRecord *record;
	record = malloc(sizeof *record);
	record->store_address = K_CLUSTER_NULL;
//	k_crypto_id_generate(record->id);
	record->derivative = NULL;
	if(journaling != K_JM_NOT_JOURNALED)
		record->flags = 1 + journaling * 4;
	else
		record->flags = 0;
	imagine_time_epoc_current(&record->timestamp.seconds, &record->timestamp.fractions);
	record->owner = user;
	record->value_count = 0;
	record->signatures = NULL;
	record->memory_used = 0; 
	record->memory_allocated = 0;
	record->allocation = NULL;
	record->timestamp_lock = FALSE;
	return record; 
}
void k_record_struct_text_free(KRecord *record, KJournalHead *handle, uint8 *array, uint64 array_length)
{
	uint i, j, k, offset;
		
	KFootStruct *foot;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		foot = (KFootStruct *)&((KJournalHead *)handle)[1];
	else
		foot = (KFootStruct *)&handle[1];
	if(foot->text)
	{
		KHead *param;
		char **text;

		for(i = 0; i < foot->element_count; i++)
		{
			param = &record->allocation[foot->elements[i]];
			offset = ((offset + k_data_type_alignment[param->type] - 1) / k_data_type_alignment[param->type]) * k_data_type_alignment[param->type]; 
			if(param->type == K_VT_STRING)
			{
				for(j = 0; j < array_length; j++)
				{
					text = &array[foot->size_of * j + offset];
					for(k = 0; k < param->array_length; k++)
						if(text[k] != NULL)
							free(text[i]);
				}
			}
			offset += k_data_type_size[param->type] * param->array_length;
		}
	}
}
void k_record_free(KRecord *record)
{
	uint i, j, k, offset;
	KJournalHead *jhandle;
	KHead *handle;
		
//	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)

	for(handle = k_record_entry_next(record, NULL, K_VT_ANY_TYPE, TRUE, TRUE); 
		handle != NULL;
		handle = k_record_entry_next(record, handle, K_VT_ANY_TYPE, TRUE, TRUE))
	{
		if(handle->type == K_VT_ENUM)
		{
			KFootEnum *foot;
			void **value;
			if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
				value = (void **)&((KJournalHead *)handle)[1];
			else
				value = (void **)&handle[1];
			foot = (KFootEnum *)&value[1];
			for(i = 0; i < foot->element_count; i++)
				free(foot->elements[i]);
			free(foot->elements);
		}else if(handle->type == K_VT_STRUCT)
			k_record_struct_text_free(record, handle, handle->array, handle->array_length);

		if(handle->array != NULL)
		{
			if(handle->type == K_VT_STRING)
				for(i = 0; i < handle->array_length; i++)
					if(((char *)handle->array)[i] != NULL)
						free(((char *)handle->array)[i]);
			free(handle->array);
		}else if(handle->type == K_VT_STRING)
		{
			char **text;
			if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			{
				jhandle = handle;
				text = (char **)&jhandle[1];
			}else
				text = (char **)&handle[1];
			free(*text);
		}
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		{
			jhandle = (KJournalHead *)handle;
			for(i = 0; i < jhandle->entry_count; i++)
			{
				if(jhandle->entry[i].array != NULL)
				{
					if(handle->type == K_VT_STRING)
						for(j = 0; j < jhandle->entry[i].size; j++)
							if(((char **)jhandle->entry[i].array)[j] != NULL)
								free(((char **)jhandle->entry[i].array)[j]);
					if(handle->type == K_VT_STRUCT)
						k_record_struct_text_free(record, handle, jhandle->entry[i].array, jhandle->entry[i].size);
					free(jhandle->entry[i].array);
				}
			}
			if(jhandle->entry != NULL)
				free(jhandle->entry);
		}
	}
	if(record->allocation != NULL)
		free(record->allocation);
	free(record); 
}

void k_record_timestamp_lock(KRecord *record)
{
	imagine_time_epoc_current(&record->timestamp.seconds, &record->timestamp.fractions);
	record->timestamp_lock = TRUE;
}
void k_record_timestamp_lock_specific(KRecord *record, int64 seconds, uint64 fractions)
{
	record->timestamp.seconds = seconds;
	record->timestamp.fractions = fractions;
	record->timestamp_lock = TRUE;
}
void k_record_timestamp_unlock(KRecord *record)
{
	record->timestamp_lock = FALSE;
}




uint64 k_record_entry_sizeof(KRecord *record, KValueType type)
{
	uint memory;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		memory = sizeof(KJournalHead);
	else
		memory = sizeof(KHead);
	memory += ((k_data_type_size[type] + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *);
	memory += ((k_data_foot_size[type] + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *);
	return memory;
}

KHead *k_record_entry_next(KRecord *record, KHead *previous, KValueType type, boolean include_struct_members, boolean include_deleted)
{
	if(record == NULL)
		return NULL;
	if(previous == NULL)
	{
		if(record->allocation == NULL)
			return NULL;
		previous = (KHead *)record->allocation;
	}else
	{
		previous = (KHead *)(&((uint8 *)previous)[k_record_entry_sizeof(record, previous->type)]);
	}
	if(type == K_VT_ANY_TYPE)
	{
		while(&record->allocation[record->memory_used] > previous)
		{
			if(include_deleted || previous->array_length != 0)
				if(include_struct_members || !(previous->flags & K_RECORD_VALUE_FLAG_LEAF))
					return previous;
			previous = (KHead *)(&((uint8 *)previous)[k_record_entry_sizeof(record, previous->type)]);
		}
	}else
	{
		while(&record->allocation[record->memory_used] > previous)
		{
			if(type == previous->type)
				if(include_deleted || previous->array_length != 0)
					if(include_struct_members || !(previous->flags & K_RECORD_VALUE_FLAG_LEAF))	
						return previous;
			previous = (KHead *)(&((uint8 *)previous)[k_record_entry_sizeof(record, previous->type)]);
		}
	}
	return NULL;
}


KHead *k_record_entry_lookup_name(KRecord *record, char *name, KValueType type, boolean include_struct_members, boolean include_deleted)
{
	KHead *entry;
	uint i;
	for(entry = k_record_entry_next(record, NULL, type, include_struct_members, include_deleted); entry != NULL; entry = k_record_entry_next(record, entry, type, include_struct_members, include_deleted))
	{
		for(i = 0; name[i] == entry->name[i] && name[i] != 0; i++);
		if(name[i] == entry->name[i])
			return entry;
	}
	return NULL;
}

KHead *k_record_entry_add(KRecord *record, KValueType type, char *name)
{
	KHead *head;
	uint64 memory;
	boolean journaling;
	uint i, size;
	uint8 *data;
	memory = k_record_entry_sizeof(record, type);

	if(record->memory_used + memory > record->memory_allocated)
	{
		record->memory_allocated = record->memory_used + memory + KEEP_RECORD_MEM_BLOCK_SIZE - 1;
		record->memory_allocated = (record->memory_allocated / KEEP_RECORD_MEM_BLOCK_SIZE) * KEEP_RECORD_MEM_BLOCK_SIZE;
		record->allocation = realloc(record->allocation, record->memory_allocated);
	}
	head = &record->allocation[record->memory_used];
	record->memory_used += memory;
	head->comment = NULL;
	head->flags = K_RECORD_VALUE_FLAG_ARRAY_COUNT; /* need to go over this one a bit more */
	for(i = 0; i < K_VALUE_NANE_BUFFER_SIZE - 1; i++)
		head->name[i] = name[i];
	head->name[i] = 0;
	head->type = type;
	head->array  = NULL;
	head->array_length = 1;
	size = ((k_data_type_size[type] + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *);
	size += ((k_data_foot_size[type] + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *);
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		KJournalHead *h;
		h = head;
		h->entry = NULL;
		h->entry_count = 0;
		data = (uint8 *)&h[1];
	}else
		data = (uint8 *)&head[1];
	for(i = 0; i < size; i++)
		data[i] = 0;
	return head; 
}

KHead *k_record_enum_add(KRecord *record, char *name, uint8 enum_count, char **names, uint8 selected_enum)
{
	KHead *handle, h_list[255];
	KFootEnum *foot;
	void **value;
	uint i, j, size_of;

	handle = k_record_entry_add(record, K_VT_ENUM, name);
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		value = (void **)&((KJournalHead *)handle)[1];
	else
		value = (void **)&handle[1];
	((uint8*)value)[0] = selected_enum;
	foot = (KFootEnum *)&value[1];
	foot->element_count = enum_count;
	foot->elements = malloc((sizeof *foot->elements) * enum_count);
	for(i = 0; i < enum_count; i++)
		foot->elements[i] = f_text_copy_allocate(names[i]);
}

uint k_record_enum_count_get(KRecord *record, KHead *handle)
{
	KFootEnum *foot;
	void **value;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		value = (void **)&((KJournalHead *)handle)[1];
	else
		value = (void **)&handle[1];
	foot = (KFootEnum *)&value[1];
	return foot->element_count;
}


uint k_record_enum_name_get(KRecord *record, KHead *handle, uint id)
{
	KFootEnum *foot;
	void **value;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		value = (void **)&((KJournalHead *)handle)[1];
	else
		value = (void **)&handle[1];
	foot = (KFootEnum *)&value[1];
	return foot->elements[id];
}

boolean k_record_struct_set_active(KRecord *record, KHead *handle, uint id)
{
	KHead *component;
	KFootStruct *foot;
	uint i, offset = 0;
	if(handle->array_length <= id || handle->type != K_VT_STRUCT)
		return FALSE;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		foot = (KFootStruct *)&((KJournalHead *)handle)[1];
	else
		foot = (KFootStruct *)&handle[1];
	foot->selected = id;
	for(i = 0; i < foot->element_count; i++)
	{
		component = &record->allocation[foot->elements[i]];
		offset = ((offset + k_data_type_alignment[component->type] - 1) / k_data_type_alignment[component->type]) * k_data_type_alignment[component->type]; 
		component->array = &((uint8 *)handle->array)[foot->size_of * id + offset];
		offset += k_data_type_size[component->type] * component->array_length;
	}
	return TRUE;
}

KHead *k_record_struct_add(KRecord *record, char *name, uint8 member_count, char **member_names)
{
	KHead *handle, *h;
	uint h_list[255];
	KFootStruct *foot;
	uint i, j, size_of = 0;
	boolean text = FALSE;
	for(i = 0; i < member_count; i++)
	{
		for(handle = k_record_entry_next(record, NULL, K_VT_STRUCT, FALSE, FALSE); handle != NULL; handle = k_record_entry_next(record, handle, K_VT_STRUCT, FALSE, FALSE))
		{
			for(j = 0; member_names[i][j] == handle->name[j] && member_names[i][j] != 0; j++);
			if(member_names[i][j] == handle->name[j])
				break;
		}
		if(handle == NULL || handle->flags & K_RECORD_VALUE_FLAG_LEAF || handle->type == K_VT_STRING) /* you cant re use struct members !*/
			return NULL;
		if(handle->type == K_VT_STRING)
			text = TRUE;
		h_list[i] = (uint8 *)(handle) - record->allocation;
		size_of = ((size_of + k_data_type_alignment[handle->type] - 1) / k_data_type_alignment[handle->type]) * k_data_type_alignment[handle->type];
		size_of += k_data_type_size[handle->type] * handle->array_length;
	}
	handle = k_record_entry_add(record, K_VT_STRUCT, name);
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		foot = (KFootEnum *)&((KJournalHead *)handle)[1];
	else
		foot = (KFootEnum *)&handle[1];
	size_of = ((size_of + sizeof(void *) - 1) / sizeof(void *)) * sizeof(void *);
	handle->array = malloc(size_of);
	foot->size_of = size_of;
	size_of /= 4;
	for(i = 0; i < size_of; i++)
		((uint32 *)handle->array)[i] = 0;
	foot->element_count = member_count;
	foot->text = text;
	foot->elements = malloc((sizeof *foot->elements) * foot->element_count);
	for(i = 0; i < member_count; i++)
		foot->elements[i] = ((uint8 *)h_list[i]) - record->allocation;
	foot->element_count = member_count;
	foot->elements = malloc((sizeof *foot->elements) * member_count);
	for(i = 0; i < member_count; i++)
	{
		h = (KHead *)&record->allocation[h_list[i]];
		h->flags |= K_RECORD_VALUE_FLAG_LEAF;
		if(h->array != NULL)
			free(h->array);
		foot->elements[i] = h_list[i];
		printf("sizes %u %u %u %u\n", (uint8 *)(h_list[i]), record->allocation, (uint8 *)(h_list[i]) - record->allocation, record->allocation - (uint8 *)(h_list[i]));
	}
	k_record_struct_set_active(record, handle, 0);
}

uint k_record_struct_count_get(KRecord *record, KHead *handle)
{
	KFootStruct *foot;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		foot = (KFootStruct *)&((KJournalHead *)handle)[1];
	else
		foot = (KFootStruct *)&handle[1];
	return foot->element_count;
}


KHead *k_record_struct_member_get(KRecord *record, KHead *handle, uint id)
{
	KFootStruct *foot;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		foot = (KFootStruct *)&((KJournalHead *)handle)[1];
	else
		foot = (KFootStruct *)&handle[1];
	return &record->allocation[foot->elements[id]];
}

//KHead *k_record_struct_add(KRecord *record, char *name, uint8 member_count, char **member_names);

KValueType k_record_entry_type(KHead *handle)
{
	return handle->type;
}

char *k_record_entry_name(KHead *handle)
{
	return handle->name;
}

uint64 k_record_entry_array(KHead *handle)
{
	return handle->array_length;
}

KTimestamp k_record_entry_version(KRecord *record, KHead *handle)
{
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		KJournalHead *h;
		h = handle;
		return h->timestamp;
	}
	return record->timestamp;
}


char *k_record_entry_name_get(KHead *handle)
{
	return handle->name;
}

void k_record_entry_name_set(KHead *handle, char *name)
{
	uint i;
	for(i = 0; name[i] != 0 && i < K_VALUE_NANE_BUFFER_SIZE - 1; i++)
		handle->name[i] = name[i];
	handle->name[i] = 0;
}

char *k_record_entry_comment_get(KHead *handle)
{
	return handle->comment;
}

void k_record_entry_comment_set(KHead *handle, char *comment)
{
	if(handle->comment != NULL)
	{
		free(handle->comment);
		if(comment == NULL)
		{
			handle->comment = NULL;
			handle->flags = handle->flags & ~K_RECORD_VALUE_FLAG_COMMENT;
			return;
		}
		handle->comment = f_text_copy_allocate(comment);
		return;
	}
	if(comment != NULL)
	{
		handle->flags = handle->flags | K_RECORD_VALUE_FLAG_COMMENT;
		handle->comment = f_text_copy_allocate(comment);
		return;
	}
}


uint k_record_entry_journal_count(KRecord *record, KJournalHead *handle)
{
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		KTimestamp timestamp;
		timestamp = record->timestamp;
		uint i, count = 0;
		if(handle->array.type == K_VT_STRUCT)
		{
			for(i = 0; i < handle->entry_count; i++)
				;
		}else
		{
			printf("handle->entry_count %u\n", handle->entry_count);
			for(i = 0; i < handle->entry_count; i++)
			{
				if(handle->entry[i].timestamp.fractions != timestamp.fractions ||
					handle->entry[i].timestamp.seconds != timestamp.seconds)
				{
					count++;
					timestamp = handle->entry[i].timestamp;
				}
			}
		}
		return count;
	}
	return 0;
}


boolean k_record_entry_journal_next(KRecord *record, KJournalHead *handle, KTimestamp *timestamp)
{
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		uint i;
		if(handle->array.type == K_VT_STRUCT)
		{
			for(i = 0; i < handle->entry_count; i++)
				;
		}else
		{
		/*	if(handle->timestamp.seconds > timestamp->seconds ||
				(handle->timestamp.fractions > timestamp->fractions &&
				handle->timestamp.seconds == timestamp->seconds))
			{
				*timestamp = handle->timestamp;
				return TRUE;
			}*/
			for(i = handle->entry_count; i > 0;)
			{
				i--;
				if(handle->entry[i].timestamp.seconds < timestamp->seconds ||
					(handle->entry[i].timestamp.fractions < timestamp->fractions &&
					handle->entry[i].timestamp.seconds == timestamp->seconds))
				{
					*timestamp = handle->entry[i].timestamp;
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}



boolean k_record_entry_leaf(KHead *handle)
{
	return handle->flags & K_RECORD_VALUE_FLAG_LEAF;
}

void k_record_entry_int32_set_internal(KRecord *record, KHead *handle, int32 value, KTimestamp timestamp)
{
	int32 *v;
	if(handle->array == NULL)
	{
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			v = (uint32 *)&((KJournalHead *)handle)[1];
		else
			v = (uint32 *)&handle[1];
	}
	else
		v = handle->array;
	if(v[0] == value)
		return;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		KJournalHead *h;
		h = handle;
		if(h->entry_count % KEEP_JOURNALING_ALLOCATION_SIZE == 0)
			h->entry = realloc(h->entry_count, (sizeof *h->entry) * (h->entry_count + KEEP_JOURNALING_ALLOCATION_SIZE));
		h->entry[h->entry_count].start = 0;
		h->entry[h->entry_count].size = 1;
		if(h->entry_count == 0)
			h->entry[h->entry_count].array = NULL;
		else
		{
			h->entry[h->entry_count].array = malloc(sizeof(int32));
			((int32 *)h->entry[h->entry_count].array)[0] = v[0]; 
		}
		h->entry[h->entry_count].timestamp = timestamp;
		h->entry_count++;
		record->timestamp = h->timestamp;
	}
	v[0] = value; 
}

int32 k_record_entry_int32_get_internal(KRecord *record, KHead *handle, int32 value)
{
	int32 *v;
	if(handle->array == NULL)
	{
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			v = (uint32 *)&((KJournalHead *)handle)[1];
		else
			v = (uint32 *)&handle[1];
	}
	else
		v = handle->array;
	return *v;
}

uint64 k_record_entry_array_get_size(KRecord *record, KHead *handle)
{
	return handle->array_length;
}

boolean k_record_entry_array_set_size(KRecord *record, KHead *handle, uint64 size)
{
	if(handle->array_length == size || size == 0)
		return TRUE;

	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		KJournalHead *jhandle;         
		KJournalEntry *entry;
		jhandle = (KJournalHead *)handle;
		if(jhandle->entry_count % KEEP_JOURNALING_ALLOCATION_SIZE == 0)
			jhandle->entry = realloc(jhandle->entry_count, (sizeof *jhandle->entry) * (jhandle->entry_count + KEEP_JOURNALING_ALLOCATION_SIZE));
		entry = &jhandle->entry[jhandle->entry_count++];
		if(size > handle->array_length)
		{
			entry->array = NULL;
			entry->start = handle->array_length;
			entry->size = size - handle->array_length;
			
 		}else
		{
			uint size_of;
			entry->array = NULL;
			entry->start = size;
			entry->size = handle->array_length; - size;			
			size_of = k_data_type_size[handle->type];
			entry->array = malloc(entry->size * size_of);
			memcpy(entry->array, ((uint8 *)handle->array)[size * size_of], entry->size * size_of);
		}
		imagine_time_epoc_current(&entry->timestamp.seconds, &entry->timestamp.fractions);
	}

	if(handle->type != K_VT_STRUCT && (size == 1 || handle->array_length == 1))
	{
		uint8 *v;
		uint size_of, i;
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			v = (uint32 *)&((KJournalHead *)handle)[1];
		else
			v = (uint32 *)&handle[1];
		size_of = k_data_type_size[handle->type];
		if(size == 1)
		{
			for(i = 0; i < size_of; i++)
				v[i] = ((uint8 *)handle->array)[i];
			handle->array = NULL;
			free(handle->array);
		}else
		{
			handle->array = calloc(size_of, size);
			if(handle->array == NULL)
				return FALSE;
			for(i = 0; i < size_of; i++)
				((uint8 *)handle->array)[i] = v[i];
		}
	}else
	{
		void *allocation;
		allocation = calloc(k_data_type_size[handle->type], size);
		if(allocation == NULL)
			return FALSE;
		if(handle->array_length < size)
			memcpy(allocation, handle->array, handle->array_length);
		else
			memcpy(allocation, handle->array, size);
		free(handle->array);
		handle->array = allocation;
	}
	handle->array_length = size;
//	k_record_struct_set_active(record, handle, id)
	return TRUE;
}

void k_record_entry_delete(KRecord *record, KHead *handle)
{
	if(handle->array_length != 0)
		k_record_entry_array_set_size(record, handle, 0);
}

boolean k_record_entry_deleted(KRecord *record, KHead *handle)
{
	return handle->array_length == 0;
}



void k_record_entry_array_handle_set(KRecord *record, KHead *handle, void *values, uint start, uint length)
{
	void *v;
	uint size_of, i, j, k;
	if(length == 0 || values == NULL)
		return;
	if(start + length > handle->array_length)
		if(!k_record_entry_array_set_size(record, handle, start + length))
			return;

	if(handle->array == NULL)
	{
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			v = &((KJournalHead *)handle)[1];
		else
			v = &handle[1];
	}
	else
		v = handle->array;

	if(start + length > handle->array_length)
		length = handle->array_length - start;
	size_of = k_data_type_size[handle->type];

	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		KJournalHead *h;
		KTimestamp timestamp;
		KJournalEntry *e;
		h = handle;

		if(!record->timestamp_lock)
			imagine_time_epoc_current(&record->timestamp.seconds, &record->timestamp.fractions);
		
		for(e = &h->entry[h->entry_count - 1] ;h->entry_count != 0 && ((e->timestamp.seconds > record->timestamp.seconds) || (
			e->timestamp.seconds == record->timestamp.seconds &&
			e->timestamp.fractions > record->timestamp.fractions)); e = &h->entry[--h->entry_count - 1])
		{
			if(e->array != NULL)
				free(e->array);
		}
		if(h->entry_count % KEEP_JOURNALING_ALLOCATION_SIZE == 0)
			h->entry = realloc(h->entry_count, (sizeof *h->entry) * (h->entry_count + KEEP_JOURNALING_ALLOCATION_SIZE));
		h->entry[h->entry_count].start = start;
		h->entry[h->entry_count].size = length;
		if(h->entry_count == 0)
		{
			h->entry[h->entry_count].array = NULL;
			h->entry[h->entry_count].timestamp = h->timestamp = record->timestamp;
		}else
		{
			char *text;
			h->entry[h->entry_count].array = malloc(length * size_of);
			memcpy(h->entry[h->entry_count].array, &((uint8 *)v)[size_of * start], length * size_of);
			text = ((char **)h->entry[h->entry_count].array)[0];
			printf("test %s %p\n", text, text);
			h->entry[h->entry_count].timestamp = h->timestamp;
			h->timestamp = record->timestamp;
		}
		h->entry_count++;
	}
	if(handle->type == K_VT_STRING)
	{
		for(i = 0; i < length; i++)
		{
			char **text;
			if(!(record->flags & K_RECORD_ROOT_FLAG_JOURNALING))
				if(((char **)v)[i + start] != NULL)
					free(((char **)v)[i + start]);
			text = (char **)values;
			if(text[i])
				text[i] = f_text_copy_allocate(text[i]);
		}
	}
	if(handle->type == K_VT_STRUCT)
	{
		KHead *member;
		KFootStruct *foot;
		char **strings;
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			foot = (KFootStruct *)&((KJournalHead *)handle)[1];
		else
			foot = (KFootStruct *)&handle[1];
		if(foot->text)
		{
			length += start;
			if(!(record->flags & K_RECORD_ROOT_FLAG_JOURNALING))
			{
				size_of = 0;
				for(i = 0; i < foot->element_count; i++)
				{
					member = k_record_struct_member_get(record, handle, i);
					for(j = start; j < length; j++)				
					{
						strings  = (char **)&((uint8 *)handle->array)[j * foot->size_of + size_of];
						for(k = 0; k < member->array_length; k++)
							if(strings[k] != NULL)
								free(strings[k]);
					}
					size_of = ((size_of + k_data_type_alignment[handle->type] - 1) / k_data_type_alignment[handle->type]) * k_data_type_alignment[handle->type];
					size_of += k_data_type_size[handle->type] * handle->array_length;
				}
			}
			memcpy(((uint8 *)handle->array)[size_of * start], values, length * foot->size_of);
			size_of = 0;
			for(i = 0; i < foot->element_count; i++)
			{
				member = k_record_struct_member_get(record, handle, i);
				for(j = start; j < length; j++)				
				{
					strings  = (char **)&((uint8 *)handle->array)[j * foot->size_of + size_of];
					for(k = 0; k < member->array_length; k++)
					{
						if(strings[k] == NULL)
							strings[k] = NULL;
						else
							strings[k] = f_text_copy_allocate(strings[k]);
					}
				}
				size_of = ((size_of + k_data_type_alignment[handle->type] - 1) / k_data_type_alignment[handle->type]) * k_data_type_alignment[handle->type];
				size_of += k_data_type_size[handle->type] * handle->array_length;
			}
			return;
		}	
	}
	memcpy(&((uint8 *)v)[size_of * start], values, length * size_of);
}

void *k_record_entry_array_handle_get(KRecord *record, KHead *handle)
{
	void *v;
	if(handle->array == NULL)
	{
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			v = &((KJournalHead *)handle)[1];
		else
			v = &handle[1];
	}
	else
		v = handle->array;
	return v;
}

boolean k_record_entry_array_handle_copy(KRecord *record, KHead *handle, void *values, uint start, uint length)
{
	void *v;
	uint size_of;
	if(start >= handle->array_length || start + length > handle->array_length || length == 0 || values == NULL || handle->flags & K_RECORD_VALUE_FLAG_LEAF)
		return FALSE;
	if(handle->array == NULL)
	{
		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
			v = &((KJournalHead *)handle)[1];
		else
			v = &handle[1];
	}
	else
		v = handle->array;
	size_of = k_data_type_size[handle->type];
	memcpy(values, &((uint8 *)v)[start * size_of], length * size_of);
	return TRUE;
}

boolean k_record_entry_array_handle_get_copy(KRecord *record, KJournalHead *handle, void *values, uint start, uint length)
{
	if(!k_record_entry_array_handle_copy(record, (KHead *)handle, values, start, length))
		return FALSE;
	if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		KJournalEntry *entry;
		uint i, s, e, size_of;
		if(!record->timestamp_lock)
			imagine_time_epoc_current(&record->timestamp.seconds, &record->timestamp.fractions);
		size_of = k_data_type_size[handle->array.type];
		i = handle->entry_count - 1;
		for(entry = &handle->entry[i]; i < handle->entry_count &&
			(entry->timestamp.seconds > record->timestamp.seconds || 
				(entry->timestamp.seconds == record->timestamp.seconds &&
				entry->timestamp.fractions > record->timestamp.fractions)); entry = &handle->entry[--i])
		{
			if(entry->array != NULL && entry->start + entry->size > start && entry->start < start + length)
			{
				if(start > entry->start)
					s = start;
				else
					s = entry->start;
				if(start + length < entry->start + entry->size)
					e  = start + length;
				else
					e = entry->start + entry->size;
				
				memcpy(&((uint8 *)values)[(s - start) * size_of], &((uint8 *)entry->array)[s * size_of], (e - s) * size_of);
			}
		}
	}
	return TRUE;
}

boolean k_record_entry_journal_print(KRecord *record, KJournalHead *handle)
{
	uint i;
	printf("Record %llu %llu\n", record->timestamp.seconds, record->timestamp.fractions);
	printf("Handle %llu %llu\n", handle->timestamp.seconds, handle->timestamp.fractions);
	for(i = 0; i <handle->entry_count; i++)
	{
		printf("event[%u] %llu %llu %h\n", i, handle->entry[i].timestamp.seconds, handle->entry[i].timestamp.fractions, handle->entry[i].array);

		if(handle->entry[i].array != NULL)
			printf("value %s %p\n", ((char **)handle->entry[i].array)[0], ((char **)handle->entry[i].array)[0]);
	}
}


char *k_record_entry_string_name_get(KRecord *record, char *name, uint id, char *default_value, boolean create)
{
	KHead *handle;
	char *string;
	handle = k_record_entry_lookup_name(record, name, K_VT_STRING, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_STRING, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &string, id, 1))
		return string;
	else
		return default_value;
}

boolean k_record_entry_string_name_set(KRecord *record, char *name, uint id, char *string)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_STRING, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_STRING, name);
	k_record_entry_array_handle_set(record, handle, &string, id, 1);
	return TRUE;
}

char *k_record_entry_string_handle_get(KRecord *record, KHead *handle, uint id)
{
	char *string;
	if(k_record_entry_array_handle_get_copy(record, handle, &string, id, 1))
		return string;
	else
		return NULL;
}

boolean	k_record_entry_string_handle_set(KRecord *record, KHead *handle, uint id, char *string)
{
	if(handle->type != K_VT_STRING)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &string, id, 1);
	return TRUE;
}



void k_generate_record_generate_functions(FILE *file, char *type, char *enum_name)
{

	fprintf(file, "%s k_record_entry_%s_name_get(KRecord *record, char *name, uint id, %s default_value, boolean create)\n", type, type, type);
	fprintf(file, "{\n");
	fprintf(file, "\tKHead *handle;\n");
	fprintf(file, "\t%s value;\n", type);
	fprintf(file, "\thandle = k_record_entry_lookup_name(record, name, %s, TRUE, TRUE);\n", enum_name);
	fprintf(file, "\tif(handle == NULL)\n");
	fprintf(file, "\t{\n");
	fprintf(file, "\t\tif(create)\n");
	fprintf(file, "\t\t{\n");
	fprintf(file, "\t\t\thandle = k_record_entry_add(record, %s, name);\n", enum_name);
	fprintf(file, "\t\t\tk_record_entry_array_handle_set(record, handle, &default_value, 0, 1);\n");
	fprintf(file, "\t\t}else\n");
	fprintf(file, "\t\t\treturn default_value;\n");
	fprintf(file, "\t}\n");
	fprintf(file, "\tif(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))\n");
	fprintf(file, "\t\treturn value;\n");
	fprintf(file, "\telse\n");
	fprintf(file, "\t\treturn default_value;\n");
	fprintf(file, "}\n\n");

	fprintf(file, "boolean k_record_entry_%s_name_set(KRecord *record, char *name, uint id, %s value)\n", type, type);
	fprintf(file, "{\n");
	fprintf(file, "\tKHead *handle;\n");
	fprintf(file, "\thandle = k_record_entry_lookup_name(record, name, %s, TRUE, TRUE);\n", enum_name);
	fprintf(file, "\tif(handle == NULL)\n");
	fprintf(file, "\t\thandle = k_record_entry_add(record, %s, name);\n", enum_name);
	fprintf(file, "\tk_record_entry_array_handle_set(record, handle, &value, id, 1);\n");
	fprintf(file, "\treturn TRUE;\n");
	fprintf(file, "}\n\n");

	fprintf(file, "%s k_record_entry_%s_handle_get(KRecord *record, KHead *handle, uint id)\n", type, type);
	fprintf(file, "{\n");
	fprintf(file, "\t%s value;\n", type);
	fprintf(file, "\tif(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))\n");
	fprintf(file, "\t\treturn value;\n");
	fprintf(file, "\telse\n");
	fprintf(file, "\t\treturn 0;\n");
	fprintf(file, "}\n\n");

	fprintf(file, "boolean	k_record_entry_%s_handle_set(KRecord *record, KHead *handle, uint id, %s value)\n", type, type);
	fprintf(file, "{\n");
	fprintf(file, "\tif(handle->type != %s)\n", enum_name);
	fprintf(file, "\t\treturn FALSE;\n");
	fprintf(file, "\tk_record_entry_array_handle_set(record, handle, &value, id, 1);\n");
	fprintf(file, "\treturn TRUE;\n");
	fprintf(file, "}\n\n");
	
}


void k_generate_record_generate_api()
{
	FILE *f;
	f = fopen("k_type_functions.c", "w");
	
	fprintf(f, "#include \"k_internal.h\"\n\n");
	k_generate_record_generate_functions(f, "int8", "K_VT_INT8");
	k_generate_record_generate_functions(f, "uint8", "K_VT_INT8");
	k_generate_record_generate_functions(f, "int16", "K_VT_INT16");
	k_generate_record_generate_functions(f, "uint16", "K_VT_INT16");
	k_generate_record_generate_functions(f, "int32", "K_VT_INT32");
	k_generate_record_generate_functions(f, "uint32", "K_VT_INT32");
	k_generate_record_generate_functions(f, "int64", "K_VT_INT64");
	k_generate_record_generate_functions(f, "uint64", "K_VT_INT64");
	k_generate_record_generate_functions(f, "float", "K_VT_REAL32");
	k_generate_record_generate_functions(f, "double", "K_VT_REAL64");
	fclose(f);
}

void k_record_serialize(KRecord *record, THandle *handle, boolean journaling);
KRecord *k_record_unserialize(THandle *handle);

extern boolean k_record_entry_int32_name_set(KRecord *record, char *name, uint id, int32 value);
extern int32 k_record_entry_int32_handle_get(KRecord *record, KHead *handle, uint id);
extern int32 k_record_entry_int32_name_get(KRecord *record, char *name, uint id, int32 default_value, boolean create);

void k_record_print(KRecord *record)
{
	KHead *handle, *member;
	void *value;
	uint i, count;
	
	for(value = k_record_entry_next(record, NULL, K_VT_ANY_TYPE, FALSE, FALSE); value != NULL; value = k_record_entry_next(record, value, K_VT_ANY_TYPE, FALSE, FALSE))
	{
		printf("Name %s\n", k_record_entry_name(value));


		printf("\ttype %u\n", (uint32)k_record_entry_type(value));


		if(K_VT_INT32 == k_record_entry_type(value))
			for(i = 1; i < k_record_entry_array_get_size(record, value); i++)
				printf("\tvalue[%u] %u\n", i, k_record_entry_int32_handle_get(record, value, i));

		if(1 != k_record_entry_array(value))
			printf("\tarray length %u\n", (uint32)k_record_entry_array(value));
	//	printf("version %u/n", (uint32)k_record_entry_version(record, value));

		if(k_record_entry_comment_get(value) != NULL)
			printf("\tcomment %s\n", k_record_entry_comment_get(value));

		if(k_record_entry_deleted(record, value))
			printf("\tdeleted\n");
		if(k_record_entry_leaf(value))
			printf("\tleaf\n", (uint32)k_record_entry_leaf(value));

		if(K_VT_ENUM == k_record_entry_type(value))
			for(i = 0; i < k_record_enum_count_get(record, value); i++)
				printf("\t - Enum %s\n", k_record_enum_name_get(record, value, i));

		if(K_VT_STRUCT == k_record_entry_type(value))
		{
			int32 *values; 
			for(i = 0; i < k_record_struct_count_get(record, value); i++)
			{
				member = k_record_struct_member_get(record, value, i);
				printf("\t - Struct member %s\n", k_record_entry_name(member));
				
			}
			values = k_record_entry_array_handle_get(record, value);
			for(i = 0; i < k_record_struct_count_get(record, value); i++)
				printf("\t - Struct list %u\n", values[i]);
		}
		count = k_record_entry_journal_count(record, value);
		if(count != 0)
		{
			KTimestamp timestamp;
			int32 read;
			printf("\tJournal count %u\n", count);
			timestamp.seconds = LLONG_MAX;
			timestamp.fractions = -1;
			while(k_record_entry_journal_next(record, value, &timestamp))
			{
				printf("\tJournal time %u %u\n", (int)timestamp.seconds, timestamp.fractions);
				k_record_timestamp_lock_specific(record, timestamp.seconds, timestamp.fractions);
				if(k_record_entry_array_handle_get_copy(record, value, &read, 0, 1))			
					printf("\tJournal value %u\n", read);
				else
					printf("\tJournal read fail\n");
			}
			k_record_timestamp_unlock(record);
		}
	}
}


void k_fuzz_test(uint8 *data, uint size)
{
	KRecord *record;
	THandle *handle;
	uint i, j, count = 0;
	uint8 *copy, save;


	printf("Data set :");
	for(i = 0; i < size; i++)
		printf(" %u", (uint)data[i]);
	printf("\n");

	printf("Sanity testing :");
	
	handle = testify_buffer_create();
	testify_buffer_set(handle, data, size);
	f_debug_mem_reset();
	record = k_record_unserialize(handle);
	k_record_free(record);
	if(f_debug_mem_consumption() != 0)
	{
		f_debug_mem_print(0); 
		printf("Sanity test failed\n");
		return;
	}
	testify_free(handle);
	printf("\nLength testing :");
	for(i = 0; i < size; i++)
	{
		printf(" %u", i);
		handle = testify_buffer_create();
		testify_buffer_set(handle, data, i);
		f_debug_mem_reset();
		if(NULL != k_record_unserialize(handle))
			printf("E");
		if(f_debug_mem_consumption() != 0)
		{
			f_debug_mem_print(0); 
			printf("Length test failed\n");
			return;
		}
		testify_free(handle);
	}
	printf("\n");
	copy = malloc(size);
	printf("Fuzz testing :");
	for(i = 0; i < size; i++)
	{
		for(j = 0; j < size; j++)
			copy[j] = data[j];
		for(j = 0; j < 256; j++)
		{
			copy[i] = j;
			handle = testify_buffer_create();
			testify_buffer_set(handle, data, i);
			f_debug_mem_reset();
			record = k_record_unserialize(handle);
			if(NULL != record)
				k_record_free(record);
			if(f_debug_mem_consumption() != 0)
			{
				f_debug_mem_print(0); 
				printf("Fuzz testing failed\n");
				return;
			} 
			testify_free(handle);
		}
		printf(" %u", i);
	}
	printf("\n");
}

void k_record_test()
{
	KRecord *record;
	KHead *member;
	void *value, *data;
	char *enums[3] = {"Hey", "Hello", "Goodbye"};
	char *members[3] = {"X", "Y", "Z"};
	uint i, j, count;
	THandle *handle, *handle2;
	uint32 size; 
	k_generate_record_generate_api();
	record = k_record_create(1, /*K_JM_NOT_JOURNALED*/ K_JM_JOURNALED_PRECISSION_LOW);

/*	k_record_timestamp_lock_specific(record, 1 * 600, 0);
	k_record_entry_int32_name_set(record, "A", 0, 1);
	k_record_timestamp_lock_specific(record, 2 * 600, 0);
	k_record_entry_int32_name_set(record, "A", 0, 2);
	k_record_timestamp_lock_specific(record, 3 * 600, 0);
	k_record_entry_int32_name_set(record, "A", 0, 3);
	k_record_timestamp_lock_specific(record, 4 * 600, 0);
	k_record_entry_int32_name_set(record, "A", 0, 4);
	k_record_timestamp_lock_specific(record, 5 * 600, 0);
	k_record_entry_int32_name_set(record, "A", 0, 5);
	k_record_timestamp_unlock(record);
	k_record_entry_int32_name_set(record, "B", 0, 3);
	k_record_entry_int32_name_get(record, "B", 0, 2, TRUE);
	for(i = 0; i < 1000; i++)
		printf("n");
	k_record_entry_int32_name_set(record, "B", 0, 3);
	k_record_entry_int32_name_set(record, "C", 0, 3);
	k_record_entry_int32_name_get(record, "C", 0, 1, TRUE);
		
	handle = k_record_entry_lookup_name(record, "C", K_VT_INT32, TRUE, TRUE);
	 k_record_entry_journal_print(record, handle);
	k_record_entry_array_set_size(record, handle, 10);
	for(i = 0 ; i < 10; i++)
		k_record_entry_int32_handle_set(record, handle, i, i + 1);
	 k_record_entry_journal_print(record, handle);
	*/
	typedef struct{
		char *x;
		int32 y;
		int32 z;
	}Name;

	k_record_entry_string_name_set(record, "X", 0, "Hey");
	k_record_entry_int32_name_set(record, "Y", 0, 10);
	k_record_entry_int32_name_set(record, "Z", 0, 10);
	k_record_struct_add(record, "struct", 3, members);

	handle = k_record_entry_lookup_name(record, "X", K_VT_STRING, TRUE, TRUE);
	k_record_entry_journal_print(record, handle);

	k_record_entry_string_name_set(record, "X", 0, "Hello");

	k_record_entry_journal_print(record, handle);

	k_record_entry_string_name_set(record, "X", 0, "Hi");

	k_record_entry_journal_print(record, handle);

/*
	k_record_enum_add(record, "Enum", 3, enums, 2);
	k_record_entry_int32_name_set(record, "X", 0, 10);
	k_record_entry_int32_name_set(record, "Y", 0, 20);
	k_record_entry_int32_name_set(record, "Z", 0, 30);
	k_record_struct_add(record, "struct", 3, members);
	k_record_entry_int32_name_set(record, "X", 0, 30);
	k_record_entry_int32_name_set(record, "Y", 0, 20);
	k_record_entry_int32_name_set(record, "Z", 0, 10);
	value = k_record_entry_lookup_name(record, "B", K_VT_ANY_TYPE, TRUE, TRUE);*/
//	if(value != NULL)
//		k_record_entry_comment_set(value, "This is my comment");

/*	k_record_print(record);
	handle = testify_buffer_create();
	testify_debug_mode_set(handle, TRUE, "record_debug_output.txt");
	k_record_serialize(record, handle, TRUE);
		testify_free(handle);
		exit(0);
	record = k_record_unserialize(handle);
	testify_free(handle);*/
	
	//	testify_debug_mode_set(handle, FALSE, "record_debug_output.txt");
//	handle = testify_file_save("test.tmp");

//	testify_free(handle);
//	exit(0);
	
//	testify_debug_mode_set(handle2, TRUE, "record_debug_output2.txt");


	handle = testify_buffer_create();
	k_record_serialize(record, handle, TRUE);
	data = testify_buffer_get(handle, &size);
	k_fuzz_test(data, size);
/*	testify_buffer_set(handle2, data, size);
	record = k_record_unserialize(handle);*/
//	testify_free(handle);
//	handle = testify_file_load("test.tmp");
	testify_free(handle);
	{
		char buffer[1024];
		FILE *f;
		f = fopen("print_test.html", "w");
		k_type_enum_si_unit_name_print(buffer, 12);
		fprintf(f, "<!DOCTYPE html>\n");
		fprintf(f, "<html>\n");
		fprintf(f, "<head>\n");
		fprintf(f, "<meta charset=\"UTF-8\">\n");
		fprintf(f, "</head> \n");
		fprintf(f, "<body>\n");
		fprintf(f, "unit = %s\n", buffer);
		fprintf(f, "</body>\n");
		fprintf(f, "</html>\n");
		fclose(f);
	}
	printf("\n-------------------------------------------------------\n");
	k_record_print(record);
	printf("END");
	{
		uint64 seconds;
		uint64 fractions, last = 0;
		while(TRUE)
		{
			imagine_time_epoc_current(&seconds, &fractions);
			if(last > fractions)
			{
				printf("Time %llu ", seconds);
				printf("%llu\n", last);
			}
			last = fractions;
		}
	}
}
/*
int32	k_record_entry_int32_id_get(KRecord *record, uint64 id);
boolean	k_record_entry_int32_id_set(KRecord *record, uint64 id, uint32 value);
int32	k_record_entry_int32_name_get(KRecord *record, char *name, int32 default_value, boolean create);
boolean	k_record_entry_int32_name_set(KRecord *record, char *name, uint32 value);
int32	k_record_entry_int32_handle_get(KRecord *record, KValue *handle);
boolean	k_record_entry_int32_handle_set(KRecord *record, KValue *handle, uint32 value);
*/


