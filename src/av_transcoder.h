/**
 * @file A libav transcoder declaration
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

#ifndef __AV_TRANSCODER__
#define __AV_TRANSCODER__

#include "av_transcode_worker.h"


class AVTranscoder: public Nan::ObjectWrap {
  public:
    static NAN_MODULE_INIT(Init);

  private:
    AVTranscoder(AVTranscodeWorker *);
    ~AVTranscoder();

    static NAN_METHOD(New);
    static NAN_METHOD(OnChunk);

    AVTranscodeWorker *worker_;
};


#endif /* end of include guard: __AV_TRANSCODER__ */
