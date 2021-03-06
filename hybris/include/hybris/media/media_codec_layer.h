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
 */

#ifndef MEDIA_CODEC_LAYER_H_
#define MEDIA_CODEC_LAYER_H_

#include <stdint.h>
#include <unistd.h>


#include <hybris/media/media_format_layer.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef void* MediaCodecDelegate;

    typedef void (*on_texture_needs_update)(void *context);
    void media_codec_set_texture_needs_update_cb(MediaCodecDelegate delegate, on_texture_needs_update cb, void *context);

    MediaCodecDelegate media_codec_create_by_codec_name(const char *name);
    MediaCodecDelegate media_codec_create_by_codec_type(const char *type);

// FIXME: This is a temporary function for testing:
#ifdef SIMPLE_PLAYER
    android::MediaCodec* media_codec_get(MediaCodecDelegate delegate);
#endif
// FIXME: Temporary

    MediaCodecDelegate media_codec_get_delegate();
    void media_codec_delegate_destroy(MediaCodecDelegate delegate);
    void media_codec_delegate_ref(MediaCodecDelegate delegate);
    void media_codec_delegate_unref(MediaCodecDelegate delegate);

    int media_codec_configure(MediaCodecDelegate delegate, MediaFormat format, uint32_t flags);

    int media_codec_queue_csd(MediaCodecDelegate delegate, MediaFormat format);
    int media_codec_start(MediaCodecDelegate delegate);
    int media_codec_stop(MediaCodecDelegate delegate);
    int media_codec_release(MediaCodecDelegate delegate);
    int media_codec_flush(MediaCodecDelegate delegate);
    size_t media_codec_get_input_buffers_size(MediaCodecDelegate delegate);
    uint8_t *media_codec_get_nth_input_buffer(MediaCodecDelegate delegate, size_t n);
    size_t media_codec_get_nth_input_buffer_capacity(MediaCodecDelegate delegate, size_t n);
    size_t media_codec_get_output_buffers_size(MediaCodecDelegate delegate);
    uint8_t *media_codec_get_nth_output_buffer(MediaCodecDelegate delegate, size_t n);
    size_t media_codec_get_nth_output_buffer_capacity(MediaCodecDelegate delegate, size_t n);

    struct _MediaCodecBufferInfo
    {
        size_t index;
        size_t offset;
        size_t size;
        int64_t presentation_time_us;
        uint32_t flags;
    };
    typedef struct _MediaCodecBufferInfo MediaCodecBufferInfo;

    int media_codec_dequeue_output_buffer(MediaCodecDelegate delegate, MediaCodecBufferInfo *info, int64_t timeout_us);
    int media_codec_queue_input_buffer(MediaCodecDelegate delegate, const MediaCodecBufferInfo *info);
    int media_codec_dequeue_input_buffer(MediaCodecDelegate delegate, size_t *index, int64_t timeout_us);
    int media_codec_release_output_buffer(MediaCodecDelegate delegate, size_t index, uint8_t render);

    MediaFormat media_codec_get_output_format(MediaCodecDelegate delegate);

#ifdef __cplusplus
}
#endif

#endif // MEDIA_CODEC_LAYER_H_
