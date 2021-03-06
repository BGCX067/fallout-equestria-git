#ifndef  MOUSE_HPP
# define MOUSE_HPP

# include "globals.hpp"
# include <panda3d/pandaFramework.h>
# include <panda3d/pandaSystem.h>
# include <panda3d/mouseWatcher.h>
# include <panda3d/collisionRay.h>
# include <panda3d/collisionHandlerQueue.h>
# include <panda3d/collisionTraverser.h>
# include "observatory.hpp"

struct Waypoint;

struct MouseHovering
{
  MouseHovering(void) : hasWaypoint(false), hasDynObject(false), waypoint_ptr(0) {}
  
  void Reset(void)
  {
    dynObject    = NodePath();
    waypoint     = NodePath();
    hasWaypoint  = hasDynObject = false;
    waypoint_ptr = 0;
  }
  
  void SetWaypoint(NodePath np)
  {
    hasWaypoint = true;
    waypoint    = np;
  }
  
  void SetDynObject(NodePath np)
  {
    hasDynObject = true;
    dynObject    = np;
  }
  
  bool      hasWaypoint, hasDynObject;
  NodePath  waypoint;
  Waypoint* waypoint_ptr;
  NodePath  dynObject;
};

struct World;

class Mouse
{
public:
  Mouse(WindowFramework* window);
  ~Mouse();

  void                      Run(void);
  const MouseHovering&      Hovering(void) const { return (_hovering); }
  LPoint2f                  GetPosition(void) const;
  LPoint2f                  GetPositionRatio(void) const;
  void                      Move(float x, float y);
  
  void                      SetMouseState(char);
  
  void                      ClosestWaypoint(World*, short currentFloor);

  Sync::Signal<void>        ButtonLeft;
  Sync::Signal<void>        ButtonMiddle;
  Sync::Signal<void>        ButtonRight;
  Sync::Signal<void>        WheelUp;
  Sync::Signal<void>        WheelDown;

  static void               CallbackButton1(const Event*, void* ptr)
  {
    reinterpret_cast<Mouse*>(ptr)->ButtonLeft.Emit();
  }

  static void               CallbackButton2(const Event*, void* ptr)
  {
    reinterpret_cast<Mouse*>(ptr)->ButtonMiddle.Emit();
  }

  static void               CallbackButton3(const Event*, void* ptr)
  {
    reinterpret_cast<Mouse*>(ptr)->ButtonRight.Emit();
  }
  
  static void               CallbackWheelUp(const Event*, void* ptr)
  {
    reinterpret_cast<Mouse*>(ptr)->WheelUp.Emit();
  }
  
  static void               CallbackWheelDown(const Event*, void* ptr)
  {
    reinterpret_cast<Mouse*>(ptr)->WheelDown.Emit();
  }

private:
  WindowFramework*          _window;
  NodePath                  _camera;
  LPoint2f                  _lastMousePos;
  PT(MouseWatcher)          _mouseWatcher;
  PT(CollisionRay)          _pickerRay;
  PT(CollisionNode)         _pickerNode;
  NodePath                  _pickerPath;
  CollisionTraverser        _collisionTraverser;
  PT(CollisionHandlerQueue) _collisionHandlerQueue;
  
  MouseHovering             _hovering;
};

#endif
