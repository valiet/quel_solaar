#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "forge.h"
#include "imagine.h"
#include "testify.h"
#define K_RECORD_INTERNAL
#include "keep.h"

#define K_RECORD_ROOT_FLAG_JOURNALING (0x00000001 | 0x00000002) /* 0, byte, 32 bit 64. is the Record journaling */	
#define K_RECORD_ROOT_FLAG_TIME_STAMP_PRECISSION (0x00000004 | 0x00000008) /* LOW MiD HIGH JOURNALING */	
#define K_RECORD_ROOT_MEMBER_COUNT (0x00000010 | 0x00000020)

#define K_RECORD_VALUE_FLAG_ARRAY_COUNT (0x00000001 | 0x00000002) /* 0, byte, 32 bit 64.*/
#define K_RECORD_VALUE_FLAG_COMMENT (0x00000004) /* does the value have acomment describing it */
#define K_RECORD_VALUE_FLAG_LEAF (0x00000008) /* is the thing a part of a struct*/

//typedef void KRecord;

#define K_VALUE_NANE_BUFFER_SIZE 44

typedef struct{
	int64 seconds;
	int64 fractions;
}KTimestamp;


typedef struct{
	char **elements;
	uint8 element_count;
}KFootEnum;


typedef struct{
	size_t *elements;
	uint8 element_count;
	uint8 text;
	uint64 size_of;
	uint64 selected;
}KFootStruct;

#define KEEP_ID_SIZE_8_BIT (256 / sizeof(uint8))
#define KEEP_ID_SIZE_16_BIT (256 / sizeof(uint16))
#define KEEP_ID_SIZE_32_BIT (256 / sizeof(uint32))
#define KEEP_ID_SIZE_64_BIT (256 / sizeof(uint64))

#define KEEP_RECORD_MEM_BLOCK_SIZE 1024
#define KEEP_JOURNALING_ALLOCATION_SIZE 32

void imagine_time_epoc_current(uint64 *seconds, uint32 *fractions);

typedef struct{
	uint16 type;
	uint16 flags;
	char name[K_VALUE_NANE_BUFFER_SIZE];
	uint64 id;
	char *comment;
	uint64 array_length;
	 void *array;
}KHead;

typedef struct{
	uint64 start;
	uint64 size;
	KTimestamp timestamp;
	void *array;
}KJournalEntry;

typedef struct{
	KHead array;
	KTimestamp timestamp;
	KJournalEntry *entry;
	uint64 entry_count;
}KJournalHead;

typedef struct{
	uint64 store_address;
	uint64 id[KEEP_ID_SIZE_64_BIT];
	uint64 *derivative;
	KTimestamp timestamp;
	uint64 owner;
	uint64 value_count;
	uint32 memory_used;  /* multiplied with KEEP_RECORD_MEM_BLOCK_SIZE */
	uint32 memory_allocated; /* multiplied with KEEP_RECORD_MEM_BLOCK_SIZE */
	uint8 *allocation;
	void *signatures;
	uint64 signature_count;
	uint16 flags;
	boolean timestamp_lock;
}KRecord;


extern uint k_data_type_size[];
extern uint k_data_type_alignment[];
extern uint k_data_foot_size[];


extern uint64 k_record_entry_sizeof(KRecord *record, KValueType type);

extern KHead *k_record_entry_add(KRecord *record, KValueType type, char *name);
extern KHead *k_record_entry_next(KRecord *record, KHead *previous, KValueType type, boolean include_struct_members, boolean include_deleted);
extern KHead *k_record_entry_lookup_name(KRecord *record, char *name, KValueType type, boolean include_struct_members, boolean include_deleted);
