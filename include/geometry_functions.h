#ifndef GEOMETRY_FUNCTIONS_H_INCLUDED
#define GEOMETRY_FUNCTIONS_H_INCLUDED


void CROSS(float in1[3],float in2[3],float out[3]);
void ADD(float in1[3],float in2[3],float out[3]);
void SUBTRACT(float in1[3],float in2[3],float out[3]);
float MOD(float in[3]);
void NORMALIZE(float in[3],float out[3],float length=1.0);
void COPY(float in[3],float out[3]);



#endif // GEOMETRY_FUNCTIONS_H_INCLUDED
