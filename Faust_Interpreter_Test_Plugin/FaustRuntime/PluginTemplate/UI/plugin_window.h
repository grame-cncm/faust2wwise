#ifndef PLUGIN_WINDOW_H
#define PLUGIN_WINDOW_H


#include "Parameters/parameter.h"
#include "PluginInfo/plugin_config.h"
#define _AFXDLL
#include <afxwin.h>
#include <windows.h>
#include <unordered_map>

#define DEFAULT_Y_SPAN 25
#define DEFAULT_X_MARGIN 20
#define SLIDER_RESOLUTION 1000 // for increasing Slider resolution

struct SliderVal
{
    HWND hWnd;
    float minVal;
    float maxVal;
    float step;
    std::atomic<float>* value;
};

class PluginWindow
{
public:
    PluginWindow(HWND, PluginConfiguration&, ParameterList&);

    ~PluginWindow();

    bool isWindowCreated();

    void Show();

    bool isActive();

    void Update();

    void Close();

private:

    HWND hwnd, parentWnd;
    ParameterList& parameters;
    PluginConfiguration& cfg;

    bool WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool createWindow();
    bool createUIcontrols();
    static bool ProcessMessages();
    bool CreateButton(Parameter&);
    bool CreateCheckbox(Parameter&);
    bool CreateSlider(Parameter&);
    bool CreateMonitor(Parameter& p);
    void UpdateMonitoringValues();
    void configureWindowScrolling();

    std::unordered_map<int, SliderVal> sliderValues;
    std::vector<std::pair<HWND, std::atomic<float>*>> monitorValues;
    std::unordered_map<int, std::atomic<float>*> checkBoxValues;

    //ui config
    bool windowCreated;
    const int y_span, x_margin;
    int curr_height;
    const wchar_t* windowTitle;
};


#endif