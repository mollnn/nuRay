#ifndef BIN_READER_H
#define BIN_READER_H

#include <fstream>
#include <string>
#include "uint.h"

class BinReader
{
public:
	BinReader();
	BinReader(const std::string& filePath);
	~BinReader();

	void open(const std::string& filePath);
	bool isOpen();

	uint8 read8();
	uint16 read16();
	uint32 read32();

private:
	std::ifstream file;
	bool opened{ false };
	uint8 buffer[4];

	uint32 bytesToInt(uint8 bytes[], int n);
};

#endif