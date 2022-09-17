/* //===========================================================\\
   || SCH RLE Image Utilities Header - schrletools.h            ||
   ||-----------------------------------------------------------||
   || 0.1 = 17 Sep 2022 - Implementing decode and encode func-  ||
   || tion                                                      ||
   \\===========================================================// */
int Compress(unsigned short *input, unsigned short *output, int length, bool isrgb444);
int Decompress(unsigned short *input, unsigned short *input2, unsigned short *output, int length);
