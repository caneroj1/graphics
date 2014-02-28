#include "QuadHeader.h"

std::vector<PoolBall> PoolBalls;
const int NUMBALLS = 15;
int checkDiscRectangleIntersection(float x1, float y1, float x2, float y2, float x3, float y3, float r);
// Quadtree node class.
class QuadtreeNode
{
public:
    QuadtreeNode(float x, float y, float s);
    int numberBallsIntersected(); // Return the number of balls intersecting the square.
    void addIntersectingBallsToList(); // Add the intersected balls to the local vector of balls.
    void clear();   //recursively clears the nodes
    void build(); // Recursive routine to split a square that intersects more than 10 balls;
    void ballCollisions();
    // if it intersects at most 10 balls leave it as a leaf and add the intersecting
    // balls, if any, to a local list of balls.
    
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

// Return the number of asteroids intersecting the square.
int QuadtreeNode::numberBallsIntersected()
{
    int numVal = 0;
    int j;
    for (j=0; j<NUMBALLS; j++)
        if (PoolBalls[j].getRadius() > 0.0)
            if ( checkDiscRectangleIntersection( SWCornerX, SWCornerY, SWCornerX+size, SWCornerY-size,
                                                PoolBalls[j].getXLocation(), PoolBalls[j].getYLocation(),
                                                PoolBalls[j].getRadius() )
                )
                numVal++;
    return numVal;
}

// Add the asteoroids intersecting the square to a local list of asteroids.
void QuadtreeNode::addIntersectingBallsToList()
{
    int j;
    for (j=0; j<NUMBALLS; j++)
        if (PoolBalls[j].getRadius() > 0.0)
            if ( checkDiscRectangleIntersection( SWCornerX, SWCornerY, SWCornerX+size, SWCornerY-size,
                                                PoolBalls[j].getXLocation(), PoolBalls[j].getYLocation(),
                                                PoolBalls[j].getRadius() )
                )
                ballList.push_back( PoolBall(PoolBalls[j]) );
}

// Recursive routine to split a square that intersects more than one asteroid; if it intersects
// at most one asteroid leave it as a leaf and add the intersecting asteroid, if any, to a local
// list of asteroids.
void QuadtreeNode::build()
{
    if ( this->numberBallsIntersected() <= 10 ) this->addIntersectingBallsToList();
    else
    {
        SWChild = new QuadtreeNode(SWCornerX, SWCornerY, size/2.0);
        NWChild = new QuadtreeNode(SWCornerX, SWCornerY - size/2.0, size/2.0);
        NEChild = new QuadtreeNode(SWCornerX + size/2.0, SWCornerY - size/2.0, size/2.0);
        SEChild = new QuadtreeNode(SWCornerX + size/2.0, SWCornerY, size/2.0);
        
        SWChild->build(); NWChild->build(); NEChild->build(); SEChild->build();
    }
}

void QuadtreeNode::ballCollisions() {
    if (SWChild == NULL) {//this square is a leaf, so we do not have to check the children, just the balls in this square
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
        std::vector<PoolBall> ballList2 = ballList;
        for (std::vector<PoolBall>::iterator iter1 = ballList.begin(); iter1 != ballList.end(); iter1++) {
            for (std::vector<PoolBall>::iterator iter2 = ballList2.begin(); iter2 != ballList2.end(); iter2++) {
                if(iter1->PoolBall::isColliding(*iter2)) ComputeElasticCollision(*iter1, *iter2);
            }
            ballList2.erase(ballList2.begin());
        }
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
    header->clear();
}

void Quadtree::checkCollisions() {
    header->ballCollisions();
}

Quadtree ballsQuadtree; // Global quadtree.

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