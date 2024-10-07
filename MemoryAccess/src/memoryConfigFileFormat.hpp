/**
* 
* @author Piotr UjemnyGH Plombon
* 
* Lib to load config files with my own file format
* 
*/

#pragma once
#ifndef _MA_MEMORY_CONFIG_FILE_FORMAT_
#define _MA_MEMORY_CONFIG_FILE_FORMAT_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Possible saved data types ( you can save any data type with this, don`t complain, just think ;) )
enum MemoryConfigDataType {
	MCDT_none = 0,
	MCDT_string,
	MCDT_u8,
	MCDT_u16,
	MCDT_u32,
	MCDT_u64,
	MCDT_COUNT
};

// Saved memory config data (could be more optimised but it is so small that union doesn`t make a difference and sense)
struct MemoryConfigData {
	std::string mStr;
	std::uint8_t mU8;
	std::uint16_t mU16;
	std::uint32_t mU32;
	std::uint64_t mU64;

	// There shouldn`t be a "none" type so this should be treated as an error
	std::uint8_t mDataType = MCDT_none;
};

class MemoryConfigFile {
private:
	// Generic file name fo saved/loaded file
	std::string mFilename;
	// File content to convert or save
	std::string mFileContent;
	// Converted data to use
	std::vector<MemoryConfigData> mConvertedData;

public:
	// Gets memory config data by id
	MemoryConfigData& operator[](int id) { return mConvertedData[id]; }
	// Returns pointer to entire memory converted data vector 
	std::vector<MemoryConfigData>* operator()() { return &mConvertedData;  }

	// Adds string to converted data
	void AddString(std::string str);

	// Adds byte to converted data
	void AddU8(uint8_t u8);

	// Adds wword to converted data
	void AddU16(uint16_t u16);

	// Adds double word to converted data
	void AddU32(uint32_t u32);

	// Adds quad word to converted data
	void AddU64(uint64_t u64);

	// Save config at desired location
	void SaveConfigToFile(std::string path);

	// Load config from desired location
	bool LoadConfigFromFile(std::string path);
};

#endif