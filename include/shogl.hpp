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

#endif // SHOGL_X

// Macro...
#define SHOGL(shogl_window_class) SHOGL_IMPL \
namespace { static std::unique_ptr<shogl_window_class> instance_; } \
shogl_window* shogl() { if (!instance_) { instance_ = std::make_unique<shogl_window_class>(); } return instance_.get(); } \
void shogl_init_impl()

typedef std::string shogl_str;

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
        virtual void make_current() = 0;
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
        win_ = XCreateWindow(dpy_, root_, 0, 0, shogl()->window_width(), shogl()->window_height(), 0, vi_->depth, InputOutput, vi_->visual, CWColormap | CWEventMask, &swa_);
        XMapWindow(dpy_, win_);
        //XStoreName(dpy_, win_, shogl()->window_title().c_str());

        glc_ = glXCreateContext(dpy_, vi_, NULL, GL_TRUE);
        make_current();

        printf("OpenGL %s\n", glGetString(GL_VERSION));
    }

    void make_current()
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

#endif // SHOGL_HPP