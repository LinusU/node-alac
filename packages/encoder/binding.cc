#include <nan.h>

#include "ALACBitUtilities.h"
#include "ALACEncoder.h"

namespace linusu {
  static int kChannelCount = 2;
  static int kBitsPerChannel = 16;

  static int kBlockSize = 16;
  static int kFramesPerPacket = 352;

  void fillInputAudioFormat(AudioFormatDescription *format) {
    format->mFormatID = kALACFormatLinearPCM;
    format->mSampleRate = 44100;
    format->mFormatFlags = 12;

    format->mBytesPerPacket = 4;
    format->mBytesPerFrame = 4;
    format->mBitsPerChannel = kBitsPerChannel;
    format->mChannelsPerFrame = kChannelCount;
    format->mFramesPerPacket = 1;

    format->mReserved = 0;
  }

  void fillOutputAudioFormat(AudioFormatDescription *format) {
    format->mFormatID = kALACFormatAppleLossless;
    format->mSampleRate = 44100;
    format->mFormatFlags = 1;

    format->mBytesPerPacket = 0;
    format->mBytesPerFrame = 0;
    format->mBitsPerChannel = 0;
    format->mChannelsPerFrame = kChannelCount;
    format->mFramesPerPacket = kFramesPerPacket;

    format->mReserved = 0;
  }

  class Wrapper: public Nan::ObjectWrap {
    public:
      static NAN_MODULE_INIT(Init);

    private:
      explicit Wrapper(ALACEncoder *encoder);
      ~Wrapper();

      static NAN_METHOD(New);
      static NAN_METHOD(EncodeChunk);

      ALACEncoder *encoder;
  };

  NAN_MODULE_INIT(Wrapper::Init) {
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("ALACEncoder").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "encodeChunk", EncodeChunk);

    auto jsClass = Nan::GetFunction(tpl).ToLocalChecked();
    auto maxEscapeHeaderBytes = Nan::New<v8::Uint32>(kALACMaxEscapeHeaderBytes);

    Nan::Set(jsClass, Nan::New("maxEscapeHeaderBytes").ToLocalChecked(), maxEscapeHeaderBytes);
    Nan::Set(target, Nan::New("ALACEncoder").ToLocalChecked(), jsClass);
  }

  Wrapper::Wrapper(ALACEncoder *encoder): encoder(encoder) {}
  Wrapper::~Wrapper() {}

  NAN_METHOD(Wrapper::New) {
    if(!info.IsConstructCall()) {
      return Nan::ThrowTypeError(Nan::New("Class constructor ALACEncoder cannot be invoked without 'new'").ToLocalChecked());
    }

    AudioFormatDescription outputFormat;
    fillOutputAudioFormat(&outputFormat);

    ALACEncoder *encoder = new ALACEncoder();

    encoder->SetFrameSize(kFramesPerPacket);
    encoder->InitializeEncoder(outputFormat);

    Wrapper *obj = new Wrapper(encoder);

    obj->Wrap(info.This());

    auto cookieSize = encoder->GetMagicCookieSize(outputFormat.mChannelsPerFrame);
    auto cookieBuffer = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), cookieSize);
    auto cookieView = v8::Uint8Array::New(cookieBuffer, 0, cookieSize);

    Nan::TypedArrayContents<uint8_t> cookieContents(cookieView);
    encoder->GetMagicCookie(*cookieContents, &cookieSize);

    auto bytesPerFrame = Nan::New<v8::Uint32>(kChannelCount * (kBitsPerChannel >> 3));
    auto bytesPerPacket = Nan::New<v8::Uint32>(kChannelCount * (kBitsPerChannel >> 3) * kFramesPerPacket);

    Nan::Set(info.This(), Nan::New("bytesPerFrame").ToLocalChecked(), bytesPerFrame);
    Nan::Set(info.This(), Nan::New("bytesPerPacket").ToLocalChecked(), bytesPerPacket);
    Nan::Set(info.This(), Nan::New("cookie").ToLocalChecked(), cookieView);

    info.GetReturnValue().Set(info.This());
  }

  NAN_METHOD(Wrapper::EncodeChunk) {
    Wrapper *obj = Nan::ObjectWrap::Unwrap<Wrapper>(info.This());
    ALACEncoder *encoder = obj->encoder;

    Nan::TypedArrayContents<uint8_t> pcmView(info[0]);
    Nan::TypedArrayContents<uint8_t> alacView(info[1]);

    uint8_t *pcmData = *pcmView;
    uint32_t pcmSize = pcmView.length();
    uint8_t *alacData = *alacView;

    AudioFormatDescription inputFormat, outputFormat;
    fillInputAudioFormat(&inputFormat);
    fillOutputAudioFormat(&outputFormat);

    int32_t alacSize = pcmSize;
    int32_t status = encoder->Encode(inputFormat, outputFormat, pcmData, alacData, &alacSize);

    if (status != ALAC_noErr) {
      return Nan::ThrowError(Nan::New("Failed to encode PCM data").ToLocalChecked());
    }

    info.GetReturnValue().Set(alacSize);
  }

  NODE_MODULE(Wrapper, Wrapper::Init)
}
