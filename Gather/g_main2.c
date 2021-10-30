#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "forge.h"
#include "imagine.h"
#define GATHER_INTERNAL

typedef struct{
	char name[64];
	void *(*file_read_create)(char *file_name);
	void (*destroy)(void *session);
	double (*duration_get)(void *session);
	void (*seek)(void *session, double time);
	uint (*image_stream_count_get)(void *session);
	void (*image_stream_active)(void *session, uint stream_id, boolean active);
	void (*image_stream_size)(void *session, uint stream_id, uint *x, uint *y);
	float (*image_stream_frame_rate)(void *session, uint stream_id);
	boolean (*image_stream_high_dynamic_range)(void *session, uint stream_id);
	uint8 *(*image_stream_uint8_get)(void *session, uint stream_id);
	float *(*image_stream_float_get)(void *session, uint stream_id);
	void (*image_stream_frame_advance)(void *session, uint stream_id); 
	double (*image_stream_frame_start)(void *session, uint stream_id);
	uint (*audio_stream_count_get)(void *session);
	void (*audio_stream_active)(void *session, uint stream_id, boolean active);
	boolean	(*audio_stream_high_dynamic_range)(void *session, uint stream_id);
	uint (*audio_stream_channel_count_get)(void *session, uint stream_id);
	void *(*audio_stream_channel_float_get)(void *session, uint *length, uint stream_id, uint channel);
	void *(*audio_stream_channel_int16_get)(void *session, uint *length, uint stream_id, uint channel);
	uint (*audio_stream_sample_rate_get)(void *session, uint stream_id);
	void (*audio_stream_frame_advance)(void *session, uint stream_id); 
	double (*audio_stream_frame_start)(void *session, uint stream_id); 
}Gatherplugin;

typedef struct{
	uint id;
	char name[16];
	boolean load;
	boolean save;
	uint plugin;
}GatherFormat;

struct{
	IInterface 	*gather_interface;
	Gatherplugin *plugins;
	uint plugin_count;
}GatherGlobal;

typedef struct{
	uint plugin_type;
	void *plugin_session;
}GatherSession;

uint gather_format_count()
{
	return 0;
}

void *gather_load(char *file_name)
{
	return NULL;
}

void gather_plugin_callback_set_open_file(void *(open_file_func)(char *file_name))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].file_read_create = open_file_func;
}

void gather_plugin_callback_set_destroy(void (*destroy)(void *session))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].destroy = destroy;
}

void gather_plugin_callback_set_duration_get(double (*duration_get)(void *session))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].duration_get = duration_get;
}

void gather_plugin_callback_set_seek(void (*seek)(void *session, double time))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].seek = seek;
}

void gather_plugin_callback_set_image_stream_count_get(uint (*image_stream_count_get)(void *session))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_count_get = image_stream_count_get;
}

void gather_plugin_callback_set_image_stream_active(void (*image_stream_active)(void *session, uint stream_id, boolean active))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_active = image_stream_active;
}

void gather_plugin_callback_set_image_stream_size(void (*image_stream_size)(void *session, uint stream_id, uint *x, uint *y))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_size = image_stream_size;
}

void gather_plugin_callback_set_image_stream_frame_rate(float (*image_stream_frame_rate)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_frame_rate = image_stream_frame_rate;
}

void gather_plugin_callback_set_image_stream_high_dynamic_range(boolean (*image_stream_high_dynamic_range)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_high_dynamic_range = image_stream_high_dynamic_range;
}

void gather_plugin_callback_set_image_stream_uint8_get(uint8 *(*image_stream_uint8_get)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_uint8_get = image_stream_uint8_get;
}

void gather_plugin_callback_set_image_stream_float_get(float *(*image_stream_float_get)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_float_get = image_stream_float_get;
}

void gather_plugin_callback_set_image_stream_frame_advance(void (*image_stream_frame_advance)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_frame_advance = image_stream_frame_advance;
}

void gather_plugin_callback_set_image_stream_frame_start(double (*image_stream_frame_start)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].image_stream_frame_start = image_stream_frame_start;
}

void gather_plugin_callback_set_audio_stream_count_get(uint (*audio_stream_count_get)(void *session))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_count_get = audio_stream_count_get;
}

void gather_plugin_callback_set_audio_stream_active(void (*audio_stream_active)(void *session, uint stream_id, boolean active))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_active = audio_stream_active;
}

void gather_plugin_callback_set_audio_stream_high_dynamic_range(boolean	(*audio_stream_high_dynamic_range)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_high_dynamic_range = audio_stream_high_dynamic_range;
}

void gather_plugin_callback_set_audio_stream_channel_count_get(uint (*audio_stream_channel_count_get)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_channel_count_get = audio_stream_channel_count_get;
}

void gather_plugin_callback_set_audio_stream_channel_float_get(void *(*audio_stream_channel_float_get)(void *session, uint *length, uint stream_id, uint channel))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_channel_float_get = audio_stream_channel_float_get;
}

void gather_plugin_callback_set_audio_stream_channel_int16_get(void *(*audio_stream_channel_int16_get)(void *session, uint *length, uint stream_id, uint channel))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_channel_int16_get = audio_stream_channel_int16_get;
}

void gather_plugin_callback_set_audio_stream_sample_rate_get(uint (*audio_stream_sample_rate_get)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_sample_rate_get = audio_stream_sample_rate_get;
}

void gather_plugin_callback_set_audio_stream_frame_advance(void (*audio_stream_frame_advance)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_frame_advance = audio_stream_frame_advance;
}

void gather_plugin_callback_set_audio_stream_frame_start(double (*audio_stream_frame_start)(void *session, uint stream_id))
{
	GatherGlobal.plugins[GatherGlobal.plugin_count].audio_stream_frame_start = audio_stream_frame_start;
}




void gather_init(void)
{
	char path[1024];
	uint i;
	GatherGlobal.gather_interface = imagine_library_interface_create();
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_open_file, "gather_plugin_callback_set_open_file");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_destroy, "gather_plugin_callback_set_destroy");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_duration_get, "gather_plugin_callback_set_duration_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_seek, "gather_plugin_callback_set_seek");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_count_get, "gather_plugin_callback_set_image_stream_count_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_active, "gather_plugin_callback_set_image_stream_active");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_size, "gather_plugin_callback_set_image_stream_size");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_frame_rate, "gather_plugin_callback_set_image_stream_frame_rate");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_high_dynamic_range, "gather_plugin_callback_set_image_stream_high_dynamic_range");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_uint8_get, "gather_plugin_callback_set_image_stream_uint8_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_float_get, "gather_plugin_callback_set_image_stream_float_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_frame_advance, "gather_plugin_callback_set_image_stream_frame_advance");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_image_stream_frame_start, "gather_plugin_callback_set_image_stream_frame_start");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_count_get, "gather_plugin_callback_set_audio_stream_count_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_active, "gather_plugin_callback_set_audio_stream_active");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_high_dynamic_range, "gather_plugin_callback_set_audio_stream_high_dynamic_range");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_channel_count_get, "gather_plugin_callback_set_audio_stream_channel_count_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_channel_float_get, "gather_plugin_callback_set_audio_stream_channel_float_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_channel_int16_get, "gather_plugin_callback_set_audio_stream_channel_int16_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_sample_rate_get, "gather_plugin_callback_set_audio_stream_sample_rate_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_sample_rate_get, "gather_plugin_callback_set_audio_stream_sample_rate_get");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_frame_advance, "gather_plugin_callback_set_audio_stream_frame_advance");
	imagine_library_interface_register(GatherGlobal.gather_interface, gather_plugin_callback_set_audio_stream_frame_start, "gather_plugin_callback_set_audio_stream_frame_start");
	for(i = 0; imagine_path_search("." /*the lack of comma is intentional*/ IMAGINE_LIBRARY_EXTENTION, TRUE, IMAGINE_DIR_HOME_PATH, FALSE, i, path, 1024); i++)
	{
		printf("Loading: %s ", path);
		if(GatherGlobal.plugin_count % 16 == 0)
			GatherGlobal.plugins = malloc((sizeof *GatherGlobal.plugins) * (GatherGlobal.plugin_count + 16));
		if(NULL == imagine_library_load(path, GatherGlobal.gather_interface, "Gather"))
		{
			printf("%s failed.\n", path);
		}
		else
		{
			printf("%s succeded.\n", path);
			GatherGlobal.plugin_count++;
		}
	}
}

extern void		gather_format_count_get(void); /* Returns the number of file formats supported by gather */
extern char		*gather_format_name(void); /* Returns the name of the format. */
extern boolean	gather_format_save(void); /* Returns TRUE if the format can be saved. */
extern boolean	gather_format_load(void); /* Returns TRUE if the format can be loaded. */
extern boolean	gather_format_image(void); /* Returns TRUE if the format can store images. */
extern boolean	gather_format_audio(void); /* Returns TRUE if the format can store audio. */
extern boolean	gather_format_video(void); /* Returns TRUE if the format can store video. */

GatherSession *gather_session_file_read_create(char *file_name)
{
	GatherSession *session;
	void *s;
	uint i;
	for(i = 0; i < GatherGlobal.plugin_count; i++)
	{
		if((s = GatherGlobal.plugins[i].file_read_create(file_name)) != 0)
		{
			session = malloc(sizeof *session);
			session->plugin_type = i;
			session->plugin_session = s;
			return session;
		}
	}
	return NULL;
}

void gather_session_destroy(GatherSession *session)
{
	GatherGlobal.plugins[session->plugin_type].destroy(session->plugin_session);
	free(session);
}

double gather_session_duration_get(GatherSession *session)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].duration_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].duration_get(session->plugin_session);
	return 1.0;
}

void gather_session_seek(GatherSession *session, double time)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].seek != NULL)
		GatherGlobal.plugins[session->plugin_type].seek(session->plugin_session, time);
}

uint gather_session_image_stream_count_get(GatherSession *session)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_count_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].image_stream_count_get(session->plugin_session);
	return 0;
}

void gather_session_image_stream_active(GatherSession *session, uint stream_id, boolean active)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_active != NULL)
		GatherGlobal.plugins[session->plugin_type].image_stream_active(session->plugin_session, stream_id, active);
}

void gather_session_image_stream_size(GatherSession *session, uint stream_id, uint *x, uint *y)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_size != NULL)
		GatherGlobal.plugins[session->plugin_type].image_stream_size(session->plugin_session, stream_id, x, y);
	else
		*x = *y = 1;
}

double gather_session_image_stream_frame_rate(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_frame_rate != NULL)
		return GatherGlobal.plugins[session->plugin_type].image_stream_frame_rate(session->plugin_session, stream_id);
	return 1.0;
}

boolean gather_session_image_stream_high_dynamic_range(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_frame_rate != NULL)
		return GatherGlobal.plugins[session->plugin_type].image_stream_frame_rate(session->plugin_session, stream_id);
	return FALSE;
}

uint8 *gather_session_image_stream_uint8_get(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_uint8_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].image_stream_uint8_get(session->plugin_session, stream_id);
	return NULL;
}

float *gather_session_image_stream_float_get(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_float_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].image_stream_float_get(session->plugin_session, stream_id);
	return NULL;
}

void gather_session_image_stream_frame_advance(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_frame_advance != NULL)
		GatherGlobal.plugins[session->plugin_type].image_stream_frame_advance(session->plugin_session, stream_id);
}

double gather_session_image_stream_frame_start(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].image_stream_frame_start != NULL)
		return GatherGlobal.plugins[session->plugin_type].image_stream_frame_start(session->plugin_session, stream_id);
	return 100000000.0;
}

uint gather_session_audio_stream_count_get(GatherSession *session)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_count_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_count_get(session->plugin_session);
	return 0;
}

void gather_session_audio_stream_active(GatherSession *session, uint stream_id, boolean active)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_active != NULL)
		GatherGlobal.plugins[session->plugin_type].audio_stream_active(session->plugin_session, stream_id, active);
}

boolean	gather_session_audio_stream_high_dynamic_range(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_high_dynamic_range != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_high_dynamic_range(session->plugin_session, stream_id);
	return FALSE;
}

uint gather_session_audio_stream_channel_count_get(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_channel_count_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_channel_count_get(session->plugin_session, stream_id);
	return 0;
}

void *gather_session_audio_stream_channel_float_get(GatherSession *session, uint *length, uint stream_id, uint channel)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_channel_float_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_channel_float_get(session->plugin_session, length, stream_id, channel);
	return NULL;
}

void *gather_session_audio_stream_channel_int16_get(GatherSession *session, uint *length, uint stream_id, uint channel)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_channel_int16_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_channel_int16_get(session->plugin_session, length, stream_id, channel);
	return NULL;
}

uint gather_session_audio_stream_sample_rate_get(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_sample_rate_get != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_sample_rate_get(session->plugin_session, stream_id);
	return 0;
}

void gather_session_audio_stream_frame_advance(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_frame_advance != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_frame_advance(session->plugin_session, stream_id);
	return 0;
}

double gather_session_audio_stream_frame_start(GatherSession *session, uint stream_id)
{
	if(session != NULL && GatherGlobal.plugins[session->plugin_type].audio_stream_frame_start != NULL)
		return GatherGlobal.plugins[session->plugin_type].audio_stream_frame_start(session->plugin_session, stream_id);
	return 1000000;
}


void *gather_image_load(char *file_name, uint *x, uint *y, boolean *hdr)
{
	GatherSession *session;
	void *data, *copy;
	session = gather_session_file_read_create(file_name); 
	if(session == NULL)
		return;
	if(0 < gather_session_image_stream_count_get(session))
	{
		gather_session_image_stream_active(session, 0, TRUE);
		*hdr = gather_session_image_stream_high_dynamic_range(session, 0);
			gather_session_image_stream_size(session, 0, x, y); /* Writes out the X and Z resolution of the image stream*/
	/*	if(*hdr)
		{
			data = gather_session_image_stream_float_get(session, 0); 
			copy = malloc(sizeof(float) * *x * *y * 4);
			memcpy(copy, data, sizeof(float) * *x * *y * 4);
		}else*/
		{
			data = gather_session_image_stream_uint8_get(session, 0);
			copy = malloc(sizeof(char) * *x * *y * 4);
			memcpy(copy, data, sizeof(char) * *x * *y * 4);
		}
	}
	gather_session_destroy(session);
	return copy;

}

extern GatherSession *gather_session_file_read_create(char *file_name); /* Opens a session by reading form a file. will return NULL if the file cant be open or the format is not understood by any plugin.*/
extern void		gather_session_destroy(GatherSession *session); /* Destropy a open session. */

extern double	gather_session_duration_get(GatherSession *session);
extern void		gather_session_seek(GatherSession *session, double time);

/* --- Image ---- 
Retrive image data from a gather session. */

extern uint		gather_session_image_stream_count_get(GatherSession *session); /* Returns the number of image streams found in a session. */
extern void		gather_session_image_stream_active(GatherSession *session, uint stream_id, boolean active); /* activate or disable the stream so that its readable. */
extern void		gather_session_image_stream_size(GatherSession *session, uint stream_id, uint *x, uint *y); /* Writes out the X and Z resolution of the image stream*/
extern double	gather_session_image_stream_frame_rate(GatherSession *session, uint stream_id); /* Returns the frame rate of the stream. */
extern boolean	gather_session_image_stream_high_dynamic_range(GatherSession *session, uint stream_id); /* Returns TRUE if the image stream stores floats, and FALSE if the image stream stores uint8. */
extern uint8	*gather_session_image_stream_uint8_get(GatherSession *session, uint stream_id); /* Returns a pointer to uint8 RGBA data containing the image. */
extern float	*gather_session_image_stream_float_get(GatherSession *session, uint stream_id); /* Returns a pointer to float RGBA data containing the image. */
extern void		gather_session_image_stream_frame_advance(GatherSession *session, uint stream_id); /* Reading the current frame has been completed, and requesting a new frame.*/
extern double	gather_session_image_stream_frame_start(GatherSession *session, uint stream_id); /* Time when the frame should first be displayed*/



