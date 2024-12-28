# Single Header Only GL window. (shogl)window

Designed to be quick and easy to use and still versatile only requiring C++14 and STL (aside from linking to GL and X11). See examples for GL1.0 (fixed-function) and GL4 (programmable) usage.

### Features Supported
* Any GL version
* MS Windows 
* X11
* Mouse and keyboard events
* Drag drop events (windows)
* Multi-paradigm, OO (derive class) or functional (use callback)
* Mouse scroll events
* Frame limiter

### Features Not Supported (yet)
* Multiple windows.
* Custom icon.
* Fullscreen.
* Multiple contexts.
* Extended mouse buttons (x1, x2 etc).

# Usage

Simply include single header file _shogl.hpp_. shogl only requires C++14 std and uses _wglext.h_ (windows) and _glx.hpp_ (X11) which should be in the same folder as _shogl.hpp_ and are provided.

shogl works by instatiating a platform/version dependant context _shogl_window::context_ and then providing an interface _shogl_window_ that can be used in two different ways. Either an OO paradim can be used by optionally deriving from _shogl_window_ class a custom class and overriding the required methods, or a more functional paradim which setting a callback for the required method.  

To initialise shogl, using a custom class, define the class and then use __SHOGL_CLASS(...)__ macro...

    class myClass : public shogl_window
    {
        ...
    };

    SHOGL_CLASS(myClass)
    {
        ...
    }

Or to use the default _shogl_window_ class, use the __SHOGL(...)__ macro. This implies callbacks can only be used...

    SHOGL()
    {
        ...
    }

Optional initialisation code can go in the body of either of the macros (or use constructor of dervied class), in either case the gl context can be assumed constructed at this point so extended gl functions (aquired via _***getProcAddress_) are valid to be used.

> It is possible to use a callback with a derived method by calling the base (which will invoke the callback if present) and then invoking the method body, however this is not recommended, favouring centralising body/logic of desired method.

### Visual Studio

Create an empty project, set the sub-system to **Windows** (_Properties->Linker->SubSystem->Windows_).

_shogl.hpp_ contains a linker parameter to include _openGL32.lib_. If this fails, then manually include **opengl32.lib** in the _Properties->Linker->Input->Additonal Dependencies_.

### GCC

Include libX11 and libGL when building.

    > g++ file.cpp -lX11 -lGL

## Events

Events are passed to the required _shogl_window_ interface methods to be consumed.

| Event | Notes |
| --------------------------------------|---------------------|
| _idle()_ | Window idle event. Frequency depends on shogl operation mode (see __Idle/Draw__) |
| _draw()_ | Window draw event. |
| _kill()_ | Window kill event. Cleanup/Free resources can be done. |
| _resize(int width, int height)_ | Window resize event. |
| _mouse_move(int x, int y)_ | Mouse move event. x,y are local window coords, with bottom left corner being 0,0. Top right is width,height |
| _mouse_down(int x, int y, mouse_button button)_ | Mouse button down event. |
| _mouse_up(int x, int y, mouse_button button)_ | Mouse button up event. |
| _mouse_scroll(int x, int y, int value)_ | Mouse scroll event, value +ve for forwards scroll, -ve otherwise |
| _key_down(int x, int y, unsigned int key)_ | Key down event. x,y is current mouse position, key is ascii keycode. |
| _key_up(int x, int y, unsigned int key)_ | Key up event. x,y is pointer location, key is ascii keycode. |
| _drag_drop(std::vector<std::wstring> files)_ | Windows only |



## Attributes

Other than events, the interface provides accessors and mutators for various window attributes.

| Attribute | Notes |
| --------------------------------------|---------------------|
| _window_title(const std::string& title)_ | Set the window title, can be done at any time |
| _window_size(int w, int h)_ | Set the window width and height, can be done at any time |
| _window_fps(int)_ | Set the framerate of the window callback |
| _window_fullscreen(bool)_ | toggle fullscreen |
| _redraw()_ | Manually redraw the window |
| _window_quit(int exit_code)_ | Manually quit the window. This will start termination process and ultimately call _kill()_. |

### Idle/Draw

shogl operates in two distinct modes which can be defined by setting the _window_fps()_ to 0 (for event driven mode) or greater than 0 to continuously redraw every fps milliseconds. The default is 0 fps, i.e event driven operation.

> N.B The _window_fps()_ can only be set in the derived class constructor, or macro intialisation body. This is because the constrcuted context will behave in the way defined after initialisation, and cannot be changed mid-session.  

This is implemented by the window main event loop either peeking or waiting for a message/event. When in event driven operation, both _idle()_ and _draw()_ will only be called when the window receives an event (i.e mouse move etc), however when in continuous operation, the _idle()_ event will be continuously called, however a frame limiter will only call the _draw()_ at the requested interval.

This can be demonstrated in the example programs by moving the _angle___ update call from the _idle()_ body into the _draw()_ body.

e.g. In the _gl1_class.cpp_ example...
    
    void draw()
    {
        ...

        // angle_ is not updated within the draw call, simply used...
        glRotatef(angle_, 0, 0, 1.0f);

        ...
    }

    void idle()
    {
        // angle_ is updated in the idle call...
        angle_ += 0.1f;
    }

Should become...

    void draw()
    {
        ...

        // angle_ is updated within the draw call...
        glRotatef(angle_ += 0.1f, 0, 0, 1.0f);

        ...
    }

_redraw()_ can be called at any time to manually redraw the window, useful for when in event driven operation and an animation is required, however should not be done if wanting the preserve the framerate when in continuous operation as this will trigger an extra draw which in escence circumvents the _frame_limiter()_.

### Resize/Title

Window resizing can be manually done at any time, likewise the title can be set at any time (useful when displaying metrics like FPS in the window title). If the resize event is dispatched by the platform (i.e window drag resize), shogl will manually update the window dimensions to track this by calling _assign_window_size(...)_ and this will happen after the _shogl::resize(...)_ is called. This should not be manually done by the user/client.

### Quitting

shogl applications can be manually quit by calling _window_quit()_ passing the desired exit code to elegantly exit the shogl application. This will ultimately call _kill()_ which should cleanup any gl resources used, which will be called before the context is destroyed.

### Context

The shogl context can be accessed via _window_context()_, however using the macro __SHOGL_CONTEXT__ will cast to the correct platform context (_win_context_ or _x_context_).

# Helpers 

There are a few other macros specific to GL which are important/useful. The required OpenGL version be used by defining one of the GL version macros. If not used shogl will default to highest version avaliable.

For OpenGL 1.2 context...

    #define SHOGL_OPENGL_12

For OpenGL 4.6 context...

    #define SHOGL_OPENGL_46

etc...

> X11 uses compatibility profiles and so specific version macro not required.

To aid in using extended gl functions which are not defined by default in gl.h either an extension library can be used (such as GLEW/GLFW), or alternatively shogl inbuilt macros can retrieve (context version valid ofcourse) the function by using the __GLFN(...)__ macro...

e.g.

    GLFN(GLUSEPROGRAM, glUseProgram)

This will define a function called _glUseProgram_ and attempt call _XXXgetProcAddress_ (_wglGetProcAddress_ for windows, or _glXGetProcAddress_) for linux.

Alternatively, the function can be forward delcared, and then resolved at a later time...

    GLFN_DECLARE(GLUSEPROGRAM, glUseProgram)

then to assign/define the function...
    
    GLFN_DEFINE(GLUSEPROGRAM, glUseProgram)

> N.B It is down to the user/client to check if these functions are valid (will be NULL is invalid context version, or no valid context).




