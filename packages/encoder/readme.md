# ALAC Encoder

An encoder that produces raw ALAC data from PCM data.

## Installation

```sh
npm install --save @alac/encoder
```

## Usage

```js
// The raw data you want to encode, could also be a stream or generated on the fly
const pcmData = new Uint8Array(/* ... */)

const encoder = new ALACEncoder()

// This buffer will be written into from the encoder, it needs to hold the largest possible packet size
const outputBuffer = new Uint8Array(encoder.bytesPerPacket + ALACEncoder.maxEscapeHeaderBytes)

for (let pos = 0; pos < pcmData.byteLength; pos += encoder.bytesPerPacket) {
  // Take the next `encoder.bytesPerPacket` bytes from the raw PCM data
  const chunk = pcmData.subarray(pos, pos + encoder.bytesPerPacket)

  // Encode those bytes into one packet
  const bytesWritten = encoder.encodeChunk(chunk, outputBuffer)

  // Read the first `bytesWritten` bytes of the output buffer to get the final packet
  const packet = outputBuffer.slice(0, bytesWritten)
}
```
