/**
 * @file
 *
 * @author Anand Suresh <anandsuresh@gmail.com>
 * @copyright Copyright 2016 Anand Suresh
 * @license Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __NAV_TRANSCODER__
#define __NAV_TRANSCODER__

#include <nan.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
}

typedef struct {
  AVIOContext *io;
  AVFormatContext *ctx;
} NAVFormat;


typedef struct {
  AVStream *stream;
  AVCodec *codec;
  AVCodecContext *ctx;
} NAVStream;


typedef struct {
  NAVFormat format;
  NAVStream audio;
  NAVStream video;
} NAVSide;


typedef struct {
  char *outputFormatName;       // libAV format name (for av_guess_format)
  char *outputFormatFilename;   // filename of output (for av_guess_format)
  char *outputFormatMime;       // MIME type, if known (for av_guess_format)

  bool videoIgnore;             // Whether to drop video from the output
  char *videoCodecName;         // libAV video codec name
  int videoFrameRate;           // frame rate of the output video
  char *videoFrameSize;         // frame size of the output video
  char *videoAspectRatio;       // aspect ratio of the output video

  bool audioIgnore;             // Whether to drop audio from the output
  char *audioCodecName;         // libABV audio codec name
  int audioSampleRate;          // sample rate for the output audio
  int audioChannels;            // channels in the output audio
  int audioVolume;              // volume of the output audio
} NAVOpts;


typedef enum {
  OPEN_DEMUXER,
  READ_STREAM_INFO,
  FIND_BEST_AUDIO_STREAM,
  FIND_BEST_VIDEO_STREAM,
  INPUT_CHECK,

  OPEN_MUXER,
  CREATE_AUDIO_STREAM,
  CREATE_VIDEO_STREAM,

  WRITE_HEADER,
  DEMUX,
  DECODE_INPUT,
  DECODE_OUTPUT,
  ENCODE_INPUT,
  ENCODE_OUTPUT,
  MUX,
  WRITE_TRAILER
} NAVState;


/**
 * Describes the native AVTranscoder class
 */
class NAVTranscoder: public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

  private:
    NAVTranscoder(v8::Local < v8::Object >);
    ~NAVTranscoder();

    NAVStream *getInputStream(AVPacket *);
    NAVStream *getOutputStream(AVPacket *);

    static NAN_METHOD(New);
    static NAN_METHOD(Transcode);

    static NAN_METHOD(createDemuxer);
    static NAN_METHOD(createMuxer);

    static int read(void *, uint8_t *, int);
    static int write(void *, uint8_t *, int);
    static void getStreams(AVFormatContext *, v8::Handle < v8::Object > *);


    // I/O state
    NAVSide in;
    NAVSide out;

    // Custom I/O between C++ and JavaScript
    char *chunk;                    // Storage for JavaScript chunk
    int chunkSize;                  // Size of chunk remaining
    v8::Local < v8::Object > self;  // Used to store info.This() for emitting events

    // Misc state
    bool initialized;
    NAVState state;
    Nan::Persistent < v8::Object > opts;
};


#endif /* end of include guard: __NAV_TRANSCODER__ */
