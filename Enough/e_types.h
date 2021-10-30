#if !defined ENOUGH_DATA_TYPES
#define ENOUGH_DATA_TYPES


#define E_GEOMETRY_REAL_PRECISION_64_BIT
/*#define E_GEOMETRY_REAL_PRECISION_32_BIT*/

#ifdef E_GEOMETRY_REAL_PRECISION_64_BIT
typedef double egreal;
#define E_GEOMETRY_SUBSCRIBE VN_FORMAT_REAL64
#define E_REAL_MAX V_REAL64_MAX
#endif

#ifdef E_GEOMETRY_REAL_PRECISION_32_BIT
typedef float egreal;
#define E_GEOMETRY_SUBSCRIBE VN_FORMAT_REAL32
#define E_REAL_MAX V_REAL32_MAX
#endif

//#define E_BITMAP_REAL_PRECISION_64_BIT
#define E_BITMAP_REAL_PRECISION_32_BIT

#ifdef E_BITMAP_REAL_PRECISION_64_BIT
typedef double ebreal;
#endif

#ifdef E_BITMAP_REAL_PRECISION_32_BIT
typedef float ebreal;
#endif

typedef struct{
	void	**data;
	uint	allocated;
}EDynLookUpTable;

void e_dlut_init(EDynLookUpTable *table);
void e_dlut_free(EDynLookUpTable *table);
void *e_dlut_get(EDynLookUpTable *table, uint key);
void *e_dlut_next(EDynLookUpTable *table, uint key);
uint e_dlut_next_empty(EDynLookUpTable *table, uint key);
void *e_dlut_add(EDynLookUpTable *table, uint key, void *pointer);
uint e_dlut_add_in_empty(EDynLookUpTable *table, void *pointer);
void e_dlut_remove(EDynLookUpTable *table, uint key);
uint e_dlut_count(EDynLookUpTable *table);
void e_dlut_clean(EDynLookUpTable *table);
void e_dlut_remove_foreach(EDynLookUpTable *table, void (*func)(uint key, void *pointer, void *user_data), void *user_data);

#endif
