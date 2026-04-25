// macOS Shim (input + resize + loop)

// This is essentially the osx code that will be copy-pasted into shogl.hpp
// Add any osx functionality to here for testing...

#include "../include/shogl.hpp"
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
    std::cout << "osx_window.mm::draw\n";
    [[self openGLContext] makeCurrentContext];
    shogl()->draw();
    [[self openGLContext] flushBuffer];
    std::cout << "osx_window.mm::draw finished\n";
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

void osx_window_run() 
{
    @autoreleasepool 
    {
        std::cout << "osx_window.mm::osx_window_run\n";
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* delegate = [AppDelegate new];
        [app setDelegate:delegate];
        //[app finishLaunching];
        [app run];
        std::cout << "osx_window.mm::osx_window_run finish\n";
    }
}
