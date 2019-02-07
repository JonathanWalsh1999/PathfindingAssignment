//Jonathan Walsh
// SearchFactory.h
// ===============
//
// Factory function declaration to create CSearchXXX objects 
//

#pragma once

#include "Search.h" // Search interface class

// List of implemented seach algorithms
enum ESearchType
{
  Dijkstra,
  AStar,
};

// Factory function to create CSearchXXX object where XXX is the given search type
ISearch* NewSearch(ESearchType search);
