//Jonathan Walsh
// Definitions.h
// =============
//
// Type definitions to support path finding 
//

#pragma once

#include <vector>//Vector class
#include <deque>//Deque class
#include <memory>//For smart pointers
#include <TL-Engine.h>//Graphics engine for visual representation

using namespace std;//Makes code clearer to read e.g cout instead of std::cout can be used
using namespace tle;

// Represents a node in the search tree.
struct SNode
{
  int x;            // x coordinate
  int y;            // y coordinate

  int score = 0;    // CumlativeCost + Heuristic
  int cost;			// Cumlative cost
					
  SNode* parent = 0; // For backtracking to start node
  
};

//Basic floating point version of structure above
struct SSubNode
{
	float x; 
	float y;
};

// Terrain costs for map squares
enum ETerrainCost
{
  Clear = 1, 
  Water = 3,
  Wood  = 2,
  Wall  = 0
};

//What mode the simulation is running in dependent on user input
enum EMode { PreLoad, RunTimeLoad };

// Lists of nodes (e.g Open, Closed and Paths) are implemented as double-ended queues
using NodeList = deque<unique_ptr<SNode>>;

// Maps of any size are implemented as 2D vectors
using TerrainMap = vector<vector<ETerrainCost>>;

//Double ended queue for SNode
using MultipleNodes = deque<SNode>; 

//Double ended queue for SSubNodes
using SubMultipleNodes = deque<SSubNode>;

//Smart Pointer for SNode
using SmartPointNode = unique_ptr<SNode>;

//Integer vector
using IntVector = vector<int>;

//deque Unique Pointer SNode Iterator
using IteratorDeque = deque<unique_ptr<SNode>>::iterator;

//2D vecotor of models
using Grid2DModels = vector<vector<IModel*>>;