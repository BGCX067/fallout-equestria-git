#include "level/interactions/action_runner.hpp"
#include "level/objects/character.hpp"
#include "level/level.hpp"

using namespace std;

Interactions::ActionRunner::ActionRunner(ObjectCharacter* user) : user(user), range(0), action_point_cost(0)
{
  if (user->current_action != 0)
    delete user->current_action;
  user->current_action = this;
  target_type = Self;
  target.object = 0;
}

Interactions::ActionRunner::~ActionRunner(void)
{
  user->current_action = 0;
}

Level* Interactions::ActionRunner::GetLevel(void) const
{
  return (user->_level);
}

void Interactions::ActionRunner::Run(void)
{
  if (target_type != Self && target.object == 0)
  {
    if (target_type == Object || target_type == Character)
      PickObject();
    else if (target_type == Waypoint)
      PickWaypoint();
  }
  else if (target_type == Object || target_type == Character)
    ReachTarget();
  else
    PlayAnimation();
}

void Interactions::ActionRunner::SetTargetType(const string& name)
{
  if (name == "characters")
    SetTargetType(ActionRunner::Character);
  else if (name == "waypoints")
    SetTargetType(ActionRunner::Waypoint);
  else if (name == "objects")
    SetTargetType(ActionRunner::Object);
  else
    SetTargetType(ActionRunner::Self);
}

void Interactions::ActionRunner::PickObject(void)
{
  MouseEvents& mouse = GetLevel()->GetMouse();
  
  mouse.SetState(MouseEvents::MouseTarget);
  observers.Connect(mouse.TargetPicked, [this](InstanceDynamicObject* object)
  {
    if (target_type == Character && object->Get<ObjectCharacter>() == 0)
      ; // wrong type
    SetTarget(object);
    PlayAnimation();
  });
}

void Interactions::ActionRunner::PickWaypoint(void)
{
  MouseEvents& mouse = GetLevel()->GetMouse();

  mouse.SetState(MouseEvents::MouseWaypointPicker);
  observers.Connect(mouse.WaypointPicked, [this](::Waypoint* waypoint)
  {
    SetTarget(waypoint);
    PlayAnimation();
  });
}

void Interactions::ActionRunner::ReachTarget(void)
{
  user->GoTo(target.object, range);
  observers.Connect(user->ReachedDestination, *this, &Interactions::ActionRunner::PlayAnimation);
}

void Interactions::ActionRunner::PlayAnimation(void)
{
  if ((target_type == Object || target_type == Character) && target.object)
    user->LookAt(target.object);
  if (animation_name != "")
  {
    user->AnimationEndForObject.DisconnectAll();
    observers.Connect(user->AnimationEndForObject, [this](AnimatedObject*)
    {
      RunAction();
    });
    user->PlayAnimation(animation_name);
  }
  else
    RunAction();
}

void Interactions::ActionRunner::RunAction(void)
{
  if ((target_type == Object || target_type == Character) && target.object)
    target.object->ThatDoesNothing(user);
}

bool Interactions::ActionRunner::CheckAndRemoveActionPoints(void)
{
  unsigned short action_points = user->GetActionPoints();
  
  if (action_points >= action_point_cost)
  {
    user->SetActionPoints(action_points - action_point_cost);
    return (true);
  }
  else if (user->IsPlayer())
    ; // TODO display message about not having enough action points
  return (false);
}

void Interactions::ActionRunner::ConsoleWrite(ObjectCharacter* user, const string& message)
{
  if (user->IsPlayer())
    user->_level->ConsoleWrite(message);
}