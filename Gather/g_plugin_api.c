
#include <stdio.h>
#include "forge.h"
#include "imagine.h"

extern void gather_plugin_init();
extern void *gather_plugin_session_file_read_create(char *file_name);
extern void gather_plugin_session_destroy(void *session);
extern double gather_plugin_session_duration_get(void *session);
extern void gather_plugin_session_seek(void *session, double time);
extern double gather_plugin_session_image_stream_frame_start(void *session, uint stream_id);
extern void gather_plugin_session_image_stream_frame_advance(void *session, uint stream_id);
extern double gather_plugin_session_audio_stream_frame_start(void *session, uint stream_id);
extern void gather_plugin_session_audio_stream_frame_advance(void *session, uint stream_id);

extern uint gather_plugin_session_image_stream_count_get(void *session);
extern uint gather_plugin_session_audio_stream_count_get(void *session);
extern uint gather_plugin_session_audio_stream_channel_count_get(void *session, uint stream_id);
extern boolean gather_plugin_session_audio_stream_high_dynamic_range(void *session, uint stream_id);
extern float *gather_plugin_session_audio_stream_channel_float_get(void *session, uint *length, uint stream_id, uint channel);
extern int16 *gather_plugin_session_audio_stream_channel_int16_get(void *session, uint *length, uint stream_id, uint channel);
extern uint gather_plugin_session_audio_stream_sample_rate_get(void *session, uint stream_id);
extern void gather_plugin_session_image_stream_size(void *session, uint stream_id, uint *x, uint *y);
extern double gather_plugin_session_image_stream_frame_rate(void *session, uint stream_id);
extern boolean	gather_plugin_session_image_stream_high_dynamic_range(void *session, uint stream_id);
extern uint8 *gather_plugin_session_image_stream_uint8_get(void *session, uint stream_id);
extern float *gather_plugin_session_image_stream_float_get(void *session, uint stream_id);
extern void gather_plugin_session_image_stream_active(void *session, uint stream_id, boolean active);
extern void gather_plugin_session_audio_stream_active(void *session, uint stream_id, boolean active);


ILibExport char *imagine_lib_name(void)
{
	return f_text_copy_allocate("Gather");
}

ILibExport IInterface *imagine_lib_main(IInterface *exe_interface)
{
	void (*gather_plugin_callback_set_open_file)(void *(open_file_func)(char *file_name));
	void (*gather_plugin_callback_set_destroy)(void (*destroy)(void *session));
	void (*gather_plugin_callback_set_duration_get)(double (*duration_get)(void *session));
	void (*gather_plugin_callback_set_seek)(void (*seek)(void *session, double time));
	void (*gather_plugin_callback_set_image_stream_count_get)(uint (*image_stream_count_get)(void *session));
	void (*gather_plugin_callback_set_image_stream_active)(void (*image_stream_active)(void *session, uint stream_id, boolean active));
	void (*gather_plugin_callback_set_image_stream_size)(void (*image_stream_size)(void *session, uint stream_id, uint *x, uint *y));
	void (*gather_plugin_callback_set_image_stream_frame_rate)(double (*image_stream_frame_rate)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_image_stream_high_dynamic_range)(boolean (*image_stream_high_dynamic_range)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_image_stream_uint8_get)(uint8 *(*image_stream_uint8_get)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_image_stream_float_get)(float *(*image_stream_float_get)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_image_stream_frame_advance)(void (*image_stream_frame_advance)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_image_stream_frame_start)(double (*image_stream_frame_start)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_audio_stream_count_get)(uint (*audio_stream_count_get)(void *session));
	void (*gather_plugin_callback_set_audio_stream_active)(void (*audio_stream_active)(void *session, uint stream_id, boolean active));
	void (*gather_plugin_callback_set_audio_stream_high_dynamic_range)(boolean	(*audio_stream_high_dynamic_range)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_audio_stream_channel_count_get)(uint (*audio_stream_channel_count_get)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_audio_stream_channel_float_get)(float *(*audio_stream_channel_float_get)(void *session, uint *length, uint stream_id, uint channel));
	void (*gather_plugin_callback_set_audio_stream_channel_int16_get)(int16 *(*audio_stream_channel_int16_get)(void *session, uint *length, uint stream_id, uint channel));
	void (*gather_plugin_callback_set_audio_stream_sample_rate_get)(uint (*audio_stream_sample_rate_get)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_audio_stream_frame_advance)(void (*audio_stream_frame_advance)(void *session, uint stream_id));
	void (*gather_plugin_callback_set_audio_stream_frame_start)(double (*audio_stream_frame_start)(void *session, uint stream_id));	
		
	gather_plugin_callback_set_open_file = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_open_file");
	gather_plugin_callback_set_destroy = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_destroy");
	gather_plugin_callback_set_duration_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_duration_get");
	gather_plugin_callback_set_seek = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_seek");
	gather_plugin_callback_set_image_stream_count_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_count_get");
	gather_plugin_callback_set_image_stream_active = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_active");
	gather_plugin_callback_set_image_stream_size = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_size");
	gather_plugin_callback_set_image_stream_frame_rate = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_frame_rate");
	gather_plugin_callback_set_image_stream_high_dynamic_range = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_high_dynamic_range");
	gather_plugin_callback_set_image_stream_uint8_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_uint8_get");
	gather_plugin_callback_set_image_stream_float_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_float_get");
	gather_plugin_callback_set_image_stream_frame_advance = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_frame_advance");
	gather_plugin_callback_set_image_stream_frame_start = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_image_stream_frame_start");
	gather_plugin_callback_set_audio_stream_count_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_count_get");
	gather_plugin_callback_set_audio_stream_active = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_active");
	gather_plugin_callback_set_audio_stream_high_dynamic_range = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_high_dynamic_range");
	gather_plugin_callback_set_audio_stream_channel_count_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_channel_count_get");
	gather_plugin_callback_set_audio_stream_channel_float_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_channel_float_get");
	gather_plugin_callback_set_audio_stream_channel_int16_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_channel_int16_get");
	gather_plugin_callback_set_audio_stream_sample_rate_get = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_sample_rate_get");
	gather_plugin_callback_set_audio_stream_frame_advance = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_frame_advance");
	gather_plugin_callback_set_audio_stream_frame_start = imagine_library_interface_get_by_name(exe_interface, "gather_plugin_callback_set_audio_stream_frame_start");
	
	gather_plugin_init();

	gather_plugin_callback_set_open_file(gather_plugin_session_file_read_create);
	gather_plugin_callback_set_destroy(gather_plugin_session_destroy);		
	gather_plugin_callback_set_duration_get(gather_plugin_session_duration_get);
	gather_plugin_callback_set_seek(gather_plugin_session_seek);
	gather_plugin_callback_set_image_stream_count_get(gather_plugin_session_image_stream_count_get);
	gather_plugin_callback_set_image_stream_active(gather_plugin_session_image_stream_active);
	gather_plugin_callback_set_image_stream_size(gather_plugin_session_image_stream_size);
	gather_plugin_callback_set_image_stream_frame_rate(gather_plugin_session_image_stream_frame_rate);
	gather_plugin_callback_set_image_stream_high_dynamic_range(gather_plugin_session_image_stream_high_dynamic_range);
	gather_plugin_callback_set_image_stream_uint8_get(gather_plugin_session_image_stream_uint8_get);
	gather_plugin_callback_set_image_stream_float_get(gather_plugin_session_image_stream_float_get);
	gather_plugin_callback_set_image_stream_frame_advance(gather_plugin_session_image_stream_frame_advance);
	gather_plugin_callback_set_image_stream_frame_start(gather_plugin_session_image_stream_frame_start);
	gather_plugin_callback_set_audio_stream_count_get(gather_plugin_session_audio_stream_count_get);
	gather_plugin_callback_set_audio_stream_active(gather_plugin_session_audio_stream_active);
	gather_plugin_callback_set_audio_stream_high_dynamic_range(gather_plugin_session_audio_stream_high_dynamic_range);
	gather_plugin_callback_set_audio_stream_channel_count_get(gather_plugin_session_audio_stream_channel_count_get);
	gather_plugin_callback_set_audio_stream_channel_float_get(gather_plugin_session_audio_stream_channel_float_get);
	gather_plugin_callback_set_audio_stream_channel_int16_get(gather_plugin_session_audio_stream_channel_int16_get);
	gather_plugin_callback_set_audio_stream_sample_rate_get(gather_plugin_session_audio_stream_sample_rate_get);
	gather_plugin_callback_set_audio_stream_frame_advance(gather_plugin_session_audio_stream_frame_advance);
	gather_plugin_callback_set_audio_stream_frame_start(gather_plugin_session_audio_stream_frame_start);
	return NULL;
}

