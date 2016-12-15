/**
 * @file Inits native components to JavaScript
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

#include <nan.h>
#include "nav_types.h"
#include "nav_transcoder.h"


extern "C" {
NAN_MODULE_INIT(NAVInit) {
  avcodec_register_all();
  av_register_all();

  InitTypes(target);
  NAVTranscoder::Init(target);
}


NODE_MODULE(nav, NAVInit);
}
