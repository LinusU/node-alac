{
  "targets": [
    {
      "target_name": "ALACDecoder",
      "sources": [
        "binding.cc",
        # "codec/ag_dec.c",
        # "codec/ag_enc.c",
        # "codec/ALACBitUtilities.c",
        "codec/ALACDecoder.cpp",
        "codec/dp_dec.c",
        # "codec/EndianPortable.c",
        "codec/matrix_dec.c"
      ],
      "include_dirs": [
        "codec",
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
