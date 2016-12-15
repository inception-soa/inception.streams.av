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
  debug: require('inception.debug')('media:transcoder'),
  primitives: require('inception.primitives')
};
const _ = require('lodash');


/**
 * A streaming audio/video transcoder
 *
 * @param {Object} opts Configuration options for the transcoder
 * @param {String} opts.sourceFormat The input format
 * @param {String} opts.targetFormat The output format
 * @constructor
 */
function MediaTranscoder(opts) {
  MediaTranscoder.super_.call(this, opts);
}
node.util.inherits(MediaTranscoder, inception.primitives.stream.Transform);


/**
 * Overridden ._transform() method for the Transform streaming
 *
 * @param {[type]} chunk The data to transcode
 * @param {[type]} encoding The encoding of the data
 * @param {Function} callback Function to execute upon completion
 * @override
 */
MediaTranscoder.prototype._transform = function (chunk, encoding, callback) {

};


/**
 * Overridden ._transform() method for the Transform streaming
 * @override
 */
MediaTranscoder.prototype._flush = function () {

};


/**
 * Export the class
 * @type {MediaTranscoder}
 */
module.exports = MediaTranscoder;
