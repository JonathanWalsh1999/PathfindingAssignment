//Jonathan Walsh
#pragma once
#include <iostream> //Input/Output stream
#include <algorithm> //Used for sorting lists
#include <string> //String class

#include "Search.h" //Interface
#include "Utilities.h" //Helper Functions
#include "Definitions.h" //Type defs

using namespace std;//Makes code clearer to read e.g cout instead of std::cout can be used

class CSearchDijkstra : public ISearch
{
public:
	CSearchDijkstra(); //Initialises and "sets up" the class

	//Finds and constructs the path from start to goal
	bool FindPath(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path);

	//Progression of search in real time
	bool Iterate(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path, NodeList& open, NodeList& close, NodeList& path2Current);

	~CSearchDijkstra(); //Deallocation of memory.  Tiddies class up
private:
	int NewScore(SmartPointNode& newNode, TerrainMap& terrain); //Cost to get to a new node.
	void ScanList(NodeList& unscannedOpenList, SmartPointNode& goal, TerrainMap& terrain); //Scans open/closed list for duplicate nodes.
	void AssignTempNode(SmartPointNode& temp, SmartPointNode& goal, TerrainMap terrain, int direction); //Gives temp nodes values.
	void DisplayList(NodeList& aList); //Displays any of the node lists
	void ConstructPath(NodeList& aPath); //Constructs the final path
	void Path2Current();//Constructs path to current
	void CopyList(NodeList& existing, NodeList& newList);//Copies any nodelist

	NodeList mpOpenList; //Generation of nodes we haven't visited yet using north, east, south, west rules.
	NodeList mpVisitedList; //Nodes that have already been visited
	NodeList mpIteratePath; //Holds node from path to current

	SmartPointNode mpCurrent; //The current node that we are using, to generate nodes etc.
	SmartPointNode mpStaticGoal;//Copy of goal node

	SmartPointNode mpN; //New node using north, south, east, west rules

	SNode* mpRawCurrent; //Holds the raw version of the current node.
	SNode* mpRawThePath; //Pointer to hold the parent values
	SNode* mpRawIteratePath; //Holds parent values from last node back to start

	bool mNEliminated; //Eliminates new node if requirements haven't been met to push onto openlist
	const int mNUMBER_OF_RULES = 4; //North, East, South, West
	const int mNORTH = 0;
	const int mEAST = 1;
	const int mSOUTH = 2;
	const int mWEST = 3;

	//Strings used for output text.
	string mOpenListSortedText1 = "Open List is sorted ";
	string mOpenListSortedText2 = " times.";
	string mScore = " Score: ";
	string mComma = ", ";
	string mSpace = " ";

	bool mGoalReached; //Is true when we have reached the goal
	int mIterations; //How many times the open list gets sorted.
};