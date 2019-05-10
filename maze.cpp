#include <bits/stdc++.h>
#include <unistd.h>
#include <termios.h>

#include <GL/gl.h>
#include <GL/glut.h>

#include "makeMaze.cpp"

float light_intensity = 0.0;

//Load texture from .bmp file (size not too big)
GLuint LoadTexture(const char *fileName){
    FILE *file;
    unsigned char header[54],*data;
    unsigned int dataPos, size, width, height;
    file = fopen(fileName, "rb");
    fread(header, 1, 54, file);
    dataPos=*(int*)&(header[0x0A]);
    size=*(int*)&(header[0x22]);
    width=*(int*)&(header[0x12]);
    height=*(int*)&(header[0x16]);
    if (size == NULL)
        size=width * height * 3;
    if (dataPos == NULL)
        dataPos = 54;
    data = new unsigned char[size];
    fread(data, 1, size, file);
    fclose(file);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
    return texture;
}

//Draw, rotate, input wrappers, etc..
class maze_functions
{
	private:
		createMaze theMaze;  	//The main maze object
		float yRot;  			//Mouse controlled Y Rotation
		int prevX;				//Used in Camera Rotaion
		int yCounter;			//			"
		int yChecker;			//			"
		double zoom;  			//Controls mouse click zoom out
		int cameraDir;  		//Camera Direction
		int goalSpin;  			//Handles ring rotation
		int height; 			//Viewport height
		int width; 				//Viewport width
	
		//Draw the walls of the maze with boxes
		void draw_wall(float x1, float y1, float z1, float x2, float y2, float z2);	
		void draw(); 
	
		void orientRight();		//Based on yRot, change controls
		void orientLeft();		//			"
	
		//Drawing the score and winner message
		void beginText();
		void displayScore(float x, float y, float z, string score);
		void displayText(float x, float y, float z, string score);
		void endText();

	public:
		maze_functions(float mazeSize, int w, int h);
		maze_functions();
		void display();
		void reshapeViewport(int w, int h);	
	
		//Handle user input from mouse and keyboard
		void mouse(int x, int y);
		void mouse2(int button, int state, int x, int y);
		void keyboard(int key);
		
		void spinGoal();	//Spins the artefact
		int playerObj;		//Holds the value on which shape will be used	
};

//Begin stacking matrixes for the score display
void maze_functions::beginText(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
}

//Draw the score
void maze_functions::displayScore(float x, float y, float z, string score)
{
	glRasterPos3f(x, y, z);
	for(int counter = 0; score[counter] != '\0'; counter++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, score[counter]);
}

//Draw winner!
void maze_functions::displayText(float x, float y, float z, string winner)
{
	glPushMatrix();
	glColor3f(1.0,1.0,0.0);
	glRasterPos3f(x, y, z);
	glTranslatef(x, y, z);
	glScalef(0.5,0.5,1);
	for (int counter = 0; winner[counter] != '\0'; counter++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, winner[counter]);

	glPopMatrix();
}

//Finish displaying text
void maze_functions::endText(){
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//Rotate the goal object
void maze_functions::spinGoal(){
	goalSpin += 25;

	if (goalSpin > 360)
		goalSpin = goalSpin - 360;

	glutPostRedisplay();
}

//maze_functions Constructor 
maze_functions::maze_functions(float mazeSize, int w, int h){
	theMaze = createMaze(mazeSize, mazeSize);

	height = h; 
	width = w;
	zoom = 7;

	yRot = 0; 
	goalSpin = 0; 
	cameraDir = 0;
	
	prevX=0; 
	yCounter=0; 
	yChecker=0;
}

//Check win
int flag = 0;

void maze_functions::display(){
	//Clear the screen
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//Set the camera position
	gluLookAt(0,sin(44.7)*zoom, cos(44.7)*zoom,  0,0,0,  0,1,-1);

	//Handle the mouse controlled Y rotation
	glRotatef(yRot, 0, 1, 0);
	draw();
	glPopMatrix();
	
	//Display score
	beginText();
	if(theMaze.score == 3 && flag==0){
		displayText(100, height-150, 0, "YOU WON!!");
		displayText(5, height-300, 0, "Resume in 3s");
		displayText(50, height-450, 0, "F1 to exit.");
		theMaze.score++;
	}
	else{	
		if(theMaze.score == 4 && flag==0){
			usleep(3000000);
			theMaze.score--;flag++;
		}
		displayScore(10, height-25, 0, "Find Object!!");
		displayScore(10, height-50, 0, theMaze.scoreString());
		displayScore(10, height-75, 0, "Press F1 to exit!");
	}
	endText();

	glutSwapBuffers();
}


void maze_functions::keyboard(int key){

	switch(key){
		case GLUT_KEY_UP:  //move ball up.
			theMaze.movePlayer((0+cameraDir)%4);
			glutPostRedisplay();
			break;
		case GLUT_KEY_LEFT:  //move ball left.
			theMaze.movePlayer((3+cameraDir)%4);
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:  //move ball down.
			theMaze.movePlayer((2+cameraDir)%4);
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:  //move ball right.
			theMaze.movePlayer((1+cameraDir)%4);
			glutPostRedisplay();
			break;
		case GLUT_KEY_F1:	  //exit for F1
			system("kill `ps | grep 'aplay'|awk '{print $1}'`") ;
			exit(0);	
		case GLUT_KEY_F2:
			light_intensity += 0.1;
		case GLUT_KEY_F3:
			light_intensity -= 0.1;
  	}
}
void maze_functions::mouse (int x, int y){
	//Clockwise/Anti-Clockwise
	int dir = 0;

	//Dragging right to left
	if(yChecker > x){
		yCounter += 1;
		dir = 1;
	}
	//Dragging left to right
	else{
		yCounter -=1;
		dir = 2;
	}
	yRot = prevX+yCounter;
	//Rotate the camera and change control orientation based on angle
	if(dir==1){
		if(yRot<=0)
			yRot = 360 - yRot;
		if(yRot>=360)
			yRot = yRot - 360;
		if(yRot == 45 || yRot == 135 || yRot == 225 || yRot == 315)
			orientLeft();	//Synchronize controls
	}
	if(dir==2){
		if(yRot<=0)
			yRot = 360 - yRot;
		if(yRot>=360)
			yRot = yRot - 360;
		if(yRot == 45 || yRot == 135 || yRot == 225 || yRot == 315)
			orientRight();	//Synchronize controls
	}
	
	yChecker=x;
}
void maze_functions::mouse2(int button, int state, int x, int y){
	//Rotate with mouse left click
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		prevX = yRot;
		yCounter = 0;
	}
	//Zoom function
	if(button == 3){
		zoom++;
	}
	if(button == 4){
		zoom--;
	}
}

//Function used to draw the walls on screen. Renders the left, right, front, back, and top segments
inline void maze_functions::draw_wall(float x1, float y1, float z1, float x2, float y2, float z2){
	glNormal3f(-1, 0, 0);	//Left Side
		glVertex3f(x1, y1, z1); 
		glVertex3f(x1, y1, z2); 
		glVertex3f(x1, y2, z2); 
		glVertex3f(x1, y2, z1);
	glNormal3f(1, 0, 0);	//Right Side
		glVertex3f(x2, y1, z2);	
		glVertex3f(x2, y1, z1); 
		glVertex3f(x2, y2, z1); 
		glVertex3f(x2, y2, z2);
	glNormal3f(0, 0, 1);	//Front Side
		glVertex3f(x1, y1, z2); 
		glVertex3f(x2, y1, z2); 
		glVertex3f(x2, y2, z2); 
		glVertex3f(x1, y2, z2);
	glNormal3f(0, 0, -1);	//Back Side
		glVertex3f(x1, y1, z1); 
		glVertex3f(x1, y2, z1); 
		glVertex3f(x2, y2, z1);	
		glVertex3f(x2, y1, z1);
	glNormal3f(0, 1, 0); 	//Top Side
		glVertex3f(x1, y2, z1); 
		glVertex3f(x1, y2, z2); 
		glVertex3f(x2, y2, z2); 
		glVertex3f(x2, y2, z1);
}

//Makes sphere, artifact(icosahedron), loads texture on sphere, draws the floor and all the walls
void maze_functions::draw(){
	glTranslatef(0,.5, 0);
	
	GLuint m_TextureID;
	m_TextureID = LoadTexture("image6.bmp");

	glEnable (GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_TextureID);

	GLUquadricObj *sphere=NULL;
	sphere = gluNewQuadric();
	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, true);
	gluQuadricNormals(sphere, GLU_SMOOTH);
	//Making a display list
	GLuint mysphereID = glGenLists(1);
	glNewList(mysphereID, GL_COMPILE);
	gluSphere(sphere, 0.4f, 13, 26);
	glEndList();
	gluDeleteQuadric(sphere);
	glColor3f(1, 1, 1);	
	glCallList(mysphereID);
	
	glDisable (GL_TEXTURE_2D);

	//glutSolidSphere(0.32f, 13, 26);

	glTranslatef(-.4-theMaze.findPlayer().x, -0.5, -.4-theMaze.findPlayer().y);

	glBegin(GL_QUADS);
		//glEnable (GL_TEXTURE_2D);
		//Color of the floor of the maze
		glColor3f(0, 0, 0);
		
		float mazeHeight = theMaze.height();
		float mazeWidth = theMaze.width();
	
		//Floor square
		draw_wall(0, -0.01, -0.2, mazeWidth, 0, mazeHeight-0.2);
	
		//Color of the walls of the maze
		glColor3f(0,0,1);

		//Draw the left and bottom border of the maze
		draw_wall(-0.2, 0, -0.2, 0, 1, mazeHeight-.2); 
		draw_wall(-0.2, 0, mazeHeight-.2, mazeWidth, 1, mazeHeight); 
	glEnd();
	
	
	//Draws all walls of maze
	for (int counter = 0; counter < mazeHeight; counter++){
		for (int counter2 = 0; counter2 < mazeWidth; counter2++){
			glBegin(GL_QUADS);
				//glEnable (GL_TEXTURE_2D);
				draw_wall(0.8, 0.0, -0.2, 1.0, 1.0, 0.0);
				if (!theMaze.stepChecker(0, location(counter2, counter)))
					draw_wall(0.0, 0.0, -0.2, 0.8, 1.0, 0.0);
				if (!theMaze.stepChecker(1, location(counter2, counter)))
					draw_wall(0.8, 0.0, 0.0, 1.0, 1.0, 0.8);
			glEnd();
			glTranslatef(1, 0, 0);
		}
		glTranslatef(-mazeWidth, 0, 1);
	}
	
	//glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glTranslatef(theMaze.findGoal().x+0.4, 0.5, theMaze.findGoal().y-theMaze.height()+0.4);
	glColor3f(1, 0, 0); 
	glRotatef(goalSpin, 0, 1, 0);
	glScalef(0.5,0.5,0.5);
	glutSolidIcosahedron();
	glPopMatrix();
	glPopMatrix();
}

//Synchronize controls with rotation
void maze_functions::orientRight(){
	if (cameraDir == 0)
		cameraDir = 3;
	else
		cameraDir--;
}

//Synchronize controls with rotation
void maze_functions::orientLeft(){
	if(cameraDir == 3)
		cameraDir = 0;
	else
		cameraDir++;
}

void maze_functions::reshapeViewport(int w, int h){
	height = h;
	width = w;
}

maze_functions::maze_functions(){
	return;
}

