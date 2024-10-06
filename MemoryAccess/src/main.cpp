/**
* 
* @author Piotr UjmenyGH Plombon
* 
* Just a main
*/

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include <stdio.h>
#include <algorithm>

#include "accessWindow.hpp"

void StyleColorsMonochrome(ImGuiStyle* dst = (ImGuiStyle*)0) {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.83f, 0.83f, 0.83f, 0.69f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.35f, 0.35f, 0.35f, 0.83f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.17f, 0.17f, 0.17f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 0.80f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.23f, 0.23f, 0.23f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.64f, 0.64f, 0.64f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.86f, 0.86f, 0.86f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.45f, 0.45f, 0.45f, 0.62f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.79f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 0.45f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.65f, 0.65f, 0.65f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.77f, 0.77f, 0.77f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.92f, 0.92f, 0.90f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.80f);
    colors[ImGuiCol_Tab] = ImVec4(0.62f, 0.62f, 0.62f, 0.79f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.32f, 0.32f, 0.32f, 0.84f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.09f, 0.09f, 0.09f, 0.82f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.15f, 0.15f, 0.15f, 0.84f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.17f, 0.17f, 0.17f, 0.80f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextLink] = ImVec4(0.00f, 0.00f, 1.00f, 0.80f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.89f, 0.89f, 0.89f, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

int main(int, char**){
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 450";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(500, 600, "MemoryAccess", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    StyleColorsMonochrome();
    ImGui::GetStyle().GrabMinSize = 10;
    ImGui::GetStyle().ScrollbarSize = 16;
    ImGui::GetStyle().WindowBorderSize = 0;
    ImGui::GetStyle().WindowRounding = 0;
    ImGui::GetStyle().ChildRounding = 0;
    ImGui::GetStyle().FrameRounding = 0;
    ImGui::GetStyle().PopupRounding = 0;
    ImGui::GetStyle().ScrollbarRounding = 0;
    ImGui::GetStyle().GrabRounding = 0;
    ImGui::GetStyle().TabRounding = 0;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // We need Win32 window handle
    HWND hWnd = glfwGetWin32Window(window);
    // To disable title bar
    SetWindowLong(hWnd, GWL_STYLE, WS_VISIBLE);
    // And not showing white window that crash
    ShowWindow(hWnd, SW_SHOW);

    EditProcessWindow epw;

    // Is mouse hovering menu bar
    bool mouseWasHovered = false;
    // Resizing on X axis
    bool resizeOnX = false;
    // Resizing on Y axis
    bool resizeOnY = false;
    // Mouse position before trying to drag window
    double lastMouseX = 0.0, lastMouseY = 0.0;
    // Window resize offset
    int windowSizeOffX = 0, windowSizeOffY = 0;
    // Window size
    int windowSizeX = 500, windowSizeY = 600;
    // Temporary values for resizing
    int tempWinSizeX = 500, tempWinSizeY = 600;
    // Resize area (border size) that can resize window
    const int resizeArea = 8;

    // Resize cursors ew = east_west, ns = north_south
    GLFWcursor* resize_ew = glfwCreateStandardCursor(GLFW_RESIZE_EW_CURSOR);
    GLFWcursor* resize_ns = glfwCreateStandardCursor(GLFW_RESIZE_NS_CURSOR);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui::SetClipboardText(glfwGetClipboardString(window));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ResetGlobalCounter();

        epw.Show(window);
        
        // TODO: Put "windowstitlebarless" stuff in independent function or class

        // Mouse position on screens, NOT in window
        POINT mouse;
        GetCursorPos(&mouse);

        // Mouse position in window
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Current window width and height, for error checking
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Current window position
        int winPosX, winPosY;
        glfwGetWindowPos(window, &winPosX, &winPosY);

        // If mouse is on right border and mouse button isn`t clicked show cursor and prepare to resize window
        if (mouseX <= width && mouseX >= width - resizeArea && !GetAsyncKeyState(VK_LBUTTON)) {
            resizeOnX = true;

            tempWinSizeX = windowSizeX;
            windowSizeOffX = mouse.x;
            glfwSetCursor(window, resize_ew);
        }
        // If mouse is on left border and mouse button isn`t clicked show cursor and prepare to resize window
        else if (mouseX >= 0 && mouseX <= resizeArea && !GetAsyncKeyState(VK_LBUTTON)) {
            resizeOnX = true;

            tempWinSizeX = windowSizeX;
            windowSizeOffX = mouse.x;
            glfwSetCursor(window, resize_ew);
        }
        // If mouse is on bottom border and mouse button isn`t clicked show cursor and prepare to resize window
        else if (mouseY <= height && mouseY >= height - resizeArea && !GetAsyncKeyState(VK_LBUTTON)) {
            resizeOnY = true;

            tempWinSizeY = windowSizeY;
            windowSizeOffY = mouse.y;
            glfwSetCursor(window, resize_ns);
        }
        // If mouse is on top border and mouse button isn`t clicked show cursor and prepare to resize window
        else if (mouseY >= 0 && mouseY <= resizeArea && !GetAsyncKeyState(VK_LBUTTON)) {
            resizeOnY = true;

            tempWinSizeY = windowSizeY;
            windowSizeOffY = mouse.y;
            glfwSetCursor(window, resize_ns);
        }
        // If mouse is behind resizible area set flags to false and set window size to temp size
        else if(mouseX > resizeArea + 1 && mouseX < width - (resizeArea + 1) && mouseY > resizeArea + 1 && mouseY < height - (resizeArea + 1) && !GetAsyncKeyState(VK_LBUTTON)) {
            resizeOnX = false;
            resizeOnY = false;
            windowSizeX = tempWinSizeX;
            windowSizeY = tempWinSizeY;

            glfwSetCursor(window, NULL);
        }

        // If there is no resizing, draging window is enabled
        if (!resizeOnX && !resizeOnY) {
            // Check if mouse is on menu bar
            if (mouseX >= 0 && mouseX <= width && mouseY >= 0 && mouseY <= 18) mouseWasHovered = true;

            // If is and mouse button is pressed drag window on screen
            if (mouseWasHovered && GetAsyncKeyState(VK_LBUTTON)) {
                // 8 and 31 are perfectly dialed in values that work I thing becouse of border size and overall windows displaying stuff
                glfwSetWindowPos(window, (int)mouse.x - (int)lastMouseX + 8, (int)mouse.y - (int)lastMouseY + 31);
            }

            // If mouse is just hovered get last mouse position and remove hover flag for convinience
            if (mouseWasHovered && !GetAsyncKeyState(VK_LBUTTON)) {
                mouseWasHovered = false;
                lastMouseX = mouseX;
                lastMouseY = mouseY;
            }
        }

        // If mouse is behind window and mouse button isnt` pressed remove resize flags
        if ((mouse.x < winPosX || mouse.x > winPosX + width || mouse.y < winPosY || mouse.y > winPosY + height) && !GetAsyncKeyState(VK_LBUTTON)) {
            resizeOnX = false;
            resizeOnY = false;
        }
        
        // X axis resize case (clamping value for convinience)
        if (resizeOnX && GetAsyncKeyState(VK_LBUTTON)) {
            windowSizeX = tempWinSizeX - (windowSizeOffX - mouse.x);
            windowSizeX = std::clamp(windowSizeX, 200, 32768);

            glfwSetWindowSize(window, windowSizeX, windowSizeY);
        }
        // Y axis resize case (clamping value for convinience)
        else if (resizeOnY && GetAsyncKeyState(VK_LBUTTON)) {
            windowSizeY = tempWinSizeY - (windowSizeOffY - mouse.y);
            windowSizeY = std::clamp(windowSizeY, 200, 32768);

            glfwSetWindowSize(window, windowSizeX, windowSizeY);
        }

        // If temporary values aren`t equal with window size, equal them (just for error checking, without this if window can resize Y axis up on resizing X axis by user)
        if (tempWinSizeX != windowSizeX || tempWinSizeY != windowSizeY) {
            windowSizeX = tempWinSizeX;
            windowSizeY = tempWinSizeY;

            glfwSetWindowSize(window, windowSizeX, windowSizeY);
        }

        //ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

// Remnance of just a simple concept of accessing process memory,
// now look where it get. If you dont have motivation to do somehing
// just think about yourself as you are this program, you can be nowere,
// but with some time and effor you can achive something great.
// Cheers Piotr UjemnyGH Plombon
/*int main() {
    Process process;

    process.GetProcessId(L"notepad.exe");

    std::vector<std::string> modules = Process::GetModuleNames(process());

    for (auto & wstr : modules) {
        std::cout << "[INFO]: Module: " << wstr << std::endl;
    }

    process.OpenProc();

    std::cout << "[INFO]: Process id: " << process() << std::endl;

    process.CloseProc();

    std::cin.get();

    return 0;
}*/
