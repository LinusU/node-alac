#include <nan.h>

#include "ALACAudioTypes.h"
#include "ALACBitUtilities.h"
#include "ALACDecoder.h"

namespace linusu {
  class Wrapper: public Nan::ObjectWrap {
    public:
      static NAN_MODULE_INIT(Init);

    private:
      explicit Wrapper(ALACDecoder *decoder);
      ~Wrapper();

      static NAN_METHOD(New);
      static NAN_METHOD(DecodeChunk);

      ALACDecoder *decoder;
  };

  NAN_MODULE_INIT(Wrapper::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ALACDecoder").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "decodeChunk", DecodeChunk);

    Nan::Set(target, Nan::New("ALACDecoder").ToLocalChecked(), Nan::GetFunction(tpl).ToLocalChecked());
  }

  Wrapper::Wrapper(ALACDecoder *decoder): decoder(decoder) {}
  Wrapper::~Wrapper() {}

  NAN_METHOD(Wrapper::New) {
    if(!info.IsConstructCall()) {
      return Nan::ThrowTypeError(Nan::New("Class constructor ALACDecoder cannot be invoked without 'new'").ToLocalChecked());
    }

    Nan::TypedArrayContents<uint8_t> cookieView(info[0]);

    void *cookieData = *cookieView;
    uint32_t cookieSize = cookieView.length();

    ALACDecoder *decoder = new ALACDecoder();

    int32_t status = decoder->Init(cookieData, cookieSize);

    if (status != ALAC_noErr) {
      return Nan::ThrowError(Nan::New("Failed to initialize ALAC decoder").ToLocalChecked());
    }

    Wrapper *obj = new Wrapper(decoder);

    obj->Wrap(info.This());

    auto bytesPerFrame = Nan::New<v8::Uint32>(decoder->mConfig.numChannels * (decoder->mConfig.bitDepth >> 3));
    auto bytesPerPacket = Nan::New<v8::Uint32>(decoder->mConfig.numChannels * (decoder->mConfig.bitDepth >> 3) * decoder->mConfig.frameLength);

    Nan::Set(info.This(), Nan::New("bytesPerFrame").ToLocalChecked(), bytesPerFrame);
    Nan::Set(info.This(), Nan::New("bytesPerPacket").ToLocalChecked(), bytesPerPacket);
    Nan::Set(info.This(), Nan::New("cookie").ToLocalChecked(), info[0]);

    info.GetReturnValue().Set(info.This());
  }

  NAN_METHOD(Wrapper::DecodeChunk) {
    Wrapper *obj = Nan::ObjectWrap::Unwrap<Wrapper>(info.This());
    ALACDecoder *decoder = obj->decoder;

    BitBuffer input;

    Nan::TypedArrayContents<uint8_t> alacView(info[0]);
    Nan::TypedArrayContents<uint8_t> pcmView(info[1]);

    BitBufferInit(&input, *alacView, alacView.length());

    uint8_t *pcmData = *pcmView;
    uint32_t numFrames;

    int32_t status = decoder->Decode(&input, pcmData, decoder->mConfig.frameLength, decoder->mConfig.numChannels, &numFrames);

    if (status != ALAC_noErr) {
      return Nan::ThrowError(Nan::New("Failed to decode ALAC data").ToLocalChecked());
    }

    info.GetReturnValue().Set(numFrames);
  }

  NODE_MODULE(Wrapper, Wrapper::Init)
}
