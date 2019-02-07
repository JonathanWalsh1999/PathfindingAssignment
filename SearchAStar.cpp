//Jonathan Walsh
#include "SearchAStar.h"//Declarations

CSearchAStar::CSearchAStar()
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
	mpStaticGoal.reset(new SNode);
	mpRawIteratePath = new SNode;

}

bool CSearchAStar::FindPath(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path)
{	
	start->cost = 0;//Cost is 0 as the path hasn't progressed yet
	start->score = ManhattanDistance(goal, start); //Distance from start to goal
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
			mpN->cost = NewCost(mpN, terrain);//(Cumlative) Cost assigned for temp node
			ScanLists(mpOpenList, mpClosedList, goal, terrain);//Scan both lists.  Remove nodes from closed list, 
															   //eliminate nodes and assign score and parent for temp node.
			if (!mNEliminated)
			{
				mpOpenList.push_back(move(mpN)); //Pushes temp node onto openlist if all conditions have been met.
			}
			else
			{
				mNEliminated = false;//If the temp node was eliminated assign it to false for next round.
			}
			mpN.reset(new SNode);//Allocate more data for next temp node.
		}
		sort(mpOpenList.begin(), mpOpenList.end(), CompareNodes);//Sort the open list, so lowest score is at the front.
																 //Hence, path with least resistance is found based on
																 //heuristic(manhattan distance current->goal) and
																 //cumlative terrain cost

		mIterations++; //Amount of times the open list has been sorted/amount of loops.

		mpClosedList.push_back(move(mpCurrent));//Push the current node onto closed list as it has now been visited.
		mpCurrent.reset(new SNode);//Allocate more memory for the next current node.
	}

	ConstructPath(path);//Construct the path
	cout << mOpenListSorted << mIterations << endl; //Shows how many time open list has been sorted.

	return true;  //The path has successfully been created.
}

int CSearchAStar::ManhattanDistance(SmartPointNode& goal, SmartPointNode& newNode)
{
	//The distance between the new node and the goal node
	int x = abs(goal->x - newNode->x); //Negative values are converted to positive values.
	int y = abs(goal->y - newNode->y);

	return x + y;
}

int CSearchAStar::NewCost(SmartPointNode& newNode, TerrainMap& terrain)
{
	if (newNode->x < 0 || newNode->y < 0) //Out of bounds checking
	{
		mNEliminated = true;
		return -1;
	}
	else if( (unsigned)newNode->y >= terrain.size())
	{

		mNEliminated = true; 
		return -1;
	}
	else if (terrain[newNode->y][newNode->x] <= 0)
	{
		mNEliminated = true; //A wall has been hit so the node will be eliminated
		return -1;
	}

	for (unsigned int i = 0; i < terrain.size(); i++)//Out of bounds checking
	{
		if ((unsigned)newNode->x >= terrain[i].size())
		{
			mNEliminated = true;
			return -1;
		}
	}
	return terrain[newNode->y][newNode->x] + mpCurrent->cost; //Cumalative cost of terrain
}

void CSearchAStar::ScanLists(NodeList& unscannedOpenList, NodeList& unscannedClosedList, SmartPointNode& goal, TerrainMap& terrain)
{
	//Scan through the openlist
	for (auto it = unscannedOpenList.begin(); it != unscannedOpenList.end(); it++)
	{
		mpN->parent = mpRawCurrent; //Assign the new node with the raw current coordinate
		//Assigns score to the new node using cumlative cost + heuristic
		mpN->score = mpN->cost + ManhattanDistance(goal, mpN);

		if (mpN->x == (*it)->x && mpN->y == (*it)->y)//n node equal to existing node
		{
			if (mpN->score >= (*it)->score)
			{
				mNEliminated = true;//New node is eliminated if it is equal to or bigger than exisiting node.
			}
			if (!mNEliminated)
			{
				mpN = move(*it);//new node is referred to existing node
				unscannedOpenList.erase(it);
				break;
			}
		}
	}

	//Scan through the closed list
	for (auto it = unscannedClosedList.begin(); it != unscannedClosedList.end(); it++)
	{
		if (mpN->x == (*it)->x && mpN->y == (*it)->y)//n node is same coordinates to existing node
		{
			if (mpN->score >= (*it)->score)//Score of n is greater than or equal to existing node
			{
				mNEliminated = true;//n is eliminated from search
			}
			if (!mNEliminated)
			{
				mpN = move(*it);//the new node becomes the exisiting node and ready to be pushed onto back of openlist
				unscannedClosedList.erase(it);//Node is removed from closed list
				mNEliminated = true;
				break;//Break out of loop to finish scan
			}
		}
	}

}

void CSearchAStar::AssignTempNode(SmartPointNode& temp, SmartPointNode& goal, TerrainMap terrain, int direction)
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

void CSearchAStar::DisplayList(NodeList& aList)
{
	for (unsigned int i = 0; i < aList.size(); i++)
	{
		//Outputs the desired list to console.
		cout << aList[i]->x << mSpace << aList[i]->y << " -> " << aList[i]->score << " -> " << aList[i]->cost << endl;
	}
	cout << endl;
}

void CSearchAStar::ConstructPath(NodeList& aPath)
{
	//The path starts from the last value of the closed list. ie the start node.
	*mpRawThePath = GetRawPtr(mpClosedList.back()); 
	while (mpRawThePath != 0) //Loops until a NULL pointer is reached. The start node's parent is NULL.
	{
		SmartPointNode tmp(new SNode);
		*tmp = *mpRawThePath; //The path is assigned to a smart pointer.
		//Each node will be pushed to the front of the path, so that index 0 is the start node.
		aPath.push_front(move(tmp));
		mpRawThePath = mpRawThePath->parent;  //Gets the chain of parents
	}
}

bool CSearchAStar::Iterate(TerrainMap& terrain, SmartPointNode start, SmartPointNode goal, NodeList& path, NodeList& open, NodeList& close, NodeList& path2Current) 
{
	//Only executed aftrer first call of iterate
	if (mIterations == 0)
	{
		start->cost = 0;//Cost is 0 as the path hasn't progressed yet
		start->score = ManhattanDistance(goal, start); //Distance from start to goal
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
			mpN->cost = NewCost(mpN, terrain);//(Cumlative) Cost assigned for temp node
			ScanLists(mpOpenList, mpClosedList, mpStaticGoal, terrain);	//Scan both lists.  Remove nodes from closed list, 
																		//eliminate nodes and assign score and parent for temp node.
			if (!mNEliminated)
			{
				mpOpenList.push_back(move(mpN)); //Pushes temp node onto openlist if all conditions have been met.
			}
			else
			{
				mNEliminated = false;//If the temp node was eliminated assign it to false for next round.
			}
			mpN.reset(new SNode);//Allocate more data for next temp node.
		}
		sort(mpOpenList.begin(), mpOpenList.end(), CompareNodes);//Sort the open list, so lowest score is at the front.
																 //Hence, path with least resistance is found based on
																 //heuristic(manhattan distance current->goal) and
																 //cumlative terrain cost

		mIterations++; //Amount of times the open list has been sorted/amount of loops.


		//Copy lists for each iteration so that it can be displayed in the visual representation
		CopyList(mpOpenList, open);
		CopyList(mpClosedList, close);
		CopyList(mpIteratePath, path2Current);

		mpClosedList.push_back(move(mpCurrent));//Push the current node onto closed list as it has now been visited.
		mpCurrent.reset(new SNode);//Allocate more memory for the next current node.
	}

	if (mGoalReached)
	{
		ConstructPath(path);//Construct the path
		cout << mOpenListSorted << mIterations << endl; //Shows how many time open list has been sorted.
		return true;
	}
	return false;
}

void CSearchAStar::Path2Current()
{
	mpIteratePath.clear(); //Clears lists after every iteration
	mpRawIteratePath = mpCurrent.get(); //Get raw pointer of current
	//Keeps looping until a parent equals 0, ie the parent of start
	//Similar to ConstructPath()
	while (mpRawIteratePath != 0) 
	{
		SmartPointNode tmp(new SNode);
		*tmp = *mpRawIteratePath; //Raw assigned to smart pointer
		mpIteratePath.push_front(move(tmp)); //node pushed onto front of list so that the nodes are the right way around
		mpRawIteratePath = mpRawIteratePath->parent; //Gets the parent of each node until it reaches start
	}
}

void CSearchAStar::CopyList(NodeList& existing, NodeList& newList)
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

CSearchAStar::~CSearchAStar()
{
	//Deallocation of memory for raw pointers.
	delete mpRawThePath;
	delete mpRawCurrent;
	delete mpRawIteratePath;
}
