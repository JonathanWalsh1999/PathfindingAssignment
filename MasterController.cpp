//Jonathan Walsh
#include "MasterController.h" //Declarations

CMasterController::CMasterController()
{
	do
	{
		//Allocation of memory for the smart pointers
		mpStart.reset(new SNode);
		mpGoal.reset(new SNode);
		mpStart2.reset(new SNode);
		mpGoal2.reset(new SNode);
		
		cout << mMapFileInstruction; //Tells user to enter file name
		cin >> mMapFile; //user input
		mMapFile += mAcceptedFileType;
		mMyMap = GetMap(mMapFile); //2D vector of the terrain map showing cost for each square

		mGetNodes = GetStartGoal(mMapFile); //Retrieves the start and goal node as a vector

		//Initialisation of start and goal nodes.
		mpStart->x = mGetNodes[mStartCoord].x; 
		mpStart->y = mGetNodes[mStartCoord].y;
		mpGoal->x	 = mGetNodes[mGoalCoord].x;
		mpGoal->y  = mGetNodes[mGoalCoord].y;

		//Copy to be used with other methods
		mpStart2->x = mGetNodes[mStartCoord].x;
		mpStart2->y = mGetNodes[mStartCoord].y;
		mpGoal2->x = mGetNodes[mGoalCoord].x;
		mpGoal2->y = mGetNodes[mGoalCoord].y;

		cout << mSearchInstruction;//Asks user which search algorithm to use.
		cin >> mMySearch;//User input
		cout << endl;
		mTheSearch = String2Enum(mMySearch);//Converts the user input(string) to enum
		
		mpPathFinder = NewSearch(mTheSearch); //Will access the constructor of the chosen algorithm

		//Asks user if they want search done in real time or pre loaded
		cout << mRealTimeQuestion;
		cin >> mRealTimeAnswer;
		cout << endl;

		if (mRealTimeAnswer == "y")
		{	
			if (!mPath)
			{
				cout << mPathErrorMessage << endl; //Error message in the event of a path not being constructed
			}
			else
			{
				mMode = RunTimeLoad; //Mode change
				//Starts the TL-Engine
				CEngineController* realEngine = new CEngineController(move(mpStart), move(mpGoal), mpMyPath, mMyMap, mTheSearch, mMode);
				delete realEngine;//Releases memory
			}
		}
		else
		{
			mpMyPath.clear(); //Clears previous data so it can be used again
			mPath = mpPathFinder->FindPath(mMyMap, move(mpStart2), move(mpGoal2), mpMyPath); //Will return true if path has been constructed
			if (!mPath)
			{
				cout << mPathErrorMessage << endl; //Error message in the event of a path not being constructed
			}
			else
			{
				mMode = PreLoad;//Mode change
				cout << mPathSuccessMessage << endl; //Let's user know path has been created successfully.
				GenerateFile(mpMyPath, mOutputFile); //Generates a file to hold each node of the path.
				//Starts TL-Engine
				CEngineController* myEngine = new CEngineController(move(mpStart), move(mpGoal), mpMyPath, mMyMap, mTheSearch, mMode);
				delete myEngine;//Releases memory
			}
		}
		cout << mAgainMessage; //Asks user if they want to do another search
		cin >> mUserAgainAnswer;//User input
		cout << endl;

	} while (mUserAgainAnswer != mExitString);//Will loop again if user doesn't enter "n"

	if (mUserAgainAnswer == mExitString)
	{
		exit(0);//Closes the program if user says "n"
	}	
}

CMasterController::~CMasterController()
{
	//Deallocates memory
	delete(mpPathFinder);
}