#ifndef SHOGL_HPP
#define SHOGL_HPP

#include <functional>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>

// Forward declarations...
void shogl_init_impl();
extern int shogl_version_major();
extern int shogl_version_minor();
class shogl_window; 
extern shogl_window* shogl();

// Platform macros...
#ifndef SHOGL_WIN
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define SHOGL_WIN
#endif
#endif

#ifndef SHOGL_X
#if defined(__linux__) || defined(__unix__)
#define SHOGL_X
#endif
#endif


#ifdef SHOGL_X

#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>

// GL Function macros...
#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))glXGetProcAddress((const GLubyte *)#name);
#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))glXGetProcAddress((const GLubyte *)#name);

typedef std::string shogl_str;

#endif // SHOGL_X


#ifdef SHOGL_WIN

#include <Windows.h>
#include <gl/GL.h>
#include "glext.h"
#include "wglext.h"

// GL Function macros...
#define GLFN_PROTOTYPE(prototype) PFN ## prototype ## PROC
#define GLFN_DECLARE(prototype, name) GLFN_PROTOTYPE(prototype) name;
#define GLFN_DEFINE(prototype, name) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);
#define GLFN(prototype, name) GLFN_PROTOTYPE(prototype) name = (GLFN_PROTOTYPE(prototype))wglGetProcAddress(#name);

typedef std::string shogl_str;

#endif


// Macro...
#define SHOGL(shogl_window_class) SHOGL_IMPL \
namespace { static std::unique_ptr<shogl_window_class> instance_; } \
shogl_window* shogl() { if (!instance_) { instance_ = std::make_unique<shogl_window_class>(); } return instance_.get(); } \
void shogl_init_impl()

// The windowing interface...
class shogl_window
{
    shogl_str window_title_;
    int window_width_ = 300;
    int window_height_ = 300;
    bool fullscreen_ = false;
    int target_fps_ = 0;
    int click_duration_ = 200;
public:
    
    enum mouse_button
    {
        none = 0,
        left,
        middle,
        right
    };

    class context 
    {
    protected:
        int major_;
        int minor_;
    public:
        virtual void make_current(void*) = 0;
        virtual void free() = 0;

        int major() const { return major_; }
        int minor() const { return minor_; }
    };

    typedef std::function<void()> idle_fn;
    typedef std::function<void()> draw_fn;
    typedef std::function<void()> kill_fn;
    typedef std::function<void(int, int)> resize_fn;
    typedef std::function<void(int, int)> mouse_move_fn;
    typedef std::function<void(int, int, mouse_button)> mouse_down_fn;
    typedef std::function<void(int, int, mouse_button)> mouse_up_fn;
    typedef std::function<void(int, int, mouse_button)> mouse_click_fn;
    typedef std::function<void(int, int, int)> mouse_scroll_fn;
    typedef std::function<void(int, int, unsigned int)> key_up_fn;
    typedef std::function<void(int, int, unsigned int)> key_down_fn;
    typedef std::function<void(std::vector<std::wstring>)> drag_drop_fn;
    
private:
    idle_fn idle_fn_ = nullptr;
    draw_fn draw_fn_ = nullptr;
    kill_fn kill_fn_ = nullptr;
    resize_fn resize_fn_ = nullptr;
    mouse_move_fn mouse_move_fn_ = nullptr;
    mouse_up_fn mouse_up_fn_ = nullptr;
    mouse_click_fn mouse_click_fn_ = nullptr;
    mouse_down_fn mouse_down_fn_ = nullptr;
    mouse_scroll_fn mouse_scroll_fn_ = nullptr;
    key_up_fn key_up_fn_ = nullptr;
    key_down_fn key_down_fn_ = nullptr;
    drag_drop_fn drag_drop_fn_ = nullptr;
    std::shared_ptr<context> context_;

    std::chrono::steady_clock::time_point fps_time_stamp_ = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point click_duration_time_stamp_ = std::chrono::steady_clock::now();

public:
    shogl_window() {}
    
    shogl_window(const shogl_str& title)
        : window_title_(title)
    {
    }

    shogl_window(const shogl_str& title, int width, int height)
        : window_title_(title), window_width_(width), window_height_(height)
    {
    }

    shogl_window(const shogl_str& title, int width, int height, bool fullscreen)
        :   window_title_(title), window_width_(width), window_height_(height), fullscreen_(fullscreen)
    {
    }

    // Events...
    void idle(idle_fn fn) { idle_fn_ = fn; }
    virtual void idle() { idle_fn_ ? idle_fn_() : (void)0; }
    
    void draw(draw_fn fn) { draw_fn_ = fn; }
    virtual void draw() { draw_fn_ ? draw_fn_() : (void)0; }
    
    void kill(kill_fn fn) { kill_fn_ = fn; }
    virtual void kill() { kill_fn_ ? kill_fn_() : (void)0; }
    
    void resize(resize_fn fn) { resize_fn_ = fn; }
    virtual void resize(int width, int height) { resize_fn_ ? resize_fn_(width, height) : (void)0; }
    
    void mouse_move(resize_fn fn) { mouse_move_fn_ = fn; }
    virtual void mouse_move(int x, int y) { mouse_move_fn_ ? mouse_move_fn_(x, y) : (void)0; }
    
    void mouse_down(mouse_down_fn fn) { mouse_down_fn_ = fn; }
    virtual void mouse_down(int x, int y, mouse_button button) { mouse_down_fn_ ? mouse_down_fn_(x, y, button) : (void)0; }
    
    void mouse_up(mouse_up_fn fn) { mouse_up_fn_ = fn; }
    virtual void mouse_up(int x, int y, mouse_button button) { mouse_up_fn_ ? mouse_up_fn_(x, y, button) : (void)0; }
    
    void mouse_click(mouse_click_fn fn) { mouse_click_fn_ = fn; }
    virtual void mouse_click(int x, int y, mouse_button button) { mouse_click_fn_ ? mouse_click_fn_(x, y, button) : (void)0; }
    
    void mouse_scroll(mouse_scroll_fn fn) { mouse_scroll_fn_ = fn; }
    virtual void mouse_scroll(int x, int y, int value) { mouse_scroll_fn_ ? mouse_scroll_fn_(x, y, value) : (void)0; }
    
    void key_down(key_down_fn fn) { key_down_fn_ = fn; }
    virtual void key_down(int x, int y, unsigned int key) { key_down_fn_ ? key_down_fn_(x, y, key) : (void)0; }
    
    void key_up(key_up_fn fn) { key_up_fn_ = fn; }
    virtual void key_up(int x, int y, unsigned int key) { key_up_fn_ ? key_up_fn_(x, y, key) : (void)0; }

    void drag_drop(drag_drop_fn fn) { drag_drop_fn_ = fn; }
    virtual void drag_drop(std::vector<std::wstring> files) { drag_drop_fn_ ? drag_drop_fn_(files) : (void)0; }

    // Attributes...
    const shogl_str& window_title() const { return window_title_; }
    bool window_fullscreen() const { return fullscreen_; }
    int window_height() const { return window_height_; }
    int window_width() const { return window_width_; }
    int window_target_fps() const { return target_fps_; }
    int window_click_duration() const { return click_duration_; }

    void window_title(const shogl_str& title) { window_title_ = title; }
    void window_fullscreen(bool fs) { fullscreen_ = fs; }
    void window_height(int height) { window_height_ = height; }
    void window_width(int width) { window_width_ = width; }
    void window_target_fps(int fps) { target_fps_ = fps; }
    void window_click_duration(int duration) { click_duration_ = duration; }
    
    void window_context(std::shared_ptr<context> ctxt) { context_ = ctxt; }
    context* window_context() { return context_.get(); }

    bool frame_limiter()
    {
        if (!target_fps_)
            return true;

        auto fps_now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(fps_now - fps_time_stamp_).count() > (1000 / target_fps_))
        {
            fps_time_stamp_ = fps_now;
            return true;
        }
        return false;
    }
};


#ifdef SHOGL_X

class x_context : public shogl_window::context
{
public:

    GLXContext              glc_;
    Display*                dpy_;
    Window                  root_;
    XVisualInfo*            vi_;
    Colormap                cmap_;
    XSetWindowAttributes    swa_;
    Window                  win_;
    XWindowAttributes       gwa_;
    XEvent                  xev_;

    x_context()
    {
        dpy_ = XOpenDisplay(NULL);
        if(!dpy_) 
            throw std::runtime_error("cannot connect to X server");
        
        root_ = DefaultRootWindow(dpy_);
        GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        vi_ = glXChooseVisual(dpy_, 0, att);
        if(!vi_) 
            throw std::runtime_error("no appropriate visual found");
        
        cmap_ = XCreateColormap(dpy_, root_, vi_->visual, AllocNone);
        swa_.colormap = cmap_;
        swa_.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPress | ButtonReleaseMask;
        win_ = XCreateWindow(dpy_, root_, 0, 0, 300, 300, 0, vi_->depth, InputOutput, vi_->visual, CWColormap | CWEventMask, &swa_);
        XMapWindow(dpy_, win_);
        //XStoreName(dpy_, win_, shogl()->window_title().c_str());

        glc_ = glXCreateContext(dpy_, vi_, NULL, GL_TRUE);
        make_current(nullptr);

        printf("OpenGL %s\n", glGetString(GL_VERSION));
    }

    void make_current(void*)
    {
        glXMakeCurrent(dpy_, win_, glc_);
    }
    
    void free()
    {
        glXMakeCurrent(dpy_, None, NULL);
        glXDestroyContext(dpy_, glc_);
    }
};

static void process_event(XEvent& event, bool& quit)
{
    switch (event.type)
    {
    case MapNotify:
        break;
    case DestroyNotify:
        shogl()->kill();
        break;
    case ButtonPress:
        {
            switch (event.xbutton.button)
            {
            case 1:
                shogl()->mouse_down(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::left);
                break;
            case 2:
                shogl()->mouse_down(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::middle);
                break;
            case 3:
                shogl()->mouse_down(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::right);
                break;
            case 4:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, 1);
                break;
            case 5:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, -1);
                break;
            default:
                break;
            }
        }
        break;
    case ButtonRelease:
        {
            switch (event.xbutton.button)
            {
            case 1:
                shogl()->mouse_up(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::left);
                break;
            case 2:
                shogl()->mouse_up(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::middle);
                break;
            case 3:
                shogl()->mouse_up(event.xbutton.x, event.xbutton.y, shogl_window::mouse_button::right);
                break;
            case 4:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, 1);
                break;
            case 5:
                shogl()->mouse_scroll(event.xbutton.x, event.xbutton.y, -1);
                break;
            default:
                break;
            }
        }
        break;
    case MotionNotify:
        {
            switch (event.xbutton.button)
            {
            case 1:
                shogl()->mouse_move(event.xbutton.x, event.xbutton.y);
                break;
            case 2:
                shogl()->mouse_move(event.xbutton.x, event.xbutton.y);
                break;
            case 3:
                shogl()->mouse_move(event.xbutton.x, event.xbutton.y);
                break;
            default:
                break;
            }
        }
        break;
    case KeyPress:
        {
            shogl()->key_down(event.xbutton.x, event.xbutton.y, event.xkey.keycode);
            
            // IF escaped pressed, quit...
            if ( event.xkey.keycode == 0x09 )
                quit = true;
        }
        break;
    case KeyRelease:
        {
            shogl()->key_up(event.xbutton.x, event.xbutton.y, event.xkey.keycode);
        }
        break;
    default:
        break;
    }
}

static int run_x()
{
    auto ctx = dynamic_cast<x_context*>(shogl()->window_context());
    XStoreName(ctx->dpy_, ctx->win_, shogl()->window_title().c_str());
    bool quit = false;
    while (!quit)
    {
        // Check resize...
        XGetWindowAttributes(ctx->dpy_, ctx->win_, &ctx->gwa_);
        if (shogl()->window_width() != ctx->gwa_.width || shogl()->window_height() != ctx->gwa_.height)
        {
            shogl()->resize(ctx->gwa_.width, ctx->gwa_.height);
            shogl()->window_width(ctx->gwa_.width);
            shogl()->window_height(ctx->gwa_.height);
        }

        // Process event(s) if any...
        if(XCheckWindowEvent(ctx->dpy_, ctx->win_, KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask, &ctx->xev_))
            process_event(ctx->xev_, quit);
        
        // Draw if time for frame...
        if (shogl()->frame_limiter())
        {
            shogl()->draw();
            glXSwapBuffers(ctx->dpy_, ctx->win_);
        }

        // Idling...
        shogl()->idle();
    }
    shogl()->kill();
    ctx->free();
    XDestroyWindow(ctx->dpy_, ctx->win_);
    XCloseDisplay(ctx->dpy_);
    return 0;
}

// For this to work, we first need to create the context since GL functions maybe members of shogl_window so valid 
// context will need to exist before resolving them via shogl_window instantiation...
#define SHOGL_IMPL int main(int argc, char **argv) \
{ \
    std::shared_ptr<x_context> ctx = std::make_shared<x_context>(); \
    shogl_init_impl(); \
    shogl()->window_context(ctx); \
    return run_x(); \
} 

#endif // SHOGL_X


#ifdef SHOGL_WIN

class win_context : public shogl_window::context
{
public:

    HWND hWnd_;
    HGLRC hGLrc_;
    HINSTANCE hInst_;
    WNDCLASSEX wcex_;

    win_context(void* hdc)
    {
        if (!hdc)
            throw std::runtime_error("No device context.");

        HDC hDC = static_cast<HDC>(hdc);
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
    }

    //void free(void* hdc) { wglDeleteContext(hGLrc_); }
    //void make_current(void* hdc) { wglMakeCurrent(static_cast<HDC>(hdc), hGLrc_); }

    void make_current(void* hdc) { wglMakeCurrent(static_cast<HDC>(hdc), hGLrc_); }
    void free() { wglDeleteContext(hGLrc_); }
};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        //KillTimer(hWnd, IDT_FRAME_TIMER);
        shogl()->kill();
        PostQuitMessage(0);
        break;
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
        shogl()->resize(static_cast<int>(LOWORD(lParam)), static_cast<int>(HIWORD(lParam)));
        shogl()->window_height(static_cast<int>(HIWORD(lParam)));
        shogl()->window_width(static_cast<int>(LOWORD(lParam)));
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
        shogl()->mouse_move(LOWORD(lParam), HIWORD(lParam));
        break;
    }
    case WM_LBUTTONDOWN:
    {
        //shogl()->click_begin(shogl_window::mouse_button::left);
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::left);
        SetCapture(hWnd);
        break;
    }
    case WM_LBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        //if (shogl()->is_click(shogl_window::mouse_button::left))
        //    shogl()->mouse_click(x, shogl()->window_height() - y, glwindow::mouse_button::left);
        shogl()->mouse_up(x, shogl()->window_height() - y, shogl_window::mouse_button::left);
        ReleaseCapture();
        break;
    }
    case WM_MBUTTONDOWN:
    {
        //shogl()->click_begin(glwindow::mouse_button::middle);
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::middle);
        SetCapture(hWnd);
        break;
    }
    case WM_MBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        //if (shogl()->is_click(glwindow::mouse_button::middle))
        //    shogl()->mouse_click(x, glwindow_get()->height() - y, glwindow::mouse_button::middle);
        shogl()->mouse_up(x, shogl()->window_height() - y, shogl_window::mouse_button::middle);
        ReleaseCapture();
        break;
    }
    case WM_RBUTTONDOWN:
    {
        //shogl()->click_begin(glwindow::mouse_button::right);
        shogl()->mouse_down(LOWORD(lParam), shogl()->window_height() - HIWORD(lParam), shogl_window::mouse_button::right);
        SetCapture(hWnd);
        break;
    }
    case WM_RBUTTONUP:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        //if (shogl()->is_click(glwindow::mouse_button::right))
        //    shogl()->mouse_click(x, shogl()->window_height() - y, shogl_window::mouse_button::right);
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
    /*case WM_TIMER:
    {
        if (wParam == IDT_FRAME_TIMER)
        {
            InvalidateRect(hWnd, NULL, NULL);
            UpdateWindow(hWnd);
        }
        break;
    }*/
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

static void init_win(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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

    // create the gl context...
    auto context = glwindow_version_major() > 1 ?
        std::make_shared<glcontext_base>(GetDC(hWnd_)) :
        std::make_shared<glcontext_extended>(GetDC(hWnd_), glwindow_version_major(), glwindow_version_minor());

    // trigger the instance to construct it (if GLWINDOW_CLASS)...
    glwindow_get()->context(context);

    // call the custom init (defined after GLWINDOW macro)...
    glwindow_init_impl();

    // set the callback frame timer...
    if (glwindow_get()->target_fps())
        SetTimer(hWnd_, IDT_FRAME_TIMER, (1000 / glwindow_get()->target_fps()), NULL);

    // Resize the window to desired...
    RECT rect;
    GetWindowRect(hWnd_, &rect);
    SetWindowPos(hWnd_, HWND_TOP, rect.left, rect.top, glwindow_get()->width(), glwindow_get()->height(), NULL);

    // Set the name of the window...
    SetWindowText(hWnd_, glwindow_get()->title().c_str());
}

static int run_win()
{
    try
    {
        init_win(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
        MSG msg; 
        ShowWindow(hWnd_, nCmdShow);
        UpdateWindow(hWnd_);
        while (GetMessage(&msg, NULL, 0, 0))
        {
            glwindow_get()->idle();
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
            return (int)msg.wParam;
    }
    catch (const std::exception& e) { MessageBox(NULL, ansi_to_wide(e.what()).c_str(), L"GLwindow", MB_ICONERROR); }
    catch (...) { MessageBox(NULL, L"Unknown error", L"GLwindow", MB_ICONERROR); }
    return 0;
}

// For this to work, we first need to create the context since GL functions maybe members of shogl_window so valid 
// context will need to exist before resolving them via shogl_window instantiation...
//#define SHOGL_IMPL int main(int argc, char **argv) \
//{ \
//    std::shared_ptr<x_context> ctx = std::make_shared<x_context>(); \
//    shogl_init_impl(); \
//    shogl()->window_context(ctx); \
//    return run_x(); \
//} 

#define SHOGL_IMPL int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) \
{\
    CoInitialize(nullptr); \
    std::shared_ptr<win_context> ctx = std::make_shared<win_context>(); \
    shogl_init_impl(); \
    shogl()->window_context(ctx); \
    return run_win(); \   
}

#endif // SHOGL_WIN

#endif // SHOGL_HPP