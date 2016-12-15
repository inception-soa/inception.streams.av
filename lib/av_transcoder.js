'use strict';
/**
 * @file A streaming AV transcoder
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

const node = {
  util: require('util')
};
const inception = {
  debug: require('inception.debug')('av:transcoder'),
  primitives: require('inception.primitives')
};
const NAV = require('./nav');
const AVError = require('./av_error');
const _ = require('lodash');


/**
 * A streaming audio/video transcoder
 *
 * @param {Object} opts Configuration options for the transcoder
 * @param {String} [opts.outputFormatName] The output container format name
 * @param {String} [opts.outputFormatFilename] The output filename
 * @param {String} [opts.outputFormatMime] The output MIME type
 * @param {Boolean} [opts.outputAudioIgnore] Whether to omit the audio stream
 * @param {String} [opts.outputAudioCodec='pcm_s16le'] The audio codec to use
 * @param {Number} [opts.outputAudioSampleRate=44100] The audio sample rate (in Hz)
 * @param {Number} [opts.outputAudioChannelLayout=STEREO] The audio channel layout
 * @param {Boolean} [opts.outputVideoIgnore=false] Whether to omit the video stream
 * @param {String} [opts.outputVideoCodec='mp4'] The video codec to use
 * @param {Number} [opts.outputVideoFrameRate=30] The video frame rate
 * @param {String} [opts.outputVideoFrameSize] The video frame size
 * @param {String} [opts.outputVideoAspect] The video aspect ratio
 * @constructor
 */
function AVTranscoder(opts) {
  opts.outputAudioIgnore = (opts.outputAudioIgnore === true);
  opts.outputAudioCodec = opts.outputAudioCodec || NAV.Encoders.pcm_s16le;
  opts.outputAudioSampleRate = opts.outputAudioSampleRate || 44100;
  opts.outputAudioChannelLayout = opts.outputAudioChannelLayout ||
    NAV.ChannelLayouts.STEREO;

  opts.outputVideoIgnore = (opts.outputVideoIgnore === true);
  opts.outputVideoCodec = opts.outputVideoCodec || NAV.Encoders.mp4;
  opts.outputVideoFrameRate = opts.outputVideoFrameRate || 30;
  opts.outputVideoFrameSize = opts.outputVideoFrameSize || '1080p';
  opts.outputVideoAspect = opts.outputVideoAspect || '16:9';

  AVTranscoder.super_.call(this, _.extend({}, opts, { decodeStrings: true }));

  this._errorOccurred = false;
  this._bytesReceived = 0;
  try {
    this._native = new NAV.Transcoder(opts)
      .on('data', (chunk) => {
        this._bytesReceived += chunk.length;
        console.log('Received %d bytes of data so far', this._bytesReceived);
        this.push(chunk);
      })
      .on('end', () => this.push(null));
  } catch (e) {
    throw AVError[e.msg]();
  }
}
node.util.inherits(AVTranscoder, inception.primitives.stream.Transform);


/**
 * Overridden _transform() method of the Transform stream class
 *
 * @param {Buffer} chunk The chunk of data to be transcoded
 * @param {String} encoding The encoding of the chunk
 * @param {Function} callback The function to execute after processing the chunk
 */
AVTranscoder.prototype._transform = function (chunk, encoding, callback) {
  try {
    console.log("_transform called with chunk of size " + chunk.length);
    this._native.transcode(chunk);
  } catch (e) {
    console.log(e);
    throw e; // FIXME
  }

  callback();
};


AVTranscoder.prototype._onError = function (errCode, msg) {
  this._errorOccurred = true;
  this.emit('error', AVError[errCode](msg));
};


/**
 * Export the class
 * @type {Class}
 */
module.exports = AVTranscoder;
