#pragma once
#ifndef __ACCESS_WINDOW_
#define __ACCESS_WINDOW_

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "processHandler.hpp"

struct AccessWindowData {
	Process mProcessPtr;
	std::string mSelectedProcess;
	int mSelectedModule = 0;
	std::uintptr_t mModuleAddress = 0;

	std::uintptr_t mAddress = 0;
	std::uint64_t mValue = 0;
};

int gGlobalCounter = 0;

void ResetGlobalCounter() {
	gGlobalCounter = 0;
}

void ShowMemAccessWindow(AccessWindowData* awd) {
	std::vector<std::string> process_names = Process::GetProcessNames();
	ImGui::Begin((std::string("Process ") + std::to_string(gGlobalCounter++)).c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

	bool open_proc = ImGui::InputText("Process name", &awd->mSelectedProcess, ImGuiInputTextFlags_EnterReturnsTrue);

	/*if (ImGui::BeginCombo("Process name", "exe")) {
		for (int i = 0; i < process_names.size(); i++) {
			if (ImGui::Selectable(process_names[i].c_str())) {
				awd->mSelectedProcess = i;

				break;
			}
		}

		ImGui::EndCombo();
	}*/

	ImGui::Separator();

	if (ImGui::Button("Open process") || open_proc) {
		awd->mProcessPtr.GetProcessId(std::wstring(awd->mSelectedProcess.begin(), awd->mSelectedProcess.end()).c_str());
		awd->mProcessPtr.OpenProc();
	}

	HANDLE phandle;
	awd->mProcessPtr(&phandle);

	if (awd->mProcessPtr() && phandle != INVALID_HANDLE_VALUE) {
		ImGui::SameLine();

		if (ImGui::Button("Close process")) {
			awd->mProcessPtr.CloseProc();
		}
	}

	if (!awd->mSelectedProcess.empty() && awd->mProcessPtr() && phandle != INVALID_HANDLE_VALUE) {
		std::vector<std::string> module_names = Process::GetModuleNames(awd->mProcessPtr());
		if (ImGui::BeginCombo("Module name", module_names[awd->mSelectedModule].c_str())) {
			for (int i = 0; i < module_names.size(); i++) {
				if (ImGui::Selectable(module_names[i].c_str())) {
					awd->mSelectedModule = i;

					break;
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		if (ImGui::Button("Get module address")) {
			awd->mModuleAddress = awd->mProcessPtr.GetModuleAddress(std::wstring(module_names[awd->mSelectedModule].c_str()[0], module_names[awd->mSelectedModule].c_str()[256]).c_str());
		}

		ImGui::SameLine();

		ImGui::Text("%ld", awd->mModuleAddress);

		ImGui::SeparatorText("Write memory");

		bool clicked = ImGui::InputScalar("Address#01", ImGuiDataType_U64, &awd->mAddress, (void*)1, (void*)1, (char*)0, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);
		clicked = ImGui::InputScalar("Value", ImGuiDataType_U64, &awd->mValue, (void*)1, (void*)1, (char*)0, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);

		if (ImGui::Button("Write") || clicked) {
			awd->mProcessPtr.WriteMemory(awd->mAddress, awd->mValue);
		}

		ImGui::SeparatorText("Read memory");

		clicked = ImGui::InputScalar("Address#02", ImGuiDataType_U64, &awd->mAddress, (void*)1, (void*)1, (char*)0, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue);

		if (ImGui::Button("Read") || clicked) {
			awd->mValue = awd->mProcessPtr.ReadMemory<std::uint64_t>(awd->mAddress);
		}

		ImGui::SameLine();

		ImGui::Text("Value: %ld", awd->mValue);
	}

	ImGui::End();
}

#endif