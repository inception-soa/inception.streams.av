/**
 * @file The libav transcoding worker declaration
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

#ifndef __AV_TRANSCODE_WORKER__
#define __AV_TRANSCODE_WORKER__

#include <mutex>
#include <condition_variable>
#include <deque>

#include <nan.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}


/**
 * Describes an event; used to pass data back to JavaScript
 */
typedef struct {
  const char *eventName;
  const void *eventData;
} AVEvent;


typedef struct {
  AVIOContext *ctxIO;
  AVFormatContext *ctxFormat;
} AVDemuxer;


typedef struct {
  AVIOContext *ctxIO;
  AVFormatContext *ctxFormat;
} AVMuxer;


typedef struct {
  AVCodec *codec;
  AVCodecContext *ctxCodec;
  AVStream *stream;
} AVDecoder;


typedef struct {
  AVStream *stream;
  AVCodec *codec;
  AVCodecContext *ctxCodec;
} AVEncoder;


/**
 * Describes stats collected for the input and output streams
 */
typedef struct {
  uint64_t bytes;
  uint64_t packets;
  uint64_t frames;
  uint64_t samples;
} AVStats;


/**
 * Stores the input stream state
 */
typedef struct {
  AVDemuxer demuxer;
  AVDecoder video;
  AVDecoder audio;
  AVStats stats;

  bool ended;
} AVInput;


/**
 * Stores the output stream state
 */
typedef struct {
  AVMuxer muxer;
  AVEncoder video;
  AVEncoder audio;
  AVStats stats;
} AVOutput;


/**
 * Stores the transcoding options
 */
typedef struct {
  const char *outputFormatName;
  const char *outputFormatFilename;
  const char *outputFormatMime;
  struct {
    enum AVCodecID codec;
    char *frameRate;
    char *frameSize;
    char *aspect;
  } outputVideo;
  struct {
    enum AVCodecID codec;
    int sampleRate;
    int channels;
    int volume;
  } outputAudio;
} AVOpts;


/**
 *
 */
class Chunk {
  public:
    Chunk();
    ~Chunk();

    int Write(const char *, size_t);
    int Read(char *, size_t);

  private:
    std::mutex mu;
    std::condition_variable cond;
    char *data;
    size_t size;
};



/**
 * The AV transcode worker definition
 */
class AVTranscodeWorker: public Nan::AsyncProgressWorkerBase < AVEvent > {
  public:
    AVTranscodeWorker(AVOpts *, Nan::Callback *, Nan::Callback *);
    ~AVTranscodeWorker();

    bool EnqueueChunk(const char *, size_t);

  private:
    void Execute(const ExecutionProgress&);
    void HandleProgressCallback(const AVEvent *, size_t);

    int InitDemuxer(AVEvent *);
    int InitDecoders(AVEvent *);
    int InitDecoder(AVMediaType, AVEvent *);

    int InitMuxer(AVEvent *);
    int InitEncoders(AVEvent *);
    int InitEncoder(AVMediaType, AVEvent *);

    int Transcode(AVEvent *);

    static int readPacket(void *, uint8_t *, int);
    static int writePacket(void *, uint8_t *, int);

    AVInput input_;
    AVOutput output_;
    AVOpts opts_;
    Nan::Callback *emit_;
}


#endif /* end of include guard: __AV_TRANSCODE_WORKER__ */
