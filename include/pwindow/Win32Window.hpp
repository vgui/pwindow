#pragma once

#include <windows.h>
#include <dwmapi.h>
#include <algorithm>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <cstring>
#include <string>


namespace pwindow
{

template<typename WindowT>
class Win32Window
{
public:

    Win32Window()
    {}

    ~Win32Window()
    {}

    HWND Handle()
    {
        return m_handle;
    }

    bool IsValid()
    {
        if(::IsWindow(m_handle))
            return true;
        else
            return false;
    }

    void Close()
    {
        ::PostQuitMessage(0);
    }

    int X()
    {
        return Position().x;
    }

    int Y()
    {
        return Position().y;
    }

    auto Position()
    {
        RECT rect;
        ::GetWindowRect(m_handle, &rect);
        return std::make_tuple(rect.left, rect.top);
    }

    int Width()
    {
        auto size = Size();
        return std::get<0>(size);
    }

    int Height()
    {
        auto size = Size();
        return std::get<1>(size);
    }

    auto Size()
    {
        RECT rect;
        ::GetWindowRect(m_handle, &rect);
        return std::make_tuple(rect.right - rect.left, rect.bottom - rect.top);
    }

    auto Rect()
    {
        RECT rect;
        ::GetWindowRect(m_handle, &rect);
        return std::make_tuple(rect.left, rect.top, rect.right, rect.bottom);
    }

    bool Visible()
    {
        return static_cast<bool>(::IsWindowVisible(m_handle));
    }

    WindowT& SetX(int x)
    {
        auto rect = Rect();
        return SetRect(x, std::get<1>(rect), std::get<2>(rect), std::get<3>(rect));
    }

    WindowT& SetY(int y)
    {
        auto rect = Rect();
        return SetRect(std::get<0>(rect), y, std::get<2>(rect), std::get<3>(rect));
    }

    WindowT& SetPosition(int x, int y)
    {
        auto size = Size();
        return SetRect(x, y, size.width, size.height);
    }

    WindowT& SetWidth(int width)
    {
        auto rect = Rect();
        return SetRect(std::get<0>(rect), std::get<1>(rect), width, std::get<3>(rect));
    }

    WindowT& SetHeight(int height)
    {
        auto rect = Rect();
        return SetRect(std::get<0>(rect), std::get<1>(rect), std::get<2>(rect), height);
    }

    WindowT& SetSize(int width, int height)
    {
        auto pos = Position();
        return SetRect(std::get<0>(pos), std::get<2>(pos), width, height);
    }

    WindowT& SetRect(int x, int y, int width, int height)
    {
        ::MoveWindow(m_handle, x, y, width, height, true);
        return *CastThis();
    }

    WindowT& SetVisible(bool show)
    {
        if(show)
            ::ShowWindow(m_handle, SW_SHOW);
        else
            ::ShowWindow(m_handle, SW_HIDE);

        return *CastThis();
    }

    int Run()
    {
        while(s_windows.size())
        {
            for(auto window : s_windows)
                window->ProcessEvents();

            for(auto window : s_windows)
                window->OnIdle();
        }

        return 0;
    }

protected:

    WindowT* CastThis()
    {
        return static_cast<WindowT*>(this);
    }

    virtual void OnIdle(){}
    virtual void OnDraw(){}
    virtual void OnSize(int width, int height){}
    virtual void OnLostFocus(){}
    virtual void OnGainedFocus(){}
    virtual void OnTextEnter(){}
    virtual void OnKeyPress(){}
    virtual void OnKeyRelease(){}
    virtual void OnMouseWheelMove(){}
    virtual void OnMouseWheelScroll(){}
    virtual void OnMouseButtonPress(){}
    virtual void OnMouseButtonRelease(){}
    virtual void OnMouseMove(){}
    virtual void OnMouseEnter(){}
    virtual void OnMouseLeft(){}
    virtual void OnJoystickButtonPress(){}
    virtual void OnJoystickButtonRelease(){}
    virtual void OnJoystickMove(){}
    virtual void OnJoystickConnect(){}
    virtual void OnJoystickDisconnect(){}
    virtual void OnTouchBegin(){}
    virtual void OnTouchMove(){}
    virtual void OnTouchEnd(){}
    virtual void OnSensorChange(){}
    virtual void OnClose(bool& close){}

    virtual bool ProcessEvent(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam, LRESULT& result)
    {
        if(handle == m_handle)
        {
            if(msg == WM_PAINT)
            {
                PAINTSTRUCT ps;
                HDC dc = ::BeginPaint(m_handle, &ps);

                OnDraw();

                ::EndPaint(m_handle, &ps);
                result = 0;
                return true;
            }
            else if(msg == WM_ERASEBKGND)
            {
                result = 1;
                return true;
            }
            else if(msg == WM_SIZE)
            {
                int width = LOWORD(lparam);
                int height = HIWORD(lparam);
                OnSize(width, height);
                result = 0;
                return true;
            }
            else if(msg == WM_CLOSE)
            {
                bool close = true;
                OnClose(close);

                if(close)
                    Destroy();

                result = 0;
                return true;
            }
        }

        return false;
    }

    static LRESULT StaticProcessEvent(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if(msg == WM_CREATE)
        {
            LONG_PTR win_ptr = LONG_PTR(LPCREATESTRUCT(lparam)->lpCreateParams);

            if(win_ptr)
                SetWindowLongPtr(handle, GWLP_USERDATA, win_ptr);
            else
                throw std::runtime_error("Window pointer is NULL in StaticProcessEvent function.");

            return 0;
        }
        else if(msg == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0;
        }

        Win32Window* window = (Win32Window*)::GetWindowLongPtr(handle, GWLP_USERDATA);
        LRESULT result = 0;

        if(window)
        {
            if(window->ProcessEvent(handle, msg, wparam, lparam, result))
                return result;
        }

        //Do not call DefWindowProc from Win32Window::ProcessEvent,  this will cause an OS error.
        return ::DefWindowProcW(handle, msg, wparam, lparam);
    }

    static int ProcessEvents()
    {
        MSG msg;
        BOOL ret = FALSE;

        while((ret = GetMessage( &msg, nullptr, 0, 0 )) != 0)
        {
            if(ret == -1)
                throw std::runtime_error("Win32 event loop error.");
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        return msg.wParam;
    }

    virtual void Create(const char* title, int x, int y, int width, int height)
    {
        if(m_handle)
            throw std::runtime_error("Wind32 window handle is not NULL. Maybe window created ?.");

        if(s_windows.size() == 0)
        {
            s_classname = L"pwindow::Win32Window";
            s_classname += std::to_wstring(reinterpret_cast<int64_t>(this));
            //std::wcout << classname << std::endl;
            s_hinstance = (HINSTANCE)::GetModuleHandle(nullptr);
        }

        WNDCLASSEXW wndclassex = {0};
        wndclassex.cbSize = sizeof(WNDCLASSEXW);
        wndclassex.lpfnWndProc = &Win32Window::StaticProcessEvent;
        wndclassex.style = CS_DBLCLKS |CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
        wndclassex.hInstance = s_hinstance;
        wndclassex.lpszClassName = s_classname.c_str();
        wndclassex.hCursor = LoadCursor(nullptr, IDC_ARROW);

        if(!s_windows.size() && !::RegisterClassExW(&wndclassex))
            throw std::runtime_error("Can't register Win32 window class.");

        //::DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);

        std::wstring wtitle = Utf8ToWstring(title);
        m_handle = ::CreateWindowExW(0, s_classname.c_str(), wtitle.c_str(), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                    x, y, width, height, nullptr, nullptr, s_hinstance, this);

        if(!m_handle)
            throw std::runtime_error("Can't create Win32 window.");

        //HDC hdc = ::GetDC(m_handle);
        ::ShowWindow(m_handle, SW_SHOW);
        ::UpdateWindow(m_handle);

        AddToEventLoop(CastThis());
    }

    virtual void Destroy()
    {
        if(IsValid())
        {
            RemoveFromEventLoop(CastThis());
            ::DestroyWindow(m_handle);
            m_handle = nullptr;

            if(s_windows.size() == 0)
            {
                ::UnregisterClassW(s_classname.c_str(), s_hinstance);
                s_classname = L"";
                s_hinstance = nullptr;
            }
        }
    }

    static std::wstring Utf8ToWstring(const char* str)
    {
        size_t length = std::strlen(str);

        if(length != 0)
        {
            int num_chars = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, length, nullptr, 0);
            std::wstring wstrTo;
            if(num_chars)
            {
                wstrTo.resize(num_chars);
                if(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str, length, &wstrTo[0], num_chars))
                {
                    return wstrTo;
                }
            }
        }

        return std::wstring();
    }

private:

    static void AddToEventLoop(WindowT* window)
    {
        auto i = std::find(s_windows.begin(), s_windows.end(), window);

        if( i== s_windows.end())
            s_windows.push_back(window);
    }

    static void RemoveFromEventLoop(WindowT* window)
    {
        auto i = std::find(s_windows.begin(), s_windows.end(), window);

        if( i!= s_windows.end())
            s_windows.erase(i);
    }

    HWND m_handle{nullptr};
    static inline std::vector<WindowT*> s_windows;
    static inline HINSTANCE s_hinstance{nullptr};
    static inline std::wstring s_classname;
};


}//namespace vgui

