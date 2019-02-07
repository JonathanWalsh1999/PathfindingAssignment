//Jonathan Walsh
// SearchFactory.cpp
// =================
//
// Factory function implementation to create CSearchXXX objects 
//

//Search Algorithms
#include "SearchAStar.h"
#include "SearchDijkstra.h"

#include "SearchFactory.h" // Factory declarations

// Create new search object of the given type and return a pointer to it.
// Note the returned pointer type is the base class. This is how we implement polymorphism.
ISearch* NewSearch(ESearchType search)
{
  switch (search)
  {
    case AStar:
      {
         return new CSearchAStar();
      }

	case Dijkstra:
	{
		return new CSearchDijkstra();
	}
  }
  return new CSearchAStar();
}
