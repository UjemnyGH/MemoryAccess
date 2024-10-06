/**
*
* @author Piotr UjemnyGH Plombon
*
* Lib to load config files with my own file format, method definitions
*
*/

#include "memoryConfigFileFormat.hpp"

void MemoryConfigFile::AddString(std::string str) {
	// Add string data type with value
	MemoryConfigData mcd = { .mStr = str, .mDataType = MCDT_string };
	mConvertedData.push_back(mcd);
}

void MemoryConfigFile::AddU8(uint8_t u8) {
	// Add byte data type with value
	MemoryConfigData mcd = { .mU8 = u8, .mDataType = MCDT_u8 };
	mConvertedData.push_back(mcd);
}

void MemoryConfigFile::AddU16(uint16_t u16) {
	// Add word data type with value
	MemoryConfigData mcd = { .mU16 = u16, .mDataType = MCDT_u16 };
	mConvertedData.push_back(mcd);
}

void MemoryConfigFile::AddU32(uint32_t u32) {
	// Add double word data type with value
	MemoryConfigData mcd = { .mU32 = u32, .mDataType = MCDT_u32 };
	mConvertedData.push_back(mcd);
}

void MemoryConfigFile::AddU64(uint64_t u64) {
	// Add quad word data type with value
	MemoryConfigData mcd = { .mU64 = u64, .mDataType = MCDT_u64 };
	mConvertedData.push_back(mcd);
}

void MemoryConfigFile::SaveConfigToFile(std::string path) {
	// Clean file content to be sure there are no error in written file
	mFileContent.clear();

	for (auto& data : mConvertedData) {
		// For every added data type add value type followed by value to file in little endian (strings are exception)
		switch (data.mDataType) {
		case MCDT_string:
			mFileContent += MCDT_string;
			mFileContent += data.mStr;
			mFileContent += '\0';

			break;

		case MCDT_u8:
			mFileContent += MCDT_u8;
			mFileContent += data.mU8;

			break;
		case MCDT_u16:
			mFileContent += MCDT_u16;
			mFileContent += (uint8_t)(data.mU16 & 0xff);
			mFileContent += (uint8_t)((data.mU16 & 0xff00) >> 8);

			break;

		case MCDT_u32:
			mFileContent += MCDT_u32;
			mFileContent += (uint8_t)(data.mU32 & 0xff);
			mFileContent += (uint8_t)((data.mU32 & 0xff00) >> 8);
			mFileContent += (uint8_t)((data.mU32 & 0xff0000) >> 16);
			mFileContent += (uint8_t)((data.mU32 & 0xff000000) >> 24);

			break;

		case MCDT_u64:
			mFileContent += MCDT_u64;
			mFileContent += (uint8_t)(data.mU64 & 0xff);
			mFileContent += (uint8_t)((data.mU64 & 0xff00) >> 8);
			mFileContent += (uint8_t)((data.mU64 & 0xff0000) >> 16);
			mFileContent += (uint8_t)((data.mU64 & 0xff000000) >> 24);
			mFileContent += (uint8_t)((data.mU64 & 0xff00000000) >> 32);
			mFileContent += (uint8_t)((data.mU64 & 0xff0000000000) >> 40);
			mFileContent += (uint8_t)((data.mU64 & 0xff000000000000) >> 48);
			mFileContent += (uint8_t)((data.mU64 & 0xff00000000000000) >> 56);

			break;
		}
	}

	std::ofstream file;
	// Make file in binary
	file.open(path + std::string(".macfg"), std::ios::binary);

	// Write to file
	file.write(mFileContent.c_str(), mFileContent.size());

	// And close file
	file.close();
}

bool MemoryConfigFile::LoadConfigFromFIle(std::string path) {
	std::ifstream file;
	// Open file in binary starting from end
	file.open(path + std::string(".macfg"), std::ios::binary | std::ios::ate);

	// If file don`t exist return false
	if (!file.is_open() || file.bad()) {
		return false;
	}

	// Get length
	std::streampos length = file.tellg();
	// And seek to beginning of a file
	file.seekg(0, std::ios::beg);

	// Clear file content as we read to it
	mFileContent.clear();
	// Resize it to fit entire file
	mFileContent.resize(length);
	// Clear converted data vector
	mConvertedData.clear();

	// Read to file content string
	file.read(mFileContent.data(), length);

	// Close file
	file.close();

	for (std::size_t i = 0; i < mFileContent.size(); i++) {
		MemoryConfigData mcd;
		
		// Read data byte by byte
		switch (mFileContent[i]) {
		case MCDT_string:
			// Set data type
			mcd.mDataType = MCDT_string;
			mcd.mStr.clear();
			i++;
			// Strings ends with '/0' so until there is no 0 in file read string
			while (mFileContent[i] != 0) {
				mcd.mStr += (uint8_t)mFileContent[i];
				i++;
			}

			break;

		case MCDT_u8:
			// Set data type and read data
			mcd.mDataType = MCDT_u8;
			mcd.mU8 = (uint8_t)mFileContent[++i];

			break;

		case MCDT_u16:
			// Set data type and read data
			mcd.mDataType = MCDT_u16;
			mcd.mU16 = (uint16_t)((uint8_t)mFileContent[++i]);
			mcd.mU16 |= ((uint16_t)((uint8_t)mFileContent[++i]) << 8);

			break;

		case MCDT_u32:
			// Set data type and read data
			mcd.mDataType = MCDT_u32;
			mcd.mU32 = (uint32_t)((uint8_t)mFileContent[++i]);
			mcd.mU32 |= ((uint32_t)((uint8_t)mFileContent[++i]) << 8);
			mcd.mU32 |= ((uint32_t)((uint8_t)mFileContent[++i]) << 16);
			mcd.mU32 |= ((uint32_t)((uint8_t)mFileContent[++i]) << 24);

			break;

		case MCDT_u64:
			// Set data type and read data
			mcd.mDataType = MCDT_u64;
			mcd.mU64 = (uint64_t)((uint8_t)mFileContent[++i]);
			mcd.mU64 |= ((uint64_t)((uint8_t)mFileContent[++i]) << 8);
			mcd.mU64 |= ((uint64_t)((uint8_t)mFileContent[++i]) << 16);
			mcd.mU64 |= ((uint64_t)((uint8_t)mFileContent[++i]) << 24);
			mcd.mU64 |= ((uint64_t)((uint8_t)mFileContent[++i]) << 32);
			mcd.mU64 |= ((uint64_t)((uint8_t)mFileContent[++i]) << 40);
			mcd.mU64 |= ((uint64_t)((uint8_t)mFileContent[++i]) << 48);
			mcd.mU64 |= ((uint64_t)((uint8_t)mFileContent[++i]) << 56);

			break;

		default:
			// On default we do nothing
			break;
		}

		// After reading dara push it to converted data
		mConvertedData.emplace_back(mcd);
	}

	// Return true on success
	return true;
}