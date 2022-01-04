#pragma once

#include <pwindow/Window.hpp>


namespace pwindow
{

class MainWindow : public pwindow::Window
{
public:

    MainWindow(const char* title, int x, int y, int width, int height) : pwindow::Window(title, x, y, width, height)
    {}

    virtual ~MainWindow()
    {
        int a = 0;
    }

    virtual void OnDraw()
    {

    }

    virtual void OnSize(int width, int height)
    {

    }

};//class Window


}//namespace vgui

