
#include "k_internal.h"


void k_record_serialize_timestamp(THandle *handle, KTimestamp *timestamps, uint64 count, uint precission_flags)
{	
	int32 time32;
	int64 i, minutes;
	uint64 fractions;
	switch(precission_flags)
	{
		case 1 :
			for(i = 0; i < count; i++)
			{
				minutes = timestamps[i].seconds / 60;
				if(minutes > INT_MAX)
					minutes = INT_MAX;
				else if(minutes < INT_MIN)
					minutes = INT_MIN;
				testify_pack_int32(handle, minutes, "time1");
			}	
			return;
		case 2 :
			for(i = 0; i < count; i++)
			{
				minutes = timestamps[i].seconds / 60;
				fractions = timestamps[i].fractions / 60 + (timestamps[i].seconds % 60) * (ULLONG_MAX / 60);
				if(minutes > INT_MAX)
				{
					minutes = INT_MAX;
					fractions = UINT_MAX;
				}else if(minutes < INT_MIN)
				{
					minutes = INT_MIN;
					fractions = UINT_MAX;
				}
				testify_pack_int32(handle, minutes, "minutes2");
				testify_pack_uint32(handle, fractions / UINT_MAX, "fractions2");
			}	
			return;
		case 3 :
			for(i = 0; i < count; i++)
			{
				testify_pack_int64(handle, timestamps[i].seconds, "seconds3");
				testify_pack_uint64(handle, timestamps[i].fractions, "fractions3");
			}	
			return;
	}
}


boolean k_record_unserialize_timestamp(THandle *handle, KTimestamp *timestamps, uint64 count, uint precission_flags)
{	
	int32 time32;
	uint64 i, minutes, fractions;
	switch(precission_flags)
	{
		case 1 :

			if(!testify_retivable(handle, count * 4))
				return FALSE;
			for(i = 0; i < count; i++)
			{
				timestamps[i].seconds = testify_unpack_int32(handle, "time1") * 60;
				timestamps[i].seconds *= 60;
				timestamps[i].fractions = 0;
			}	
		return TRUE;
		case 2 :
			if(!testify_retivable(handle, count * 8))
				return FALSE;
			for(i = 0; i < count; i++)
			{
				timestamps[i].seconds = testify_unpack_int32(handle, "minutes2");
				timestamps[i].seconds *= 60;
				timestamps[i].fractions = testify_unpack_uint32(handle, "fractions2");
				timestamps[i].seconds += timestamps[i].fractions / (UINT_MAX / 60);
				timestamps[i].fractions = timestamps[i].fractions % (UINT_MAX / 60);
				timestamps[i].fractions = timestamps[i].fractions * UINT_MAX * 60;
			}	
		return TRUE;
		case 3 :
			if(!testify_retivable(handle, count * 16))
				return FALSE;
			for(i = 0; i < count; i++)
			{
				timestamps[i].seconds = testify_unpack_int64(handle, "seconds3");
				timestamps[i].fractions = testify_unpack_uint64(handle, "fractions3");
			}	
		return TRUE;
	}
	return FALSE;
}


void  k_record_serialize_type(THandle *handle, void *data, uint count, uint type)
{
	uint i, output;
	switch(type)
	{
		case K_VT_INT8 :
		case K_VT_UINT8 :
			for(i = 0; i < count; i++)
				testify_pack_uint8(handle, *((uint8 *)data)++, "data");
			return;
		case K_VT_INT16 :
		case K_VT_UINT16 :
		case K_VT_ENUM :
			for(i = 0; i < count; i++)
				testify_pack_uint16(handle, *((uint16 *)data)++, "data");
			return;
		case K_VT_USER :  /* Lets think really hard about what we wan tto do with this */
		case K_VT_INT64 :
		case K_VT_UINT64 :
		case K_VT_REAL64 :
			for(i = 0; i < count; i++)
				testify_pack_uint64(handle, *((uint64 *)data)++, "data");
			return;
		case K_VT_INT32 :
		case K_VT_UINT32 :
		case K_VT_REAL32 :
			for(i = 0; i < count; i++)
				testify_pack_uint32(handle, *((uint32 *)data)++, "data");
			return;
		case K_VT_TIMESTAMP_LOW :
			k_record_serialize_timestamp(handle, (KTimestamp *)data, count, 1);
			return;
		case K_VT_TIMESTAMP_MID :
			k_record_serialize_timestamp(handle, (KTimestamp *)data, count, 2);
			return;
		case K_VT_TIMESTAMP_HIGH :
			k_record_serialize_timestamp(handle, (KTimestamp *)data, count, 3);
			return;
		case K_VT_ID :
			count *= KEEP_ID_SIZE_8_BIT;
			for(i = 0; i < count; i++)
				testify_pack_uint8(handle, *((uint8 *)data)++, "data");
		return count;
		case K_VT_STRING :
			output = 0;
			for(i = 0; i < count; i++)
				testify_pack_string(handle, ((char **)data)[i], "data");
			return;
	}
}


boolean k_record_unserialize_type(THandle *handle, void *data, uint count, uint type)
{
	boolean broken;
	uint i, output;
	switch(type)
	{
		case K_VT_INT8 :
		case K_VT_UINT8 :
			if(!testify_retivable(handle, count))
				return FALSE;
			for(i = 0; i < count; i++)
				((uint8 *)data)[i] = testify_unpack_uint8(handle, "data");
			return TRUE;
		case K_VT_INT16 :
		case K_VT_UINT16 :
		case K_VT_ENUM :
			if(!testify_retivable(handle, count * 2))
				return FALSE;
			for(i = 0; i < count; i++)
				((uint16 *)data)[i] = testify_unpack_uint16(handle, "data");
			return TRUE;
		case K_VT_USER :  /* Lets think really hard about what we wan tto do with this */
		case K_VT_INT64 :
		case K_VT_UINT64 :
		case K_VT_REAL64 :
			if(!testify_retivable(handle, count * 8))
				return FALSE;
			for(i = 0; i < count; i++)
				((uint64 *)data)[i] = testify_unpack_uint64(handle, "data");
			return TRUE;
		case K_VT_INT32 :
		case K_VT_UINT32 :
		case K_VT_REAL32 :
			if(!testify_retivable(handle, count * 4))
				return FALSE;
			for(i = 0; i < count; i++)
				((uint32 *)data)[i] = testify_unpack_uint32(handle, "data");
			return TRUE;
		case K_VT_TIMESTAMP_LOW :
			return k_record_unserialize_timestamp(handle, (KTimestamp *)data, count, 1);
		case K_VT_TIMESTAMP_MID :
			return k_record_unserialize_timestamp(handle, (KTimestamp *)data, count, 2);
			return;
		case K_VT_TIMESTAMP_HIGH :
			return k_record_unserialize_timestamp(handle, (KTimestamp *)data, count, 3);
		case K_VT_ID :
			if(!testify_retivable(handle, count * 2))
				return FALSE;
			count *= KEEP_ID_SIZE_8_BIT;
			for(i = 0; i < count; i++)
				((uint8 *)data)[i] = testify_unpack_uint8(handle, "data");
			return TRUE;
		case K_VT_STRING :
			for(i = 0; i < count; i++)
			{
				if(testify_retivable_terminated(handle))
					((char **)data)[i] = testify_unpack_string_allocate(handle, "data");
				else
				{
					while(i < count)
						((char **)data)[i++] = NULL;
					return FALSE;
				}
			}
			return TRUE;
	}
}

void k_record_serialize_struct(KRecord *record, KFootStruct *foot, THandle *handle, uint8 *data, uint64 count)
{
	KHead *element, *entry;
	uint64 i, j, start, size_of = 0, offset;
	int32 *d, a, b, c;


	testify_pack_uint8(handle, foot->element_count, "struct count");
	for(i = start = 0; i < foot->element_count; i++)
	{
		element = &record->allocation[foot->elements[i]];
		j = 0;
		for(entry = k_record_entry_next(record, NULL, K_VT_ANY_TYPE, TRUE, FALSE); entry != element; entry = k_record_entry_next(record, entry, K_VT_ANY_TYPE, TRUE, FALSE))
			j++;
		testify_pack_uint16(handle, j, "struct");
	}
	d = data;
	a = d[0];
	b = d[1];
	c = d[2];

	for(i = start = offset = 0; i < foot->element_count; i++)
	{
		element = &record->allocation[foot->elements[i]];
		offset = ((offset + k_data_type_alignment[element->type] - 1) / k_data_type_alignment[element->type]) * k_data_type_alignment[element->type];
		for(j = 0; j < count; j++)
			k_record_serialize_type(handle, &data[j * foot->size_of + offset], element->array_length, element->type);
		offset += k_data_type_size[element->type] * element->array_length;	
	}
}

boolean k_record_unserialize_struct(KRecord *record, KFootStruct *foot, THandle *handle, uint8 *data, uint64 count)
{
	KHead *element, *entry;
	uint64 i, j, start, offset = 0;

	foot->element_count = testify_unpack_uint8(handle, "struct count");
	foot->elements = malloc((sizeof *foot->elements) * foot->element_count);
	for(i = start = 0; i < foot->element_count; i++)
	{

		foot->elements[i] = testify_unpack_uint16(handle, "struct");
		element = &record->allocation[foot->elements[i]];
		j = 0;
		for(entry = k_record_entry_next(record, NULL, K_VT_ANY_TYPE,TRUE, FALSE); entry != NULL && j < foot->elements[i]; entry = k_record_entry_next(record, entry, K_VT_ANY_TYPE,TRUE, FALSE))
			j++;
		if(entry == NULL)
		{
			free(foot->elements);
			foot->elements = NULL;
			return FALSE;
		}
		foot->elements[i] = ((uint8 *)entry) - record->allocation;
	}


	for(i = start = 0; i < foot->element_count; i++)
	{
		element = &record->allocation[foot->elements[i]];
		offset = ((offset + k_data_type_alignment[element->type] - 1) / k_data_type_alignment[element->type]) * k_data_type_alignment[element->type];	
		for(j = 0; j < count; j++)
			k_record_unserialize_type(handle, &data[offset + j * foot->size_of], element->array_length, element->type);
		offset += k_data_type_size[element->type] * element->array_length;	
	}
	return TRUE;
}

uint k_record_serialize_size_select(uint64 size, boolean bit16)
{
	if(bit16)
	{
		if(size < 256 * 256)
		{
			if(size < 256)
				return 0;
			else
				return 1;
		}
	}else
	{ 
		if(size == 1)
			return 0;
		if(size < 256)
			return 1;
	}
	if(size < UINT_MAX)
		return 2;
	else 
		return 3;
}

void k_record_serialize_array_size(THandle *handle, uint64 size, uint16 flag, boolean bit16)
{
	if(bit16)
	{
		if(flag > 1)
		{
			if(flag == 2)	
				testify_pack_uint32(handle, size, "size16");
			else
				testify_pack_uint64(handle, size, "size16");
			return;
		}else
		{
			if(flag == 1)
				testify_pack_uint16(handle, size, "size16");
			else
				testify_pack_uint8(handle, size, "size16");
		}
	}else
	{
		if(flag > 1)
		{
			if(flag == 2)	
				testify_pack_uint32(handle, size, "size");
			else
				testify_pack_uint64(handle, size, "size");
		}else if(flag == 1)
			testify_pack_uint8(handle, size, "size");
	}
}

boolean k_record_unserialize_array_size(THandle *handle, uint16 flag, uint64 *size, boolean bit16)
{
	if(bit16)
	{
		switch(flag)
		{
			case  0 : 
				if(!testify_retivable(handle, 1))
					return FALSE;
				*size = (uint64)testify_unpack_uint8(handle, "size16");
			return TRUE;
			case  1 : 
				if(!testify_retivable(handle, 2))
				return FALSE;
				*size = (uint64)testify_unpack_uint16(handle, "size16");
			return TRUE;
			case  2 : 
				if(!testify_retivable(handle, 4))
				return FALSE;
				*size = (uint64)testify_unpack_uint32(handle, "size16");
			return TRUE;
			case  3 : 
				if(!testify_retivable(handle, 8))
				return FALSE;
				*size = (uint64)testify_unpack_uint64(handle, "size16");
			return TRUE;
		}
	}
	switch(flag)
	{
		case  0 : 
			*size = 1;
		return TRUE;
		case  1 : 
			if(!testify_retivable(handle, 2))
			return FALSE;
			*size = (uint64)testify_unpack_uint8(handle, "size");
		return TRUE;
		case  2 : 
			if(!testify_retivable(handle, 4))
			return FALSE;
			*size = (uint64)testify_unpack_uint32(handle, "size");
		return TRUE;
		case  3 : 
			if(!testify_retivable(handle, 8))
			return FALSE;
			*size = (uint64)testify_unpack_uint64(handle, "size");
		return TRUE;
	}
}

void k_record_serialize(KRecord *record, THandle *handle, boolean journaling)
{
	size_t pos= 0;
	uint i, j;
	uint64 size, s; 
	uint16 flags, size_flags, journal_entry_flags = 0, journal_time_precission_flags = 0, member_count_flag;
	KHead *values;
	KJournalHead *jvalues;

	if(journaling && record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
	{
		journal_time_precission_flags = (record->flags & K_RECORD_ROOT_FLAG_TIME_STAMP_PRECISSION) / 4;
		size = 1;
		for(jvalues = k_record_entry_next(record, NULL, K_VT_ANY_TYPE, TRUE, TRUE); jvalues != NULL; jvalues = k_record_entry_next(record, jvalues, K_VT_ANY_TYPE, TRUE, TRUE))
			if(size < jvalues->entry_count)
				size = jvalues->entry_count;
		journal_entry_flags = k_record_serialize_size_select(size, FALSE);
	}else
		journaling = FALSE;
	i = 0;
	for(values = k_record_entry_next(record, NULL, K_VT_ANY_TYPE, TRUE, journaling); values != NULL; values = k_record_entry_next(record, values, K_VT_ANY_TYPE, TRUE, journaling))
		i++;
	member_count_flag = k_record_serialize_size_select(i, TRUE);
	testify_pack_uint8(handle, 16 * member_count_flag + 4 * journal_time_precission_flags + journal_entry_flags, "flags");
	if(journaling)
	{
		k_record_serialize_timestamp(handle, &record->timestamp, 1, journal_time_precission_flags);
	}
	k_record_serialize_array_size(handle, i, member_count_flag, TRUE);

	for(values = k_record_entry_next(record, NULL, K_VT_ANY_TYPE, TRUE, journaling); values != NULL; values = k_record_entry_next(record, values, K_VT_ANY_TYPE, TRUE, journaling))
	{
		flags = 0;
		size = values->array_length;
		if(journaling)
		{
			jvalues = (KJournalHead *)values;
			for(i = 0; i < jvalues->entry_count; i++)
			{
				s = jvalues->entry[i].start + jvalues->entry[i].size;
				if(s > size)
					size = s;
			}
		}
		flags = size_flags = k_record_serialize_size_select(size, FALSE);

		if(values->comment != NULL)
			flags |= K_RECORD_VALUE_FLAG_COMMENT;/* does the value have acomment describing it */
		flags |= values->flags & K_RECORD_VALUE_FLAG_LEAF; /* is the thing a part of a struct*/
		testify_pack_uint8(handle, flags, "member_flags");
		testify_pack_uint8(handle, values->type, "member_type");
		if(values->comment != NULL)
			testify_pack_string(handle, values->comment, "comment");
		testify_pack_string(handle, values->name, "name");
		k_record_serialize_array_size(handle, values->array_length, size_flags, FALSE);

			
		if(values->type == K_VT_STRUCT)
		{
			KFootStruct *foot;
			if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
				foot = (KFootStruct *)&((KJournalHead *)values)[1];
			else
				foot = (KFootStruct *)&values[1];
			k_record_serialize_struct(record, foot, handle, values->array, values->array_length);
		}else if(values->type == K_VT_ENUM)
		{
			KFootEnum *foot;
			void **value;
			if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
				value = (void **)&((KJournalHead *)values)[1];
			else
				value = (void **)&values[1];
			foot = (KFootEnum *)&value[1];
			if(!(flags & K_RECORD_VALUE_FLAG_LEAF))
				k_record_serialize_type(handle, k_record_entry_array_handle_get(record, values), values->array_length, values->type);
			testify_pack_uint8(handle, foot->element_count, "element_count");
			for(j = 0; j < foot->element_count; j++)
				testify_pack_string(handle, foot->elements[j], "enum");

		}else if(!(flags & K_RECORD_VALUE_FLAG_LEAF))
			k_record_serialize_type(handle, k_record_entry_array_handle_get(record, values), values->array_length, values->type);

		if(journaling)
		{	

			k_record_serialize_timestamp(handle, &jvalues->timestamp, 1, journal_time_precission_flags);
			k_record_serialize_array_size(handle, jvalues->entry_count, journal_entry_flags, TRUE);
			for(i = 0; i < jvalues->entry_count; i++)
			{
					
				k_record_serialize_array_size(handle, jvalues->entry[i].start + 1, size_flags, FALSE);				
				k_record_serialize_array_size(handle, jvalues->entry[i].size, size_flags, FALSE);
				k_record_serialize_timestamp(handle, &jvalues->entry[i].timestamp, 1, journal_time_precission_flags);
				if(jvalues->entry[i].array == NULL)
					testify_pack_uint8(handle, TRUE, "NULL");
				else
				{
					testify_pack_uint8(handle, FALSE, "NULL");
					if(values->type == K_VT_STRUCT)
					{
						k_record_serialize_struct(record, (KFootStruct *)&((KJournalHead *)values)[1], handle, jvalues->entry[i].array, jvalues->entry[i].size);
					}else
						k_record_serialize_type(handle, jvalues->entry[i].array, jvalues->entry[i].size, values->type);
				}
			}	
		}

	}
}

KRecord *k_record_unserialize(THandle *handle)
{
	KRecord *record;
	size_t pos= 0;
	uint i, j, k;
	uint64 size, s, member_count, entry_count; 
	uint8 flags, member_flags, size_flags, journal_entry_flags = 0, journal_time_precission_flags = 0, member_count_flags = 0, type;
	KHead *values;
	KJournalHead *jvalues;
	KTimestamp timestamps;
	if(!testify_retivable(handle, 4))
		return NULL;
	flags = testify_unpack_uint8(handle, "flags");
	
	member_count_flags = (flags & K_RECORD_ROOT_MEMBER_COUNT) / 16;
	journal_time_precission_flags = (flags & K_RECORD_ROOT_FLAG_TIME_STAMP_PRECISSION) / 4;
	journal_entry_flags = (flags & K_RECORD_ROOT_FLAG_JOURNALING);
	if(journal_entry_flags)
	{
		if(!k_record_unserialize_timestamp(handle, &timestamps, 1, journal_time_precission_flags))
			return NULL;
	}
	if(!k_record_unserialize_array_size(handle, member_count_flags % 4, &member_count, TRUE))
		return NULL;
	record = k_record_create(1, journal_time_precission_flags);
	if(journal_entry_flags)
		record->timestamp = timestamps;
	record->flags = flags;

	for(i = 0; i < member_count; i++)
	{
		
		if(!testify_retivable(handle, 4))
		{
			k_record_free(record);
			return NULL;
		}
		member_flags = testify_unpack_uint8(handle, "member_flags");
		type = testify_unpack_uint8(handle, "member_type");
		values = k_record_entry_add(record, type, "");
		values->flags = member_flags;
		if(member_flags & K_RECORD_VALUE_FLAG_COMMENT)
			values->comment = testify_unpack_string_allocate(handle, "comment");
		if(!testify_unpack_string(handle, values->name, K_VALUE_NANE_BUFFER_SIZE, "name") ||
			!k_record_unserialize_array_size(handle, member_flags % 4, &values->array_length, FALSE))
		{
			k_record_free(record);
			return NULL;
		}
		if(type == K_VT_STRUCT)
		{
			KFootStruct *foot;
			if(flags & K_RECORD_ROOT_FLAG_JOURNALING)
				foot = (KFootStruct *)&((KJournalHead *)values)[1];
			else
				foot = (KFootStruct *)&values[1];

			values->array = malloc(k_data_type_size[type] * values->array_length);
			if(!k_record_unserialize_struct(record, foot, handle, values->array, values->array_length))
			{
				k_record_free(record);
				return NULL;
			}
		}else if(values->type == K_VT_ENUM)
		{
			KFootEnum *foot;
			void **value;
			if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
				value = (void **)&((KJournalHead *)values)[1];
			else
				value = (void **)&values[1];
			foot = (KFootEnum *)&value[1];
			
			values->array = NULL;
			foot->element_count = 0;
			foot->elements = NULL;
			if(!(member_flags & K_RECORD_VALUE_FLAG_LEAF))
			{
				if(values->array_length > 1)
					values->array = malloc(k_data_type_size[type] * values->array_length);
				if(!k_record_unserialize_type(handle, k_record_entry_array_handle_get(record, values), values->array_length, type))
				{
					k_record_free(record);
					return NULL;
				}
			}

			if(!testify_retivable(handle, 1))
			{
				k_record_free(record);
				return NULL;
			}
			foot->element_count = testify_unpack_uint8(handle, "element_count");
			foot->elements = malloc((sizeof *foot->elements) * foot->element_count);
			for(j = 0; j < foot->element_count; j++)
				foot->elements[j] = NULL;
			for(j = 0; j < foot->element_count; j++)
			{
				if(!testify_retivable_terminated(handle))
				{
					k_record_free(record);
					return NULL;
				}
				foot->elements[j] = testify_unpack_string_allocate(handle, "enum");
			}
		}else if(!(member_flags & K_RECORD_VALUE_FLAG_LEAF))
		{
			if(values->array_length > 1)
			{
				values->array = malloc(k_data_type_size[type] * values->array_length);
				if(type == K_VT_STRING)
					for(j = 0; j < values->array_length; j++)
						((char **)values->array)[j] = NULL;
			}else if(type == K_VT_STRING)
				;
			if(!k_record_unserialize_type(handle, k_record_entry_array_handle_get(record, values), values->array_length, type))
			{
				k_record_free(record);
				return NULL;
			}
		}

		if(record->flags & K_RECORD_ROOT_FLAG_JOURNALING)
		{			

			jvalues = values;
			jvalues->entry_count = 0;
			if(!k_record_unserialize_timestamp(handle, &jvalues->timestamp, 1, journal_time_precission_flags) || 
				!k_record_unserialize_array_size(handle, journal_entry_flags, &entry_count, TRUE))
			{
				k_record_free(record);
				return NULL;
			}

			if(entry_count == 0)
				jvalues->entry = NULL;
			else
			{
				jvalues->entry = malloc((sizeof *jvalues->entry) * (((entry_count + KEEP_JOURNALING_ALLOCATION_SIZE - 1) / KEEP_JOURNALING_ALLOCATION_SIZE) * KEEP_JOURNALING_ALLOCATION_SIZE));
				for(j = 0; j < entry_count; j++)
				{
			/*		if(!k_record_unserialize_array_size(handle, member_flags % 4, &jvalues->entry[j].start, FALSE) ||
						!k_record_unserialize_array_size(handle, member_flags % 4, &jvalues->entry[j].size, FALSE) ||
						!k_record_unserialize_timestamp(handle, &jvalues->entry[j].timestamp, 1, journal_time_precission_flags) ||
						!testify_retivable(handle, 1))
					{
						k_record_free(record);
						return NULL;
					}*/


					if(!k_record_unserialize_array_size(handle, member_flags % 4, &jvalues->entry[j].start, FALSE))
					{
						k_record_free(record);
						return NULL;
					}

					if(!k_record_unserialize_array_size(handle, member_flags % 4, &jvalues->entry[j].size, FALSE))
					{
						k_record_free(record);
						return NULL;
					}

					if(!k_record_unserialize_timestamp(handle, &jvalues->entry[j].timestamp, 1, journal_time_precission_flags))
					{
						k_record_free(record);
						return NULL;
					}

					if(!testify_retivable(handle, 1))
					{
						k_record_free(record);
						return NULL;
					}


					jvalues->entry[j].start -= 1;
					jvalues->entry[j].array = NULL;
					jvalues->entry_count++;
					if(!testify_unpack_uint8(handle, "NULL"))
					{
						if(values->type == K_VT_STRUCT)
						{
							KFootStruct *foot;
							foot = (KFootStruct *)&jvalues[1];
							jvalues->entry[j].array = malloc(jvalues->entry[j].size * foot->size_of);
							if(!k_record_unserialize_struct(record, foot, handle, jvalues->entry[j].array, jvalues->entry[j].size))
							{
								k_record_free(record);
								return NULL;
							}

						}else
						{
							jvalues->entry[j].array = malloc(jvalues->entry[j].size * k_data_type_size[type]);
							if(!k_record_unserialize_type(handle, jvalues->entry[j].array, jvalues->entry[j].size, values->type))
							{
								k_record_free(record);
								return NULL;
							}
						}
					}
				}	
			}
		}
	}
	return record;
}
