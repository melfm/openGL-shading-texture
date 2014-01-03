//
//  model.h
//  ShadingTexture
//
//  Created by 100012340 on 06/11/2013.
//
//

#ifndef __ShadingTexture__model__
#define __ShadingTexture__model__

#include <iostream>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "objectModel.hpp"


#endif /* defined(__ShadingTexture__model__) */

#define PI 3.1416

 
typedef struct Model
{
    Modelobject* object;
    GLuint texture[1];
    
}Model;


// Vertices
float x=0.0, y=0.0, z=0.0, rotation=0.0;
int fps=0, displayList=0;



/*  Picking */
#define BUFSIZE 512
#define RENDER	
GLuint selectBuf[BUFSIZE];
GLint pickedObjID;
int mode = GL_RENDER;

// Texture Flag
int textured= 0;

int cursorX, cursorY;

// actual vector representing the camera's direction
float lx=0.0 ,lz=-1.0;


/*  Lighting Settings */
float lxpos, lypos, lzpos, lwpos;

float ratio;
int height, weight;

float posx, posy, posz;

int lightSetting1;
int lightSetting2;
int lightSetting3;
int lightSetting4;
int lightSetting5;
int animateLight;

float lightAngle;

void LoadModels();
int LoadGLTextures();
/* Drawing function  */
GLvoid drawModel(Modelobject* model, GLuint mode);
void processSpecialKeys(int key, int xx, int yy);
void processNormalKeys(unsigned char key, int x, int y);
void performDrawing(GLenum mode);
void processHits (GLint hits, GLuint buffer[]);
void help();


