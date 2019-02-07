//Jonathan Walsh
#include "Utilities.h"//Declarations

ESearchType String2Enum(string& str)
{
	//Converts strings to ESearchType
	if (str == "AStar" || str == "astar" || str == "Astar" || str == "A-Star")	return AStar;
	else if (str == "Dijkstra" || str == "Di") return Dijkstra;
	return AStar;
}

TerrainMap GetMap(string mapFile)
{
	ifstream file; //Set up the file stream
	file.open(mapFile); //Open map file

	//Dimmensions of map
	int mapHeight; //How many rows in the map (Ys)
	int mapWidth; //How many columns are on each row (Xs)
	string fileError = "File not opened!";
	string fileMistake = "There is an error in the map file.  Make sure dimmensions are correct";

	//File could not be found or opened. i.e. user may have mistyped the map name.
	if (!file)
	{
		cout << fileError << endl;
		system("pause");
		exit(0);
	}

	//Reads the first line of the file containing dimmensions
	if (!file.eof())
	{
		string readFile; //Variable to hold data extracted from file.
		getline(file, readFile);//Get's the first line from the file.

		stringstream split(readFile); //Put's the string into a stream

		vector<string> dimmensions; //Vector to hold the dimmensions separately.

		copy(istream_iterator<string>(split),
			 istream_iterator<string>(),
			 back_inserter(dimmensions)); //Separates the string by space into a vector of strings

		//The strings are then converted to an integer and put into the below variables.
		//stoi used to convert to int.
		mapWidth = stoi(dimmensions[0]); 
		mapHeight = stoi(dimmensions[1]);

	}
	vector <vector<int>> tmpMap(mapHeight, vector<int>(mapWidth)); //Temporary 2D vector to hold the map as int values
	TerrainMap map(mapHeight, vector<ETerrainCost>(mapWidth)); //2D vector, which everything will be converted copied to.

	int row = 0;//Used to iterate onto the next row during the loop.
	while (!file.eof())
	{
		if (!file.eof())
		{		
			string tmp;//String to hold the data before it's pushed onto a vector
			file >> tmp;//Reads file starting from the first line excluding dimmensions.
			
			if (tmp != "")
			{
				if (tmp.length() != mapWidth)
				{
					//Length doesn't match up
					cout << fileMistake << endl;
					system("pause");
					exit(0);
				}
				else
				{
					for (int i = 0; i < mapWidth; i++)
					{
						vector<int> a(100); //Temporary vector to hold each of the column values.  Starting, from first row(top-left).
						//String passed onto the vector of ints.  -48 is to get rid of the offset given by characters.  
						//As the code for number 1 in a char is '49', 2 is '50' etc.
						a[i] = tmp[i] - 48;
						//Values are being pushed onto the temp 2D vector starting from row 0.
						//After each iteration, row is increased by 1, so then that row can be filled in etc.
						tmpMap[row][i] = a[i];
					}
				}
			}


		}
		if (row < mapHeight-1) 
		{
			row++; //Loops through every row in the map.  If statement to stop out of bounds error.
		}

	}

	for (int i = 0; i < mapHeight; i++)
	{
		for (int j = 0; j < mapWidth; j++)
		{
			map[i][j] = (ETerrainCost) tmpMap[i][j]; //Copies and converts integer 2D vectors to ETerrainCost 2D vectors
		}
	}
	file.close();

	TerrainMap flipMap = FlipMap(map, mapWidth, mapHeight);//Makes y go up rather down
	
	return flipMap;
}

MultipleNodes GetStartGoal(string mapFile)
{

	MultipleNodes startGoal;
	startGoal.resize(SIZE_START_GOAL);//2 nodes, start and goal
	string coordsText = "Coords.txt";
	string fileError = "File could not be opened";

	//Read coordinates file
	string coordinatesFile;	
	coordinatesFile.push_back(mapFile[0]); //Put the first character of the mapFile into the string.
	coordinatesFile += coordsText;//Add this onto the end so we know which coords file to open without additional user input.

	SNode start;	
	SNode goal;

	ifstream file;
	file.open(coordinatesFile);
	//Check if file can be opened
	if (!file)
	{
		cout <<  fileError << endl;
	}

	//Extract start node
	if (!file.eof())
	{
		string read;
		getline(file, read);

		stringstream split(read);
		vector<string> element;


		copy(istream_iterator<string>(split),
			istream_iterator<string>(),
			back_inserter(element)); //Separates the string by space into a vector of strings

		//Converts to numbers
		start.x = stoi(element[0]);
		start.y = stoi(element[1]);
	}

	//Extract goal node
	if (!file.eof())
	{
		string read;
		getline(file, read);

		stringstream split(read);
		vector<string> element;


		copy(istream_iterator<string>(split),
			istream_iterator<string>(),
			back_inserter(element)); //Separates the string by space into a vector of strings

		//Converts to numbers
		goal.x = stoi(element[0]);
		goal.y = stoi(element[1]);
	}

	//Push start and goal onto the deque of nodes
	startGoal.push_front(goal);
	startGoal.push_front(start);

	return startGoal;
}

//Get raw pointer of a smart pointer
SNode GetRawPtr(const SmartPointNode& ptr)
{
	return (*ptr.get());
}

void GenerateFile(NodeList& generatedPath, string outputFile)
{
	string errorFile = "could not open file";
	ofstream outFile;
	outFile.open(outputFile);

	if (!outFile)
	{
		cout << errorFile << endl;
	}
	for (unsigned int i = 0; i < generatedPath.size(); i++)
	{
		//Writes path nodes to file
		 outFile << generatedPath[i]->x << " ";
		 outFile << generatedPath[i]->y;
		 outFile << endl;
	}
	outFile.close();
}

//Make y go in the opposite direction
TerrainMap FlipMap(TerrainMap map, int mapWidth, int mapHeight)
{
	TerrainMap flippedMap;
	flippedMap.resize(mapHeight);
	for (int i = 0; i < mapHeight; i++)
	{
		flippedMap[i].resize(mapWidth);
	}

	for (int i = 0; i < mapHeight; i++)
	{
		for (int j = 0; j < mapWidth; j++)
		{
			flippedMap[i][j] = map[map.size() - 1 - i][j];
		}
	}
	
	return flippedMap;
}

//Display map to screen
void DisplayMap(TerrainMap map)
{
	for (unsigned int i = 0; i < map.size(); i++)
	{
		for (unsigned int j = 0; j < map.size(); j++)
		{
			cout << map[i][j] << " ";
		}
		cout << endl;
	}
}

//Compare value of one end of list to the other end
bool CompareNodes(SmartPointNode& lhs, SmartPointNode& rhs)
{
	return lhs->score < rhs->score;
}

//Display coords from any MultipleNodes type
void DisplayCoords(MultipleNodes coords)
{
	for (auto it = coords.begin(); it != coords.end(); it++)
	{
		cout << (*it).x << " " << (*it).y << endl;
	}
}

//Clear output file when finished with it
void ClearFile(string outputFile)
{
	ofstream myFile;
	myFile.open(outputFile, ofstream::out, ofstream::trunc);
	myFile.close();
}