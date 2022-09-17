/* //===========================================================\\
   || SCH RLE Image Utilities Code - schrletools.cpp            ||
   ||-----------------------------------------------------------||
   || 0.1 = 17 Sep 2022 - Implementing decode and encode func-  ||
   || tion                                                      ||
   \\===========================================================// */
#include <algorithm>
#include "schrletools.h"
unsigned short SetSignalByte(unsigned short *input, int length)
{
	unsigned short signalbyte;
	for (unsigned short a = 0xffff; 0xfff0 <= a; a--){
		int isa = 0;
		for (int f = 0; f < length; f++) {
			if (input[f] == a) {
				isa++;
			}
		}
		if (isa == 0) {
			signalbyte = a;
			break;
		}
	}
	return signalbyte;
}

int Compress(unsigned short *input, unsigned short *output, int length, bool rgb444 = false)
{
	unsigned short signalbyte;
	int inread = 0, outlength = 0;
	if(rgb444)
	{
		signalbyte = 0xffff;
	}
	else
	{
		signalbyte = SetSignalByte(input, length); //SCH RLE expects a signal byte to tell if reuse or not
		output[outlength++] = signalbyte;
	}
	//Please note that the compression is similar to the Carmack compression. The algorithm is
	//a port from gamecomp.js's carmackize compression, with slight changes to make data that
	//can be interpreted properly.
	//Copyright for gamecomp.js: Copyright 2010-2021 Adam Nielsen <malvineous@shikadi.net>.
	while (inread < length)
	{
		// Look back in the input data, comparing it to the upcoming input data,
		// and see if there is a match for a number of UINT16LE words.
		int maxRun = 0, maxRunStart = 0;
		for (int o = 1; o < 65536; o++)
		{
			// Jump back one more word.
			if (o > inread)
			{
				// Gone all the way back to the start of the output data.
				break;
			}
			//debug('    looking back', o, 'words (index', offPos - o, ')');
			// See if the sequence from this point matches the input data.
			int i;
			int maxLookForward = std::min(std::min(65535, o), (length - inread));
			for (i = 0; i < maxLookForward; i++)
			{
				//debug(`    in: ${input.getPos()}, out: ${output.getPos()}`);
				unsigned short nextInput = input[inread - o + i];
				unsigned short nextOutput = input[inread + i];
				//debug(`        nextInput = ${nextInput.toString(16)}, nextOutput = ${nextOutput.toString(16)}`);
				if (nextInput != nextOutput) break;
			}
			// The run finished with `i` bytes matching.
			//debug(`        run finished with ${i} words matching`);
			if (maxRun < i)
			{
				maxRun = i;
				maxRunStart = inread - o;
			}
			// Keep going with the next word back to see if we can find a longer run.
		}
		if (maxRun >= 4)
		{
			// We found a run worth compressing.
			output[outlength++] = signalbyte;
			output[outlength++] = maxRun;
			output[outlength++] = maxRunStart;
			inread += maxRun;
		}
		else 
		{
			// Just copy across the next word.
			unsigned short nextInput = input[inread++];
			output[outlength++] = nextInput;
		}
	}
	return outlength;
}

int Decompress(unsigned short *input, unsigned short *input2, unsigned short *output, unsigned short length)
{
	int inread = 0, outlength = 0;
	unsigned short signalbyte = input[inread++]; //SCH RLE expects a signal byte to tell if reuse or not
	if (signalbyte < 0xfff0)
	{
		signalbyte = 0xffff;
		inread--;
	}
	while (inread < length)
	{
		unsigned short v = input[inread++];
		if (v == signalbyte)
		{
			// We found a run worth decompressing.
			int count = input[inread++];
			int location = input[inread++];
			for(int i = 0; i < count; i++)
			{
				if(location & 0x8000)
				{
					output[outlength++] = input2[(location - 0x8000) + i];
				}
				else
				{
					output[outlength++] = output[location + i];
				}
			}
		}
		else
		{
			// Just copy across the next word.
			output[outlength++] = v;
		}
	}
	return outlength;
}