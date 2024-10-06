/**
* 
* @author Piotr UjemnyGH Plombon
* 
* App core
* 
*/

#pragma once
#ifndef _MA_CORE_
#define _MA_CORE_

#include <Windows.h>
#include <iostream>

// Macro to make software version in just 4 bytes
#define MA_MAKE_SOFT_VERSION(release, major, minor, snap) ((release << 24) | (major << 16) | (minor << 8) | snap)

// Global counter (quite useful)
extern int gGlobalCounter;

// Software version
extern std::uint32_t gSoftwareVersion;

std::string SoftwareVerisonToString();

// Reset global counter
void ResetGlobalCounter();

// Convert string into int
constexpr std::uint32_t StringToInt(const char* str, int h = 0);

// Convert keyname into virtual key value
const std::int32_t KeynameToVkValue(const char* keyname);

#endif // _MA_CORE_