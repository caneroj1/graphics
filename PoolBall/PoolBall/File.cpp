///////////////////////////////////////////////////////////////////////////////////////////////////
// Joe Canero, CSC350, Final Project
//
// This program is going to simulate the playing of a game of billiards. The cue ball starts at the center of the
// and the user's cue stick is denoted by a thick brown line between the location of the mouse and the center of
// the cue ball. Clicking will hit the cue ball. The longer the cue stick becomes, the harder you will hit the cue
// ball.
//
// I've also tried to implement a quad tree with this program to handle collisions between the balls. It's probably
// not the best quad tree. :)
//
// I know you said that my proposal was unclear concerning my objectives, so can I make the following 5 items
// my objectives for this project?
//
// At this stage of programming, I have achieved a few of my five objectives.
// 1) DONE -- Implement collision checking between the balls on the table and the walls of the table
// 2) DONE -- Be able to get balls into pockets and remove them from play
// 3) DONE -- Be able to change the angle that the cue stick makes with the cue ball.
// 4) DONE -- Add texture to the balls, and to the table. Possibly look into making the walls look like wood. That didn't happen, but I added a grass
//            texture to the table to make it look something like felt.
// 5) DONE -- Add lighting to the simulation.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "PoolBall.h"
#include "ElasticCollision.h"
#include <cstdlib>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define PI 3.141596

using namespace std;

//Function declarations
void drawStick();                                           //declaration of function to draw the cue stick
void deleteBalls();                                         //declaration of function to mark balls so they are moved away when they go in a pocket
void drawBalls();                                           //declaration of function to draw the balls of the table
void drawPockets();                                         //declaration of function to draw the pockets
void drawPoolTable();                                       //declaration of function to draw the pool table
void increaseAngle();                                       //declaration of function to increase of the ball's rotation
void animate(int);                                          //declaration of the function that calls the proper movement functions if the ball should
                                                            //be moving
void moveBall(int&);                                        //declaration of function that finds the ball's new coordinates
void calculateVelocity(float, float);                       //declaration of function that computes the initial velocity of the ball after it is hit
void checkCollisions();                                     //declaration of function that checks if there is a collision happening at each unit of time

//Globals
GLUquadricObj *stick;                                       //pointer to the cue stick glu quadric object
static unsigned int texture[3];                             //Array of texture indices
bool scratch = false;                                       //variable to store the case of a scratch
double xC = 0.0;                                            //variable that stores the x-coordinate for passive motion tracking
double yC = 0.0;                                            //variable that stores the y-coordinate for passive motion tracking
double cueStartX = -7.0;                                    //x-coordinate of the cue ball's initial location
double cueStartY = 0.0;                                     //y-coordinate of the cue ball's initial location
double cueStickBeginX = 0.0;                                //x-coordinate of the begining of the pool stick (begins at user's mouse)
double cueStickBeginY = 0.0;                                //y-coordinate of the beginning of the pool stick (begins at user's mouse)
double cueStickEndX = 0.0;                                  //x-coordinate for the end of the pool stick (ends at the cue ball)
double cueStickEndY = 0.0;                                  //y-coordiante for the end of the pool stick (ends at the cue ball)
double cueStickLength = 0.0;                                //variable that maintains the length of the cue stick between shots
double cueAngle = 0.0;                                      //angle of the shot. angle that the cue stick is making with the cue ball
double shotAngle = 0.0;                                     //angle of rotation of the shot
double a = cueStartX;                                       //this variable is the x coordinate of the end of the pool cue
double b = cueStartY;                                       //this variable is the y coordinate of the end of the pool cue
float pocketConst = 11.75;                                  //constant that marks the location on the table where the ball goes into a pocket
double angle = 3.0;                                         //angle of the ball's rotation
float cueRadius = 0.75;                                     //radius of the cue ball
bool animating = false;                                     //condition determining if there is animation or not
int width = 750;                                            //width of viewing window
int height = 500;                                           //height of viewing window
int frustumWidth = 5;                                       //width of half of frustum
int frustumHeight = 5;                                      //height of half of frustum
int widthRatio = width/(frustumWidth * 2);                  //ratio of pixels of viewing screen to units of frustum width
int heightRatio = height/(frustumHeight * 2);               //ratio of pixels of viewing screen to units of frustum height
float pushIntoFrustumAmount = -15.0;                        //amount all objects are translated back into frustum
float ratioOfFarPlaneToNear = -pushIntoFrustumAmount/5.0;   //ratio of plane of translation to near plane
float rectSize = 13.0;                                      //size of pool table
float frictionCo = 0.00005;                                 //coefficient of friction on the pool table
float frictionAmount = 0;                                   //amount of friction at a given time
float testFloat = 0.0085;                                   //this float is used to test if the magnitude is sufficiently close to zero
                                                            //indicating the ball is almost coming to rest. making it smaller will make the ball
                                                            //move for longer, making it larger will stop motion prematurely.
int t = 0;                                                  //parameter of the time of motion
std::vector<PoolBall> PoolBalls;                            // a global vector of PoolBall objects
const int NUMBALLS = 2;

// Return 1 if the axes-parallel rectangle with diagonally opposite corners at (x1,y1) and (x2,y2)
// intersects the disc centered (x3,y3) of radius r, otherwise return 0.
int checkDiscRectangleIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float r)
{
    float minX, maxX, minY, maxY;
    
    // Set minX to smaller of x1 and x2, and maxX to the larger; likewise minY and maxY.
    if (x1 <= x2)
    {
        minX = x1; maxX = x2;
    }
    else
    {
        minX = x2; maxX = x1;
    }
    if (y1 <= y2)
    {
        minY = y1; maxY = y2;
    }
    else
    {
        minY = y2; maxY = y1;
    }
    
    // The disc intersects the rectangle if its center lies in the strip with corners at
    // (minX-r, minY) and (maxX+r, maxY), or if its center lies in the strip with corners
    // at (minX, minY-r) and (maxX, maxY+r), or if its center is within distance r of one
    // the four corners of the rectangles.
    if      ( (x3 >= minX - r) && (x3 <= maxX + r) && (y3 >= minY) && (y3 <= maxY) ) return 1;
    else if ( (x3 >= minX) && (x3 <= maxX) && (y3 >= minY - r) && (y3 <= maxY + r) ) return 1;
    else if ( (x3 - x1)*(x3-x1) + (y3 - y1)*(y3 - y1) <= r*r ) return 1;
    else if ( (x3 - x1)*(x3-x1) + (y3 - y2)*(y3 - y2) <= r*r ) return 1;
    else if ( (x3 - x2)*(x3-x2) + (y3 - y2)*(y3 - y2) <= r*r ) return 1;
    else if ( (x3 - x2)*(x3-x2) + (y3 - y1)*(y3 - y1) <= r*r ) return 1;
    else return 0;
}


// Quadtree node class.
class QuadtreeNode
{
public:
    QuadtreeNode(float x, float y, float s);
    int numberBallsIntersected(); // Return the number of balls intersecting the square.
    void addIntersectingBallsToList(); // Add the intersected balls to the local vector of balls.
    // if it intersects at most 10 balls leave it as a leaf and add the intersecting
    // balls, if any, to a local list of balls.
    void clear();   //recursively clears the nodes
    void build(); // Recursive routine to split a square that intersects more than 10 balls;
    void ballCollisions(); //do the collision checking routine for each ball. it is recursive if the node has children
    void drawBalls(); //execute the drawing routine for each ball. recursive if there are children
    
private:
    float SWCornerX, SWCornerY; // x and y co-ordinates of the SW corner of the square.
    float size; // Side length of square.
    QuadtreeNode *SWChild, *NWChild, *NEChild, *SEChild; // Children nodes.
    std::vector<PoolBall> ballList; // Local list of pool balls intersecting the square - only filled for leaf nodes.
    
    friend class Quadtree;
};


// QuadtreeNode constructor.
QuadtreeNode::QuadtreeNode(float x, float y, float s)
{
    SWCornerX = x; SWCornerY = y; size = s;
    SWChild = NWChild = NEChild = SEChild = NULL;
    ballList.clear();
}

// Return the number of balls intersecting the square.
int QuadtreeNode::numberBallsIntersected()
{
    int numVal = 0;
    for (std::vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
        if (iter->getRadius() > 0.0) {
            if ( checkDiscRectangleIntersection( SWCornerX, SWCornerY, SWCornerX+size, SWCornerY+size,
                                                iter->getXLocation(), iter->getYLocation(),
                                                iter->getRadius() )
                )
                numVal++;
        }
    }
    return numVal;
}

// Add the balls intersecting the square to a local list of pool balls.
void QuadtreeNode::addIntersectingBallsToList()
{
    for (std::vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
        if (iter->getRadius() > 0.0) {
            if ( checkDiscRectangleIntersection( SWCornerX, SWCornerY, SWCornerX+size, SWCornerY+size,
                                                iter->getXLocation(), iter->getYLocation(),
                                                iter->getRadius() )
                )
            ballList.push_back( *iter );
        }
    }
}

// Recursive routine to split a square that intersects more than 10 pool balls; if it intersects
// at most 17 balls leave it as a leaf and add the intersecting ball, if any, to a local
// list of balls.
void QuadtreeNode::build()
{
    if ( this->numberBallsIntersected() <= 17 ) this->addIntersectingBallsToList();
    else
    {
        SWChild = new QuadtreeNode(SWCornerX, SWCornerY, size/2.0);
        NWChild = new QuadtreeNode(SWCornerX, SWCornerY + size/2.0, size/2.0);
        NEChild = new QuadtreeNode(SWCornerX + size/2.0, SWCornerY + size/2.0, size/2.0);
        SEChild = new QuadtreeNode(SWCornerX + size/2.0, SWCornerY, size/2.0);
        
        SWChild->build(); NWChild->build(); NEChild->build(); SEChild->build();
    }
}

//recursive routine to draw each ball in a quadtree node's ball list. branches into the child nodes if they exist.
void QuadtreeNode::drawBalls() {
    if(SWChild == NULL) { //no children
        for (vector<PoolBall>::iterator iter1 = ballList.begin(); iter1 != ballList.end(); iter1++) {
            iter1->drawBall();
        }
    }
    else { //branch into the children and draw the pool balls in their ball lists
        SWChild->drawBalls();
        NWChild->drawBalls();
        SEChild->drawBalls();
        NEChild->drawBalls();
    }
}

//recursive routine to check the balls of each quadtree node's ball list if they are intersecting
//if the quadtree node has children, the routine will branch into the children and check the collisions there
void QuadtreeNode::ballCollisions() {
    if (SWChild == NULL) {
        
        //this square is a leaf, so we do not have to check the children, just the balls in this square
        // Create a second vector of PoolBalls
        // this vector will be used to ensure that there are no duplicate collision checks
        
        /*
         The PoolBall function ComputeElasticCollision only computes an elastic collision if the two balls are unique; that is, if
         the function call is ComptueElasticCollision(Ball1, Ball1), the function will not compute a collision between a ball and itself.
         That is one way redundancy is prevented.
         
         The second way redundancy is prevented is through the use of the second vector. Since after one full iteration of the inner loop,
         the ball at the location of the first iterator has been checked against every other ball, the first ball in the second vector is erased.
         
         Think of it this way:
         
         At the start of the loop, iter1 has b1, b2, b3,...,bN
         iter2 has b1, b2, b3,...,bN
         
         After the inner for loop completes one whole iteration, b1 from iter1 has been checked against b1, b2, b3,...,bN. If we erase b1
         from iter2, iter2 will now have b2, b3,...,bN. After the erase, iter1 increments and is now pointing to b2.
         
         Upon the second iteration of the inner loop, b2 will be checked against b2, b3,...,bN. This procedure effectively removes checking
         b2 against b1, which is a check that has already been accomplished.
         
         */
        
        int i = 0;
        int j = 0;
        std::vector<PoolBall> ballList2 = ballList;
        for (std::vector<PoolBall>::iterator iter1 = ballList.begin(); iter1 != ballList.end(); iter1++) {
            for (std::vector<PoolBall>::iterator iter2 = ballList2.begin(); iter2 != ballList2.end(); iter2++) {
                if(iter1->PoolBall::isColliding(*iter2)) ComputeElasticCollision(*iter1, *iter2);
                ballList[j+i] = ballList2[i];
                i++;
            }
            i = 0; j++;
            ballList2.erase(ballList2.begin());
        }
        
        //the main vector of pool balls, PoolBalls, was not being updated properly even though this function was updating the pool balls.
        //what was happening was that the function was only updating the local vectors, so this loop updates the vector PoolBalls with the results
        
        std::vector<PoolBall>::iterator iter2 = PoolBalls.begin(); int k = 0;
        for (std::vector<PoolBall>::iterator iter1 = ballList.begin(); iter1 != ballList.end(); iter1++) {
            if(iter1->getID() == iter2->getID()) {
                iter2->setXComponent(iter1->getXComponent());
                iter2->setYComponent(iter1->getYComponent());
                iter2->setZComponent(iter1->getZComponent());
            }
            k++;
            iter2++;
        }
    }
    else {
        SWChild->ballCollisions();
        NWChild->ballCollisions();
        SEChild->ballCollisions();
        NEChild->ballCollisions();
    }
}

void QuadtreeNode::clear() {
    if(SWChild != NULL) SWChild->clear();
    if(NWChild != NULL) NWChild->clear();
    if(SEChild != NULL) NEChild->clear();
    if(SEChild != NULL) SEChild->clear();
    delete this;
}


// Quadtree class.
class Quadtree
{
public:
    Quadtree() { header = NULL; } // Constructor.
    void initialize(float x, float y, float s); // Initialize quadtree by splitting nodes
    // till each leaf node intersects at
    // most one asteroid.
    void clearTree();   //clears the tree
    void checkCollisions(); //checks for collisions between the balls in the quadtree
    void drawBalls(); //starts the drawing routine for each ball

private:
    QuadtreeNode *header;
};

// Initialize quadtree by splitting nodes till each leaf node intersects at most one asteroid.
void Quadtree::initialize(float x, float y, float s)
{
    header = new QuadtreeNode(x, y, s);
    header->build();
}

void Quadtree::clearTree()
{
    header->clear(); //clear the entire quadtree
}

void Quadtree::checkCollisions() {
    header->ballCollisions(); //check for collisions in each quadtree node
}

void Quadtree::drawBalls() {
    header->drawBalls(); //draw the balls in each quadtree node
}

Quadtree ballsQuadtree; // Global quadtree.




// Struct of bitmap file.
struct BitMapFile
{
    int sizeX;
    int sizeY;
    unsigned char *data;
};

// Routine to read a bitmap file.
// Works only for uncompressed bmp files of 24-bit color.
BitMapFile *getBMPData(string filename)
{
    BitMapFile *bmp = new BitMapFile;
    unsigned int size, offset, headerSize;
    
    // Read input file name.
    ifstream infile(filename.c_str(), ios::binary);

    // Get the starting point of the image data.
    infile.seekg(10);
    infile.read((char *) &offset, 4);
    
    // Get the header size of the bitmap.
    infile.read((char *) &headerSize,4);
    
    // Get width and height values in the bitmap header.
    infile.seekg(18);
    infile.read( (char *) &bmp->sizeX, 4);
    infile.read( (char *) &bmp->sizeY, 4);
    
    // Allocate buffer for the image.
    size = bmp->sizeX * bmp->sizeY * 24;
    
    bmp->data = new unsigned char[size];
    // Read bitmap data.
    infile.seekg(offset);
    infile.read((char *) bmp->data , size);
    
    // Reverse color from bgr to rgb.
    int temp;
    for (int i = 0; i < sizeof(bmp->data); i += 3)
    {
        temp = bmp->data[i];
        bmp->data[i] = bmp->data[i+2];
        bmp->data[i+2] = temp;
    }
    
    return bmp;
}

// Load external textures.
void loadExternalTextures()
{
    // Local storage for bmp image data.
    BitMapFile *image[3];
    
    // Load the texture.
    image[0] = getBMPData("/Users/joecanero/Documents/Computer Graphics/Book Code/Textures/nightSky.bmp");
    image[1] = getBMPData("/Users/joecanero/Documents/Computer Graphics/Book Code/Textures/grass.bmp");
    image[2] = getBMPData("/Users/joecanero/Documents/Computer Graphics/Book Code/Textures/sky.bmp");
    
    //image[0] = getBMPData("/Users/joecanero/Desktop/Untitled.bmp");
    // Bind striped image to texture index[0].
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);
    
    // Bind grass image to texture index[1]. This is going to be the felt
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);
    
    // Bind cue stick texture to texture index[2]. This will make the cue stick look like a cue stick
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);
}


// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(0.0, 0.0, 0.0);
    
    drawPoolTable();
    drawPockets();
    drawBalls();
    
    //only draw the cue stick when we are ready to take a shot
    if(!animating && !scratch) drawStick();
    
    glutSwapBuffers();
}

//This function will draw the cue stick by taking the dot product of the angle that the user's mouse is making with the ball
//and rotating a tapered cylinder by the proper amount
void drawStick(void) {
    
    //material lighting properties for the cue stick
    float matAmbAndDif[4] = { 0.8, 0.6, 0.0, 1.0 };
    float matSpec[4] { 1.0, 1.0, 1.0, 1.0 };
    float matShine[1] = { 20.0 };
    
    //initialization of variables for calculating the cue stick's position
    double result = 0;
    double x1 = 0;
    double y1 = 1;
    double z1 = 0;
    double x2 = cueStickEndX - cueStickBeginX; //x2 is the x component of the vector between the cue stick's end and beginning points
    double y2 = cueStickEndY - cueStickBeginY; //y2 is the y component of the vector between the cue stick's end and beginning points
    double dot = x1*x2 + y1 *y2;               //take the dot product of a vertical vector and the cue stick's vector to get the angle between them
    dot /= cueStickLength;                     //divide by the magnitude of the cue stick, the magnitude of the vertical is 1, so not needed here
    double theta = acos(dot);                  //to get the angle between the two vectors, take the arccos of the dot product
    if(x2 > 0) theta *= -1*180.0 / PI;         //convert the angle into degrees, and if x2 > 0, which means the cue stick is pointing to the right of
                                               //the screen, multiply by a negative to get the proper rotation, else
    else theta *= 180.0 / PI;                  //just convert to degrees
    
    
    //set lighting properties for the cue stick
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
    
    //isolate its transformation matrix, and push the cue stick into the frustum and set its beginning point to point = (cueStickBeginX, cueStickBeginY)
    glPushMatrix();
    glTranslatef(cueStickBeginX, cueStickBeginY, pushIntoFrustumAmount+1);
    glRotatef(theta + (-shotAngle), 0.0, 0.0, 1.0); //rotate the cue stick by the amount calculated above, take into account the user changing the angle
    glRotatef(-90.0, 1.0, 0.0, 0.0);                //make the cue stick point vertically up first, before rotating it according to the calculation
    glBindTexture(GL_TEXTURE_2D, texture[2]);       //map the third texture to the cue stick
    glEnable(GL_TEXTURE_GEN_S);                     //enable mapping on a gluCylinder object
    glEnable(GL_TEXTURE_GEN_T);
    gluCylinder(stick, 0.25, 0.10, cueStickLength, 12, 15);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glPopMatrix();                                  //disable mapping and pop the transformation matrix
    
}

//this function will draw the balls
void drawBalls(void) {
    
    // Colors of the balls of the pool table. The balls are all red for now.
    /*****************************************/
    
    //the material ambience and diffuse will be specified differently for each ball depending upon a switch statement
    //the specularity and shininess is the same for each ball
    
    float matAmbAndDif[4];
    float matSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float matShine[] = { 50.0 };
    
    /*****************************************/

    //draw all of the balls
    glLineWidth(1.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    for (vector<PoolBall>::iterator iter1 = PoolBalls.begin(); iter1 != PoolBalls.end(); iter1++) {
        if(!(iter1->isMarked())) {
            switch (iter1->getID()) { //specify different lighting properties for each ball
                case 1:
                    matAmbAndDif[0] = 1.0;
                    matAmbAndDif[1] = 1.0;
                    matAmbAndDif[2] = 1.0;
                    matAmbAndDif[3] = 1.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                    
                case 2:
                    matAmbAndDif[0] = 1.0;
                    matAmbAndDif[1] = 1.0;
                    matAmbAndDif[2] = 0.0;
                    matAmbAndDif[3] = 1.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 3:
                    matAmbAndDif[0] = 1.0;
                    matAmbAndDif[1] = 0.5;
                    matAmbAndDif[2] = 0.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 4:
                    matAmbAndDif[0] = 0.7;
                    matAmbAndDif[1] = 0.0;
                    matAmbAndDif[2] = 0.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 5:
                    matAmbAndDif[0] = 0.0;
                    matAmbAndDif[1] = 0.0;
                    matAmbAndDif[2] = 0.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 6:
                    matAmbAndDif[0] = 1.0;
                    matAmbAndDif[1] = 0.0;
                    matAmbAndDif[2] = 0.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 7:
                    matAmbAndDif[0] = 0.1;
                    matAmbAndDif[1] = 0.3;
                    matAmbAndDif[2] = 0.8;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 8:
                    matAmbAndDif[0] = 0.0;
                    matAmbAndDif[1] = 1.0;
                    matAmbAndDif[2] = 0.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 9:
                    matAmbAndDif[0] = 0.5;
                    matAmbAndDif[1] = 0.0;
                    matAmbAndDif[2] = 1.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 10:
                    matAmbAndDif[0] = 0.5;
                    matAmbAndDif[1] = 0.5;
                    matAmbAndDif[2] = 1.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    break;
                case 11: //this ball is the striped ball, in addition to its lighting properties, we must specify it to have a striped texture
                    matAmbAndDif[0] = 1.0;
                    matAmbAndDif[1] = 1.0;
                    matAmbAndDif[2] = 1.0;
                    matAmbAndDif[3] = 0.0;
                    
                    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDif);
                    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
                    glMaterialfv(GL_FRONT, GL_SHININESS, matShine);
                    
                    //bind the first texture, the yellow stripe to this ball
                    glBindTexture(GL_TEXTURE_2D, texture[0]);
                    glEnable(GL_TEXTURE_GEN_S);
                    glEnable(GL_TEXTURE_GEN_T);
                    break;
                    
                    
            }
            iter1->updatePosition();    //update each ball's position
            iter1->drawBall();          //draw each ball
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
        }
    }
    glDisable(GL_CULL_FACE);            //disable face culling
}

//this function will draw the pockets
void drawPockets(void) {
    int pocketRad = 1;
    int pocketSlices = 12;
    int pocketStacks = 12;
    // Manage the colors of the pockets of the pool table. The pockets are black with no shininess.
    /*****************************************/
    
    //pockets are black, so they have very little lighting properties
    
    float matAmbAndDifPocket[] = { 0.0, 0.0, 0.0, 1.0 };
    float matSpecPocket[] = { 0.0, 0.0, 0.0, 0.0 };
    float matShinePocket[] = { 0.0 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDifPocket);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDifPocket);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecPocket);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShinePocket);
    
    /*****************************************/
    
    //top right pocket
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount);
    glTranslatef(rectSize, rectSize, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(pocketRad, pocketSlices, pocketStacks);
    glPopMatrix();
    
    //top left pocket
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount);
    glTranslatef(-rectSize, rectSize, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(pocketRad, pocketSlices, pocketStacks);
    glPopMatrix();
    
    //bottom right pocket
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount);
    glTranslatef(rectSize, -rectSize, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(pocketRad, pocketSlices, pocketStacks);
    glPopMatrix();
    
    //bottom left pocket
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount);
    glTranslatef(-rectSize, -rectSize, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(pocketRad, pocketSlices, pocketStacks);
    glPopMatrix();
    
    //top middle pocket
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount);
    glTranslatef(0.0, rectSize, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(pocketRad, pocketSlices, pocketStacks);
    glPopMatrix();
    
    //bottom middle pocket
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount);
    glTranslatef(0.0, -rectSize, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(pocketRad, pocketSlices, pocketStacks);
    glPopMatrix();
    
}
//this function draws the pool table
void drawPoolTable(void) {
    
    // Manage the color of the pool table itself. The table is a green with no shininess.
    /*****************************************/
    
    float matAmbAndDifTable[] = { 0.2, 1.0, 0.2, 1.0 };
    float matSpecTable[] = { 1.0, 1.0, 0.0, 1.0 };
    float matShineTable[] = { 30.0 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDifTable);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDifTable);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecTable);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShineTable);
    
    /*****************************************/
    
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
    
    glColor3f(0.20, 0.60, 0.0); //color of the pool table
    
    // the felt texture of a pool table is achieved by mapping grass to the pool table
    // Map the grass texture onto the pool table
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glBegin(GL_POLYGON);
    glNormal3f(0.0, 1.0, 0.0);
    glTexCoord2f(0.0, 0.0); glVertex3f(-rectSize, -rectSize, 0);
    glTexCoord2f(1.0, 0.0); glVertex3f(rectSize, -rectSize, 0);
    glTexCoord2f(1.0, 1.0); glVertex3f(rectSize, rectSize, 0);
    glTexCoord2f(0.0, 1.0); glVertex3f(-rectSize, rectSize, 0);
    glEnd();
    
    glPopMatrix();
    
    glColor3f(0.80, 0.60, 0.0); //color of the walls of the table
    
    // Manage the color of the walls of the pool table. The walls are a brown with no shininess.
    /*****************************************/
    
    float matAmbAndDifWalls[] = { 0.8, 0.6, 0.0, 1.0 };
    float matSpecWalls[] = { 0.0, 0.0, 0.0, 1.0 };
    float matShineWalls[] = { 0.0 };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbAndDifWalls);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matAmbAndDifWalls);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecWalls);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShineWalls);
    
    /*****************************************/

    float wallY = 13.25; //variables for use in making the walls
    float wallX = 0.3;
    float translationF = 13.25; //translation factor
    
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
    
    glTranslatef(-translationF, 0.0, 0.0); //translate the left wall to the left
    glRotatef(90.0, 0.0, 1.0, 0.0); //rotation of the left side of the pool table wall
    glRectf(-wallX, -wallY, wallX, wallY);
    
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
    
    glTranslatef(translationF, 0.0, 0.0); //translate the right wall to the right
    glRotatef(-90.0, 0.0, 1.0, 0.0); //rotation of the right side of the pool table wall
    glRectf(-wallX, -wallY, wallX, wallY);
    
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
    
    glTranslatef(0.0, translationF, 0.0); //translate the top wall to the top
    glRotatef(-90.0, 1.0, 0.0, 0.0); //rotation of the top side of the pool table wall
    glRectf(-wallY, -wallX, wallY, wallX);
    
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
    
    glTranslatef(0.0, -translationF, 0.0); //translate the bottom wall to the bottom
    glRotatef(90.0, 1.0, 0.0, 0.0); //rotation of the bottom side of the pool table wall
    glRectf(-wallY, -wallX, wallY, wallX);
    
    glPopMatrix();
    
    //additional parameters for constructing the walls of the pool table
    //these extra walls make the pockets flush with the sides of the pool table
    float boundX = 0.5;
    float boundY = 16.15;
    float boundX2 = 0.72;
    float boundY2 = 15.70;
    
    //draw additional boundaries on the outside of the pool table so the pockets look better
    //right boundary
    glPushMatrix();
    glTranslatef(15.50, 0.0, pushIntoFrustumAmount - 2); //translate into the frustum
    glRectf(-boundX, -boundY, boundX, boundY);
    glPopMatrix();
    
    //left boundary
    glPushMatrix();
    glTranslatef(-15.50, 0.0, pushIntoFrustumAmount - 2); //translate into the frustum
    glRectf(-boundX, -boundY, boundX, boundY);
    glPopMatrix();
    
    //top boundary
    glPushMatrix();
    glTranslatef(0.0, 15.5, pushIntoFrustumAmount - 2); //translate into the frustum
    glRotatef(90.0, 0.0, 0.0, 1.0);
    glRectf(-boundX2, -boundY2, boundX2, boundY2);
    glPopMatrix();
    
    //bottom boundary
    glPushMatrix();
    glTranslatef(0.0, -15.5, pushIntoFrustumAmount - 2); //translate into the frustum
    glRotatef(90.0, 0.0, 0.0, 1.0);
    glRectf(-boundX2, -boundY2, boundX2, boundY2);
    glPopMatrix();
}

// Initialization routine.
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
    
    /*****************************************/
    
    //specify the properties of Light 0
    //Light 0 is a positional white light located at the origin
    
    glEnable(GL_LIGHTING);
    
    float lightAmb[] = { 0.0, 0.0, 0.0, 0.0 };
    float DifAndSpec[] = { 1.0, 1.0, 1.0, 1.0 };
    float lightPos[] = { 0.0, 0.0, 0.0, 1.0 };
    float globAmb[] = { 0.5, 0.5, 0.5, 1 };
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, DifAndSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, DifAndSpec);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);
    
    glEnable(GL_LIGHT0);
    
    /*****************************************/
    
    //put all of the balls to be used into the PoolBalls vector
    //the balls are put in a 9-ball arrangement
    
    PoolBalls.push_back(PoolBall(cueStartX, cueStartY, 0.0, 1));
    PoolBalls.push_back(PoolBall(4.0, 0.0, 0.0, 2));
    PoolBalls.push_back(PoolBall(6.0, 1.0, 0.0, 3));
    PoolBalls.push_back(PoolBall(6.0, -1.0, 0.0, 4));
    PoolBalls.push_back(PoolBall(8.0, 2.0, 0.0, 5));
    PoolBalls.push_back(PoolBall(8.0, -2.0, 0.0, 6));
    PoolBalls.push_back(PoolBall(10.0, 1.0, 0.0, 7));
    PoolBalls.push_back(PoolBall(10.0, -1.0, 0.0, 8));
    PoolBalls.push_back(PoolBall(12.0, 0.0, 0.0, 9));
    PoolBalls.push_back(PoolBall(8.0, 0.0, 0.0, 11));
    
    //initialize the cue stick glu object
    stick = gluNewQuadric();
    gluQuadricDrawStyle(stick, GLU_FILL);
    gluQuadricNormals(stick, GLU_SMOOTH);
    
    glEnable(GL_DEPTH_TEST); // Enable depth testing.
    
    // Create texture index array and load external textures.
    glGenTextures(3, texture);
    loadExternalTextures();
    
    //enable texturing
    glEnable(GL_TEXTURE_2D);
    
    // Specify how texture values combine with current surface color values.
    // Since there is a light source, we use Gl_Modulate to get the proper combination of texture and lighting
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Specify automatic texture generation for a sphere map.
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    
    //initialize the quadtree for the first time
    ballsQuadtree.initialize(-rectSize, -rectSize, 2*rectSize);
    cout << "Interaction: Click anywhere on the table to \"hit\" the cue ball in the direction of the vector pointing from the mouse to the center of the ball. While the ball is moving, clicking again will stop the animation. You are able to change the angle that the cue stick makes with the cue ball, but it is not reflected visually. Press \"A\" to angle the cue to the left, or press \"S\" to angle the cue to the right. If you get a ball in the pocket, it is removed from play. Scratches are not implemented yet." << endl;
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-frustumWidth, frustumWidth, -frustumHeight, frustumHeight, 5.0, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine. Will handle the user input to increase angle of the sot
void keyInput(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 27:
            exit(0);
            break;
        case 'a':
            shotAngle -= PI/20; //increase angle of shot to the left
            break;
        case 's':
            shotAngle += PI/20; //increase angle of the shot to the right
            break;
        default:
            break;
    }
    if(shotAngle > PI/2.0) { shotAngle -= PI/20; cueAngle -= (asin(0.75/cueStickLength)/20); } //if angle exceeds PI/2, subtract what was just added to make it PI/2 again
    else if(shotAngle < -PI/2.0) { shotAngle += PI/20; cueAngle += (asin(0.75/cueStickLength)/20); } //if angle exceeds -PI/2, add what was just substracted to make it -PI/2 again
    if (key == 'a' || key == 's') {
        glutPostRedisplay();
    }
}

//Passive Motion Function
void passiveMotion(int x, int y) {
    
    vector<PoolBall>::iterator iter = PoolBalls.begin();
    
    yC = (height - y);
    yC = (yC - (height/2.0)) / heightRatio;
    yC *= ratioOfFarPlaneToNear; //ratio of how much Y values change between the planes
    xC = (x - (width/2.0)) / widthRatio;
    xC *= ratioOfFarPlaneToNear; //ratio of how much X values change between the planes
    
    if (!animating && !scratch) {
       
        // the end of the cue stick will be placed at the user's cursor
        cueStickBeginX = xC;
        cueStickBeginY = yC;
        
        // the end of the cue stick is placed at the center of the cue ball
        cueStickEndX = iter->getXLocation();
        cueStickEndX = cueStickEndX*cos(cueAngle) - cueStickEndY*sin(cueAngle);
        cueStickEndY = iter->getYLocation();
        cueStickEndY = cueStickEndY*sin(cueAngle) + cueStickEndY*cos(cueAngle);
        
        // get the length of the cue stick from the magnitude of the vector connecting the end of the stick to the beginning
        cueStickLength = sqrt( pow(cueStickEndX - cueStickBeginX, 2) + pow(cueStickEndY - cueStickBeginY,2) );
        glutPostRedisplay();
    }
    else if (!animating && scratch) { //passive motion tracking of the cue ball when the user scratches. this will follow the cursor and show where the
        iter->setXLocation(xC);       //ball can be placed
        iter->setYLocation(yC);
        glutPostRedisplay();
    }
}

//Mouse callback function
void mouseCallback(int button, int state, int x, int y) {
    
    /*  this function will trigger the animation
        if the scene is currently not being animated and the left mouse key is pressed,
        the scene will begin animation.
     
        if animation is happening and the left mouse key is pressed, the animation will
        stop
     */

    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !animating) {
        animating = true;
        ballsQuadtree.initialize(-rectSize, -rectSize, 2*rectSize);
    }
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && animating) {
        t = 0;
        animating = false;
    }
    
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP && animating) {
        calculateVelocity((float)x, (float)(height - y)); // pass the coordinates of the mouse to the function so it can calculate the velocity of the cue ball
        animate(0); //animate with a value of 0, indicating it is to animate after user strikes the ball
    }
    
    //whenever the user clicks again after scratching, the cue ball will be placed at that location
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP && scratch) {
        vector<PoolBall>::iterator iter = PoolBalls.begin();
        iter->setXLocation(xC);
        iter->setYLocation(yC);
        scratch = false;
    }
    
}

// Function to increase the angle of the ball's rotation
void increaseAngle() {
    
    //this function increases the angle of the ball's rotation
    //and if it exceeds 360, it wraps it around
    for (vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
        if(!(iter->isMarked())) {
            iter->setAngle(iter->getAngle() + t * (sqrt(pow(iter->getXComponent(),2) + pow(iter->getYComponent(), 2))));
            if(iter->getAngle() > 360.0) iter->setAngle(iter->getAngle() - 360.0);
        }
    }
}

// Function to animate the scene if the animating bool says to
void animate(int someValue) {
    
    //  reinitialize the quadtree at the beginning of each frame
    ballsQuadtree.initialize(-rectSize, -rectSize, 2*rectSize);
    
    //  if the velocity of the object is <= 0, stop animation because the ball has stopped
    //  need to use a small float to test if the magnitude is sufficiently close to 0.
    //  we want animation to stop only when all of the balls have effectively ceased moving
    //  this will loop through all of the balls to see if their animation is practically 0. it performs a compound OR operation
    //  that will result in animation continuing if at least 1 ball is still registered as moving
    
    /*  the variable testing will store the results of the compound OR. if animation is being initiated by a mouse click, the check will never happen: it will always go through with the animation. But if the animate method is being called through the glutTimerFunc, the compound OR check will happen. It will then AND testing with animating. If testing is a 1, that means at least one ball is moving and so animating will continue. If testing is a zero, then animating && testing will return a 0 and the animation will stop
     */
    
    if(someValue) {
        bool testing = false;
        for (vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
            if(!(iter->isMarked())) testing |= ((sqrt(pow(iter->getXComponent(),2) + pow(iter->getYComponent(), 2)) - testFloat) > 0);
        }
        animating &= testing;
    }
    else animating = true;
    if(animating) { //if the scene should currently be animated, the angle is incremented
                    //and the timer function is called again
        increaseAngle();
        moveBall(t);
        animating = 1;
        glutTimerFunc(25, animate, 1);
    }
    else {
        t = 0; //  reset the time since animation is stopping
               //  set all ball components to 0
        for (vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
            if(!(iter->isMarked())) iter->setXComponent(0.0); iter->setYComponent(0.0);
        }
    }
}

//Function to check for collisions between all the balls and the walls, in addition to any corner or side pockets
void checkCollisions() {
    
    //when there is a collision along the side walls, the x component must be negated
    //when there is a collision along the top or bottom walls, the y component must be negated
    ballsQuadtree.checkCollisions();
    
    for(vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
        if (!(iter->isMarked())) {
            //this block will check if the ball is colliding with a wall, if not it will check if it is colliding with a pocket
            //check if there is a collision with any of the walls
            //right wall
            if(iter->getXLocation() + iter->getRadius() + iter->getXComponent() > rectSize &&
               (iter->getYLocation() <= pocketConst && iter->getYLocation() >= -pocketConst)) {
                iter->setXComponent(iter->getXComponent() * -1);
            }
            //else if colliding in top or bottom right pockets
            else if(iter->getXLocation() + iter->getRadius() + iter->getXComponent() > rectSize &&
                    (iter->getYLocation() > pocketConst || iter->getYLocation() < -pocketConst)) {
                //PoolBalls.erase(iter);
                iter->markBall(true);
            }
            
            //left wall
            if(iter->getXLocation() - iter->getRadius() + iter->getXComponent() < -rectSize &&
               (iter->getYLocation() <= pocketConst && iter->getYLocation() >= -pocketConst)) {
                iter->setXComponent(-1 * iter->getXComponent());
            }
            //else if colliding in top or bottom left pockets
            else if(iter->getXLocation() - iter->getRadius() - iter->getXComponent() < -rectSize &&
                    (iter->getYLocation() > pocketConst || iter->getYLocation() < -pocketConst)) {
                //PoolBalls.erase(iter);
                iter->markBall(true);
            }
            
            //top wall
            if(iter->getYLocation() + iter->getRadius() + iter->getYComponent() > rectSize &&
               ((iter-> getXLocation() <= pocketConst && iter->getXLocation() >= .6125) || (iter-> getXLocation() <= -.6125 && iter->getXLocation() >= -pocketConst) )) {
                iter->setYComponent(-1 * iter->getYComponent());
            }
            //else if colliding in top right or left or middle pockets
            else if (iter->getYLocation() + iter->getRadius() + iter->getYComponent() > rectSize &&
                     (iter-> getXLocation() > pocketConst || iter->getXLocation() < -pocketConst || ( iter->getXLocation() < 0.6125 && iter->getXLocation() > -0.6125) )) {
                //PoolBalls.erase(iter);
                iter->markBall(true);
            }
            
            //bottom wall
            if(iter->getYLocation() - iter->getRadius() + iter->getYComponent() < -rectSize &&
               ((iter-> getXLocation() <= pocketConst && iter->getXLocation() >= .6125) || (iter-> getXLocation() <= -.6125 && iter->getXLocation() >= -pocketConst) )) {
                iter->setYComponent(-1 * iter->getYComponent());
            }
            //else if colliding in bottom right or left or middle  pockets
            else if (iter->getYLocation() - iter->getRadius() - iter->getYComponent() < -rectSize &&
                     (iter-> getXLocation() > pocketConst || iter->getXLocation() < -pocketConst || ( iter->getXLocation() < 0.6125 && iter->getXLocation() > -0.6125) )) {
                //PoolBalls.erase(iter);
                iter->markBall(true);
            }
        }
    }
    
    //clear the quadtree
    ballsQuadtree.clearTree();
}

// Function to delete the balls that have been marked after they are sunk into pockets
void deleteBalls() {
    float clear = 0.0; float zClear = 50.0;
    
    //each ball is moved off of the pool table to location (0, 0, 50) so as not to interfere with any collision testing
    //deleting the balls was causing memory access issues so this was a quick solution to that problem
    for (vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
        if (iter->isMarked() && iter->getID() != 1) {
            iter->setZLocation(zClear);
            iter->setXLocation(clear);
            iter->setYLocation(clear);
            iter->setXComponent(clear);
            iter->setYComponent(clear);
            iter->setZComponent(clear);
        }
        else if(iter->isMarked() && iter->getID() == 1) { //if the ball that is marked is the cue ball, put it back in the middle and allow it to be moved
            iter->setXLocation(clear);                    //so the user can place it after a scratch
            iter->setYLocation(clear);
            iter->setXComponent(clear);
            iter->setYComponent(clear);
            iter->setZComponent(clear);
            iter->markBall(false);
            scratch = true;                               //since the cue ball went into a pocket, the user scratched, allow the cue ball to be placed
        }
    }
}

// This function will return the ball's translation components for a given point in time
// in terms of where its center will move.
// It will also recalculate the velocity vector of the ball's motion for each step in time
void moveBall(int& t) {
    
    //time (t) is initially 0 and it is unaffected by drag
    
    checkCollisions();
    deleteBalls();
    frictionAmount = -frictionCo*t; //amount of friction on the table at the current time
    
    //this part will incorporate friction
    //change each ball's velocity by the amount of friction at that time
    for (vector<PoolBall>::iterator iter = PoolBalls.begin(); iter!=PoolBalls.end(); iter++) {
        if(!(iter->isMarked())) {
            iter->setXComponent(iter->getXComponent() + iter->getXComponent()*frictionAmount);
            iter->setYComponent(iter->getYComponent() + iter->getYComponent()*frictionAmount);
        }
    }
    
    t+= 1; //increment time
    glutPostRedisplay();
}

// Function to initially calculate the velocity of the ball's rolling when it is hit
// applies only to cue ball
void calculateVelocity(float x, float y) {
    
    // scale the coordinates of the screen into the coordinates of the near screen
    /*
     since the width of the screen is 750, half of that is 375.
     and the width of the near plane of the frustum is 10, it goes from -5 to 5.
     this length of 10 is what is projected onto the screen width of 750 so each unit
     of the frustum has a length of 75 in terms of the screen.
     
     and since the "far" end of our frustum is located at -15, the ratio between the far
     and near planes is 3, so the coordinates obtained on the near plane must be multiplied
     by three to properly end up on the far plane
     
     the same logic follows for the height of the screen, but since the viewing window
     is not square, a different constant needs to be the divisor.
     
     */
    
    x = (x-(width/2.0)) / widthRatio;
    x *= ratioOfFarPlaneToNear; //ratio of how much X values change between the planes
    y = (y-(height/2.0)) / heightRatio;
    y *= ratioOfFarPlaneToNear; //ratio of how much Y values change between the planes
    
    
    //change the hit vector of the cue ball if the user decides to change the angle of the shot themselves
    //a simple rotation matrix is used to rotate the original hit vector in accordance with the new angle
    vector<PoolBall>::iterator iter = PoolBalls.begin();
    a = ((x - iter->getXLocation()) / 10);
    b = ((y - iter->getYLocation()) / 10);
    a = a*cos(shotAngle) - b*sin(shotAngle);
    b = a*sin(shotAngle) + b*cos(shotAngle);
    a = -a;
    b = -b;
    
    iter->setXComponent(a); iter->setYComponent(b);
    shotAngle = 0.0;
}

// Main routine.
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("PoolTable.cpp");
    setup();
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutMouseFunc(mouseCallback);
    glutPassiveMotionFunc(passiveMotion);
    glutTimerFunc(50, animate, 1);
    glutMainLoop();
    
    return 0;
}