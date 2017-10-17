#include "b64.h"

#define ENCODED_BLOCK_SIZE 4
#define RAW_BLOCK_SIZE 3

const char Base64::cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

const char Base64::cd64[] = {
	62,
	-1, -1, -1,
	63, 52,53,54,55,56,57,58,59,60,61,
	-1, -1, -1, -1, -1, -1, -1,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
	-1, -1, -1, -1, -1, -1,
	26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51
};

void Base64::encodeblock_(unsigned char *in, unsigned char *out, long len)
{
	out[0] = cb64[(int)(in[0] >> 2)];
	if (len == 1)
	{
		out[1] = cb64[(int)(((in[0] & 0x03) << 4))];
		out[2] = '=';
		out[3] = '=';
		return;
	}
	out[1] = (cb64[(int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4))]);
	if (len == 2)
	{
		out[2] = cb64[(int)((in[1] & 0x0f) << 2)];
		out[3] = '=';
		return;
	}
	out[2] = (len > 1 ? cb64[(int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6))] : '=');
	out[3] = (len > 2 ? cb64[(int)(in[2] & 0x3f)] : '=');
}

void Base64::encodeblock(unsigned char *in, unsigned char *out)
{
	out[0] = cb64[(int)(in[0] >> 2)];
	out[1] = cb64[(int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4))];
	out[2] = cb64[(int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6))];
	out[3] = cb64[(int)(in[2] & 0x3f)];
}

int Base64::decodeblock_(unsigned char *in, unsigned char *out)
{
	unsigned char in_[ENCODED_BLOCK_SIZE];
	in_[0] = cd64[in[0] - '+'];
	in_[1] = cd64[in[1] - '+'];
	in_[2] = cd64[in[2] - '+'];
	in_[3] = cd64[in[3] - '+'];
	out[0] = (((in_[0] << 2) & 0xfc) | in_[1] >> ENCODED_BLOCK_SIZE);
	if (in[2] == '=')
		return 2;
	else
		out[1] = (((in_[1] << ENCODED_BLOCK_SIZE) & 0xf0) | in_[2] >> 2);
	if (in[3] == '=')
		return 1;
	else
		out[2] = (((in_[2] << 6) & 0xc0) | in_[3]);
	return 0;
}

void Base64::decodeblock(unsigned char *in, unsigned char *out)
{
	unsigned char in_[ENCODED_BLOCK_SIZE];
	in_[0] = cd64[in[0] - '+'];
	in_[1] = cd64[in[1] - '+'];
	in_[2] = cd64[in[2] - '+'];
	in_[3] = cd64[in[3] - '+'];
	out[0] = (((in_[0] << 2) & 0xfc) | in_[1] >> 4);
	out[1] = (((in_[1] << 4) & 0xf0) | in_[2] >> 2);
	out[2] = (((in_[2] << 6) & 0xc0) | in_[3]);
}

bool Base64::b64checkChar(char c) 
{
	if (c == '=') 
		return true;
	else if (c < ('+' + 80) && (c >= '+'))
		if (cd64[c - '+'] >= 0)
			return true;

	return false;
}

bool  Base64::CheckIsBase64(char *src, long length) {
	for (long i = 0; i < length; i++)
		if (!b64checkChar(src[i]))
			return false;

	return true;
}

long Base64::CalcEncodedSize(long rawSize) {
	long rest = RAW_BLOCK_SIZE - rawSize % RAW_BLOCK_SIZE;
	long absSize = rawSize + rest;
	return absSize / RAW_BLOCK_SIZE * ENCODED_BLOCK_SIZE;
}
long Base64::CalcDecodedSize(long size)
{
	return size / ENCODED_BLOCK_SIZE * RAW_BLOCK_SIZE;
}

long Base64::Encode(char *dest, char *src, long sizeSrc)
{
	long sizeEncoded = CalcEncodedSize(sizeSrc);
	int rest = sizeSrc % RAW_BLOCK_SIZE;
	long absSize = sizeSrc - rest;
	for (long i = 0, j = 0; i < absSize; i += RAW_BLOCK_SIZE, j += ENCODED_BLOCK_SIZE) {
		encodeblock((unsigned char*)&src[i], (unsigned char*)&dest[j]);
	}
	if (rest)
		encodeblock_((unsigned char*)&src[absSize], (unsigned char*)&dest[sizeEncoded-ENCODED_BLOCK_SIZE],rest);
	return sizeEncoded;
}

long Base64::Decode(char *dest, char *src, long srcLength)
{
	long destLength = CalcDecodedSize(srcLength);
	long length = srcLength - ENCODED_BLOCK_SIZE;
	unsigned char *up_dest = (unsigned char*)dest;
	unsigned char *up_src = (unsigned char*)src;
	for (long i = 0, j = 0; i < length; i += ENCODED_BLOCK_SIZE, j += RAW_BLOCK_SIZE) {
		decodeblock(&up_src[i], &up_dest[j]);
	}
	return destLength - decodeblock_(&up_src[length], &up_dest[destLength - RAW_BLOCK_SIZE]); //result decoded size
}