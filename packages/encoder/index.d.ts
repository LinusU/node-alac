declare class ALACEncoder {
  static readonly maxEscapeHeaderBytes: number
  readonly bytesPerFrame: number
  readonly bytesPerPacket: number
  readonly cookie: Uint8Array
  constructor ()
  encodeChunk (inputView: TypedArrayView, outputView: TypedArrayView): number
}

export = ALACEncoder
