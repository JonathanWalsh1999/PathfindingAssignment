//Jonathan Walsh
#pragma once

#include <iostream>//Input/output stream so text can be outputted to console and input from user can be taken.
#include <fstream> //File stream, so that we are able to write or read from a file
#include <string> //Allows string class to be used

#include "Search.h" //Interface which ALL search algorithms are derived from polymorphically
#include "SearchFactory.h" //Decides what algorithm to use
#include "Definitions.h" //Supports pathfinding
#include "Utilities.h" //Helper functions
#include "EngineController.h" //Where TL-Engine code is

using namespace std; //Makes code clearer to read e.g cout instead of std::cout can be used

class CMasterController
{
public:
	CMasterController(); //Constructor
	~CMasterController();//Destructor

private:
	ESearchType mTheSearch; //Search user chose converted to ESearchType
	MultipleNodes mGetNodes; //Get the start and goal nodes

	NodeList mpMyPath; //The path which will be generated
	TerrainMap mMyMap; //2D vector of ETerrainCosts (Values 0-3).
	bool mPath; //Returns true if path successfully created
	ISearch* mpPathFinder; //A search derived polymorphically

	//Start and goal nodes
	SmartPointNode mpGoal;
	SmartPointNode mpStart;
	SmartPointNode mpGoal2;
	SmartPointNode mpStart2;

	EMode mMode;//Distinguish between load time and run time

	//Strings to be outputted to screen
	string mMapFileInstruction = "Please enter name of map file: ";
	string mSearchInstruction = "Enter a search algorithm (AStar/Dijkstra): ";
	string mPathErrorMessage = "Unable to generate path";
	string mPathSuccessMessage = "Path generated!";
	string mAgainMessage = "Would you like to do another search? (y/n) ";
	string mRealTimeQuestion = "Would you like the search to be done in real-time? (y/n): ";

	string mOutputFile = "output.txt";//Name of file that path will be outputted to.
	string mAcceptedFileType = ".txt";

	//User input
	string mMapFile; //File user chose
	string mMySearch; //Search user chose
	string mUserAgainAnswer; 
	string mRealTimeAnswer;

	//Comparison strings
	string mExitString = "n";
	string mYesAnswer = "y";

	//Index positions
	int mStartCoord = 0;
	int mGoalCoord = 1;
};