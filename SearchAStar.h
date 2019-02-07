//Jonathan Walsh
#pragma once

#include <iostream> //Input/output stream
#include <algorithm> //For sorting lists
#include <string> //String class

#include "Utilities.h" //Helper Functions
#include "Definitions.h"//Type defs
#include "Search.h" //Interface

using namespace std;//Makes code clearer to read e.g cout instead of std::cout can be used

class CSearchAStar : public ISearch
{
public:
	CSearchAStar(); //Initialises and "sets up" the class

	//Finds and constructs the path from start to goal
	bool FindPath(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path); 

	//Progesses path in real time
	bool Iterate(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path, NodeList& open, NodeList& close, NodeList& path2Current);

	~CSearchAStar(); //Deallocation of memory.  Tiddies class up


private:	
	int ManhattanDistance(SmartPointNode& goal, SmartPointNode& newNode); //Grid distance from start to node.
	int NewCost(SmartPointNode& newNode, TerrainMap& terrain); //Cost to get to a new node.
	void ScanLists(NodeList& unscannedOpenList, NodeList& unscannedClosedList, SmartPointNode& goal, TerrainMap& terrain); //Scans open/closed list for duplicate nodes.
	void AssignTempNode(SmartPointNode& temp, SmartPointNode& goal, TerrainMap terrain, int direction); //Gives temp nodes values.
	void DisplayList(NodeList& aList); //Displays any of the node lists
	void ConstructPath(NodeList& aPath); //Constructs the final path
	void Path2Current();//Constructs path to current
	void CopyList(NodeList& existing, NodeList& newList); //Copies any nodelist

	NodeList mpOpenList; //Generation of nodes we haven't visited yet using north, east, south, west rules.
	NodeList mpClosedList; //Nodes that we have visited.

	NodeList mpIteratePath; //Holds nodes for path to current

	SmartPointNode mpCurrent; //The current node that we are using, to generate nodes etc.
	SmartPointNode mpStaticGoal;

	//Temporary nodes
	SmartPointNode mpN;


	SNode* mpRawCurrent; //Holds the raw version of the current node.
	SNode* mpRawThePath; //Pointer to the parent values
	SNode* mpRawIteratePath; //Points to parent values
	
	bool mNEliminated; //Eliminates N if it doesn't meet requirements
	const int mNUMBER_OF_RULES = 4; //North, East, South, West
	const int mNORTH = 0;
	const int mEAST  = 1;
	const int mSOUTH = 2;
	const int mWEST  = 3;

	//Strings used for output text.
	string mOpenListSortedText1 = "Open List is sorted ";
	string mOpenListSortedText2 = " times.";
	string mScore				= " Score: ";
	string mComma				= ", ";
	string mSpace				= " ";
	string mOpenListSorted		= "Open list is sorted: ";

	bool mGoalReached; //Is true when we have reached the goal
	int mIterations; //How many times the open list gets sorted.
};