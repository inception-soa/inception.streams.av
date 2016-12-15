/**
 * @file Transcodes input streams
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

#include "nav_types.h"
#include "nav_transcoder.h"
extern "C" {
#include <libavutil/channel_layout.h>
}


#define LOG(fmt, ...)                                                    \
  fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__, __func__, \
          ## __VA_ARGS__)

#define LOG_CODEC_CONTEXT(ctx)                                     \
  do {                                                             \
    fprintf(stderr, "Codec context(%p):\n", ctx);                  \
    fprintf(stderr, "\t:Codec ID: %d\n", ctx->codec->id);          \
    fprintf(stderr, "\t:Codec Name: %s\n", ctx->codec->name);      \
    fprintf(stderr, "\t:Codec Type: %d\n", ctx->codec->type);      \
    fprintf(stderr, "\t:Codec Desc: %s\n", ctx->codec->long_name); \
    fprintf(stderr, "\t:Bit-rate: %d\n", ctx->bit_rate);           \
    fprintf(stderr, "\t:Sample rate: %d\n", ctx->sample_rate);     \
    fprintf(stderr, "\t:Channels: %d\n", ctx->channels);           \
    fprintf(stderr, "\t:Frame Size: %d\n", ctx->frame_size);       \
  } while (0)

#define GET_STRING_OPT(k)          \
  (*Nan::Utf8String(Nan::Get(opts, \
                             Nan::New(k).ToLocalChecked()).ToLocalChecked()))

#define GET_NUMBER_OPT(k) \
  (Nan::Get(opts, Nan::New(k).ToLocalChecked()).ToLocalChecked()->Uint32Value())

#define GET_BOOLEAN_OPT(k) \
  (Nan::Get(opts,          \
            Nan::New(k).ToLocalChecked()).ToLocalChecked()->BooleanValue())

#define THROW_AND_RETURN(errno, func)                              \
  do {                                                             \
    LOG("%s() failed with %x", func, errno);                       \
    Nan::ThrowError(Nan::ErrnoException(errno, func, NULL, NULL)); \
    return;                                                        \
  } while (0)



int decode_packet(NAVStream *in, AVPacket *pkt, AVFrame *frame) {
  int ret;

  ret = avcodec_send_packet(in->ctx, pkt);
  if (ret < 0) {
    LOG("avcodec_send_packet(): %d", ret);
    return ret;
  }

  ret = avcodec_receive_frame(in->ctx, frame);
  if ((ret < 0) && (ret != AVERROR(EAGAIN)) && (ret != AVERROR_EOF)) {
    LOG("avcodec_receive_frame(): %d", ret);
    return ret;
  }

  return 0;
}


/**
 * NAVTranscoder constructor
 */
NAVTranscoder::NAVTranscoder(v8::Local<v8::Object> options) {
  in.format.io = out.format.io = NULL;
  in.format.ctx = out.format.ctx = NULL;

  in.audio.stream = out.video.stream = NULL;
  in.audio.codec = out.video.codec = NULL;
  in.audio.ctx = out.video.ctx = NULL;

  chunk = NULL;
  chunkSize = 0;

  state = OPEN_DEMUXER;
  opts.Reset(options);
}


/**
 * NAVTranscoder destructor
 */
NAVTranscoder::~NAVTranscoder() {
  opts.Reset();
}


/**
 * Returns whether or not the specified packet is to be processed
 */
NAVStream *NAVTranscoder::getInputStream(AVPacket *pkt) {
  if (pkt == NULL)
    return NULL;

  if (in.audio.stream)
    if (in.audio.stream->index == pkt->stream_index)
      return &in.audio;

  if (in.video.stream)
    if (in.video.stream->index == pkt->stream_index)
      return &in.video;

  return NULL;
}


NAVStream *NAVTranscoder::getOutputStream(AVPacket *pkt) {
  if (pkt == NULL)
    return NULL;

  if (in.audio.stream)
    if (in.audio.stream->index == pkt->stream_index)
      return &out.audio;

  if (in.video.stream)
    if (in.video.stream->index == pkt->stream_index)
      return &out.video;

  return NULL;
}


/**
 * NAVTranscoder constructor for JavaScript
 */
NAN_METHOD(NAVTranscoder::New) {
  if (info.IsConstructCall()) {
    auto self = new NAVTranscoder(info[0].As<v8::Object>());
    self->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    LOG("NAVTranscoder initialized!");
    return;
  }

  LOG("NAVTranscoder initialization in non-construct mode!");
  std::vector<v8::Local<v8::Value> > args(info.Length());
  for (size_t i = 0; i < args.size(); i++) {
    args[i] = info[i];
  }

  auto inst = Nan::NewInstance(info.Callee(), args.size(), args.data());
  info.GetReturnValue().Set(inst.ToLocalChecked());
}


/**
 * Transcodes a single chunk of data
 */
NAN_METHOD(NAVTranscoder::Transcode) {
  NAVTranscoder *self = Nan::ObjectWrap::Unwrap<NAVTranscoder>(info.Holder());
  NAVSide *in = &self->in;
  NAVSide *out = &self->out;
  NAVStream *inStream = NULL;
  NAVStream *outStream = NULL;

  self->self = info.This();
  v8::Local<v8::Object> opts = Nan::New(self->opts);
  int ret;
  uint8_t *buf;
  bool inputEnded = false;

  AVFrame *inFrame = NULL;
  AVPacket inPkt;
  AVPacket outPkt;


  // Cache the incoming chunk of data for use with the read() method
  self->chunk = node::Buffer::Data(info[0]);
  self->chunkSize = node::Buffer::Length(info[0]);
  LOG("%d bytes received", self->chunkSize);

  // Loop while the chunk is not fully consumed
  while (self->chunkSize) {
    switch (self->state) {
    case OPEN_DEMUXER:
      if (!(buf = (uint8_t *)av_malloc(4096)))
        THROW_AND_RETURN(ret, "av_malloc(demuxer)");

      if (!(in->format.io = avio_alloc_context(buf, 4096, 0, self,
                                               &read, NULL, NULL))) {
        av_freep(&buf);
        THROW_AND_RETURN(ret, "avio_alloc_context(demuxer)");
      }

      if (!(in->format.ctx = avformat_alloc_context()))
        THROW_AND_RETURN(ret, "avformat_alloc_context(demuxer)");

      in->format.ctx->pb = in->format.io;

      if ((ret = avformat_open_input(&in->format.ctx, NULL, NULL, NULL)) < 0)
        THROW_AND_RETURN(ret, "avformat_open_input");

      /* Deliberate fall-thru */
      LOG("Demuxer opened");
      self->state = READ_STREAM_INFO;


    case READ_STREAM_INFO:
      if ((ret = avformat_find_stream_info(in->format.ctx, NULL)) < 0) {
        if (ret == AVERROR(EAGAIN)) {
          LOG("avformat_find_stream_info() returned EAGAIN");
          continue;
        }

        THROW_AND_RETURN(ret, "avformat_find_stream_info");
      }

      /* Deliberate fall-thru */
      av_dump_format(in->format.ctx, 0, NULL, 0);
      self->state = FIND_BEST_AUDIO_STREAM;


    case FIND_BEST_AUDIO_STREAM:
      if (!GET_BOOLEAN_OPT("outputAudioIgnore")) {
        if ((ret = av_find_best_stream(in->format.ctx, AVMEDIA_TYPE_AUDIO,
                                       -1, -1, &(in->audio.codec), 0)) < 0) {
          if (ret != AVERROR_STREAM_NOT_FOUND)
            THROW_AND_RETURN(ret, "av_find_best_stream(audio)");
        } else {
          in->audio.stream = in->format.ctx->streams[ret];
          if (!(in->audio.ctx = avcodec_alloc_context3(in->audio.codec)))
            THROW_AND_RETURN(ENOMEM, "avcodec_alloc_context3(audio)");

          if ((ret = avcodec_open2(in->audio.ctx, in->audio.codec, NULL)))
            THROW_AND_RETURN(ret, "avcodec_open2(audio)");
        }
      }

      /* Deliberate fall-thru */
      self->state = FIND_BEST_VIDEO_STREAM;


    case FIND_BEST_VIDEO_STREAM:
      if (!GET_BOOLEAN_OPT("outputVideoIgnore")) {
        if ((ret = av_find_best_stream(in->format.ctx, AVMEDIA_TYPE_VIDEO,
                                       -1, -1, &(in->video.codec), 0)) < 0) {
          if (ret != AVERROR_STREAM_NOT_FOUND)
            THROW_AND_RETURN(ret, "av_find_best_stream(video)");
        } else {
          in->video.stream = in->format.ctx->streams[ret];
          if (!(in->video.ctx = avcodec_alloc_context3(in->video.codec)))
            THROW_AND_RETURN(ENOMEM, "avcodec_alloc_context3(video)");

          if ((ret = avcodec_open2(in->video.ctx, in->video.codec, NULL)))
            THROW_AND_RETURN(ret, "avcodec_open2(video)");
        }
      }

      /* Deliberate fall-thru */
      self->state = INPUT_CHECK;


    case INPUT_CHECK:
      // Ensure that the input has at least one stream
      if (!in->audio.stream && !in->video.stream) {
        LOG("No audio or video streams found!");
        THROW_AND_RETURN(AVERROR_STREAM_NOT_FOUND, "");
      }

      /* Deliberate fall-thru */
      self->state = OPEN_MUXER;


    case OPEN_MUXER:
      if (!(buf = (uint8_t *)av_malloc(4096)))
        THROW_AND_RETURN(ret, "av_malloc(demuxer)");

      if (!(out->format.io = avio_alloc_context(buf, 4096, 0, self,
                                                NULL, &write, NULL))) {
        av_freep(&buf);
        THROW_AND_RETURN(ret, "avio_alloc_context(demuxer)");
      }

      if (!(out->format.ctx = avformat_alloc_context()))
        THROW_AND_RETURN(ret, "avformat_alloc_context(demuxer)");

      out->format.ctx->pb = out->format.io;

      if (!(out->format.ctx->oformat = av_guess_format(
              GET_STRING_OPT("outputFormatName"),
              GET_STRING_OPT("outputFormatFilename"),
              GET_STRING_OPT("outputFormatMime"))))
        THROW_AND_RETURN(AVERROR_MUXER_NOT_FOUND, "av_guess_format");

      /* Deliberate fall-thru */
      self->state = CREATE_AUDIO_STREAM;


    case CREATE_AUDIO_STREAM:
      if (in->audio.stream) {
        AVCodecID codecId = (AVCodecID)GET_NUMBER_OPT("outputAudioCodec");
        if (!(out->audio.codec = avcodec_find_encoder(codecId)))
          THROW_AND_RETURN(AVERROR_ENCODER_NOT_FOUND, "avcodec_find_encoder");

        if (!(out->audio.stream = avformat_new_stream(out->format.ctx, NULL)))
          THROW_AND_RETURN(AVERROR_UNKNOWN, "avformat_new_stream(audio)");

        if (!(out->audio.ctx = avcodec_alloc_context3(out->audio.codec)))
          THROW_AND_RETURN(ENOMEM, "avcodec_alloc_context3(audio)");

        AVCodecParameters *p = out->audio.stream->codecpar;
        p->codec_type = AVMEDIA_TYPE_AUDIO;
        p->codec_id = out->audio.codec->id;
        p->sample_rate = GET_NUMBER_OPT("outputAudioSampleRate");
        p->channel_layout = GET_NUMBER_OPT("outputAudioChannelLayout");
        p->channels = av_get_channel_layout_nb_channels(p->channel_layout);
        p->format = out->audio.codec->sample_fmts[0];

        out->audio.stream->time_base = { 1, p->sample_rate };

        if ((ret = avcodec_parameters_to_context(out->audio.ctx, p)) < 0)
          THROW_AND_RETURN(ENOMEM, "avcodec_parameters_to_context(audio)");

        if ((ret = avcodec_open2(out->audio.ctx, out->audio.codec, NULL)))
          THROW_AND_RETURN(ret, "avcodec_open2(audio)");
      }

      /* Deliberate fall-thru */
      self->state = CREATE_VIDEO_STREAM;


    case CREATE_VIDEO_STREAM:
      if (in->video.stream) {
        AVCodecID codecId = (AVCodecID)GET_NUMBER_OPT("outputVideoCodec");
        if (!(out->video.codec = avcodec_find_encoder(codecId)))
          THROW_AND_RETURN(AVERROR_ENCODER_NOT_FOUND, "avcodec_find_encoder");

        if (!(out->video.stream = avformat_new_stream(out->format.ctx, NULL)))
          THROW_AND_RETURN(AVERROR_UNKNOWN, "avformat_new_stream");

        AVCodecParameters *p = out->video.stream->codecpar;
        p->codec_type = AVMEDIA_TYPE_VIDEO;
        p->codec_id = out->video.codec->id;

        if ((ret = avcodec_parameters_to_context(out->video.ctx, p)) < 0)
          THROW_AND_RETURN(ENOMEM, "avcodec_parameters_to_context(video)");

        if ((ret = avcodec_open2(out->video.ctx, out->video.codec, NULL)))
          THROW_AND_RETURN(ret, "avcodec_open2(video)");
      }

      /* Deliberate fall-thru */
      self->state = WRITE_HEADER;
      av_dump_format(out->format.ctx, 0, NULL, 1);


    case WRITE_HEADER:
      if ((ret = avformat_write_header(out->format.ctx, NULL)) < 0)
        THROW_AND_RETURN(ret, "avformat_write_header");

      /* Deliberate fall-thru */
      self->state = DEMUX;


    case DEMUX:
      if (!inputEnded) {
        if ((ret = av_read_frame(in->format.ctx, &inPkt)) < 0) {
          if (ret == AVERROR(EAGAIN)) {
            LOG("av_read_frame() returned EAGAIN\n");
            continue;
          } else if (ret == AVERROR_EOF) {
            LOG("av_read_frame() returned AVERROR_EOF. Ending in->..\n");
            inputEnded = true;
          } else {
            THROW_AND_RETURN(ret, "av_read_frame");
          }
        } else {
          // Ensure that the packet is for a stream of interest
          if (!(inStream = self->getInputStream(&inPkt))) {
            LOG("av_read_frame() returned uninteresting packet. Ignoring...\n");
            av_packet_unref(&inPkt);
            continue;
          }

          if (!(outStream = self->getOutputStream(&inPkt)))
            THROW_AND_RETURN(ret, "getOutputStream");
        }
      }

      /* Deliberate fall-thru */
      self->state = DECODE_INPUT;


    case DECODE_INPUT:
      if ((ret = avcodec_send_packet(inStream->ctx, &inPkt))) {
        if (ret == AVERROR(EAGAIN)) {
          LOG("avcodec_send_packet(): EAGAIN. Going to DECODE_OUTPUT\n");
        } else if (ret != AVERROR_EOF) {
          av_packet_unref(&inPkt);
          THROW_AND_RETURN(ret, "avcodec_send_packet");
        }
      }

      /* Deliberate fall-thru */
      self->state = DECODE_OUTPUT;


    case DECODE_OUTPUT:
      if (!(inFrame = av_frame_alloc())) {
        av_packet_unref(&inPkt);
        THROW_AND_RETURN(ENOMEM, "av_frame_alloc");
      }

      if ((ret = avcodec_receive_frame(inStream->ctx, inFrame))) {
        if (ret == AVERROR(EAGAIN)) {
          LOG("avcodec_receive_frame(): EAGAIN. Going to ENCODE_INPUT\n");
        } else if (ret != AVERROR_EOF) {
          av_frame_free(&inFrame);
          av_packet_unref(&inPkt);
          THROW_AND_RETURN(ret, "avcodec_receive_frame");
        }
      }

      /* Deliberate fall-thru */
      self->state = ENCODE_INPUT;


    case ENCODE_INPUT:
      if ((ret = avcodec_send_frame(outStream->ctx, inFrame)) < 0) {
        if (ret == AVERROR(EAGAIN)) {
          LOG("avcodec_receive_frame(): EAGAIN. Going to ENCODE_OUTPUT\n");
        } else if (ret != AVERROR_EOF) {
          av_frame_free(&inFrame);
          av_packet_unref(&inPkt);
          THROW_AND_RETURN(ret, "avcodec_receive_frame");
        }
      }

      av_frame_free(&inFrame);

      /* Deliberate fall-thru */
      self->state = ENCODE_OUTPUT;


    case ENCODE_OUTPUT:
      av_init_packet(&outPkt);
      outPkt.data = NULL;
      outPkt.size = 0;

      if ((ret = avcodec_receive_packet(outStream->ctx, &outPkt))) {
        if (ret == AVERROR(EAGAIN)) {
          self->state = DECODE_OUTPUT;
          continue;
        } else if (ret != AVERROR_EOF) {
          av_packet_unref(&inPkt);
          THROW_AND_RETURN(ret, "avcodec_send_packet");
        }
      }

      /* Deliberate fall-thru */
      self->state = MUX;


    case MUX:
      if ((ret = av_interleaved_write_frame(out->format.ctx, &outPkt)) < 0) {
        if (ret == AVERROR(EAGAIN)) {
          self->state = DECODE_OUTPUT;
          continue;
        } else if (ret != AVERROR_EOF) {
          av_packet_unref(&inPkt);
          THROW_AND_RETURN(ret, "avcodec_send_packet");
        }
      }

      /* Deliberate fall-thru */
      self->state = WRITE_TRAILER;


    case WRITE_TRAILER:
      ret = av_write_trailer(out->format.ctx);
      if (ret < 0) {
        THROW_AND_RETURN(ret, "av_write_trailer");
        return;
      }
      LOG("Trailer written to output!");

      return;

    default:
      THROW_AND_RETURN(AVERROR_UNKNOWN, "");
      return;
    }
  }
}


/**
 * Helper read function for custom IO context
 */
int NAVTranscoder::read(void *opaque, uint8_t *buf, int size) {
  auto self = (NAVTranscoder *)opaque;

  size = FFMIN(size, self->chunkSize);
  LOG("Reading %d bytes of data", size);

  memcpy(buf, self->chunk, size);
  self->chunkSize -= size;
  self->chunk += size;

  return size;
}


/**
 * Helper write function for custom IO context
 */
int NAVTranscoder::write(void *opaque, uint8_t *buf, int size) {
  auto self = (NAVTranscoder *)opaque;

  Nan::EscapableHandleScope scope;
  v8::Handle<v8::Value> argv[2] = {
    Nan::New("data").ToLocalChecked(),
    Nan::NewBuffer((char *)buf, (uint32_t)size).ToLocalChecked()
  };
  Nan::MakeCallback(self->self, "emit", 2, argv);
  LOG("Write %d bytes of data", size);
  return 0;
}


/**
 * Exports the native transcoder
 */
NAN_MODULE_INIT(NAVTranscoder::Init) {
  auto ctor = Nan::New<v8::FunctionTemplate>(NAVTranscoder::New);
  auto ctorName = Nan::New<v8::String>("Transcoder").ToLocalChecked();
  auto ctorInstance = ctor->InstanceTemplate();

  ctor->SetClassName(ctorName);
  ctorInstance->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(ctor, "transcode", NAVTranscoder::Transcode);

  Nan::Set(target, ctorName, Nan::GetFunction(ctor).ToLocalChecked());
}
