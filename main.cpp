//To run:  g++ main.cpp -lGL -lGLU -lglut -w
//											3D - Maze - Game
// By Anurag Verma - 160001005 && Aravind Ravikumar 160001006
#include <bits/stdc++.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "maze.cpp"

using namespace std;

maze_functions Maze;

//Wrapper functions to handle user input 
void displayWrapper();
void mouseWrapper(int x, int y);
void mouse2Wrapper(int button, int state, int x, int y);
void spinGoalWrapper(int temp);
void arrowKeysWrapper(int key, int x, int y);

int theSize=7;

int main(int argc, char** argv){	
	
	Maze = maze_functions(theSize, 500, 500);

	system("kill `ps | grep 'aplay'|awk '{print $1}'`") ;	//Stop any previous bgm
	system("aplay bgm.wav&") ;			//to play sound

	//Create the glut window
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (900, 900);
	glutInitWindowPosition (0, 0);
	glutCreateWindow ("3D Maze Game");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.0, 1.0, 400.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Enabling some default functions to remove hidden surfaces,
	//and add the lighting, all built into glut
	glEnable(GL_DEPTH_TEST); glEnable(GL_CULL_FACE);  
	glEnable(GL_LIGHTING); glEnable(GL_COLOR_MATERIAL);

	//Set lighting parameters
	GLfloat ambientLight[] = 
		{ 0.1f, 0.1f, 0.1f, 0.1f }; 
	GLfloat diffuseLight[] = 
		{ 0.8f, 0.8f, 0.8f, 0.0f };
	GLfloat specular[] = 
		{ 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightPos0[] = 
		{ 100.0f, 125.0f, 200.0f, 1.0f };

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	//Implement lighting paramets
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight); glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular); glLightfv(GL_LIGHT0,GL_POSITION,lightPos0);
	glEnable(GL_LIGHT0);

	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glClearColor(0.3f, 0.3f, 0.3f,1.0f);

	//Handle all user input through mouse and keyboard
	glutDisplayFunc(displayWrapper); 
	glutMotionFunc(mouseWrapper);
	glutMouseFunc(mouse2Wrapper); 
	glutSpecialFunc(arrowKeysWrapper);
	//Spin icosahedron
	glutTimerFunc(100, spinGoalWrapper, 0);
	
	glLineWidth(7);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	
	glutMainLoop();

	return 0;	
}

void spinGoalWrapper(int temp){
	Maze.spinGoal();
	//Determines how fast the goal object is rotating
	glutTimerFunc(80, spinGoalWrapper, 0);
}

//Wrapper functions to handle user input properly
void arrowKeysWrapper(int key, int x, int y){
	Maze.keyboard(key);
}

void displayWrapper(){
	Maze.display();
}

void mouseWrapper (int x, int y){
	Maze.mouse(x, y);
}

void mouse2Wrapper (int button, int state, int x, int y){
	Maze.mouse2(button, state, x, y);
}


