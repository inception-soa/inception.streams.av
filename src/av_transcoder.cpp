/**
 * @file A libav transcoder implementation
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

#include "av_transcoder.h"


using namespace v8;
using namespace std;


/**
 * The AVTranscoder constructor
 */
AVTranscoder::AVTranscoder(AVTranscodeWorker *worker) : worker_(worker) {}


/**
 * The AVTranscoder destructor
 */
AVTranscoder::~AVTranscoder() {}


/**
 * Creates and returns a new instance of the AVTranscoder
 */
NAN_METHOD(AVTranscoder::New) {
  // Handle non-construct calls (instantiation attempts without using `new`)
  if (!info.IsConstructCall()) {
    vector<Local<Value> > args(info.Length());
    for (size_t i = 0; i < args.size(); i++) {
      args[i] = info[i];
    }

    auto inst = Nan::NewInstance(info.Callee(), args.size(), args.data());
    if (!inst.IsEmpty()) {
      info.GetReturnValue().Set(inst.ToLocalChecked());
    }

    return;
  }

  // Instantiate a native transcoder
  auto opts = info[0].As<Object>();
  auto emit = new Nan::Callback(info[1].As<Function>());
  auto cb = new Nan::Callback(info[2].As<Function>());
  auto avTranscoder = new AVTranscoder(new AVTranscodeWorker(opts, emit, cb));

  // Return the newly instantiated native transcoder
  avTranscoder->Wrap(info.This());
  info.GetReturnValue().Set(info.This());

  // Start the worker
  Nan::AsyncQueueWorker(avTranscoder->worker_);
}


/**
 * Enqueues an incoming chunk of data for the transcode worker
 */
NAN_METHOD(AVTranscoder::OnChunk) {
  auto data = node::Buffer::Data(info[0]);
  auto size = node::Buffer::Length(info[0]);
  auto avTranscoder = Nan::ObjectWrap::Unwrap<AVTranscoder>(info.Holder());

  avTranscoder->worker_->EnqueueChunk(data, size);
}


/**
 * Initializes the native module
 */
NAN_MODULE_INIT(AVTranscoder::Init) {
  av_register_all();

  auto ctor = Nan::New<FunctionTemplate>(New);
  auto ctorName = Nan::New<String>("NAVTranscoder").ToLocalChecked();
  auto ctorInstance = ctor->InstanceTemplate();

  ctor->SetClassName(ctorName);
  ctorInstance->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(ctor, "onChunk", AVTranscoder::OnChunk);

  Nan::Set(target, ctorName, Nan::GetFunction(ctor).ToLocalChecked());
}


NODE_MODULE(nav_transcoder, AVTranscoder::Init)
