#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "betray.h"
#include "relinquish.h" /*little opengl wrapper i use, in the case only to draw lines*/

#define inline

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#define G_FFMPEG_FRAME_STORAGE 8


typedef enum{
	GPF_ST_VIDEO,
	GPF_ST_AUDIO,
	GPF_ST_INVALID = -1
}GPFStreamType;

typedef struct{
	GPFStreamType type;
	AVFrame *pFrameRGB[G_FFMPEG_FRAME_STORAGE];
	struct SwsContext *image_conversion;
	uint8_t *buffer[8];
	uint used;
	uint eaten;
	double time_stamp[8];
	boolean active;
}GatherFFMpegStream;

typedef struct{
	AVFormatContext *pFormatCtx;
	GatherFFMpegStream *streams;
	AVFrame *pFrame;
}GatherFFMpegSession;

uint gather_plugin_session_find_stream(GatherFFMpegSession *session, GPFStreamType type, uint stream_id)
{
	uint i, j;
	for(i = j = 0; i < session->pFormatCtx->nb_streams; i++)
		if(session->streams[i].type == type)
			if(j++ == stream_id)
				return i;
	return -1;
}

void gather_plugin_init()
{
	av_register_all();
}

double gather_plugin_session_image_stream_frame_rate(void *session, uint stream_id)
{

}

GatherFFMpegSession *gather_plugin_session_file_read_create(char *file_name)
{
	GatherFFMpegSession *session;
/*	GatherFFMpegStream *streams;*/

	AVFormatContext *pFormatCtx = NULL;
	uint i, j, buffer_size;
	AVCodecContext *pCodecCtx = NULL;
	AVCodec *pCodec = NULL;
	boolean video = FALSE;

	AVDictionary *optionsDict = NULL;
	struct SwsContext *sws_ctx = NULL;
  // Register all formats and codecs

  // Open video file
	if(avformat_open_input(&pFormatCtx, file_name, NULL, NULL)!=0)
		return NULL; // Couldn't open file
  // Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
		return NULL; 

	session = malloc(sizeof *session);
	session->streams = malloc((sizeof *session->streams) * pFormatCtx->nb_streams);
	buffer_size = 0;
	for(i = 0; i < pFormatCtx->nb_streams; i++)
	{
		session->streams[i].type = GPF_ST_INVALID;
		session->streams[i].active = TRUE;
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			pCodecCtx = pFormatCtx->streams[i]->codec;
			pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
			if(pCodec != NULL && avcodec_open2(pCodecCtx, pCodec, &optionsDict) >= 0)
			{
				buffer_size = avpicture_get_size(PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height);
				session->streams[i].type = GPF_ST_VIDEO;
				session->streams[i].image_conversion = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGBA, SWS_FAST_BILINEAR, NULL, NULL, NULL);
				for(j = 0; j < G_FFMPEG_FRAME_STORAGE; j++)
				{
					session->streams[i].buffer[j] = (uint8_t *)av_malloc(buffer_size * sizeof(uint8_t));
					session->streams[i].pFrameRGB[j] = avcodec_alloc_frame();
					if(session->streams[i].pFrameRGB[j] != NULL)
					{
						avpicture_fill((AVPicture *)session->streams[i].pFrameRGB[j], session->streams[i].buffer[j], PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height);
						video = TRUE;
					}
				}
				session->streams[i].used = 0;  
				session->streams[i].eaten = 0;
			}
		}
		if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			pCodecCtx = pFormatCtx->streams[i]->codec;
			pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
			if(pCodec != NULL && avcodec_open2(pCodecCtx, pCodec, &optionsDict) >= 0)
			{
				session->streams[i].type = GPF_ST_AUDIO;

				if(pCodecCtx->sample_fmt == AV_SAMPLE_FMT_FLT ||
					pCodecCtx->sample_fmt == AV_SAMPLE_FMT_DBL ||
					pCodecCtx->sample_fmt == AV_SAMPLE_FMT_FLTP ||
					pCodecCtx->sample_fmt == AV_SAMPLE_FMT_DBLP)
				{
					for(j = 0; j < pCodecCtx->channels && j < 8; j++)
						session->streams[i].buffer[j] = malloc(sizeof(float) * pCodecCtx->sample_rate);
				}else
				{
					for(j = 0; j < pCodecCtx->channels && j < 8; j++)
						session->streams[i].buffer[j] = malloc(sizeof(int16) * pCodecCtx->sample_rate);
				}
				for(; j < 8; j++)
					session->streams[i].buffer[j] = NULL;
				session->streams[i].used = 0;  
				session->streams[i].eaten = 0;
			}
		}
	}
	session->pFormatCtx = pFormatCtx;
	if(video)
		session->pFrame = avcodec_alloc_frame();
	else
		session->pFrame = NULL;
	return session;
}

double gather_plugin_session_duration_get(GatherFFMpegSession *session)
{

	return (double)session->pFormatCtx->duration / (double)AV_TIME_BASE;
}



void session_read(GatherFFMpegSession *session)
{
	static FILE *text_output = NULL;
	AVPacket packet;
	uint i, j, k;
	int frameFinished;
	if(text_output == NULL)
		text_output = fopen("ffmpeg_debug.txt", "w"); 

	for(i = 0; i < session->pFormatCtx->nb_streams; i++)
		if(session->streams[i].type == GPF_ST_VIDEO && session->streams[i].used == G_FFMPEG_FRAME_STORAGE)
			return;
 
	while(av_read_frame(session->pFormatCtx, &packet) >= 0)
	{
		if(session->streams[packet.stream_index].active)
		{
			// Is this a packet from the video stream?
			if(packet.stream_index < session->pFormatCtx->nb_streams && packet.stream_index >= 0 && session->streams[packet.stream_index].type == GPF_ST_VIDEO)
			{
			
				AVCodecContext *pCodecCtx = NULL;
				pCodecCtx = session->pFormatCtx->streams[packet.stream_index]->codec;
				// Decode video frame
				avcodec_decode_video2(pCodecCtx, session->pFrame, &frameFinished, &packet);
      
				// Did we get a video frame?
				if(frameFinished && session->streams[packet.stream_index].pFrameRGB[session->streams[packet.stream_index].used] != NULL)
				{
					double time;
					time = ((double)session->pFrame->pkt_pts * (double)session->pFormatCtx->streams[packet.stream_index]->time_base.num) / (double)session->pFormatCtx->streams[packet.stream_index]->time_base.den;
					session->streams[packet.stream_index].time_stamp[session->streams[packet.stream_index].used] = time;
					sws_scale(session->streams[packet.stream_index].image_conversion,
							(uint8_t const * const *)session->pFrame->data,
							session->pFrame->linesize,
							0,
							pCodecCtx->height,
							session->streams[packet.stream_index].pFrameRGB[session->streams[packet.stream_index].used]->data,
							session->streams[packet.stream_index].pFrameRGB[session->streams[packet.stream_index].used]->linesize);
					av_frame_unref(session->pFrame);
					if(++session->streams[packet.stream_index].used >= G_FFMPEG_FRAME_STORAGE)
					{
						av_free_packet(&packet);
						return;
					}
				}
			}
			if(packet.stream_index < session->pFormatCtx->nb_streams && packet.stream_index >= 0 && session->streams[packet.stream_index].type != GPF_ST_VIDEO && session->streams[packet.stream_index].type != GPF_ST_INVALID)
			{
				AVCodecContext *pCodecCtx = NULL;
				pCodecCtx = session->pFormatCtx->streams[packet.stream_index]->codec;
				fprintf(text_output, "Sound A\n");
				if(0 <= avcodec_decode_audio4(pCodecCtx, session->pFrame, &frameFinished, &packet) && frameFinished)
				{
					uint channels, length, eaten;
					int16 *iout;
					float *fout;
					double time;
					time = ((double)session->pFrame->pkt_pts * (double)session->pFormatCtx->streams[packet.stream_index]->time_base.num) / (double)session->pFormatCtx->streams[packet.stream_index]->time_base.den;
					fprintf(text_output, " Audio PTS %u %u %f\n", session->pFrame->pts, session->pFrame->pkt_pts, time);

					channels = session->pFrame->channels;
					fprintf(text_output, "Sound %u %u\n", pCodecCtx->sample_fmt, AV_SAMPLE_FMT_S16);
					switch(pCodecCtx->sample_fmt)
					{
						case AV_SAMPLE_FMT_U8 :          ///< unsigned 8 bits
						break;
						case AV_SAMPLE_FMT_S16 :         ///< signed 16 bits
							{
								uint16 *data;
								data = session->pFrame->data[0];
								for(i = 0; i < channels; i++)
								{
									length = session->pFrame->linesize[0] / (sizeof(int16));
									if(length > pCodecCtx->sample_rate - session->streams[packet.stream_index].used)
										length = pCodecCtx->sample_rate - session->streams[packet.stream_index].used;
									iout = (int16 *)session->streams[packet.stream_index].buffer[0];
									iout = &iout[session->streams[packet.stream_index].used];
									k = 0;
									for(j = i; j < length * channels; j += channels)
										iout[k++] = data[j];
								}
								if(session->streams[packet.stream_index].used == 0)
									session->streams[packet.stream_index].time_stamp[0] = ((double)session->pFrame->pkt_pts * (double)session->pFormatCtx->streams[packet.stream_index]->time_base.num) / (double)session->pFormatCtx->streams[packet.stream_index]->time_base.den;
								session->streams[packet.stream_index].used += length;
							}
						break;
						case AV_SAMPLE_FMT_S32 :         ///< signed 32 bits
						break;
						case AV_SAMPLE_FMT_FLT :         ///< float
						break;
						case AV_SAMPLE_FMT_DBL :         ///< double
						break;
						case AV_SAMPLE_FMT_U8P :         ///< unsigned 8 bits, planar
						break;
						case AV_SAMPLE_FMT_S16P :        ///< signed 16 bits, planar
						break;
						case AV_SAMPLE_FMT_S32P :        ///< signed 32 bits, planar
						break;
						case AV_SAMPLE_FMT_FLTP :        ///< float, planar
							{
								float *data, f;
								length = session->pFrame->linesize[0] / (sizeof *data) / channels;
								if(length > pCodecCtx->sample_rate - session->streams[packet.stream_index].used)
									length = pCodecCtx->sample_rate - session->streams[packet.stream_index].used;
								for(i = 0; i < channels; i++)
								{
									data = session->pFrame->data[i];
									fout = session->streams[packet.stream_index].buffer[i];
									fout = &fout[session->streams[packet.stream_index].used];
									for(j = 0; j < length; j++)
										fout[j] = data[j];
								}
								if(session->streams[packet.stream_index].used == 0)
									session->streams[packet.stream_index].time_stamp[0] = ((double)session->pFrame->pkt_pts * (double)session->pFormatCtx->streams[packet.stream_index]->time_base.num) / (double)session->pFormatCtx->streams[packet.stream_index]->time_base.den;
								session->streams[packet.stream_index].used += length;
							}
						break;
						case AV_SAMPLE_FMT_DBLP :        ///< double, planar
						break;
					}
					av_frame_unref(session->pFrame);
				}
			}
		}
		av_free_packet(&packet);
	}
}

extern void gather_plugin_session_image_stream_frame_advance(GatherFFMpegSession *session, uint stream_id);

void gather_plugin_session_seek(GatherFFMpegSession *session, double time)
{
	static FILE *file = NULL;
	uint i, j, k;
	if(file == NULL)
		file = fopen("seek_debug_file.txt", "a");
	av_seek_frame(session->pFormatCtx, -1, (int64_t)(time * (double)AV_TIME_BASE), AVSEEK_FLAG_BACKWARD);
	for(i = 0; i < session->pFormatCtx->nb_streams; i++)
		if(session->streams[i].type == GPF_ST_VIDEO && session->streams[i].type == GPF_ST_AUDIO)
			session->streams[i].used = 0;
	fprintf(file, "Starting time: %f\n", time);
	for(i = 0; i < session->pFormatCtx->nb_streams; i++)
	{
		if(session->streams[i].type == GPF_ST_VIDEO)
		{
			AVCodecContext *pCodecCtx = NULL;
			pCodecCtx = session->pFormatCtx->streams[i]->codec;
		}
	}
	for(j = 0; /*j < 100*/; j++)
	{
		session_read(session);
		for(i = 0; i < session->pFormatCtx->nb_streams; i++)
			if(session->streams[i].type == GPF_ST_VIDEO)
			{	
				fprintf(file, "Seek time: (%f -> %f) == %f\n", session->streams[i].time_stamp[0], session->streams[i].time_stamp[1], time);
				if((session->streams[i].time_stamp[0] <= time && session->streams[i].time_stamp[1] > time) ||
					(session->streams[i].time_stamp[0] > session->streams[i].time_stamp[1] && session->streams[i].time_stamp[1] >= time))
						return;
			}
		for(i = k = 0; i < session->pFormatCtx->nb_streams; i++)
			if(session->streams[i].type == GPF_ST_VIDEO)
				gather_plugin_session_image_stream_frame_advance(session, k++);

	}
	fclose(file);

}



double gather_plugin_session_image_stream_frame_start(GatherFFMpegSession *session, uint stream_id)
{
	uint i;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_VIDEO, stream_id)) == -1)
		return 77777777777.0;
	if(session->streams[i].used < 2)
	{
		session_read(session);
		return 10000000000.0;
	}
	return session->streams[i].time_stamp[0];
}

void gather_plugin_session_image_stream_frame_advance(GatherFFMpegSession *session, uint stream_id)
{
	AVFrame *frame;
	void *buffer;
	uint i, j;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_VIDEO, stream_id)) == -1)
		return;
	session_read(session);
	if(session->streams[i].used == 0)
		return;
	
	session->streams[i].used--;

	frame = session->streams[i].pFrameRGB[0];
	buffer = session->streams[i].buffer[0];
	for(j = 0; j < G_FFMPEG_FRAME_STORAGE - 1; j++)
	{
		session->streams[i].time_stamp[j] = session->streams[i].time_stamp[j + 1];
		session->streams[i].pFrameRGB[j] = session->streams[i].pFrameRGB[j + 1];
		session->streams[i].buffer[j] = session->streams[i].buffer[j + 1];
	}
	session->streams[i].pFrameRGB[j] = frame;
	session->streams[i].buffer[j] = buffer;
	session_read(session);
}


double gather_plugin_session_audio_stream_frame_start(GatherFFMpegSession *session, uint stream_id)
{
	uint i, j;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_AUDIO, stream_id)) == -1)
		return 10000000000.0;
	if(session->streams[i].used < 2)
	{
		session_read(session);
		return 10000000000.0;
	}
	return session->streams[i].time_stamp[0];
}

void gather_plugin_session_audio_stream_frame_advance(GatherFFMpegSession *session, uint stream_id)
{
	AVFrame *frame;
	void *buffer;
	uint i, j, k, length, eaten;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_AUDIO, stream_id)) == -1)
		return;

	eaten = session->streams[i].eaten;
	length = session->streams[i].used;
	for(j = 0; j < 8 && session->streams[i].buffer[j] != 0; j++)
	{
		if(TRUE) /* */
		{
			float *fout;
			fout = session->streams[i].buffer[j];
			for(k = eaten; k < length; k++)
				fout[k - eaten] = fout[k];
		}else
		{
			int16 *iout;
			iout = session->streams[i].buffer[j];
			for(k = eaten; k < length; k++)
				iout[k - eaten] = iout[k];
		}
	}
	session->streams[i].used -= session->streams[i].eaten;
	session->streams[i].eaten = 0;
	session_read(session);
}

void gather_plugin_session_destroy(GatherFFMpegSession *session)
{
	uint i, j;
	for(i = 0; i < session->pFormatCtx->nb_streams; i++)
	{
		if(session->streams[i].type == GPF_ST_AUDIO)
		{
			for(j = 0; j < 8; j++)
				if(session->streams[i].buffer[j] != NULL)
					free(session->streams[i].buffer[j]);
			avcodec_close(session->pFormatCtx->streams[i]->codec);
		}
		if(session->streams[i].type == GPF_ST_VIDEO)
		{
			for(j = 0; j < G_FFMPEG_FRAME_STORAGE; j++)
			{
				if(session->streams[i].buffer[j] != NULL)
					av_free(session->streams[i].buffer[j]);
				if(session->streams[i].buffer[j] != NULL)
					av_free(session->streams[i].pFrameRGB[j]);
			}
			avcodec_close(session->pFormatCtx->streams[i]->codec);
		}
	}
	av_free(session->pFrame);
	avformat_close_input(&session->pFormatCtx);
	free(session);
}

uint gather_plugin_session_image_stream_count_get(GatherFFMpegSession *session)
{
	uint i, j;
	for(i = j = 0; i < session->pFormatCtx->nb_streams; i++)
		if(session->streams[i].type == GPF_ST_VIDEO)
			j++;
	return j;
}

uint gather_plugin_session_audio_stream_count_get(GatherFFMpegSession *session)
{
	uint i, j;
	for(i = j = 0; i < session->pFormatCtx->nb_streams; i++)
		if(session->streams[i].type == GPF_ST_AUDIO)
			j++;

	return j;
}


uint gather_plugin_session_audio_stream_channel_count_get(GatherFFMpegSession *session, uint stream_id)
{
	AVCodecContext *pCodecCtx = NULL;
	uint i;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_AUDIO, stream_id)) == -1)
		return 0;
	pCodecCtx = session->pFormatCtx->streams[i]->codec;
	return pCodecCtx->channels;
}

boolean	gather_plugin_session_audio_stream_high_dynamic_range(GatherFFMpegSession *session, uint stream_id)
{
	return TRUE;
}

float *gather_plugin_session_audio_stream_channel_float_get(GatherFFMpegSession *session, uint *length, uint stream_id, uint channel)
{
	AVCodecContext *pCodecCtx = NULL;
	float *buf;
	uint i, j;
	session_read(session);
	if((i = gather_plugin_session_find_stream(session, GPF_ST_AUDIO, stream_id)) == -1)
		return NULL;
	if(session->streams[i].used == 0)
		return NULL;
	pCodecCtx = session->pFormatCtx->streams[i]->codec;
	if(session->streams[i].eaten == 0)
		session->streams[i].eaten = session->streams[i].used;
	if(session->streams[i].time_stamp[0] < 0.00001)
	{
		float *output;
		*length = session->streams[i].eaten - pCodecCtx->delay;
		output = (float *)session->streams[i].buffer[channel];
		return &output[pCodecCtx->delay];
	}else
	{
		*length = session->streams[i].eaten;
		return (float *)session->streams[i].buffer[channel];
	}
}

int16 *gather_plugin_session_audio_stream_channel_int16_get(GatherFFMpegSession *session, uint *length, uint stream_id, uint channel)
{
	float *buf;
	uint i, j;
	session_read(session);
	if((i = gather_plugin_session_find_stream(session, GPF_ST_AUDIO, stream_id)) == -1)
		return NULL;	
	if(session->streams[i].used == 0)
		return NULL;
	if(session->streams[i].eaten == 0)
		session->streams[i].eaten = session->streams[i].used;
	*length = session->streams[i].eaten;
	return (uint16 *)session->streams[i].buffer[channel];
}


uint gather_plugin_session_audio_stream_sample_rate_get(GatherFFMpegSession *session, uint stream_id)
{
	AVCodecContext *pCodecCtx = NULL;
	uint i, j;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_AUDIO, stream_id)) == -1)
		return NULL;
	pCodecCtx = session->pFormatCtx->streams[i]->codec;
	return pCodecCtx->sample_rate;
}


void gather_plugin_session_image_stream_size(GatherFFMpegSession *session, uint stream_id, uint *x, uint *y)
{
	AVCodecContext *pCodecCtx = NULL;
	uint i, j;
	*x = *y = 0;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_VIDEO, stream_id)) == -1)
		return;
	pCodecCtx = session->pFormatCtx->streams[i]->codec;
	*x = pCodecCtx->width;
	*y = pCodecCtx->height;	
}

boolean	gather_plugin_session_image_stream_high_dynamic_range(GatherFFMpegSession *session, uint stream_id)
{
	return FALSE;
}

uint8 *gather_plugin_session_image_stream_uint8_get(GatherFFMpegSession *session, uint stream_id)
{
	AVFrame *frame;
	void *buffer;
	uint i, j;
	session_read(session);
	if((i = gather_plugin_session_find_stream(session, GPF_ST_VIDEO, stream_id)) == -1)
		return NULL;
	if(session->streams[i].used == 0)
		return NULL;
	frame = session->streams[i].pFrameRGB[0];
	buffer = session->streams[i].buffer[0];
	return buffer;
}

float *gather_plugin_session_image_stream_float_get(GatherFFMpegSession *session, uint stream_id)
{
	return NULL;
}

void gather_plugin_session_image_stream_active(GatherFFMpegSession *session, uint stream_id, boolean active)
{
	uint i;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_VIDEO, stream_id)) == -1)
		return;
	session->streams[i].active = active;
}
void gather_plugin_session_audio_stream_active(GatherFFMpegSession *session, uint stream_id, boolean active)
{
	uint i;
	if((i = gather_plugin_session_find_stream(session, GPF_ST_AUDIO, stream_id)) == -1)
		return;
	session->streams[i].active = active;
}

