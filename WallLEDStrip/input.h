/*
  Game library header for Wall LED Strip
  Written by Tyler Gordon Hill
*/
#define YPin A3
#define XPin A2
#define SWPin 4

class Input {
  public:
    Input();
    bool up(bool sensitive = false);
    bool down(bool sensitive = false);
    bool left(bool sensitive = false);
    bool right(bool sensitive = false);
    bool click();
};
