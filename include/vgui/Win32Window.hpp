#pragma once

#include <windows.h>
#include <dwmapi.h>

#include "Point.hpp"
#include "Size.hpp"
#include "Rect.hpp"
#include "Exception.hpp"-

#include <agg2d.h>
#include <platform/agg_platform_support.h>
#include <platform/win32/agg_platform_support.cpp>


namespace vgui
{

template<typename WindowT>
class Win32Window
{
public:

    Win32Window()
    {
        m_platform = new agg::platform_specific(m_pixformat, true);
    }

    ~Win32Window()
    {
        delete m_platform;
    }

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

    vgui::PointI Position()
    {
        RECT rect;
        ::GetWindowRect(m_handle, &rect);
        return vgui::PointI(rect.left, rect.top);
    }

    int Width()
    {
        return Size().width;
    }

    int Height()
    {
        return Size().height;
    }

    vgui::SizeI Size()
    {
        RECT rect;
        ::GetWindowRect(m_handle, &rect);
        return vgui::SizeI(rect.right - rect.left, rect.bottom - rect.top);
    }

    vgui::RectI Rect()
    {
        RECT rect;
        ::GetWindowRect(m_handle, &rect);
        return vgui::RectI(rect.left, rect.top, rect.right, rect.bottom);
    }

    bool Visible()
    {
        return static_cast<bool>(::IsWindowVisible(m_handle));
    }

    WindowT& SetX(int x)
    {
        vgui::RectI rect = Rect();
        return SetRect(x, rect.y, rect.width, rect.height);
    }

    WindowT& SetY(int y)
    {
        vgui::RectI rect = Rect();
        return SetRect(rect.x, y, rect.width, rect.height);
    }

    WindowT& SetPosition(int x, int y)
    {
        vgui::SizeI size = Size();
        return SetRect(x, y, size.width, size.height);
    }

    WindowT& SetPosition(Point position)
    {
        vgui::SizeI size = Size();
        return SetRect(position.x, position.y, size.width, size.height);
    }

    WindowT& SetWidth(int width)
    {
        vgui::RectI rect = Rect();
        return SetRect(rect.x, rect.y, width, rect.height);
    }

    WindowT& SetHeight(int height)
    {
        vgui::RectI rect = Rect();
        return SetRect(rect.x, rect.y, rect.width, height);
    }

    WindowT& SetSize(int width, int height)
    {
        vgui::PointI pos = Position();
        return SetRect(pos.x, pos.y, width, height);
    }

    WindowT& SetSize(vgui::SizeI size)
    {
        vgui::PointI pos = Position();
        return SetRect(pos.x, pos.y, size.width, size.height);
    }

    WindowT& SetRect(vgui::RectI rect)
    {
        return SetRect(rect.x, rect.y, rect.width, rect.height);
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
        return dynamic_cast<WindowT*>(this);
    }

    virtual void OnIdle()
    {
         OnDraw();
    }

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
                m_graphics.attach(m_renbuff.buf(), m_renbuff.width(), m_renbuff.height(), m_renbuff.stride());
                m_graphics.clearAll(255,255,255);
                //m_graphics.blendMode(Agg2D::BlendAdd);

                m_graphics.antiAliasGamma(1.4);

                // Set flipText(true) if you have the Y axis upside down.
                //m_graphics.flipText(true);


                // ClipBox.
                //m_graphics.clipBox(50, 50, rbuf_window().width() - 50, rbuf_window().height() - 50);

                // Transfornations - Rotate around (300,300) to 5 degree
                //m_graphics.translate(-300, -300);
                //m_graphics.rotate(Agg2D::deg2Rad(5.0));
                //m_graphics.translate(300, 300);

                // Viewport - set 0,0,600,600 to the actual window size
                // preserving aspect ratio and placing the viewport in the center.
                // To ignore aspect ratio use Agg2D::Anisotropic
                // Note that the viewport just adds transformations to the current
                // affine matrix. So that, set the viewport *after* all transformations!
        /*        m_graphics.viewport(0, 0, 600, 600,
                                    0, 0, width, height,
                                    //Agg2D::Anisotropic);
                                    Agg2D::XMidYMid);

                m_graphics.attach(rbuf_window().buf(),
                                  rbuf_window().width(),
                                  rbuf_window().height(),
                                  rbuf_window().stride());
        */
                //m_graphics.clearAll(0, 0, 0);

                //m_graphics.blendMode(Agg2D::BlendSub);
                //m_graphics.blendMode(Agg2D::BlendAdd);

                m_graphics.antiAliasGamma(1.4);

                // Set flipText(true) if you have the Y axis upside down.
                //m_graphics.flipText(true);


                // ClipBox.
                //m_graphics.clipBox(50, 50, rbuf_window().width() - 50, rbuf_window().height() - 50);

                // Transfornations - Rotate around (300,300) to 5 degree
                //m_graphics.translate(-300, -300);
                //m_graphics.rotate(Agg2D::deg2Rad(5.0));
                //m_graphics.translate(300, 300);

                // Viewport - set 0,0,600,600 to the actual window size
                // preserving aspect ratio and placing the viewport in the center.
                // To ignore aspect ratio use Agg2D::Anisotropic
                // Note that the viewport just adds transformations to the current
                // affine matrix. So that, set the viewport *after* all transformations!
        //        m_graphics.viewport(0, 0, 600, 600,
        //                            0, 0, width, height,
        //                            //Agg2D::Anisotropic);
        //                            Agg2D::XMidYMid);


                // Rounded Rect
                m_graphics.lineColor(0, 0, 0);
                m_graphics.noFill();
                m_graphics.roundedRect(0.5, 0.5, 600-0.5, 600-0.5, 20.0);


                // Reglar Text
                m_graphics.font("Times New Roman", 14.0, false, false);
                m_graphics.fillColor(0, 0, 0);
                m_graphics.noLine();
                m_graphics.text(100, 20, "Regular Raster Text -- Fast, but can't be rotated");

                // Outlined Text
                m_graphics.font("Times New Roman", 50.0, false, false, Agg2D::VectorFontCache);
                m_graphics.lineColor(50, 0, 0);
                m_graphics.fillColor(180, 200, 100);
                m_graphics.lineWidth(1.0);
                m_graphics.text(100.5, 50.5, "Outlined Text");

                // Text Alignment
                m_graphics.line(250.5-150, 150.5,    250.5+150, 150.5);
                m_graphics.line(250.5,     150.5-20, 250.5,     150.5+20);
                m_graphics.line(250.5-150, 200.5,    250.5+150, 200.5);
                m_graphics.line(250.5,     200.5-20, 250.5,     200.5+20);
                m_graphics.line(250.5-150, 250.5,    250.5+150, 250.5);
                m_graphics.line(250.5,     250.5-20, 250.5,     250.5+20);
                m_graphics.line(250.5-150, 300.5,    250.5+150, 300.5);
                m_graphics.line(250.5,     300.5-20, 250.5,     300.5+20);
                m_graphics.line(250.5-150, 350.5,    250.5+150, 350.5);
                m_graphics.line(250.5,     350.5-20, 250.5,     350.5+20);
                m_graphics.line(250.5-150, 400.5,    250.5+150, 400.5);
                m_graphics.line(250.5,     400.5-20, 250.5,     400.5+20);
                m_graphics.line(250.5-150, 450.5,    250.5+150, 450.5);
                m_graphics.line(250.5,     450.5-20, 250.5,     450.5+20);
                m_graphics.line(250.5-150, 500.5,    250.5+150, 500.5);
                m_graphics.line(250.5,     500.5-20, 250.5,     500.5+20);
                m_graphics.line(250.5-150, 550.5,    250.5+150, 550.5);
                m_graphics.line(250.5,     550.5-20, 250.5,     550.5+20);


                m_graphics.fillColor(100, 50, 50);
                m_graphics.noLine();
                //m_graphics.textHints(false);
                m_graphics.font("Times New Roman", 40.0, false, false, Agg2D::VectorFontCache);

                m_graphics.textAlignment(Agg2D::AlignLeft, Agg2D::AlignBottom);
                m_graphics.text(250.0,     150.0, "Left-Bottom", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignCenter, Agg2D::AlignBottom);
                m_graphics.text(250.0,     200.0, "Center-Bottom", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignRight, Agg2D::AlignBottom);
                m_graphics.text(250.0,     250.0, "Right-Bottom", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignLeft, Agg2D::AlignCenter);
                m_graphics.text(250.0,     300.0, "Left-Center", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignCenter, Agg2D::AlignCenter);
                m_graphics.text(250.0,     350.0, "Center-Center", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignRight, Agg2D::AlignCenter);
                m_graphics.text(250.0,     400.0, "Right-Center", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignLeft, Agg2D::AlignTop);
                m_graphics.text(250.0,     450.0, "Left-Top", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignCenter, Agg2D::AlignTop);
                m_graphics.text(250.0,     500.0, "Center-Top", true, 0, 0);

                m_graphics.textAlignment(Agg2D::AlignRight, Agg2D::AlignTop);
                m_graphics.text(250.0,     550.0, "Right-Top", true, 0, 0);


                // Gradients (Aqua Buttons)
                //=======================================
                m_graphics.font("Verdana", 20.0, false, false, Agg2D::VectorFontCache);
                double xb1 = 400;
                double yb1 = 80;
                double xb2 = xb1 + 150;
                double yb2 = yb1 + 36;

                m_graphics.fillColor(Agg2D::Color(0,50,180,180));
                m_graphics.lineColor(Agg2D::Color(0,0,80, 255));
                m_graphics.lineWidth(1.0);
                m_graphics.roundedRect(xb1, yb1, xb2, yb2, 12, 18);

                m_graphics.lineColor(Agg2D::Color(0,0,0,0));
                m_graphics.fillLinearGradient(xb1, yb1, xb1, yb1+30,
                                              Agg2D::Color(100,200,255,255),
                                              Agg2D::Color(255,255,255,0));
                m_graphics.roundedRect(xb1+3, yb1+2.5, xb2-3, yb1+30, 9, 18, 1, 1);

                m_graphics.fillColor(Agg2D::Color(0,0,50, 200));
                m_graphics.noLine();
                m_graphics.textAlignment(Agg2D::AlignCenter, Agg2D::AlignCenter);
                m_graphics.text((xb1 + xb2) / 2.0, (yb1 + yb2) / 2.0, "Aqua Button", true, 0.0, 0.0);

                m_graphics.fillLinearGradient(xb1, yb2-20, xb1, yb2-3,
                                              Agg2D::Color(0,  0,  255,0),
                                              Agg2D::Color(100,255,255,255));
                m_graphics.roundedRect(xb1+3, yb2-20, xb2-3, yb2-2, 1, 1, 9, 18);


                // Aqua Button Pressed
                xb1 = 400;
                yb1 = 30;
                xb2 = xb1 + 150;
                yb2 = yb1 + 36;

                m_graphics.fillColor(Agg2D::Color(0,50,180,180));
                m_graphics.lineColor(Agg2D::Color(0,0,0,  255));
                m_graphics.lineWidth(2.0);
                m_graphics.roundedRect(xb1, yb1, xb2, yb2, 12, 18);

                m_graphics.lineColor(Agg2D::Color(0,0,0,0));
                m_graphics.fillLinearGradient(xb1, yb1+2, xb1, yb1+25,
                                              Agg2D::Color(60, 160,255,255),
                                              Agg2D::Color(100,255,255,0));
                m_graphics.roundedRect(xb1+3, yb1+2.5, xb2-3, yb1+30, 9, 18, 1, 1);

                m_graphics.fillColor(Agg2D::Color(0,0,50, 255));
                m_graphics.noLine();
                m_graphics.textAlignment(Agg2D::AlignCenter, Agg2D::AlignCenter);
                m_graphics.text((xb1 + xb2) / 2.0, (yb1 + yb2) / 2.0, "Aqua Pressed", 0.0, 0.0);

                m_graphics.fillLinearGradient(xb1, yb2-25, xb1, yb2-5,
                                              Agg2D::Color(0,  180,255,0),
                                              Agg2D::Color(0,  200,255,255));
                m_graphics.roundedRect(xb1+3, yb2-25, xb2-3, yb2-2, 1, 1, 9, 18);




                // Basic Shapes -- Ellipse
                //===========================================
                m_graphics.lineWidth(3.5);
                m_graphics.lineColor(20,  80,  80);
                m_graphics.fillColor(200, 255, 80, 200);
                m_graphics.ellipse(450, 200, 50, 90);


                // Paths
                //===========================================
                m_graphics.resetPath();
                m_graphics.fillColor(255, 0, 0, 100);
                m_graphics.lineColor(0, 0, 255, 100);
                m_graphics.lineWidth(2);
                m_graphics.moveTo(300/2, 200/2);
                m_graphics.horLineRel(-150/2);
                m_graphics.arcRel(150/2, 150/2, 0, 1, 0, 150/2, -150/2);
                m_graphics.closePolygon();
                m_graphics.drawPath();

                m_graphics.resetPath();
                m_graphics.fillColor(255, 255, 0, 100);
                m_graphics.lineColor(0, 0, 255, 100);
                m_graphics.lineWidth(2);
                m_graphics.moveTo(275/2, 175/2);
                m_graphics.verLineRel(-150/2);
                m_graphics.arcRel(150/2, 150/2, 0, 0, 0, -150/2, 150/2);
                m_graphics.closePolygon();
                m_graphics.drawPath();


                m_graphics.resetPath();
                m_graphics.noFill();
                m_graphics.lineColor(127, 0, 0);
                m_graphics.lineWidth(5);
                m_graphics.moveTo(600/2, 350/2);
                m_graphics.lineRel(50/2, -25/2);
                m_graphics.arcRel(25/2, 25/2, Agg2D::deg2Rad(-30), 0, 1, 50/2, -25/2);
                m_graphics.lineRel(50/2, -25/2);
                m_graphics.arcRel(25/2, 50/2, Agg2D::deg2Rad(-30), 0, 1, 50/2, -25/2);
                m_graphics.lineRel(50/2, -25/2);
                m_graphics.arcRel(25/2, 75/2, Agg2D::deg2Rad(-30), 0, 1, 50/2, -25/2);
                m_graphics.lineRel(50, -25);
                m_graphics.arcRel(25/2, 100/2, Agg2D::deg2Rad(-30), 0, 1, 50/2, -25/2);
                m_graphics.lineRel(50/2, -25/2);
                m_graphics.drawPath();


                // Master Alpha. From now on everything will be translucent
                //===========================================
                m_graphics.masterAlpha(0.85);


                // Image Transformations
                //===========================================
        //        Agg2D::Image img(rbuf_img(0).buf(),
        //                         rbuf_img(0).width(),
        //                         rbuf_img(0).height(),
        //                         rbuf_img(0).stride());
        //        m_graphics.imageFilter(Agg2D::Bilinear);

                //m_graphics.imageResample(Agg2D::NoResample);
                //m_graphics.imageResample(Agg2D::ResampleAlways);
        //        m_graphics.imageResample(Agg2D::ResampleOnZoomOut);

                // Set the initial image blending operation as BlendDst, that actually
                // does nothing.
                //-----------------
        //        m_graphics.imageBlendMode(Agg2D::BlendDst);


                // Transform the whole image to the destination rectangle
                //-----------------
                //m_graphics.transformImage(img, 450, 200, 595, 350);

                // Transform the rectangular part of the image to the destination rectangle
                //-----------------
                //m_graphics.transformImage(img, 60, 60, img.width()-60, img.height()-60,
                //                          450, 200, 595, 350);

                // Transform the whole image to the destination parallelogram
                //-----------------
                //double parl[6] = { 450, 200, 595, 220, 575, 350 };
                //m_graphics.transformImage(img, parl);

                // Transform the rectangular part of the image to the destination parallelogram
                //-----------------
                //double parl[6] = { 450, 200, 595, 220, 575, 350 };
                //m_graphics.transformImage(img, 60, 60, img.width()-60, img.height()-60, parl);

                // Transform image to the destination path. The scale is determined by a rectangle
                //-----------------
                //m_graphics.resetPath();
                //m_graphics.moveTo(450, 200);
                //m_graphics.cubicCurveTo(595, 220, 575, 350, 595, 350);
                //m_graphics.lineTo(470, 340);
                //m_graphics.transformImagePath(img, 450, 200, 595, 350);


                // Transform image to the destination path.
                // The scale is determined by a rectangle
                //-----------------
                m_graphics.resetPath();
                m_graphics.moveTo(450, 200);
                m_graphics.cubicCurveTo(595, 220, 575, 350, 595, 350);
                m_graphics.lineTo(470, 340);
                //m_graphics.transformImagePath(img, 60, 60, img.width()-60, img.height()-60,
                                              //450, 200, 595, 350);

                // Transform image to the destination path.
                // The transformation is determined by a parallelogram
                //m_graphics.resetPath();
                //m_graphics.moveTo(450, 200);
                //m_graphics.cubicCurveTo(595, 220, 575, 350, 595, 350);
                //m_graphics.lineTo(470, 340);
                //double parl[6] = { 450, 200, 595, 220, 575, 350 };
                //m_graphics.transformImagePath(img, parl);

                // Transform the rectangular part of the image to the destination path.
                // The transformation is determined by a parallelogram
                //m_graphics.resetPath();
                //m_graphics.moveTo(450, 200);
                //m_graphics.cubicCurveTo(595, 220, 575, 350, 595, 350);
                //m_graphics.lineTo(470, 340);
                //double parl[6] = { 450, 200, 595, 220, 575, 350 };
                //m_graphics.transformImagePath(img, 60, 60, img.width()-60, img.height()-60, parl);


                // Add/Sub/Contrast Blending Modes
                m_graphics.noLine();
                m_graphics.fillColor(70, 70, 0);
                m_graphics.blendMode(Agg2D::BlendAdd);
                m_graphics.ellipse(500, 280, 20, 40);

                m_graphics.fillColor(255, 255, 255);
        //        m_graphics.blendMode(Agg2D::BlendContrast);
                m_graphics.ellipse(500+40, 280, 20, 40);



                // Radial gradient.
                m_graphics.blendMode(Agg2D::BlendAlpha);
                m_graphics.fillRadialGradient(400, 500, 40,
                                              Agg2D::Color(255, 255, 0, 0),
                                              Agg2D::Color(0, 0, 127),
                                              Agg2D::Color(0, 255, 0, 0));
                m_graphics.ellipse(400, 500, 40, 40);

                m_platform->display_pmap(dc, &m_renbuff);

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
                m_platform->create_pmap(width, height, &m_renbuff);
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

        OnIdle();
        return false;
    }

    static LRESULT CALLBACK StaticProcessEvent(HWND handle, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if(msg == WM_CREATE)
        {
            LONG_PTR win_ptr = LONG_PTR(LPCREATESTRUCT(lparam)->lpCreateParams);

            if(win_ptr)
                SetWindowLongPtr(handle, GWLP_USERDATA, win_ptr);
            else
                throw ExceptionInfo << "NULL Win32Window pointer in CreateWin32Window function.";

            return 0;
        }
        else if(msg == WM_DESTROY)
        {
            PostQuitMessage(0);
            return 0;
        }
        else if(msg == WM_NCCREATE)
        {
            return ::DefWindowProc(handle, msg, wparam, lparam);
        }

        Win32Window* window = (Win32Window*)::GetWindowLongPtr(handle, GWLP_USERDATA);
        LRESULT result = 0;

        if(window)
        {
            if(window->ProcessEvent(handle, msg, wparam, lparam, result))
                return result;
        }

        //Do not call DefWindowProc from Win32Window::ProcessEvent,  this will cause an OS error.
        return ::DefWindowProc(handle, msg, wparam, lparam);
    }

    static int ProcessEvents()
    {
        MSG msg;
        BOOL ret = FALSE;

        while((ret = GetMessage( &msg, NULL, 0, 0 )) != 0)
        {
            if(ret == -1)
                throw ExceptionInfo << "Win32 event loop error.";
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
            throw ExceptionInfo << "Wind32 window handle is not NULL. Maybe window created ?.";

        std::wstring classname = L"vgu::iWin32Window";
        classname += std::to_wstring(reinterpret_cast<int64_t>(this));
        //std::wcout << classname << std::endl;

        HINSTANCE hinstance = (HINSTANCE)::GetModuleHandle(NULL);

        WNDCLASSEXW wndclassex = {0};
        wndclassex.cbSize = sizeof(WNDCLASSEXW);
        wndclassex.lpfnWndProc = Win32Window::StaticProcessEvent;
        wndclassex.style = CS_DBLCLKS |CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
        wndclassex.hInstance = hinstance;
        wndclassex.lpszClassName = classname.c_str();
        wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);

        if(!::RegisterClassExW(&wndclassex))
            throw ExceptionInfo << "Can't register Win32 window class.";

        //::DwmEnableComposition(DWM_EC_DISABLECOMPOSITION);

        std::wstring wtitle = vgui::Utf8ToWstring(title);
        m_handle = ::CreateWindowExW(0, classname.c_str(), wtitle.c_str(), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                                    x, y, width, height, NULL, NULL, hinstance, this);

        if(!m_handle)
            throw ExceptionInfo << "Can't create Win32 window.";

        HDC hdc = ::GetDC(m_handle);
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
            m_handle = NULL;
        }
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

    HWND m_handle{NULL};
    agg::pix_format_e m_pixformat {agg::pix_format_rgba32};
    agg::platform_specific* m_platform = nullptr;
    agg::rendering_buffer m_renbuff;
    Agg2D m_graphics;

    static inline std::vector<WindowT*> s_windows;
};


}//namespace vgui

