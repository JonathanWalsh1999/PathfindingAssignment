//Jonathan Walsh
#include "EngineController.h"//Declarzations

CEngineController::CEngineController(SmartPointNode start, SmartPointNode goal, NodeList& myPath, TerrainMap& myMap, ESearchType theSearch, EMode mode)
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	mpMyEngine = New3DEngine(mMyEngineType);
	mpMyEngine->StartWindowed();

	//Start and Goal positions
	mStartPos = { start->x, start->y };
	mGoalPos = { goal->x, goal->y };

	mFinalCoords.push_back(mStartPos);//Starting position added to the path as a starting point for real time pathfinding

	if (mode == PreLoad)
	{
		mFinalCoords = ReadFile(mOutputFile);//Reads file if path is preloaded
		mNewSmoothPath = SmoothPath(mFinalCoords);//Smooths that path
	}

	// Add default folder for meshes and other media
	mpMyEngine->AddMediaFolder( mMediaDirectory );

	InitialiseVars(theSearch);//Initialise variables

	LoadMeshes(); //Load all meshes
	CreateModels();//Create all models
	SetUpCamera();//Set camera up, position etc.

	SetUpTiles(myMap, mGoalPos);//Make a 2D grid of tiles for visual representation

	mpMyEngine->Timer();//Get the base time

	GameLoop(move(start), move(goal), myMap, mode); //Start the game loop
}

//Runs the engine loop which updates once per frame
void CEngineController::GameLoop(SmartPointNode start, SmartPointNode goal, TerrainMap& myMap, EMode mode)
{
	while (mpMyEngine->IsRunning())
	{
		mpMyEngine->DrawScene();
		mFrameTime = mpMyEngine->Timer(); //Get the time it takes to loop

		
		if ((unsigned) mCurrentNode < mNewSmoothPath.size())//Stops index going out of bounds
		{
			//Change dummy position when required, so model knows which way to face
			mpDummy->SetPosition((float)mNewSmoothPath[mCurrentNode].x, 0, (float)mNewSmoothPath[mCurrentNode].y);
			mpMe->LookAt(mpDummy);//Model looks at dummy
			mpMe->MoveLocalZ(mFrameTime*mSpeedMultiplier); //Model moves along facing vector

			FacingCoordinatesToScreen();//Shows the coordinates that the model is currently facing
		}
		UserInstructionsToScreen();//Shows controls to screen

		if (mCurrentNode > 0 && (unsigned)mCurrentNode < mNewSmoothPath.size())
		{
			//Finds out which direction the model is moving.
			//eg positive in x and z
			SetMovementDirection();
		}

		//Only executes during real time pathfinding
		if(mode == RunTimeLoad)
		{
			if (!mIterationFinished)//Until the search has been completed
			{
				if (mpMyEngine->KeyHeld(mContinuousProgress))
				{
					NodeList openList;
					NodeList closedList;
					NodeList path2Current;

					//Each iteration of the search node by node
					mPath = mpMyIteration->Iterate(myMap, move(start), move(goal), mpThePath, openList, closedList, path2Current);

					for (unsigned int i = 0; i < openList.size(); i++)
					{
						//Sets every tile corresponding to a node on openlist to purple.
						mpSquare[openList[i]->y][openList[i]->x]->SetSkin(mPurpleTile);
					}
					for (unsigned int i = 0; i < closedList.size(); i++)
					{
						//Sets every tile corresponding to a node on closedlist to grey.
						mpSquare[closedList[i]->y][closedList[i]->x]->SetSkin(mGreyTile);
					}
					for (unsigned int i = 0; i < path2Current.size(); i++)
					{
						//Sets every tile corresponding to a node on path2current to violet.
						mpSquare[path2Current[i]->y][path2Current[i]->x]->SetSkin(mVioletTile);
					}
					//Path successfully generated
					if (mPath)
					{					
						GenerateFile(mpThePath, mOutputFile);//Output path to output file
						mFinalCoords = ReadFile(mOutputFile);//Read file
						mNewSmoothPath = SmoothPath(mFinalCoords);//Smooth the path
						mIterationFinished = true;//Iterations completed
					}
				}

				if (mpMyEngine->KeyHit(mNodeByNodeProgress))
				{
					NodeList openList;
					NodeList closedList;
					NodeList path2Current;

					//Each iteration of the search node by node
					mPath = mpMyIteration->Iterate(myMap, move(start), move(goal), mpThePath, openList, closedList, path2Current);

					for (unsigned int i = 0; i < openList.size(); i++)
					{
						//Sets every tile corresponding to a node on openlist to purple.
						mpSquare[openList[i]->y][openList[i]->x]->SetSkin(mPurpleTile);
					}
					for (unsigned int i = 0; i < closedList.size(); i++)
					{
						//Sets every tile corresponding to a node on closedlist to grey.
						mpSquare[closedList[i]->y][closedList[i]->x]->SetSkin(mGreyTile);
					}
					for (unsigned int i = 0; i < path2Current.size(); i++)
					{
						//Sets every tile corresponding to a node on path2current to violete.
						mpSquare[path2Current[i]->y][path2Current[i]->x]->SetSkin(mVioletTile);
					}
					if (mPath)
					{
						GenerateFile(mpThePath, mOutputFile);//Output path to file
						mFinalCoords = ReadFile(mOutputFile);
						mNewSmoothPath = SmoothPath(mFinalCoords);//Smooth path
						mIterationFinished = true; //Iterations complete
					}
				}
			}

		}
		
		//Path successully generated
		if (mPath)
		{
			DisplayStatus(); //Current status of model
			PauseSimulation(); //User can pause simulation
			ResetSimulation(); 
			IterateNode(); //Increases the index of finalcoords by 1
		}
		ExitSimulation();
	}
}

//Reads output file to a deque to be used in visual representation
MultipleNodes CEngineController::ReadFile(string outputFile)
{
	ifstream infile;
	infile.open(outputFile);
	MultipleNodes finalCoords;
	string errorMessage = "File did not open!";

	if (!infile)
	{
		cout << errorMessage << endl;
	}
	while (!infile.eof())
	{	
		//Read each line and put it into a deque
		SNode tmp;			
		infile >> tmp.x >> tmp.y;
		if (infile.eof())	break;		
		finalCoords.push_back(tmp);
	}
	infile.close();
	return finalCoords;
}


//Load all meshes
void CEngineController::LoadMeshes()
{
	mpFloorMesh = mpMyEngine->LoadMesh(mFloorMeshName);
	mpMeMesh	= mpMyEngine->LoadMesh(mMeMeshName);
	mpDummyMesh = mpMyEngine->LoadMesh(mDummyMeshName);
	mpSquareMesh = mpMyEngine->LoadMesh(mSquareMeshName);
}

//Create all models
void CEngineController::CreateModels()
{
	float floorHeight = -0.2f;
	mpFloor = mpFloorMesh->CreateModel(0, floorHeight, 0);
	mpMe = mpMeMesh->CreateModel(mMePos.x, 0, mMePos.y);
	mpDummy = mpDummyMesh->CreateModel();
}

//Set the camera up
void CEngineController::SetUpCamera()
{
	float cameraHeight = 15.0f;
	mCameraPos = { 3.0f, 4.0f }; //Position of camera
	mpMyCamera = mpMyEngine->CreateCamera(kManual, mCameraPos.x, cameraHeight, mCameraPos.y); //Creation of camera	
	mpMyCamera->RotateLocalX(mCameraXRot); //Rotation of camera in x
}

//Initialise all variables
void CEngineController::InitialiseVars(ESearchType theSearch)
{	
	mpMyFont = mpMyEngine->LoadFont(mFont, mFontSize); //Intialise the font size and font type.
	mCurrentNode = 0; //Intialise currentNode to start on the first node

	mMePos = { (float)mFinalCoords[0].x, (float)mFinalCoords[0].y, }; //Initialise position of model	


	mSpeedMultiplier = 0.3f;//Intialise speed multiplier
	mPaused = false;//Game is intialally not paused
	mpMyIteration = NewSearch(theSearch);

	mPositiveX = true;
	mPositiveZ = true;
	mIterationFinished = false;
}

//Set up the tiled grid which represents the map
void CEngineController::SetUpTiles(TerrainMap& myMap, SNode goal)
{
	mpSquare.resize(myMap.size());
	for (unsigned int i = 0; i < myMap.size(); i++)
	{
		mpSquare[i].resize(myMap[i].size());
	}

	for (unsigned int i = 0; i < myMap.size(); i++)//Height
	{
		for (unsigned int j = 0; j < myMap[i].size(); j++)//Width
		{
			mpSquare[i][j] = mpSquareMesh->CreateModel((1.0f*j), 0.0f, (1.0f*i));
			if (myMap[i][j] == Clear)
			{
				mpSquare[i][j]->SetSkin(mGreenTile); //Set tile skin to green for clear
			}
			else if (myMap[i][j] == Water)
			{
				mpSquare[i][j]->SetSkin(mBlueTile); //Blue for water
			}
			else if (myMap[i][j] == Wood)
			{
				mpSquare[i][j]->SetSkin(mBrownTile); //Brown for wood
			}
			else
			{
				mpSquare[i][j]->SetSkin(mRedTile);//Red for wall
			}
		}
	}
	mpSquare[mFinalCoords[0].y][mFinalCoords[0].x]->SetSkin(mStartTile);
	mpSquare[goal.y][goal.x]->SetSkin(mGoalTile);
}

//Shows the coordinates the model is facing at
void CEngineController::FacingCoordinatesToScreen()
{
	stringstream waypointText;
	string comma = ", ";
	waypointText << mWaypointToScreen;
	waypointText << mNewSmoothPath[mCurrentNode].x;
	waypointText << comma << mNewSmoothPath[mCurrentNode].y;
	mpMyFont->Draw(waypointText.str(), 0, 0);//Draw to screen
}

//Gives user possible instructions/controls
void CEngineController::UserInstructionsToScreen()
{
	stringstream instructions;
	instructions <<  mPauseInstruction << endl;
	instructions <<  mStopInstruction << endl;
	instructions << mRepeatInstruction << endl << endl;
	instructions << mRealTimeOnly << endl;
	instructions << mSpaceBarControl << endl;
	instructions << mNControl << endl << endl;
	instructions << mInfoOpen << endl;
	instructions << mInfoClosed << endl;
	instructions << mInfoPath << endl << endl;
	instructions << mInfoOther1 << endl;
	instructions << mInfoOther2;
	mpMyFont->Draw(instructions.str(), 0, mYPositionInstructions); //Draw to screen
}

//Shows the direction of model movement
void CEngineController::SetMovementDirection()
{
	if (mNewSmoothPath[mCurrentNode].x < mNewSmoothPath[mCurrentNode - 1].x)
	{
		mPositiveX = false; //Model moves in negative x
	}
	else
	{
		mPositiveX = true; //Model moves in positive x
	}
	if (mNewSmoothPath[mCurrentNode].y < mNewSmoothPath[mCurrentNode - 1].y)
	{
		mPositiveZ = false; //Model moves in negative z
	}
	else
	{
		mPositiveZ = true; //Model moves in positive z
	}
}

//Increases index of deque to next node when the model reaches the facing coordinate
void CEngineController::IterateNode()
{
	//Move onto next node if the current node reaches the dummy model
	if (mPositiveX && mPositiveZ)
	{
		if ((mpMe->GetX() >= mpDummy->GetX() - 0.1f && mpMe->GetZ() >= mpDummy->GetZ() - 0.1f) &&
			(unsigned)mCurrentNode < mNewSmoothPath.size())
		{
			mCurrentNode++;
		}
	}
	else if (mPositiveX && !mPositiveZ)
	{
		if ((mpMe->GetX() >= mpDummy->GetX() - 0.1f && mpMe->GetZ() <= mpDummy->GetZ() + 0.1f) &&
			(unsigned)mCurrentNode < mNewSmoothPath.size())
		{
			mCurrentNode++;
		}
	}
	else if (!mPositiveX && mPositiveZ)
	{
		if ((mpMe->GetX() <= mpDummy->GetX() + 0.1f && mpMe->GetZ() >= mpDummy->GetZ() - 0.1f) &&
			(unsigned)mCurrentNode < mNewSmoothPath.size())
		{
			mCurrentNode++;
		}
	}
	else if (!mPositiveX && !mPositiveZ)
	{
		if ((mpMe->GetX() <= mpDummy->GetX() + 0.1f && mpMe->GetZ() <= mpDummy->GetZ() + 0.1f) &&
			(unsigned)mCurrentNode < mNewSmoothPath.size())
		{
			mCurrentNode++;
		}
	}
}

//Pauses the simulation
void CEngineController::PauseSimulation()
{

	if (mpMyEngine->KeyHit(mPause))
	{
		if (!mPaused)
		{
			mSpeedMultiplier = 0;//Make the speed of the model 0
			mPaused = true;
		}
		else
		{
			if (mCurrentNode != mFinalCoords.size())
			{
				mSpeedMultiplier = 1.0f; //Resume simulation
				mPaused = false;
			}
		}
	}
}
//---------------
//Displays the status of the simulation
void CEngineController::DisplayStatus()
{
	stringstream status;
	if (mCurrentNode == mNewSmoothPath.size() && mCurrentNode != 0)
	{
		status << mGoalNodeReached; //Text output to screen
		mSpeedMultiplier = 0; //Stop model moving if goal has been reached
		mPaused = true;
	}
	else
	{
		if (mSpeedMultiplier == 0)
		{
			status << mPauseSimulation;
		}
		else
		{
			if (mCurrentNode > 0)
			{
				status << mProgression;
			}
			else
			{
				status << mStartNodeStatus;
			}
		}
	}
	mpMyFont->Draw(status.str(), 0, mYPositionStatus);
}

//Resets the simulation
void CEngineController::ResetSimulation()
{
	//Move the model back to start position.
	if (mpMyEngine->KeyHit((mResetSimulation)))
	{
		mCurrentNode = 0;
		mpMe->SetPosition((float)mFinalCoords[mCurrentNode].x, 0, (float)mFinalCoords[mCurrentNode].y);
	}
}

//Exits the simulation
void CEngineController::ExitSimulation()
{
	if (mpMyEngine->KeyHit(mExitSimulation))
	{
		mpMyEngine->Stop();
	}
}

//Pathsmooth algorithm
SubMultipleNodes CEngineController::SmoothPath(MultipleNodes& controlNodes)
{
	SubMultipleNodes curve;
	int numberOfSubPoints = 150; //Amount of points in between each control node

	for (unsigned int i = 0; i < controlNodes.size();)
	{
		//4 control nodes per loop
		unsigned int controlIndex1 = i;
		unsigned int controlIndex2 = i + 1;
		unsigned int controlIndex3 = i + 2;
		unsigned int controlIndex4 = i + 3;

		//Stops the control node indices from going out of bounds.
		if (controlIndex2 >= controlNodes.size()) controlIndex2 = controlIndex1;
		if (controlIndex3 >= controlNodes.size()) controlIndex3 = controlIndex2;
		if (controlIndex4 >= controlNodes.size()) controlIndex4 = controlIndex3;
		
		//Intialise positions for each control node.
		SSubNode p1 = { (float)controlNodes[controlIndex1].x, (float)controlNodes[controlIndex1].y };
		SSubNode p2 = { (float)controlNodes[controlIndex2].x, (float)controlNodes[controlIndex2].y };
		SSubNode p3 = { (float)controlNodes[controlIndex3].x, (float)controlNodes[controlIndex3].y };
		SSubNode p4 = { (float)controlNodes[controlIndex4].x, (float)controlNodes[controlIndex4].y };

		if (i == 0)
		{
			curve.push_back(CalculateSubPoint(0, p1, p2, p3, p4));//First point on curve
		}

		for(int j = 1; j < numberOfSubPoints; j++)
		{
			float t = j / (float)numberOfSubPoints; //position on the curve ie quarter, half or 3quarter
			curve.push_back(CalculateSubPoint(t, p1, p2, p3, p4)); //Creates the point and adds it onto the list
		}
		i += 3;
	}
	//add goal node to end of spline
	SNode goal = controlNodes.back();
	SSubNode goalFloat = { (float)goal.x, (float)goal.y };
	curve.push_back(goalFloat); 

	return curve;//Return the smoothed path
}

//Calculate a point on the spline
SSubNode CEngineController::CalculateSubPoint(float t, SSubNode p1, SSubNode p2, SSubNode p3, SSubNode p4)
{
	//Variables to make formula more readable
	float u = 1 - t;
	float tSquared = t * t;
	float uSquared = u * u;
	float uCubed = uSquared * u;
	float tCubed = tSquared * t;

	//Beizer curve
	//L(t) = (1-t)^3*p1 + 3t*(1-t)^2*p2 + 3t^2*(1-t)*p3 + t^3*p4 
	SSubNode subPoint;
	subPoint.x = uCubed * p1.x; //First term in formula above
	subPoint.y = uCubed * p1.y;

	subPoint.x += 3 * t * uSquared * p2.x;//2nd
	subPoint.y += 3 * t * uSquared * p2.y;

	subPoint.x += 3 * tSquared * u * p3.x;//3rd
	subPoint.y += 3 * tSquared * u * p3.y;

	subPoint.x += tCubed * p4.x;//4th
	subPoint.y += tCubed * p4.y;


	return subPoint; //Result from using the Bezier equation above
}

CEngineController::~CEngineController()
{
	//Releases memory
	mpMyEngine->Delete();
}