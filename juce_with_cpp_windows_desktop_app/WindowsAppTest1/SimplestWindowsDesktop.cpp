// HelloWindowsDesktop.cpp
// compile with: /D_UNICODE /DUNICODE /DWIN32 /D_WINDOWS /c

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include <format>
#include <cassert>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <iostream>
#include <sstream>

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

// Stored instance handle for use in Win32 API calls such as FindResource
HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 100,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);


    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return (int)msg.wParam;
}

int init_audio() {

    /// JUCE -------------------------------------------------------------
    const int request_device_type_id = 0;
    const int request_device_name_id = 0;
    std::stringstream ss;

    juce::AudioDeviceManager adm;
    juce::ToneGeneratorAudioSource tone_generator;
    juce::AudioSourcePlayer player;

    //MessageBox(NULL, "Starting tests of audio device setup with JUCE module!", "Info", MB_OK);

    // List up available juce::AudioIODeviceType
    //     (Example):
    //     Windows Audio
    //     Windows Audio(Exclusive Mode)
    //     Windows Audio(Low Latency Mode)
    //     DirectSound
    const auto& device_types = adm.getAvailableDeviceTypes();
    assert(!device_types.isEmpty());

    ss.clear();
    ss << "getAvailableDeviceTypes(): " << std::endl;
    for (auto type : device_types) {
        ss << std::format("{} {}", "", type->getTypeName().toStdString()).c_str() << std::endl;
    }

	// Enable request audio device type
    ss.clear();
    adm.setCurrentAudioDeviceType(
        device_types[request_device_type_id]->getTypeName(), false
    );
    MessageBox(NULL, ss.str().c_str(), "Info", MB_OK);

    //ss << "AudioDeviceManager.setCurrentAudioDeviceType():" << std::endl <<  "   " << device_types[request_device_type_id]->getTypeName().toStdString();
    //MessageBox(NULL, ss.str().c_str(), "Info", MB_OK);

    auto current_type = adm.getCurrentDeviceTypeObject();

    current_type->scanForDevices();    // List up available devices

    //adm.initialiseWithDefaultDevices(0, 2);

    juce::AudioDeviceManager::AudioDeviceSetup setup;
    setup.outputChannels.setRange(0, 2, true);
    setup.inputChannels.setRange(0, 2, false);
    setup.bufferSize = 480;
    setup.sampleRate = 48000;
    setup.useDefaultOutputChannels = false;
    setup.useDefaultInputChannels = false;

    auto device_type = device_types[request_device_type_id];
    setup.outputDeviceName = device_type->getDeviceNames()[request_device_name_id];
    setup.inputDeviceName = device_type->getDeviceNames(true)[request_device_name_id];

    auto error_message = adm.setAudioDeviceSetup(setup, false);

    if (error_message.isEmpty()) {
        MessageBox(NULL, "AudioDeviceManager.setAudioDeviceSetup() successfully,", "Info", MB_OK);
    } else {
        MessageBox(NULL, error_message.toStdString().c_str(), "Error", MB_OK);
        assert(error_message.isEmpty());
    }
    
	// Get opened audio device information
    auto current_device = adm.getCurrentAudioDevice();

    ss.clear();
    ss << std::format("{} {}", "Device Type Name:", current_device->getTypeName().toStdString()).c_str() << std:: endl;
    ss << std::format("{} {}", "Device Name:", current_device->getName().toStdString()).c_str() << std::endl;

    for (auto name : current_device->getOutputChannelNames()) {
        ss << std::format("{} {}", "Available Output Channel:", name.toStdString()).c_str() << std::endl;
    }

    for (auto name : current_device->getInputChannelNames()) {
        ss << std::format("{} {}", "Available Input Channel:", name.toStdString()).c_str() << std::endl;
    }

    for (double sampling_rate : current_device->getAvailableSampleRates()) {
        ss << std::format("{} {}", "Available Sampling Rates:", std::round(sampling_rate)).c_str() << std::endl;
    }

    for (auto buffer_size : current_device->getAvailableBufferSizes()) {
        ss << std::format("{} {}", "Available Buffer Sizes:", buffer_size).c_str() << std::endl;
    }
    MessageBox(NULL, ss.str().c_str(), "Info", MB_OK);

    MessageBox(NULL, "Playing sine wave sound start!,", "Info", MB_OK);
    tone_generator.setFrequency(440);
    player.setSource(&tone_generator);
    adm.addAudioCallback(&player);

    MessageBox(NULL, "Playing sine wave sound finished.", "Info", MB_OK);

	// Close audio device
    adm.closeAudioDevice();

    MessageBox(NULL, "closeAudioDevice() finished!", "Info", MB_OK);

    return 0;
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, Windows desktop!");

    switch (message)
    {
    case WM_CREATE:
        init_audio();


    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // Here your application is laid out.
        // For this introduction, we just print out "Hello, Windows desktop!"
        // in the top left corner.
        TextOut(hdc,
            5, 5,
            greeting, _tcslen(greeting));
        // End application-specific layout section.

        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        //adm.closeAudioDevice();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}