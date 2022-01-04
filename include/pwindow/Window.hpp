#pragma once

#include "Win32Window.hpp"


namespace pwindow
{

class Window : public Win32Window<Window>
{
public:

    Window(const char* title, int x, int y, int width, int height)
    {
        Win32Window<Window>::Create(title, x, y, width, height);
    }

    virtual ~Window()
    {
        Win32Window<Window>::Destroy();
    }

};//class Window


}//namespace vgui

