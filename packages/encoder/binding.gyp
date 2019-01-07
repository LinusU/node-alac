{
  "targets": [
    {
      "target_name": "ALACEncoder",
      "sources": [
        "binding.cc",
        "codec/ag_dec.c",
        "codec/ag_enc.c",
        "codec/ALACBitUtilities.c",
        "codec/ALACEncoder.cpp",
        "codec/dp_enc.c",
        "codec/EndianPortable.c",
        "codec/matrix_enc.c"
      ],
      "include_dirs": [
        "codec",
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
