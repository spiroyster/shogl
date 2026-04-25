/*

MIT License

Copyright(c) 2026 Cordell Barron

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef SHOGL_DELEGATE_HPP
#define SHOGL_DELEGATE_HPP

#include <functional>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <stdexcept>


class shogl_delegate
{
public:

    /// <summary>
    /// Mouse button enum.
    /// </summary>
    enum mouse_button
    {
        none = 0,
        left,
        middle,
        right
    };

    /// <summary>
    /// Window event behaviour.
    /// 
    /// This is how the window events are consumed/processed. PeekEvent is a continuous polling behaviour
    /// which is useful for animations as it perpetually draws the window. WaitForEvent, does is more efficient
    /// in that is only calls idle and redraw whenever there is a window event.
    /// </summary>
    enum event_behaviour
    {
        peek,
        waitForEvent
    };

    typedef std::function<void()> idle_fn;
    typedef std::function<void()> draw_fn;
    typedef std::function<void()> kill_fn;
    typedef std::function<void(int, int)> resize_fn;
    typedef std::function<void(int, int)> mouse_move_fn;
    typedef std::function<void(int, int, mouse_button)> mouse_down_fn;
    typedef std::function<void(int, int, mouse_button)> mouse_up_fn;
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
    mouse_down_fn mouse_down_fn_ = nullptr;
    mouse_scroll_fn mouse_scroll_fn_ = nullptr;
    key_up_fn key_up_fn_ = nullptr;
    key_down_fn key_down_fn_ = nullptr;
    drag_drop_fn drag_drop_fn_ = nullptr;

public:

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

    void mouse_scroll(mouse_scroll_fn fn) { mouse_scroll_fn_ = fn; }
    virtual void mouse_scroll(int x, int y, int value) { mouse_scroll_fn_ ? mouse_scroll_fn_(x, y, value) : (void)0; }

    void key_down(key_down_fn fn) { key_down_fn_ = fn; }
    virtual void key_down(int x, int y, unsigned int key) { key_down_fn_ ? key_down_fn_(x, y, key) : (void)0; }

    void key_up(key_up_fn fn) { key_up_fn_ = fn; }
    virtual void key_up(int x, int y, unsigned int key) { key_up_fn_ ? key_up_fn_(x, y, key) : (void)0; }

    void drag_drop(drag_drop_fn fn) { drag_drop_fn_ = fn; }
    virtual void drag_drop(std::vector<std::wstring> files) { drag_drop_fn_ ? drag_drop_fn_(files) : (void)0; }

    shogl_delegate() {}
    shogl_delegate(const std::string& title, int width, int height) {}

private:

    // The state
    std::string title_;
    int width_ = 300;
    int height_ = 300;
    bool fullscreen_ = false;
    int quit_ = -1;
    int fps_ = 0;
    event_behaviour event_behaviour_ = event_behaviour::peek;
    std::vector<std::string> args_;
    std::shared_ptr<context> context_;
    std::chrono::steady_clock::time_point fps_time_stamp_ = std::chrono::steady_clock::now();

public:

    class context
    {
    public:
        context(delegate* d)

        // mutator chnages the context state
        virtual void title(const std::string& title) = 0;
        virtual void width_height(int width, int height) = 0;
        virtual void fps(int fps) = 0;
        virtual void behaviour(const event_behaviour& b) = 0;
        virtual void quit(int code) = 0;
        virtual void free() = 0;
    };

    // Mutates the state
    const std::vector<std::string> args() const { return args_; }

    const std::string& title() const { return window_title_; }
    void title(const std::string& title) 
    { 
        title_ = title; 
        if (context_) { context_->title(title_); }
    }

    int width() const { return window_width_; }
    int height() const { return window_height_; }
    void width_height(int width, int height) 
    { 
        width_ = width;
        height_ = height;
        if (context_) { context_->width_height(width_, height_); }
    }

    int fps() const { return fps_; }
    void fps(int fps) 
    { 
        fps_ = fps; 
        if (context_) { context_->width_height(width_, height_); }
    }

    int quit() 
    {
        if (context_) { quit_ = context_->quit(code) }
        return quit_;
    }

    const event_behaviour& behaviour() consr { return event_behaviour_; }
    void behaviour(const event_behaviour& b) 
    {
        event_behaviour_ = b;
        if (context_) { context_->behaviour(b); }
    } 

    void set_context(std::shared_ptr<context>& c) { context_ = c; }
     
};


// GL version macros. Shogl supports any version of OpenGL and will default to the lastest supported by the host.
// However, specific vesions can be used by defining the relevant version before including shogl.hpp in the source.
#ifdef SHOGL_OPENGL_10 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_11 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_12 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 2;
#elif defined SHOGL_OPENGL_13 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 3;
#elif defined SHOGL_OPENGL_14 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 4;
#elif defined SHOGL_OPENGL_15 
constexpr int shogl_version_major = 1; constexpr int shogl_version_minor = 5;
#elif defined SHOGL_OPENGL_20 
constexpr int shogl_version_major = 2; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_21 
constexpr int shogl_version_major = 2; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_30 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_31 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_32 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 2;
#elif defined SHOGL_OPENGL_33 
constexpr int shogl_version_major = 3; constexpr int shogl_version_minor = 3;
#elif defined SHOGL_OPENGL_40 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 0;
#elif defined SHOGL_OPENGL_41 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 1;
#elif defined SHOGL_OPENGL_42 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 2;
#elif defined SHOGL_OPENGL_43 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 3;
#elif defined SHOGL_OPENGL_44 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 4;
#elif defined SHOGL_OPENGL_45 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 5;
#elif defined SHOGL_OPENGL_46 
constexpr int shogl_version_major = 4; constexpr int shogl_version_minor = 6;
#else 
constexpr int shogl_version_major = 0; constexpr int shogl_version_minor = 0;
#endif


// Forward declarations...
int shogl_main_impl();

// Shogl instance accessor...
class shogl_delegarte;
extern shogl_delegate* shogl();

#define SHOGL() SHOGL_MAIN_IMPL \
namespace { static std::unique_ptr<shogl_delegate> instance_; } \
shogl_delegate* shogl() { if (!instance_) { instance_ = std::make_unique<shogl_delegate>(); } return instance_.get(); } \
int shogl_main_impl()

#define SHOGL_CLASS(shogl_class) SHOGL_MAIN_IMPL \
namespace { static std::unique_ptr<shogl_class> instance_; } \
shogl_delegate* shogl() { if (!instance_) { instance_ = std::make_unique<shogl_class>(); } return instance_.get(); } \
int shogl_main_impl()


#endif // SHOGL_DELEGATE_HPP