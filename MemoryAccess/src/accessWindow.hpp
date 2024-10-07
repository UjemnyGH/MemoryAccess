/**
* 
* @author Piotr UjemnyGH Plombon
* 
* Entire process window class
* 
*/

#pragma once
#ifndef _MA_ACCESS_WINDOW_
#define _MA_ACCESS_WINDOW_

/**
* 
* TODO:
* 
*/

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_stdlib.h"
#include "processHandler.hpp"
#include "memoryAccessCore.hpp"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <WinUser.h>

// Simple access struct
struct AccessProcessData {
	Process mProcessPtr;
	std::string mSelectedProcess;
	std::string mSelectedModule;
	std::uintptr_t mModuleAddress = 0;

	std::uintptr_t mAddress = 0;
	std::uint64_t mValue = 0;
	bool mShowModules = false;
	bool mTabOpen = true;
	std::string mSearchProcess;
	std::string mSearchModule;
	std::string mSearchAdvancedData;
};

// Advanced access flags
enum AdvancedAccessProcessDataFlags {
	AAPDF_readOnKey = 0x1,
	AAPDF_writeOnKey = 0x2,
	AAPDF_writeLoop = 0x4,
	AAPDF_readLoop = 0x8,
	AAPDF_readFromModuleAddress = 0x10,
	AAPDF_writeFromModuleAddress = 0x20,
	AAPDF_statusLooping = 0x40,
	AAPDF_loopOnKey = 0x80,
	AAPDF_limitLoopIndex = 0x100,
	// Use separate value buffers
	AAPDF_dontUseValueAsReadBuffer = 0x200,
};

// Advanced access struct
struct AdvancedAccessProcessData {
	struct Data {
		std::string mDataName;
		std::uintptr_t mAddress;
		std::uint64_t mValue;
		std::uint64_t mReadValue;
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

// Main editor class
class EditProcessWindow {
private:
	// Our simple data
	std::vector<AccessProcessData> mAccessProcessDatas;
	// Our advanced data
	std::vector<AdvancedAccessProcessData> mAdvAccessData;
	// Current tab index
	std::size_t mCurrentTab = 0;

	// Open calculator popup
	bool mOpenCalculatorPopup = false;
	// Opens software info popup
	bool mOpenInfoPopup = false;
	// Opens popup for getting values between 2 addresses
	bool mOpenGetValuesBetweenAddressesPopup = false;
	// Open save config popup
	bool mOpenSaveConfigPopup = false;
	// Open load config popup
	bool mOpenLoadConfigPopup = false;
	// Open selected process up on loading
	bool mOpenProcessOnLoad = false;
	// Open selected module up on loading
	bool mOpenModuleOnLoad = false;
	// Save/load config filename 
	std::string mConfigFilename;
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
	// Is advanced view opened
	bool mAdvancedView = false;
	// Dialog box data
	OPENFILENAMEA mDialogBoxData;

	// Get values between 2 addresses popup variables
	std::uintptr_t mFetchedAddressLow;
	std::uintptr_t mFetchedAddressHigh;
	bool mBeginFromModuleAddress = false;
	bool mRunFetchingReadStatus = false;

	/**
	* Shows tooltip
	*/
	void HelpTooltip(const char* desc);

	/**
	* Opens calculator popup window
	*/
	void OpenCalculatorPopup();

	/**
	* Opens software info popup window
	*/
	void OpenSoftwareInfoPopup();

	/**
	* Opens getting values between addresses popup
	*/
	void OpenGetValuesBetweenAddressesPopup(AccessProcessData* apd);

	/**
	* Opens save config dialog box
	*/
	void OpenSaveConfigDialogBox(GLFWwindow* wnd);

	/**
	* Opens load config dialog box
	*/
	void OpenLoadConfigDialogBox(GLFWwindow* wnd);

	/**
	* Opens save current tab config popup window
	*/
	void OpenSaveConfigPopup();

	/**
	* Opens load to current tab config popup window
	*/
	void OpenLoadConfigPopup();

	/**
	* Open process in access window data struct and show its header
	*/
	void OpenProcess(AccessProcessData* apd);

	/**
	* Opens selected process module defined by user
	*/
	void OpenSelectedModule(AccessProcessData* apd);

	/**
	* Show modules header
	*/
	void ShowModules(AccessProcessData* apd);

	/**
	* Writes memory to process address shows it
	*/
	void ShowWriteMemory(AccessProcessData* apd);

	/**
	* Reads memory from process address shows it
	*/
	void ShowReadMemory(AccessProcessData* apd);

	/**
	* Writes memory to process address shows it, but more advanced
	*/
	void AdvancedShowWriteMemory(AccessProcessData* apd, AdvancedAccessProcessData::Data* aapd);

	/**
	* Reads memory from process address shows it, but more advanced
	*/
	void AdvancedShowReadMemory(AccessProcessData* apd, AdvancedAccessProcessData::Data* aapd);

	/**
	* Add advanced access process data with name
	*/
	void AddAdvancedData(AdvancedAccessProcessData* aapd, std::string name);

	/**
	* Shows advanced access process data for current tab <currentApd>, 
	*/
	void AdvancedView(AccessProcessData* apd, int currentApd);

	/**
	* Shows menu bar
	*/
	void ShowMenuBar(GLFWwindow* window);

	/**
	* Adds new process data
	*/
	void AddNewProcess();

	/**
	* Delete process by id (tab id, NOT process id)
	*/
	void DeleteProcessById(std::uint32_t id);

	/**
	* Shows everything combined per process tab (quite short function but helps to understand code)
	*/
	void EditProcess(AccessProcessData* apd);

	/**
	* Shows all process data tabs
	*/
	void ShowProcessTabs();

	bool CloseButtonReimpl(const ImVec2& pos);

	bool MinimizeButtonReimpl(const ImVec2& pos);

public:
	bool mMouseHoveredOnMenuBar = false;
	bool mLMBClicked = false;

	/**
	* Shows main window
	*/
	void Show(GLFWwindow* window);
};

#endif // _MA_ACCESS_WINDOW