//
//  model.cpp
//  ShadingTexture
//
//  Created by 100012340 on 06/11/2013.
//
//

#include "model.h"
#include "SOIL.h"
#include <math.h>


using namespace std;

Model ground;
Model wallRightCorner;
Model wallLeftCorner;
Model angel;
Model cube;
Model sphere;
Model cone;
Model cylinder;
Model monkey;
Model apple;

objectModel objModel;

// Default, same as camera eye coordinates
GLfloat lightPos1[4] = {20.0, 15.0,	15.0, 0.0};
GLfloat lightPos2[4] = {30.0, 30.0 ,10.0, 0.0};
GLfloat lightPos3[4] = {8.0, -20.0 ,8.0, 0.0};
GLfloat lightPos4[4] = {0.0, 0.0 ,0.0, 0.0};

GLfloat xeye, yeye, zeye;

GLfloat animateLightPos[4] = {xeye, yeye ,zeye, 0.0};


GLfloat lightAmb[4] = {2.0f, 2.0f, 2.0f, 2.0f };
GLfloat lightAmb2[4] = {0.05f, 0.05f, 0.05f, 0.05f };


GLfloat lightDiff[4] = {1.0, 1.0, 1.0, 1.0};
GLfloat lightDiff2[4] = {5.0, 5.0, 5.0, 5.0};

static int spin = 0;

/*
 * Using Blender to generate data
 * Tricks used to get all the data needed
 * http://www.idevgames.com/forums/thread-5344.html
 
 */
void LoadModels()
{
    
    ground.object = (Modelobject*)malloc(sizeof(Modelobject));
    wallRightCorner.object = (Modelobject*)malloc(sizeof(Modelobject));
    wallLeftCorner.object = (Modelobject*)malloc(sizeof(Modelobject));
    angel.object = (Modelobject*)malloc(sizeof(Modelobject));
    
	angel.object = objModel.readOBJ((char*)"Models/angel.obj");
    
    ground.object  = objModel.readOBJ((char*)"Models/ground.obj");
    wallRightCorner.object = objModel.readOBJ((char*)"Models/wall1.obj");
    wallLeftCorner.object = objModel.readOBJ((char*)"Models/darkWallTest.obj");
    
    cube.object = objModel.readOBJ((char*)"Models/cube.obj");
    
    sphere.object = objModel.readOBJ((char*)"Models/sphere.obj");
    
    cone.object = objModel.readOBJ((char*)"Models/cone.obj");
    cylinder.object = objModel.readOBJ((char*)"Models/cylinder.obj");
    
    monkey.object = objModel.readOBJ((char*)"Models/monkey.obj");
    
    apple.object = objModel.readOBJ((char*)"Models/apple.obj");
    
}

void init()
{
    
    if (!LoadGLTextures())
	{
        cout << " Failed to load texture " << endl;
	}
    LoadModels();
    
    glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
    
	glShadeModel(GL_SMOOTH); // Enable Smooth Shading
    // Start with this viewing coordinates
    y = 6;
    z = 2;
    // Initial light setting
    posx = 20.0;
    posy = 15.0;
    posz = 15.0;
    
    performDrawing(GL_RENDER);
    
    help();
    
}

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    // eye(x,y,z), focal(x,y,z), up(x,y,z)
    gluLookAt(posx, posy,	posz,		// Camera position
			  0.0,	5.0,	0.0,
		      0.0f,	1.0f,	0.0f);
    
	glTranslatef(x,y,z);
    glColor3f(1.0, 1.0, 1.0);
    
	glRotatef(rotation, 1.0, 0.0, 0.0);
    
    if(lightSetting1)
    {
        // Turn lights on
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos1);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
    }
    
    if(lightSetting2)
    {
        
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos2);
        
        glEnable(GL_LIGHTING);
        
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb2);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff2);
        glEnable(GL_LIGHT0);
        
        glEnable(GL_COLOR_MATERIAL);
        
    }
    
    if(lightSetting3){
        
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos3);
        
        glEnable(GL_LIGHTING);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
        glEnable(GL_LIGHT0);
        glEnable(GL_COLOR_MATERIAL);
        
    }
    
    if(lightSetting4)
    {
        // Turn the lights off
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_COLOR_MATERIAL);
        glCallList(displayList);
    }
    
    /*
     Note the two pairs of glPushMatrix() and glPopMatrix() calls,
     which are used to isolate the viewing and modeling transformations
     all of which occur on the modelview stack
     http://www.glprogramming.com/red/chapter05.html
     */
    
    if(animateLight)
    {
        GLfloat light_position[] = { 20.0, 15.0, 15.0, 1.0 };
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glPushMatrix();
        glRotated(spin, 0.0, spin, 0.0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        
        glPopMatrix();
        glFlush();
    }
    
    glCallList(displayList);
    
    glutSwapBuffers();
}

void myReshape(int w, int h)
{
    weight = w;
    height =h;
    
    if(h == 0) h = 1;
    float ratio= 1.0* w / h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    glViewport(0, 0, w, h);
	gluPerspective(30, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}


void animateLightAround()
{
    lightAngle += 0.5f;
}

/* Switch between different light settings */
void switchLightSettings()
{
    static int counter = 1;
    
    switch(counter)
    {
        case 1:
            // Switch to setting one
            lightSetting1 = 1;
            lightSetting2 = 0;
            lightSetting3 = 0;
            lightSetting4 = 0;
            counter++;
            
            break;
            
        case 2:
            lightSetting1 = 0;
            lightSetting2 = 1;
            lightSetting3 = 0;
            lightSetting4 = 0;
            counter ++;
            break;
            
        case 3:
            lightSetting1 = 0;
            lightSetting2 = 0;
            lightSetting3 = 1;
            lightSetting4 = 0;
            counter ++;
            break;
            
        case 4:
            lightSetting1 = 0;
            lightSetting2 = 0;
            lightSetting3 = 0;
            lightSetting4 = 1;
            counter = 1;
            break;
    }
    
    
}
/* Switch between different view points by pressing 'v' */
void cycleThroughVP()
{
    static int counter = 1;
    
    switch(counter)
    {
            
        case 1:
            posx = 15.0;
            posy = 15.0;
            posz = 15.0;
            glutPostRedisplay();
            counter++;
            break;
            
            
        case 2:
            
            posx = 10.0;
            posy = 10.0;
            posz = 10.0;
            glutPostRedisplay();
            counter++;
            break;
            
        case 3:
            posx = 3.0;
            posy = 10.0;
            posz = 5.0;
            
            glutPostRedisplay();
            counter++;
            break;
            
        case 4:
            posx = 10.0;
            posy = 20.0;
            posz = 40.0;
            glutPostRedisplay();
            counter++;
            break;
            
        case 5:
            posx = 5.0;
            posy = 20.0;
            posz = 30.0;
            counter++;
            break;
            
        case 6:
            posx = 20.0;
            posy = 15.0;
            posz = 15.0;
            glutPostRedisplay();
            counter = 1;
            break;
            
            
    }
    
}
/*  Switch the texture - This is for demo purposes
    TODO - Add the rest of the objects in the scene
    
*/
void switchTexture(){
    static int counter = 1;
    
    switch(counter)
    {
            
        case 1:
            // The particualr object pickedObjID
            if(pickedObjID ==1){
                ground.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/stone1.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
            }
            if(pickedObjID ==9){
                angel.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/pinkchalk.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
            }
            
            if(pickedObjID ==5){
                cube.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/stone1.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
            }
            
            if(pickedObjID ==2){
                
                wallLeftCorner.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/shiny.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
                
            }
            if(pickedObjID ==4){
                sphere.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/glassNonSmooth.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
            }
            
            performDrawing(GL_RENDER);
            counter++;
            break;
            
            
        case 2:
            // The particualr object pickedObjID
            if(pickedObjID ==1){
                ground.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/darkMarble.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
            }
            if(pickedObjID ==9){
                angel.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/marbleTexture2.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
            }
            if(pickedObjID ==2){
                
                wallLeftCorner.texture[0] = SOIL_load_OGL_texture
                (
                 "Texture/bricks.jpg",
                 SOIL_LOAD_AUTO,
                 SOIL_CREATE_NEW_ID,
                 SOIL_FLAG_INVERT_Y
                 );
                
            }
            performDrawing(GL_RENDER);
            counter = 1;
            break;
            
            
    }
}
void processNormalKeys(unsigned char key, int x, int y)
{
    switch(key){
            
        case 'a' : x-=2; break; // Move along x left
        case 'd' : x+=2; break; // Move along x right
        case 's' : z+=2; break; // Move along z forwards
        case 'w' : z-=2; break; // Move along z backwards
        case 'k' : rotation-=2.0; break; // Rotate up
        case 'i' : rotation+=2.0; break; // Rotate down
        case 'l':
        case 'L':
            switchLightSettings();
            break;
            
        case 't':
        case 'T':
            // Texture on/off
            textured = !textured;
            // Redraw
            performDrawing(GL_RENDER);
            break;
            
        case 'v':
        case 'V':
            //cycle through view point locations
            animateLight = 0;
            // spin = 0.0;
            cycleThroughVP();
            break;
            
        case ' ':
            // Circulate animated light around the model
            animateLight = 1;
            spin = (spin + 30) % 360;
            glutPostRedisplay();
            break;
            
        case 'm':
        case 'M':
            // Cycle through different texture
            switchTexture();
            
            break;
        case 27  : exit(0);
            
    }
    glutPostRedisplay();
}

void processSpecialKeys(int key, int xx, int yy)
{
    switch (key) {
        case GLUT_KEY_LEFT :
            
            break;
        case GLUT_KEY_RIGHT :
            
            break;
        case GLUT_KEY_UP :
            y+=2; break; // Shift down
            
            break;
        case GLUT_KEY_DOWN :
            y-=2; break; // Shift up
            
            break;
    }
    glutPostRedisplay();
}

/*  Perform picking  */

void mouse(int button, int state, int x, int y)
{
    GLuint selectBuf[BUFSIZE];
    GLint hits;
    GLint viewport[4];
   // float ratio= 1.0* weight / height;
    
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_DOWN) {
                
                glSelectBuffer (BUFSIZE, selectBuf);
                (void) glRenderMode (GL_SELECT);
                glInitNames();
                glPushName(0);
                
                glGetIntegerv (GL_VIEWPORT, viewport);
                glMatrixMode(GL_PROJECTION);
                
                glPushMatrix ();
                glLoadIdentity();
                
                gluPickMatrix(x, y, 0.5, 0.5, viewport);
                
                gluPerspective(30, (float)viewport[2]/(float)viewport[3], 1, 1000);
                //gluPerspective(30, ratio, 1, 1000);
                // Draw the objects onto the screen
                glMatrixMode(GL_MODELVIEW);
                // Draw only names in the stack and fill the array
                performDrawing(GL_SELECT);
                glCallList(displayList);
                // We do pushMatrix in PROJECTION mode!
                glMatrixMode(GL_PROJECTION);
                
                glPopMatrix ();
                // glFlush ();
                
                hits = glRenderMode (GL_RENDER);
                processHits (hits, selectBuf);
                
                glMatrixMode(GL_MODELVIEW);
            }
            break;
        default:
            break;
    }
}

int main(int argc, char **argv)
{
    
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(1200,1000);
    glutCreateWindow("Shading & Texture");
    
    init();
    
    // register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(myReshape);
    glutKeyboardFunc(processNormalKeys);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(mouse);
    
    glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
    // Comment this out to enable lighting as default
   /* glLightfv(GL_LIGHT0, GL_POSITION, lightPos2);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);*/
    
    glutMainLoop();
    
    return 1;
}


void performDrawing(GLenum mode)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_POLYGON);
    displayList=glGenLists(1);
    glNewList(displayList,GL_COMPILE);
    
    if(textured){
        glBindTexture(GL_TEXTURE_2D, ground.texture[0]);
        drawModel(ground.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(1);
        }
        
        glBindTexture(GL_TEXTURE_2D, wallRightCorner.texture[0]);
        drawModel(wallRightCorner.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(2);
        }
        
        glBindTexture(GL_TEXTURE_2D, wallLeftCorner.texture[0]);
        drawModel(wallLeftCorner.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(3);
        }
        glBindTexture(GL_TEXTURE_2D, angel.texture[0]);
        drawModel(angel.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(4);
        }
        
        glBindTexture(GL_TEXTURE_2D, cube.texture[0]);
        drawModel(cube.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        
        if(mode == GL_SELECT){
            glLoadName(5);
        }
        glBindTexture(GL_TEXTURE_2D, sphere.texture[0]);
        drawModel(sphere.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(6);
        }
        glBindTexture(GL_TEXTURE_2D, cone.texture[0]);
        drawModel(cone.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(7);
        }
        glBindTexture(GL_TEXTURE_2D, cylinder.texture[0]);
        drawModel(cylinder.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(8);
        }
        glBindTexture(GL_TEXTURE_2D, monkey.texture[0]);
        drawModel(monkey.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(9);
        }
        glBindTexture(GL_TEXTURE_2D, apple.texture[0]);
        drawModel(apple.object, GL_SMOOTH|GL_TEXTURE|GL_MATERIAL);
        if(mode == GL_SELECT){
            glLoadName(10);
        }
    }
    else{
        glBindTexture(GL_TEXTURE_2D, ground.texture[0]);
        drawModel(ground.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            //cout << "we are here" << endl;
            glLoadName(1);
        }
        
        glBindTexture(GL_TEXTURE_2D, wallRightCorner.texture[0]);
        drawModel(wallRightCorner.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(2);
        }
        
        glBindTexture(GL_TEXTURE_2D, wallLeftCorner.texture[0]);
        drawModel(wallLeftCorner.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(3);
        }
        
        
        glBindTexture(GL_TEXTURE_2D, angel.texture[0]);
        drawModel(angel.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(4);
        }
        
        
        glBindTexture(GL_TEXTURE_2D, cube.texture[0]);
        drawModel(cube.object, GL_SMOOTH);
        
        if(mode == GL_SELECT){
            glLoadName(5);
        }
        
        
        glBindTexture(GL_TEXTURE_2D, sphere.texture[0]);
        drawModel(sphere.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(6);
        }
        
        
        glBindTexture(GL_TEXTURE_2D, cone.texture[0]);
        drawModel(cone.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(7);
        }
        
        
        glBindTexture(GL_TEXTURE_2D, cylinder.texture[0]);
        drawModel(cylinder.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(8);
        }
        
        
        glBindTexture(GL_TEXTURE_2D, monkey.texture[0]);
        drawModel(monkey.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(9);
        }
        
        
        glBindTexture(GL_TEXTURE_2D, apple.texture[0]);
        drawModel(apple.object, GL_SMOOTH);
        if(mode == GL_SELECT){
            glLoadName(10);
        }
        
    }
    
	glEndList();
}

/*  processHits prints out the contents of the
 *  selection array.
 */
void processHits (GLint hits, GLuint buffer[])
{
    unsigned int i, j;
    GLuint ii, jj, names, *ptr;
    
   // printf ("hits = %d\n", hits);
    ptr = (GLuint *) buffer;
    for (i = 0; i < hits; i++) { /*  for each hit  */
        names = *ptr;
       // printf (" number of names for this hit = %d\n", names);
        ptr++;
       // printf("  z1 is %g;", (float) *ptr/0x7fffffff);
        ptr++;
        //printf(" z2 is %g\n", (float) *ptr/0x7fffffff);
        ptr++;
        //printf ("   names are ");
        for (j = 0; j < names; j++) { /*  for each name */
            //printf ("%d ", *ptr);
            if(*ptr == 1){
                cout << "You have picked the ground" <<endl;
                pickedObjID = 1;
            }
            if(*ptr == 2){
                cout << "You have picked the wall" <<endl;
                pickedObjID = 2;
            }
            if(*ptr == 3){
                cout << "You have picked the other wall" <<endl;
                pickedObjID = 3;
            }
            if(*ptr == 4){
                cout << "You have picked the sphere" <<endl;
                pickedObjID = 4;
            }
            if(*ptr == 5){
                cout << "You have picked the monkey" <<endl;
                pickedObjID = 5;
            }
            
            if(*ptr == 6){
                cout << "You have picked the cube" <<endl;
                pickedObjID = 6;
            }
            if(*ptr == 7){
                cout << "You have picked the cone" <<endl;
                pickedObjID = 7;
            }
            if(*ptr == 8){
                cout << "You have picked the cylinder" <<endl;
                pickedObjID = 8;
            }
            if(*ptr == 9){
                cout << "You have picked the angel" <<endl;
                pickedObjID = 9;
            }
            if(*ptr == 10){
                cout << "You have picked the apple" <<endl;
                pickedObjID = 10;
            }
            
            if (j == 0)  /*  set row and column  */
                ii = *ptr;
            else if (j == 1)
                jj = *ptr;
            ptr++;
        }
        
    }
}


/* This function makes OpenGL calls using the data
 * which has already been obtained from the files.
 * model    - initialized Modelobject structure
 * mode     - a bitwise OR of values describing what is to be rendered.
 *            GL_SMOOTH   -  render with vertex normals
 *            GL_TEXTURE  -  render with texture coords
 *            GL_COLOR    -  render with colors (color material)
 *            GL_MATERIAL -  render with materials
 *            GL_COLOR and GL_MATERIAL should not both be specified.
 */
GLvoid drawModel(Modelobject* model, GLuint mode)
{
    static GLuint i;
    static Modelgroup* group;
    static Modeltriangle* triangle;
    static Modelmaterial* material;
    
    // Make sure model is not NULL
    assert(model);
    assert(model->vertices);
    
    /* Give warning if data is missing */
    if (mode & GL_SMOOTH && !model->normals) {
        printf("drawComplexModel() warning: smooth render mode requested "
               "with no normals defined.\n");
        mode &= ~GL_SMOOTH;
    }
    if (mode & GL_TEXTURE && !model->texcoords) {
        printf("drawComplexModel() warning: texture render mode requested "
               "with no texture coordinates defined.\n");
        mode &= ~GL_TEXTURE;
    }
    if (mode & GL_FLAT && mode & GL_SMOOTH) {
        printf("drawComplexModel() warning: flat render mode requested "
               "and smooth render mode requested (using smooth).\n");
        mode &= ~GL_FLAT;
    }
    if (mode & GL_COLOR)
        glEnable(GL_COLOR_MATERIAL);
    else if (mode & GL_MATERIAL)
        glDisable(GL_COLOR_MATERIAL);
    
    
    group = model->groups;
    while (group) {
        if (mode & GL_MATERIAL) {
            material = &model->materials[group->material];
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
        }
        
        glBegin(GL_TRIANGLES);
        for (i = 0; i < group->numtriangles; i++) {
            triangle = &Triangulate((group->triangles[i]));
            
            if (mode & GL_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[0]]);
            if (mode & GL_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[0]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
            
            if (mode & GL_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[1]]);
            if (mode & GL_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[1]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
            
            if (mode & GL_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[2]]);
            if (mode & GL_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[2]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
            
        }
        glEnd();
        
        group = group->next;
    }
}


// Load Bitmaps And Convert To Textures
// http://3dgep.com/?p=2417
int LoadGLTextures()
{
	/* load an image file directly as a new OpenGL texture */
	ground.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/wooden.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(ground.texture[0] == 0)
		return false;
    
    wallRightCorner.texture[0]= SOIL_load_OGL_texture
    (
     "Texture/graybricks.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
    if(wallRightCorner.texture[0] == 0)
		return false;
    
    /* load an image file directly as a new OpenGL texture */
	wallLeftCorner.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/graybricks.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(wallLeftCorner.texture[0] == 0)
		return false;
    
    /* load an image file directly as a new OpenGL texture */
	cube.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/cubeTexture.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(cube.texture[0] == 0)
		return false;
    
    /* load an image file directly as a new OpenGL texture */
	sphere.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/metal.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(sphere.texture[0] == 0)
		return false;
    
    
    /* load an image file directly as a new OpenGL texture */
	cone.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/coneTexture.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(cone.texture[0] == 0)
		return false;
    
    /* load an image file directly as a new OpenGL texture */
	cylinder.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/sand.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(cylinder.texture[0] == 0)
		return false;
    
    /* load an image file directly as a new OpenGL texture */
	angel.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/marbleTexture2.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(angel.texture[0] == 0)
		return false;
    
    /* load an image file directly as a new OpenGL texture */
	monkey.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/wooden.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(monkey.texture[0] == 0)
		return false;
    
    /* load an image file directly as a new OpenGL texture */
	apple.texture[0] = SOIL_load_OGL_texture
    (
     "Texture/Apple_Sphere.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_INVERT_Y
     );
    
	if(apple.texture[0] == 0)
		return false;
    
    
	// Typical Texture Generation Using Data From The Bitmap
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
	return true;// Return Success
}

//-------------------------------------------------------------------------
//  Help
//-------------------------------------------------------------------------
void help()
{
	printf("\n***************** \t\t Using the program \t\t *****************\n\n");
	
	printf("Keyboard\n\n");
    
	printf(" %s\t\t%s\n", "Up\\Down", "Move the scene up and down");
	printf("\n");
	printf(" %s\t\t\t%s\n", "l or L", "Cycle through light settings");
	printf(" %s\t\t\t%s\n", "t or T", "Texture on/off");
	printf(" %s\t\t\t%s\n", "space bar", "Animate light in circular path");
	printf(" %s\t\t\t%s\n", "v or V", "Cycle through predefined viewpoints");
    printf(" %s\t\t\t%s\n", "Pick and press m or M", "Switch texture");
    printf(" %s\t\t\t%s\n", "s", "To move scene towards left");
    printf(" %s\t\t\t%s\n", "w", "To move scene towards right");
    
}
