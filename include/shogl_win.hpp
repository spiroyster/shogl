#ifndef SHOGL_WIN_HPP
#define SHOGL_WIN_HPP

#include "shogl_delegate.hpp"

#pragma comment(lib, "opengl32.lib")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")

#include <Windows.h>
#include <gl/GL.h>
#include "glext.h"
#include "wglext.h"

// GL Function macros...
#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);
#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);

#define SHOGL_QUIT(exitCode) PostQuitMessage(exitCode);
#define SHOGL_CONTEXT dynamic_cast<win_context*>(shogl()->window_context());


class win_context : public shogl_delegate::context
{
    std::string get_last_error_as_string()
    {
        //Get the error message ID, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0) { return std::string(); }//No error message has been recorded
        LPSTR messageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        //Copy the error message into a std::string.
        std::string message(messageBuffer, size);

        //Free the Win32's string's buffer.
        LocalFree(messageBuffer);

        return message;
    }

    std::wstring ansi_to_wide(const std::string& str)
    {
        int count = MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), NULL, 0);
        std::wstring wstr(count, 0);
        MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), &wstr[0], count);
        return wstr;
    }

    HWND hWnd_;
    HGLRC hGLrc_;
    HINSTANCE hInst_;
    WNDCLASSEX wcex_;
    int nCmdShow_;

public:

    win_context(HINSTANCE hInstance, int nCmdShow)
        : nCmdShow_(nCmdShow)
    {
        CoInitialize(nullptr);

        wcex_.cbSize = sizeof(WNDCLASSEX);
        wcex_.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wcex_.lpfnWndProc = WndProc;
        wcex_.cbClsExtra = 0;
        wcex_.cbWndExtra = 0;
        wcex_.hInstance = hInst_;
        wcex_.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex_.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex_.lpszMenuName = NULL;
        wcex_.lpszClassName = L"glwindow";

        if (!RegisterClassEx(&wcex_))
            throw std::runtime_error("Unable to register Window class");

        hWnd_ = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW | WS_EX_ACCEPTFILES, wcex_.lpszClassName, L"glwindow", WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 320, 240, NULL, NULL, hInst_, NULL);

        if (!hWnd_)
            throw std::runtime_error(get_last_error_as_string());

        hInst_ = hInstance;

        HDC hDC = static_cast<HDC>(GetDC(hWnd_));
        if (!hDC)
            throw std::runtime_error("Unable to get handle device context.");

        // we then need to get a valid DC...
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 32;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int nPixelFormat = ChoosePixelFormat(hDC, &pfd);

        if (nPixelFormat == 0)
            throw std::runtime_error("Unable to choose pixel format.");

        if (!SetPixelFormat(hDC, nPixelFormat, &pfd))
            throw std::runtime_error("Unable to set pixel format.");

        hGLrc_ = wglCreateContext(hDC);
        wglMakeCurrent(hDC, hGLrc_);

        // Get a valid context for requested version...
        std::vector<std::vector<int>> gl_version = { { shogl_version_major, shogl_version_minor } };

        // If GL version is not set, attempt all...
        if (!shogl_version_major)
            gl_version =
        {
            { 4, 6 }, { 4, 5 }, { 4, 4 }, { 4, 3 }, { 4, 2 }, { 4, 1 }, { 4, 0 },
            { 3, 3 }, { 3, 2 }, { 3, 1 }, { 3, 0 },
            { 2, 2 }, { 2, 1 }, { 2, 0 },
            { 1, 5 }, { 1, 4 }, { 1, 3 }, { 1, 2 }, { 1, 1 }
        };

        PFNWGLCREATECONTEXTATTRIBSARBPROC glwindow_context_extended_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
        if (glwindow_context_extended_wglCreateContextAttribsARB)
        {
            auto tmpContext = hGLrc_;

            // Keep trying to make a valid context...
            hGLrc_ = NULL;
            for (unsigned int ver = 0; ver < gl_version.size() && !hGLrc_; ++ver)
            {
                int attribs[] =
                {
                    WGL_CONTEXT_MAJOR_VERSION_ARB, gl_version[ver][0],
                    WGL_CONTEXT_MINOR_VERSION_ARB, gl_version[ver][1],
                    WGL_CONTEXT_FLAGS_ARB, 0,
                    0
                };
                hGLrc_ = glwindow_context_extended_wglCreateContextAttribsARB(hDC, 0, attribs);
                major_ = gl_version[ver][0];
                minor_ = gl_version[ver][1];
            }

            if (!hGLrc_)
                throw std::runtime_error("Unable to make current.");

            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(tmpContext);

            if (!wglMakeCurrent(hDC, hGLrc_))
                throw std::runtime_error("Unable to make current.");
        }
    }

    void make_current(void* hdc) 
    { 
        wglMakeCurrent(static_cast<HDC>(hdc), hGLrc_); 
    }
    
    void* DC() 
    { 
        return GetDC(hWnd_); 
    }
    
    void* GLC() 
    { 
        return hGLrc_; 
    }
    
    void free() 
    { 
        wglDeleteContext(hGLrc_); 
    }
    
    void window_title(const std::string& title) 
    { 
        SetWindowText(hWnd_, ansi_to_wide(title).c_str()); 
    }
    
    void window_redraw()
    {
        InvalidateRect(hWnd_, NULL, NULL);
        UpdateWindow(hWnd_);
    }
    
    void window_size(int width, int height)
    {
        RECT rect;
        GetWindowRect(hWnd_, &rect);
        SetWindowPos(hWnd_, HWND_TOP, rect.left, rect.top, width, height, NULL);
    }
    
    void window_quit(int exit_code)
    {
        PostQuitMessage(exit_code);
    }

    int window_show()
    {
        try
        {
            auto shogl_window = shogl();

            // Set the window title...
            window_title(shogl_window->window_title());

            // Resize the window to desired width height...
            shogl_window->window_size(shogl_window->window_width(), shogl_window->window_height());

            // Main loop...
            MSG msg;
            ShowWindow(hWnd_, nCmdShow_);
            UpdateWindow(hWnd_);

            // Keep looping until exit code...
            while (!shogl_window->window_quit())
            {
                // Check the event mode for the window, this defines the loop behaviour...
                shogl_window::event_behaviour current_behaviour = shogl()->window_event_behaviour();
                if (current_behaviour == shogl_window::event_behaviour::peekEvent)
                {
                    while (!shogl_window->window_quit() && current_behaviour == shogl_window->window_event_behaviour())
                    {
                        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                        {
                            if (msg.message == WM_QUIT)
                                shogl_window->window_quit();

                            TranslateMessage(&msg);
                            DispatchMessage(&msg);
                        }

                        if (shogl_window->frame_limiter())
                            shogl_window->redraw();

                        shogl_window->idle();
                    }
                }
                else
                {
                    while (GetMessage(&msg, NULL, 0, 0) && !shogl_window->window_quit() && current_behaviour == shogl_window->window_event_behaviour())
                    {
                        // framerate limiter...
                        shogl_window->idle();
                        shogl_window->redraw();
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                    }
                }
            }

            // Kill
            shogl()->kill();
            free();

            return (int)msg.wParam;
        }
        catch (const std::exception& e) { MessageBox(NULL, ansi_to_wide(e.what()).c_str(), L"GLwindow", MB_ICONERROR); }
        catch (...) { MessageBox(NULL, L"Unknown error", L"GLwindow", MB_ICONERROR); }
        return 0;
    }
};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        shogl()->window_quit(0);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        shogl()->draw();
        SwapBuffers(GetDC(hWnd));
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_SIZE:
    {
        shogl()->window_size(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
        break;
    }
    case WM_MOUSEWHEEL:
    {
        POINT pt; pt.x = LOWORD(lParam); pt.y = HIWORD(lParam); ScreenToClient(hWnd, &pt);
        shogl()->mouse_scroll(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
        break;
    }
    case WM_MOUSEMOVE:
    {
        shogl()->mouse_move(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam));
        break;
    }
    case WM_LBUTTONDOWN:
    {
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::left);
        SetCapture(hWnd);
        break;
    }
    case WM_LBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        shogl()->mouse_up(x, shogl()->window_height() - y, shogl_window::mouse_button::left);
        ReleaseCapture();
        break;
    }
    case WM_MBUTTONDOWN:
    {
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::middle);
        SetCapture(hWnd);
        break;
    }
    case WM_MBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        shogl()->mouse_up(x, shogl()->window_height() - y, shogl_window::mouse_button::middle);
        ReleaseCapture();
        break;
    }
    case WM_RBUTTONDOWN:
    {
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::right);
        SetCapture(hWnd);
        break;
    }
    case WM_RBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        shogl()->mouse_up(x, shogl()->window_height() - y, shogl_window::mouse_button::right);
        ReleaseCapture();
        break;
    }
    case WM_KEYDOWN:
    {
        int ll = LOWORD(lParam); 
        int hl = HIWORD(lParam);
        int lw = LOWORD(wParam);
        int hw = HIWORD(wParam);
        POINT pt; GetCursorPos(&pt); ScreenToClient(hWnd, &pt);
        shogl()->key_down(pt.x, shogl()->window_height() - pt.y, static_cast<unsigned int>(wParam));
        break;
    }
    case WM_KEYUP:
    {
        POINT pt; GetCursorPos(&pt); ScreenToClient(hWnd, &pt);
        shogl()->key_up(pt.x, shogl()->window_height() - pt.y, static_cast<unsigned int>(wParam));
        break;
    }
    case WM_DROPFILES:
    {
        TCHAR szName[MAX_PATH];
        HDROP hDrop = (HDROP)wParam;
        int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

        std::vector<std::wstring> files;
        for (int i = 0; i < count; i++)
        {
            DragQueryFile(hDrop, i, szName, MAX_PATH);
            files.emplace_back(szName);
        }

        DragFinish(hDrop);
        shogl()->drag_drop(files);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


#define SHOGL_MAIN_IMPL int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \
{ \
    std::shared_ptr<win_context> ctx = std::make_shared<win_context>(hInstance, nCmdShow); \
    shogl()->window_context(ctx); \
    return shogl_main_impl(); \
}



#endif // SHOGL_WIN_HPP