# ALAC Decoder

A decoder that turns raw ALAC data into PCM data.

## Installation

```sh
npm install --save @alac/decoder
```

## Usage

```js
const cookie = new Uint8Array(/* ... */) // e.g. from CAF 'kuki' chunk
const packets = new Uint32Array(/* ... */) // e.g. from CAF 'pakt' chunk

const decoder = new ALACDecoder(cookie)

// This buffer will be written into from the decoder, it needs to hold at least one full decoded packet
const outputBuffer = new Uint8Array(decoder.bytesPerPacket)

for (const size of packets) {
  const packet = new Uint8Array(/* ... */) // the next `size` bytes from your ALAC data

  const framesWritten = decoder.decodeChunk(packet, outputBuffer)
  const bytesWritten = framesWritten * decoder.bytesPerFrame

  // The first `bytesWritten` bytes will now be filled with PCM data
  const decodedChunk = outputBuffer.slice(0, bytesWritten)
}
```
