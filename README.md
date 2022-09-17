# History

0.1 = 17 Sep 2022 - Implementing decode and encode function

# Functions
Compress(input, output, length, isrgb444), where input is 16bpp image buffer as unsigned short pointer, output is compressed result as unsigned short pointer, length is input buffer size (not full size, it is divided by 2) as int and isrgb444 bool to determine if signal byte is 0xffff and not between 0xfff0 and 0xffff (generally used for RGB444-type images).
Decompress(input, input2, output, length), where input is compressed image data as unsigned short pointer, input2 is previously decoded frame (useful for some frames where they reuse pixels from previous image) as unsigned short pointer, output is decompressed result as unsigned short pointer and length of input buffer size (not full size, it is divided by 2) as int

# Notes
The compression is similar to the Carmack compression, but has differences to accomodate the "SCH RLE" format specification.
Similarities:
-reuses words from previous data
-has signal byte
Differences:
-sturcture of command (Carmack has near and far pointers (0xa700/0xa800 | length + pointer) and "SCH RLE" has signal byte + length + pointer)
-the length of data is not maxed out to 255
-does not decompress data by going back n words, rather it decompresses from the n'th word

For compression, please note that the algorithm is a port of gamecompress's cmp-carmackize.js's obscure function, but there are some things it can't do:
-Compress with previous image's data
-Have a proper offset for the last run when data has many runs (e.g. if ffff is repeated 3c34 times, the last run doesn't have offset 0)
