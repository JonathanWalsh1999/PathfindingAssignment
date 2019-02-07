//Jonathan Walsh
#pragma once

#include <fstream>//File stream
#include <iostream>//Input/output stream
#include <sstream>//String stream
#include <iterator>//Used for splitting stringstreans
#include <string>//String class


#include "SearchFactory.h"//Type definitions
using namespace std; //Makes code clearer to read e.g cout instead of std::cout can be used


//Constants
const int SIZE_START_GOAL = 2;

//Global Functions
ESearchType String2Enum(string& str); //Converts a string to ESearchType
TerrainMap GetMap(string mapFile);//Read map file and put it into TerrainMap
TerrainMap FlipMap(TerrainMap map, int mapWidth, int mapHeight);//Flip the map in the y axis.  So y is going up rather than down
MultipleNodes GetStartGoal(string mapFile);//Get the start and end node from the xCoords file
SNode GetRawPtr(const SmartPointNode& ptr);//Extract raw pointer from a smart pointer
void GenerateFile(NodeList& generatedPath, string outputFile); //Generate the outputfile from the path contructed
void DisplayMap(TerrainMap map);//Display the terrain map to console window
void DisplayCoords(MultipleNodes coords);//Display the start and goal coordinates to console window
bool CompareNodes(SmartPointNode& lhs, SmartPointNode& rhs); //Used for sort algorithm
void ClearFile(string outputFile);//Clear outfile when we have finished with it

