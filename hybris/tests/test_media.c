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

#include <hybris/media/media_compatibility_layer.h>
#include <hybris/media/media_codec_layer.h>

#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "parser.h"

#define FRAME_SIZE 1000000
enum {
	OK          = 0,
	NO_ERROR    = 0,
};

static struct mfc_parser_context ctx = {0};
int main(int argc, char **argv)
{
	MediaCodecBufferInfo input;
	MediaCodecBufferInfo output;
	int fd;
	size_t size;
	struct stat statbuf;
	static int64_t timeout = 500ll;
	int used, fs;
	uint8_t *head = malloc(4096);
	uint8_t *frm = malloc(FRAME_SIZE);
	uint8_t *buf = NULL;
	int ret;

	size_t i = 0;
	if (argc < 2) {
		printf("Usage: test_media <video_to_play>\n");
		return EXIT_FAILURE;
	}
	if ((fd = open(argv[1], O_RDONLY, 0)) == -1)
		err(1, "open");

	 /* find size of input file */
	if (fstat (fd,&statbuf) < 0)
		err("fstat error");

	uint8_t *file = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(file == NULL)
		err(1, "mmap");
	printf("Input %u bytes mapped at %x\n", statbuf.st_size, file);
	size = statbuf.st_size;
	buf = file;
	readahead(fd, 0, size);

	parse_stream_init(&ctx);
	MediaFormat format = media_format_create_video_format("video/avc", 1920, 1080, 0, 0);
	if (!format) {
		printf("Format not created!");
	}

	if(!parse_h264_stream(&ctx, buf, size, head, 4096, &used, &fs, 1)) {
		printf("Could not parse stream!\n");
		return -1;
	}

	printf("Streamer header OK! fs = %d used = %d\n", fs, used);
	media_format_set_byte_buffer(format, "csd-0", head, fs);
//	media_format_set_byte_buffer(format, "csd-1", head + 31, 10);

	MediaCodecDelegate codec = media_codec_create_by_codec_type("video/avc");
	//media_codec_queue_csd(codec, format);
	if (!codec) {
		printf("Codec not created!\n");
		return -1;
	}
	if (media_codec_configure(codec, format, 0) != OK) {
		printf("Codec configure failed!\n");
	}

	media_codec_start(codec);
	printf("Input buffers: %u\n", media_codec_get_input_buffers_size(codec));
	printf("Output buffers: %u\n", media_codec_get_output_buffers_size(codec));
	int frames = 0;
	bool parser_finished = false;
	bool signaled_eos = false;
	while(true) {
		if(!parser_finished) {
			if(!parse_h264_stream(&ctx, buf, size, frm, FRAME_SIZE, &used, &fs, 0)) {
				printf("Parse stream finished!\n");
				parser_finished = true;
				goto dequeue;
			}
			buf += used;
			size -= used;
	 		ret = media_codec_dequeue_input_buffer(codec, &i, timeout);;
			if(ret != OK) {
				if(ret != -1) {
					printf("failed to deque input buffer %d!", ret);
					break;
				}
				goto dequeue;
				
			}
			uint8_t *p = media_codec_get_nth_input_buffer(codec, i);
			memcpy(p, frm, fs);
			input.index = i;
			input.offset = 0;
			input.size = fs;
			input.presentation_time_us = 0;
			input.flags = 0;
			if(media_codec_queue_input_buffer(codec, &input) != OK) {
				printf("Failed to queue input buffer!\n");
				break;
			}
		}
		if(parser_finished && !signaled_eos) {
	 		ret = media_codec_dequeue_input_buffer(codec, &i, timeout);;
			if(ret != OK) {
				if(ret != -1) {
					printf("failed to deque input buffer %d!", ret);
					break;
				}
				goto dequeue;
			}
			input.index = i;
			input.offset = 0;
			input.size = fs;
			input.presentation_time_us = 0;
			input.flags = 4; //BUFFER_FLAG_EOS
			if(media_codec_queue_input_buffer(codec, &input) != OK) {
				printf("Failed to queue input buffer!\n");
				break;
			}
			signaled_eos = true;
	
		}
dequeue:
		ret = media_codec_dequeue_output_buffer(codec, &output, timeout);
		switch(ret) {
		case -1: //INFO_TRY_AGAIN_LATER: 
			//printf("Try again to dequeue output buffer!\n");
			continue;
			break;
		case -3: //INFO_OUTPUT_BUFFERS_CHANGED:
			printf("Output buffer number changed!\n");
		        printf("Output buffers: %u\n", media_codec_get_output_buffers_size(codec));
			continue;
			break;
		case -2: //INFO_OUTPUT_FORMAT_CHANGED
			printf("Output format changed!\n");
			media_codec_get_output_format(codec);
			break;
		case OK:
			frames++;
			media_codec_release_output_buffer(codec, output.index, 0);
			usleep(50000);
			if (output.flags & 4) {
				printf("Finish output stream!\n");
				goto finish;
			}
			break;
		default:
			printf("Error dequeueing output buffer %d!\n", ret);
			return -1;
			break;		
		}

	}
finish:
	printf("Decoded %d frames!\n", frames);
	media_codec_stop(codec);
	media_codec_delegate_destroy(codec);
	media_format_destroy(format);
	//media_format_destroy(output);
	munmap(file, statbuf.st_size);
	close(fd);
	return EXIT_SUCCESS;
}
