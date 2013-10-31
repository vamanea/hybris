/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Authored by: Jim Hodapp <jim.hodapp@canonical.com>
 *              Ricardo Salveti de Araujo <ricardo.salveti@canonical.com>
 */

#include <assert.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdbool.h>

#include <hybris/internal/binding.h>
#include <hybris/media/media_compatibility_layer.h>
#include <hybris/media/recorder_compatibility_layer.h>
#include <hybris/media/media_codec_layer.h>
#include <hybris/media/media_codec_list.h>
#include <hybris/media/media_format_layer.h>
#include <hybris/media/surface_texture_client_hybris.h>

#define COMPAT_LIBRARY_PATH "/system/lib/libmedia_codec_compat_layer.so"

#ifdef __ARM_PCS_VFP
#define FP_ATTRIB __attribute__((pcs("aapcs")))
#else
#define FP_ATTRIB
#endif

HYBRIS_LIBRARY_INITIALIZE(media, COMPAT_LIBRARY_PATH);

int media_compat_check_availability()
{
	/* Both are defined via HYBRIS_LIBRARY_INITIALIZE */
	hybris_media_initialize();
	return media_handle ? 1 : 0;
}

// Media Codecs
HYBRIS_IMPLEMENT_FUNCTION1(media, MediaCodecDelegate,
	media_codec_create_by_codec_name, const char*);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(media, media_codec_delegate_destroy,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(media, media_codec_delegate_ref,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(media, media_codec_delegate_unref,
	MediaCodecDelegate);

HYBRIS_IMPLEMENT_FUNCTION4(media, int, media_codec_configure,
	MediaCodecDelegate, MediaFormat, uint32_t);
HYBRIS_IMPLEMENT_FUNCTION2(media, int, media_codec_queue_csd,
	MediaCodecDelegate, MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int, media_codec_start,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_FUNCTION1(media, int, media_codec_stop,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_FUNCTION1(media, int, media_codec_release,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_FUNCTION1(media, int, media_codec_flush,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_FUNCTION1(media, size_t, media_codec_get_input_buffers_size,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_FUNCTION2(media, uint8_t*, media_codec_get_nth_input_buffer,
	MediaCodecDelegate, size_t);
HYBRIS_IMPLEMENT_FUNCTION2(media, size_t, media_codec_get_nth_input_buffer_capacity,
	MediaCodecDelegate, size_t);
HYBRIS_IMPLEMENT_FUNCTION1(media, size_t, media_codec_get_output_buffers_size,
	MediaCodecDelegate);
HYBRIS_IMPLEMENT_FUNCTION2(media, uint8_t*, media_codec_get_nth_output_buffer,
	MediaCodecDelegate, size_t);
HYBRIS_IMPLEMENT_FUNCTION2(media, size_t, media_codec_get_nth_output_buffer_capacity,
	MediaCodecDelegate, size_t);
HYBRIS_IMPLEMENT_FUNCTION3(media, int, media_codec_dequeue_output_buffer,
	MediaCodecDelegate, MediaCodecBufferInfo*, int64_t);
HYBRIS_IMPLEMENT_FUNCTION2(media, int, media_codec_queue_input_buffer,
	MediaCodecDelegate, const MediaCodecBufferInfo*);
HYBRIS_IMPLEMENT_FUNCTION3(media, int, media_codec_dequeue_input_buffer,
	MediaCodecDelegate, size_t*, int64_t);
HYBRIS_IMPLEMENT_FUNCTION3(media, int, media_codec_release_output_buffer,
	MediaCodecDelegate, size_t, uint8_t);
HYBRIS_IMPLEMENT_FUNCTION1(media, MediaFormat, media_codec_get_output_format,
	MediaCodecDelegate);

HYBRIS_IMPLEMENT_FUNCTION5(media, MediaFormat, media_format_create_video_format,
	const char*, int32_t, int32_t, int64_t, int32_t);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(media, media_format_destroy,
	MediaFormat);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(media, media_format_ref,
	MediaFormat);
HYBRIS_IMPLEMENT_VOID_FUNCTION1(media, media_format_unref,
	MediaFormat);
HYBRIS_IMPLEMENT_VOID_FUNCTION4(media, media_format_set_byte_buffer,
	MediaFormat, const char*, uint8_t*, size_t);
HYBRIS_IMPLEMENT_FUNCTION1(media, const char*, media_format_get_mime,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int64_t, media_format_get_duration_us,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_width,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_height,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_max_input_size,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_stride,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_slice_height,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_color_format,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_crop_left,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_crop_right,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_crop_top,
	MediaFormat);
HYBRIS_IMPLEMENT_FUNCTION1(media, int32_t, media_format_get_crop_bottom,
	MediaFormat);

