//Jonathan Walsh
// Search.h
// ========
//
// Interface for all search classes
//

#pragma once

#include "Definitions.h" // type definitions

// ISearch interface class - cannot be instantiated
// Implementation classes for specific search algorithms should inherit from this interface
class ISearch
{
public:
  // Interfaces don't need a constructor - makes sense really, given they cannot be instantiated.

  // They DO need a virtual destructor if there are other virtual functions in the interface.
  // This compels derived objects destructors to be called as appropriate.
  // However, at the interface level, there is nothing to do, so the body is empty.
  virtual ~ISearch() {}

  // Constructs the path from start to goal for the given terrain
  // Pure Virtual function to be implemented in derived class.
  virtual bool FindPath(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path) = 0;

  //Finds path as search progesses
  virtual bool Iterate(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path, NodeList& open, NodeList& close, NodeList& path2Current) = 0;
};