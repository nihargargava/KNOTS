#include <math.h>
#include "geometry_functions.h"


void CROSS(float in1[3],float in2[3],float out[3])
{   float out2[3];
    out2[0]=in1[1]*in2[2]-in1[2]*in2[1];
    out2[1]=in1[2]*in2[0]-in1[0]*in2[2];
    out2[2]=in1[0]*in2[1]-in1[1]*in2[0];
    out[0]=out2[0];
    out[1]=out2[1];
    out[2]=out2[2];
}
void SUBTRACT(float in1[3],float in2[3],float out[3])
{
    out[0]=in1[0]-in2[0];
    out[1]=in1[1]-in2[1];
    out[2]=in1[2]-in2[2];
}
void ADD(float in1[3],float in2[3],float out[3])
{
    out[0]=in1[0]+in2[0];
    out[1]=in1[1]+in2[1];
    out[2]=in1[2]+in2[2];
}
float MOD(float in[3])
{
    return sqrt(in[0]*in[0] + in[1]*in[1] + in[2]*in[2]);
}
void NORMALIZE(float in[3],float out[3],float length)
{
    float mod=MOD(in);
    out[0]=in[0]/mod;
    out[1]=in[1]/mod;
    out[2]=in[2]/mod;
    out[0]*=length;
    out[1]*=length;
    out[2]*=length;
}
void COPY(float in[3],float out[3])
{
    out[0]=in[0];
    out[1]=in[1];
    out[2]=in[2];
}

