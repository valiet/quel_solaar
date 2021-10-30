
#include <stdio.h>
#include <windows.h>
#include <GdiPlus.h>
#include "forge.h"

typedef struct{
	uint8 *data;
	uint size_x;
	uint size_y;
}GatherGDIPlusSession;

using namespace Gdiplus;

extern "C" {
	void gather_plugin_init();
	double gather_plugin_session_image_stream_frame_rate(void *session, uint stream_id);
	GatherGDIPlusSession *gather_plugin_session_file_read_create(char *file_name);
	double gather_plugin_session_duration_get(GatherGDIPlusSession *session);
	void gather_plugin_session_seek(GatherGDIPlusSession *session, double time);
	double gather_plugin_session_image_stream_frame_start(GatherGDIPlusSession *session, uint stream_id);
	void gather_plugin_session_image_stream_frame_advance(GatherGDIPlusSession *session, uint stream_id);
	double gather_plugin_session_audio_stream_frame_start(GatherGDIPlusSession *session, uint stream_id);
	void gather_plugin_session_audio_stream_frame_advance(GatherGDIPlusSession *session, uint stream_id);
	void gather_plugin_session_destroy(GatherGDIPlusSession *session);
	uint gather_plugin_session_image_stream_count_get(GatherGDIPlusSession *session);
	uint gather_plugin_session_audio_stream_count_get(GatherGDIPlusSession *session);
	uint gather_plugin_session_audio_stream_channel_count_get(GatherGDIPlusSession *session, uint stream_id);
	boolean	gather_plugin_session_audio_stream_high_dynamic_range(GatherGDIPlusSession *session, uint stream_id);
	float *gather_plugin_session_audio_stream_channel_float_get(GatherGDIPlusSession *session, uint *length, uint stream_id, uint channel);
	int16 *gather_plugin_session_audio_stream_channel_int16_get(GatherGDIPlusSession *session, uint *length, uint stream_id, uint channel);
	uint gather_plugin_session_audio_stream_sample_rate_get(GatherGDIPlusSession *session, uint stream_id);
	void gather_plugin_session_image_stream_size(GatherGDIPlusSession *session, uint stream_id, uint *x, uint *y);
	boolean	gather_plugin_session_image_stream_high_dynamic_range(GatherGDIPlusSession *session, uint stream_id);
	uint8 *gather_plugin_session_image_stream_uint8_get(GatherGDIPlusSession *session, uint stream_id);
	float *gather_plugin_session_image_stream_float_get(GatherGDIPlusSession *session, uint stream_id);
	void gather_plugin_session_image_stream_active(GatherGDIPlusSession *session, uint stream_id, boolean active);
	void gather_plugin_session_audio_stream_active(GatherGDIPlusSession *session, uint stream_id, boolean active);
}


// call this once at application init
void gather_plugin_init()
{
    ULONG_PTR glidplusToken;
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&glidplusToken, &gdiplusStartupInput, NULL);
}

// returns malloc'd buffer on success, null on failure
void *load_data_to_argb(void *data, uint dataSize, uint *width, uint *height, uint max_size)
{
    void *argb = 0;
	HGLOBAL hSrcBuf = GlobalAlloc(GMEM_MOVEABLE, dataSize);
	LPVOID pSrcBuf = GlobalLock(hSrcBuf);
	CopyMemory(pSrcBuf, data, dataSize);
	GlobalUnlock(hSrcBuf);
	LPSTREAM pStream;
	HRESULT hr;
	if((hr = CreateStreamOnHGlobal(hSrcBuf, FALSE, &pStream)) == S_OK)
	{
		Bitmap *bmpSrc = Bitmap::FromStream(pStream);
			
		pStream->Release();
		if(bmpSrc)
		{
			int w = bmpSrc->GetWidth(), h = bmpSrc->GetHeight();
			if(w && h)
			{
				if(w > max_size || h > max_size)
				{
					if(w > h)
					{
						h = (h * max_size) / w;
						w = (w * max_size) / w;
					}else
					{
						w = (w * max_size) / h;
						h = (h * max_size) / h;
					}
				}
                *width = w;
                *height = h;
				argb = malloc(w * h * 4);
				ZeroMemory(argb, w * h * 4);
				Bitmap bmpArgb(w, h, w * 4, PixelFormat32bppARGB, (BYTE *)argb);
				Graphics gArgb(&bmpArgb);
				gArgb.DrawImage(bmpSrc, Rect(0, 0, w, h));
				
			}
			delete bmpSrc;
		}
	}
	GlobalFree(hSrcBuf);
    return argb;
}

uint8 *pl_emu_load_image(char *file_name, uint *width, uint *height, uint max_size)
{
	uint length, i = 0, size;
	uint8 *array, *image, rgba[4];
	FILE *f;
	if((f = fopen(file_name, "rb")) != NULL)
	{
		fseek(f, 0 , SEEK_END);
		length = ftell(f);
		rewind(f);
		array = (uint8 *)malloc(length);
		for(i = 0; i < length; i++)
			array[i] = fgetc(f);
		image = (uint8 *)load_data_to_argb((void *)array, length, width, height, max_size);
		size = *width * *height * 4;
		for(i = 0; i < size; i += 4)
		{
			rgba[0] = image[i];
			rgba[1] = image[i + 1];
			rgba[2] = image[i + 2];
			rgba[3] = image[i + 3];
			image[i] = rgba[2];
			image[i + 1] = rgba[1];
			image[i + 2] = rgba[0];
			image[i + 3] = rgba[3];
		}
		return image;
	}
	return NULL;
}

double gather_plugin_session_image_stream_frame_rate(void *session, uint stream_id)
{
	return 1;
}

GatherGDIPlusSession *gather_plugin_session_file_read_create(char *file_name)
{
	GatherGDIPlusSession *session;
	session = (GatherGDIPlusSession *)malloc(sizeof *session);
	session->data = pl_emu_load_image(file_name, &session->size_x, &session->size_y, 256 * 256);
	if(session->data == NULL)
	{
		free(session);
		return NULL;
	}
	return session;

}

double gather_plugin_session_duration_get(GatherGDIPlusSession *session)
{
	return 0;
}

void gather_plugin_session_seek(GatherGDIPlusSession *session, double time)
{
}

double gather_plugin_session_image_stream_frame_start(GatherGDIPlusSession *session, uint stream_id)
{
	return 0;
}

void gather_plugin_session_image_stream_frame_advance(GatherGDIPlusSession *session, uint stream_id)
{
}


double gather_plugin_session_audio_stream_frame_start(GatherGDIPlusSession *session, uint stream_id)
{
	return 0;
}

void gather_plugin_session_audio_stream_frame_advance(GatherGDIPlusSession *session, uint stream_id)
{
}

void gather_plugin_session_destroy(GatherGDIPlusSession *session)
{
	free(session->data);
	free(session);
}

uint gather_plugin_session_image_stream_count_get(GatherGDIPlusSession *session)
{
	return 1;
}

uint gather_plugin_session_audio_stream_count_get(GatherGDIPlusSession *session)
{
	return 0;
}


uint gather_plugin_session_audio_stream_channel_count_get(GatherGDIPlusSession *session, uint stream_id)
{
	return 0;
}

boolean	gather_plugin_session_audio_stream_high_dynamic_range(GatherGDIPlusSession *session, uint stream_id)
{
	return TRUE;
}

float *gather_plugin_session_audio_stream_channel_float_get(GatherGDIPlusSession *session, uint *length, uint stream_id, uint channel)
{
	return NULL;
}

int16 *gather_plugin_session_audio_stream_channel_int16_get(GatherGDIPlusSession *session, uint *length, uint stream_id, uint channel)
{
	return NULL;
}


uint gather_plugin_session_audio_stream_sample_rate_get(GatherGDIPlusSession *session, uint stream_id)
{
	return 0;
}


void gather_plugin_session_image_stream_size(GatherGDIPlusSession *session, uint stream_id, uint *x, uint *y)
{
	*x = session->size_x;
	*y = session->size_y;
}

boolean	gather_plugin_session_image_stream_high_dynamic_range(GatherGDIPlusSession *session, uint stream_id)
{
	return FALSE;
}

uint8 *gather_plugin_session_image_stream_uint8_get(GatherGDIPlusSession *session, uint stream_id)
{
	return session->data;
}

float *gather_plugin_session_image_stream_float_get(GatherGDIPlusSession *session, uint stream_id)
{
	return NULL;
}

void gather_plugin_session_image_stream_active(GatherGDIPlusSession *session, uint stream_id, boolean active)
{
}

void gather_plugin_session_audio_stream_active(GatherGDIPlusSession *session, uint stream_id, boolean active)
{
}

