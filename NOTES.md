Notes:

Three modules:
    Capture the current window attributes, and the callbacks and behaviours.

shogl_window, is the class that holds the state and overriden behaviour
shogl_context, calls shogl_window behaviour and reads state from shogl_window
shogl_window, also holds triggers that calls context, which calls shogl_window

class shogl_delegate
{
    // attributes
    int width_, height_, x_, y_;
    std::string title_;
    behaviour behaviour_;               // event driven, loop driven
    int state_;                         // quit
    std::vector<std::string> args_;     // arguments
    
    fn resize_;
    fn draw_;
    fn idle_;
    fn mouse_move_;
    fn mouse_up_;
    fn mouse_down_;
    fn mouse_scroll_;
    fn key_up_;
    fn key_down_;
    fn drag_drop_;
    fn kill_;
    fn title_;
}

// Called by user to adjust window
class shogl_window
{
    void title(const std::string& title);
    const std::string& title();
    
    void ressize(int width, int height);
    int width();
    int height();

    void quit(int code);

    void behaviour(int type);
    int behaviour();
}

// OS/Platform depedent context
class shogl_context
{
    // GL version, 
    // GL context handles

}



delegate - overriden by user, called by context
mutator - used by user to mutate state, calls context
context - held by platform, repsonds to platform events
state - hold the state, set by context and delegate


// how it processes...
mutator -> context -> state -> delegate
    user mutates, changes the state, calls the delegate e.g. set window size 

context -> state -> delegate
    platform/OS events occur, changes the state and calls the delegate e.g. window size changed



so when user changes something, sets the state, then context updates based on the state
so when OS changes something, sets the state, then context updates based on the state


