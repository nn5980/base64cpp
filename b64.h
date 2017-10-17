class Base64 {
	static const char cb64[];
	static const char cd64[];
	static void encodeblock_(unsigned char *in, unsigned char *out, long len);
	static void encodeblock(unsigned char *in, unsigned char *out);
	static int decodeblock_(unsigned char *in, unsigned char *out);
	static void decodeblock(unsigned char *in, unsigned char *out);
	static bool  b64checkChar(char);
public:
	static bool CheckIsBase64(char *src, long length);
	static long CalcEncodedSize(long rawSize);
	static long CalcDecodedSize(long size);
	static long Encode(char *dest, char *src, long srcLength);
	static long Decode(char *dest, char *src, long srcLength);
};
