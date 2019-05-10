#include <bits/stdc++.h>

#include <GL/gl.h>
#include <GL/glut.h>

using namespace std;

//Co-ordinate in maze
class location {
public:
	float x;
	float y;
	location(float first, float second){
		x = first;
		y = second;
	}
	location(){
		x = NULL;
		y = NULL;
	}
};

//Check if wall present in location after maze creation
class check {
	public:
		bool upClear;
		bool rightClear;

		check(bool up, bool right){
			upClear = up;
			rightClear = right;
		}
		
		check(){
			upClear = NULL;
			rightClear = NULL;
		}
};


class createMaze
{
	private:
		vector<vector<check> > theMaze;					//All walls in maze [2D check matrix]
		vector<vector<int> > setChecker;				//The set information of theMaze
		location goal;									//Locaiton of the artifact
		location player;								//Location of the player
		void resetMaze();								//regenerates the maze after artifact is found
		location move(location start, int dir);			//Handles player movement
	public:
		int score;											//Player's score out of 5
		createMaze(float height, float width);				//Creates theMaze object with height and width
		bool stepChecker(unsigned int dir, location start);	//Determines if allowed to move and checks for collision
		void movePlayer(int dir);							//Actual moving of the player object
		location findPlayer();								
		float height();
		float width();
		location findGoal();
		createMaze();
		string scoreString();
		bool makeMaze(location start);
};


createMaze::createMaze(float height, float width){
	goal = location(width-1, height-1);		//Start the goal in the far corner
	player = location(0.0,0.0);				//Start the player in the opposite corner
	srand(time(0));
	score = 0;								//Start the score at 0
	
	//Set the maze height
	theMaze.resize(height);			
	setChecker.resize(height);
	
	//Set the maze width, fill with walls in actual maze, fill
	//with sequential numbers for pseudo-maze
	for (int counter = 0; counter < height; counter++)
	{
		theMaze[counter].resize(width, check(false, false));
		setChecker[counter].resize(width, NULL);

		for (int counter2 = 0; counter2 < width; counter2++)
			setChecker[counter][counter2] = (counter*width)+counter2;
	}

	//Iterate randomly through every wall in the maze and 
	//clear a path through the walls using Kruskal's algorithm,
	//implemented in the makeMaze function
	for (int counter = 0; counter < theMaze.size()*theMaze[0].size()*10; counter++)
		makeMaze(location(rand()%(int)theMaze[0].size(), rand()%(int)theMaze.size()));
}

//Collision Function 
bool createMaze::stepChecker(unsigned int dir, location start){
	//Check for up direction
	if (dir == 0)
		return theMaze[start.y][start.x].upClear;

	//Check for right direction
	else if (dir == 1)
		return theMaze[start.y][start.x].rightClear;

	//Check for down direction
	else if (dir == 2 & start.y < theMaze.size()-1)
		return theMaze[start.y+1][start.x].upClear;

	//Check for left direction
	else if (dir == 3 & start.x > 0)
		return theMaze[start.y][start.x-1].rightClear;
	return false;
}

//After checking for collision move 
//the player in the corresponding direction
location createMaze::move(location start, int dir){
	//Move up
	if (dir == 0)
		return location(start.x, start.y-1);

	//Move right
	else if (dir == 1)
		return location(start.x+1, start.y);

	//Move down
	else if (dir == 2)
		return location(start.x, start.y+1);

	//Move left
	else if (dir == 3)
		return location(start.x-1, start.y);
	else
		return start;
}

bool createMaze::makeMaze(location start){
	int dir = rand()%2;								//Random up or right
	int setID = setChecker[start.y][start.x];	
	int newSetID;
	
	//Check to make sure the border of the maze stays solid
	if ((dir == 0 && start.y == 0) || (dir == 1 && start.x == theMaze[0].size()-1))
		return false;
		
	//Depending on the random, check if the upper or right wall should be 
	//removed based on previous removals
	if (dir == 0){
		newSetID = setChecker[start.y-1][start.x];
		if (newSetID == setID)
			return false;
		theMaze[start.y][start.x].upClear = true;
	}

	if (dir == 1){
		newSetID = setChecker[start.y][start.x+1];
		if (newSetID == setID)
			return false;
		theMaze[start.y][start.x].rightClear = true;
	}

	//Here renumber the big numbered-grid to start creating sets
	for (int row = 0; row < theMaze.size(); row++)
		for (int column = 0; column < theMaze[0].size(); column++)
			if (setChecker[row][column] == setID)
				setChecker[row][column] = newSetID;
}

//Recreates maze with larger sides
void createMaze::resetMaze(){

	//Set the maze height
	theMaze.resize(height()+2);			
	setChecker.resize(height()+2);
	
	//Set the maze width, fill with walls in actual maze, fill with sequential numbers for pseudo-maze
	for (int counter = 0; counter < height(); counter++)
	{
		theMaze[counter].resize( height(), check(false, false));
		setChecker[counter].resize( height(), NULL);

		for (int counter2 = 0; counter2 <  height(); counter2++)
			setChecker[counter][counter2] = (counter* height())+counter2;
	}

	//Placing all the walls back in the maze
	for (int row = 0; row < theMaze.size(); row++)
		for (int column = 0; column < theMaze[0].size(); column++){
			theMaze[row][column].upClear = false;
			theMaze[row][column].rightClear = false;
			setChecker[row][column] = (row*theMaze[0].size())+column;
		}

	//Iterate through every wall in the maze randomly and run Kruskal's algorithm to create a path
	for (int counter = 0; counter < theMaze.size()*theMaze[0].size()*10; counter++)
		makeMaze(location(rand()%(int)theMaze[0].size(), rand()%(int)theMaze.size()));
	
	//Place the goal object at a random location in the maze
	goal = location(rand()%(int)(height()-1), rand()%(int)(width()-1));

	//If intially the goal object is placed on the player object, reset again
	if(goal.x == player.x && goal.y == player.y)
		resetMaze();

	//Add to total score 
	score++;
}

//Move the player based on the permissions
void createMaze::movePlayer(int dir){
	if(stepChecker(dir, player)){
		player = move(player, dir);
	
		//If player reaches the artefact
		//reset the maze and increase score
		if(player.x == goal.x && player.y == goal.y){
			resetMaze();
		}
	}
}

location createMaze::findPlayer() {return player;}
float createMaze::height() {return theMaze.size();}
float createMaze::width() {return theMaze[1].size();}
location createMaze::findGoal() {return goal;}
createMaze::createMaze() {return;}

//Returns score to print
string createMaze::scoreString(){
	stringstream output;	
	output << "Your score: "<<score<<"/3";
	return output.str();
}







