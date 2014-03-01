#ifndef  WORLD_DYNAMIC_OBJECT_HPP
# define WORLD_DYNAMIC_OBJECT_HPP

# include "map_object.hpp"
# include <list>

struct DynamicObject : public MapObject
{
    enum Type
    {
        Door,
        Shelf,
        Locker,
        Character,
    Item
    };

    Waypoint*   waypoint;
    Type        type;
    // Interactions
    char        interactions;
    std::string dialog;
    // All
    std::string script;
    // Character
    std::string charsheet;


    // Door / Locker
    bool                            locked;
    std::string                     key; // Also used to store item names for DynamicObject::Item
    std::list<std::pair<int, int> > lockedArcs;

    // Shelf / Character
    std::list<std::pair<std::string, int> > inventory;

    void UnSerialize(World*, Utils::Packet& packet);
    void Serialize(Utils::Packet& packet) const;
};


#endif