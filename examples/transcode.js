'use strict';
/**
 * @file Transcodes the input file and writes a new output file
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
  fs: require('fs')
};
const Transcoder = require('..');


node.fs.createReadStream(process.argv[2])
  .pipe(new Transcoder())
  .on('progress', () => {
    process.stdout.write('.');
  })
  .pipe(node.fs.createWriteStream(process.argv[3]));
