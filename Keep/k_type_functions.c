#include "k_internal.h"

int8 k_record_entry_int8_name_get(KRecord *record, char *name, uint id, int8 default_value, boolean create)
{
	KHead *handle;
	int8 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT8, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT8, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_int8_name_set(KRecord *record, char *name, uint id, int8 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT8, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT8, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

int8 k_record_entry_int8_handle_get(KRecord *record, KHead *handle, uint id)
{
	int8 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_int8_handle_set(KRecord *record, KHead *handle, uint id, int8 value)
{
	if(handle->type != K_VT_INT8)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint8 k_record_entry_uint8_name_get(KRecord *record, char *name, uint id, uint8 default_value, boolean create)
{
	KHead *handle;
	uint8 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT8, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT8, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_uint8_name_set(KRecord *record, char *name, uint id, uint8 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT8, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT8, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint8 k_record_entry_uint8_handle_get(KRecord *record, KHead *handle, uint id)
{
	uint8 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_uint8_handle_set(KRecord *record, KHead *handle, uint id, uint8 value)
{
	if(handle->type != K_VT_INT8)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

int16 k_record_entry_int16_name_get(KRecord *record, char *name, uint id, int16 default_value, boolean create)
{
	KHead *handle;
	int16 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT16, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT16, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_int16_name_set(KRecord *record, char *name, uint id, int16 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT16, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT16, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

int16 k_record_entry_int16_handle_get(KRecord *record, KHead *handle, uint id)
{
	int16 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_int16_handle_set(KRecord *record, KHead *handle, uint id, int16 value)
{
	if(handle->type != K_VT_INT16)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint16 k_record_entry_uint16_name_get(KRecord *record, char *name, uint id, uint16 default_value, boolean create)
{
	KHead *handle;
	uint16 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT16, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT16, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_uint16_name_set(KRecord *record, char *name, uint id, uint16 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT16, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT16, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint16 k_record_entry_uint16_handle_get(KRecord *record, KHead *handle, uint id)
{
	uint16 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_uint16_handle_set(KRecord *record, KHead *handle, uint id, uint16 value)
{
	if(handle->type != K_VT_INT16)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

int32 k_record_entry_int32_name_get(KRecord *record, char *name, uint id, int32 default_value, boolean create)
{
	KHead *handle;
	int32 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT32, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT32, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_int32_name_set(KRecord *record, char *name, uint id, int32 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT32, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT32, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

int32 k_record_entry_int32_handle_get(KRecord *record, KHead *handle, uint id)
{
	int32 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_int32_handle_set(KRecord *record, KHead *handle, uint id, int32 value)
{
	if(handle->type != K_VT_INT32)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint32 k_record_entry_uint32_name_get(KRecord *record, char *name, uint id, uint32 default_value, boolean create)
{
	KHead *handle;
	uint32 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT32, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT32, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_uint32_name_set(KRecord *record, char *name, uint id, uint32 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT32, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT32, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint32 k_record_entry_uint32_handle_get(KRecord *record, KHead *handle, uint id)
{
	uint32 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_uint32_handle_set(KRecord *record, KHead *handle, uint id, uint32 value)
{
	if(handle->type != K_VT_INT32)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

int64 k_record_entry_int64_name_get(KRecord *record, char *name, uint id, int64 default_value, boolean create)
{
	KHead *handle;
	int64 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT64, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT64, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_int64_name_set(KRecord *record, char *name, uint id, int64 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT64, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT64, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

int64 k_record_entry_int64_handle_get(KRecord *record, KHead *handle, uint id)
{
	int64 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_int64_handle_set(KRecord *record, KHead *handle, uint id, int64 value)
{
	if(handle->type != K_VT_INT64)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint64 k_record_entry_uint64_name_get(KRecord *record, char *name, uint id, uint64 default_value, boolean create)
{
	KHead *handle;
	uint64 value;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT64, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_INT64, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_uint64_name_set(KRecord *record, char *name, uint id, uint64 value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_INT64, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_INT64, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

uint64 k_record_entry_uint64_handle_get(KRecord *record, KHead *handle, uint id)
{
	uint64 value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_uint64_handle_set(KRecord *record, KHead *handle, uint id, uint64 value)
{
	if(handle->type != K_VT_INT64)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

float k_record_entry_float_name_get(KRecord *record, char *name, uint id, float default_value, boolean create)
{
	KHead *handle;
	float value;
	handle = k_record_entry_lookup_name(record, name, K_VT_REAL32, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_REAL32, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_float_name_set(KRecord *record, char *name, uint id, float value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_REAL32, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_REAL32, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

float k_record_entry_float_handle_get(KRecord *record, KHead *handle, uint id)
{
	float value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_float_handle_set(KRecord *record, KHead *handle, uint id, float value)
{
	if(handle->type != K_VT_REAL32)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

double k_record_entry_double_name_get(KRecord *record, char *name, uint id, double default_value, boolean create)
{
	KHead *handle;
	double value;
	handle = k_record_entry_lookup_name(record, name, K_VT_REAL64, TRUE, TRUE);
	if(handle == NULL)
	{
		if(create)
		{
			handle = k_record_entry_add(record, K_VT_REAL64, name);
			k_record_entry_array_handle_set(record, handle, &default_value, 0, 1);
		}else
			return default_value;
	}
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return default_value;
}

boolean k_record_entry_double_name_set(KRecord *record, char *name, uint id, double value)
{
	KHead *handle;
	handle = k_record_entry_lookup_name(record, name, K_VT_REAL64, TRUE, TRUE);
	if(handle == NULL)
		handle = k_record_entry_add(record, K_VT_REAL64, name);
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

double k_record_entry_double_handle_get(KRecord *record, KHead *handle, uint id)
{
	double value;
	if(k_record_entry_array_handle_get_copy(record, handle, &value, id, 1))
		return value;
	else
		return 0;
}

boolean	k_record_entry_double_handle_set(KRecord *record, KHead *handle, uint id, double value)
{
	if(handle->type != K_VT_REAL64)
		return FALSE;
	k_record_entry_array_handle_set(record, handle, &value, id, 1);
	return TRUE;
}

