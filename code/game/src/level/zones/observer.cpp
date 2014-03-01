#include "level/zones/observer.hpp"

using namespace std;

Zones::Observer::Observer(Waypoint* to_observe) : waypoint(to_observe)
{
  for_each(waypoint->arcs.begin(), waypoint->arcs.end(), [this](Waypoint::Arc& arc)
  {
    arc.observer = this;
  });
  for_each(waypoint->arcs_withdrawed.begin(), waypoint->arcs_withdrawed.end(), [this](std::pair<Waypoint::Arc, unsigned short>& arc)
  {
    arc.first.observer = this;
  });
}

Zones::Observer::~Observer()
{
  for_each(waypoint->arcs.begin(), waypoint->arcs.end(), [this](Waypoint::Arc& arc)
  {
    if (arc.observer == this)
      arc.observer = 0;
  });
  for_each(waypoint->arcs_withdrawed.begin(), waypoint->arcs_withdrawed.end(), [this](std::pair<Waypoint::Arc, unsigned short>& arc)
  {
    if (arc.first.observer == this)
      arc.first.observer = 0;
  });
}

bool Zones::Observer::CanGoThrough(unsigned char)
{
  // TODO Implement doors management in ZoneManagement
  return (true);
}

void Zones::Observer::GoingThrough(void* _object)
{
  InstanceDynamicObject* object = reinterpret_cast<InstanceDynamicObject*>(_object);

  if (object)
  {
    for_each(zones.begin(), zones.end(), [object](Controller* entry)
    {
      entry->ObjectMovesWithinZone(object);
    });
  }
}