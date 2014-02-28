/*  PoolBall Header
    CSC350, Final Project
    Created by Joseph Canero

    This header file contains the definition for the PoolBall class, each instance of which is a pool ball (obviously)
 
    What does a pool ball need to know?
    A pool ball also needs to know about its physical characteristics! It will need to know its radius and its mass for collision equations.
    A pool ball needs to know its velocity--so it will store the x-,y-,z-components of the motion of its center.
    A pool ball also needs to know where it is at any given time; therefore, each pool ball will store the x-,y-,z-coordinates of its center.
    A ball will also need to store the angle of its rotation
    What will a pool ball be able to do with this information?
    A pool ball needs to know if it is colliding with another pool ball, so that means that this class must have a function to check to see if another pool ball is within collision radius!
    A pool ball will not need to update its velocity if there is a collision, for that is something that the physics methods of this project will handle. All that is required of each pool ball is that it be nice and offer functions to get and set its attributes, such as velocity and position and mass.
    A pool ball also needs to update its location after each frame, so it needs a function that will change its coordinates based on its velocity
    Finally, A ball will need its own draw method that will apply the proper transformations in this order: rotation, movement translation, translation into frustum
*/


/* TO ADD:
 
            FUNCTIONS FOR DRAWING EACH BALL
 */


#ifndef PoolBall_Header_h
#define PoolBall_Header_h
#ifdef __APPLE__
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glut.h>
#endif
#include <cmath>
#include <iostream>

class PoolBall  {

//private data for each pool ball object
private:
    double xV; double yV; double zV;            //the components of a ball's velocity
    double xLoc; double yLoc; double zLoc;      //the coordinates of a ball's location
    double angle;                               //angle of the ball's rotation
    float radius;                               //radius of a ball
    float mass;                                 //mass of a ball
    int ballID;                                 //numerical integer ID of the ball
    bool marked;                                //bool indicating this ball is marked for deletion
    GLUquadricObj *qobj;                        //Create a pointer to a quadric object.
    float maxVelocity = 1.0;                    //maximum size a specific component of a ball's velocity can be
    
public:
    PoolBall();                                                     //default constructor
    PoolBall(double xPos, double yPos, double zPos, int bID);       //parameterized constructor
    bool isColliding(PoolBall b2);                                  //function will return a bool indicating if another ball
    void updatePosition();                                          //function to update the position of the ball based on its velocity
    bool operator!=(PoolBall);                                      //function to compare two PoolBall objects
    void drawBall();
    
    //GETTERS
    double getXComponent();  //functions to return the components of this ball's velocity, and angle
    double getYComponent();
    double getZComponent();
    double getAngle();
    double getXLocation();   //functions to return the coordinates of this ball's location
    double getYLocation();
    double getZLocation();
    float getMass();         //get mass
    float getRadius();       //get radius
    int getID();             //get the ID
    bool isMarked();
    
    //SETTERS
    void setXComponent(double xVel);  //functions to set the components of this ball's velocity, and angle
    void setYComponent(double yVel);
    void setZComponent(double zVel);
    void setAngle(double ang);        //function to set the angle of the ball's rotation
    void setXLocation(double xL);     //functions to set the coordinates of this ball's location
    void setYLocation(double yL);
    void setZLocation(double zL);
    void setMass(float m);            //function to set the ball's mass
    void setRadius(float r);          //function to set the ball's radius
    void markBall(bool);                  //function that will mark the ball once it is sunk into a pocket
};

#endif
