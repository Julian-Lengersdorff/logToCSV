// Minimal x64dbg plugin
#include <cstring>
#include "pluginmain.h"

// Global variables required by the plugin framework
int pluginHandle = 0;
HWND hwndDlg = 0;
int hMenu = 0;
int hMenuDisasm = 0;
int hMenuDump = 0;
int hMenuStack = 0;
int hMenuGraph = 0;
int hMenuMemmap = 0;
int hMenuSymmod = 0;

// Simple test command
static bool cb_test(int argc, char* argv[])
{
    _plugin_logprintf("[%s] Test command executed successfully!\n", PLUGIN_NAME);
    return true;
}

// Simple timestamp command
static bool cb_timestamp(int argc, char* argv[])
{
    // Get precise local time with microseconds
    FILETIME ftUtc;
    GetSystemTimePreciseAsFileTime(&ftUtc);

    // Convert to local time
    FILETIME ftLocal;
    FileTimeToLocalFileTime(&ftUtc, &ftLocal);

    // Convert to SYSTEMTIME for easy formatting
    SYSTEMTIME st;
    FileTimeToSystemTime(&ftLocal, &st);

    // Calculate microseconds from the FILETIME
    ULARGE_INTEGER uli;
    uli.LowPart = ftLocal.dwLowDateTime;
    uli.HighPart = ftLocal.dwHighDateTime;
    
    // Get microseconds within the current second
    unsigned long long ticks100ns = uli.QuadPart;
    unsigned long long microseconds = (ticks100ns % 10000000ULL) / 10ULL;
    
    // Split into milliseconds and remaining microseconds
    unsigned int ms = (unsigned int)(microseconds / 1000);
    unsigned int us = (unsigned int)(microseconds % 1000);

    char timestamp[80];
    sprintf_s(timestamp, sizeof(timestamp), "%04d-%02d-%02d %02d:%02d:%02d.%03d%03d",
              st.wYear, st.wMonth, st.wDay,
              st.wHour, st.wMinute, st.wSecond, ms, us);

    // Check if there's a label argument
    if (argc >= 2 && argv && argv[1] && strlen(argv[1]) > 0)
    {
        _plugin_logprintf("[%s] %s | %s\n", PLUGIN_NAME, timestamp, argv[1]);
    }
    else
    {
        _plugin_logprintf("[%s] %s\n", PLUGIN_NAME, timestamp);
    }
    
    return true;
}

// Plugin initialization
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
    if (!initStruct)
        return false;
        
    // Set the required fields that x64dbg expects
    initStruct->sdkVersion = PLUG_SDKVERSION;
    initStruct->pluginVersion = PLUGIN_VERSION;
    strcpy_s(initStruct->pluginName, sizeof(initStruct->pluginName), PLUGIN_NAME);
        
    pluginHandle = initStruct->pluginHandle;
    
    // Register commands
    _plugin_registercommand(pluginHandle, "testcmd", cb_test, false);
    _plugin_registercommand(pluginHandle, "timestamp", cb_timestamp, false);
    
    _plugin_logprintf("[%s] Plugin initialized\n", PLUGIN_NAME);
    _plugin_logprintf("[%s] Available commands:\n", PLUGIN_NAME);
    _plugin_logprintf("[%s]   testcmd - Test the plugin\n", PLUGIN_NAME);
    _plugin_logprintf("[%s]   timestamp [label] - Log current timestamp with microsecond precision\n", PLUGIN_NAME);
    return true;
}

// Plugin cleanup
bool pluginStop()
{
    _plugin_logprintf("[%s] Plugin stopped\n", PLUGIN_NAME);
    return true;
}

// Plugin setup (GUI)
void pluginSetup()
{
    // Nothing for now
}

// Required DLL exports
extern "C" __declspec(dllexport) bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    return pluginInit(initStruct);
}

extern "C" __declspec(dllexport) bool plugstop()
{
    return pluginStop();
}

extern "C" __declspec(dllexport) void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    if (setupStruct)
    {
        hwndDlg = setupStruct->hwndDlg;
        hMenu = setupStruct->hMenu;
        hMenuDisasm = setupStruct->hMenuDisasm;
        hMenuDump = setupStruct->hMenuDump;
        hMenuStack = setupStruct->hMenuStack;
        hMenuGraph = setupStruct->hMenuGraph;
        hMenuMemmap = setupStruct->hMenuMemmap;
        hMenuSymmod = setupStruct->hMenuSymmod;
    }
    
    pluginSetup();
}
