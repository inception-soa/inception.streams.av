'use strict';
/**
 * @file A streaming audio/video transcoder
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

const NAV = require('./lib/nav');
const AVTranscoder = require('./lib/av_transcoder');
const AVError = require('./lib/av_error');


/**
 * Export the interface
 */
const AV = exports = module.exports = {
  Transcoder: AVTranscoder,

  MediaTypes: NAV.MediaTypes,
  SampleFormats: NAV.SampleFormats,
  ChannelLayouts: NAV.ChannelLayouts,
  PixelFormats: NAV.PixelFormats,
  Encoders: NAV.Encoders,
  Decoders: NAV.Decoders,
  Demuxers: NAV.Demuxers,
  Muxers: NAV.Muxers
};


/**
 * Transcodes the specified stream
 *
 * @param {Readable} stream [description]
 * @param {Object} opts Configuration options for the transcoder
 * @param {String} [opts.outputFormat] The output container format
 * @param {Object} [opts.outputVideo] The output video specification
 * @param {Boolean} [opts.outputVideo.ignore] Whether to omit the video stream
 * @param {String} [opts.outputVideo.codec] The video codec to use
 * @param {Number} [opts.outputVideo.frameRate] The video frame rate
 * @param {String} [opts.outputVideo.frameSize] The video frame size
 * @param {String} [opts.outputVideo.aspect] The video aspect ratio
 * @param {Object} [opts.outputAudio] The output audio specification
 * @param {Boolean} [opts.outputAudio.ignore] Whether to omit the audio stream
 * @param {String} [opts.outputAudio.codec] The audio codec to use
 * @param {Number} [opts.outputAudio.sampleRate] The audio sample rate (in Hz)
 * @param {String} [opts.outputAudio.channelLayout] The audio channel layout
 * @param {String} [opts.outputAudio.volume] The audio volume
 * @return {AVTranscoder}
 */
AV.transcode = function (stream, opts) {
  if (opts)
    return stream.pipe(new AVTranscoder(opts));

  throw AVError.BadArguments({
    message: 'No transcoding options specified!',
    arguments: arguments
  });
};
