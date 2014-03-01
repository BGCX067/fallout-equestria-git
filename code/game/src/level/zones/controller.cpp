#include "level/zones/controller.hpp"
#include "level/zones/manager.hpp"
#include "level/level.hpp"
#include <ui/ui_next_zone.hpp>

using namespace std;

Zones::Controller::Controller(Zone& zone) : manager(0), zone(zone), enabled(true)
{
}

void Zones::Controller::ObjectMovesWithinZone(InstanceDynamicObject* object)
{
  if (enabled)
  {
    if (IsExitZone())
      ExitingZone(object);
    else
      RegisterResident(object);
  }
}

void Zones::Controller::RegisterResident(InstanceDynamicObject* object)
{
  auto it = find(residents.begin(), residents.end(), object);

  if (it == residents.end())
    InsertResident(object);
  else
    RefreshResident(*it);
}

void Zones::Controller::GoFromHereTo(const string& destination)
{
  Level::Exit& exit_data = manager->level.GetExit();

  exit_data.level = destination;
  exit_data.zone  = zone.name; // Current zone name is used as entry zone name for the destination
}

void Zones::Controller::ExitingZone(InstanceDynamicObject* object)
{
  if (starts_with(zone.name, "LocalExit"))
  {
    manager->InsertObjectInZone(object, zone.destinations.front());
  }
  else if (manager->level.GetPlayer() == object)
  {
    if (zone.destinations.size() == 1)
      GoFromHereTo(zone.destinations.front());
    else
    {
      UiNextZone* ui = manager->level.GetLevelUi().OpenZonePicker(zone.destinations);
      
      ui->Done.Connect            (manager->level.GetLevelUi(), &LevelUi::CloseRunningUi<LevelUi::UiItNextZone>);
      ui->NextZoneSelected.Connect(*this,                       &Zones::Controller::GoFromHereTo);
    }
  }
}

void Zones::Controller::InsertObject(InstanceDynamicObject* object)
{
  auto it      = zone.waypoints.begin();
  auto end     = zone.waypoints.end();
  bool success = false;

  for (; it != end ; ++it)
  {
    cout << manager->ZoneExists(zone.name) << endl;
    cout << manager->level.GetPlayer()->GetName() << endl;
    if (!(manager->level.IsWaypointOccupied((*it)->id)))
    {
      InsertObjectOnWaypoint(object, *it);
      success  = true;
      break ;
    }
  }
  if (success)
    InsertResident(object);
  else
    throw "bite";
}

void Zones::Controller::InsertObjectOnWaypoint(InstanceDynamicObject* object, Waypoint* waypoint)
{
  ObjectCharacter* character      = object->Get<ObjectCharacter>();
  NodePath         nodepath       = object->GetNodePath();
  DynamicObject&   dynamic_object = *object->GetDynamicObject();
  World*           world          = manager->level.GetWorld();
  
  world->DynamicObjectSetWaypoint(dynamic_object, *waypoint);
  dynamic_object.floor = -1;
  object->SetOccupiedWaypoint(waypoint);
  world->DynamicObjectChangeFloor(dynamic_object, waypoint->floor);
  object->GetNodePath().set_pos(waypoint->nodePath.get_pos());
  if (character)
  {
    character->TruncatePath(0);
    if (character == manager->level.GetPlayer())
      manager->level.GetCamera().CenterCameraInstant(nodepath.get_pos());
  }
}

void Zones::Controller::InsertResident(Resident resident)
{
  residents.push_back(resident);
  EnteredZone.Emit(resident.object);
}

void Zones::Controller::RefreshResident(Resident& resident)
{
  Waypoint* current_waypoint = resident.object->GetOccupiedWaypoint();
  
  if (current_waypoint != resident.waypoint)
  {
    resident.waypoint = current_waypoint;
    MovedWithinZone.Emit(resident.object);
  }
}

void Zones::Controller::Refresh(void)
{
  auto it  = residents.begin();
  auto end = residents.end();
  
  while (it != end)
  {
    Resident& resident = *it;

    if (resident.HasMoved())
    {
      if (IsInZone(resident.object))
        resident.waypoint = resident.object->GetOccupiedWaypoint();
      else
      {
        ExitedZone.Emit(resident.object);
        it = residents.erase(it);
        continue ;
      }
    }
    ++it;
  }
}

void Zones::Controller::SetEnabled(bool enabled)
{
  if (enabled == false && this->enabled == true)
    DisableZone();
  this->enabled = enabled;
}

void Zones::Controller::DisableZone(void)
{
  for_each(residents.begin(), residents.end(), [this](Resident& resident)
  {
    ExitedZone.Emit(resident.object);
  });
  residents.clear();
}

bool Zones::Controller::IsInZone(InstanceDynamicObject* object) const
{
  Waypoint* waypoint = object->GetOccupiedWaypoint();
  auto      match    = find(zone.waypoints.begin(), zone.waypoints.end(), waypoint);
  
  return (match != zone.waypoints.end());
}