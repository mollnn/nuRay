#ifndef BITMAP_H
#define BITMAP_H

#include <string>
#include "uint.h"
#include "BinReader.h"

struct BITMAP_FILE_HEADER
{
	uint16 signature;
	uint32 fileSize;
	uint16 reserved1;
	uint16 reserved2;
	uint32 pixelsOffset;

	std::string toString();
};

struct BITMAP_CORE_HEADER
{
	uint32 size;
	uint32 width;
	uint32 height;
	uint16 planes;
	uint16 bpp;

	std::string toString();
};

struct BITMAP_INFO_HEADER
{
	uint32 size;
	uint32 width;
	uint32 height;
	uint16 planes;
	uint16 bpp;

	uint32 compression;
	uint32 imageSize;
	uint32 xPxPerMeter;
	uint32 yPxPerMeter;
	uint32 usedColors;
	uint32 importantColors;

	std::string toString();
};

class Bitmap
{
public:
	Bitmap(const std::string& filePath);
	~Bitmap();

	void load(const std::string& filePath);
	bool isLoaded();

	BITMAP_FILE_HEADER& getFileHeader();
	BITMAP_INFO_HEADER& getInfoHeader();
	uint8* getPixels();

	void toString();

private:
	BinReader file;

	uint8* pixels{ nullptr };
	BITMAP_FILE_HEADER fileHeader;
	BITMAP_INFO_HEADER infoHeader;

	void parseFileHeader();
	void parseInfoHeader(uint32 headerSize);
	void parsePixels();
};

#endif