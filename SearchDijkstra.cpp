//Jonathan Walsh
#include "SearchDijkstra.h"//Declarations

CSearchDijkstra::CSearchDijkstra()
{
	//Allocation of memory
	mpN.reset(new SNode);
	mpCurrent.reset(new SNode);
	mpRawCurrent = new SNode;
	mpRawThePath = new SNode; //Pointer to hold the parent values
	mGoalReached = false; //Goal has not been reached yet.
	mIterations = 0; //0 iterations of the while loop have occurred
	mNEliminated = false;//New node has not been eliminated yet.  
						 //It will be pushed onto open if it remains false.
	mpStaticGoal.reset(new SNode);//Holds a copy of the goal node
}

bool CSearchDijkstra::FindPath(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path)
{
	start->cost = 0;//Cost is 0 as the path hasn't progressed yet
	start->score = 0; //Score is the same as cost in this algorithm.  Cost attribute will not be used.
	mpOpenList.push_back(move(start)); //Move the start node onto the front of openlist
	*mpRawCurrent = GetRawPtr(mpOpenList.front()); //The raw pointer is given the value of the raw version of the first node on open list.

	while (!mGoalReached) //Only executes when the goal node has not been reached.
	{
		//Returns false if their are no nodes on openList, making the search unsuccessful.
		if (mpOpenList.empty())
		{
			return false;
		}

		mpCurrent = move(mpOpenList.front()); //The first node of openList is given to current
		mpOpenList.pop_front(); //Clears space for the new first node of openList.

		//Assigns the raw version of current node to the raw pointer. This only occurs after the while loop has executed
		//more than once, the raw pointer is already assigned to the 1st node of open list as you can see just above
		//the loop.
		if (mIterations > 0)
		{
			mpRawCurrent = mpCurrent.get();
		}

		if (mpCurrent->x == goal->x && mpCurrent->y == goal->y)
		{
			mGoalReached = true;  //Goal node has now been reached.  Exit loop and construct path.
		}
		
		for (int i = 0; i < mNUMBER_OF_RULES; i++)
		{
			//Temp node created and initialised for the north, east, south, west rules.
			if (i == mNORTH)
			{
				AssignTempNode(mpN, goal, terrain, mNORTH);

			}
			else if (i == mEAST)
			{
				AssignTempNode(mpN, goal, terrain, mEAST);

			}
			else if (i == mSOUTH)
			{
				AssignTempNode(mpN, goal, terrain, mSOUTH);

			}
			else if (i == mWEST)
			{
				AssignTempNode(mpN, goal, terrain, mWEST);
			}
			mpN->parent = mpRawCurrent; //Assign the new node's parent with the raw current coordinate so that we can back track later
			ScanList(mpOpenList, goal, terrain);//Scan open list
			mpN->score = NewScore(mpN, terrain);//(Cumlative) Cost assigned for new node
	
			if (!mNEliminated)
			{
				mpOpenList.push_back(move(mpN)); //Pushes temp node onto openlist if all conditions have been met.
			}
			else
			{
				mNEliminated = false;//If the temp node was eliminated assign bool to false for next round.
			}
			mpN.reset(new SNode);//Allocate more data for next temp node.
		}
		sort(mpOpenList.begin(), mpOpenList.end(), CompareNodes);//Sort the open list, so lowest score is at the front.
																 //Hence, path with least resistance is found based on
																 //cumlative terrain cost

		mIterations++; //Amount of times the open list has been sorted/amount of loops.
		mpVisitedList.push_back(move(mpCurrent));
		mpCurrent.reset(new SNode);//Allocate more memory for the next current node.
	}
	ConstructPath(path);//Construct the path

	return true;  //The path has successfully been created.
}
int CSearchDijkstra::NewScore(SmartPointNode& newNode, TerrainMap& terrain)
{
	if (newNode->x < 0 || newNode->y < 0) //Checks for out of bounds nodes and eliminates them
	{
		mNEliminated = true;
		return -1;
	}
	else if ((unsigned)newNode->y >= terrain.size()) //Same as previous comment
	{

		mNEliminated = true;
		return -1;
	}

	for (unsigned int i = 0; i < terrain.size(); i++)//Checks for out of bounds on each row
	{
		if ((unsigned)newNode->x >= terrain[i].size())
		{
			mNEliminated = true;
			return -1;
		}
	}

	if (terrain[newNode->y][newNode->x] <= 0)
	{
		mNEliminated = true; //A wall has been hit so the node will be eliminated
		return -1;
	}
	


	if (!mNEliminated) //Give new node a score if it has been eliminated yet
	{
		return terrain[newNode->y][newNode->x] + mpCurrent->score; //Cumalative cost of terrain
	}
	else
	{
		return -1;
	}
}

void CSearchDijkstra::ScanList(NodeList& unscannedOpenList, SmartPointNode& goal, TerrainMap& terrain)
{
	//Scan through the openlist
	for (auto it = unscannedOpenList.begin(); it != unscannedOpenList.end(); it++)
	{
		if (mpN->x == (*it)->x && mpN->y == (*it)->y)//n node equal to existing node
		{
			if (mpN->score >= (*it)->score)
			{
				mNEliminated = true;//New node is eliminated if it is equal to or bigger than exisiting node.
			}
			if (!mNEliminated)
			{
				mpN = move(*it); //New node refers to existing node if it is the same coordinate
				unscannedOpenList.erase(it);
				break;
			}
		}
	}
	for (auto it = mpVisitedList.begin(); it != mpVisitedList.end(); it++)
	{
		if (mpN->x == (*it)->x && mpN->y == (*it)->y)
		{
			mNEliminated = true;//Eliminates nodes which have already been visited
		}
	}
}

void CSearchDijkstra::AssignTempNode(SmartPointNode& temp, SmartPointNode& goal, TerrainMap terrain, int direction)
{
	//A node is checked above, below, right and left of current (no diagonals)
	//Each are given their coordinates.
	//e.g. north is above current, so north's y value would increase by 1.
	if (direction == mNORTH)
	{
		temp->y = mpCurrent->y + 1;
		temp->x = mpCurrent->x;
	}
	else if (direction == mEAST)
	{
		temp->y = mpCurrent->y;
		temp->x = mpCurrent->x + 1;
	}
	else if (direction == mSOUTH)
	{
		temp->y = mpCurrent->y - 1;
		temp->x = mpCurrent->x;
	}
	else if (direction == mWEST)
	{
		temp->y = mpCurrent->y;
		temp->x = mpCurrent->x - 1;
	}
}

void CSearchDijkstra::DisplayList(NodeList& aList)
{
	for (unsigned int i = 0; i < aList.size(); i++)
	{
		//Outputs the desired list to console.
		cout << aList[i]->x << mSpace << aList[i]->y << " -> " << aList[i]->score << endl;
	}
	cout << endl;
}

void CSearchDijkstra::ConstructPath(NodeList& aPath)
{
	//The path starts from the last value of the closed list. ie the start node.
	*mpRawThePath = GetRawPtr(mpVisitedList.back());
	while (mpRawThePath != 0) //Loops until a NULL pointer is reached. The start node's parent is NULL.
						   //From this, we can keep back tracking until we reach a null value
	{
		SmartPointNode tmp(new SNode);
		*tmp = *mpRawThePath; //The path(raw ptr) is assigned to a smart pointer.
							 //Each node will be pushed to the front of the path, so that index 0 is the start node.
		aPath.push_front(move(tmp));
		mpRawThePath = mpRawThePath->parent;  //Gets the chain of parents
	}
}



bool CSearchDijkstra::Iterate(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path, NodeList& open, NodeList& close, NodeList& path2Current)
{	
	//Only executes when Iterate is called once
	if (mIterations == 0)
	{
		start->cost = 0;//Cost is 0 as the path hasn't progressed yet
		start->score = start->cost;
		mpOpenList.push_back(move(start)); //Move the start node onto the front of openlist
		*mpRawCurrent = GetRawPtr(mpOpenList.front()); //The raw pointer is given the value of the raw version of the first node on open list.
		mpStaticGoal->x = goal->x;
		mpStaticGoal->y = goal->y;
	}

	if (!mGoalReached) //Only executes when the goal node has not been reached.
	{
		//Returns false if their are no nodes on openList, making the search unsuccessful.
		if (mpOpenList.empty())
		{
			return false;
		}

		mpCurrent = move(mpOpenList.front()); //The first node of openList is given to current
		Path2Current();
		mpOpenList.pop_front(); //Clears space for the new first node of openList.

		//Assigns the raw version of current node to the raw pointer. This only occurs after the while loop has executed
		//more than once, the raw pointer is already assigned to the 1st node of open list as you can see just above
		//the loop.
		if (mIterations > 0)
		{
			mpRawCurrent = mpCurrent.get();
		}

		if (mpCurrent->x == mpStaticGoal->x && mpCurrent->y == mpStaticGoal->y)
		{

			mGoalReached = true;  //Goal node has now been reached.  Exit loop and construct path.
		}


		for (int i = 0; i < mNUMBER_OF_RULES; i++)
		{
			//Temp node created and initialised for the north, east, south, west rules.
			if (i == mNORTH)
			{
				AssignTempNode(mpN, mpStaticGoal, terrain, mNORTH);

			}
			else if (i == mEAST)
			{
				AssignTempNode(mpN, mpStaticGoal, terrain, mEAST);

			}
			else if (i == mSOUTH)
			{
				AssignTempNode(mpN, mpStaticGoal, terrain, mSOUTH);

			}
			else if (i == mWEST)
			{
				AssignTempNode(mpN, mpStaticGoal, terrain, mWEST);
			}

			mpN->parent = mpRawCurrent;
			ScanList(mpOpenList, mpStaticGoal, terrain);//Scan openlist
			mpN->score = NewScore(mpN, terrain);//(Cumlative) Cost assigned for new node
			if (!mNEliminated)
			{
				mpOpenList.push_back(move(mpN)); //Pushes temp node onto openlist if all conditions have been met.
			}
			else
			{
				mNEliminated = false;//If the new node was eliminated assign it to false for next round.
			}
			mpN.reset(new SNode);//Allocate more data for next temp node.
		}
		sort(mpOpenList.begin(), mpOpenList.end(), CompareNodes);//Sort the open list, so lowest cost is at the front.
															

		mIterations++; //Amount of times the open list has been sorted/amount of loops.

		//Copy lists for each iteration so that it can be displayed in the visual representation
		CopyList(mpOpenList, open);
		CopyList(mpVisitedList, close);
		CopyList(mpIteratePath, path2Current);

		mpVisitedList.push_back(move(mpCurrent));//Push the current node onto closed list as it has now been visited.
		mpCurrent.reset(new SNode);//Allocate more memory for the next current node.
	}

	if (mGoalReached)
	{
		ConstructPath(path);//Construct the path
		return true;
	}
	return false;
}

void CSearchDijkstra::Path2Current()
{
	mpIteratePath.clear(); //Get rid of nodes from previous iteration
	mpRawIteratePath = mpCurrent.get(); //Get the raw pointer of current
	//Construct path to current until a NULL pointer is reached.  
	//Similar to construct path
	while (mpRawIteratePath != 0)
	{
		SmartPointNode tmp(new SNode);
		*tmp = *mpRawIteratePath; //Assign smart pointer to raw pointer
		mpIteratePath.push_front(move(tmp)); //Push node onto front of list, so the path is the 
											 //right way around
		mpRawIteratePath = mpRawIteratePath->parent; //Get the chain of parents
	}
}

void CSearchDijkstra::CopyList(NodeList& existing, NodeList& newList)
{
	//Copy existing list onto a new list
	//so progression can be shown in visual representation
	for (unsigned int i = 0; i < existing.size(); i++)
	{
		SmartPointNode tmp(new SNode);
		tmp->x = existing[i]->x;
		tmp->y = existing[i]->y;
		newList.push_back(move(tmp));
	}
}

CSearchDijkstra::~CSearchDijkstra()
{
	//Deallocation of memory for raw pointers.
	delete mpRawThePath;
	delete mpRawCurrent;
	delete mpRawIteratePath;
}