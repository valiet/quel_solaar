/* -------------- Imagine ------------
Imagine is a platform layer that wraps some basic operating system functionality. It is the smaller sibling of Betray the much larger platform layer. The difference is that Imagine doesnt require a display, drawing or any user input. It contains things like treading, file system traversal, plugin loading and settings management. */


#if defined __cplusplus		/* Declare as C symbols for C++ users. */
extern "C" {
#endif


#ifndef uint
typedef unsigned int uint;
#endif
#ifndef uint
typedef unsigned char boolean;
#endif

#define ILibExport _declspec (dllexport)

/* ---------- Library loading --------
Imagine allows the loading of dynamic libraries and the description of function call interfaces between libraries and host applications. The host starts by creating an interface and then load function pointers in to it. It then loads a library that can access these functions pointers to comunicate with the host application. The same interface can be used for multiple loaded libraries.*/

typedef void IInterface;
typedef void ILib;

extern IInterface 	*imagine_library_interface_create(); /* Create an interface. */
extern void		imagine_library_interface_destroy(IInterface *i);  /* Destroy an interface. */
extern void		imagine_library_interface_register(IInterface *i, void *funtion_pointer, char *name); /* load a function pointer with a name in to identy fy it in to an interface*/
extern int		imagine_library_interface_count(IInterface *i); /* Find ount hown many functions are available in the interface.*/
extern char		*imagine_library_interface_list(IInterface *i, uint number); /* Get the name of a specific function in the interface. */
extern void		*imagine_library_interface_get_by_number(IInterface *i, uint number); /* Get the function pointer belonging to a specific number from an interface. */
extern void		*imagine_library_interface_get_by_name(IInterface *i, char *name); /* Look up a function pointer by name from an interface. Returns NULL is not available */
extern ILib		*imagine_library_load(char *path, IInterface *i, char *interface_name); /* Load a library. */
extern void		imagine_library_unload(ILib *lib, boolean interface_delete); /* Deletes a library. if interface_delete is set top TRUE, the interface used to load the library will als be deleted*/

/*ILibExport IInterface *(*imagine_lib_main)(IInterface *exe_interface); *//* library entry point */

/* ----- Multi Treading ----- 
Imagine has full thread supprot that lets you create threads and thread safe Mutex locks. */

extern void		*imagine_mutex_create(); /* Creates a Mutex. Mutex locks are unlocked when created.*/
extern void		imagine_mutex_lock(void *mutex); /* Lock a mutex. If the lock is already locked, the thread will wait on the lock until the lock is unlocked so that it can lock it */
extern boolean	imagine_mutex_lock_try(void *mutex); /* The thread will atempt to lock the thread, if the lock is already locked if will returne FALSE and fail, If is is not locked, it will lock the mutex and return TRUE */
extern void		imagine_mutex_unlock(void *mutex); /* Un locks the Mutex */
extern void		imagine_mutex_destroy(void *mutex); /* Destroys the mutex */

extern void		*imagine_signal_create(); /* Creates a signal bocker*/
extern void		*magine_signal_destroy(void *signal); /* Destroys a signal blocker */
extern boolean	imagine_signal_wait(void *signal); /*Sets a thread to wait on the blocker for another thread to a activate it.*/
extern boolean	imagine_signal_activate(void *signal); /*Activates the blocker so that one or more threads waiting on the signal will be released */

extern void		imagine_thread(void (*func)(void *data), void *data); /* launches a thread that will execute the function pointer. The void pointer will be given as i parameter. Onec the function returns the thread will be deleted.*/

/* ----- timing ----- 
Imagine has full thread supprot that lets you create threads and thread safe Mutex locks. */

extern void		imagine_current_time_get(uint32 *seconds, uint32 *fractions); /*  get time in seconds and fractions of seconds */
extern double	imagine_delta_time_compute(uint new_seconds, uint new_fractions, uint old_seconds, uint old_fractions); /* computes the ammount of time that has elapsed between two time messurements.*/
extern void		imagine_current_date_get(uint *seconds, uint *minutes, uint *hours, uint *week_days, uint *month_days, uint *month, uint *year);  /* Outputs the curent date information. */

/* ------- Execution ------- 
Imagine allows applications to launch other applications. These applications will run concurrent to the launching application*/

extern boolean	imagine_execute(const char *command); /* Execute comand on platform */

/* ---------Settings storage -----
Imagines setting system is convinient system for storing application settings in an XML file. An application can both set and get a settings value. If an application is using any of the "get" functions requesting a non existent setting, then the setting will be automaticaly created using the default value. This makes the system very robust as a deleter file, or even part of a file, can be recreated by the setting sytem system. An application also has the abillity to provide comments to make the settings file easier for humans to read.*/

extern boolean	imagine_setting_boolean_get(const char *setting, boolean default_value, const char *comment); /* Get boolean setting. The function will return default_value if the setting is not available, and create the setting in the setting li*/
extern void		imagine_setting_boolean_set(const char *setting, boolean value, const char *comment); /* set boolean setting */
extern int		imagine_setting_integer_get(const char *setting, int default_value, const char *comment); /* get uint setting */
extern void		imagine_setting_integer_set(const char *setting, int value, const char *comment); /* set uint setting */
extern double	imagine_setting_double_get(const char *setting, double default_value, const char *comment); /* get double setting */
extern void		imagine_setting_double_set(const char *setting, double value, const char *comment); /* set double setting */
extern char		*imagine_setting_text_get(const char *setting, char *default_text, const char *comment); /* get text setting */
extern void		imagine_setting_text_set(const char *setting, char *text, const char *comment); /* set text setting */
extern void		imagine_settings_save(const char *file_name); /* saves all settings to a file */
extern void		imagine_settings_load(const char *file_name); /* loads all settings from a file */
extern boolean  imagine_setting_test(const char *setting); /* Test if a setting exists */

/* ---- Directory listing --------
Under window this code will create a imaginary root directory containing all volumes. It is therefor possible to list all files in all volumes.*/

#ifdef _WIN32
	#define IMAGINE_DIR_ROOT_PATH "" /* Defines the root path on Windows. */
	#define IMAGINE_LIBRARY_EXTENTION "dll" /* Defines the name of a library on Windows. */
#else
	#define IMAGINE_DIR_ROOT_PATH "\" /* Defines the root path on Unix based platforms. */
	#define IMAGINE_LIBRARY_EXTENTION "lib" /* Defines the name of a library on Windows. */
#endif
#define IMAGINE_DIR_HOME_PATH "." /* Defines the path to the applications Home directory.*/

extern boolean imagine_path_search(char *file, boolean partial, char *path, boolean folders, uint number, char *out_buffer, uint out_buffer_size); /* Searches for a file recursevly in a path, and writes its location to the out_buffer. If "partial" is set the search will also yeild results wher the seacr string only makes up part of the file name.*/

typedef void IDir;

extern IDir		*imagine_path_open(char *path); /* Opens a path for traversial. If the path is not legal or not a directry the fuction will return NULL. */
extern boolean	imagine_path_next(IDir *d, char *file_name_buffer, uint buffer_size); /* Writes the name of the next member of the directory to file_name_buffer. Returns FALSE if there are no files left in the directory to write out. */
extern void		imagine_path_close(IDir *d); /* Closes a directory. */

extern boolean	imagine_path_is_dir(char *path); /* Returns True if the path is a valid directory. */

#if defined __cplusplus
}
#endif