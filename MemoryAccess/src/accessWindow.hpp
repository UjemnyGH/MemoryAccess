#pragma once
#ifndef __ACCESS_WINDOW_
#define __ACCESS_WINDOW_

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "processHandler.hpp"

int gGlobalCounter = 0;

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

void ResetGlobalCounter() {
	gGlobalCounter = 0;
}

class EditProcessWindow {
private:
	std::vector<AccessWindowData> mAccessWindowDatas;
	std::size_t mCurrentTab = 0;

	bool mOpenCalculatorPopup = false;
	uint64_t mCalculatorData = 0;
	bool mListModules = false;
	bool mListProcesses = false;
	bool mWriteStartingFromModule = false;
	bool mReadStartingFromModule = false;

	void OpenCalculatorPopup() {
		if (mOpenCalculatorPopup) {
			ImGui::OpenPopup("Calculator");
		}

		if (ImGui::BeginPopupModal("Calculator", &mOpenCalculatorPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::InputScalar("Dec", ImGuiDataType_U64, &mCalculatorData, (void*)0, (void*)0, (char*)0, ImGuiInputTextFlags_CharsDecimal);
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

		bool open_proc = ImGui::InputText("Process name", &awd->mSelectedProcess, ImGuiInputTextFlags_EnterReturnsTrue);

		if (mListProcesses) {
			if (ImGui::BeginCombo("Processes list", awd->mSelectedProcess.c_str())) {
				for (int i = 0; i < process_names.size(); i++) {
					ImGui::PushID(&process_names[i]);

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

		if (ImGui::Button("Open process") || open_proc) {
			awd->mProcessPtr.GetProcessId(std::wstring(awd->mSelectedProcess.begin(), awd->mSelectedProcess.end()).c_str());
			awd->mProcessPtr.OpenProc();
		}

		if (awd->mProcessPtr() != 0) {
			ImGui::SameLine();

			if (ImGui::Button("Close process")) {
				awd->mProcessPtr.CloseProc();
			}
		}

		ImGui::Text("Process ID: %llu", awd->mProcessPtr());
	}

	void ShowModules(AccessWindowData* awd) {
		ImGui::Checkbox("Show modules", &awd->mShowModules);

		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			if (awd->mShowModules) {
				std::vector<std::string> module_names = Process::GetModuleNames(awd->mProcessPtr());

				bool open_module = ImGui::InputText("Module name", &awd->mSelectedModule, ImGuiInputTextFlags_EnterReturnsTrue);

				if (mListModules) {
					if (ImGui::BeginCombo("Modules list", awd->mSelectedModule.c_str())) {
						for (int i = 0; i < module_names.size(); i++) {
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

				if (ImGui::Button("Get module address") || open_module) {
					awd->mModuleAddress = awd->mProcessPtr.GetModuleAddress(std::wstring(awd->mSelectedModule.begin(), awd->mSelectedModule.end()).c_str());
				}

				ImGui::SameLine();

				ImGui::Text("Module @ %llu", awd->mModuleAddress);
			}
		}
	}

	void ShowWriteMemory(AccessWindowData* awd) {
		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			ImGui::SeparatorText("Write memory");

			ImGui::Checkbox("Write staring from module address", &mWriteStartingFromModule);

			ImGui::InputScalar("Address##01", ImGuiDataType_U64, (uint64_t*)&awd->mAddress, (void*)1, (void*)1, (char*)0, ImGuiInputTextFlags_CharsDecimal);
			ImGui::InputScalar("Value", ImGuiDataType_U64, &awd->mValue, (void*)1, (void*)1, (char*)0, ImGuiInputTextFlags_CharsDecimal);

			if (ImGui::Button("Write")) {
				awd->mProcessPtr.WriteMemory((mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress, awd->mValue);
			}

			ImGui::SameLine();

			ImGui::Text("Wrote @ %llu", (mWriteStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);
		}
	}

	void ShowReadMemory(AccessWindowData* awd) {
		if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() != 0) {
			ImGui::SeparatorText("Read memory");

			ImGui::Checkbox("Read starting from module address", &mReadStartingFromModule);

			ImGui::InputScalar("Address##02", ImGuiDataType_U64, (uint64_t*) & awd->mAddress, (void*)1, (void*)1, (char*)0, ImGuiInputTextFlags_CharsDecimal);

			if (ImGui::Button("Read")) {
				awd->mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>((mReadStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);
			}
			
			ImGui::SameLine();

			ImGui::Text("Read @ %llu", (mReadStartingFromModule == true ? awd->mModuleAddress : 0) + awd->mAddress);

			ImGui::Text("Value: %llu", awd->mValue);
		}
	}
	
	void EditProcess(AccessWindowData* awd) {
		if(ImGui::CollapsingHeader("Process")) OpenProcess(awd);

		if (ImGui::CollapsingHeader("Modules")) ShowModules(awd);

		if (ImGui::CollapsingHeader("Edit memory")) {
			ShowWriteMemory(awd);

			ShowReadMemory(awd);
		}
	}

	void ShowMenuBar() {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("Process")) {
				if (ImGui::MenuItem("Interrupt new process")) {
					mAccessWindowDatas.emplace_back(AccessWindowData());
				}

				if (ImGui::MenuItem("Close current tab")) {
					mAccessWindowDatas[mCurrentTab].mProcessPtr.CloseProc();
					mAccessWindowDatas.erase(mAccessWindowDatas.begin() + mCurrentTab);
				}

				ImGui::MenuItem("List processes", "", &mListProcesses);
				ImGui::MenuItem("List modules", "", &mListModules);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("Calculator")) {
					mOpenCalculatorPopup = true;
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Separator();
	}

	void ShowProcessTabs() {
		for (std::size_t i = 0; i < mAccessWindowDatas.size(); i++) {
			if (!mAccessWindowDatas[i].mTabOpen) {
				mAccessWindowDatas[i].mProcessPtr.CloseProc();
				mAccessWindowDatas.erase(mAccessWindowDatas.begin() + i);
			}
		}

		if (ImGui::BeginTabBar("##process_tabs")) {
			for (std::size_t i = 0; i < mAccessWindowDatas.size(); i++) {
				ImGui::PushID(&mAccessWindowDatas[i]);
				bool visible = ImGui::BeginTabItem(
					mAccessWindowDatas[i].mSelectedProcess.empty() == true || mAccessWindowDatas[i].mProcessPtr() == 0 ? 
					(std::string("Empty process") + std::to_string(gGlobalCounter++)).c_str() : mAccessWindowDatas[i].mSelectedProcess.c_str(), 
					&mAccessWindowDatas[i].mTabOpen);
			
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