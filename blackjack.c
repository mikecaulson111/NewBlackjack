#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "opengl.h"
#include "customBMP.h"

// in degrees
#define Cos(th) cos(3.14159265/180*(th))
#define Sin(th) sin(3.14159265/180*(th))

int fov=53;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=8;     //  Size of world

double dt = 0.1;

int th = 0;
int ph = 0;

int lighting = 1;

double Ex = 0;
double Ez = 5;

double Cy;
float eyeposy = 1;
double dx;
double dz;

double ylight = 9.0;

bool walk_straight = false;
bool walk_right = false;
bool walk_back = false;
bool walk_left = false;

unsigned int textures[5];
int numTextures = 1;

int shader[3] = {0,0,0};

const float Emission[]  = {0.0,0.0,0.0,1.0};
const float Ambient[]   = {0.5,0.5,0.5,1.0};
// const float Ambient[] = {0.03,0.03,0.03,1.0}; // USE THIS IN THE END
const float Diffuse[]   = {1.0,1.0,1.0,1.0};
const float Specular[]  = {1.0,1.0,1.0,1.0};
const float Direction[] = {0.0,-1.0,0.0,1.0};
const float Shinyness[] = {64}; // was 16

/*
 * Set color
 */
void SetColor(float R,float G,float B)
{
   float color[] = {R,G,B,1.0};
   glColor3f(R,G,B);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
}

void project(double fov, double asp, double dim)
{
    glMatrixMode(GL_PROJECTION);

    glLoadIdentity();

    if ( fov )
    {
        gluPerspective(fov, asp, dim/16, dim * 16);
    }
    else
    {
        glOrtho(-asp*dim,asp*dim,-dim,dim,-dim,dim);
    }

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
}

// rectangle with normals pointing outward
void Rectang(double xlow, double ylow, double zlow, double xhigh, double yhigh, double zhigh,int s, int t) {
    double dx = xhigh-xlow;
    double dy = yhigh-ylow;
    double dz = zhigh-zlow;
    
    glPushMatrix();
    glTranslated(xlow,ylow,zlow);
    glScaled(dx,dy,dz);

    glBegin(GL_QUADS);

    // front side
    glNormal3f(0,0,1);
    glTexCoord2f(0,t); glVertex3d(0, 1,  1);
    glTexCoord2f(0,0); glVertex3d(0, 0, 1);
    glTexCoord2f(s,0); glVertex3d(1, 0, 1);
    glTexCoord2f(s,t); glVertex3d(1, 1,  1);


    // bottom
    glNormal3f(0,-1,0);
    glTexCoord2f(0,t); glVertex3d(0,  0, 1);
    glTexCoord2f(0,0); glVertex3d(0,  0, 0);
    glTexCoord2f(s,0); glVertex3d(1, 0, 0);
    glTexCoord2f(s,t); glVertex3d(1, 0, 1);

    // top
    glNormal3f(0,1,0);
    glTexCoord2f(0,t); glVertex3d(0,  1, 1);
    glTexCoord2f(0,0); glVertex3d(0,  1, 0);
    glTexCoord2f(s,0); glVertex3d(1, 1, 0);
    glTexCoord2f(s,t); glVertex3d(1, 1, 1);

    // left side:
    glNormal3f(-1,0,0);
    glTexCoord2f(0,t); glVertex3d(0, 1,  0);
    glTexCoord2f(0,0); glVertex3d(0, 0, 0);
    glTexCoord2f(s,0); glVertex3d(0, 0, 1);
    glTexCoord2f(s,t); glVertex3d(0, 1,  1);

    // right side:
    glNormal3f(1,0,0);
    glTexCoord2f(0,t); glVertex3d(1, 1,  1);
    glTexCoord2f(0,0); glVertex3d(1, 0, 1);
    glTexCoord2f(s,0); glVertex3d(1, 0, 0);
    glTexCoord2f(s,t); glVertex3d(1, 1,  0);

    // back side:
    glNormal3f(0,0,-1);
    glTexCoord2f(0,t); glVertex3d(1, 1,  0);
    glTexCoord2f(0,0); glVertex3d(1, 0, 0);
    glTexCoord2f(s,0); glVertex3d(0,   0, 0);
    glTexCoord2f(s,t); glVertex3d(0,   1,  0);

    glEnd();

    glPopMatrix();
}

// cylinder with normal pointing out
void Cylinder(double x, double y, double z, double r, double h,double th, double ph) {
    glPushMatrix();
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glRotated(ph,1,0,0);
    // glScaled(r,h,r);
    glBegin(GL_QUAD_STRIP);
    for(int th = 0; th <= 360; th += 30) {
        glNormal3f(Cos(th), 0, Sin(th));
        glTexCoord2f(3*(360./th),0); glVertex3d(r*Cos(th), 0, r*Sin(th));
        glTexCoord2f(3*(360./th),0.98);glVertex3d(r*Cos(th), h, r*Sin(th));
    }
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,1,0);
    glVertex3d(0,h,0);
    for(int th = 0; th <= 360; th += 30) {
        glVertex3d(r*Cos(th),h,r*Sin(th));
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0,-1,0);
    glVertex3d(0,0,0);
    for(int th = 0; th <= 360; th += 30) {
        glVertex3d(r*Cos(th),0,r*Sin(th));
    }
    glEnd();
    glPopMatrix();

}

// // This is cented on the MIDDLE NOT THE LEFT BOTTOM
// void Table(double x, double y, double z, double dx, double dy, double dz, double th) {
//     glPushMatrix();

//     glTranslated(x,y,z);
//     glRotated(th,0,1,0);
//     glScaled(dx,dy,dz);

//     // SetColor(1,1,1);
//     Rectang(-1,1.2,-4,1,1.3,3,4,1);
//     Cylinder(0,0,-0.5,0.1,1.25,0,0);
//     Cylinder(0,0,-2.75,0.1,1.25,0,0);
//     Cylinder(0,0,1.75,0.1,1.25,0,0);

//     // ErrCheck("Table");
//     glPopMatrix();
// }

void HalfCircle(double x, double y, double z, double r, double h, double th)
{
    glPushMatrix();

    glBegin(GL_TRIANGLE_FAN);

    glNormal3f(0, 1, 0);
    glVertex3f(0,0,0);
    // adding a comment here for fun
    for(int i = 0; i < 180; i += 10)
    {
        double tx = Cos(i);
        double tz = Sin(i);
        glTexCoord2d(tx/2 + 0.5, tz/2 + 0.5);
        glVertex3f(tx,0,tz);
    }

    glEnd();

    glPopMatrix();
}

void Table(double x, double y, double z, double dx, double dy, double dz, double th) {
    glPushMatrix();

    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);

    HalfCircle(0, 1, 0, 1, 1, 0);

    glPopMatrix();
}

void display() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glLoadIdentity();

    // testing mjc
    // glEnable(GL_CULL_FACE);

    //  Set materials
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT, Ambient);

    float position[4] = {0, ylight, 0, 1.0};
    // float direction[3] = {0, -1, 0};

    glPushMatrix();
    if (lighting)
    {
        glEnable(GL_LIGHT0);
        
        // glColor3f(1,1,1);
        SetColor(1,1,1);
        glUseProgram(0);
        Rectang(position[0]-0.05, position[1]-0.05, position[2]-0.05, position[0]+0.05, position[1]+0.05, position[2]+0.05, 0, 0);
    }
    else
    {
        glDisable(GL_LIGHT0);
    }

    glLightfv(GL_LIGHT0, GL_AMBIENT, Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    // glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction);


    glEnable(GL_NORMALIZE);
    glPopMatrix();

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

    glUseProgram(shader[1]);

    double Cx = +2*Sin(th+180)*Cos(ph+180) + Ex;
    Cy = -2*Sin(ph+180) + eyeposy;
    double Cz = -2*Cos(th+180)*Cos(ph+180) + Ez;
    gluLookAt(Ex,eyeposy,Ez, Cx, Cy, Cz, 0,1,0);

    // glTranslated(0,0,0);
    // glRotated(0,0,0,0);

    // skybox:
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    SetColor(1, 1, 1);
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    glTexCoord2d(0, 0); glVertex3f(-10,0,-10);
    glTexCoord2d(0, 10); glVertex3f(-10,0,10);
    glTexCoord2d(10, 10); glVertex3f(10,0,10);
    glTexCoord2d(10, 0); glVertex3f(10,0,-10);
    glEnd();

    SetColor(173./255., 216./255., 230./255.);
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glBegin(GL_QUADS);
    glNormal3f(0,-1,0);
    glTexCoord2d(0,0); glVertex3f(-10,10,-10);
    glTexCoord2d(0,1); glVertex3f(10,10,-10);
    glTexCoord2d(1,1); glVertex3f(10,10,10);
    glTexCoord2d(1,0); glVertex3f(-10,10,10);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    SetColor(55./255., 80./255., 166./255.);
    glBegin(GL_QUADS);
    // right wall
    glNormal3f(-1,0,0);
    glTexCoord2d(0, 0); glVertex3f(10, 0, 10);
    glTexCoord2d(0, 20); glVertex3f(10, 10, 10);
    glTexCoord2d(10, 20); glVertex3f(10, 10, -10);
    glTexCoord2d(10, 0); glVertex3f(10,0,-10);
    
    // left wall
    glNormal3f(1,0,0);
    glTexCoord2d(0, 0); glVertex3f(-10, 0, 10);
    glTexCoord2d(10, 0); glVertex3f(-10, 0, -10);
    glTexCoord2d(10, 20); glVertex3f(-10, 10, -10);
    glTexCoord2d(0, 20); glVertex3f(-10, 10, 10);

    // front wall
    glColor3f(98./255., 127./255., 229./255.);
    glNormal3f(0,0,-1);
    glTexCoord2d(0 ,0); glVertex3f(-10, 0, 10);
    glTexCoord2d(0 ,20); glVertex3f(-10, 10, 10);
    glTexCoord2d(10 ,20); glVertex3f(10, 10, 10);
    glTexCoord2d(10 ,0); glVertex3f(10, 0, 10);

    // back wall
    glNormal3f(0,0,1);
    glTexCoord2d(0 ,0); glVertex3f(-10, 0, -10);
    glTexCoord2d(10 ,0); glVertex3f(10, 0, -10);
    glTexCoord2d(10 ,20); glVertex3f(10, 10, -10);
    glTexCoord2d(0 ,20); glVertex3f(-10, 10, -10);
    glEnd();


    // end of skybox

    // glColor3f(1,1,1);
    SetColor(1,1,1);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    Table(0,0,0,1,0.5,1,0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // red plane in middle
    // glColor3f(1.0, 0.0, 0.0);
    SetColor(1,0,0);
    glBegin(GL_QUADS);
    glVertex2d(-0.5, 0.5);
    glVertex2d(0.5, 0.5);
    glVertex2d(0.5, 1.5);
    glVertex2d(-0.5, 1.5);
    glEnd();

    glFlush();
    glutSwapBuffers();
}

void keys(unsigned char ch, int x, int y)
{
    switch (ch)
    {
        case 27:
        case 'q':
            exit(0);
            break;
        case 'w':
            walk_straight = true;
            break;
        case 'a':
            walk_left = true;
            break;
        case 's':
            walk_back = true;
            break;
        case 'd':
            walk_right = true;
            break;
        case 'l':
            lighting += 1;
            lighting %=2;
            break;
        case '-':
            ylight -= 0.5;
            break;
        case '+':
            ylight += 0.5;
    }

    project(fov, asp, dim);
    glutPostRedisplay();
}

void keyUp(unsigned char ch, int x, int y)
{
    switch (ch)
    {
        case 'w':
            walk_straight = false;
            break;
        case 'a':
            walk_left = false;
            break;
        case 's':
            walk_back = false;
            break;
        case 'd':
            walk_right = false;
            break;
    }
}

void idle()
{
    if ( walk_straight == true && walk_back == false )
    {
        dx = 2*Sin(th);
        dz = -2*Cos(th);
        Ex = Ex + (dt * dx);
        Ez = Ez + (dt * dz);
    } else if ( walk_straight == false && walk_back == true )
    {
        dx = 2*Sin(th);
        dz = -2*Cos(th);
        Ex = Ex - (dt * dx);
        Ez = Ez - (dt * dz);
    }

    if ( walk_right == true && walk_left == false )
    {
        dx = 2*Sin(th+90);
        dz = -2*Cos(th+90);
        Ex = Ex + (dt * dx);
        Ez = Ez + (dt * dz);
    } else if ( walk_right == false && walk_left == true )
    {
        dx = 2*Sin(th-90);
        dz = -2*Cos(th-90);
        Ex = Ex + (dt * dx);
        Ez = Ez + (dt * dz);
    }

    glutPostRedisplay();
}

// Controls the rotations of the scene
void special(int key, int x, int y) {
    if(key == GLUT_KEY_RIGHT) {
        th += 5;
    } else if (key == GLUT_KEY_LEFT) {
        th -= 5;
    } else if (key == GLUT_KEY_UP) {
        if(ph < 90) {
            ph += 5;
        }
    } else if (key == GLUT_KEY_DOWN) {
        if(ph > -85) {
            ph -= 5;
        }
    } 

    ph %= 360;
    th %= 360;

    project(fov,asp,dim);

    glutPostRedisplay();
}

void reshape(int width,int height)
{
   //  Set the viewport to the entire window
    asp = (height>0) ? (double)width/height : 1;
    glViewport(0,0, RES*width,RES*height);
    // Calls the Project to deal with different projections:
    project(fov,asp,dim);
}

// mjc rewrite this function to make it more my own --------------------------------------------------------

/*
 *  Read text file
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) printf("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) printf("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) printf("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
//    PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
//    PrintProgramLog(prog);
   //  Return name
   return prog;
}

int main(int argc, char* argv[]) 
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    glutInitWindowSize(500,500);

    glutCreateWindow("stopwatch");

#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif


    glutKeyboardFunc(keys);

    glutSpecialFunc(special);

    glutDisplayFunc(display);

    glutKeyboardUpFunc(keyUp);

    // glutKeyboardDownFunc(keyDown);

    // glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutIdleFunc(idle);

    glutReshapeFunc(reshape);

    shader[1] = CreateShaderProg("pixtex.vert","pixtex.frag");

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

    // Textures
    textures[0] = loadBMP("classic-cards.bmp");
    textures[1] = loadBMP("tempwood.bmp");
    textures[2] = loadBMP("greenfelt2.bmp");
    textures[3] = loadBMP("newcobble.bmp");
    textures[4] = loadBMP("btable.bmp");

    printf("HELLO AGAIN\n");

    glutMainLoop();
}