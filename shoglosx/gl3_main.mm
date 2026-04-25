// Running example 

#include "../include/shogl.hpp"

#include <OpenGL/gl3.h>
#include <iostream>


#import <Cocoa/Cocoa.h>
#import <CoreVideo/CoreVideo.h>

@interface GLView : NSOpenGLView {
    CVDisplayLinkRef displayLink;
}
@end

static CVReturn DisplayLinkCallback(
    CVDisplayLinkRef,
    const CVTimeStamp*,
    const CVTimeStamp*,
    CVOptionFlags,
    CVOptionFlags*,
    void* context
);

@implementation GLView

- (void)prepareOpenGL 
{
    std::cout << "osx_window.mm::prepareOpenGL\n";

    [super prepareOpenGL];
    [[self openGLContext] makeCurrentContext];

    NSRect b = [self bounds];
    //initGL(b.size.width, b.size.height);

    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, DisplayLinkCallback, (__bridge void*)self);

    CGLContextObj ctx = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj pf = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, ctx, pf);

    CVDisplayLinkStart(displayLink);

    std::cout << "osx_window.mm::prepareOpenGL finish\n";
}

- (void)reshape 
{
    std::cout << "osx_window.mm::reshape\n";
    [super reshape];
    NSRect b = [self bounds];
    shogl()->resize(b.size.width, b.size.height);
}

- (void)drawRect:(NSRect)dirtyRect 
{
    //std::cout << "osx_window.mm::draw\n";
    [[self openGLContext] makeCurrentContext];
    shogl()->draw();
    [[self openGLContext] flushBuffer];
    //std::cout << "osx_window.mm::draw finished\n";
}

// -------- INPUT --------

- (BOOL)acceptsFirstResponder { return YES; }

- (void)keyDown:(NSEvent*)event 
{
    //onKey((int)[event keyCode], true);
}

- (void)keyUp:(NSEvent*)event 
{
    //onKey((int)[event keyCode], false);
}

- (void)mouseMoved:(NSEvent*)event 
{
    NSPoint p = [self convertPoint:[event locationInWindow] fromView:nil];
    //onMouseMove(p.x, p.y);
}

- (void)updateTrackingAreas {
    [super updateTrackingAreas];
    NSTrackingArea* area = [[NSTrackingArea alloc]
        initWithRect:self.bounds
             options:NSTrackingMouseMoved | NSTrackingActiveAlways | NSTrackingInVisibleRect
               owner:self
            userInfo:nil];
    [self addTrackingArea:area];
}

- (void)dealloc {
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
}

@end

static CVReturn DisplayLinkCallback(
    CVDisplayLinkRef,
    const CVTimeStamp*,
    const CVTimeStamp*,
    CVOptionFlags,
    CVOptionFlags*,
    void* context
) {
    GLView* view = (__bridge GLView*)context;

    dispatch_async(dispatch_get_main_queue(), ^{
        [view setNeedsDisplay:YES];
    });

    return kCVReturnSuccess;
}

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property NSWindow* window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification 
{
    std::cout << "osx_window.mm::applicationDidFinishLaunching\n";

    NSRect frame = NSMakeRect(100, 100, 800, 600);

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

    [self.window setTitle:@"OpenGL Minimal Engine"];
    [self.window setContentView:view];
    [self.window makeFirstResponder:view];
    [self.window makeKeyAndOrderFront:nil];

    std::cout << "osx_window.mm::applicationDidFinishLaunching finish\n";
}

@end

GLuint compileShader(GLenum type, const char* src) 
{
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    return s;
}

int impl(NSApplication* app);
namespace { static std::unique_ptr<shogl_window> instance_; }
shogl_window* shogl() { if (!instance_) { instance_ = std::make_unique<shogl_window>(); } return instance_.get(); }
int main()
{
    @autoreleasepool 
    {
        std::cout << "osx_window.mm::osx_window_app_run\n";
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* delegate = [AppDelegate new];
        [app setDelegate:delegate];
        [app finishLaunching];
        impl(app);
        std::cout << "osx_window.mm::osx_window_app_run finish\n";
    }
    return 0;
}
int impl(NSApplication* app)
{
    std::cout << "impl...\n";

    GLuint vao, vbo, program;

    const char* vs = R"(
        #version 150
        in vec2 position;
        void main() {
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )";

    const char* fs = R"(
        #version 150
        out vec4 color;
        void main() {
            color = vec4(1.0, 0.5, 0.2, 1.0);
        }
    )";

    GLuint vert = compileShader(GL_VERTEX_SHADER, vs);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fs);

    program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glBindAttribLocation(program, 0, "position");
    glLinkProgram(program);

    float verts[] = {
         0.0f,  0.5f,
        -0.5f, -0.5f,
         0.5f, -0.5f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    shogl()->draw([=]()
    {
        std::cout << "gl3_main.cpp:draw\n";

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    });

    shogl()->resize([](int width, int height)
    {
        glViewport(0, 0, width, height);
    });

    std::cout << "impl [app run]...\n";
    [app run];
    return 0;
}
