/////////////////////////////////
// Joe Canero, CSC350, Assignment 5
//
// This program draws a cue ball on a pool table.
// Clicking on the pool table will move the ball in a vector in the direction from the mouse
// to the ball. The ball will slow down according to some coefficient of friction on the
// table.
//
// The program also as collision checking that can test if the ball has reached the end of the
// pool table and must have a new velocity
/////////////////////////////////

#include <iostream>
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
//
//using namespace std;
//
//void drawPoolTable(); //declaration of function to draw the pool table
//void increaseAngle(); //declaration of function to increase of the ball's rotation
//void animate(int); //declaration of the function that calls the proper movement functions if the ball should be moving
//void moveBall(int&); //declaration of function that finds the ball's new coordinates
//void calculateRotationAxis(float, float); //declaration of function that calculates the axis of rotation of the ball after it is hit
//void calculateVelocity(float, float); //declaration of function that computes the initial velocity of the ball after it is hit
//void checkCollisions(); //declaration of function that checks if there is a collision happening at each unit of time
//
////Globals
//float ballCenterX = 0.0; //coordinate of the ball's center. initially 0.
//float ballCenterY = 0.0; //coordinate of the ball's center. initially 0.
//float velocityX = 0.0; //x component of the velocity vector of the ball
//float velocityY = 0.0; //y component of the velocity vector of the ball
//float velocityMag = 0.0; //magnitude of the velocity vector of the ball
//double angle = 3.0; //angle of the ball's rotation
//float cueRadius = 0.75; //radius of the cue ball
//bool animating = false; //condition determining if there is animation or not
//float rotationX = 0.0; //x component of rotation axis
//float rotationY = 0.0; //y component of rotation axis
//int width = 750; //width of viewing window
//int height = 500; //height of viewing window
//int frustumWidth = 5; //width of half of frustum
//int frustumHeight = 5; //height of half of frustum
//int widthRatio = width/(frustumWidth * 2); //ratio of pixels of viewing screen to units of frustum width
//int heightRatio = height/(frustumHeight * 2); //ratio of pixels of viewing screen to units of frustum height
//float pushIntoFrustumAmount = -15.0; //amount all objects are translated back into frustum
//float ratioOfFarPlaneToNear = -pushIntoFrustumAmount/5.0; //ratio of plane of translation to near plane
//float rectSize = 13.0; //size of pool table
//float frictionCo = 0.0005; //coefficient of friction on the pool table
//float testFloat = 0.0085; //this float is used to test if the magnitude is sufficiently close to zero indicating the ball is almost coming to rest. making it smaller will make the ball move for longer, making it larger will stop motion prematurely.
//int t = 0; //parameter of the time of motion
//
//std::vector<PoolBall> PoolBalls; // a global vector of PoolBall objects
//const int NUMBALLS = 2;
//
//// Return 1 if the axes-parallel rectangle with diagonally opposite corners at (x1,y1) and (x2,y2)
//// intersects the disc centered (x3,y3) of radius r, otherwise return 0.
//int checkDiscRectangleIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float r)
//{
//    float minX, maxX, minY, maxY;
//    
//    // Set minX to smaller of x1 and x2, and maxX to the larger; likewise minY and maxY.
//    if (x1 <= x2)
//    {
//        minX = x1; maxX = x2;
//    }
//    else
//    {
//        minX = x2; maxX = x1;
//    }
//    if (y1 <= y2)
//    {
//        minY = y1; maxY = y2;
//    }
//    else
//    {
//        minY = y2; maxY = y1;
//    }
//    
//    // The disc intersects the rectangle if its center lies in the strip with corners at
//    // (minX-r, minY) and (maxX+r, maxY), or if its center lies in the strip with corners
//    // at (minX, minY-r) and (maxX, maxY+r), or if its center is within distance r of one
//    // the four corners of the rectangles.
//    if      ( (x3 >= minX - r) && (x3 <= maxX + r) && (y3 >= minY) && (y3 <= maxY) ) return 1;
//    else if ( (x3 >= minX) && (x3 <= maxX) && (y3 >= minY - r) && (y3 <= maxY + r) ) return 1;
//    else if ( (x3 - x1)*(x3-x1) + (y3 - y1)*(y3 - y1) <= r*r ) return 1;
//    else if ( (x3 - x1)*(x3-x1) + (y3 - y2)*(y3 - y2) <= r*r ) return 1;
//    else if ( (x3 - x2)*(x3-x2) + (y3 - y2)*(y3 - y2) <= r*r ) return 1;
//    else if ( (x3 - x2)*(x3-x2) + (y3 - y1)*(y3 - y1) <= r*r ) return 1;
//    else return 0;
//}
//
//
//// Quadtree node class.
//class QuadtreeNode
//{
//public:
//    QuadtreeNode(float x, float y, float s);
//    int numberBallsIntersected(); // Return the number of balls intersecting the square.
//    void addIntersectingBallsToList(); // Add the intersected balls to the local vector of balls.
//    void clear();   //recursively clears the nodes
//    void build(); // Recursive routine to split a square that intersects more than 10 balls;
//    void ballCollisions();
//    // if it intersects at most 10 balls leave it as a leaf and add the intersecting
//    // balls, if any, to a local list of balls.
//    
//private:
//    float SWCornerX, SWCornerY; // x and y co-ordinates of the SW corner of the square.
//    float size; // Side length of square.
//    QuadtreeNode *SWChild, *NWChild, *NEChild, *SEChild; // Children nodes.
//    std::vector<PoolBall> ballList; // Local list of pool balls intersecting the square - only filled for leaf nodes.
//    
//    friend class Quadtree;
//};
//
//
//// QuadtreeNode constructor.
//QuadtreeNode::QuadtreeNode(float x, float y, float s)
//{
//    SWCornerX = x; SWCornerY = y; size = s;
//    SWChild = NWChild = NEChild = SEChild = NULL;
//    ballList.clear();
//}
//
//// Return the number of balls intersecting the square.
//int QuadtreeNode::numberBallsIntersected()
//{
//    int numVal = 0;
//    for (std::vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
//        if (iter->getRadius() > 0.0) {
//            if ( checkDiscRectangleIntersection( SWCornerX, SWCornerY, SWCornerX+size, SWCornerY-size,
//                                                iter->getXLocation(), iter->getYLocation(),
//                                                iter->getRadius() )
//                )
//                numVal++;
//        }
//    }
//    return numVal;
//}
//
//// Add the balls intersecting the square to a local list of pool balls.
//void QuadtreeNode::addIntersectingBallsToList()
//{
//    for (std::vector<PoolBall>::iterator iter = PoolBalls.begin(); iter != PoolBalls.end(); iter++) {
//        if (iter->getRadius() > 0.0) {
//            if ( checkDiscRectangleIntersection( SWCornerX, SWCornerY, SWCornerX+size, SWCornerY-size,
//                                                iter->getXLocation(), iter->getYLocation(),
//                                                iter->getRadius() )
//                )
//                cout << "here!" << endl;
//                ballList.push_back( PoolBall(*iter) );
//        }
//    }
//}
//
//// Recursive routine to split a square that intersects more than 10 pool balls; if it intersects
//// at most ten balls leave it as a leaf and add the intersecting ball, if any, to a local
//// list of balls.
//void QuadtreeNode::build()
//{
//    if ( this->numberBallsIntersected() <= 10 ) this->addIntersectingBallsToList();
//    else
//    {
//        SWChild = new QuadtreeNode(SWCornerX, SWCornerY, size/2.0);
//        NWChild = new QuadtreeNode(SWCornerX, SWCornerY - size/2.0, size/2.0);
//        NEChild = new QuadtreeNode(SWCornerX + size/2.0, SWCornerY - size/2.0, size/2.0);
//        SEChild = new QuadtreeNode(SWCornerX + size/2.0, SWCornerY, size/2.0);
//        
//        SWChild->build(); NWChild->build(); NEChild->build(); SEChild->build();
//    }
//}
//
//void QuadtreeNode::ballCollisions() {
//    if (SWChild == NULL) {
//        //this square is a leaf, so we do not have to check the children, just the balls in this square
//        // Create a second vector of PoolBalls
//        // this vector will be used to ensure that there are no duplicate collision checks
//        
//        /*
//         The PoolBall function ComputeElasticCollision only computes an elastic collision if the two balls are unique; that is, if
//         the function call is ComptueElasticCollision(Ball1, Ball1), the function will not compute a collision between a ball and itself.
//         That is one way redundancy is prevented.
//         
//         The second way redundancy is prevented is through the use of the second vector. Since after one full iteration of the inner loop,
//         the ball at the location of the first iterator has been checked against every other ball, the first ball in the second vector is erased.
//         
//         Think of it this way:
//         
//         At the start of the loop, iter1 has b1, b2, b3,...,bN
//         iter2 has b1, b2, b3,...,bN
//         
//         After the inner for loop completes one whole iteration, b1 from iter1 has been checked against b1, b2, b3,...,bN. If we erase b1
//         from iter2, iter2 will now have b2, b3,...,bN. After the erase, iter1 increments and is now pointing to b2.
//         
//         Upon the second iteration of the inner loop, b2 will be checked against b2, b3,...,bN. This procedure effectively removes checking
//         b2 against b1, which is a check that has already been accomplished.
//         
//         */
//        std::vector<PoolBall> ballList2 = ballList;
//        for (std::vector<PoolBall>::iterator iter1 = ballList.begin(); iter1 != ballList.end(); iter1++) {
//            for (std::vector<PoolBall>::iterator iter2 = ballList2.begin(); iter2 != ballList2.end(); iter2++) {
//                if(iter1->PoolBall::isColliding(*iter2)) ComputeElasticCollision(*iter1, *iter2);
//            }
//            ballList2.erase(ballList2.begin());
//        }
//    }
//}
//
//void QuadtreeNode::clear() {
//    if(SWChild != NULL) SWChild->clear();
//    if(NWChild != NULL) NWChild->clear();
//    if(SEChild != NULL) NEChild->clear();
//    if(SEChild != NULL) SEChild->clear();
//    delete this;
//}
//
//
//// Quadtree class.
//class Quadtree
//{
//public:
//    Quadtree() { header = NULL; } // Constructor.
//    void initialize(float x, float y, float s); // Initialize quadtree by splitting nodes
//    // till each leaf node intersects at
//    // most one asteroid.
//    void clearTree();   //clears the tree
//    void checkCollisions(); //checks for collisions between the balls in the quadtree
//private:
//    QuadtreeNode *header;
//};
//
//// Initialize quadtree by splitting nodes till each leaf node intersects at most one asteroid.
//void Quadtree::initialize(float x, float y, float s)
//{
//    header = new QuadtreeNode(x, y, s);
//    header->build();
//}
//
//void Quadtree::clearTree()
//{
//    header->clear();
//}
//
//void Quadtree::checkCollisions() {
//    header->ballCollisions();
//}
//
//Quadtree ballsQuadtree; // Global quadtree.
//
//
//// Drawing routine.
//void drawScene(void)
//{
//    ballsQuadtree.initialize(rectSize, rectSize, 10);
//    glClear(GL_COLOR_BUFFER_BIT);
//    glLoadIdentity();
//    glColor3f(0.0, 0.0, 0.0);
//    
//    drawPoolTable();
//    //glPushMatrix();
//    //rotate the cue ball
//    //glTranslatef(ballCenterX, ballCenterY, 0.0); // this will create the motion of the ball
//    //glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate ball into the frustum
//    //glRotatef(angle, velocityY, -velocityX, 0.0);
//    //cout << "X: " << ballCenterX << " Y: " << ballCenterY << endl;
//    //make the cue ball
//    glColor3f(1.0, 1.0, 1.0); //white
//    vector<PoolBall>::iterator iter1 = PoolBalls.begin();
//    //cout << "LOCATION: " << iter1->getXLocation() << " " << iter1->getYLocation() << " " << iter1->getZLocation() << endl;
//   // iter1->setXComponent(ballCenterX);
//    iter1->setYComponent(ballCenterY);
//    cout << "THE X LOCATION: " << iter1->getXLocation() << endl;
//    iter1->setAngle(angle);
//    iter1->updatePosition();
//    //cout << "NEW LOCATION: " << iter1->getXLocation() << " " << iter1->getYLocation() << " " << iter1->getZLocation() << endl;
//    iter1->drawBall();
//    //glutWireSphere(cueRadius, 12.0, 10.0); //coordinates (X,Y)
//    //glPopMatrix();
//    iter1++;
//    //glPushMatrix();
//    //iter1->updatePosition();
//    //glTranslatef(iter1->getXLocation(), iter1->getYLocation(), iter1->getZLocation());
//    //glTranslatef(0.0, 0.0, pushIntoFrustumAmount);
//    iter1->drawBall();
//    //glPopMatrix();
//    
//    glutSwapBuffers();
//}
//
////this function draws the pool table
//void drawPoolTable(void) {
//    glPushMatrix();
//    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
//    
//    glColor3f(0.20, 0.60, 0.0); //color of the pool table
//    glRectf(-rectSize, -rectSize, rectSize, rectSize); //the pool table
//    
//    glPopMatrix();
//    
//    glColor3f(0.80, 0.60, 0.0); //color of the walls of the table
//    
//    glPushMatrix();
//    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
//    
//    glTranslatef(-13.25, 0.0, 0.0); //translate the left wall to the left
//    glRotatef(90.0, 0.0, 1.0, 0.0); //rotation of the left side of the pool table wall
//    glRectf(-0.3, -13.25, 0.3, 13.25);
//    
//    glPopMatrix();
//    glPushMatrix();
//    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
//    
//    glTranslatef(13.25, 0.0, 0.0); //translate the right wall to the right
//    glRotatef(-90.0, 0.0, 1.0, 0.0); //rotation of the right side of the pool table wall
//    glRectf(-0.3, -13.25, 0.3, 13.25);
//    
//    glPopMatrix();
//    glPushMatrix();
//    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
//    
//    glTranslatef(0.0, 13.25, 0.0); //translate the top wall to the top
//    glRotatef(-90.0, 1.0, 0.0, 0.0); //rotation of the top side of the pool table wall
//    glRectf(-13.25, -0.3, 13.25, 0.3);
//    
//    glPopMatrix();
//    glPushMatrix();
//    glTranslatef(0.0, 0.0, pushIntoFrustumAmount); //translate everything into the frustum
//    
//    glTranslatef(0.0, -13.25, 0.0); //translate the bottom wall to the bottom
//    glRotatef(90.0, 1.0, 0.0, 0.0); //rotation of the bottom side of the pool table wall
//    glRectf(-13.25, -0.3, 13.25, 0.3);
//    
//    glPopMatrix();
//}
//
//// Initialization routine.
//void setup(void)
//{
//    PoolBalls.push_back(PoolBall(0.0, 0.0, 0.0, 1));
//    PoolBalls.push_back(PoolBall(10.0, 0.0, 0.0, 2));
//    //ballsQuadtree.initialize(rectSize, rectSize, 10);
//    cout << "Interaction: Click anywhere on the table to \"hit\" the cue ball in the direction of the vector pointing from the mouse to the center of the ball. While the ball is moving, clicking again will stop the animation." << endl;
//    glClearColor(1.0, 1.0, 1.0, 0.0);
//}
//
//// OpenGL window reshape routine.
//void resize(int w, int h)
//{
//    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glFrustum(-frustumWidth, frustumWidth, -frustumHeight, frustumHeight, 5.0, 100.0);
//    
//    glMatrixMode(GL_MODELVIEW);
//}
//
//// Keyboard input processing routine.
//void keyInput(unsigned char key, int x, int y)
//{
//    switch(key)
//    {
//        case 27:
//            exit(0);
//            break;
//        default:
//            break;
//    }
//}
//
//void mouseCallback(int button, int state, int x, int y) {
//    //this function will trigger the animation
//    /*
//     if the scene is currently not being animated and the left mouse key is pressed,
//     the scene will begin animation.
//     
//     if animation is happening and the left mouse key is pressed, the animation will
//     stop
//     */
//    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !animating) {
//        animating = true;
//    }
//    else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && animating) {
//        t = 0;
//        animating = false;
//    }
//    
//    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP && animating) {
//        //calculateRotationAxis((float)x, (float)(height - y)); //pass the coordinates of the mouse to the function so it can calculate the rotation axis
//        calculateVelocity((float)x, (float)(height - y)); // pass the coordinates of the mouse to the function so it can calculate the velocity of the ball
//        animate(1);
//    }
//    
//}
//
//// Function to increase the angle of the ball's rotation
//void increaseAngle() {
//    //this function increases the angle of the ball's rotation
//    //and if it exceeds 360, it wraps it around
//    angle += t * velocityMag;
//    if(angle > 360.0) angle -= 360.0;
//}
//
//// Function to animate the scene if the animating bool says to
//void animate(int someValue) {
//    
//    //if the velocity of the object is <= 0, stop animation because the ball has stopped
//    //need to use a small float to test if the magnitude is sufficiently close to 0.
//    if(velocityMag - testFloat <= 0) {
//        animating = false;
//        t = 0;
//    }
//    
//    if(animating) { //if the scene should currently be animated, the angle is incremented
//        //and the timer function is called again
//        increaseAngle();
//        moveBall(t);
//        glutTimerFunc(25, animate, 1);
//    }
//}
//
//void checkCollisions() {
//    
//    //when there is a collision along the side walls, the x component must be negated
//    //when there is a collision along the top or bottom walls, the y component must be negated
//    /*
//    if(ballCenterX + cueRadius + velocityX > rectSize) { // colliding with right wall
//        velocityX *= -1; // reverse the X velocity
//    }
//    
//    if(ballCenterX - cueRadius + velocityX < -rectSize) { // colliding with left wall
//        velocityX *= -1; // reverse the X velocity
//    }
//    
//    if(ballCenterY + cueRadius > rectSize) { // colliding with top wall
//        velocityY *= -1; // reverse the Y velocity
//    }
//    
//    if(ballCenterY - cueRadius < -rectSize) { // colliding with bottom wall
//        velocityY *= -1; // reverse the Y velocity
//    }
//    */
//    vector<PoolBall>::iterator iter = PoolBalls.begin();
//    if(iter->getXLocation() + iter->getRadius() + iter->getXComponent() - iter->getXLocation() > rectSize) {
//        cout << "BX: " << ballCenterX << endl;
//        cout << "MY X: " << iter->getXLocation() << endl;
//        cout << "HERE!" << endl;
//        double xx = iter->getXComponent();
//        cout << xx << endl;
//        xx *= -1;
//        cout << xx << endl;
//        iter->setXComponent(xx);
//    }
//    
//    if(iter->getXLocation() - iter->getRadius() + iter->getXComponent() < -rectSize) {
//        iter->setXComponent(-1 * iter->getXComponent());
//    }
//    
//    if(iter->getYLocation() + iter->getRadius() + iter->getYComponent() > rectSize) {
//        iter->setYComponent(-1 * iter->getYComponent());
//    }
//    
//    if(iter->getYLocation() - iter->getRadius() + iter->getYComponent() < -rectSize) {
//        iter->setYComponent(-1 * iter->getYComponent());
//    }
//    cout << iter->getXComponent() << endl;
//    ballsQuadtree.checkCollisions();
//    ballsQuadtree.clearTree();
//}
//
//// This function will return the ball's translation components for a given point in time
//// in terms of where its center will move.
//// It will also recalculate the velocity vector of the ball's motion for each step in time
//
//void moveBall(int& t) {
//    
//    //time (t) is initially 0 and it is unaffected by drag
//    
//    checkCollisions();
//    ballCenterX += velocityX; //the ball's center is moved by its velocity in that direction
//    ballCenterY += velocityY;
//    
//    velocityX += -frictionCo*t*velocityX; //change the velocity of the object according to drag
//    velocityY += -frictionCo*t*velocityY;
//    
//    //recalculate the magnitude of the velocity
//    velocityMag = sqrt(pow(velocityX, 2.0) + pow(velocityY, 2.0));
//    
//    t+= 1; //increment time
//    /*cout << "velo: " << velocityX << endl;
//    cout << ballCenterX << endl;*/
//    glutPostRedisplay();
//}
//
//// Function to initially calculate the velocity of the ball's rolling when it is hit
//// applies only to cue ball
//void calculateVelocity(float x, float y) {
//    
//    // scale the coordinates of the screen into the coordinates of the near screen
//    /*
//     since the width of the screen is 750, half of that is 375.
//     and the width of the near plane of the frustum is 10, it goes from -5 to 5.
//     this length of 10 is what is projected onto the screen width of 750 so each unit
//     of the frustum has a length of 75 in terms of the screen.
//     
//     and since the "far" end of our frustum is located at -15, the ratio between the far
//     and near planes is 3, so the coordinates obtained on the near plane must be multiplied
//     by three to properly end up on the far plane
//     
//     the same logic follows for the height of the screen, but since the viewing window
//     is not square, a different constant needs to be the divisor.
//     
//     */
//    x = (x-(width/2.0)) / widthRatio;
//    x *= ratioOfFarPlaneToNear; //ratio of how much it increases between the two planes
//    y = (y-(height/2.0)) / heightRatio;
//    y *= ratioOfFarPlaneToNear;
//    
//    //calculate X component
//    
//    vector<PoolBall>::iterator iter = PoolBalls.begin();
//    cout << "*************************"<<endl;
//    //velocityX = (ballCenterX - x) / 10;
//    cout << ballCenterX << iter->getXLocation() << endl;
//    /*velocityX = (iter->getXLocation() - x) / 10;*/
//    iter->setXComponent((iter->getXLocation() - x) / 10);
//    cout << iter->getXComponent() << endl;
//    //calculate Y component
//    //velocityY = (ballCenterY - y) / 10;
//    velocityY = (iter->getYLocation() - y) / 10;
//    //calculate the magnitude of this vector
//    velocityMag = sqrt(pow(velocityX, 2.0) + pow(velocityY, 2.0));
//    
//    //calculate the axis of the ball's rotation given its components
//    calculateRotationAxis(velocityX, velocityY);
//    
//}
//
//// Function to calculate the rotation axis of the ball's rolling
//void calculateRotationAxis(float x, float y) {
//    
//    //this function will multiply a vector of (x, y) by (ysin(90), -xsin(90)) to yield the
//    //new direction of the rotation axis
//    
//    rotationX = sin(PI/2) * y;
//    rotationY = -sin(PI/2) * x;
//}
//
//// Main routine.
/*int main(int argc, char **argv)
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
    glutTimerFunc(50, animate, 1);
    glutMainLoop();
    
    return 0;
}*/