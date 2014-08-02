#include "knot.h"
#include <math.h>
#include "geometry_functions.h"
#include <iostream>
#include <set>
#include <queue>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


//Poor man's definition of PI
#define PI 3.14159265359



using namespace std;

float abs_f(float a)
{
    if(a<0)return -a;
    else return a;
}

class set_compare
{   float* sweep_status;
    knot* being_used;
    public:
    set_compare(float* a, knot* b)
    {
        sweep_status=a;
        being_used=b;
    }
    bool operator()(const line_segment& lhs, const line_segment& rhs) const
    {  // cout<<"\nComparing "<<lhs.index<<" & "<<rhs.index;
        float rhs_abcissa=((((being_used->vertex[rhs.s][1]-being_used->vertex[rhs.d][1])/(being_used->vertex[rhs.s][0]-being_used->vertex[rhs.d][0]))*(*sweep_status-being_used->vertex[rhs.d][0]) + being_used->vertex[rhs.d][1]));
        float lhs_abcissa=(((being_used->vertex[lhs.s][1]-being_used->vertex[lhs.d][1])/(being_used->vertex[lhs.s][0]-being_used->vertex[lhs.d][0]))*(*sweep_status-being_used->vertex[lhs.d][0]) + being_used->vertex[lhs.d][1]);
        if(abs_f(lhs_abcissa - rhs_abcissa)<=0.00001)
        {  // cout<<"\nSame abcissa!";
            if(abs_f(being_used->vertex[lhs.s][0]-being_used->vertex[lhs.d][0])<0.00001)
            {
              //  cout<<"\nReturning true because lhs is vertical!";
                return true;
            }
            if(abs_f(being_used->vertex[rhs.s][0]-being_used->vertex[rhs.d][0])<0.00001)
            {
              //  cout<<"\nReturning false because rhs is vertical!";
                return false;
            }
            float slope_diff=abs_f(((being_used->vertex[lhs.s][1]-being_used->vertex[lhs.d][1])/(being_used->vertex[lhs.s][0]-being_used->vertex[lhs.d][0])) - ((being_used->vertex[rhs.s][1]-being_used->vertex[rhs.d][1])/(being_used->vertex[rhs.s][0]-being_used->vertex[rhs.d][0])));
            if (slope_diff < 0.00001)
            { //  cout<<"\nSame slope!:";
               // cout<<abs_f(((being_used->vertex[lhs.s][1]-being_used->vertex[lhs.d][1])/(being_used->vertex[lhs.s][0]-being_used->vertex[lhs.d][0])) - ((being_used->vertex[rhs.s][1]-being_used->vertex[rhs.d][1])/(being_used->vertex[rhs.s][0]-being_used->vertex[rhs.d][0])));
             //   cout<<"\nReturning false!";
                return false;
            }
           // cout<<"\nReturning lhs_slope>rhs_slope!";
            return (((being_used->vertex[lhs.s][1]-being_used->vertex[lhs.d][1])/(being_used->vertex[lhs.s][0]-being_used->vertex[lhs.d][0])) > ((being_used->vertex[rhs.s][1]-being_used->vertex[rhs.d][1])/(being_used->vertex[rhs.s][0]-being_used->vertex[rhs.d][0])));
        }
      //  cout<<"\nReturning lhs_abcissa>rhs_abcissa! (" <<lhs_abcissa<<" > "<<rhs_abcissa<<")";
      //  cout<<" which is "<<(lhs_abcissa > rhs_abcissa);
        return (lhs_abcissa > rhs_abcissa);
    }

};

class crossingCompare
{   knot* K;
    public:
    crossingCompare(knot* k)
    {
        K=k;
    }
    bool operator()(const edge_pair& lhs, const edge_pair& rhs) const
    { //  cout<<"\nComparing :"<<"("<<lhs.a<<","<<lhs.b<<")"<<" & "<<"("<<rhs.a<<","<<rhs.b<<")";
       // cout<<"\nReturning :"<<!((lhs.a+lhs.b == rhs.a +rhs.b) && (lhs.a*lhs.b == rhs.a*rhs.b));
        return !((lhs.a+lhs.b == rhs.a +rhs.b) && (lhs.a*lhs.b == rhs.a*rhs.b));
    }
};

struct point_for_BO
{
    float x;
    float y;
    int type;
    int edge_no;
    int edge_no2;
};


void pointlessArrayCreatingFunction(float input[4],float a,float b, float c, float d) //I feel shameful to have created this. Perhaps there was no other way. Life is unfair.
{
    input[0]=a;
    input[1]=b;
    input[2]=c;
    input[3]=d;
}



knot::knot()
{

    vertex_count=0;
    radius=0.2;
    tessellation=50;
    color[0]=0.6;
    color[1]=0.6;
    color[2]=0.8;
    isClosed=0;
    isBeingEdited=1;
    isLines=0;
    isHighlighted=0;
    for(int i=0;i<500;i++)
    {
        edge[i].index=i;
    }

    pointlessArrayCreatingFunction(light_ambient , 0.40f, 0.40f, 0.40f, 0.0f );
    pointlessArrayCreatingFunction( light_diffuse , 1.0f, 1.0f, 1.0f, 0.0f );
    pointlessArrayCreatingFunction(light_specular , 1.0f, 1.0f, 1.0f, 1.0f);
    pointlessArrayCreatingFunction(light_position , 2.0f, 5.0f, 5.0f, 0.0f);
    pointlessArrayCreatingFunction(mat_ambient  ,0.7f, 0.7f, 0.7f, 1.0f );
    pointlessArrayCreatingFunction(mat_diffuse  ,0.8f, 0.8f, 0.8f, 1.0f );
    pointlessArrayCreatingFunction(mat_specular , 1.0f, 1.0f, 1.0f, 1.0f);
    high_shininess[0]=100.0f ;



}
void knot::pushPoint(float x,float y, float z)
{   if(isBeingEdited)
    {
        vertex[vertex_count][0]=x;
        vertex[vertex_count][1]=y;
        vertex[vertex_count][2]=z;
        if(vertex_count!=0)
        {
            edge[vertex_count].d=vertex_count;
            edge[vertex_count].s=vertex_count-1;
        }
        edge[0].d=0;
        edge[0].s=vertex_count;

        vertex_count++;
    }
}

void knot::pushPoint(float a[3])
{
    pushPoint(a[0],a[1],a[2]);
}

void knot::popPoint()
{   if(vertex_count>0 && isBeingEdited)
        vertex_count--;
}

void knot::drawEdge(int i,float r=-1,float g=-1, float b=-1)
{   float v1[3],v2[3],v3[3],v4[3],n[3];
    float tempx[3],tempy[3];
    float x[3],y[3],z[3],s,c;
    y[0]=1;
    y[1]=0;
    y[2]=0;
    if(r!=-1)
    {
        glColor3f(r,g,b);
    }
    SUBTRACT(vertex[edge[i].s],vertex[edge[i].d],z);
    CROSS(z,y,x);
    CROSS(z,x,y);
    NORMALIZE(x,x);
    NORMALIZE(y,y);
    s=0;c=1;
    for(int j=1;j<=tessellation;j++)
    {   NORMALIZE(x,tempx,c*radius);
        NORMALIZE(y,tempy,s*radius);
        ADD(vertex[i],tempx,v1);
        ADD(v1,tempy,v1);
        ADD(v1,z,v2);

        s=sin((2*PI*j)/((float)tessellation));
        c=cos((2*PI*j)/((float)tessellation));

        NORMALIZE(x,tempx,c*radius);
        NORMALIZE(y,tempy,s*radius);
        ADD(vertex[i],tempx,v4);
        ADD(v4,tempy,v4);
        ADD(v4,z,v3);
        SUBTRACT(v3,v2,n);
        CROSS(z,n,n);
        NORMALIZE(n,n,-1);


        glNormal3fv(n);

        glVertex3fv(v1);
        glVertex3fv(v2);
        glVertex3fv(v3);
        glVertex3fv(v4);
     //   glNormal3fv(n);
    }

}


void knot::draw()
{

 //   float radius_t;
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glColor3fv(color);
    for(int i=0;i<vertex_count;i++)
    {
        glPushMatrix();
        glTranslated(vertex[i][0],vertex[i][1],vertex[i][2]);
        if(isLines);
        else glutSolidSphere(radius,tessellation,tessellation);
        glPopMatrix();
    }
    if(isLines)glBegin(GL_LINES);
    else glBegin(GL_QUADS);
    for(int i=1;i<vertex_count;i++)
    {
        drawEdge(i);
    }
    if(isClosed)
    {
        drawEdge(0);
    }
   /* for(int i=0;i<crossing_count;i++)
    {   radius_t=radius;
        radius*=1.1;
        drawEdge(crossing[i].a.index,0.0,1.0,0.0);
        drawEdge(crossing[i].b.index,0.0,1.0,1.0);
        radius=radius_t;
    }*/
    glEnd();



    glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);


}

 class queue_compare
{
    public:

    bool operator()(const point_for_BO& lhs, const point_for_BO&rhs) const
    {
        if(abs_f(lhs.x-rhs.x)<0.00001)
        {
            if(abs_f(lhs.y-rhs.y)<0.00001) return (lhs.type<rhs.type);
            else return (lhs.y>rhs.y);
        }
        else return (lhs.x>rhs.x);
    }
};



point_for_BO findIntersection(line_segment a, line_segment b,knot* K)
{  // cout<<"\nChecking for intersection between "<<a.index<<" & "<<b.index;
    point_for_BO out;
    float d11=K->vertex[a.s][0]-K->vertex[a.d][0];
    float d12=K->vertex[b.d][0]-K->vertex[b.s][0];
    float d21=K->vertex[a.s][1]-K->vertex[a.d][1];
    float d22=K->vertex[b.d][1]-K->vertex[b.s][1];

    float x1=K->vertex[b.d][0] - K->vertex[a.d][0];
    float x2=K->vertex[b.d][1] - K->vertex[a.d][1];

    float p,q;

    float det=d11*d22-d12*d21;
    if(det==0)
    {
        out.type=1;
        return out;
    }
    else
    {
        p= (x1*d22 -x2*d12)/det;
        q= (x2*d11 -x1*d21)/det;
        if(p<=0.001 || q<=0.001 || p>=0.999 || q>=0.999)
        {
            out.type=1;
            return out;
        }
        else
        {
            out.x=p*K->vertex[a.s][0] + (1-p)*K->vertex[a.d][0];
            out.y=p*K->vertex[a.s][1] + (1-p)*K->vertex[a.d][1];
            out.type=0;
        //    cout<<"\nIntersection reported!";
            return out;
        }

    }
}

void knot::findCrossings()
{ //  cout<<"Commencing crossing detection";
    priority_queue<point_for_BO,vector<point_for_BO>,queue_compare> E;

  //  priority_queue<point_for_BO,vector<point_for_BO>,queue_compare> F;

    for(int i=0;i<vertex_count;i++)
    {  // cout<<"\nAdded vertex "<<i;
        point_for_BO temp;
        temp.edge_no=i;
        temp.x=vertex[edge[i].s][0];
        temp.y=vertex[edge[i].s][1];
        if(vertex[edge[i].s][0]<vertex[edge[i].d][0])
            temp.type=-1;
        else temp.type=1;
        E.push(temp);
        temp.edge_no=i;
        temp.x=vertex[edge[i].d][0];
        temp.y=vertex[edge[i].d][1];
        temp.type*=-1;;
        E.push(temp);

    }

    float *sweep_status;
    sweep_status=new float;
    *sweep_status=E.top().x;
    set<edge_pair,crossingCompare> crossing_set (crossingCompare(this));
    set<line_segment,set_compare> S (set_compare(sweep_status,this));
    set<line_segment,set_compare>::iterator S_it;
    set<line_segment,set_compare>::iterator S_it2;
    edge_pair obtained;
    while(!E.empty())
    {
        point_for_BO temp=E.top();
        *sweep_status=temp.x;
   //     cout<<"\nSweep Status ="<<*sweep_status;
       switch(temp.type)
        {
            case -1://cout<<"\nLeft endpoint detected for "<<temp.edge_no;

                    S_it=S.insert(edge[temp.edge_no]).first;

                    if(S_it!=S.begin())
                    {
                        S_it--;
                        point_for_BO newCross=findIntersection(*S_it,edge[temp.edge_no],this);
                        if(newCross.type==0 && newCross.x>*sweep_status)
                        {
                            newCross.edge_no2=temp.edge_no;
                            newCross.edge_no=S_it->index;
                            E.push(newCross);
                        }
                        S_it++;
                    }
                    if(++S_it!=S.end())
                    {
                        point_for_BO newCross=findIntersection(*S_it,edge[temp.edge_no],this);
                        if(newCross.type==0 && newCross.x>*sweep_status)
                        {
                            newCross.edge_no=temp.edge_no;
                            newCross.edge_no2=S_it->index;
                            E.push(newCross);
                        }
                        S_it--;
                    }

                    break;

            case 1: //cout<<"\nRight endpoint detected for "<<temp.edge_no;
                    *sweep_status-=0.001;
                   // cout<<"\nErasing!! an element";
                  //  cout<<"\n\t\t\t\tErased ";
                  S.erase(edge[temp.edge_no]);
                   // cout<<"\nErased an element";
                    break;

            case 0:
                    //cout<<"\n!!!!    !!!!!Crossing detected of edge "<<temp.edge_no<<" and "<<temp.edge_no2;
                    if((vertex[edge[temp.edge_no].s][3]+(((vertex[edge[temp.edge_no].d][2]-vertex[edge[temp.edge_no].s][2])/(vertex[edge[temp.edge_no].d][1]-vertex[edge[temp.edge_no].s][1]))*(temp.y-vertex[edge[temp.edge_no].s][1]))) > (vertex[edge[temp.edge_no2].s][3]+(((vertex[edge[temp.edge_no2].d][2]-vertex[edge[temp.edge_no2].s][2])/(vertex[edge[temp.edge_no2].d][1]-vertex[edge[temp.edge_no2].s][1]))*(temp.y-vertex[edge[temp.edge_no2].s][1]))) )
                    {
                        obtained.a=temp.edge_no;
                        obtained.b=temp.edge_no2;
                    }
                    else
                    {
                        obtained.a=temp.edge_no2;
                        obtained.b=temp.edge_no;
                    }
                    obtained.location[0]=temp.x;
                    obtained.location[1]=temp.y;
                    crossing_set.insert(obtained).second;
                    *sweep_status-=0.0001;
                    S.erase(edge[temp.edge_no]);
                    S.erase(edge[temp.edge_no2]);
                    *sweep_status+=0.0002;
                    S_it2=S.insert(edge[temp.edge_no]).first;
                    S_it=S.insert(edge[temp.edge_no2]).first;
                    *sweep_status-=0.0001;
                    if(S_it!=S.begin())
                    {
                        S_it--;
                        point_for_BO newCross=findIntersection(*S_it,edge[temp.edge_no2],this);
                        if(newCross.type==0 && newCross.x>*sweep_status)
                        {
                            newCross.edge_no2=temp.edge_no2;
                            newCross.edge_no=S_it->index;
                            E.push(newCross);
                        }
                        S_it++;
                    }
                    if(++S_it2!=S.end())
                    {
                        point_for_BO newCross=findIntersection(*S_it2,edge[temp.edge_no],this);
                        if(newCross.type==0 && newCross.x>*sweep_status)
                        {
                            newCross.edge_no=temp.edge_no;
                            newCross.edge_no2=S_it2->index;
                            E.push(newCross);
                        }
                        S_it2--;
                    }

                    break;

        }

     /*  cout<<"\n___________________\nSweep Line Set:";
       for(S_it=S.begin();S_it!=S.end();S_it++)
       {
           cout<<" "<<S_it->index;
       }

       cout<<"\n___________________\nCrossing Set:";
       for(set<edge_pair,crossingCompare>::iterator i=crossing_set.begin();i!=crossing_set.end();i++)
       {
           cout<<" | "<<i->location[0]<<" , "<<i->location[1]<<" | ";
       }
       cout<<"\nEvent Queue:";
       while(!E.empty())
       {
           temp=E.top();
           cout<<" "<<temp.x<<"_"<<temp.type;
           E.pop();
           F.push(temp);
       }
       while(!F.empty())
       {
           E.push(F.top());
           F.pop();
       }

        cout<<"\n\n";
*/

        E.pop();
    }
    int j=0;

    for(set<edge_pair,crossingCompare>::iterator i=crossing_set.begin();i!=crossing_set.end();i++)
    {
        crossing[j].a=i->a;

        crossing[j].b=i->b;

        crossing[j].location[0]=i->location[0];
        crossing[j].location[1]=i->location[1];
        j++;
    }
    crossing_count=j;

}

void knot::findWrithe()
{   writhe=0;
    for(int i=0;i<crossing_count;i++)
    {   crossing[i].weight=23;
        float ax=vertex[edge[crossing[i].a].d][0]-vertex[edge[crossing[i].a].s][0];
        float ay=vertex[edge[crossing[i].a].d][1]-vertex[edge[crossing[i].a].s][1];
        float bx=vertex[edge[crossing[i].b].d][0]-vertex[edge[crossing[i].b].s][0];
        float by=vertex[edge[crossing[i].b].d][1]-vertex[edge[crossing[i].b].s][1];
        if((ax*by-ay*bx) > 0)
            crossing[i].weight=+1;
        if ((ax*by-ay*bx) < 0)
            crossing[i].weight=-1;
     //   cout<<ax<<" "<<ay<<" "<<bx<<" "<<by<<" "<<"\t";
    //    cout<<"\n"<<crossing[i].weight;
        writhe+=crossing[i].weight;
    }
}
class set_compare2
{   knot* K;
    public:
    set_compare2(knot* k)
    {
        K=k;
    }
    bool operator()(const pair<int,int>& lhs, const pair<int,int>& rhs) const
    {
        if(lhs.first==rhs.first)
        {
           if(abs_f(K->vertex[K->edge[lhs.first].s][0]-K->vertex[K->edge[lhs.first].d][0])<0.00001)
           {
               if(K->vertex[K->edge[lhs.first].s][1]>K->vertex[K->edge[lhs.first].d][1])
                    return (K->crossing[lhs.second].location[1] > K->crossing[rhs.second].location[1]);
               else
                    return (K->crossing[lhs.second].location[1] < K->crossing[rhs.second].location[1]);
           }
           else
           {
               if(K->vertex[K->edge[lhs.first].s][0]>K->vertex[K->edge[lhs.first].d][0])
                    return (K->crossing[lhs.second].location[0] > K->crossing[rhs.second].location[0]);
               else
                    return (K->crossing[lhs.second].location[0] < K->crossing[rhs.second].location[0]);
           }
        }
        return (lhs.first<rhs.first);
    }
};

void knot::findDTandBridges()
{   set<pair<int,int>,set_compare2> S (set_compare2(this));
    set_compare2 optor(this);
    pair<int,int> temp;
    pair<int,int>temp2;

    for(int i=0;i<crossing_count;i++)
    {
        temp.second=i;
        temp.first=crossing[i].a;
        S.insert(temp).second;
        temp.first=crossing[i].b;
        S.insert(temp).second;
    }
    vector< pair<int,int> > V;
    for(set<pair<int,int>,set_compare2>::iterator i=S.begin();i!=S.end();i++)
    {
        temp.second=i->second;
   //     temp2.second=i->second;
        temp.first=i->first;
   //     temp2.first=(crossing[temp2.second].a==temp.first)?  crossing[temp2.second].b  :  crossing[temp2.second].a;
        V.push_back(temp);
  //      index++;
    }
    /*
    if(V.size()!=2*crossing_count)
    {
        cout<<"\n!!!! I FEAR OBLIVION!";
    }*/
    for(int q=0;q<2*crossing_count;q++)
    {
        cout<<q+1<<".)"<<V[q].first<<" "<<V[q].second<<" | "<<crossing[V[q].second].a<<" "<<crossing[V[q].second].b<<endl;
    }
    int index=0;
    int DTindex=0;
    for(set<pair<int,int>,set_compare2>::iterator i=S.begin();i!=S.end();i++)
    {
        temp.second=i->second;
        temp2.second=i->second;
        temp.first=i->first;
        temp2.first=(crossing[temp2.second].a==temp.first)?  crossing[temp2.second].b  :  crossing[temp2.second].a;
        {   int out=-1;
            int l=0;
            int r=2*crossing_count;
            int mid=(l+r)/2;
            if(V[l]==temp2)
            {
                out=l;
            }
            if(V[r]==temp2)
                out=r;
            while(out==-1)
            {
                if(V[mid]==temp2)
                {
                    out=mid;
                }
                else if (optor.operator()(V[mid],temp2))
                {
                    l=mid;
                    mid=(l+r)/2;
                }
                else if(optor.operator()(temp2,V[mid]))
                {
                    r=mid;
                    mid=(l+r)/2;
                }
            }
          //  cout<<"\nPairing "<<index+1<<" with "<<out+1;
            DTcode[DTindex]=out+1;
            if(crossing[V[out].second].a==V[out].first)DTcode[DTindex]*=-1; //Is BUGGY
        }

        i++;
        index++;
        index++;
        DTindex++;
    }/*
    bridge_count=0;
    for(set<pair<int,int>,set_compare2>::iterator i=S.begin();i!=S.end();i++)
    {
        if(crossing[i->second].b==i->first)
            bridge[bridge_count].d=i->second;
        if(++i!=S.end())
        {
            --i;
            bridge_count++;
            bridge[bridge_count].s=i->second;
        }
        else
        {
            --i;
            bridge[0].s=i->second;
        }
    }
    */

}


