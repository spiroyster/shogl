// macOS Shim (input + resize + loop)

// This is essentially the osx code that will be copy-pasted into shogl.hpp
// Add any osx functionality to here for testing...

#include "../include/shogl.hpp"
#include <iostream>
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>

#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>

//#if defined(SHOGL_OPENGL_30) || defined(SHOGL_OPENGL_31) || defined(SHOGL_OPENGL_32) || defined(SHOGL_OPENGL_33)
// #define GL_SILENCE_DEPRECATION
// #include <OpenGL/gl3.h>
//#endif // defined(SHOGL_OPENGL_30) || defined(SHOGL_OPENGL_31) || defined(SHOGL_OPENGL_32) || defined(SHOGL_OPENGL_33)

// GL Function macros...
#define GLFN_DECLARE(prototype, name) ;
#define GLFN_DEFINE(prototype, name) ;
#define GLFN(prototype, name) ;

#define SHOGL_QUIT(exitCode) ;
#define SHOGL_CONTEXT dynamic_cast<win_context*>(shogl()->window_context());


@interface GLView : NSOpenGLView 
{
    CVDisplayLinkRef displayLink;
    //@property bool peek;
}
@end

static CVReturn DisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp*, 
    const CVTimeStamp*, CVOptionFlags, CVOptionFlags*, void* context) 
{
    GLView* view = (__bridge GLView*)context;

    dispatch_async(dispatch_get_main_queue(), ^{
        [view setNeedsDisplay:YES];
    });

    return kCVReturnSuccess;
}

static std::shared_ptr<shogl_window::context> osxctx;

@implementation GLView

- (void)prepareOpenGL 
{
    std::cout << "osx_window.mm::prepareOpenGL\n";

    [super prepareOpenGL];
    [[self openGLContext] makeCurrentContext];

    NSRect b = [self bounds];
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, DisplayLinkCallback, (__bridge void*)self);

    CGLContextObj ctx = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj pf = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, ctx, pf);

    shogl()->window_context(osxctx);
    osxctx->window_title("OpenGL OSX !!!");
    shogl()->window_size(b.size.width, b.size.height);
    if (shogl()->window_event_behaviour() == shogl_window::peekEvent)
        CVDisplayLinkStart(displayLink);
    else
        CVDisplayLinkStop(displayLink);

    std::cout << "osx_window.mm::prepareOpenGL finish\n";
}

- (void)reshape 
{
    [super reshape];
    NSRect b = [self bounds];
    int w = (int) b.size.width;//shogl()->window_width();
    int h = (int) b.size.height;//shogl()->window_height();
    std::cout << "osx_window.mm::resize " << w << ", " << h << "\n";
    shogl()->resize(w, h);
    [self setNeedsDisplay:YES];
}

- (void)drawRect:(NSRect)dirtyRect 
{
    [[self openGLContext] makeCurrentContext];
    shogl()->draw();
    [[self openGLContext] flushBuffer];
}

- (void)keyDown:(NSEvent*)event 
{
    NSLog(@"key down at: %@", NSStringFromPoint([event locationInWindow]));

    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    auto* s = shogl();
    s->key_down(p.x, p.y, (int)[event keyCode]);
    
    if (s->window_event_behaviour() == shogl_window::waitForEvent)
        [self setNeedsDisplay:YES];
}

- (void)keyUp:(NSEvent*)event 
{
    NSLog(@"key up at: %@", NSStringFromPoint([event locationInWindow]));

    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    auto* s = shogl();
    s->key_up(p.x, p.y, (int)[event keyCode]);
    
    if (s->window_event_behaviour() == shogl_window::waitForEvent)
        [self setNeedsDisplay:YES];
}

- (void)mouseMoved:(NSEvent*)event 
{
    //NSLog(@"mouse move at: %@", NSStringFromPoint([event locationInWindow]));

    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    
    auto* s = shogl();
    s->mouse_move(p.x, p.y);
    
    if (s->window_event_behaviour() == shogl_window::waitForEvent)
        [self setNeedsDisplay:YES];
}

- (void)mouseDown:(NSEvent *)event 
{
    NSLog(@"mouse down at: %@", NSStringFromPoint([event locationInWindow]));

    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    
    auto* s = shogl();
    s->mouse_down(p.x, p.y, shogl_window::left);
    
    if (s->window_event_behaviour() == shogl_window::waitForEvent)
        [self setNeedsDisplay:YES];
}

- (void)mouseUp:(NSEvent *)event 
{
    NSLog(@"mouse up at: %@", NSStringFromPoint([event locationInWindow]));

    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    
    auto* s = shogl();
    s->mouse_up(p.x, p.y, shogl_window::left);
    
    if (s->window_event_behaviour() == shogl_window::waitForEvent)
        [self setNeedsDisplay:YES];
}

- (void)scrollWheel:(NSEvent *)event 
{
    CGFloat deltaX = [event scrollingDeltaX];
    CGFloat deltaY = [event scrollingDeltaY];
    
    // Check if it's a precise trackpad scroll vs discrete mouse wheel
    if (event.hasPreciseScrollingDeltas) 
    {
        // Trackpad or Magic Mouse
    } 
    else 
    {
        // Traditional mouse wheel
    }
    
    NSLog(@"Scrolling: X:%.2f, Y:%.2f", deltaX, deltaY);
}

- (void)updateTrackingAreas 
{
    [super updateTrackingAreas];
    NSTrackingArea* area = [[NSTrackingArea alloc]
        initWithRect:self.bounds
             options:NSTrackingMouseMoved | NSTrackingActiveAlways | NSTrackingInVisibleRect
               owner:self
            userInfo:nil];
    [self addTrackingArea:area];
}

- (void)window_event_behaviour:(BOOL)waitForEvent
{
    if (waitForEvent)
        CVDisplayLinkStart(displayLink);
    else
        CVDisplayLinkStop(displayLink);
}



- (BOOL)acceptsFirstResponder { return YES; }

- (void)dealloc 
{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
}

@end



@interface AppDelegate : NSObject <NSApplicationDelegate>
@property NSWindow* window;
@end




class osx_context : public shogl_window::context
{
    AppDelegate* app_;
public:
    osx_context(AppDelegate* app)
        :   app_(app)
    {
    }

    void make_current(void*) {}
    void free() {}
    
    void window_title(const std::string& title) 
    {
        NSString* myString = [NSString stringWithUTF8String:title.c_str()];
        [app_.window setTitle:myString];
    }
    void window_size(int width, int height) 
    {
        NSRect frame = [app_.window frame];
        frame.size.width = width;
        frame.size.height = height;
        [app_.window setFrame: frame display: YES animate: YES];
    }
    void window_redraw() 
    {
        [app_.window setNeedsDisplay:YES];
    }
    
    void window_quit(int exit_code) {}
    
    void window_event_behaviour(const shogl_window::event_behaviour& mode) 
    { 
        event_behaviour_ = mode; 
        if (mode == shogl_window::peekEvent)
            [app_.window window_event_behaviour:YES];
        else
            [app_.window window_event_behaviour:NO];
    }
    
    shogl_window::event_behaviour window_event_behaviour() { return event_behaviour_; }
    
    int window_show() {}

    void* DC() { return nullptr; }
    void* GLC() { return nullptr; }
};


@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification 
{
    std::cout << "osx_window.mm::applicationDidFinishLaunching\n";

    NSRect frame = NSMakeRect(100, 100, 300, 300);

    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        0
    };

    NSOpenGLPixelFormat* pf =
        [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

    GLView* view = [[GLView alloc] initWithFrame:frame pixelFormat:pf];

    self.window = [[NSWindow alloc]
        initWithContentRect:frame
                  styleMask:(NSWindowStyleMaskTitled |
                             NSWindowStyleMaskClosable |
                             NSWindowStyleMaskResizable)
                    backing:NSBackingStoreBuffered
                      defer:NO];

    // Initialise the context (will get assigned during init)...
    osxctx = std::make_shared<osx_context>(self);
    
    [self.window setContentView:view];
    [self.window makeFirstResponder:view];
    [self.window makeKeyAndOrderFront:nil];
    [self.window setStyleMask:[self.window styleMask] | NSWindowStyleMaskResizable];

    std::cout << "osx_window.mm::applicationDidFinishLaunching finish\n";

    // CFRunLoopObserverRef observer = CFRunLoopObserverCreateWithHandler( kCFAllocatorDefault,
    // kCFRunLoopBeforeWaiting,   // fires when run loop is idle
    // true,
    // 0,
    // ^(CFRunLoopObserverRef observer, CFRunLoopActivity activity) 
    // {
    //     // Your idle work here
    // });

    // // Start idle loop...
    // CFRunLoopAddObserver(CFRunLoopGetMain(), observer, kCFRunLoopCommonModes);
}

@end






class myWindow : public shogl_window
{
    GLuint vao_;
    GLuint program_;
    unsigned int pointBuffer_, colourBuffer_;

    // Rotation angle...
    float angle_ = 0;

    GLuint rotationMatrixLocation_;
    float rotationMatrix_[16] = 
    { 
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f 
    };

    GLuint glsl_compile(GLuint type, const std::string& source)
    {
        GLFN(GLCREATESHADER, glCreateShader)
        GLFN(GLSHADERSOURCE, glShaderSource)
        GLFN(GLCOMPILESHADER, glCompileShader)
        GLFN(GLLINKPROGRAM, glLinkProgram)
        GLFN(GLGETSHADERIV, glGetShaderiv)
        GLFN(GLGETSHADERINFOLOG, glGetShaderInfoLog)

        GLuint shaderID = glCreateShader(type);
        const char* src = source.c_str();

        glShaderSource(shaderID, 1, &src, NULL);
        glCompileShader(shaderID);

        GLint result = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int infoLogLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
            std::vector<GLchar> error(infoLogLength);
            glGetShaderInfoLog(shaderID, infoLogLength, &infoLogLength, &error[0]);
            throw std::runtime_error(std::string(&error[0], error.size()));
        }
        return shaderID;
    }

    GLuint glsl_link(const std::vector<GLuint>& shaders)
    {
        GLFN(GLCREATEPROGRAM, glCreateProgram)
        GLFN(GLATTACHSHADER, glAttachShader)
        GLFN(GLDETACHSHADER, glDetachShader)
        GLFN(GLLINKPROGRAM, glLinkProgram)
        GLFN(GLGETPROGRAMIV, glGetProgramiv)
        GLFN(GLGETPROGRAMINFOLOG, glGetProgramInfoLog)

        GLuint programID = glCreateProgram();
        for (unsigned int s = 0; s < shaders.size(); ++s)
            glAttachShader(programID, shaders[s]);

        glLinkProgram(programID);
        GLint result = GL_FALSE;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            int InfoLogLength;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
            std::vector<GLchar> error(InfoLogLength);
            glGetProgramInfoLog(programID, InfoLogLength, &InfoLogLength, &error[0]);
            throw std::runtime_error(std::string(&error[0], error.size()));
        }

        for (unsigned int s = 0; s < shaders.size(); ++s)
            glDetachShader(programID, shaders[s]);

        return programID;
    }

public:
    myWindow()
    {
        std::cout << "myWindow constructor (initGL)\n";

        // Create our geometry...
        std::vector<float> points = { -0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f };
        std::vector<float> colours = 
        { 
            1.0f, 0.0f, 0.0f, 
            0.0f, 1.0f, 0.0f, 
            0.0f, 0.0f, 1.0f 
        };

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &pointBuffer_);
        glBindBuffer(GL_ARRAY_BUFFER, pointBuffer_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), &points.front(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, NULL);

        glGenBuffers(1, &colourBuffer_);
        glBindBuffer(GL_ARRAY_BUFFER, colourBuffer_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colours.size(), &colours.front(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, NULL);

        glBindVertexArray(NULL);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        GLuint vertexShader = glsl_compile(GL_VERTEX_SHADER, R"(
            #version 330
            layout(location = 0) in vec3 inPoint;
            layout(location = 1) in vec3 inColour;
            out vec3 colour;
            uniform mat4 rotationMatrix;
            void main()
            {
                gl_Position = rotationMatrix * vec4(inPoint, 1.0);
                colour = inColour;
            }
        )");

        GLuint fragmentShader = glsl_compile(GL_FRAGMENT_SHADER, R"(
            #version 330
            in vec3 colour;
            out vec4 frag;
            void main()
            {
                frag = vec4(colour, 1.0);
            }
        )");

        program_ = glsl_link({ vertexShader, fragmentShader });
        rotationMatrixLocation_ = glGetUniformLocation(program_, "rotationMatrix");

        glUseProgram(NULL);
    }

    void draw()
    {
        angle_ += 0.1f;
        rotationMatrix_[0] = static_cast<float>(cos(angle_));
        rotationMatrix_[1] = static_cast<float>(sin(angle_));
        rotationMatrix_[4] = static_cast<float>(-sin(angle_));
        rotationMatrix_[5] = static_cast<float>(cos(angle_));
        rotationMatrix_[10] = 1.0f;
        rotationMatrix_[15] = 1.0f;

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program_);
        glUniformMatrix4fv(rotationMatrixLocation_, 1, GL_TRUE, rotationMatrix_);
        glBindVertexArray(vao_);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(NULL);
        glUseProgram(NULL);
    }

    void resize(int width, int height)
    {
        //std::cout << "myWindow::resize " << width << ", " << height << "\n";

        glViewport(0, 0, width, height);
    }

    void kill()
    {
        glDeleteBuffers(1, &pointBuffer_);
        glDeleteBuffers(1, &colourBuffer_);
        glDeleteVertexArrays(1, &vao_);
        glDeleteProgram(program_);
    }

    void mouse_down(int x, int y, shogl_window::mouse_button button)
    {
        std::cout << "mouse down " << x << ", " << y << "\n";

        shogl()->window_event_behaviour(shogl()->window_event_behaviour() == shogl_window::peekEvent ?
            shogl_window::waitForEvent : shogl_window::peekEvent);
    }

    void key_down(int x, int y, unsigned int key)
    {
        std::cout << "key down " << key << "  " << x << ", " << y << "\n";
        
        if (key == 27)
            shogl()->window_quit(0);
        else
            shogl()->window_event_behaviour(shogl()->window_event_behaviour() == shogl_window::peekEvent ?
                shogl_window::waitForEvent : shogl_window::peekEvent);
    }
};

namespace { static std::unique_ptr<myWindow> instance_; }
shogl_window* shogl() { if (!instance_) { instance_ = std::make_unique<myWindow>(); } return instance_.get(); }
int main() 
{
    @autoreleasepool 
    {
        std::cout << "osx_window.mm::osx_window_run\n";
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* delegate = [AppDelegate new];
        [app setDelegate:delegate];
        [app run];
        std::cout << "osx_window.mm::osx_window_run finish\n";
    }
    return 0;
}