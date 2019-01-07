/* eslint-env mocha */

const fs = require('fs')
const path = require('path')
const assert = require('assert')

const ALACDecoder = require('./packages/decoder')
const ALACEncoder = require('./packages/encoder')

const pcmSample = fs.readFileSync(path.join(__dirname, 'fixtures/sample.pcm'))
const kukiSample = fs.readFileSync(path.join(__dirname, 'fixtures/sample.kuki'))
const alacSample = fs.readFileSync(path.join(__dirname, 'fixtures/sample.alac'))
const paktSample = fs.readFileSync(path.join(__dirname, 'fixtures/sample.pakt'))

describe('ALAC', () => {
  it('encodes PCM bytes', () => {
    const encoder = new ALACEncoder()

    assert.strictEqual(encoder.bytesPerFrame, 4)
    assert.strictEqual(encoder.bytesPerPacket, 1408)
    assert.deepStrictEqual(Buffer.from(encoder.cookie), kukiSample)

    const chunkSize = encoder.bytesPerPacket
    const outputBuffer = new Uint8Array(chunkSize + 8)

    const result = []
    for (let pos = 0; pos < pcmSample.byteLength; pos += chunkSize) {
      const chunk = pcmSample.subarray(pos, pos + chunkSize)
      const bytesWritten = encoder.encodeChunk(chunk, outputBuffer)
      result.push(Buffer.from(outputBuffer.subarray(0, bytesWritten)))
    }

    const packets = new Uint32Array(result.map(b => b.byteLength))
    const expectedPackets = new Uint32Array(paktSample.buffer, paktSample.byteOffset, paktSample.byteLength / Uint32Array.BYTES_PER_ELEMENT)

    assert.deepStrictEqual(Buffer.concat(result), alacSample)
    assert.deepStrictEqual(packets, expectedPackets)
  })

  it('decodes ALAC bytes', () => {
    assert.strictEqual(ALACEncoder.maxEscapeHeaderBytes, 8)

    const decoder = new ALACDecoder(kukiSample)

    assert.strictEqual(decoder.bytesPerFrame, 4)
    assert.strictEqual(decoder.bytesPerPacket, 1408)
    assert.deepStrictEqual(Buffer.from(decoder.cookie), kukiSample)

    const packets = new Uint32Array(paktSample.buffer, paktSample.byteOffset, paktSample.byteLength / Uint32Array.BYTES_PER_ELEMENT)
    const outputBuffer = new Uint8Array(decoder.bytesPerPacket)

    let pos = 0
    const result = []
    for (const packetSize of packets) {
      const packet = alacSample.subarray(pos, pos + packetSize)
      pos += packetSize
      const framesWritten = decoder.decodeChunk(packet, outputBuffer)
      const bytesWritten = framesWritten * decoder.bytesPerFrame
      result.push(Buffer.from(outputBuffer.subarray(0, bytesWritten)))
    }

    assert.deepStrictEqual(Buffer.concat(result), pcmSample)
  })
})
