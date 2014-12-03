/***************************************************************
Code written by: Nihar Prakash Gargava, IIT Kanpur.
Undergraduate Student of Mathematics

This is my attempt at making a simulator of Mathematical Knots.
At this point of time, this is what the program can do (as of now):

1.) Take an input knot: The input methods are quite intuitive.
    Mouse motion is for x and y axes, scroll is for z axis (or use '1', '2').

2.) Find the crossings and mark them: Red is for positive weighted crossing,
    Black is for negative. Writhe is calculated as the total of them all.

3.) Find the DT code: The DT code is calculated according to the orientation
    Assigned while drawing the knot. The first crossing is taken as 1.

4.) Rotate the knot: Use 'z' to toggle rotation mode.

List of things needing an improvement:
1.) The OpenGL version used is OBSELETE! Rewrite the rendering commands
in a newer OpenGL as soon as the Knot handling part is stable.

2.) No GUI/HUD text on the screen. This needs to be added.

3.) Must add more comment on everything

4.) Spline interpolation must be implemented
*******************************************************************/
#include<GL/gl.h>
#include<GL/glu.h>
#include<stdio.h>
#include<GL/freeglut.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif
#include <iostream>
#include <stdlib.h>
#include <knot.h>
#include <math.h>

int lastState;
int ctrl_flag;
int shift_flag;
int crossing_marker_flag;
int crossing_number_flag;

float rot_100;
float rot_010;

/* GLUT callback Handlers */

knot test,pointer;
knot crossing_marker;

void initGL() {
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}


void text(char str[],double x,double y,double z=0,int choice=1)
{
    glColor3d(0.0,0.0,0.0);
    glRasterPos3d(x,y,z);
    if(choice)glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*)str);
    else glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_10,(const unsigned char*)str);
}



static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
  //  glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
  //  glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
//    gluPerspective(45.0f, ar, 0.1f, 100.0f);
	glOrtho(-5.0,5.0,-5.0,5.0,-100.0,100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glColor3d(0.5,0.5,0.5);
    glPushMatrix();
    glTranslatef(0,0,-10);
    glRotatef(rot_100,1,0,0);
    glRotatef(rot_010,0,1,0);
    char STRING[10];
    test.draw();
    if(crossing_marker_flag)
    {
        for(int i=0;i<test.crossing_count;i++)
        {
            glPushMatrix();
            glTranslatef(test.crossing[i].location[0],test.crossing[i].location[1],0);
            if(test.crossing[i].weight==1)
            {
                crossing_marker.color[0]=1.0;
                if(crossing_marker_flag==2)text("+1",0,0,10,0);
            }
            else
            {
                crossing_marker.color[0]=0.0;
                if(crossing_marker_flag==2)text("-1",0,0,10,0);
            }
            if(i<10)
            {
                STRING[0]=i+'0';
                STRING[1]='\0';
            }
            else if(i<100)
            {  // printf("%d",i);
                STRING[0]=i/10;
                STRING[1]=i%10;
                STRING[2]='\0';
            }
            else
            {
                STRING[0]=i/100;
                STRING[1]=(i%100)/10;
                STRING[2]=i%10;
                STRING[3]='\0';
            }
            text(STRING,0.2,0.2,10.0);

            if(crossing_marker_flag==1)crossing_marker.draw();
            glPopMatrix();
        }
    }
    if(test.isBeingEdited)
    {   pointer.draw();
        text("     DRAW" ,pointer.vertex[0][0],pointer.vertex[0][1],pointer.vertex[0][2]);
    }
    glTranslatef(0,0,10);
    glPopMatrix();
    text("Created by: Nihar Gargava",2.5,-4.8,10);
    glutSwapBuffers();

}


static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'q':
        exit(0);
        break;

    case '+':test.tessellation++;
        break;

    case '-':
        if (test.tessellation>4)
        {
            test.tessellation--;
        }
        break;

    case ' ':
        test.popPoint();
        test.popPoint();
        test.pushPoint(pointer.vertex[0]);
        break;
    case 13:
        test.isBeingEdited=0;
        test.isClosed=1;
        test.findCrossings();
        test.findWrithe();
        test.findDTandBridges();
        for(int h=3;h<test.crossing_count+2;h+=2)
        {
            test.findTricolorability(h);
            cout<<"\n"<<h<<"-colorability :";
            if(test.tricolorability)
            {
                cout<<"Yes";
            }
            else
            {
                cout<<"No";
            }
        }
        cout<<"\nWrithe: "<<test.writhe<<"\nDT Code: ";
        for(int i=0;i<test.crossing_count;i++)
        {
            cout<<"  "<<test.DTcode[i];
        }
        cout<<"\n\nArcs =("<<test.crossing_count<<") : ";
        for(int i=0;i<test.crossing_count;i++)
        {
            cout<<"\n> Crossing "<<i<<" has u1,2,o as "<<test.crossing[i].under1<<","<<test.crossing[i].under2<<" and "<<test.crossing[i].over<<" respectively";
        }


        break;
    case 'z':
        ctrl_flag=1-ctrl_flag;
        break;

    case '1':
        pointer.vertex[0][2]+=0.2;
        test.popPoint();
        test.pushPoint(pointer.vertex[0]);
        break;
    case '2':
        pointer.vertex[0][2]-=0.2;
        test.popPoint();
        test.pushPoint(pointer.vertex[0]);
        break;
    case '0':
        crossing_marker_flag++;
        crossing_marker_flag%=3;
        break;
    }


    glutPostRedisplay();

}

static void idle(void)
{
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    switch(button)
    {   case GLUT_LEFT_BUTTON:  if(state==GLUT_UP && lastState==GLUT_DOWN)
                                 {
                                     test.pushPoint(pointer.vertex[0]);
                                    test.popPoint();
                                    test.pushPoint(pointer.vertex[0]);

                                 }
                                break;
    }
    lastState=state;

//    glutPostRedisplay();
}

void motionMouse(int x,int y)
{
    pointer.vertex[0][0]=x/64.0 -5.0;
    pointer.vertex[0][1]=5.0 -y/64.0;

    test.popPoint();
    test.pushPoint(pointer.vertex[0]);

    if(ctrl_flag)
    {
        rot_010=pointer.vertex[0][0]*-20;
        rot_100=pointer.vertex[0][1]*-20;
    }

    test.findCrossings();
    test.findWrithe();
}
void mousewheel(int button,int state, int x, int y)
{
    switch(state)
    {
        case 1: pointer.vertex[0][2]+=0.1;
                break;
        case -1:pointer.vertex[0][2]-=0.1;
                break;
    }
    test.popPoint();
    test.pushPoint(pointer.vertex[0]);
}


int main(int argc, char *argv[])
{
    pointer.pushPoint(0,0,0);
    test.pushPoint(pointer.vertex[0]);
    crossing_marker_flag=2;
    crossing_marker.pushPoint(0,0,-100);
    crossing_marker.pushPoint(0,0,+100);
    crossing_marker.isLines=0;
    crossing_marker.isBeingEdited=0;
    crossing_marker.isClosed=0;
    //crossing_marker.tessellation=40;
    crossing_marker.color[0]=1.0;
    crossing_marker.color[1]=0.0;
    crossing_marker.color[2]=0.0;
    crossing_marker.radius=0.070;
  //  test.radius=0.3;
    test.isClosed=0;
    test.isBeingEdited=1;
   // test.isLines=1;
    ctrl_flag=0;
    shift_flag=0;
    rot_010=0;
    rot_100=0;

    glutInit(&argc, argv);
    glutInitWindowSize(640,640);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("KNOTS");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);
    initGL();
 	glutMouseFunc(mouse);
 	glutPassiveMotionFunc(motionMouse);
	glutMouseWheelFunc(mousewheel);

    glutMainLoop();

    return EXIT_SUCCESS;
}
