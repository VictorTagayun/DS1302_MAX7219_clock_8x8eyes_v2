/***********************************************************************
 * fontTiny.h - 8x8 fonts defined for ascii characters with codes > 32;
 *         - used in Wise Clock 3;
 * Copyright by FlorinC;
 *   Copyrighted and distributed under the terms of the Berkeley license
 *   (copy freely, but include this notice of original author.)
 ***********************************************************************
 */

// define all ascii characters starting with 32 (blank);
// 4 column-wide font;

char tinyFont[12][8] = {
  
  {
    0x00,
    0x00,    // ____  0
    0x02,    // __X_
    0x05,    // _X_X
    0x05,    // _X_X
    0x05,    // _X_X
    0x02,    // __X_
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  1
    0x02,    // __X_
    0x06,    // _XX_
    0x02,    // __X_
    0x02,    // __X_
    0x07,    // _XXX
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  2
    0x06,    // _XX_
    0x01,    // ___X
    0x02,    // __X_
    0x04,    // _X__
    0x07,    // _XXX
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  3
    0x07,    // _XXX
    0x01,    // ___X
    0x06,    // _XX_
    0x01,    // ___X
    0x06,    // _XX_
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  4
    0x05,    // _X_X
    0x05,    // _X_X
    0x07,    // _XXX
    0x01,    // ___X
    0x01,    // ___X
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  5
    0x07,    // _XXX
    0x04,    // _X__
    0x07,    // _XXX
    0x01,    // ___X
    0x06,    // _XX_
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  6
    0x03,    // __XX
    0x04,    // _X__
    0x07,    // _XXX
    0x05,    // _X_X
    0x02,    // _XXX
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  7
    0x07,    // _XXX
    0x01,    // ___X
    0x02,    // __X_
    0x04,    // _X__
    0x04,    // _X__
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  8
    0x07,    // _XXX
    0x05,    // _X_X
    0x07,    // _XXX
    0x05,    // _X_X
    0x07,    // _XXX
    0x00,    // ____
  },
  {
    0x00,
    0x00,    // ____  9
    0x07,    // _XXX
    0x05,    // _X_X
    0x07,    // _XXX
    0x01,    // ___X
    0x06,    // _XX_
    0x00,    // ____
  },
  {
    0x00,    // ____   A
    0x02,    // __X_
    0x05,    // _X_X
    0x05,    // _X_X
    0x07,    // _XXX
    0x05,    // _X_X
    0x05,    // _X_X
    0x00     // ____
  },
  {
    0x00,    // ____  P
    0x06,    // _XX_
    0x05,    // _X_X
    0x06,    // _XX_
    0x04,    // _X__
    0x04,    // _X__
    0x04,    // _X__
    0x00,    // ____
  }
};
