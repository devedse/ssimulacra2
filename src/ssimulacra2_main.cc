// Copyright (c) Jon Sneyers, Cloudinary. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <stdio.h>

#include "lib/extras/codec.h"
#include "lib/jxl/color_management.h"
#include "lib/jxl/enc_color_management.h"
#include "ssimulacra2.h"

int PrintUsage(char **argv) {
  fprintf(stderr, "Usage: %s orig.png distorted.png\n", argv[0]);
  fprintf(stderr,
          "Returns a score in range -inf..100, which correlates to subjective "
          "visual quality:\n");
  fprintf(stderr,
          "     30 = low quality (p10 worst output of mozjpeg -quality 30)\n");
  fprintf(stderr,
          "     50 = medium quality (average output of cjxl -q 40 or mozjpeg "
          "-quality 40,\n");
  fprintf(stderr,
          "                          p10 output of cjxl -q 50 or mozjpeg "
          "-quality 60)\n");
  fprintf(stderr,
          "     70 = high quality (average output of cjxl -q 70 or mozjpeg "
          "-quality 70,\n");
  fprintf(stderr, "                        p10 output of cjxl -q 75 or mozjpeg "
                  "-quality 80)\n");
  fprintf(stderr, "     90 = very high quality (impossible to distinguish from "
                  "original at 1:1,\n");
  fprintf(stderr,
          "                             average output of cjxl -q 90 or "
          "mozjpeg -quality 90)\n");
  return 1;
}

int main(int argc, char **argv) {
  if (argc != 3)
    return PrintUsage(argv);

  jxl::CodecInOut io1;
  jxl::CodecInOut io2;
  if (!SetFromFile(argv[1], jxl::extras::ColorHints(), &io1)) {
    fprintf(stderr, "Could not load original image: %s\n", argv[1]);
    return 1;
  }

  if (io1.xsize() < 8 || io1.ysize() < 8) {
    fprintf(stderr, "Minimum image size is 8x8 pixels\n");
    return 1;
  }

  if (!SetFromFile(argv[2], jxl::extras::ColorHints(), &io2)) {
    fprintf(stderr, "Could not load distorted image: %s\n", argv[2]);
    return 1;
  }

  if (io1.xsize() != io2.xsize() || io1.ysize() != io2.ysize()) {
    fprintf(stderr, "Image size mismatch\n");
    return 1;
  }

  if (!io1.Main().HasAlpha()) {
    Msssim msssim = ComputeSSIMULACRA2(io1.Main(), io2.Main());
    printf("%.8f\n", msssim.Score());
  } else {
    // in case of alpha transparency: blend against dark and bright backgrounds
    // and return the worst of both scores
    Msssim msssim0 = ComputeSSIMULACRA2(io1.Main(), io2.Main(), 0.1f);
    Msssim msssim1 = ComputeSSIMULACRA2(io1.Main(), io2.Main(), 0.9f);
    printf("%.8f\n", std::min(msssim0.Score(), msssim1.Score()));
  }
  return 0;
}
