////////////////////////////////////////////////////////////////////////////////////
// Joe Canero
// CSC350, Computer Graphics, Dr. Salgian
// Assignment 3
//
// This program allows the user to draw simple shapes on a canvas, including points,
// lines, rectangles, circles, and multiple-segment lines
//
// Interaction:
// Left click on a box on the left to select a primitive.
// Then left click on the drawing area: once for point, twice for line or rectangle or circle.
// Or two or more times for the PolyLine
//
// Right click for menu options. Menu options include grid display, fill, and colors
//
////////////////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <vector>
#include <iostream>
#include <cmath>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define INACTIVE 0
#define POINT 1
#define LINE 2
#define RECTANGLE 3
#define CIRCLE 4
#define POLYLINE 5
#define NUMBERPRIMITIVES 5

// Use the STL extension of C++.
using namespace std;

// Globals.
static GLsizei width, height; // OpenGL window size.
static float pointSize = 3.0; // Size of point
static int primitive = INACTIVE; // Current drawing primitive.
static int pointCount = 0; // Number of  specified points.
static int tempX, tempY; // Co-ordinates of clicked point.
static int isGrid = 1; // Is there grid?
static float colors[3] = {0.0, 0.0, 0.0}; //array of colors
static float const PI = 3.14159; //PI
static vector<float> PolyLinePoints; // a vector that will hold points for each polyline object

//vector::clear() was not working as intended, so this is an empty vector that will be used
//to reset the PolyLinePoints vector every time 'e' is pressed
static vector<float> clearedArray;
static bool ePressed = false; //if e key is pressed, draw polyLine
static bool isFill; //bool variable to determine if filled or not

// Point class.
class Point
{
public:
    Point(int xVal, int yVal)
    {
        x = xVal; y = yVal;
        //Point gets color data from the current colors
        color[0] = colors[0];
        color[1] =  colors[1];
        color[2] = colors[2];
    }
    void drawPoint(void); // Function to draw a point.
private:
    int x, y; // x and y co-ordinates of point.
    static float size; // Size of point.
    float color[3] = {0.0, 0.0, 0.0};
};

float Point::size = pointSize; // Set point size.

// Function to draw a point.
void Point::drawPoint()
{
    glPointSize(size);
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_POINTS);
    glVertex3f(x, y, 0.0);
    glEnd();
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

// Line class.
class Line
{
public:
    Line(int x1Val, int y1Val, int x2Val, int y2Val)
    {
        x1 = x1Val; y1 = y1Val; x2 = x2Val; y2 = y2Val;
        //Line gets color data from the current colors
        color[0] = colors[0];
        color[1] =  colors[1];
        color[2] = colors[2];
    }
    void drawLine();
private:
    int x1, y1, x2, y2; // x and y co-ordinates of endpoints.
    float color[3] = {0.0, 0.0, 0.0};
};


// Function to draw a line.
void Line::drawLine()
{
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINES);
    glVertex3f(x1, y1, 0.0);
    glVertex3f(x2, y2, 0.0);
    glEnd();
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

// Rectangle class.
class Rectangle
{
public:
    Rectangle(int x1Val, int y1Val, int x2Val, int y2Val, bool fill)
    {
        x1 = x1Val; y1 = y1Val; x2 = x2Val; y2 = y2Val;
        filled = fill;
        //Rectangle gets color data from the current colors
        color[0] = colors[0];
        color[1] =  colors[1];
        color[2] = colors[2];
    }
    void drawRectangle();
private:
    int x1, y1, x2, y2; // x and y co-ordinates of diagonally opposite vertices.
    bool filled;
    float color[3] = {0.0, 0.0, 0.0};
};

// Function to draw a rectangle.
void Rectangle::drawRectangle()
{
    if(filled) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(color[0], color[1], color[2]);
    glRectf(x1, y1, x2, y2);
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

//********************************************************************************
//CIRCLES

//Circle class
class Circle {
    
    //private data for the circle, coordinates of its center and its radius
private:
    int x; int y; //coordinates of the center
    double R; //radius of the circle
    bool filled;
    float color[3] = {0.0, 0.0, 0.0};
    
public:
    Circle(int centerX, int centerY, int circumX, int circumY, bool fill)
    {
        x = centerX;
        y = centerY;
        //calculate the length of the radius using the distance formula
        R = sqrt( pow(double(x - circumX), 2) + pow(double(y - circumY), 2) );
        //Circle gets color data from the current colors
        color[0] = colors[0];
        color[1] =  colors[1];
        color[2] = colors[2];
        filled = fill;
    }
    void drawCircle(void);
};

//function to draw a circle
void Circle::drawCircle()
{
    if(filled) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(color[0], color[1], color[2]);
    double increment = PI/50.0;
    glBegin(GL_POLYGON);
    //draws a circle using a parametrization
    for (float t = 0; t <= 2 * PI; t += increment) {
        glVertex3f(x + R * cos(t), y + R * sin(t), 0.0);
    }
    glEnd();
    
}

//vector of Circle objects
vector<Circle> circles;

//interator for Circle vector
vector<Circle>::iterator circlesIterator;

//function to draw all of the circles in the array
void drawCircles(void) {
    //loop through each circle in the array
    circlesIterator = circles.begin();
    while(circlesIterator != circles.end()) {
        circlesIterator->drawCircle();
        circlesIterator++;
    }
}

//********************************************************************************

//PolyLine Class

class PolyLine {
private:
    vector<float> pointList; //each polyline will maintain its own list of points
    float color[3] = {0.0, 0.0, 0.0};
    
public:
    PolyLine(vector<float> points) {
        pointList = points; //the passed in global point vector will be copied into private data
        //PolyLine gets color data from the current colors
        color[0] = colors[0];
        color[1] =  colors[1];
        color[2] = colors[2];
    }
    void drawPolyLine(void);
};

//vector of PolyLine objects
vector<PolyLine> polylines;

//iterator for polyline vector
vector<PolyLine>::iterator polylineIterator;

//function for drawing the polylines
void PolyLine::drawPolyLine(void) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < pointList.size(); i += 3) {
        glVertex3f(pointList[i], pointList[i+1], pointList[i+2]);
    }
    glEnd();
}

void drawPolyLines(void) {

    //loop through each polyline in the array to draw it
    polylineIterator = polylines.begin();
    while(polylineIterator != polylines.end()) {
        polylineIterator->drawPolyLine();
        polylineIterator++;
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
    glColor3f(colors[0], colors[1], colors[2]);
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
    glColor3f(colors[0], colors[1], colors[2]);
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
    if (isFill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(colors[0], colors[1], colors[2]);
    glRectf(0.025*width, 0.735*height, 0.075*width, 0.765*height);
    glEnd();
}

//function to draw circle selection box in left selection area
void drawCircleSelectionBox(void)
{
    if (primitive == CIRCLE) glColor3f(1.0, 1.0, 1.0); // Highlight
    else glColor3f(0.8, 0.8, 0.8); // No highlight
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.6*height, 0.1*width, 0.7*height);
    
    //draw black boundary
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.6*height, 0.1*width, 0.7*height);
    
    //draw circle icon
    double R = (0.1*width)/4.0;
    double increment = PI/50.0;
    if (isFill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(colors[0], colors[1], colors[2]);
    glBegin(GL_POLYGON);
    for (float t = 0; t <= 2 * PI; t += increment) {
        glVertex3f((0.1*width)/2.0 + R * cos(t), 0.6*height + ((0.1*width)/2.0) + R * sin(t), 0.0);
    }
    glEnd();
}

//function to draw poly-line selection box in left selection area
void drawPolylineSelectionBox(void) {
    if (primitive == POLYLINE) glColor3f(1.0, 1.0, 1.0); // Highlight
    else glColor3f(0.8, 0.8, 0.8); // No highlight
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glRectf(0.0, 0.5*height, 0.1*width, 0.6*height);
    
    //draw black boundary
    glColor3f(0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(0.0, 0.5*height, 0.1*width, 0.6*height);
    
    //draw polyline icon
    glColor3f(colors[0], colors[1], colors[2]);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.025*width, 0.5*height + 0.05*width, 0.0);
    glVertex3f(0.05*width, 0.6*height - 0.025*width, 0.0);
    glVertex3f(0.075*width, 0.5*height + 0.05*width, 0.0);
    glVertex3f(0.05*width, 0.5*height + 0.025*width, 0.0);
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

// Function to draw a temporary polyline so the user can see how it will look
void drawTempLine(void)
{
    glColor3f(colors[0], colors[1], colors[2]);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < PolyLinePoints.size(); i += 3) {
        glVertex3f(PolyLinePoints[i], PolyLinePoints[i+1], PolyLinePoints[i+2]);
    }
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
    
    drawPoints();
    drawLines();
    drawRectangles();
    drawCircles();
    
    //if the e key has been pressed, then push the current list of points
    if(ePressed) {
        polylines.push_back(PolyLinePoints);
        PolyLinePoints = clearedArray; //reset the current list of points
        pointCount = 0;
        ePressed = false; //e is no longer pressed
    }
    
    //draw polylines goes after the ePressed function because it needs to be displayed
    //right when the polyline is pushed into the vector instead of waiting for drawScene
    //to be called again
    drawPolyLines();
    
    if ( ((primitive == LINE) || (primitive == RECTANGLE) || (primitive == CIRCLE) || (primitive == POLYLINE)) &&
        (pointCount == 1) ) {
        drawTempPoint(); //draw temp points when the user is making a shape that requires
                         //multiple clicks
        drawTempLine();  //draw a temp line when the using is making a polyline
    }
    
    drawPointSelectionBox();
    drawLineSelectionBox();
    drawRectangleSelectionBox();
    drawCircleSelectionBox();
    drawPolylineSelectionBox();
    drawInactiveArea();
    
    if (isGrid) drawGrid();
    
    glutSwapBuffers();
}

// Function to pick primitive if click is in left selection area.
void pickPrimitive(int y)
{
    if ( y < (1- NUMBERPRIMITIVES*0.1)*height ) primitive = INACTIVE;
    else if ( y < (1 - 4*0.1)*height ) primitive = POLYLINE;
    else if ( y < (1 - 3*0.1)*height ) primitive = CIRCLE;
    else if ( y < (1 - 2*0.1)*height ) primitive = RECTANGLE;
    else if ( y < (1 - 1*0.1)*height ) primitive = LINE;
    else primitive = POINT;
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
                    rectangles.push_back( Rectangle(tempX, tempY, x, y, isFill) );
                    pointCount = 0;
                }
            }
            else if (primitive == CIRCLE)
            {
                if (pointCount == 0)
                {
                    tempX = x; tempY = y;
                    pointCount++;
                }
                else
                {
                    circles.push_back( Circle(tempX, tempY, x, y, isFill));
                    pointCount = 0;
                }
            }
            else if (primitive == POLYLINE)
            {
                if (pointCount == 0)
                {
                    tempX = x; tempY = y;
                    //add the current points into the vector of polyline points
                    //pointCount will stay at 1 so the red temp points will always be drawn
                    PolyLinePoints.push_back(x); PolyLinePoints.push_back(y);
                    PolyLinePoints.push_back(0.0);
                    pointCount++;
                }
                else
                {
                    //continue to add all points into the vector of polyline points
                    //pointCount will be reset when e is pressed, and that is when
                    //a new polyline object will be created
                    //pointCount will stay at 1 so the red temp points will always be drawn
                    tempX = x; tempY = y;
                    PolyLinePoints.push_back(x); PolyLinePoints.push_back(y);
                    PolyLinePoints.push_back(0.0);
                }
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
        //the next two cases handle whether polyline objects should be drawn
        //and the button only works if polyline is the active primitive
        case 'e':
            if(primitive == POLYLINE) {
                ePressed = true;
                glutPostRedisplay();
            }
            break;
        case 'E':
            if(primitive == POLYLINE) {
                ePressed = true;
                glutPostRedisplay();
            }
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
    circles.clear();
    polylines.clear();
    primitive = INACTIVE;
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

//the fill callback function
void fillMenu(int id)
{
    if (id == 5) isFill = true;
    if (id == 6) isFill = false;
    glutPostRedisplay();
}

//the color callback function
void colorMenu(int id)
{
    //based on what menu option is selected, the current color scheme will be changed
    
    if (id == 7) { //RED
        colors[0] = 1.0;
        colors[1] = 0.0;
        colors[2] = 0.0;
    }
    if (id == 8) { //GREEN
        colors[0] = 0.0;
        colors[1] = 1.0;
        colors[2] = 0.0;
    }
    if (id == 9) { //BLUE
        colors[0] = 0.0;
        colors[1] = 0.0;
        colors[2] = 1.0;
    }
    if (id == 10) { //BLACK
        colors[0] = 0.0;
        colors[1] = 0.0;
        colors[2] = 0.0;
    }
    glutPostRedisplay();
}

// Function to create menu.
void makeMenu(void)
{
    int sub_menu;
    sub_menu = glutCreateMenu(grid_menu);
    glutAddMenuEntry("On", 3);
    glutAddMenuEntry("Off",4);
    
    int fill_menu = glutCreateMenu(fillMenu);
    glutAddMenuEntry("Filled", 5);
    glutAddMenuEntry("Not Filled", 6);
    
    int color_menu = glutCreateMenu(colorMenu);
    glutAddMenuEntry("Red", 7);
    glutAddMenuEntry("Green", 8);
    glutAddMenuEntry("Blue", 9);
    glutAddMenuEntry("Black", 10);
    
    glutCreateMenu(rightMenu);
    glutAddSubMenu("Grid", sub_menu);
    glutAddSubMenu("Fill", fill_menu);
    glutAddSubMenu("Colors", color_menu);
    glutAddMenuEntry("Clear",1);
    glutAddMenuEntry("Quit",2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Left click on a box on the left to select a primitive." << endl
    << "Then left click on the drawing area: once for point, twice for line or rectangle or circle, and as many times as you want for a polyline." << endl
    << "Right click for menu options, includes grid, color, and fill." <<  endl; 
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