/**
*
* @author Piotr UjemnyGH Plombon
*
* Entire process window class, method definitions
*
*/

#include "accessWindow.hpp"
#include "memoryConfigFileFormat.hpp"

void EditProcessWindow::HelpTooltip(const char* desc)
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

void EditProcessWindow::OpenCalculatorPopup() {
	if (mOpenCalculatorPopup) {
		ImGui::OpenPopup("Calculator");
	}

	if (ImGui::BeginPopupModal("Calculator", &mOpenCalculatorPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::InputScalar("Hex", ImGuiDataType_U64, &mCalculatorData, (void*)0, (void*)0, "%lx", ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::InputDouble("Flt", (double*)&mCalculatorData);
		ImGui::InputText("Text", (char*)&mCalculatorData, sizeof(uint64_t));

		if (ImGui::Button("Close")) {
			mOpenCalculatorPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

// Just a software info
void EditProcessWindow::OpenSoftwareInfoPopup() {
	if (mOpenInfoPopup) {
		ImGui::OpenPopup("Software info");
	}

	if (ImGui::BeginPopupModal("Software info", &mOpenInfoPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Version: %s", SoftwareVerisonToString().c_str());
		ImGui::TextUnformatted("Software created by Piotr \"UjemnyGH\" Plombon");
		ImGui::TextUnformatted("Software can be found on https://github.com/UjemnyGH/MemoryAccess");
		ImGui::TextUnformatted("Thanks for downloading and have fun breaking stuff!");


		if (ImGui::Button("Close")) {
			mOpenInfoPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void EditProcessWindow::OpenGetValuesBetweenAddressesPopup(AccessProcessData* apd) {
	if (mOpenGetValuesBetweenAddressesPopup) {
		ImGui::OpenPopup("Get values between addresses");
	}

	if (ImGui::BeginPopupModal("Get values between addresses", &mOpenGetValuesBetweenAddressesPopup)) {
		ImGui::Text("Get values between 2 addresses on\nprocess in current tab");

		bool editing_start = ImGui::InputScalar("Start address", ImGuiDataType_U64, &mFetchedAddressLow, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);
		bool editing_end = ImGui::InputScalar("End address", ImGuiDataType_U64, &mFetchedAddressHigh, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::Checkbox("Add module address", &mBeginFromModuleAddress);
		
		if (editing_start || editing_end) mRunFetchingReadStatus = false;

		ImGui::Checkbox("Run", &mRunFetchingReadStatus);

		std::uintptr_t startAddress = (mBeginFromModuleAddress ? apd->mModuleAddress : 0) + mFetchedAddressLow;
		std::uintptr_t endAddress = (mBeginFromModuleAddress ? apd->mModuleAddress : 0) + mFetchedAddressHigh;

		ImGui::Text("@%p - @%p", startAddress, endAddress);

		if (startAddress > endAddress) {
			mFetchedAddressHigh = mFetchedAddressLow;
		}

		if (ImGui::Button("Close")) {
			mOpenGetValuesBetweenAddressesPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::Text("Data shown in 8 bytes");

		if (mRunFetchingReadStatus && apd->mProcessPtr() != 0) {
			for (int i = 0; i < (endAddress - startAddress) / 8; i++) {
				std::uint64_t value = apd->mProcessPtr.ReadMemory<std::uint64_t>(startAddress + (i * 8));

				ImGui::Text((mDecimalValues ? "%lld  @  %p" : "%llx  @  %p"), value, startAddress + (i * 8));
			}
		}

		ImGui::EndPopup();
	}
}

void EditProcessWindow::OpenSaveConfigDialogBox(GLFWwindow* wnd) {
	mConfigFilename.clear();
	mConfigFilename.resize(0x1000);
	mDialogBoxData.lStructSize = sizeof(mDialogBoxData);
	mDialogBoxData.lpstrFile = mConfigFilename.data();
	mDialogBoxData.nMaxFile = 0x1000;
	mDialogBoxData.lpstrFilter = ".macfg\0";
	mDialogBoxData.nFilterIndex = 1;
	mDialogBoxData.Flags = OFN_OVERWRITEPROMPT;
	mDialogBoxData.hwndOwner = glfwGetWin32Window(wnd);

	GetSaveFileNameA(&mDialogBoxData);

	mConfigFilename.shrink_to_fit();

	while (mConfigFilename.find('\0') != std::string::npos) {
		mConfigFilename.erase(mConfigFilename.begin() + mConfigFilename.find('\0'));
	}
}

void EditProcessWindow::OpenLoadConfigDialogBox(GLFWwindow* wnd) {
	mConfigFilename.clear();
	mConfigFilename.resize(0x1000);
	mDialogBoxData.lStructSize = sizeof(mDialogBoxData);
	mDialogBoxData.lpstrFile = mConfigFilename.data();
	mDialogBoxData.nMaxFile = 0x1000;
	mDialogBoxData.lpstrFilter = ".macfg\0";
	mDialogBoxData.nFilterIndex = 1;
	mDialogBoxData.hwndOwner = glfwGetWin32Window(wnd);

	GetOpenFileNameA(&mDialogBoxData);

	if (mConfigFilename.find(".macfg") != std::string::npos) {
		mConfigFilename.erase(mConfigFilename.begin() + mConfigFilename.find(".macfg"), mConfigFilename.end());
	}

	while (mConfigFilename.find('\0') != std::string::npos) {
		mConfigFilename.erase(mConfigFilename.begin() + mConfigFilename.find('\0'));
	}
}

void EditProcessWindow::OpenSaveConfigPopup() {
	if (mOpenSaveConfigPopup) {
		ImGui::OpenPopup("Save current tab in config");
	}

	if (ImGui::BeginPopupModal("Save current tab in config", &mOpenSaveConfigPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
		bool enter_file_path = ImGui::InputText("File path", &mConfigFilename, ImGuiInputTextFlags_EnterReturnsTrue);

		if (mAccessProcessDatas.size() > 0 && (ImGui::Button("Save") || enter_file_path)) {
			MemoryConfigFile file;

			// We need pointers to data to save, so we sabe current tab data
			AccessProcessData* apd = &mAccessProcessDatas[mCurrentTab];
			AdvancedAccessProcessData* aapd = &mAdvAccessData[mCurrentTab];

			// Add simple data first (adding order matters as we need to load it in the same order)
			file.AddString(apd->mSelectedProcess);
			file.AddString(apd->mSelectedModule);
			file.AddU64(apd->mAddress);
			file.AddU64(apd->mValue);
			file.AddU8((apd->mShowModules == true ? 1 : 0));

			// Add advanced data next
			for (std::size_t i = 0; i < aapd->mData.size(); i++) {
				file.AddString(aapd->mData[i].mDataName);
				file.AddU64(aapd->mData[i].mAddress);
				file.AddU64(aapd->mData[i].mValue);
				file.AddU64(aapd->mData[i].mReadValue);
				file.AddU32(aapd->mData[i].mFlags);
				file.AddU32(aapd->mData[i].mValueType);
				file.AddString(aapd->mData[i].mLoopingKeyName);
				file.AddString(aapd->mData[i].mRWKeyName);
				file.AddU32(aapd->mData[i].mLoopIndexLimit);
			}

			// Save to demanded location
			file.SaveConfigToFile(mConfigFilename);

			// And exit
			mOpenSaveConfigPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Close")) {
			mOpenSaveConfigPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


void EditProcessWindow::OpenLoadConfigPopup() {
	if (mOpenLoadConfigPopup) {
		ImGui::OpenPopup("Load config into current tab");
	}

	if (ImGui::BeginPopupModal("Load config into current tab", &mOpenLoadConfigPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
		bool enter_file_path = ImGui::InputText("File path", &mConfigFilename, ImGuiInputTextFlags_EnterReturnsTrue);
		
		// Flags for opening process and module on load
		ImGui::Checkbox("Open process after loading", &mOpenProcessOnLoad);
		ImGui::Checkbox("Open module after loading", &mOpenModuleOnLoad);

		// If we dont have opened any process tab dont show "Load" button
		if (mAccessProcessDatas.size() > 0 && (ImGui::Button("Load") || enter_file_path)) {
			MemoryConfigFile file;

			// Current tab process data
			AccessProcessData* apd = &mAccessProcessDatas[mCurrentTab];
			AdvancedAccessProcessData* aapd = &mAdvAccessData[mCurrentTab];
			// Clear advanced data for no errors
			aapd->mData.clear();

			// If file loaded correctly write all data to memory
			if (file.LoadConfigFromFile(mConfigFilename)) {
				// Order matters, so firstly load simple data struct
				apd->mSelectedProcess = file[0].mStr;
				apd->mSelectedModule = file[1].mStr;
				apd->mAddress = file[2].mU64;
				apd->mValue = file[3].mU64;
				apd->mShowModules = (file[4].mU8 == 1 ? true : false);

				// Then load all advanced data structs
				for (std::size_t i = 0; i < (file()->size() - 5) / 9; i++) {
					AdvancedAccessProcessData::Data data;

					data.mDataName = file[5 + (i * 9 + 0)].mStr;
					data.mAddress = file[5 + (i * 9 + 1)].mU64;
					data.mValue = file[5 + (i * 9 + 2)].mU64;
					data.mReadValue = file[5 + (i * 9 + 3)].mU64;
					data.mFlags = file[5 + (i * 9 + 4)].mU32;
					data.mValueType = file[5 + (i * 9 + 5)].mU32;
					data.mLoopingKeyName = file[5 + (i * 9 + 6)].mStr;
					data.mRWKeyName = file[5 + (i * 9 + 7)].mStr;
					data.mLoopIndexLimit = file[5 + (i * 9 + 8)].mU32;

					aapd->mData.emplace_back(data);
				}

				// If open process on load checkbox was set, open process
				if (mOpenProcessOnLoad) {
					apd->mProcessPtr.GetProcessId(std::wstring(apd->mSelectedProcess.begin(), apd->mSelectedProcess.end()).c_str());
					apd->mProcessPtr.OpenProc();
				}

				// If open module on load checkbox was set, open module
				if(mOpenModuleOnLoad) OpenSelectedModule(apd);

				// And close popup
				mOpenLoadConfigPopup = false;
				ImGui::CloseCurrentPopup();
			}
		}

		// We dont want SameLine when we dont have "Load" button
		if(mAccessProcessDatas.size() > 0) ImGui::SameLine();

		if (ImGui::Button("Close")) {
			mOpenLoadConfigPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void EditProcessWindow::OpenProcess(AccessProcessData* apd) {
	std::vector<std::string> process_names = Process::GetProcessNames();
	ImGui::TextUnformatted("This opens process handle");

	// Input process name 
	bool open_proc = ImGui::InputText("Process name", &apd->mSelectedProcess, ImGuiInputTextFlags_EnterReturnsTrue);
	// Search process name
	ImGui::InputText("Search process", &apd->mSearchProcess);

	// All found process names separated by new line
	std::string found_valid_process_names_from_search;

	// Run through all process names
	for (std::string & name : process_names) {
		// Check if process name contain desired string
		if (name.find(apd->mSearchProcess) != std::string::npos) {
			// If yes add name to all found valid process
			found_valid_process_names_from_search += std::string(name.begin(), name.begin() + name.find_first_of('\0')) + '\n';
		}
	}

	// Then display it if found valid processes and search box arent empty, user still needs to type process name
	if (!found_valid_process_names_from_search.empty() && !apd->mSearchProcess.empty()) {
		ImGui::InputTextMultiline("Found processes", &found_valid_process_names_from_search, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
	}

	// Show all processes on user desire
	if (mListProcesses) {
		if (ImGui::BeginCombo("Processes list", apd->mSelectedProcess.c_str())) {
			for (int i = 0; i < process_names.size(); i++) {
				// We need custom ids for nameing collisions
				if (apd->mSearchProcess.empty() || process_names[i].find(apd->mSearchProcess) != std::string::npos) {
					ImGui::PushID(&process_names[i]);

					// Select process (dont work)
					if (ImGui::Selectable(process_names[i].c_str())) {
						apd->mSelectedProcess = process_names[i];

						ImGui::PopID();

						break;
					}

					ImGui::PopID();
				}
			}

			ImGui::EndCombo();
		}
	}

	ImGui::Separator();

	// Open process on user desire
	if (ImGui::Button("Open process") || open_proc) {
		// We need process id firstly to open process
		apd->mProcessPtr.GetProcessId(std::wstring(apd->mSelectedProcess.begin(), apd->mSelectedProcess.end()).c_str());
		apd->mProcessPtr.OpenProc();
	}

	// Close process on user desire
	if (apd->mProcessPtr() != 0) {
		ImGui::SameLine();

		if (ImGui::Button("Close process")) {
			apd->mProcessPtr.CloseProc();
		}
	}

	// Show process id based on user preferences
	if (mDecimalValues) {
		ImGui::Text("Process ID: %llu", apd->mProcessPtr());
	}
	else {
		ImGui::Text("Process ID: %llx", apd->mProcessPtr());
	}
}

// Open module based on currently selected module (can be empty)
void EditProcessWindow::OpenSelectedModule(AccessProcessData* apd) {
	apd->mModuleAddress = apd->mProcessPtr.GetModuleAddress(std::wstring(apd->mSelectedModule.begin(), apd->mSelectedModule.end()).c_str());
}

void EditProcessWindow::ShowModules(AccessProcessData* apd) {
	ImGui::TextUnformatted("This allows to connect to process module after\nenabling \"Show module\" checkbox");

	// Show selecting module on user desire
	ImGui::Checkbox("Show modules", &apd->mShowModules);

	if (!apd->mSelectedProcess.empty() && apd->mProcessPtr() != 0) {
		if (apd->mShowModules) {
			// Get all process modules
			std::vector<std::string> module_names = Process::GetModuleNames(apd->mProcessPtr());

			// Input module name
			bool open_module = ImGui::InputText("Module name", &apd->mSelectedModule, ImGuiInputTextFlags_EnterReturnsTrue);

			// Search module name
			ImGui::InputText("Search modules", &apd->mSearchModule);

			// All found module names separated by new line
			std::string found_valid_module_names_from_search;

			// Run through all process names
			for (std::string& name : module_names) {
				// Check if process name contain desired string
				if (name.find(apd->mSearchModule) != std::string::npos) {
					// If yes add name to all found valid process
					found_valid_module_names_from_search += std::string(name.begin(), name.begin() + name.find_first_of('\0')) + '\n';
				}
			}

			// Then display it if found valid processes and search box arent empty, user still needs to type process name
			if (!found_valid_module_names_from_search.empty() && !apd->mSearchModule.empty()) {
				ImGui::InputTextMultiline("Found module", &found_valid_module_names_from_search, ImVec2(0, 0), ImGuiInputTextFlags_ReadOnly);
			}

			// List all modules on user demand
			if (mListModules) {
				if (ImGui::BeginCombo("Modules list", apd->mSelectedModule.c_str())) {
					for (int i = 0; i < module_names.size(); i++) {
						// This needs custom id
						ImGui::PushID(&module_names[i]);
						if (ImGui::Selectable(module_names[i].c_str())) {
							apd->mSelectedModule = module_names[i];

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
				OpenSelectedModule(apd);
			}

			ImGui::SameLine();

			// Show module address based on user preferences
			if (mDecimalValues) {
				ImGui::Text("Module @ %llu", apd->mModuleAddress);
			}
			else {
				ImGui::Text("Module @ %llx", apd->mModuleAddress);
			}
		}
	}
}

void EditProcessWindow::ShowWriteMemory(AccessProcessData* apd) {
	// If we dont have process dont show this
	if (!apd->mSelectedProcess.empty() && apd->mProcessPtr() != 0) {
		ImGui::SeparatorText("Write memory");

		// Include module address to write
		ImGui::Checkbox("Write staring from module address", &mWriteStartingFromModule);

		// Address of value to write to process memory
		ImGui::InputScalar("Address##01", ImGuiDataType_U64, (uint64_t*)&apd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

		// Unsigned : 0, 1, 2, 3; Signed : 4, 5, 6, 7; Floating point: 8, 9 
		const char* basic_value_types[] = { "Unsigned 64", "Unsigned 32", "Unsigned 16", "Unsigned 8", "Signed 64", "Signed 32", "Signed 16", "Signed 8", "Float 64", "Float 32" };

		// Written value type 
		ImGui::Combo("Value type", &mCurrentWriteValueType, basic_value_types, IM_ARRAYSIZE(basic_value_types));

		switch (mCurrentWriteValueType) {
		case 0: // U64
			// Written value
			ImGui::InputScalar("Value", ImGuiDataType_U64, (uint64_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			// Write to process memory address on user demand
			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint64_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 1: // U32
			ImGui::InputScalar("Value", ImGuiDataType_U32, (uint32_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint32_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 2: // U16
			ImGui::InputScalar("Value", ImGuiDataType_U16, (uint16_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint16_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 3: // U8
			ImGui::InputScalar("Value", ImGuiDataType_U8, (uint8_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%lx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint8_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 4: // S64
			ImGui::InputScalar("Value", ImGuiDataType_S64, (int64_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int64_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 5: // S32
			ImGui::InputScalar("Value", ImGuiDataType_S32, (int32_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int32_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 6: // S16
			ImGui::InputScalar("Value", ImGuiDataType_S16, (int16_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int16_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 7: // S8
			ImGui::InputScalar("Value", ImGuiDataType_S8, (int8_t*)&apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int8_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 8: // F64
			ImGui::InputScalar("Value", ImGuiDataType_Double, (double*)&apd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal*/);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint64_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		case 9: // F32
			ImGui::InputScalar("Value", ImGuiDataType_Float, (float*)&apd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal */);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint32_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;

		default: // U64
			ImGui::InputScalar("Value", ImGuiDataType_U64, &apd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint64_t>((mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress, apd->mValue);
			}

			break;
		}

		ImGui::SameLine();

		// Show address based on user preferences
		if (mDecimalValues) {
			ImGui::Text("Wrote @ %llu", (mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress);
		}
		else {
			ImGui::Text("Wrote @ %llx", (mWriteStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress);
		}
	}
}

void EditProcessWindow::ShowReadMemory(AccessProcessData* apd) {
	// If we dont have process dont show this
	if (!apd->mSelectedProcess.empty() && apd->mProcessPtr() != 0) {
		ImGui::SeparatorText("Read memory");

		// Include module offset
		ImGui::Checkbox("Read starting from module address", &mReadStartingFromModule);

		// Set address in memory ro read from
		ImGui::InputScalar("Address##02", ImGuiDataType_U64, (uint64_t*)&apd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

		// Read memory from process on user demand
		if (ImGui::Button("Read")) {
			apd->mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>((mReadStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress);
		}

		ImGui::SameLine();

		// Show value and address based on user preferences
		if (mDecimalValues) {
			ImGui::Text("Read @ %llu", (mReadStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress);

			ImGui::Text("Value: %llu", apd->mValue);
		}
		else {
			ImGui::Text("Read @ %llx", (mReadStartingFromModule == true ? apd->mModuleAddress : 0) + apd->mAddress);

			ImGui::Text("Value: %llx", apd->mValue);
		}
	}
}

void EditProcessWindow::AdvancedShowWriteMemory(AccessProcessData* apd, AdvancedAccessProcessData::Data* aapd) {
	if (!apd->mSelectedProcess.empty() && apd->mProcessPtr() != 0) {
		ImGui::SeparatorText("Write memory");

		ImGui::InputScalar("Address##01", ImGuiDataType_U64, (uint64_t*)&aapd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

		// Unsigned : 0, 1, 2, 3; Signed : 4, 5, 6, 7; Floating point: 8, 9 
		const char* basic_value_types[] = { "Unsigned 64", "Unsigned 32", "Unsigned 16", "Unsigned 8", "Signed 64", "Signed 32", "Signed 16", "Signed 8", "Float 64", "Float 32" };

		// Set value type
		ImGui::Combo("Value type", &aapd->mValueType, basic_value_types, IM_ARRAYSIZE(basic_value_types));

		switch (aapd->mValueType) {
		case 0: // U64
			// And set value (im not wtiring 10 same comments)
			ImGui::InputScalar("Value", ImGuiDataType_U64, (uint64_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			// Write on user demand
			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint64_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 1: // U32
			ImGui::InputScalar("Value", ImGuiDataType_U32, (uint32_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint32_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 2: // U16
			ImGui::InputScalar("Value", ImGuiDataType_U16, (uint16_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint16_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 3: // U8
			ImGui::InputScalar("Value", ImGuiDataType_U8, (uint8_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint8_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 4: // S64
			ImGui::InputScalar("Value", ImGuiDataType_S64, (int64_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int64_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 5: // S32
			ImGui::InputScalar("Value", ImGuiDataType_S32, (int32_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int32_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 6: // S16
			ImGui::InputScalar("Value", ImGuiDataType_S16, (int16_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int16_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 7: // S8
			ImGui::InputScalar("Value", ImGuiDataType_S8, (int8_t*)&aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<int8_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 8: // F64
			ImGui::InputScalar("Value", ImGuiDataType_Double, (double*)&aapd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal*/);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint64_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		case 9: // F32
			ImGui::InputScalar("Value", ImGuiDataType_Float, (float*)&aapd->mValue, (void*)0, (void*)0/*, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal */);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint32_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;

		default: // U64
			ImGui::InputScalar("Value", ImGuiDataType_U64, &aapd->mValue, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

			if (ImGui::Button("Write")) {
				apd->mProcessPtr.WriteMemory<uint64_t>(((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress, aapd->mValue);
			}

			break;
		}

		ImGui::SameLine();

		// Show address based on user preferences
		if (mDecimalValues) {
			ImGui::Text("Wrote @ %llu", ((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress);
		}
		else {
			ImGui::Text("Wrote @ %llx", ((aapd->mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress);
		}
	}
}

void EditProcessWindow::AdvancedShowReadMemory(AccessProcessData* apd, AdvancedAccessProcessData::Data* aapd) {
	if (!apd->mSelectedProcess.empty() && apd->mProcessPtr() != 0) {
		ImGui::SeparatorText("Read memory");

		ImGui::InputScalar("Address##02", ImGuiDataType_U64, (uint64_t*)&aapd->mAddress, (void*)0, (void*)0, (mDecimalValues == true ? (char*)0 : "%llx"), ImGuiInputTextFlags_CharsHexadecimal);

		// Read from address on user demand
		if (ImGui::Button("Read")) {
			if (aapd->mFlags & AAPDF_dontUseValueAsReadBuffer) aapd->mReadValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress);
			else aapd->mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress);
		}

		ImGui::SameLine();

		// Show value and address based on user preferences
		if (mDecimalValues) {
			ImGui::Text("Read @ %llu", ((aapd->mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress);

			// Do we want to just ead value or also write it
			if (aapd->mFlags & AAPDF_dontUseValueAsReadBuffer) ImGui::Text("Value: %llu", aapd->mReadValue);
			else ImGui::Text("Value: %llu", aapd->mValue);
		}
		else {
			ImGui::Text("Read @ %llx", ((aapd->mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mAddress);

			// Do we want to just ead value or also write it
			if (aapd->mFlags & AAPDF_dontUseValueAsReadBuffer) ImGui::Text("Value: %llx", aapd->mReadValue);
			else ImGui::Text("Value: %llx", aapd->mValue);
		}
	}
}

void EditProcessWindow::AddAdvancedData(AdvancedAccessProcessData* aapd, std::string name) {
	aapd->mData.emplace_back(AdvancedAccessProcessData::Data());
	aapd->mData[aapd->mData.size() - 1].mDataName = name;
}

void EditProcessWindow::AdvancedView(AccessProcessData* apd, int currentApd) {
	if (ImGui::CollapsingHeader("Advanced functions", ImGuiTreeNodeFlags_DefaultOpen)) {
		// Access AdvancedAccessProcessData by our current AccessProcessData
		AdvancedAccessProcessData* aapd = &mAdvAccessData[currentApd];

		ImGui::TextUnformatted("This allows to create macros, loop reading and writing memory\nand other cool stuff. For macros \"Advanced\" tab needs\nto be open");

		// Add data to R/W
		if (ImGui::Button("Add")) {
			AddAdvancedData(aapd, std::string("Data ") + std::to_string(gGlobalCounter++));
		}

		// Search data name
		ImGui::InputText("Search data", &apd->mSearchAdvancedData);

		std::int64_t delete_index = -1;

		for (std::size_t i = 0; i < aapd->mData.size(); i++) {
			// We need custom ids for no collision
			if (aapd->mData[i].mDataName.find(apd->mSearchAdvancedData) == std::string::npos) {
				continue;
			}

			//ImGui::PushID(&aapd->mData[i]);

			//ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if (ImGui::TreeNode(&aapd->mData[i], aapd->mData[i].mDataName.c_str())) {
				// Delete data
				if (ImGui::Button("Delete")) {
					delete_index = (std::int64_t)i;
				}

				ImGui::InputText("Data name", &aapd->mData[i].mDataName);

				// Flags should be self explainatory
				ImGui::CheckboxFlags("Read on key press", &aapd->mData[i].mFlags, AAPDF_readOnKey);
				ImGui::CheckboxFlags("Write on key press", &aapd->mData[i].mFlags, AAPDF_writeOnKey);
				ImGui::CheckboxFlags("Looping read", &aapd->mData[i].mFlags, AAPDF_readLoop);
				ImGui::CheckboxFlags("Looping write", &aapd->mData[i].mFlags, AAPDF_writeLoop);
				ImGui::CheckboxFlags("Read starting from module address", &aapd->mData[i].mFlags, AAPDF_readFromModuleAddress);
				ImGui::CheckboxFlags("Write starting from module address", &aapd->mData[i].mFlags, AAPDF_writeFromModuleAddress);
				ImGui::CheckboxFlags("Start loop on key", &aapd->mData[i].mFlags, AAPDF_loopOnKey);
				ImGui::CheckboxFlags("Limit loop index", &aapd->mData[i].mFlags, AAPDF_limitLoopIndex);
				ImGui::CheckboxFlags("Read returns to separate buffer", &aapd->mData[i].mFlags, AAPDF_dontUseValueAsReadBuffer);
				// Start looping only if process exist, address exist, write or read flag is on
				if ((aapd->mData[i].mFlags & AAPDF_readLoop || aapd->mData[i].mFlags & AAPDF_writeLoop) && apd->mProcessPtr() != 0 && aapd->mData[i].mAddress != 0)
					ImGui::CheckboxFlags("Start looping", &aapd->mData[i].mFlags, AAPDF_statusLooping);

				HelpTooltip("Make sure to use capital letters when defining regular key (special keys like \"end\" are typed as small)!\nPossible special key names: \nback, tab, clear, return, shift, control, pause, capital, escape, space, end, home, left, up, right, down, select, print, insert, delete, lwin, rwin, sleep, num0, num1, num2, num3, num4, num5, num6, num7, num8, num9, multiply, add, separator, subtract, decimal, divide, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, numlock, scroll, lshift, rshift, lcontrol, rcontrol, lmenu, rmenu");
				ImGui::InputText("Read/write key", &aapd->mData[i].mRWKeyName);
				ImGui::InputText("Start loop key", &aapd->mData[i].mLoopingKeyName);
				ImGui::InputInt("Loop index limit", (int*)&aapd->mData[i].mLoopIndexLimit);

				// Reset loop index
				if (ImGui::Button("Reset loop index")) aapd->mData[i].mLoopIndex = 0;

				// Show read and write
				AdvancedShowWriteMemory(apd, &aapd->mData[i]);
				AdvancedShowReadMemory(apd, &aapd->mData[i]);

				ImGui::TreePop();
			}

			//ImGui::PopID();

			// We need to change key names to key values that GetAsyncKeyState understand
			aapd->mData[i].mRWKey = KeynameToVkValue(aapd->mData[i].mRWKeyName.c_str());
			aapd->mData[i].mLoopingKey = KeynameToVkValue(aapd->mData[i].mLoopingKeyName.c_str());

			// If key specified by user is pressed...
			if (aapd->mData[i].mRWKey != 0 && GetAsyncKeyState(aapd->mData[i].mRWKey)) {
				// ... check if we want to read, process and address to read from exist...
				if (apd->mProcessPtr() != 0 && aapd->mData[i].mAddress != 0 && aapd->mData[i].mFlags & AAPDF_readOnKey) {
					// ... then read from specified address
					//aapd->mData[i].mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);

					if (aapd->mData[i].mFlags & AAPDF_dontUseValueAsReadBuffer) aapd->mData[i].mReadValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);
					else aapd->mData[i].mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);
				}

				// .. check if we want to write, process and address to write to exist..
				if (apd->mProcessPtr() != 0 && aapd->mData[i].mAddress != 0 && aapd->mData[i].mFlags & AAPDF_writeOnKey) {
					// ... then write specified value to specified earlier address
					apd->mProcessPtr.WriteMemory(((aapd->mData[i].mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress, aapd->mData[i].mValue);
				}
			}

			// If looping key is specified, process and address to read form exist and flag to start looping is set...
			if (apd->mProcessPtr() != 0 && aapd->mData[i].mAddress != 0 && aapd->mData[i].mFlags & AAPDF_readLoop && aapd->mData[i].mFlags & AAPDF_statusLooping) {
				// ... then check looping index limit, if there is none loop infinitly
				if (aapd->mData[i].mLoopIndexLimit == 0) {
					//aapd->mData[i].mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);
					if (aapd->mData[i].mFlags & AAPDF_dontUseValueAsReadBuffer) aapd->mData[i].mReadValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);
					else aapd->mData[i].mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);
				}
				else {
					// If there is some limit to loop then use it
					if (aapd->mData[i].mLoopIndex < aapd->mData[i].mLoopIndexLimit) {
						//aapd->mData[i].mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);
						if (aapd->mData[i].mFlags & AAPDF_dontUseValueAsReadBuffer) aapd->mData[i].mReadValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);
						else aapd->mData[i].mValue = apd->mProcessPtr.ReadMemory<std::uint64_t>(((aapd->mData[i].mFlags & AAPDF_readFromModuleAddress) == AAPDF_readFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress);

						aapd->mData[i].mLoopIndex++;
					}
				}
			}

			// If looping key is specified, process and address to write to exist and flag to start looping is set...
			if (apd->mProcessPtr() != 0 && aapd->mData[i].mAddress != 0 && aapd->mData[i].mFlags & AAPDF_writeLoop && aapd->mData[i].mFlags & AAPDF_statusLooping) {
				// ... then check looping index limit, if there is none loop infinitly
				if (aapd->mData[i].mLoopIndexLimit == 0) {
					apd->mProcessPtr.WriteMemory(((aapd->mData[i].mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress, aapd->mData[i].mValue);
				}
				else {
					// If there is some limit to loop then use it
					if (aapd->mData[i].mLoopIndex < aapd->mData[i].mLoopIndexLimit) {
						apd->mProcessPtr.WriteMemory(((aapd->mData[i].mFlags & AAPDF_writeFromModuleAddress) == AAPDF_writeFromModuleAddress ? apd->mModuleAddress : 0) + aapd->mData[i].mAddress, aapd->mData[i].mValue);

						aapd->mData[i].mLoopIndex++;
					}
				}
			}

			// Change looping status on key press earlier specyfied by user 
			if (GetAsyncKeyState(aapd->mData[i].mLoopingKey)) {
				aapd->mData[i].mFlags ^= AAPDF_statusLooping;
			}
		}

		// If we want to delete Aapd, we do it by index and set index back to -1 (accualy not needed)
		if (delete_index != -1) {
			aapd->mData.erase(aapd->mData.begin() + delete_index);
			delete_index = -1;
		}
	}
}

// Adds new process data
void EditProcessWindow::AddNewProcess() {
	mAccessProcessDatas.emplace_back(AccessProcessData());
	mAdvAccessData.emplace_back(AdvancedAccessProcessData());
}

// Delete process data
void EditProcessWindow::DeleteProcessById(std::uint32_t id) {
	mAccessProcessDatas[id].mProcessPtr.CloseProc();
	mAccessProcessDatas.erase(mAccessProcessDatas.begin() + id);

	mAdvAccessData.erase(mAdvAccessData.begin() + id);
}

// Shows menu bar
void EditProcessWindow::ShowMenuBar(GLFWwindow* window) {
	if (ImGui::BeginMenuBar()) {
		// Show minimize icon (needs to be in BeginMenuBar for displaying, otherwise it display behind menu bar)
		if (MinimizeButtonReimpl(ImVec2(ImGui::GetWindowSize().x - 40.0, 3.0))) {
			// Minimize window
			ShowWindow(glfwGetWin32Window(window), SW_MINIMIZE);
		}

		// Show close icon (needs to be in BeginMenuBar for displaying, otherwise it display behind menu bar)
		if (CloseButtonReimpl(ImVec2(ImGui::GetWindowSize().x - 20.0, 3.0))) {
			// Close window
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		// Render title
		ImGui::RenderText(ImVec2((ImGui::GetWindowSize().x / 2.0), 3.0), "MemoryAccess");

		if (ImGui::BeginMenu("Process")) {
			// Open new process
			if (ImGui::MenuItem("Interrupt new process")) {
				AddNewProcess();
			}

			// Close current process, 'X' also works
			if (ImGui::MenuItem("Close current tab")) {
				DeleteProcessById(mCurrentTab);
			}

			// List processes and modules in combo box
			ImGui::MenuItem("List processes", "", &mListProcesses);
			ImGui::MenuItem("List modules", "", &mListModules);

			// Quit program
			if(ImGui::MenuItem("Quit")) {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}

			ImGui::EndMenu();
		}

		// Begin config menu (with saving and loading configs)
		if (ImGui::BeginMenu("Config")) {
			if (ImGui::MenuItem("Save current tab to config")) {
				OpenSaveConfigDialogBox(window);
				mOpenSaveConfigPopup = true;
			}

			if (ImGui::MenuItem("Load config to current tab")) {
				OpenLoadConfigDialogBox(window);
				mOpenLoadConfigPopup = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			// If someone hate hex values
			ImGui::MenuItem("Decimal values", "", &mDecimalValues);
			// For advanced users
			ImGui::MenuItem("Advanced", "", &mAdvancedView);

			if (ImGui::MenuItem("Get values between addresses")) {
				mOpenGetValuesBetweenAddressesPopup = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help")) {
			// Just a calculator
			// TODO: Do it better
			if (ImGui::MenuItem("Calculator")) {
				mOpenCalculatorPopup = true;
			}

			// About program
			if (ImGui::MenuItem("About")) {
				mOpenInfoPopup = true;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Separator();
}

void EditProcessWindow::EditProcess(AccessProcessData* apd) {
	// Some headers and functions defined earlier in code
	if (ImGui::CollapsingHeader("Process", ImGuiTreeNodeFlags_DefaultOpen)) OpenProcess(apd);

	if (ImGui::CollapsingHeader("Modules", ImGuiTreeNodeFlags_DefaultOpen)) ShowModules(apd);

	if (ImGui::CollapsingHeader("Edit memory", ImGuiTreeNodeFlags_DefaultOpen)) {
		ShowWriteMemory(apd);

		ShowReadMemory(apd);
	}

	// Show advanced view
	if (mAdvancedView) {
		AdvancedView(apd, mCurrentTab);
	}
}

void EditProcessWindow::ShowProcessTabs() {
	// If tab is closed by 'X' delete it
	for (std::size_t i = 0; i < mAccessProcessDatas.size(); i++) {
		if (!mAccessProcessDatas[i].mTabOpen) {
			DeleteProcessById(i);
		}
	}

	// Open all process tabs
	if (ImGui::BeginTabBar("##process_tabs")) {
		for (std::size_t i = 0; i < mAccessProcessDatas.size(); i++) {
			// We need custom id
			ImGui::PushID(&mAccessProcessDatas[i]);
			// If it is visible...
			bool visible = ImGui::BeginTabItem(
				mAccessProcessDatas[i].mSelectedProcess.empty() == true || mAccessProcessDatas[i].mProcessPtr() == 0 ?
				(std::string("Empty process") + std::to_string(gGlobalCounter++)).c_str() : mAccessProcessDatas[i].mSelectedProcess.c_str(),
				&mAccessProcessDatas[i].mTabOpen);

			// ... then show it
			if (visible) {
				mCurrentTab = i;
				EditProcess(&mAccessProcessDatas[i]);

				ImGui::EndTabItem();
			}

			ImGui::PopID();
		}

		ImGui::EndTabBar();
	}
}

// Close button ('X') reimplementation from originam imgui_widget.cpp
bool EditProcessWindow::CloseButtonReimpl(const ImVec2& pos) {
	// Get context and current window
	ImGuiContext* g = ImGui::GetCurrentContext();
	ImGuiWindow* window = g->CurrentWindow;

	// Make rectangle for collision
	const ImRect bb(pos, ImVec2(pos.x + g->FontSize, pos.y + g->FontSize));
	// Check mouse collision with rectangle
	bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);

	bool pressed = false;
	// If left mouse button is pressed set pressed state to true
	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		pressed = true;
	}

	// Rect color
	ImU32 bg_col = ImGui::GetColorU32(pressed ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
	// If hovered change color
	if (hovered)
		window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_col);

	// 'X' color same as text
	ImU32 cross_col = ImGui::GetColorU32(ImGuiCol_Text);
	// Center of 'X'
	ImVec2 cross_center = ImVec2(bb.GetCenter().x - 0.5f, bb.GetCenter().y - 0.5f);
	// Extent of 'X'
	float cross_extent = g->FontSize * 0.5f * 0.7071f - 1.0f;
	// Add 2 lines to draw list with proper vertices
	window->DrawList->AddLine(ImVec2(cross_center.x + cross_extent, cross_center.y + cross_extent), ImVec2(cross_center.x - cross_extent, cross_center.y - cross_extent), cross_col, 1.0f);
	window->DrawList->AddLine(ImVec2(cross_center.x + cross_extent, cross_center.y - cross_extent), ImVec2(cross_center.x - cross_extent, cross_center.y + cross_extent), cross_col, 1.0f);

	// return state
	return pressed;
}

bool EditProcessWindow::MinimizeButtonReimpl(const ImVec2& pos) {
	// Get context and current window
	ImGuiContext* g = ImGui::GetCurrentContext();
	ImGuiWindow* window = g->CurrentWindow;
	
	// Make rectangle for collision
	const ImRect bb(pos, ImVec2(pos.x + g->FontSize, pos.y + g->FontSize));
	// Check mouse collision with rectangle
	bool hovered = ImGui::IsMouseHoveringRect(bb.Min, bb.Max);

	bool pressed = false;
	// If left mouse button is pressed set pressed state to true
	if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		pressed = true;
	}

	// Rect color
	ImU32 bg_col = ImGui::GetColorU32(pressed ? ImGuiCol_ButtonActive : ImGuiCol_ButtonHovered);
	// If hovered change color
	if (hovered)
		window->DrawList->AddRectFilled(bb.Min, bb.Max, bg_col);

	// Line color sane as text
	ImU32 line_col = ImGui::GetColorU32(ImGuiCol_Text);
	// Line center
	ImVec2 line_center = ImVec2(bb.GetCenter().x - 0.5f, bb.GetCenter().y - 0.5f);
	// Line extent
	float line_extent = g->FontSize * 0.5f * 0.7071f - 1.0f;
	// Add line to draw list
	window->DrawList->AddLine(ImVec2(line_center.x + line_extent, line_center.y + line_extent), ImVec2(line_center.x - line_extent, line_center.y + line_extent), line_col, 1.0f);

	// return state
	return pressed;
}

//public:
void EditProcessWindow::Show(GLFWwindow* window) {
	// Display window in entire window area
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	bool smaw_visible = ImGui::Begin("#process_window", nullptr,
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	// showMemowyAccessWindow visible state, if false end window and return
	if (!smaw_visible) {
		ImGui::End();

		return;
	}

	//
	// Self explainatory methods
	//

	// We pass here GLFWwindow only for closing window
	ShowMenuBar(window);
	
	ShowProcessTabs();

	OpenCalculatorPopup();

	OpenSoftwareInfoPopup();

	OpenSaveConfigPopup();

	OpenLoadConfigPopup();

	if (!mAccessProcessDatas.empty()) {
		OpenGetValuesBetweenAddressesPopup(&mAccessProcessDatas[mCurrentTab]);
	}

	ImGui::End();
}