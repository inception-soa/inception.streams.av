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

const node = {
  util: require('util')
};
const inception = {
  debug: require('inception.debug')('av:transcoder'),
  primitives: require('inception.primitives')
};
const native = require('bindings')('av_transcoder.node');


/**
 * A streaming audio/video transcoder
 *
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
 * @param {String} [opts.outputVideo.channels] The audio channel count
 * @param {String} [opts.outputVideo.volume] The audio volume
 * @constructor
 */
function AVTranscoder(opts) {
  AVTranscoder.super_.call(this, opts);

  this._errorOccurred = false;
  this._native = new native.AVTranscoder.stream(opts, (eventName, eventArg) => {
    switch (eventName) {
    case 'error':
      this._onError(eventArg);
      break;

    case 'drain':
      this.uncork();
      break;

    case 'data':
      this.push(eventArg);
      break;
    }
  }, (err) => {
    if (err)
      return this._onError(err);
  });
}
node.util.inherits(AVTranscoder, inception.primitives.stream.Transform);


/**
 * Overridden _transform() method of the Transform stream class
 *
 * @param {Buffer} chunk The chunk of data to be transcoded
 * @param {String} encoding The encoding of the chunk; must be `buffer`
 * @param {Function} callback The function to execute after processing the chunk
 */
AVTranscoder.prototype._transform = function (chunk, encoding, callback) {
  this._native.onChunk(chunk);
  this.cork();

  callback();
};


AVTranscoder.prototype._onError = function (err) {
  this._errorOccurred = true;
  this.emit('error', err);
};


/**
 * Export the class
 * @type {Class}
 */
module.exports = AVTranscoder;
