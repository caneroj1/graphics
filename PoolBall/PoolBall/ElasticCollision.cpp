/*  ElasticCollision Implementation File
    CSC350, Final Project
    Created by Joseph Canero
 
    This is the implementation file of the ElasticCollision header and contains definitions of several functions that will be used to ensure realistc motion is achieved for the pool simulator
 */

#include "ElasticCollision.h"

// Calculates the scalar value of a vector project. x1 and y1 are components of the vector to be projected onto
// the vector <x2, y2>
double calcProjection(double x1, double y1, double x2, double y2) {
    return ((x1 * x2 + y1 * y2) / (x2 * x2 + y2 * y2));
}

void ComputeElasticCollision(PoolBall &Ball1, PoolBall &Ball2) {
    float testFloat = 0.0085;
    if (Ball1 != Ball2) { //only  do the calculation if the balls colliding are two different objects
        if((sqrt(pow(Ball1.getXComponent(),2) + pow(Ball1.getYComponent(), 2)) - testFloat > 0) && (sqrt(pow(Ball2.getXComponent(),2) + pow(Ball2.getYComponent(), 2)) - testFloat > 0)) {
            // Variables that store the velocities of each ball
            double x1 = Ball1.getXComponent();
            double y1 = Ball1.getYComponent();
            double x2 = Ball2.getXComponent();
            double y2 = Ball2.getYComponent();
            
            // Calculate the components of the vector connecting the centers of the two balls
            double nX = Ball2.getXLocation() - Ball1.getXLocation();
            double nY = Ball2.getYLocation() - Ball1.getYLocation();
            
            // Project each ball's movement vector onto the vector connecting their centers
            // Perpendicular vectors are calculated via y - yProj = z, where y represents the original vector, yProj is the projected vector, and z is what we
            // want
            double projection1 = calcProjection(x1, y1, nX, nY);
            double b1ProjX = projection1 * nX; //this vector represents the projection of ball 1's x movement onto the vector connecting their centers
            double b1ProjY = projection1 * nY; //this vector represents the projection of ball 1's y movement onto the vector connecting their centers
            double b1PerpX = x1 - b1ProjX; //calculate the perpendicular vector to the projection
            double b1PerpY = y1 - b1ProjY;
            
            
            // Project the second ball's movement onto the oppositely oriented movement vector connecting their centers
            // Perpendicular vectors are calculated same as above
            double projection2 = calcProjection(x2, y2, -nX, -nY);
            double b2ProjX = projection2 * -nX;
            double b2ProjY = projection2 * -nY;
            double b2PerpX = x2 - b2ProjX;
            double b2PerpY = y2 - b2ProjY;
            
            // Recombine the vectors for each ball and give them a little push in that direction
            Ball1.setXComponent(b2ProjX  + b1PerpX);
            Ball1.setYComponent(b2ProjY + b2PerpY);
            Ball1.setXLocation(Ball1.getXLocation() + Ball1.getXComponent());
            Ball1.setYLocation(Ball1.getYLocation() + Ball1.getYComponent());
            
            Ball2.setXComponent(b1ProjX + b2PerpX );
            Ball2.setYComponent(b1ProjY + b2PerpY );
            Ball2.setXLocation(Ball2.getXLocation() + Ball2.getXComponent());
            Ball2.setYLocation(Ball2.getYLocation() + Ball2.getYComponent());
        }
        else if ((sqrt(pow(Ball1.getXComponent(),2) + pow(Ball1.getYComponent(), 2)) - testFloat > 0)) { //if ball 1 is moving, ball 2 not
            // Variables that store the velocities of each ball
            double x1 = Ball1.getXComponent();
            double y1 = Ball1.getYComponent();
            
            // Calculate the components of the vector connecting the centers of the two balls
            double nX = Ball2.getXLocation() - Ball1.getXLocation();
            double nY = Ball2.getYLocation() - Ball1.getYLocation();
            
            // Project each ball's movement vector onto the vector connecting their centers
            // Perpendicular vectors are calculated via y - yProj = z, where y represents the original vector, yProj is the projected vector, and z is what we
            // want
            double projection1 = calcProjection(x1, y1, nX, nY);
            double b1ProjX = projection1 * nX; //this vector represents the projection of ball 1's x movement onto the vector connecting their centers
            double b1ProjY = projection1 * nY; //this vector represents the projection of ball 1's y movement onto the vector connecting their centers
            double b1PerpX = x1 - b1ProjX; //calculate the perpendicular vector to the projection
            double b1PerpY = y1 - b1ProjY;
            
            //set new velocities for each ball
            Ball1.setXComponent(b1PerpX);
            Ball1.setYComponent(b1PerpY);
            
            Ball2.setXComponent(b1ProjX);
            Ball2.setYComponent(b1ProjY);
            
        }
        else if ((sqrt(pow(Ball2.getXComponent(),2) + pow(Ball2.getYComponent(), 2)) - testFloat > 0)) { //if ball 2 moving, ball 1 not
            // Variables that store the velocities of each ball
            double x2 = Ball2.getXComponent();
            double y2 = Ball2.getYComponent();
            
            // Calculate the components of the vector connecting the centers of the two balls
            double nX = Ball1.getXLocation() - Ball2.getXLocation();
            double nY = Ball1.getYLocation() - Ball2.getYLocation();
            
            // Project the second ball's movement onto the oppositely oriented movement vector connecting their centers
            // Perpendicular vectors are calculated same as above
            double projection2 = calcProjection(x2, y2, nX, nY);
            double b2ProjX = projection2 * nX;
            double b2ProjY = projection2 * nY;
            double b2PerpX = x2 - b2ProjX;
            double b2PerpY = y2 - b2ProjY;
            
            //set new velocities for each ball
            Ball1.setXComponent(b2ProjX);
            Ball1.setYComponent(b2ProjY);
            
            Ball2.setXComponent(b2PerpX);
            Ball2.setYComponent(b2PerpY);
        }
    }
}