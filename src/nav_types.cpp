/**
 * @file Exports libAV data types to JavaScript
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
 * distributed under the License is distributed on an "AS IS" BASIS));
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nav_types.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
}


using namespace v8;


#define SET(o, k, v) \
  Nan::Set(o, Nan::New<String>(k).ToLocalChecked(), v);


NAN_MODULE_INIT(InitMediaTypes) {
  auto o = Nan::New<Object>();

  SET(o, "UNKNOWN", Nan::New<Number>(AVMEDIA_TYPE_UNKNOWN));
  SET(o, "VIDEO", Nan::New<Number>(AVMEDIA_TYPE_VIDEO));
  SET(o, "AUDIO", Nan::New<Number>(AVMEDIA_TYPE_AUDIO));
  SET(o, "DATA", Nan::New<Number>(AVMEDIA_TYPE_DATA));
  SET(o, "SUBTITLE", Nan::New<Number>(AVMEDIA_TYPE_SUBTITLE));
  SET(o, "ATTACHMENT", Nan::New<Number>(AVMEDIA_TYPE_ATTACHMENT));

  SET(target, "MediaTypes", o);
}


NAN_MODULE_INIT(InitSampleFormats) {
  auto o = Nan::New<Object>();

  SET(o, "NONE", Nan::New<Number>(AV_SAMPLE_FMT_NONE));
  SET(o, "UNSIGNED_8_BIT", Nan::New<Number>(AV_SAMPLE_FMT_U8));
  SET(o, "SIGNED_16_BIT", Nan::New<Number>(AV_SAMPLE_FMT_S16));
  SET(o, "SIGNED_32_BIT", Nan::New<Number>(AV_SAMPLE_FMT_S32));
  SET(o, "FLOAT", Nan::New<Number>(AV_SAMPLE_FMT_FLT));
  SET(o, "DOUBLE", Nan::New<Number>(AV_SAMPLE_FMT_DBL));
  SET(o, "PLANAR_UNSIGNED_8_BIT", Nan::New<Number>(AV_SAMPLE_FMT_U8P));
  SET(o, "PLANAR_SIGNED_16_BIT", Nan::New<Number>(AV_SAMPLE_FMT_S16P));
  SET(o, "PLANAR_SIGNED_32_BIT", Nan::New<Number>(AV_SAMPLE_FMT_S32P));
  SET(o, "PLANAR_FLOAT", Nan::New<Number>(AV_SAMPLE_FMT_FLTP));
  SET(o, "PLANAR_DOUBLE", Nan::New<Number>(AV_SAMPLE_FMT_DBLP));

  SET(target, "SampleFormats", o);
}


NAN_MODULE_INIT(InitChannelLayouts) {
  auto o = Nan::New<Object>();

  SET(o, "MONO", Nan::New<Number>(AV_CH_LAYOUT_MONO));
  SET(o, "STEREO", Nan::New<Number>(AV_CH_LAYOUT_STEREO));
  SET(o, "2POINT1", Nan::New<Number>(AV_CH_LAYOUT_2POINT1));
  SET(o, "2_1", Nan::New<Number>(AV_CH_LAYOUT_2_1));
  SET(o, "SURROUND", Nan::New<Number>(AV_CH_LAYOUT_SURROUND));
  SET(o, "3POINT1", Nan::New<Number>(AV_CH_LAYOUT_3POINT1));
  SET(o, "4POINT0", Nan::New<Number>(AV_CH_LAYOUT_4POINT0));
  SET(o, "4POINT1", Nan::New<Number>(AV_CH_LAYOUT_4POINT1));
  SET(o, "2_2", Nan::New<Number>(AV_CH_LAYOUT_2_2));
  SET(o, "QUAD", Nan::New<Number>(AV_CH_LAYOUT_QUAD));
  SET(o, "5POINT0", Nan::New<Number>(AV_CH_LAYOUT_5POINT0));
  SET(o, "5POINT1", Nan::New<Number>(AV_CH_LAYOUT_5POINT1));
  SET(o, "5POINT0_BACK", Nan::New<Number>(AV_CH_LAYOUT_5POINT0_BACK));
  SET(o, "5POINT1_BACK", Nan::New<Number>(AV_CH_LAYOUT_5POINT1_BACK));
  SET(o, "6POINT0", Nan::New<Number>(AV_CH_LAYOUT_6POINT0));
  SET(o, "6POINT0_FRONT", Nan::New<Number>(AV_CH_LAYOUT_6POINT0_FRONT));
  SET(o, "HEXAGONAL", Nan::New<Number>(AV_CH_LAYOUT_HEXAGONAL));
  SET(o, "6POINT1", Nan::New<Number>(AV_CH_LAYOUT_6POINT1));
  SET(o, "6POINT1_BACK", Nan::New<Number>(AV_CH_LAYOUT_6POINT1_BACK));
  SET(o, "6POINT1_FRONT", Nan::New<Number>(AV_CH_LAYOUT_6POINT1_FRONT));
  SET(o, "7POINT0", Nan::New<Number>(AV_CH_LAYOUT_7POINT0));
  SET(o, "7POINT0_FRONT", Nan::New<Number>(AV_CH_LAYOUT_7POINT0_FRONT));
  SET(o, "7POINT1", Nan::New<Number>(AV_CH_LAYOUT_7POINT1));
  SET(o, "7POINT1_WIDE", Nan::New<Number>(AV_CH_LAYOUT_7POINT1_WIDE));
  SET(o, "7POINT1_WIDE_BACK", Nan::New<Number>(AV_CH_LAYOUT_7POINT1_WIDE_BACK));
  SET(o, "OCTAGONAL", Nan::New<Number>(AV_CH_LAYOUT_OCTAGONAL));
  SET(o, "HEXADECAGONAL", Nan::New<Number>(AV_CH_LAYOUT_HEXADECAGONAL));
  SET(o, "STEREO_DOWNMIX", Nan::New<Number>(AV_CH_LAYOUT_STEREO_DOWNMIX));

  SET(target, "ChannelLayouts", o);
}


NAN_MODULE_INIT(InitPixelFormats) {
  auto o = Nan::New<Object>();

  SET(o, "NONE", Nan::New<Number>(AV_PIX_FMT_NONE));
  SET(o, "YUV420P", Nan::New<Number>(AV_PIX_FMT_YUV420P));
  SET(o, "YUYV422", Nan::New<Number>(AV_PIX_FMT_YUYV422));
  SET(o, "RGB24", Nan::New<Number>(AV_PIX_FMT_RGB24));
  SET(o, "BGR24", Nan::New<Number>(AV_PIX_FMT_BGR24));
  SET(o, "YUV422P", Nan::New<Number>(AV_PIX_FMT_YUV422P));
  SET(o, "YUV444P", Nan::New<Number>(AV_PIX_FMT_YUV444P));
  SET(o, "YUV410P", Nan::New<Number>(AV_PIX_FMT_YUV410P));
  SET(o, "YUV411P", Nan::New<Number>(AV_PIX_FMT_YUV411P));
  SET(o, "GRAY8", Nan::New<Number>(AV_PIX_FMT_GRAY8));
  SET(o, "MONOWHITE", Nan::New<Number>(AV_PIX_FMT_MONOWHITE));
  SET(o, "MONOBLACK", Nan::New<Number>(AV_PIX_FMT_MONOBLACK));
  SET(o, "PAL8", Nan::New<Number>(AV_PIX_FMT_PAL8));
  SET(o, "YUVJ420P", Nan::New<Number>(AV_PIX_FMT_YUVJ420P));
  SET(o, "YUVJ422P", Nan::New<Number>(AV_PIX_FMT_YUVJ422P));
  SET(o, "YUVJ444P", Nan::New<Number>(AV_PIX_FMT_YUVJ444P));
#if FF_API_XVMC
    SET(o, "XVMC_MPEG2_MC", Nan::New<Number>(AV_PIX_FMT_XVMC_MPEG2_MC));
    SET(o, "XVMC_MPEG2_IDCT", Nan::New<Number>(AV_PIX_FMT_XVMC_MPEG2_IDCT));
#endif /* FF_API_XVMC */
  SET(o, "UYVY422", Nan::New<Number>(AV_PIX_FMT_UYVY422));
  SET(o, "UYYVYY411", Nan::New<Number>(AV_PIX_FMT_UYYVYY411));
  SET(o, "BGR8", Nan::New<Number>(AV_PIX_FMT_BGR8));
  SET(o, "BGR4", Nan::New<Number>(AV_PIX_FMT_BGR4));
  SET(o, "BGR4_BYTE", Nan::New<Number>(AV_PIX_FMT_BGR4_BYTE));
  SET(o, "RGB8", Nan::New<Number>(AV_PIX_FMT_RGB8));
  SET(o, "RGB4", Nan::New<Number>(AV_PIX_FMT_RGB4));
  SET(o, "RGB4_BYTE", Nan::New<Number>(AV_PIX_FMT_RGB4_BYTE));
  SET(o, "NV12", Nan::New<Number>(AV_PIX_FMT_NV12));
  SET(o, "NV21", Nan::New<Number>(AV_PIX_FMT_NV21));
  SET(o, "ARGB", Nan::New<Number>(AV_PIX_FMT_ARGB));
  SET(o, "RGBA", Nan::New<Number>(AV_PIX_FMT_RGBA));
  SET(o, "ABGR", Nan::New<Number>(AV_PIX_FMT_ABGR));
  SET(o, "BGRA", Nan::New<Number>(AV_PIX_FMT_BGRA));
  SET(o, "GRAY16BE", Nan::New<Number>(AV_PIX_FMT_GRAY16BE));
  SET(o, "GRAY16LE", Nan::New<Number>(AV_PIX_FMT_GRAY16LE));
  SET(o, "YUV440P", Nan::New<Number>(AV_PIX_FMT_YUV440P));
  SET(o, "YUVJ440P", Nan::New<Number>(AV_PIX_FMT_YUVJ440P));
  SET(o, "YUVA420P", Nan::New<Number>(AV_PIX_FMT_YUVA420P));
#if FF_API_VDPAU
    SET(o, "VDPAU_H264", Nan::New<Number>(AV_PIX_FMT_VDPAU_H264));
    SET(o, "VDPAU_MPEG1", Nan::New<Number>(AV_PIX_FMT_VDPAU_MPEG1));
    SET(o, "VDPAU_MPEG2", Nan::New<Number>(AV_PIX_FMT_VDPAU_MPEG2));
    SET(o, "VDPAU_WMV3", Nan::New<Number>(AV_PIX_FMT_VDPAU_WMV3));
    SET(o, "VDPAU_VC1", Nan::New<Number>(AV_PIX_FMT_VDPAU_VC1));
#endif
  SET(o, "RGB48BE", Nan::New<Number>(AV_PIX_FMT_RGB48BE));
  SET(o, "RGB48LE", Nan::New<Number>(AV_PIX_FMT_RGB48LE));
  SET(o, "RGB565BE", Nan::New<Number>(AV_PIX_FMT_RGB565BE));
  SET(o, "RGB565LE", Nan::New<Number>(AV_PIX_FMT_RGB565LE));
  SET(o, "RGB555BE", Nan::New<Number>(AV_PIX_FMT_RGB555BE));
  SET(o, "RGB555LE", Nan::New<Number>(AV_PIX_FMT_RGB555LE));
  SET(o, "BGR565BE", Nan::New<Number>(AV_PIX_FMT_BGR565BE));
  SET(o, "BGR565LE", Nan::New<Number>(AV_PIX_FMT_BGR565LE));
  SET(o, "BGR555BE", Nan::New<Number>(AV_PIX_FMT_BGR555BE));
  SET(o, "BGR555LE", Nan::New<Number>(AV_PIX_FMT_BGR555LE));
#if FF_API_VAAPI
    SET(o, "VAAPI_MOCO", Nan::New<Number>(AV_PIX_FMT_VAAPI_MOCO));
    SET(o, "VAAPI_IDCT", Nan::New<Number>(AV_PIX_FMT_VAAPI_IDCT));
    SET(o, "VAAPI_VLD", Nan::New<Number>(AV_PIX_FMT_VAAPI_VLD));
    SET(o, "VAAPI", Nan::New<Number>(AV_PIX_FMT_VAAPI));
#else
    SET(o, "VAAPI", Nan::New<Number>(AV_PIX_FMT_VAAPI));
#endif
  SET(o, "YUV420P16LE", Nan::New<Number>(AV_PIX_FMT_YUV420P16LE));
  SET(o, "YUV420P16BE", Nan::New<Number>(AV_PIX_FMT_YUV420P16BE));
  SET(o, "YUV422P16LE", Nan::New<Number>(AV_PIX_FMT_YUV422P16LE));
  SET(o, "YUV422P16BE", Nan::New<Number>(AV_PIX_FMT_YUV422P16BE));
  SET(o, "YUV444P16LE", Nan::New<Number>(AV_PIX_FMT_YUV444P16LE));
  SET(o, "YUV444P16BE", Nan::New<Number>(AV_PIX_FMT_YUV444P16BE));
#if FF_API_VDPAU
    SET(o, "VDPAU_MPEG4", Nan::New<Number>(AV_PIX_FMT_VDPAU_MPEG4));
#endif
  SET(o, "DXVA2_VLD", Nan::New<Number>(AV_PIX_FMT_DXVA2_VLD));
  SET(o, "RGB444LE", Nan::New<Number>(AV_PIX_FMT_RGB444LE));
  SET(o, "RGB444BE", Nan::New<Number>(AV_PIX_FMT_RGB444BE));
  SET(o, "BGR444LE", Nan::New<Number>(AV_PIX_FMT_BGR444LE));
  SET(o, "BGR444BE", Nan::New<Number>(AV_PIX_FMT_BGR444BE));
  SET(o, "YA8", Nan::New<Number>(AV_PIX_FMT_YA8));
  SET(o, "Y400", Nan::New<Number>(AV_PIX_FMT_Y400A));
  SET(o, "BGR48BE", Nan::New<Number>(AV_PIX_FMT_BGR48BE));
  SET(o, "BGR48LE", Nan::New<Number>(AV_PIX_FMT_BGR48LE));
  SET(o, "YUV420P9BE", Nan::New<Number>(AV_PIX_FMT_YUV420P9BE));
  SET(o, "YUV420P9LE", Nan::New<Number>(AV_PIX_FMT_YUV420P9LE));
  SET(o, "YUV420P10BE", Nan::New<Number>(AV_PIX_FMT_YUV420P10BE));
  SET(o, "YUV420P10LE", Nan::New<Number>(AV_PIX_FMT_YUV420P10LE));
  SET(o, "YUV422P10BE", Nan::New<Number>(AV_PIX_FMT_YUV422P10BE));
  SET(o, "YUV422P10LE", Nan::New<Number>(AV_PIX_FMT_YUV422P10LE));
  SET(o, "YUV444P9BE", Nan::New<Number>(AV_PIX_FMT_YUV444P9BE));
  SET(o, "YUV444P9LE", Nan::New<Number>(AV_PIX_FMT_YUV444P9LE));
  SET(o, "YUV444P10BE", Nan::New<Number>(AV_PIX_FMT_YUV444P10BE));
  SET(o, "YUV444P10LE", Nan::New<Number>(AV_PIX_FMT_YUV444P10LE));
  SET(o, "YUV422P9BE", Nan::New<Number>(AV_PIX_FMT_YUV422P9BE));
  SET(o, "YUV422P9LE", Nan::New<Number>(AV_PIX_FMT_YUV422P9LE));
  SET(o, "VDA_VLD", Nan::New<Number>(AV_PIX_FMT_VDA_VLD));
  SET(o, "GBRP", Nan::New<Number>(AV_PIX_FMT_GBRP));
  SET(o, "GBRP9BE", Nan::New<Number>(AV_PIX_FMT_GBRP9BE));
  SET(o, "GBRP9LE", Nan::New<Number>(AV_PIX_FMT_GBRP9LE));
  SET(o, "GBRP10BE", Nan::New<Number>(AV_PIX_FMT_GBRP10BE));
  SET(o, "GBRP10LE", Nan::New<Number>(AV_PIX_FMT_GBRP10LE));
  SET(o, "GBRP16BE", Nan::New<Number>(AV_PIX_FMT_GBRP16BE));
  SET(o, "GBRP16LE", Nan::New<Number>(AV_PIX_FMT_GBRP16LE));
  SET(o, "YUVA422P", Nan::New<Number>(AV_PIX_FMT_YUVA422P));
  SET(o, "YUVA444P", Nan::New<Number>(AV_PIX_FMT_YUVA444P));
  SET(o, "YUVA420P9BE", Nan::New<Number>(AV_PIX_FMT_YUVA420P9BE));
  SET(o, "YUVA420P9LE", Nan::New<Number>(AV_PIX_FMT_YUVA420P9LE));
  SET(o, "YUVA422P9BE", Nan::New<Number>(AV_PIX_FMT_YUVA422P9BE));
  SET(o, "YUVA422P9LE", Nan::New<Number>(AV_PIX_FMT_YUVA422P9LE));
  SET(o, "YUVA444P9BE", Nan::New<Number>(AV_PIX_FMT_YUVA444P9BE));
  SET(o, "YUVA444P9LE", Nan::New<Number>(AV_PIX_FMT_YUVA444P9LE));
  SET(o, "YUVA420P10BE", Nan::New<Number>(AV_PIX_FMT_YUVA420P10BE));
  SET(o, "YUVA420P10LE", Nan::New<Number>(AV_PIX_FMT_YUVA420P10LE));
  SET(o, "YUVA422P10BE", Nan::New<Number>(AV_PIX_FMT_YUVA422P10BE));
  SET(o, "YUVA422P10LE", Nan::New<Number>(AV_PIX_FMT_YUVA422P10LE));
  SET(o, "YUVA444P10BE", Nan::New<Number>(AV_PIX_FMT_YUVA444P10BE));
  SET(o, "YUVA444P10LE", Nan::New<Number>(AV_PIX_FMT_YUVA444P10LE));
  SET(o, "YUVA420P16BE", Nan::New<Number>(AV_PIX_FMT_YUVA420P16BE));
  SET(o, "YUVA420P16LE", Nan::New<Number>(AV_PIX_FMT_YUVA420P16LE));
  SET(o, "YUVA422P16BE", Nan::New<Number>(AV_PIX_FMT_YUVA422P16BE));
  SET(o, "YUVA422P16LE", Nan::New<Number>(AV_PIX_FMT_YUVA422P16LE));
  SET(o, "YUVA444P16BE", Nan::New<Number>(AV_PIX_FMT_YUVA444P16BE));
  SET(o, "YUVA444P16LE", Nan::New<Number>(AV_PIX_FMT_YUVA444P16LE));
  SET(o, "VDPAU", Nan::New<Number>(AV_PIX_FMT_VDPAU));
  SET(o, "XYZ12LE", Nan::New<Number>(AV_PIX_FMT_XYZ12LE));
  SET(o, "XYZ12BE", Nan::New<Number>(AV_PIX_FMT_XYZ12BE));
  SET(o, "NV16", Nan::New<Number>(AV_PIX_FMT_NV16));
  SET(o, "NV20LE", Nan::New<Number>(AV_PIX_FMT_NV20LE));
  SET(o, "NV20BE", Nan::New<Number>(AV_PIX_FMT_NV20BE));
  SET(o, "RGBA64BE", Nan::New<Number>(AV_PIX_FMT_RGBA64BE));
  SET(o, "RGBA64LE", Nan::New<Number>(AV_PIX_FMT_RGBA64LE));
  SET(o, "BGRA64BE", Nan::New<Number>(AV_PIX_FMT_BGRA64BE));
  SET(o, "BGRA64LE", Nan::New<Number>(AV_PIX_FMT_BGRA64LE));
  SET(o, "YVYU422", Nan::New<Number>(AV_PIX_FMT_YVYU422));
  SET(o, "VDA", Nan::New<Number>(AV_PIX_FMT_VDA));
  SET(o, "YA16BE", Nan::New<Number>(AV_PIX_FMT_YA16BE));
  SET(o, "YA16LE", Nan::New<Number>(AV_PIX_FMT_YA16LE));
  SET(o, "GBRAP", Nan::New<Number>(AV_PIX_FMT_GBRAP));
  SET(o, "GBRAP16BE", Nan::New<Number>(AV_PIX_FMT_GBRAP16BE));
  SET(o, "GBRAP16LE", Nan::New<Number>(AV_PIX_FMT_GBRAP16LE));
  SET(o, "QSV", Nan::New<Number>(AV_PIX_FMT_QSV));
  SET(o, "MMAL", Nan::New<Number>(AV_PIX_FMT_MMAL));
  SET(o, "D3D11VA_VLD", Nan::New<Number>(AV_PIX_FMT_D3D11VA_VLD));
  SET(o, "CUDA", Nan::New<Number>(AV_PIX_FMT_CUDA));
  SET(o, "P010LE", Nan::New<Number>(AV_PIX_FMT_P010LE));
  SET(o, "P010BE", Nan::New<Number>(AV_PIX_FMT_P010BE));

  SET(target, "PixelFormats", o);
}


NAN_MODULE_INIT(InitEncoders) {
  auto o = Nan::New<Object>();
  AVCodec *codec = NULL;

  while ((codec = av_codec_next(codec))) {
    if (codec->encode2 || codec->encode_sub)
      SET(o, codec->name, Nan::New<Number>(codec->id));
  }

  SET(target, "Encoders", o);
}


NAN_MODULE_INIT(InitDecoders) {
  auto o = Nan::New<Object>();
  AVCodec *codec = NULL;

  while ((codec = av_codec_next(codec))) {
    if (codec->decode)
      SET(o, codec->name, Nan::New<Number>(codec->id));
  }

  SET(target, "Decoders", o);
}


NAN_MODULE_INIT(InitDemuxers) {
  auto o = Nan::New<Object>();
  AVInputFormat *format = NULL;

  while ((format = av_iformat_next(format))) {
    SET(o, format->name, Nan::New(format->name).ToLocalChecked());
  }

  SET(target, "Demuxers", o);
}


NAN_MODULE_INIT(InitMuxers) {
  auto o = Nan::New<Object>();
  AVOutputFormat *format = NULL;

  while ((format = av_oformat_next(format))) {
    SET(o, format->name, Nan::New(format->name).ToLocalChecked());
  }

  SET(target, "Muxers", o);
}


NAN_MODULE_INIT(InitTypes) {
  InitMediaTypes(target);
  InitSampleFormats(target);
  InitChannelLayouts(target);
  InitPixelFormats(target);
  InitEncoders(target);
  InitDecoders(target);
  InitDemuxers(target);
  InitMuxers(target);
}
