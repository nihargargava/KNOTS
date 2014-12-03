#include <set>
#include <vector>
using namespace std;


#ifndef KNOT_H
#define KNOT_H




struct line_segment
{
    int index;
   int s;  //index of left endpoint
   int d;  //index of right endpoint


};


struct edge_pair
{
    int a;
    int b;
    float location[2];
    int weight;
    int over;
    int under1;
    int under2;
};





struct bridge_struct
{
    int s;
    int d;
    int label;
};



class knot
{
public:

    float vertex[500][3];
    line_segment edge[500];
    edge_pair crossing[500];
    bridge_struct bridge[500];
    int DTcode[500];
    int crossing_count;
    int vertex_count;
    int tricolorability;
    float radius;
    int tessellation;
    float color[3];
    int isClosed;
    int isBeingEdited;
    int isLines;
    int writhe;
    int isHighlighted;
    int DrawType; //0 for polynomial, 1 for cubic splines

    float light_ambient[4];
    float light_diffuse[4];
    float light_specular[4];
    float light_position[4];
    float mat_ambient[4];
    float mat_diffuse[4];
    float mat_specular[4];
    float high_shininess[1];

    knot();
    void drawEdge(int edge,float r,float g, float b);
    void draw();
    void pushPoint(float x,float y,float z);
    void pushPoint(float a[3]);
    void popPoint();
    void findCrossings();
    void findWrithe();
    void findDTandBridges();
    void findTricolorability(int);

};


#endif // KNOT_H
