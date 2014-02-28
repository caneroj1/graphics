/*  ElasticCollision Header
    CSC350, Final Project
    Created by Joseph Canero
 
    This is a header file that contains the declaration of a function that will handle elastic collisions between two pool balls
 */

#ifndef __PoolBall__ElasticCollision__
#define __PoolBall__ElasticCollision__

#define PI 3.141596

#include "PoolBall.h"
#include <iostream>
//function to update the velocities of two pool balls if they are colliding
void ComputeElasticCollision(PoolBall &Ball1, PoolBall &Ball2);

//function to compute the scalar component of a projection
double calcProjection(double, double, double, double);

#endif /* defined(__PoolBall__ElasticCollision__) */
