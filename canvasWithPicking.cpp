////////////////////////////////////////////////////////////////////////////////////
// Joe Canero, CSC350, Assignment 6
//
// canvas.cpp
//
// This program allows the user to draw simple shapes on a canvas with the added functionality
// of being able to move and delete shapes
//
// Interaction:
// Left click on a box on the left to select a primitive.
// Then left click on the drawing area: once for point, twice for line or rectangle.
//
// The "Move" option is indicated by a green arrow. After selecting that option, the user must
// click on a shape and then click another location on the canvas to move it there
//
// The "Delete" option is indicated by a red X. After selecting tat option, the user must
// click on a shape and it will be deleted.
//
// Right click for menu options.
//
////////////////////////////////////////////////////////////////////////////////////


//each class has edit function and delete functions that compare the current
//value of closest name with that object's ID.
//EDIT: if they match then that object will be shifted according to the
//distance from the original click and the second click
//DELETE: if they match then this function will true, indicating that this shape
//should be removed from its storage vector

#include <cstdlib>
#include <vector>
#include <iostream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

#define INACTIVE 0
#define POINT 1
#define LINE 2
#define RECTANGLE 3
#define PICKING 4 //this is the definition of the "Pick and Move" icon
#define DELETE 5 //this is the definition of the "Delete" icon
#define NUMBERPRIMITIVES 5

// Use the STL extension of C++.
using namespace std;

// Globals.
static GLsizei width, height;          // OpenGL window size.
static float pointSize = 3.0;          // Size of point
static int primitive = INACTIVE;       // Current drawing primitive.
static int pointCount = 0;             // Number of  specified points.
static int tempX, tempY;               // Co-ordinates of clicked point.
static int isGrid = 1;                 // Is there grid?
static int isSelecting = 0;            // In selection mode?
static bool isMoving = false;          //bool that indicates whether the next mouse click will move the picked object 
static int hits;                       // Number of entries in hit buffer.
static unsigned int buffer[1024];      // Hit buffer.
static int closestName = 0;            // Name of closest hit.
static unsigned int numberOfNames = 1; //the current number of names stored for all objects
                                       //it will be incremented whenever a new object is made and that value will be that object's id

// Point class.
class Point
{
public:
    Point(int xVal, int yVal)
    {
        objectID = numberOfNames; //set this object's ID to the current value of numberOfNames
        numberOfNames++;          // increment, since we have created a new object
        x = xVal; y = yVal;
    }
    void drawPoint(void);         // Function to draw a point.
    void editPoint(int, int);     // Function to edit the location of the point
    bool deletePoint();
private:
    int x, y;                     // x and y co-ordinates of point.
    int objectID;                 // ID of this object
    static float size;            // Size of point.
};

float Point::size = pointSize; // Set point size.

// Function to draw a point.
void Point::drawPoint()
{
    if(objectID == closestName) glColor3f(1.0, 0.0, 0.0); //draw the object in a red outline if it is going to be moved
    if(isSelecting) glLoadName(objectID);
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex3f(x, y, 0.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
}

// Function to edit the location of a point
// since a point only knows its x- and y-coordinates, those coordiantes will become
// the location of the spot the user clicked
void Point::editPoint(int newX, int newY)
{
    if (objectID == closestName) {
        x = newX;
        y = newY;
    }
}

// Function to delete a point
// This function returns a bool indicating if this point should be deleted and removed
// from the points vector
bool Point::deletePoint() {
    if(objectID == closestName) return true;
    else return false;
}

// Vector of points.
vector<Point> points;

// Iterator to traverse a Point array.
vector<Point>::iterator pointsIterator;

// Function to draw all points in the points array.
void drawPoints(void)
{
    // Loop through the points array drawing each point.
    pointsIterator = points.begin();
    while(pointsIterator != points.end() )
    {
        pointsIterator->drawPoint();
        pointsIterator++;
    }
}

// Function to remove a point from its vector
// This function loops through the points and determines which one should be deleted
// if deletePoint() returns a true, that point will be removed
void deletePoints() {
    for (int i = 0; i < points.size(); i++) {
        if(points[i].deletePoint()) {
            points.erase(points.begin() + i);
        }
    }
}

// Function to loop through the points array and edit the correct point
void editPoints(int x, int y)
{
    // Loop through the points array and call the editPoint function for the correct point
    pointsIterator = points.begin();
    while (pointsIterator != points.end()) {
        pointsIterator->editPoint(x, y);
        pointsIterator++;
    }
}

// Line class.
class Line
{
public:
    Line(int x1Val, int y1Val, int x2Val, int y2Val)
    {
        x1 = x1Val; y1 = y1Val; x2 = x2Val; y2 = y2Val;
        objectID = numberOfNames;                       // set this object's ID
        numberOfNames++;                                // increment
    }
    void drawLine();
    void editLine(int, int);                            // Function to edit a line
    bool deleteLine();                                  // Function to delete a lline
private:
    int x1, y1, x2, y2;                                 // x- and y-coordinates of endpoints.
    int objectID;                                       // ID of this object
};


// Function to draw a line.
void Line::drawLine()
{
    if(objectID == closestName) glColor3f(1.0, 0.0, 0.0); //draw the object in a red outline if it is going to be moved
    if(isSelecting) glLoadName(objectID);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, 0.0);
    glVertex3f(x2, y2, 0.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);

}

// Function to delete a line
// if the line's id matches the id of the selected object, it will return a boolean
// indicating that the line should be deleted and popped from its storage vector
bool Line::deleteLine() {
    if(objectID == closestName) {
        return true;
    }
    else return false;
}

// Function to edit a line
void Line:: editLine(int newX, int newY) {
    
    // calculate the distance between the new coordinates and the old one
    // shift the line's vertices by that much
    if(objectID == closestName) {
        int distanceX = newX - tempX;
        int distanceY = newY - tempY;
        
        //shift each vertex's location
        x1 += distanceX;
        x2 += distanceX;
        y1 += distanceY;
        y2 += distanceY;
    }
}

// Vector of lines.
vector<Line> lines;

// Iterator to traverse a Line array.
vector<Line>::iterator linesIterator;

// Function to draw all lines in the lines array.
void drawLines(void)
{
    // Loop through the lines array drawing each line.
    linesIterator = lines.begin();
    while(linesIterator != lines.end() )
    {
        linesIterator->drawLine();
        linesIterator++;
    }
}

// Function to loop through the lines and delete the right one when deleteLine() returns true
void deleteLines() {
    for (int i = 0; i < lines.size(); i++) {
        if(lines[i].deleteLine()) {
            lines.erase(lines.begin() + i);
        }
    }
}

// Function to loop through the lines and edit the proper one
void editLines(int x, int y)
{
    // Loop through the lines array editing the proper line.
    linesIterator = lines.begin();
    while(linesIterator != lines.end() )
    {
        linesIterator->editLine(x, y);
        linesIterator++;
    }
}

// Rectangle class.
class Rectangle
{
public:
    Rectangle(int x1Val, int y1Val, int x2Val, int y2Val)
    {
        x1 = x1Val; y1 = y1Val; x2 = x2Val; y2 = y2Val;
        objectID = numberOfNames;                           // Set the ID of this object
        numberOfNames++;                                    // increment
    }
    void drawRectangle();
    bool deleteRectangle();                                 //Function to delete a rectangle
    void editRect(int, int);                                //Function to edit a rectangle
private:
    int x1, y1, x2, y2;                                     // x- and y-coordinates of diagonally opposite vertices.
    int objectID;                                           // ID of this object
};

// Function to delete a rectangle
// if the rectangle's id matches the id of the selected object, it will return a boolean
// indicating that the rectangle should be deleted and popped from its storage vector
bool Rectangle::deleteRectangle() {
    if (objectID == closestName) {
        return true;
    }
    else return false;
}

// Function to draw a rectangle.
void Rectangle::drawRectangle()
{
    if(isSelecting) glLoadName(objectID);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if(objectID == closestName) glColor3f(1.0, 0.0, 0.0); //draw the object in a red outline if it is going to be moved
    glRectf(x1, y1, x2, y2);
    glColor3f(0.0, 0.0, 0.0);

}

// this function will edit the rectangles location by computing the distance between
// the pixels selected and then shift the vertices of the rectangle by that distance
void Rectangle::editRect(int newX, int newY) {
    if(objectID == closestName) {
        // calculate the distance between the new coordinates and the old one
        // shift the rectangle's vertices by that much
        int distanceX = newX - tempX;
        int distanceY = newY - tempY;
        
        x1 += distanceX;
        x2 += distanceX;
        y1 += distanceY;
        y2 += distanceY;
    }
}

// Vector of rectangles.
vector<Rectangle> rectangles;

// Iterator to traverse a Rectangle array.
vector<Rectangle>::iterator rectanglesIterator;

// Function to draw all rectangles in the rectangles array.
void drawRectangles(void)
{
    // Loop through the rectangles array drawing each rectangle.
    rectanglesIterator = rectangles.begin();
    while(rectanglesIterator != rectangles.end() )
    {
        rectanglesIterator->drawRectangle();
        rectanglesIterator++;
    }
}

// Function to loop through all the rectangles and edit the right one
void editRectangles(int x, int y)
{
    // Loop through the rectangles array and edit the right one
    rectanglesIterator = rectangles.begin();
    while(rectanglesIterator != rectangles.end() )
    {
        rectanglesIterator->editRect(x, y);
        rectanglesIterator++;
    }
}

// Function to loop through all the rectangles and delete the appropriate one
void deleteRectangles() {
    for (int i = 0; i < rectangles.size(); i++) {
        if(rectangles[i].deleteRectangle()) {
            rectangles.erase(rectangles.begin() + i);
        }
    }
}

// Function to draw point selection box in left selection area.
void drawPointSelectionBox(void)
{
    if (primitive == POINT) glColor3f(1.0, 1.0, 1.0); // Highlight.
    else glColor3f(0.8, 0.8, 0.8); // No highlight.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.9*height, 0.1*width, height);
    
    // Draw black boundary.
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.9*height, 0.1*width, height);
    
    // Draw point icon.
    glPointSize(pointSize);
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_POINTS);
    glVertex3f(0.05*width, 0.95*height, 0.0);
    glEnd();
}

// Function to draw line selection box in left selection area.
void drawLineSelectionBox(void)
{
    if (primitive == LINE) glColor3f(1.0, 1.0, 1.0); // Highlight.
    else glColor3f(0.8, 0.8, 0.8); // No highlight.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.8*height, 0.1*width, 0.9*height);
    
    // Draw black boundary.
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.8*height, 0.1*width, 0.9*height);
    
    // Draw line icon.
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.025*width, 0.825*height, 0.0);
    glVertex3f(0.075*width, 0.875*height, 0.0);
    glEnd();
}

// Function to draw rectangle selection box in left selection area.
void drawRectangleSelectionBox(void)
{
    if (primitive == RECTANGLE) glColor3f(1.0, 1.0, 1.0); // Highlight.
    else glColor3f(0.8, 0.8, 0.8); // No highlight.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.7*height, 0.1*width, 0.8*height);
    
    // Draw black boundary.
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.7*height, 0.1*width, 0.8*height);
    
    // Draw rectangle icon.
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.025*width, 0.735*height, 0.075*width, 0.765*height);
    glEnd();
}

// Function to draw picking selection box in left selection area.
void drawPickingSelectionBox(void) {
    if (primitive == PICKING) glColor3f(1.0, 1.0, 1.0); // Highlight.
    else glColor3f(0.8, 0.8, 0.8); //no highlight
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.6*height, 0.1*width, 0.7*height);
    
    // Draw black boundary.
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.6*height, 0.1*width, 0.7*height);
    
    //draw picking icon: a green arrow
    glColor3f(0.0, 1.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glBegin(GL_LINES);
    glVertex3f(0.005*width, 0.650*height, 0.0);
    glVertex3f(0.045*width, 0.650*height, 0.0);
    glEnd();
    
    glBegin(GL_TRIANGLES);
    glVertex3f(0.045*width, 0.625*height, 0.0);
    glVertex3f(0.045*width, 0.675*height, 0.0);
    glVertex3f(0.095*width, 0.650*height, 0.0);
    glEnd();
}

// Function to draw the delete selection box in left selection area
void drawDeleteSelectionBox(void) {
    if (primitive == DELETE) glColor3f(1.0, 1.0, 1.0); // Highlight.
    else glColor3f(0.8, 0.8, 0.8); //no highlight
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.5*height, 0.1*width, 0.6*height);
    
    // Draw black boundary.
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.5*height, 0.1*width, 0.6*height);
    
    //draw delete icon
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    //the delete icon is a red X
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    glVertex3f(0.015*width, 0.585*height, 0.0);
    glVertex3f(0.085*width, 0.515*height, 0.0);
    glVertex3f(0.015*width, 0.515*height, 0.0);
    glVertex3f(0.085*width, 0.585*height, 0.0);
    glEnd();
}

// Function to draw unused part of left selection area.
void drawInactiveArea(void)
{
    glColor3f(0.6, 0.6, 0.6);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.0, 0.1*width, (1 - NUMBERPRIMITIVES*0.1)*height);
    
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.0, 0.1*width, (1 - NUMBERPRIMITIVES*0.1)*height);
}

// Function to draw temporary point.
void drawTempPoint(void)
{
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(pointSize);
    glBegin(GL_POINTS);
    glVertex3f(tempX, tempY, 0.0);
    glEnd();
}

// Function to draw a grid.
void drawGrid(void)
{
    int i;
    
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x5555);
    glColor3f(0.75, 0.75, 0.75);
    
    glBegin(GL_LINES);
    for (i=2; i <=9; i++)
    {
        glVertex3f(i*0.1*width, 0.0, 0.0);
        glVertex3f(i*0.1*width, height, 0.0);
    }
    for (i=1; i <=9; i++)
    {
        glVertex3f(0.1*width, i*0.1*height, 0.0);
        glVertex3f(width, i*0.1*height, 0.0);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);
    
    isSelecting = 0;
    
    drawPoints();
    drawLines();
    drawRectangles();
    
    drawPointSelectionBox();
    drawLineSelectionBox();
    drawRectangleSelectionBox();
    drawPickingSelectionBox();
    drawDeleteSelectionBox();
    drawInactiveArea();
    
    if ( ((primitive == LINE) || (primitive == RECTANGLE)) &&
        (pointCount == 1) ) drawTempPoint();
    if (isGrid) drawGrid();
    
    glutSwapBuffers();
}

// Function to pick primitive if click is in left selection area.
void pickPrimitive(int y)
{
    if ( y < (1- NUMBERPRIMITIVES*0.1)*height ) primitive = INACTIVE;
    else if ( y < (1 - 4*0.1)*height ) primitive = DELETE;
    else if ( y < (1 - 3*0.1)*height ) primitive = PICKING;
    else if ( y < (1 - 2*0.1)*height ) primitive = RECTANGLE;
    else if ( y < (1 - 1*0.1)*height ) primitive = LINE;
    else primitive = POINT;
}

// Process hit buffer to find record with smallest min-z value.
void findClosestHit(int hits, unsigned int buffer[])
{
    unsigned int *ptr, minZ;
    
    minZ= 0xffffffff; // 2^32 - 1
    ptr = buffer;
    closestName = 0;
    for (int i = 0; i < hits; i++)
    {
        ptr++;
        if (*ptr < minZ)
        {
            minZ = *ptr;
            ptr += 2;
            closestName = *ptr;
            ptr++;
        }
        else ptr += 3;
    }
    if (closestName != 0 && primitive == PICKING) isMoving = true;
}

// The mouse callback routine.
void mouseControl(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        y = height - y; // Correct from mouse to OpenGL co-ordinates.
        
        // Click outside canvas - do nothing.
        if ( x < 0 || x > width || y < 0 || y > height ) ;
        
        // Click in left selection area.
        else if ( x < 0.1*width )
        {
            pickPrimitive(y);
            pointCount = 0;
        }
        
        // Click in canvas.
        else
        {
            if (primitive == POINT) points.push_back( Point(x,y) );
            else if (primitive == LINE)
            {
                if (pointCount == 0)
                {
                    tempX = x; tempY = y;
                    pointCount++;
                }
                else
                {
                    lines.push_back( Line(tempX, tempY, x, y) );
                    pointCount = 0;
                }
            }
            else if (primitive == RECTANGLE)
            {
                if (pointCount == 0)
                {
                    tempX = x; tempY = y;
                    pointCount++;
                }
                else
                {
                    rectangles.push_back( Rectangle(tempX, tempY, x, y) );
                    pointCount = 0;
                }
            }
            // if the user has opted to delete or pick, this function is called
            else if ((primitive == PICKING || primitive == DELETE) && !isMoving)
            {
                tempX = x; tempY = y; // when moving an object, the position of the first
                                      // mouse click must be stored
                
                //PICKING FUNCTION
                //************************************************************************
                
                int viewport[4]; // Viewport data.
                
                if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return; // Don't react unless left button is pressed.
                
                glGetIntegerv(GL_VIEWPORT, viewport); // Get viewport data.
                
                glSelectBuffer(1024, buffer); // Specify buffer to write hit records in selection mode
                (void) glRenderMode(GL_SELECT); // Enter selection mode.
                
                // Save the viewing volume defined in the resize routine.
                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                
                // Define a viewing volume corresponding to selecting in 5 x 5 region around the cursor.
                glLoadIdentity();
                gluPickMatrix((float)x, (float)(y), 5.0, 5.0, viewport); // pick matrix
                glOrtho(0.0, (float)width, 0.0, (float)height, -1.0, 1.0); // copied from reshape

                
                glMatrixMode(GL_MODELVIEW); // Return to modelview mode before drawing.
                glLoadIdentity();
                
                glInitNames(); // Initializes the name stack to empty.
                glPushName(0); // Puts name 0 on top of stack.
                
                // Determine hits by calling the different drawing routines so that names are assigned.
                isSelecting = 1;
                drawPoints();
                drawLines();
                drawRectangles();
                
                hits = glRenderMode(GL_RENDER); // Return to rendering mode, returning number of hits.
                
                // Restore viewing volume of the resize routine and return to modelview mode.
                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                
                // Determine closest of the hit objects (if any).
                findClosestHit(hits, buffer);
            }
            else if(primitive == PICKING && isMoving) {
                // now we have clicked on a new spot and we want to move the selected object
                // to the new spot
                // the isMoving bool indicates that the selected object is being moved to
                // a new location
                editLines(x, y);
                editPoints(x, y);
                editRectangles(x, y);
                isMoving = false; // the right object has been moved, set to false
                closestName = 0; // set closest name back to 0, no object should be selected
            }
            if(primitive == DELETE && closestName > 0) {
                // now we have clicked on an object and we want to delete it
                // call the delete functions for all the shapes to find the shape that needs
                // to be deleted
                deleteRectangles();
                deleteLines();
                deletePoints();
                closestName = 0; // set closest name back to 0, no object should be selected
            }
        }
    }
    glutPostRedisplay();
}

// Initialization routine.
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set viewing box dimensions equal to window dimensions.
    glOrtho(0.0, (float)w, 0.0, (float)h, -1.0, 1.0);
    
    // Pass the size of the OpenGL window to globals.
    width = w;
    height = h;
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27:
            exit(0);
            break;
        default:
            break;
    }
}

// Clear the canvas and reset for fresh drawing.
void clearAll(void)
{
    points.clear();
    lines.clear();
    rectangles.clear();
    primitive = INACTIVE;
    numberOfNames = 1;
    pointCount = 0;
}

// The right button menu callback function.
void rightMenu(int id)
{
    if (id==1)
    {
        clearAll();
        glutPostRedisplay();
    }
    if (id==2) exit(0);
}

// The sub-menu callback function.
void grid_menu(int id)
{
    if (id==3) isGrid = 1;
    if (id==4) isGrid = 0;
    glutPostRedisplay();
}

// Function to create menu.
void makeMenu(void)
{
    int sub_menu;
    sub_menu = glutCreateMenu(grid_menu);
    glutAddMenuEntry("On", 3);
    glutAddMenuEntry("Off",4);
    
    glutCreateMenu(rightMenu);
    glutAddSubMenu("Grid", sub_menu);
    glutAddMenuEntry("Clear",1);
    glutAddMenuEntry("Quit",2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Left click on a box on the left to select a primitive." << endl
    << "Then left click on the drawing area: once for point, twice for line or rectangle. The green arrow indicates the MOVE function and will move a selected shape to a new spot. The red X indicates the DELETE function and will deleted the selected shape." << endl
    << "Right click for menu options." <<  endl; 
}

// Main routine.
int main(int argc, char **argv) 
{
    printInteraction();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DOUBLE); 
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100); 
    glutCreateWindow("canvas.cpp");
    setup(); 
    glutDisplayFunc(drawScene); 
    glutReshapeFunc(resize);  
    glutKeyboardFunc(keyInput);
    glutMouseFunc(mouseControl); 
    
    makeMenu(); // Create menu.
    
    glutMainLoop(); 
    
    return 0;  
}

