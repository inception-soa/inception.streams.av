/**
 * @file A libav transcoding worker implementation
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

#include <libavutil/time.h>
#include "av_transcode_worker.h"


#define BUF_SIZE    4096


Chunk::Chunk() {}


~Chunk::Chunk() {}


int Chunk::Write(const char *buf, size_t bufSize) {
  while (true) {
    std::unique_lock<std::mutex> locker(mu);
    memcpy(data, buf, bufSize);
    size = bufSize;
    locker.unlock();
    cond.notify_all();
  }
}


Data Chunk::Read() {}


static int readPacket(void *opaque, uint8_t *buf, int bufSize) {}


static int writePacket(void *opaque, uint8_t *buf, int bufSize) {}


/**
 * Initializes the transcoding worker
 */
AVTranscodeWorker::AVTranscodeWorker(AVOpts        *opts,
                                     Nan::Callback *emit,
                                     Nan::Callback *cb) :
  Nan::AsyncProgressWorkerBase<AVEvent>(cb), emit_(emit) {
  opts_ = *opts;

  memset(&input_, 0, sizeof(AVInput));
  memset(&output_, 0, sizeof(AVOutput));
}


/**
 * Destroys the transcoding worker
 */
AVTranscodeWorker::~AVTranscodeWorker() {
  avformat_close_input(&input_.demuxer.ctxFormat);
  if (input_.demuxer.ctxIO) {
    av_freep(&(input_.demuxer.ctxIO->buffer));
    av_freep(&input_.demuxer.ctxIO);
  }
}


/**
 * Initializes the demuxer
 */
int AVTranscodeWorker::InitDemuxer(AVEvent *event) {
  int ret;
  uint8_t *buffer;
  AVDemuxer *demuxer = &input_.demuxer;

  // Allocate space for the IO buffer
  buffer = (uint8_t *)av_malloc(BUF_SIZE);
  if (!buffer) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ENOMEM);
  }

  // Create an IO context
  demuxer->ctxIO = avio_alloc_context(buffer, BUF_SIZE, 0, this, &readPacket,
                                      NULL, NULL);
  if (!demuxer->ctxIO) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ENOMEM);
  }

  // Create the Format context
  demuxer->ctxFormat = avformat_alloc_context();
  if (!demuxer->ctxFormat) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ENOMEM);
  }

  // Link the IO context to the Format context
  demuxer->ctxFormat->pb = demuxer->ctxIO;

  // Open the input
  ret = avformat_open_input(&demuxer->ctxFormat, NULL, NULL, NULL);
  if (ret < 0) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ret);
  }

  // Read the stream metadata
  ret = avformat_find_stream_info(demuxer->ctxFormat, NULL);
  if (ret < 0) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ret);
  }

  return 0;
}


// /**
//  * Initializes the decoders
//  */
// int AVTranscodeWorker::InitDecoders(AVEvent *event) {
//   int ret;
//
//   // Find the best video stream in the input
//   ret = InitDecoder(AVMEDIA_TYPE_VIDEO, event);
//   if (ret < 0)
//     return AVERROR(ret);
//
//   // Find the best audio stream in the input
//   ret = InitDecoder(AVMEDIA_TYPE_AUDIO, event);
//   if (ret < 0)
//     return AVERROR(ret);
//
//   return 0;
// }
//
//
// /**
//  * Initializes the specified type of decoder
//  */
// int AVTranscodeWorker::InitDecoder(AVMediaType type, AVEvent *event) {
//   int ret;
//   AVFormatContext *fmt = input_.demuxer.ctxFormat;
//   AVDecoder *decoder = (type == AVMEDIA_TYPE_VIDEO) ?
//                        &input_.video :
//                        &input_.audio;
//
//   ret = av_find_best_stream(fmt, type, -1, -1, &decoder->codec, 0);
//   if ((ret < 0) && (ret != AVERROR(AVERROR_STREAM_NOT_FOUND))) {
//     *event = { .eventName = "error", .eventData = "OutOfMemory" };
//     return AVERROR(ret);
//   } else if (ret >= 0) {
//     decoder->stream = fmt->streams[ret];
//     decoder->ctxCodec = avcodec_alloc_context3(decoder->codec);
//     if (!decoder->ctxCodec) {
//       *event = { .eventName = "error", .eventData = "OutOfMemory" };
//       return AVERROR(ENOMEM);
//     }
//   }
//
//   return 0;
// }


/**
 * Initializes the muxer
 */
int AVTranscodeWorker::InitMuxer(AVEvent *event) {
  uint8_t *buffer;
  AVMuxer *muxer = &output_.muxer;

  // Allocate space for the IO buffer
  buffer = (uint8_t *)av_malloc(BUF_SIZE);
  if (!buffer) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ENOMEM);
  }

  // Create an IO context
  muxer->ctxIO = avio_alloc_context(buffer, BUF_SIZE, 0, 0, 0, 0, 0);
  if (!muxer->ctxIO) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ENOMEM);
  }

  // Create the Format context
  muxer->ctxFormat = avformat_alloc_context();
  if (!muxer->ctxFormat) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(ENOMEM);
  }

  // Link the IO context to the Format context
  muxer->ctxFormat->pb = muxer->ctxIO;

  // Setup the output format
  muxer->ctxFormat->oformat = av_guess_format(opts_.outputFormatName,
                                              NULL,
                                              opts_.outputFormatMime);
  if (!muxer->ctxFormat->oformat) {
    *event = { .eventName = "error", .eventData = "OutOfMemory" };
    return AVERROR(AVERROR_UNKNOWN);
  }

  return 0;
}


// int AVTranscodeWorker::InitEncoders(AVEvent *event) {
//   int ret;
//
//   if (opts_.outputVideo.codec != AV_CODEC_ID_NONE) {
//     ret = InitEncoder(AVMEDIA_TYPE_VIDEO, event);
//     if (ret < 0)
//       return AVERROR(ret);
//   }
//
//   if (opts_.outputAudio.codec != AV_CODEC_ID_NONE) {
//     ret = InitEncoder(AVMEDIA_TYPE_AUDIO, event);
//     if (ret < 0)
//       return AVERROR(ret);
//   }
//
//   return 0;
// }
//
//
// int AVTranscodeWorker::InitEncoder(AVMediaType type, AVEvent *event) {
//   int ret;
//   AVFormatContext *fmt = output_.muxer.ctxFormat;
//   AVEncoder *encoder = (type == AVMEDIA_TYPE_VIDEO) ?
//                        &output_.video :
//                        &output_.audio;
//   AVCodecID codecId = (type == AVMEDIA_TYPE_VIDEO) ?
//                        opts_.outputVideo.codec :
//                        opts_.outputAudio.codec;
//
//   // Create a new stream
//   encoder->stream = avformat_new_stream(fmt, NULL);
//   if (!encoder->stream) {
//     *event = { .eventName = "error", .eventData = "OutOfMemory" };
//     return AVERROR(AVERROR_UNKNOWN);
//   }
//
//   return 0;
// }


int AVTranscodeWorker::Transcode(AVEvent *event) {
  int ret;
  AVPacket pkt;

  // Write the headers
  ret = avformat_write_header(output_.muxer.ctxFormat, NULL);
  if (ret < 0) {
    *event = { .eventName = "error", .eventData = "Error" };
    return AVERROR(ret);
  }

  // Transcode the input
  while (true) {
    // Initialize the packet
    pkt = { .data = NULL, .size = 0 };
    av_init_packet(&pkt);

    // Read a packet from the input
    ret = av_read_frame(input_.demuxer.ctxFormat, &pkt);
    if (ret < 0) {
      switch (ret) {
      case AVERROR(EAGAIN):
        av_usleep(10000);
        continue;

      case AVERROR_EOF:
        input_.ended = true;
        break;

      default:
        *event = { .eventName = "error", .eventData = "Error" };
        break;
      }

      av_packet_unref(&pkt);
      return AVERROR(input_.ended ? 0 : ret);
    }

    // Write the packet to the output
    ret = av_write_frame(output_.muxer.ctxFormat, &pkt);
    if (ret < 0) {
      *event = { .eventName = "error", .eventData = "Error" };
      return AVERROR(ret);
    }

    // Unreference the packet
    av_packet_unref(&pkt);
  }

  // Unreference the packet
  av_packet_unref(&pkt);

  // Write the trailers
  ret = av_write_trailer(output_.muxer.ctxFormat);
  if (ret < 0) {
    *event = { .eventName = "error", .eventData = "Error" };
    return AVERROR(ret);
  }

  return 0;
}


void AVTranscodeWorker::Execute(const ExecutionProgress& progress) {
  int ret;
  AVEvent event;


  // Initialize the demuxer
  if ((ret = InitDemuxer(&event)) < 0) {
    progress.Send(&event, sizeof(event));
    return;
  }

  // Open the output stream
  if ((ret = InitMuxer(&event)) < 0) {
    progress.Send(&event, sizeof(event));
    return;
  }

  // Start transcoding
  if ((ret = Transcode(&event)) < 0) {
    progress.Send(&event, sizeof(event));
    return;
  }
}


void AVTranscodeWorker::HandleProgressCallback(const AVEvent *event,
                                               size_t         size) {}


bool AVTranscodeWorker::EnqueueChunk(const char *chunk, size_t size) {}


// static int initFormatState(AVFormatState *state, AVEvent *event) {
//   // Allocate space for the IO buffer
//   state->ioBuffer = (uint8_t *)av_malloc(BUF_SIZE);
//   if (state->ioBuffer == NULL) {
//     *event = { "error", "av_malloc(input): Out of memory!" };
//     return AVERROR(ENOMEM);
//   }
//
//   // Allocate an IO context
//   state->ioContext = avio_alloc_context(state->ioBuffer, BUF_SIZE, 0, NULL,
//                                         NULL, NULL, NULL);
//   if (state->ioContext == NULL) {
//     *event = { "error", "avio_alloc_context(input): Out of memory!" };
//     return AVERROR(ENOMEM);
//   }
//
//   // Allocate the format context
//   state->formatContext = avformat_alloc_context();
//   if (state->formatContext == NULL) {
//     *event = { "error", "avformat_alloc_context(input): Out of memory!" };
//     return AVERROR(ENOMEM);
//   }
//
//   // Link the IO context to the format context
//   state->formatContext->pb = state->ioContext;
//
//   return 0;
// }
//
//
// static int initStreamState(AVStreamState *state,
//                            AVCodecID codecId, AVCodecParameters *codecParams,
//                            AVEvent *event, bool encoder) {
//   int ret;
//   AVCodec *codec;
//   AVCodecContext *codecContext;
//
//
//   // If there is no stream, there is nothing to do here
//   if (state->stream == NULL)
//     return 0;
//
//   // Find the stream codec
//   codec = state->codec = encoder ?
//                          avcodec_find_encoder(codecId) :
//                          avcodec_find_decoder(codecId);
//   if (codec == NULL) {
//     const char *err = encoder ?
//                       "avcodec_find_encoder(): Cannot find encoder!" :
//                       "avcodec_find_decoder(): Cannot find decoder!";
//     *event = { "error", err };
//     return AVERROR(AVERROR_UNKNOWN);
//   }
//
//   // Allocate a codec context for the stream
//   codecContext = state->codecContext = avcodec_alloc_context3(codec);
//   if (codecContext == NULL) {
//     *event = { "error", "avcodec_alloc_context3(): Out of memory!" };
//     return AVERROR(ENOMEM);
//   }
//
//   // Override the codec defaults with values from the stream
//   ret = avcodec_parameters_to_context(codecContext, codecParams);
//   if (ret < 0) {
//     *event = { "error", "avcodec_parameters_to_context(): Error!" };
//     return AVERROR(ret);
//   }
//
//   // Initialize the codec context to use the codec
//   ret = avcodec_open2(codecContext, codec, NULL);
//   if (ret < 0) {
//     *event = { "error", "avcodec_open2(): Error opening codec!" };
//     return AVERROR(ret);
//   }
//
//   return 0;
// }
//
//
// /**
//  * Opens the input stream
//  */
// int AVTranscodeWorker::OpenInput(AVEvent *event) {
//   int ret;
//   AVFormatContext *format = NULL;
//   AVStream *video, *audio;
//   AVCodecID videoCodecId, audioCodecId;
//
//
//   // Initialize the format state
//   ret = initFormatState(&input_.format, event);
//   if (ret < 0)
//     return AVERROR(ret);
//
//   format = input_.format.formatContext;
//
//   // Open the input
//   ret = avformat_open_input(&format, NULL, NULL, NULL);
//   if (ret < 0) {
//     *event = { "error", "avformat_open_input(input): Error opening input!" };
//     return AVERROR(AVERROR_UNKNOWN);
//   }
//
//   // Read the stream metadata
//   ret = avformat_find_stream_info(format, NULL);
//   if (ret < 0) {
//     *event = { "error", "avformat_find_stream_info(): Error reading format!" };
//     return AVERROR(AVERROR_UNKNOWN);
//   }
//
//   // Discover input codecs/metadata
//   int maxWidth = -1;
//   int maxChannels = -1;
//   for (int i = 0; i < format->nb_streams; i++) {
//     AVStream *iStream = format->streams[i];
//     AVMediaType codecType = iStream->codecpar->codec_type;
//
//     // Identify the best audio/video streams and ignore the rest
//     switch (codecType) {
//     case AVMEDIA_TYPE_VIDEO:
//       if (opts_.outputVideo.codec != AV_CODEC_ID_NONE) {
//         if (iStream->codecpar->width > maxWidth) {
//           maxWidth = iStream->codecpar->width;
//           input_.video.stream = iStream;
//           videoCodecId = iStream->codecpar->codec_id;
//         }
//       }
//       break;
//
//     case AVMEDIA_TYPE_AUDIO:
//       if (opts_.outputAudio.codec != AV_CODEC_ID_NONE) {
//         if (iStream->codecpar->channels > maxChannels) {
//           maxChannels = iStream->codecpar->channels;
//           input_.audio.stream = iStream;
//           audioCodecId = iStream->codecpar->codec_id;
//         }
//       }
//       break;
//
//     default:
//       continue;
//     }
//   }
//
//   // Initialize the audio and video streams
//   if ((ret =
//          initStreamState(&input_.video, videoCodecId, input_video event,
//                          false)) < 0)
//     return AVERROR(ret);
//
//   if ((ret = initStreamState(&input_.audio, event, false)) < 0)
//     return AVERROR(ret);
//
//   return 0;
// }
//
//
// int AVTranscodeWorker::OpenOutput(AVEvent *event) {
//   int ret;
//   AVFormatContext *format = NULL;
//
//
//   // Initialize the format state
//   ret = initFormatState(&input_.format, event);
//   if (ret < 0)
//     return AVERROR(ret);
//
//   format = input_.format.formatContext;
//
//   // Setup the output format
//   format->oformat = av_guess_format(opts_.outputFormat, NULL, NULL);
//   if (format->oformat == NULL) {
//     *event = { "error", "Unknown output format!" };
//     return AVERROR(AVERROR_UNKNOWN);
//   }
//
//   // Setup the output video stream
//   if (opts_.outputVideo.codec != AV_CODEC_ID_NONE) {}
//
//   // Setup the video codec and stream
//   if (opts_.outputVideo.codec != AV_CODEC_ID_NONE) {
//     AVCodec *codec;
//
//     if (opts_.outputVideo.codec != AV_CODEC_ID_PROBE) {
//       codec = avcodec_find_encoder(opts_.outputVideo.codec);
//       if (codec == NULL) {
//         *event = { "error", "avcodec_find_encoder(out): Unknown video codec!" };
//         return AVERROR(AVERROR_UNKNOWN);
//       }
//
//       output_.vCodecContext = avcodec_alloc_context3(codec);
//       if (output_.vCodecContext == NULL) {
//         *event = { "error", "avcodec_alloc_context3(out): Out of memory!" };
//         return AVERROR(ENOMEM);
//       }
//     } else {
//       codec = avcodec_find_encoder(opts_.outputVideo.codec);
//       if (codec == NULL) {
//         *event = { "error", "avcodec_find_encoder(out): Unknown video codec!" };
//         return AVERROR(AVERROR_UNKNOWN);
//       }
//
//       output_.aCodecContext = avcodec_alloc_context3(codec);
//       if (output_.aCodecContext == NULL) {
//         *event = { "error", "avcodec_alloc_context3(out): Out of memory!" };
//         return AVERROR(ENOMEM);
//       }
//     }
//
//     // Setup the global header flag, if required
//     if (output_.ctxFormat->oformat->flags & AVFMT_GLOBALHEADER) {
//       output_.vCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//     }
//
//     output_.vStream = avformat_new_stream(output_.ctxFormat, codec);
//     if (output_.vStream == NULL) {
//       *event = { "error", "avformat_new_stream(out): Out of memory!" };
//       return AVERROR(ENOMEM);
//     }
//   }
//
//   // Setup the audio codec and stream
//   if (opts_.outputAudio.codec != AV_CODEC_ID_NONE) {
//     AVCodec *codec;
//
//     if (opts_.outputAudio.codec != AV_CODEC_ID_PROBE) {
//       codec = avcodec_find_encoder(opts_.outputAudio.codec);
//       if (codec == NULL) {
//         *event = { "error", "avcodec_find_encoder(out): Unknown audio codec!" };
//         return AVERROR(AVERROR_UNKNOWN);
//       }
//
//       output_.aCodecContext = avcodec_alloc_context3(codec);
//       if (output_.aCodecContext == NULL) {
//         *event = { "error", "avcodec_alloc_context3(out): Out of memory!" };
//         return AVERROR(ENOMEM);
//       }
//     } else {
//       codec = avcodec_find_encoder(opts_.outputAudio.codec);
//       if (codec == NULL) {
//         *event = { "error", "avcodec_find_encoder(out): Unknown audio codec!" };
//         return AVERROR(AVERROR_UNKNOWN);
//       }
//
//       output_.aCodecContext = avcodec_alloc_context3(codec);
//       if (output_.aCodecContext == NULL) {
//         *event = { "error", "avcodec_alloc_context3(out): Out of memory!" };
//         return AVERROR(ENOMEM);
//       }
//     }
//
//     // Setup the global header flag, if required
//     if (output_.ctxFormat->oformat->flags & AVFMT_GLOBALHEADER) {
//       output_.aCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//     }
//
//     output_.aStream = avformat_new_stream(output_.ctxFormat, codec);
//     if (output_.aStream == NULL) {
//       *event = { "error", "avformat_new_stream(out): Out of memory!" };
//       return AVERROR(ENOMEM);
//     }
//   }
//
//   ret = avformat_write_header(output_.ctxFormat, NULL);
//   if (ret < 0) {
//     *event = { "error", "avformat_write_header(): Error writing header!" };
//     return AVERROR(ret);
//   }
//
//   return 0;
// }
