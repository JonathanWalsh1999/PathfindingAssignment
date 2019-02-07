//Jonathan Walsh
#pragma once

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <memory> //For smart pointers
#include <vector> //Dynamic arrays
#include <fstream> //File stream
#include <iostream> //Input/output
#include <sstream>//String stream

#include "Definitions.h" //Definitions that can be used across all classes
#include "Utilities.h"//Functions that can be used across all classes
#include "Search.h" //Interface which ALL search algorithms are derived from polymorphically
#include "SearchFactory.h" //Decides what algorithm to use
#include "MasterController.h"//Holds the whole program

using namespace std;
using namespace tle;

class CEngineController
{
public:
	CEngineController(SmartPointNode start, SmartPointNode goal, NodeList& myPath, TerrainMap& myMap, ESearchType theSearch, EMode mode); //Constructor
	~CEngineController(); //Destructor

private:
	//Member methods
	void LoadMeshes();//Load meshes from files
	void CreateModels(); //Create models from meshes
	void SetUpCamera(); //Set up the camera position, type etc.
	void InitialiseVars(ESearchType theSearch); //Initialise variables before going into gameloop
	void SetUpTiles(TerrainMap& myMap, SNode goal); //Set up the grid of tiles, setting them to various colour for visual representation
	void FacingCoordinatesToScreen(); //Displays the coordinates model is facing whilst moving
	void UserInstructionsToScreen(); //Shows the controls to user
	void SetMovementDirection(); //Finds out whether model is moving in positive/ngative x or z
	void IterateNode(); //Moves onto next node if model has reached the current node
	void PauseSimulation(); //User can pause the movement of the model
	void DisplayStatus(); //The status of the model is displayed
	void ResetSimulation(); //User can reset the position of the model so that is follows the path again from the start
	void ExitSimulation(); //User can exit the simulation at any time
	MultipleNodes ReadFile(string outputFile); //Reads the output file into a deque

	//path smoothing
	SSubNode CalculateSubPoint(float t, SSubNode p1, SSubNode p2, SSubNode p3, SSubNode p4); //Gets the position of each sub node using formula
	SubMultipleNodes SmoothPath(MultipleNodes& controlNodes); //Smooths path and therefore outputs more nodes from the control nodes
	
	void GameLoop(SmartPointNode start, SmartPointNode goal, TerrainMap& myMap, EMode mode); //The while loop where the engine runs

	//Member Variables
	I3DEngine* mpMyEngine; //Holds the 3D engine
	EEngineType mMyEngineType = kTLX;
	
	string mMediaDirectory = ".\\media"; //Directory for all meshes and skins
	string mOutputFile		= "output.txt";//Output file name

	//Mesh files
	string mFloorMeshName	= "Floor.x";
	string mMeMeshName		= "casual_A.x";
	string mDummyMeshName	= "Dummy.x";
	string mSquareMeshName = "Square.x";

	//Text to be displays to screen
	string mWaypointToScreen = "Model now facing: ";
	string mPauseInstruction	= "To pause/unpause the simulation press P";
	string mStopInstruction		= "To stop simulation press Escape";
	string mRepeatInstruction	= "To run simulation again press R";
	string mGoalNodeReached = "Goal Reached!";
	string mPauseSimulation = "Simulation paused";
	string mProgression		= "Model moving along path...";
	string mRealTimeOnly = "REAL-TIME SEARCH ONLY CONTROLS BELOW";
	string mSpaceBarControl = "Press Spacebar to continuously search";
	string mNControl = "Press N to search node by node";
	string mStartNodeStatus = "Model at start node";
	string mInfoOpen = "Open list is purple";
	string mInfoClosed = "Closed list is grey";
	string mInfoPath = "Path to current is violet/pink";
	string mInfoOther1 = "Green is clear, water is blue, ";
	string mInfoOther2 = "wood is brown and wall is red";

	string mFont = "Verdana"; //Font for text to be displayed as
	int mFontSize = 24; //Size of text onscreen

	//Tile colours
	string mGreenTile = "green.png";
	string mBlueTile = "blue.png";
	string mBrownTile = "brown.png";
	string mRedTile = "red.png";
	string mPurpleTile = "purple.png";
	string mGreyTile = "grey.png";
	string mVioletTile = "violet.png";
	string mStartTile = "start.png";
	string mGoalTile = "goal.png";

	//Positioning of text
	int mYPositionStatus		= 50; 
	int mYPositionInstructions	= 100;

	ICamera* mpMyCamera; //Camera -> User's view of the game world

	//Mesh pointers used for each model
	IMesh* mpFloorMesh;
	IMesh* mpMeMesh;
	IMesh* mpDummyMesh;
	IMesh* mpSquareMesh;

	//Model pointers for each indiviual object in the game world
	IModel* mpFloor;
	IModel* mpMe;
	IModel* mpDummy;
	
	//Positions
	SSubNode mCameraPos;
	SSubNode mMePos;

	SNode mStartPos;
	SNode mGoalPos;

	float mCameraXRot = 90.0f; //Angle of camera in x

	Grid2DModels mpSquare; //Grid of tiles illustrating map from file

	IFont* mpMyFont; //Pointer for font to be displayed to screen
	

	int mCurrentNode; //The current index in the deque extracted from output file
	bool mPositiveX; //If the model is moving in positive x
	bool mPositiveZ; //If the model is moving in positive z
	bool mPaused;//Simulation is paused
	float mSpeedMultiplier;//The speed at which the model is moving
	float mFrameTime; //The time it takes to do one loop of the game

	MultipleNodes mFinalCoords;//The deque holding the nodes extracted from output file
	SubMultipleNodes mNewSmoothPath; //Deque holding the nodes for the smoothed path
	NodeList mpThePath; //The current path being contructed

	bool mPath; //Returns true when a path has been successfully contructed
	bool mIterationFinished;//Returns true when the search has been completed
	ISearch* mpMyIteration; //Real time path finding
	SmartPointNode mpCurrent; //Current node
	SmartPointNode mpGoalMove;//Goal node

	//Keyboard input
	EKeyCode mContinuousProgress = Key_Space;
	EKeyCode mNodeByNodeProgress = Key_N;
	EKeyCode mPause = Key_P;
	EKeyCode mResetSimulation = Key_R;
	EKeyCode mExitSimulation = Key_Escape;
};