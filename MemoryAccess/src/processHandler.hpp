/**
* 
* @author Piotr UjemnyGH Plombon
* 
* This stay as header only for convinience 
* and ease of implementing in other projects
* 
*/

#pragma once
#ifndef __PROCESS_HANDLER_
#define __PROCESS_HANDLER_

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>

/**
* Process handler
*/
class Process {
private:
	// Process id
	uint32_t mProcessId = 0;
	// Process handle
	HANDLE mProcessHandle = nullptr;

public:	
	// Gets process id
	uint32_t operator()() { return mProcessId; }
	// Gets process handle
	void operator()(PHANDLE handle) { handle = &mProcessHandle; }

	/** 
	* Open process from process id
	*/
	void OpenProc() {
		// If there is no process id, show error in console
		if (mProcessId != 0) 
			mProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, mProcessId);
		else
			std::cout << "[WARN]: mProcessId = 0 in " << __FUNCSIG__ << " - No process id!\n";

		if(!mProcessHandle) {
			std::cout << "[ERROR]: mProcessHandle = nullptr in " << __FUNCSIG__ << " - No process handle!\n";
		}

		if (mProcessHandle == INVALID_HANDLE_VALUE) {
			std::cout << "[ERROR]: mProcessHandle = INVALID_HANDLE_VALUE in " << __FUNCSIG__ << " - Invalid process handle!\n";
		}
	}

	/** 
	* Close process
	*/
	void CloseProc() {
		// Process handle must exist to close process (self explainatory)
		if (mProcessHandle) {
			CloseHandle(mProcessHandle);
			mProcessHandle = nullptr;
			mProcessId = 0;
		}
		else 
			std::cout << "[WARN]: mProcessHandle = nullptr in " << __FUNCSIG__ << " - No process handle.\n";
	}

	/** 
	* Write <val> to process memory at location <addr>
	*/
	template<typename T>
	void WriteMemory(const std::uintptr_t& addr, const T& val) {
		// Process must exist to write to memory
		if (mProcessId != 0 && mProcessHandle)
			WriteProcessMemory(mProcessHandle, reinterpret_cast<void*>(addr), &val, sizeof(T), NULL);
		else 
			std::cout << "[ERROR]: No mProcessId or mProcessHandle" << __FUNCSIG__ << " - Cannot write to process!\n";
	}

	/** 
	* Read process memory at location <addr>
	*/
	template<typename T>
	T ReadMemory(const std::uintptr_t& addr) {
		T result = {};

		// Process must exist to read value from memory
		if (mProcessId != 0 && mProcessHandle)
			ReadProcessMemory(mProcessHandle, reinterpret_cast<void*>(addr), &result, sizeof(T), NULL);
		else
			std::cout << "[ERROR]: No mProcessId or mProcessHandle" << __FUNCSIG__ << " - Cannot read from process!\n";

		return result;
	}

	static std::vector<std::string> GetModuleNames(const DWORD pid) {
		std::vector<std::string> result;

		// Create snapshot that gets all process <pid> module names
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

		// No snapshot = no modules, so return error and get out this function
		if (snapshot == INVALID_HANDLE_VALUE) {
			std::cout << "[ERROR]: INVALID_HANDLE_VALUE in " << __FUNCSIG__ << " - No snapshot!\n";

			return result;
		}

		// Module entry, with module data
		MODULEENTRY32 entry = { };
		entry.dwSize = sizeof(MODULEENTRY32);

		// Iterate through modules to get names
		if (Module32FirstW(snapshot, &entry) == TRUE) {
			result.emplace_back(std::string(&entry.szModule[0], &entry.szModule[256]));

			while (Module32NextW(snapshot, &entry) == TRUE) {
				result.emplace_back(std::string(&entry.szModule[0], &entry.szModule[256]));
			}
		}

		// Close snapshot
		if (snapshot) CloseHandle(snapshot);

		// And return result
		return result;
	}

	static std::vector<std::string> GetProcessNames() {
		std::vector<std::string> result;

		// Create snapshot of all processes running on machine
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		// No snapshot = no modules, so return error and get out this function
		if (snapshot == INVALID_HANDLE_VALUE) {
			std::cout << "[ERROR]: INVALID_HANDLE_VALUE in " << __FUNCSIG__ << " - No snapshot!\n";

			return result;
		}

		// Process entry with process data
		PROCESSENTRY32 entry = {};
		entry.dwSize = sizeof(decltype(entry));

		// Iterate through processes to gte names
		if (Process32FirstW(snapshot, &entry) == TRUE) {
			result.emplace_back(std::string(&entry.szExeFile[0], &entry.szExeFile[260]));

			while (Process32NextW(snapshot, &entry) == TRUE) {
				result.emplace_back(std::string(&entry.szExeFile[0], &entry.szExeFile[260]));
			}
		}

		// Close snapshot
		if(snapshot) CloseHandle(snapshot);

		// And return what we found
		return result;
	}

	/**
	* Gets process module address <ret std::uintptr_t> from name <moduleName>
	*/
	const std::uintptr_t GetModuleAddress(const wchar_t* moduleName) const noexcept {
		std::uintptr_t module_base = 0;

		// Get all processes
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, mProcessId);

		// no snapshot = no module pointer
		if (snapshot == INVALID_HANDLE_VALUE) {
			std::cout << "[ERROR]: INVALID_HANDLE_VALUE in " << __FUNCSIG__ << " - No snapshot!\n";

			return module_base;
		}

		MODULEENTRY32 entry = { };
		entry.dwSize = sizeof(MODULEENTRY32);

		// Find module entry
		if (Module32FirstW(snapshot, &entry) == TRUE) {
			if (wcsstr(moduleName, entry.szModule) == nullptr) {
				module_base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
			}
			else {
				while (Module32NextW(snapshot, &entry) == TRUE) {
					if (wcsstr(moduleName, entry.szModule) == nullptr) {
						module_base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);

						break;
					}
				}
			}
		}

		// Close snapshot handle
		if (snapshot) CloseHandle(snapshot);

		// Return pointer
		return module_base;
	}

	/** 
	* Get process id <mProcessId> from process name <processName>
	*/
	void GetProcessId(const wchar_t* processName) {
		// Get all processes
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		// No process = no pid
		if (snapshot == INVALID_HANDLE_VALUE) {
			std::cout << "[ERROR]: INVALID_HANDLE_VALUE in " << __FUNCSIG__ << " - No snapshot!\n";

			return;
		}

		PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof(PROCESSENTRY32);

		// Find process entry
		if (Process32FirstW(snapshot, &entry) == TRUE) {
			if (_wcsicmp(processName, entry.szExeFile) == 0) {
				mProcessId = entry.th32ProcessID;
			}
			else {
				while (Process32NextW(snapshot, &entry) == TRUE) {
					if (_wcsicmp(processName, entry.szExeFile) == 0) {
						mProcessId = entry.th32ProcessID;

						break;
					}
				}
			}
		}

		// Close snapshot handle
		if (snapshot) CloseHandle(snapshot);
	}

	// Cause a lot of warnings in TTY
	/*~Process() {
		CloseProc();
	}*/
};

#endif