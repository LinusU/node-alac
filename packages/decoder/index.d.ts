declare class ALACDecoder {
  readonly bytesPerFrame: number
  readonly bytesPerPacket: number
  readonly cookie: Uint8Array
  constructor (cookie: Uint8Array)
  decodeChunk (inputView: TypedArrayView, outputView: TypedArrayView): number
}

export = ALACDecoder
