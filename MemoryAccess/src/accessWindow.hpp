#pragma once
#ifndef __ACCESS_WINDOW_
#define __ACCESS_WINDOW_

/**
* 
* TODO
* 
* More address edit input
* Address value RW looping
* Address value RW on key
* Loaded offsets
* 
* Sabe and load configs
* 
*/

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "processHandler.hpp"

// Global counter (quite useful)
int gGlobalCounter = 0;

// Simple access struct
struct AccessWindowData {
	Process mProcessPtr;
	std::string mSelectedProcess;
	std::string mSelectedModule;
	std::uintptr_t mModuleAddress = 0;

	std::uintptr_t mAddress = 0;
	std::uint64_t mValue = 0;
	bool mShowModules = false;
	bool mTabOpen = true;
};

// Advanced access flags
enum AdvancedAccessWindowDataFlags {
	AAWDF_readOnKey = 0x1,
	AAWDF_writeOnKey = 0x2,
	AAWDF_writeLoop = 0x4,
	AAWDF_readLoop = 0x8,
	AAWDF_readFromModuleAddress = 0x10,
	AAWDF_writeFromModuleAddress = 0x20,
	AAWDF_statusLooping = 0x40,
	AAWDF_loopOnKey = 0x80,
	AAWDF_limitLoopIndex = 0x100,
	AAWDF_dontUseValueAsReadBuffer = 0x200,
};

// Advanced access struct
struct AdvancedAccessWindowData {
	struct Data {
		std::string mDataName;
		std::uintptr_t mAddress;
		std::uint64_t mValue;
		std::uint64_t mReadValue;
		std::int32_t mKeybind;
		std::uint32_t mFlags;
		std::int32_t mValueType;

		std::uint32_t mLoopingKey;
		std::uint32_t mRWKey;
		std::string mLoopingKeyName;
		std::string mRWKeyName;

		std::uint32_t mLoopIndexLimit;
		std::uint32_t mLoopIndex;
	};
	
	std::vector<Data> mData;
};

// Reset global counter
void ResetGlobalCounter() {
	gGlobalCounter = 0;
}

// Convert string into int
constexpr unsigned int StringToInt(const char* str, int h = 0) {
	return !str[h] ? 5381 : (StringToInt(str, h + 1) * 33) ^ str[h];
}

// Main editor class
class EditProcessWindow {
private:
	// Our simple data
	std::vector<AccessWindowData> mAccessWindowDatas;
	// Our advanced data
	std::vector<AdvancedAccessWindowData> mAdvAccessData;
	// Current tab index
	std::size_t mCurrentTab = 0;

	// Open calculator popup
	bool mOpenCalculatorPopup = false;
	// Calculator data
	uint64_t mCalculatorData = 0;
	// List all modules for convinience
	bool mListModules = false;
	// List all processes for convinience
	bool mListProcesses = false;
	// include module address in wrire
	bool mWriteStartingFromModule = false;
	// Include module address in read
	bool mReadStartingFromModule = false;
	// Show all values in decimal
	bool mDecimalValues = false;
	// Write value type (U64, U32 etc.)
	int mCurrentWriteValueType = 0;
	bool mAdvancedView = false;

	void HelpTooltip(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void OpenCalculatorPopup() {
		if (mOpenCalculatorPopup) {
			ImGui::OpenPopup("Calculator");
		}

		if (ImGui::BeginPopupModal("Calculator", &mOpenCalculatorPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputScalar("Hex", ImGuiDataType_U64, &mCalculatorData, (void*)0, (void*)0, "%x", ImGuiInputTextFlags_CharsHexadecimal);
			ImGui::InputDouble("Flt", (double*)&mCalculatorData);
			ImGui::InputText("Text", (char*) & mCalculatorData, sizeof(uint64_t));

			if (ImGui::Button("Close")) {
				mOpenCalculatorPopup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void OpenProcess(AccessWindowData* awd) {
		std::vector<std::string> process_names = Process::GetProcessNames();

		// Input process name 
		bool open_proc = ImGui::InputText("Process name", &awd->mSelectedProcess, ImGuiInputTextFlags_EnterReturnsTrue);

		// Show all processes on user desire
		if (mListProcesses) {
			if (ImGui::BeginCombo("Processes list", awd->mSelectedProcess.c_str())) {
				for (int i = 0; i < process_names.size(); i++) {
					// We need custom ids for nameing collisions
					ImGui::PushID(&process_names[i]);

					// Select process (dont work)
					if (ImGui::Selectable(process_names[i].c_str())) {
						awd->mSelectedProcess = process_names[i];

						ImGui::PopID();

						break;
					}

					ImGui::PopID();
				}

				ImGui::EndCombo();
			}
		}

		ImGui::Separator();

		// Open process on user desire
		if (ImGui::Button("Open process") || open_proc) {
			// We need process id firstly to open process
			awd->mProcessPtr.GetProcessId(std::wstring(awd->mSelectedProcess.begin(), awd->mSelectedProcess.end()).c_str());
			awd->mProcessPtr.OpenProc();
		}

		// Close process on user desire
		if (awd->mProcessPtr() != 0) {
			ImGui::SameLine();

			if (ImGui::Button("Close process")) {
				awd->mProcessPtr.CloseProc();
			}
		}

		// Show process id based on user preferences
		if (mDecimalValues) {
			ImGui::Text("Process ID: %llu", awd->mProcessPtr());
		}
		else {
			ImGui::Text("Process ID: %llx", awd->mProcessPtr());
		}
	}

	void ShowModules(AccessWindowData* awd) {
		// Show selecting module on user desire
		ImGui::Checkbox("Show modules", &awd->mShowModules);

		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			if (awd->mShowModules) {
				// Get all process modules
				std::vector<std::string> module_names = Process::GetModuleNames(awd->mProcessPtr());

				// Input module name
				bool open_module = ImGui::InputText("Module name", &awd->mSelectedModule, ImGuiInputTextFlags_EnterReturnsTrue);

				// List all modules on user demand
				if (mListModules) {
					if (ImGui::BeginCombo("Modules list", awd->mSelectedModule.c_str())) {
						for (int i = 0; i < module_names.size(); i++) {
							// This needs custom id
							ImGui::PushID(&module_names[i]);
							if (ImGui::Selectable(module_names[i].c_str())) {
								awd->mSelectedModule = module_names[i];

								ImGui::PopID();

								break;
							}

							ImGui::PopID();
						}

						ImGui::EndCombo();
					}
				}

				ImGui::Separator();

				// Get desired module address
				if (ImGui::Button("Get module address") || open_module) {
					awd->mModuleAddress = awd->mProcessPtr.GetModuleAddress(std::wstring(awd->mSelectedModule.begin(), awd->mSelectedModule.end()).c_str());
				}

				ImGui::SameLine();

				// Show module address based on user preferences
				if (mDecimalValues) {
					ImGui::Text("Module @ %llu", awd->mModuleAddress);
				}
				else {
					ImGui::Text("Module @ %llx", awd->mModuleAddress);
				}
			}
		}
	}

	void ShowWriteMemory(AccessWindowData* awd) {
		// If we dont have process dont show this
		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			ImGui::SeparatorText("Write memory");

			// Include module address to write
			ImGui::Checkbox("Write staring from module address", &mWriteStartingFromModule);

			// Address of value to write to process memory
			ImGui::InputScalar("Address##01", ImGuiDataType_U64, (uint64_t*)&awd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

			// Unsigned : 0, 1, 2, 3; Signed : 4, 5, 6, 7; Floating point: 8, 9 
			const char* basic_value_types[] = { "Unsigned 64", "Unsigned 32", "Unsigned 16", "Unsigned 8", "Signed 64", "Signed 32", "Signed 16", "Signed 8", "Float 64", "Float 32" };

			// Written value type 
			ImGui::Combo("Value type", &mCurrentWriteValueType, basic_value_types, IM_ARRAYSIZE(basic_value_types));

			switch (mCurrentWriteValueType) {
			case 0: // U64
				// Written value
				ImGui::InputScalar("Value", ImGuiDataType_U64, (uint64_t*) & awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				// Write to process memory address on user demand
				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint64_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 1: // U32
				ImGui::InputScalar("Value", ImGuiDataType_U32, (uint32_t*) & awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint32_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 2: // U16
				ImGui::InputScalar("Value", ImGuiDataType_U16, (uint16_t*) & awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint16_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 3: // U8
				ImGui::InputScalar("Value", ImGuiDataType_U8, (uint8_t*) & awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint8_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 4: // S64
				ImGui::InputScalar("Value", ImGuiDataType_S64, (int64_t*) &awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int64_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 5: // S32
				ImGui::InputScalar("Value", ImGuiDataType_S32, (int32_t*) &awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int32_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 6: // S16
				ImGui::InputScalar("Value", ImGuiDataType_S16, (int16_t*) &awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int16_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 7: // S8
				ImGui::InputScalar("Value", ImGuiDataType_S8, (int8_t*) & awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int8_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 8: // F64
				ImGui::InputScalar("Value", ImGuiDataType_Double, (double*) & awd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal*/);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<double>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			case 9: // F32
				ImGui::InputScalar("Value", ImGuiDataType_Float, (float*) & awd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal */);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<float>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;

			default: // U64
				ImGui::InputScalar("Value", ImGuiDataType_U64, &awd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint64_t>((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
				}

				break;
			}

			ImGui::SameLine();

			// Show address based on user preferences
			if (mDecimalValues) {
				ImGui::Text("Wrote @ %llu", (mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);
			}
			else {
				ImGui::Text("Wrote @ %llx", (mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);
			}
		}
	}

	void ShowReadMemory(AccessWindowData* awd) {
		// If we dont have process dont show this
		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			ImGui::SeparatorText("Read memory");

			// Include module offset
			ImGui::Checkbox("Read starting from module address", &mReadStartingFromModule);

			// Set address in memory ro read from
			ImGui::InputScalar("Address##02", ImGuiDataType_U64, (uint64_t*)&awd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

			// Read memory from process on user demand
			if (ImGui::Button("Read")) {
				awd->mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>((mReadStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);
			}
			
			ImGui::SameLine();

			// Show value and address based on user preferences
			if (mDecimalValues) {
				ImGui::Text("Read @ %llu", (mReadStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);

				ImGui::Text("Value: %llu", awd->mValue);
			}
			else {
				ImGui::Text("Read @ %llx", (mReadStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);

				ImGui::Text("Value: %llx", awd->mValue);
			}
		}
	}

	void AdvancedShowWriteMemory(AccessWindowData* awd, AdvancedAccessWindowData::Data* aawd) {
		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			ImGui::SeparatorText("Write memory");

			ImGui::InputScalar("Address##01", ImGuiDataType_U64, (uint64_t*)&aawd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

			// Unsigned : 0, 1, 2, 3; Signed : 4, 5, 6, 7; Floating point: 8, 9 
			const char* basic_value_types[] = { "Unsigned 64", "Unsigned 32", "Unsigned 16", "Unsigned 8", "Signed 64", "Signed 32", "Signed 16", "Signed 8", "Float 64", "Float 32" };

			// Set value type
			ImGui::Combo("Value type", &aawd->mValueType, basic_value_types, IM_ARRAYSIZE(basic_value_types));

			switch (aawd->mValueType) {
			case 0: // U64
				// And set value (im not wtiring 10 same comments)
				ImGui::InputScalar("Value", ImGuiDataType_U64, (uint64_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				// Write on user demand
				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint64_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 1: // U32
				ImGui::InputScalar("Value", ImGuiDataType_U32, (uint32_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint32_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 2: // U16
				ImGui::InputScalar("Value", ImGuiDataType_U16, (uint16_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint16_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 3: // U8
				ImGui::InputScalar("Value", ImGuiDataType_U8, (uint8_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint8_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 4: // S64
				ImGui::InputScalar("Value", ImGuiDataType_S64, (int64_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int64_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 5: // S32
				ImGui::InputScalar("Value", ImGuiDataType_S32, (int32_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int32_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 6: // S16
				ImGui::InputScalar("Value", ImGuiDataType_S16, (int16_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int16_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 7: // S8
				ImGui::InputScalar("Value", ImGuiDataType_S8, (int8_t*)&aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<int8_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 8: // F64
				ImGui::InputScalar("Value", ImGuiDataType_Double, (double*)&aawd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal*/);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<double>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			case 9: // F32
				ImGui::InputScalar("Value", ImGuiDataType_Float, (float*)&aawd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal */);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<float>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;

			default: // U64
				ImGui::InputScalar("Value", ImGuiDataType_U64, &aawd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

				if (ImGui::Button("Write")) {
					awd->mProcessPtr.WriteMemory<uint64_t>(((aawd->mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress, aawd->mValue);
				}

				break;
			}

			ImGui::SameLine();

			// Show address based on user preferences
			if (mDecimalValues) {
				ImGui::Text("Wrote @ %llu", ((aawd->mFlags& AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress);
			}
			else {
				ImGui::Text("Wrote @ %llx", ((aawd->mFlags& AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress);
			}
		}
	}

	void AdvancedShowReadMemory(AccessWindowData* awd, AdvancedAccessWindowData::Data* aawd) {
		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			ImGui::SeparatorText("Read memory");

			ImGui::InputScalar("Address##02", ImGuiDataType_U64, (uint64_t*)&aawd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%x"), ImGuiInputTextFlags_CharsHexadecimal);

			// Read from address on user demand
			if (ImGui::Button("Read")) {
				if(aawd->mFlags & AAWDF_dontUseValueAsReadBuffer) aawd->mReadValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress);
				else aawd->mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress);
			}

			ImGui::SameLine();

			// Show value and address based on user preferences
			if (mDecimalValues) {
				ImGui::Text("Read @ %llu", ((aawd->mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress);

				// Do we want to just ead value or also write it
				if (aawd->mFlags & AAWDF_dontUseValueAsReadBuffer) ImGui::Text("Value: %llu", aawd->mReadValue);
				else ImGui::Text("Value: %llu", aawd->mValue);
			}
			else {
				ImGui::Text("Read @ %llx", ((aawd->mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mAddress);

				// Do we want to just ead value or also write it
				if (aawd->mFlags & AAWDF_dontUseValueAsReadBuffer) ImGui::Text("Value: %llx", aawd->mReadValue);
				else ImGui::Text("Value: %llx", aawd->mValue);
			}
		}
	}

	std::int32_t KeynameToVkValue(const char* keyname) {
		switch (StringToInt(keyname)) {
		case StringToInt("back"):
			return VK_BACK; 
			
			break;
		case StringToInt("tab"):
			return VK_TAB; 
			
			break;
		case StringToInt("clear"):
			return VK_CLEAR; 
			
			break;
		case StringToInt("return"):
			return VK_RETURN; 
			
			break;
		case StringToInt("shift"):
			return VK_SHIFT; 
			
			break;
		case StringToInt("control"):
			return VK_CONTROL; 
			
			break;
		case StringToInt("pause"):
			return VK_PAUSE; 
			
			break;
		case StringToInt("capital"):
			return VK_CAPITAL; 
			
			break;
		case StringToInt("escape"):
			return VK_ESCAPE; 
			
			break;
		case StringToInt("space"):
			return VK_SPACE; 
			
			break;
		case StringToInt("end"):
			return VK_END; 
			
			break;
		case StringToInt("home"):
			return VK_HOME; 
			
			break;
		case StringToInt("left"):
			return VK_LEFT; 
			
			break;
		case StringToInt("up"):
			return VK_UP; 
			
			break;
		case StringToInt("right"):
			return VK_RIGHT; 
			
			break;
		case StringToInt("down"):
			return VK_DOWN; 
			
			break;
		case StringToInt("select"):
			return VK_SELECT; 
			
			break;
		case StringToInt("print"):
			return VK_PRINT; 
			
			break;
		case StringToInt("insert"):
			return VK_INSERT; 
			
			break;
		case StringToInt("delete"):
			return VK_DELETE; 
			
			break;
		case StringToInt("lwin"):
			return VK_LWIN; 
			
			break;
		case StringToInt("rwin"):
			return VK_RWIN; 
			
			break;
		case StringToInt("sleep"):
			return VK_SLEEP; 
			
			break;
		case StringToInt("num0"):
			return VK_NUMPAD0; 
			
			break;
		case StringToInt("num1"):
			return VK_NUMPAD1; 
			
			break;
		case StringToInt("num2"):
			return VK_NUMPAD2; 
			
			break;
		case StringToInt("num3"):
			return VK_NUMPAD3; 
			
			break;
		case StringToInt("num4"):
			return VK_NUMPAD4; 
			
			break;
		case StringToInt("num5"):
			return VK_NUMPAD5; 
			
			break;
		case StringToInt("num6"):
			return VK_NUMPAD6; 
			
			break;
		case StringToInt("num7"):
			return VK_NUMPAD7; 
			
			break;
		case StringToInt("num8"):
			return VK_NUMPAD8; 
			
			break;
		case StringToInt("num9"):
			return VK_NUMPAD9; 
			
			break;
		case StringToInt("multiply"):
			return VK_MULTIPLY; 
			
			break;
		case StringToInt("add"):
			return VK_ADD; 
			
			break;
		case StringToInt("separator"):
			return VK_SEPARATOR; 
			
			break;
		case StringToInt("subtract"):
			return VK_SUBTRACT; 
			
			break;
		case StringToInt("decimal"):
			return VK_DECIMAL; 
			
			break;
		case StringToInt("divide"):
			return VK_DIVIDE; 
			
			break;
		case StringToInt("f1"):
			return VK_F1; 
			
			break;
		case StringToInt("f2"):
			return VK_F2; 
			
			break;
		case StringToInt("f3"):
			return VK_F3; 
			
			break;
		case StringToInt("f4"):
			return VK_F4; 
			
			break;
		case StringToInt("f5"):
			return VK_F5; 
			
			break;
		case StringToInt("f6"):
			return VK_F6; 
			
			break;
		case StringToInt("f7"):
			return VK_F7; 
			
			break;
		case StringToInt("f8"):
			return VK_F8; 
			
			break;
		case StringToInt("f9"):
			return VK_F9; 
			
			break;
		case StringToInt("f10"):
			return VK_F10; 
			
			break;
		case StringToInt("f11"):
			return VK_F11; 
			
			break;
		case StringToInt("f12"):
			return VK_F12; 
			
			break;
		case StringToInt("f13"):
			return VK_F13; 
			
			break;
		case StringToInt("f14"):
			return VK_F14; 
			
			break;
		case StringToInt("f15"):
			return VK_F15; 
			
			break;
		case StringToInt("f16"):
			return VK_F16; 
			
			break;
		case StringToInt("f17"):
			return VK_F17; 
			
			break;
		case StringToInt("f18"):
			return VK_F18; 
			
			break;
		case StringToInt("f19"):
			return VK_F19; 
			
			break;
		case StringToInt("f20"):
			return VK_F20; 
			
			break;
		case StringToInt("f21"):
			return VK_F21; 
			
			break;
		case StringToInt("f22"):
			return VK_F22; 
			
			break;
		case StringToInt("f23"):
			return VK_F23; 
			
			break;
		case StringToInt("f24"):
			return VK_F24; 
			
			break;
		case StringToInt("numlock"):
			return VK_NUMLOCK; 
			
			break;
		case StringToInt("scroll"):
			return VK_SCROLL; 
			
			break;
		case StringToInt("lshift"):
			return VK_LSHIFT; 
			
			break;
		case StringToInt("rshift"):
			return VK_RSHIFT; 
			
			break;
		case StringToInt("lcontrol"):
			return VK_LCONTROL; 
			
			break;
		case StringToInt("rcontrol"):
			return VK_RCONTROL; 
			
			break;
		case StringToInt("lmenu"):
			return VK_LMENU; 
			
			break;
		case StringToInt("rmenu"):
			return VK_RMENU; 
			
			break;

		default:
			// If none of that bullshit above, just return normal key
			if (std::string(keyname).size() == 1) {
				return keyname[0];
			}

			break;
		}

		return 0;
	}

	void AdvancedView(AccessWindowData* awd, int currentAwd) {
		if (ImGui::CollapsingHeader("Advanced functions")) {
			// Access AdvancedAccessWindowData by our current AccessWindowData
			AdvancedAccessWindowData* aawd = &mAdvAccessData[currentAwd];

			// Add data to R/W
			if (ImGui::Button("Add")) {
				aawd->mData.emplace_back(AdvancedAccessWindowData::Data());
				aawd->mData[aawd->mData.size() - 1].mDataName = std::string("Data ") + std::to_string(gGlobalCounter++);
			}

			std::int64_t delete_index = -1;

			for (std::size_t i = 0; i < aawd->mData.size(); i++) {
				// We need custom ids for no collision
				ImGui::PushID(&aawd->mData[i]);

				if (ImGui::TreeNode(aawd->mData[i].mDataName.c_str())) {
					// Delete data
					if (ImGui::Button("Delete")) {
						delete_index = (std::int64_t)i;
					}

					ImGui::InputText("Data name", &aawd->mData[i].mDataName);

					// Flags should be self explainatory
					ImGui::CheckboxFlags("Read on key press", &aawd->mData[i].mFlags, AAWDF_readOnKey);
					ImGui::CheckboxFlags("Write on key press", &aawd->mData[i].mFlags, AAWDF_writeOnKey);
					ImGui::CheckboxFlags("Looping read", &aawd->mData[i].mFlags, AAWDF_readLoop);
					ImGui::CheckboxFlags("Looping write", &aawd->mData[i].mFlags, AAWDF_writeLoop);
					ImGui::CheckboxFlags("Read starting from module address", &aawd->mData[i].mFlags, AAWDF_readFromModuleAddress);
					ImGui::CheckboxFlags("Write starting from module address", &aawd->mData[i].mFlags, AAWDF_writeFromModuleAddress);
					ImGui::CheckboxFlags("Start loop on key", &aawd->mData[i].mFlags, AAWDF_loopOnKey);
					ImGui::CheckboxFlags("Limit loop index", &aawd->mData[i].mFlags, AAWDF_limitLoopIndex);
					ImGui::CheckboxFlags("Read returns to separate buffer", &aawd->mData[i].mFlags, AAWDF_dontUseValueAsReadBuffer);
					// Start looping only if process exist, address exist, write or read flag is on
					if((aawd->mData[i].mFlags & AAWDF_readLoop || aawd->mData[i].mFlags & AAWDF_writeLoop) && awd->mProcessPtr() != 0 && aawd->mData[i].mAddress != 0)
						ImGui::CheckboxFlags("Start looping", &aawd->mData[i].mFlags, AAWDF_statusLooping);

					ImGui::InputText("Read/write key", &aawd->mData[i].mRWKeyName);
					HelpTooltip("Make sure to use capital letters when defining regular key (special keys like \"end\" are typed as small)!\nPossible special key names: \nback, tab, clear, return, shift, control, pause, capital, escape, space, end, home, left, up, right, down, select, print, insert, delete, lwin, rwin, sleep, num0, num1, num2, num3, num4, num5, num6, num7, num8, num9, multiply, add, separator, subtract, decimal, divide, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, numlock, scroll, lshift, rshift, lcontrol, rcontrol, lmenu, rmenu");
					ImGui::InputText("Start loop key", &aawd->mData[i].mLoopingKeyName);
					ImGui::InputInt("Loop index limit", (int*) & aawd->mData[i].mLoopIndexLimit); 

					// Reset loop index
					if (ImGui::Button("Reset loop index")) aawd->mData[i].mLoopIndex = 0;

					// We need to change key names to key values that GetAsyncKeyState understand
					aawd->mData[i].mRWKey = KeynameToVkValue(aawd->mData[i].mRWKeyName.c_str());
					aawd->mData[i].mLoopingKey = KeynameToVkValue(aawd->mData[i].mLoopingKeyName.c_str());

					// If key specified by user is pressed...
					if (aawd->mData[i].mRWKey != 0 && GetAsyncKeyState(aawd->mData[i].mRWKey)) {
						// ... check if we want to read, process and address to read from exist...
						if (awd->mProcessPtr() != 0 && aawd->mData[i].mAddress != 0 && aawd->mData[i].mFlags & AAWDF_readOnKey) {
							// ... then read from specified address
							//aawd->mData[i].mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);

							if (aawd->mData[i].mFlags & AAWDF_dontUseValueAsReadBuffer) aawd->mData[i].mReadValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);
							else aawd->mData[i].mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);
						}
						
						// .. check if we want to write, process and address to write to exist..
						if ( awd->mProcessPtr() != 0 && aawd->mData[i].mAddress != 0 && aawd->mData[i].mFlags & AAWDF_writeOnKey) {
							// ... then write specified value to specified earlier address
							awd->mProcessPtr.WriteMemory(((aawd->mData[i].mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress, aawd->mData[i].mValue);
						}
					}

					// If looping key is specified, process and address to read form exist and flag to start looping is set...
					if (awd->mProcessPtr() != 0 && aawd->mData[i].mAddress != 0 && aawd->mData[i].mFlags & AAWDF_readLoop && aawd->mData[i].mFlags & AAWDF_statusLooping) {
						// ... then check looping index limit, if there is none loop infinitly
						if (aawd->mData[i].mLoopIndexLimit == 0) {
							//aawd->mData[i].mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);
							if (aawd->mData[i].mFlags & AAWDF_dontUseValueAsReadBuffer) aawd->mData[i].mReadValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);
							else aawd->mData[i].mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);
						}
						else {
							// If there is some limit to loop then use it
							if (aawd->mData[i].mLoopIndex < aawd->mData[i].mLoopIndexLimit) {
								//aawd->mData[i].mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);
								if (aawd->mData[i].mFlags & AAWDF_dontUseValueAsReadBuffer) aawd->mData[i].mReadValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);
								else aawd->mData[i].mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(((aawd->mData[i].mFlags & AAWDF_readFromModuleAddress) == AAWDF_readFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress);

								aawd->mData[i].mLoopIndex++;
							}
						}
					}

					// If looping key is specified, process and address to write to exist and flag to start looping is set...
					if (awd->mProcessPtr() != 0 && aawd->mData[i].mAddress != 0 && aawd->mData[i].mFlags & AAWDF_writeLoop && aawd->mData[i].mFlags & AAWDF_statusLooping) {
						// ... then check looping index limit, if there is none loop infinitly
						if (aawd->mData[i].mLoopIndexLimit == 0) {
							awd->mProcessPtr.WriteMemory(((aawd->mData[i].mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress, aawd->mData[i].mValue);
						}
						else {
							// If there is some limit to loop then use it
							if (aawd->mData[i].mLoopIndex < aawd->mData[i].mLoopIndexLimit) {
								awd->mProcessPtr.WriteMemory(((aawd->mData[i].mFlags & AAWDF_writeFromModuleAddress) == AAWDF_writeFromModuleAddress ? awd->mModuleAddress : 0) + aawd->mData[i].mAddress, aawd->mData[i].mValue);

								aawd->mData[i].mLoopIndex++;
							}
						}
					}

					// Change looping status on key press earlier specyfied by user 
					if (GetAsyncKeyState(aawd->mData[i].mLoopingKey)) {
						aawd->mData[i].mFlags ^= AAWDF_statusLooping;
					}

					// Show read and write
					AdvancedShowReadMemory(awd, &aawd->mData[i]);
					AdvancedShowWriteMemory(awd, &aawd->mData[i]);

					ImGui::TreePop();
				}

				ImGui::PopID();
			}

			// If we want to delete AAWD, we do it by index and set index back to -1 (accualy not needed)
			if (delete_index != -1) {
				aawd->mData.erase(aawd->mData.begin() + delete_index);
				delete_index = -1;
			}
		}
	}

	void ShowMenuBar() {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Process")) {
				// Open new process
				if (ImGui::MenuItem("Interrupt new process")) {
					mAccessWindowDatas.emplace_back(AccessWindowData());
					mAdvAccessData.emplace_back(AdvancedAccessWindowData());
				}

				// Close current process, 'X' also works
				if (ImGui::MenuItem("Close current tab")) {
					mAccessWindowDatas[mCurrentTab].mProcessPtr.CloseProc();
					mAccessWindowDatas.erase(mAccessWindowDatas.begin() + mCurrentTab);

					mAdvAccessData.erase(mAdvAccessData.begin() + mCurrentTab);
				}

				// List processes and modules in combo box
				ImGui::MenuItem("List processes", "", &mListProcesses);
				ImGui::MenuItem("List modules", "", &mListModules);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {
				// If someone hate hex values
				ImGui::MenuItem("Decimal values", "", &mDecimalValues);
				// For advanced users
				ImGui::MenuItem("Advanced", "", &mAdvancedView);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				// Just a calculator
				// TODO: Do it better
				if (ImGui::MenuItem("Calculator")) {
					mOpenCalculatorPopup = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Separator();
	}

	void EditProcess(AccessWindowData* awd) {
		// Some headers and functions
		if (ImGui::CollapsingHeader("Process")) OpenProcess(awd);

		if (ImGui::CollapsingHeader("Modules")) ShowModules(awd);

		if (ImGui::CollapsingHeader("Edit memory")) {
			ShowWriteMemory(awd);

			ShowReadMemory(awd);
		}

		// Show advanced view
		if (mAdvancedView) {
			AdvancedView(awd, mCurrentTab);
		}
	}

	void ShowProcessTabs() {
		// If tab is closed by 'X' delete it
		for (std::size_t i = 0; i < mAccessWindowDatas.size(); i++) {
			if (!mAccessWindowDatas[i].mTabOpen) {
				mAccessWindowDatas[i].mProcessPtr.CloseProc();
				mAccessWindowDatas.erase(mAccessWindowDatas.begin() + i);

				mAdvAccessData.erase(mAdvAccessData.begin() + i);
			}
		}

		// Open all process tabs
		if (ImGui::BeginTabBar("##process_tabs")) {
			for (std::size_t i = 0; i < mAccessWindowDatas.size(); i++) {
				// We need custom id
				ImGui::PushID(&mAccessWindowDatas[i]);
				// If it is visible...
				bool visible = ImGui::BeginTabItem(
					mAccessWindowDatas[i].mSelectedProcess.empty() == true || mAccessWindowDatas[i].mProcessPtr() == 0 ? 
					(std::string("Empty process") + std::to_string(gGlobalCounter++)).c_str() : mAccessWindowDatas[i].mSelectedProcess.c_str(), 
					&mAccessWindowDatas[i].mTabOpen);
			
				// ... then show it
				if (visible) {
					mCurrentTab = i;
					EditProcess(&mAccessWindowDatas[i]);

					ImGui::EndTabItem();
				}

				ImGui::PopID();
			}

			ImGui::EndTabBar();
		}
	}

public:
	void Show() {
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		bool smaw_visible = ImGui::Begin((std::string("Process ") + std::to_string(gGlobalCounter++)).c_str(), nullptr, 
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		if (!smaw_visible) {
			ImGui::End();

			return;
		}

		ShowMenuBar();

		ShowProcessTabs();

		OpenCalculatorPopup();

		ImGui::End();
	}
};

#endif