#include<windows.h>
#include <GL/glut.h>
#include<bits/stdc++.h>
#include <stdlib.h>
#define rad (3.1416/180)
#define EN_SIZE 20

//#include "RGBpixmap.cpp"

using namespace std;

//RGBpixmap pix[6];

float zoom=4;
int tola[5000][5000];
float tX=0,tY=0,tZ=-8,rX=0,rY=0,rZ=4;
float tZ1=-20,tZ2=-40,tZ3=-60,tZ4=-80,tZ5=-100,tZ6=-120;
float rotX=0,rotY=0,rotZ=0;
float cosX=0,cosY=1,cosZ=0;
float angle=0;
float xEye=0.0f,yEye=5.0f,zEye=30.0f;
float cenX=0,cenY=0,cenZ=0,roll=0;
float radius=0;
float theta=0,slope=0;
float speed = 0.3;
float angleBackFrac = 0.2;
float r[] = {0.1,0.4,0.0,0.9,0.2,0.5,0.0,0.7,0.5,0.0};
float g[] = {0.2,0.0,0.4,0.5,0.2,0.0,0.3,0.9,0.0,0.2};
float b[] = {0.4,0.5,0.0,0.7,0.9,0.0,0.1,0.2,0.5,0.0};
int TIME=0;
bool START = false,gameOver = false;
float torusPosX[7] = {2,-2,2.6,-1,-3.5,3,0};
float torusPosY[7] = {3,1,4,1,5,3,2};
float torusPosZ[7] = {6,1,7,10,5,8,2};
float cubePosX[] = {2, 0, -1,-2,0,0, };
float cubePosY[] = {2, 3, 4, 1,1,5, };
bool rot = false;
unsigned int ID1, ID2;



class BmpLoader
{
    public:
        unsigned char* textureData;
        int iWidth, iHeight;

        BmpLoader(const char*);
        ~BmpLoader();

    private:
        BITMAPFILEHEADER bfh;
        BITMAPINFOHEADER bih;
};


BmpLoader::BmpLoader(const char* filename)
{
    FILE *file=0;
    file=fopen(filename, "rb");
    if(!file)
        std::cout<<"File not found"<<std::endl;
    fread(&bfh, sizeof(BITMAPFILEHEADER),1,file);
    if(bfh.bfType != 0x4D42)
        std::cout<<"Not a valid bitmap"<<std::endl;
    fread(&bih, sizeof(BITMAPINFOHEADER),1,file);
    if(bih.biSizeImage==0)
        bih.biSizeImage=bih.biHeight*bih.biWidth*3;
    textureData = new unsigned char[bih.biSizeImage];
    fseek(file, bfh.bfOffBits, SEEK_SET);
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for(int i=0; i<bih.biSizeImage; i+=3)
    {
        temp = textureData[i];
        textureData[i] = textureData[i+2];
        textureData[i+2] = temp;

    }

    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader()
{
    delete [] textureData;
}


void LoadTexture1(const char*filename)
{
    glGenTextures(1, &ID1);
    glBindTexture(GL_TEXTURE_2D, ID1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}




static GLfloat v_box[8][3] =
{
    {-2.0, 0.0, 0.0},
    {2.0, 0.0, 0.0},
    {-2.0, 0.0, 2.0},
    {2.0, 0.0, 2.0},

    {-2.0, 2.0, 0.0},
    {2.0, 2.0, 0.0},
    {-2.0, 2.0, 2.0},
    {2.0, 2.0, 2.0}
};

static GLubyte quadIndices1[6][4] =
{

    {0,2,6,4},
    {2,3,7,6},
    {1,5,7,3},
    {1,5,4,0},
    {0,2,3,1},
    {6,7,5,4}

};

static GLubyte quadIndices2[2][4] =
{
    {0,2,3,1},
    {6,7,5,4}
};


static void getNormal3p
(GLfloat x1, GLfloat y1,GLfloat z1, GLfloat x2, GLfloat y2,GLfloat z2, GLfloat x3, GLfloat y3,GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}






static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void drawGround()
{
    //glColor3d(0,1,0);
    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        //glColor3f(colors[4][0],colors[4][1],colors[4][2]);
        getNormal3p(v_box[quadIndices1[i][0]][0], v_box[quadIndices1[i][0]][1], v_box[quadIndices1[i][0]][2],
                    v_box[quadIndices1[i][1]][0], v_box[quadIndices1[i][1]][1], v_box[quadIndices1[i][1]][2],
                    v_box[quadIndices1[i][2]][0], v_box[quadIndices1[i][2]][1], v_box[quadIndices1[i][2]][2]);

        glVertex3fv(&v_box[quadIndices1[i][0]][0]);glTexCoord2f(0,1);
        glVertex3fv(&v_box[quadIndices1[i][1]][0]);glTexCoord2f(1,1);
        glVertex3fv(&v_box[quadIndices1[i][2]][0]);glTexCoord2f(1,0);
        glVertex3fv(&v_box[quadIndices1[i][3]][0]);glTexCoord2f(0,0);
    }
    glEnd();



//    glBegin(GL_QUADS);
//    for (GLint i = 0; i <6 ; i++)
//    {
//        //glColor3f(0.3,0.3,1);
//        //glColor3f(colors[i][0],colors[i][1],colors[i][2]);
//        glVertex3fv(&v_cube[quadIndices[i][0]][0]);
//        glVertex3fv(&v_cube[quadIndices[i][1]][0]);
//        glVertex3fv(&v_cube[quadIndices[i][2]][0]);
//        glVertex3fv(&v_cube[quadIndices[i][3]][0]);
//    }
//    glEnd();
}





void drawBox2()
{

    glBegin(GL_QUADS);
    for (GLint i = 0; i <2; i++)
    {
        //glColor3f(colors[4][0],colors[4][1],colors[4][2]);
        getNormal3p(v_box[quadIndices2[i][0]][0], v_box[quadIndices2[i][0]][1], v_box[quadIndices2[i][0]][2],
                    v_box[quadIndices2[i][1]][0], v_box[quadIndices2[i][1]][1], v_box[quadIndices2[i][1]][2],
                    v_box[quadIndices2[i][2]][0], v_box[quadIndices2[i][2]][1], v_box[quadIndices2[i][2]][2]);

        glVertex3fv(&v_box[quadIndices2[i][0]][0]);glTexCoord2f(1,1);
        glVertex3fv(&v_box[quadIndices2[i][1]][0]);glTexCoord2f(0,1);
        glVertex3fv(&v_box[quadIndices2[i][2]][0]);glTexCoord2f(1,0);
        glVertex3fv(&v_box[quadIndices2[i][3]][0]);glTexCoord2f(0,0);
    }
    glEnd();
    //glutSolidSphere (3.0, 20, 16);


}


void environment(int n,double aa,int x,int y){


    //if(tY == -torusPosY[n]) START = false;
    int tmp = n;
    /// Ground
    glColor3d(0.7,0.7,0.7);
//    glEnable(GL_TEXTURE_2D);
//    LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\ground.bmp");
    glPushMatrix();
        glTranslated(0,0,0);
        glScaled(EN_SIZE*1.3,0.3,EN_SIZE*2);
        glutSolidCube(1);
       // drawGround();
    glPopMatrix();
    //glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\obj2.bmp");
     glColor3d(1,1,1);
    glPushMatrix();
        glTranslated(cubePosX[n],cubePosY[n]-1,5);
        //glTranslated(2,3,0);
        glScaled(0.4,0.8,0.8);
        drawGround();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    //printf("%d\n",n);


    int a = aa;
    glColor3d(0.9,0.8,0);
    glPushMatrix();
        glTranslated(torusPosX[n],torusPosY[n],torusPosZ[n]);
        glScaled(0.3,0.3,0.07);
        glRotated(aa,0,1,0);
        glutSolidTorus(1,1,30,30);
    glPopMatrix();
    //printf("%d\n",n);

    n += 2;
    if(n>6) n-=5;
    glColor3d(0.9,0.8,0);
    glPushMatrix();
        glTranslated(torusPosX[n]-2,torusPosY[n]+1,torusPosZ[n]+9);
        glScaled(0.3,0.3,0.07);
        glRotated(aa,0,1,0);
        glutSolidTorus(1,1,30,30);
    glPopMatrix();


    n += 3;
    if(n>6) n-=6;
    glPushMatrix();
        glTranslated(torusPosX[n],torusPosY[n],torusPosZ[n]);
        glScaled(0.3,0.3,0.07);
        glRotated(aa,0,1,0);
        glutSolidTorus(1,1,30,30);
    glPopMatrix();


    n= tmp;
    glEnable(GL_TEXTURE_2D);

    switch (n)
    {
        case 0:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new1.bmp");
            break;
        case 1:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new1.bmp");
            break;
        case 2:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new3.bmp");
            break;
        case 3:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new3.bmp");
            break;
        case 4:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new1.bmp");
            break;
        case 5:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new3.bmp");
            break;

    }

    //LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new3.bmp");

    glColor3d(1.0,1.0,1.0);
    glPushMatrix();
        glTranslated(6,0,0);
        glScaled(0.6,2,0.6);
        drawGround();


        switch (n)
    {
        case 0:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new3.bmp");
            break;
        case 1:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new3.bmp");
            break;
        case 2:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new1.bmp");
            break;
        case 3:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new1.bmp");
            break;
        case 4:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new3.bmp");
            break;
        case 5:
            LoadTexture1("C:\\Users\\Suzon\\Desktop\\CGProject\\new1.bmp");
            break;

    }

        glTranslated(-20,0,0);
        drawGround();
       // printf("%d   %f  %f  %f\n",n,tX,tY,tZ);
        //drawBox2();
        if((tZ>-n*20-3 and tZ<-n*20 )and (tX <-4.5 or tX>4.5) and (tY>= -3.4)) {gameOver = true;START = false;}





    glPopMatrix();
    glDisable(GL_TEXTURE_2D);




   // glColor3d(0,1,0.7);

//    glPushMatrix();
//        glRotated(angle,0,1,0);
//        glPushMatrix();
//            glTranslated(tX,tY,tZ);
//            glScaled(1,1,2);
//            //glRotated(90,1,0,0);
//            glutSolidCube(1);
//        glPopMatrix();
//    glPopMatrix();

    if((tZ>-n*20-8 and tZ<-n*20-5 )and (tX >1 and tX<3) and (tY>= -1)) {gameOver = true;START = false;}
    else if((tZ>-n*20-8 and tZ<-n*20-5 )and (tX >-1 and tX<1) and (tY>-3 and tY<-1)) {gameOver = true;START = false;}
    else if((tZ>-n*20-8 and tZ<-n*20-5 )and (tX >-1 and tX<1) and (tY>-5 and tY<-3)) {gameOver = true;START = false;}

   // else if((tZ>-n*20-8 and tZ<-n*20-5 )and (tX >-1 and tX<1) and (tY>-6 and tY<-5)) {gameOver = true;START = false;}
//
//    else if((tZ>-n*20-8 and tZ<-n*20-5 )and (tX >-1 and tX<1) and (tY>-1 and tY<1)) {gameOver = true;START = false;}
//    else if((tZ>-n*20-8 and tZ<-n*20-5 )and (tX >0 and tX<2) and (tY>-4 and tY<-2)) {gameOver = true;START = false;}
//    else if((tZ>-n*20-8 and tZ<-n*20-5 )and (tX >-3 and tX<-1) and (tY>0 and tY<-2)) {gameOver = true;START = false;}
}

void draw(){
    double t = glutGet(GLUT_ELAPSED_TIME) / 100.0;
    double a = t*90.0;
    //if(gameOver == true) t = 0;

    TIME = t;

    ///Plane
    if(rotX>11)rotX=11;
    if(rotX<-11)rotX=-11;
    if(rotZ>10)rotZ=10;
    if(rotZ<-15)rotZ=-15;

    //if((tZ >=0 ) and (tX <-5) and (tY>=0 and tY<=2)) START = false;

    glPushMatrix();
        glTranslated(0,1,0);
        glRotated(90,0,1,0);
        glRotated(5,0,0,1);
        glRotated(rotX,1,0,0);
        glRotated(rotY,0,1,0);
        glRotated(rotZ,0,0,1);
        glColor3d(1,0,0);
        glutSolidSphere (0.15, 50, 50);
        glScaled(0.4,0.4,0.4);

    glPopMatrix();

    ///Environment
    if(tX>=7.1)tX=7.1;
    if(tX<=-7.1)tX=-7.1;
    if(tY>0.5)tY= 0.5;
    if(tY<-6)tY= -6;

    int addX = rand()% 4;
    int sign = rand()%2;
    if(sign) addX *= -1;
    int addY = rand() % 6;

    glPushMatrix();
        glTranslated(tX,tY,tZ);
        //if(tX and tY and tZ) START = false;
        environment(0,a,addX,addY);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ2);
        environment(3,a,addX,addY);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ3);
        environment(1,a,addX,addY);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ4);
        environment(5,a,addX,addY);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ5);
        environment(4,a,addX,addY);
    glPopMatrix();

    glPushMatrix();
        glTranslated(tX,tY,tZ6);
        environment(2,a,addX,addY);
    glPopMatrix();

    tZ+=speed;
    tZ1+=speed;
    tZ2+=speed;
    tZ3+=speed;
    tZ4+=speed;
    tZ5+=speed;
    tZ6+=speed;

    if(tZ>=20)tZ=-110;
    if(tZ1>=20)tZ1=-110;
    if(tZ2>=20)tZ2=-110;
    if(tZ3>=20)tZ3=-110;
    if(tZ4>=20)tZ4=-110;
    if(tZ5>=20)tZ5=-110;
    if(tZ6>=20)tZ6=-110;

    if(rotX>0)rotX-=angleBackFrac;
    if(rotX<0)rotX+=angleBackFrac;
    if(rotY>0)rotY-=angleBackFrac;
    if(rotY<0)rotY+=angleBackFrac;
    if(rotZ>0)rotZ-=angleBackFrac;
    if(rotZ<0)rotZ+=angleBackFrac;


    speed += 0.0008;
    //if(speed>=0.7)speed=0.7;
}


void drawBitmapText(char *str,float x,float y,float z)
{
	char *c;
	glRasterPos3f(x,y+8,z);

	for (c=str; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *c);
	}
}

void drawStrokeText(char* str,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x, y+8,z);
	  glScalef(0.002f,0.002f,z);

	  for (c=str; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}

void drawStrokeText2(char* str,int x,int y,int z)
{
	  char *c;
	  glPushMatrix();
	  glTranslatef(x, y+8,z);
	  glScalef(0.005f,0.005f,z);

	  for (c=str; *c != '\0'; c++)
	  {
    		glutStrokeCharacter(GLUT_STROKE_ROMAN , *c);
	  }
	  glPopMatrix();
}
void drawStrokeChar(char c,float x,float y,float z)
{
	  glPushMatrix();
          glTranslatef(x, y+8,z);
          glScalef(0.002f,0.002f,z);
          glutStrokeCharacter(GLUT_STROKE_ROMAN , c);
	  glPopMatrix();
}


static void display(void)
{
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    double a = t*90.0;
    double aa=a;

    if(!rot){
        a=0;
    }
    int mod,number=0;
    float tmp=0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();


    gluLookAt(	0.0, 6, 10.0,
                0, 4, 0,
                0, 1.0f, 0.0f);

    if(START){

        glPushMatrix();
            glTranslated(0,0,0);
            glScaled(zoom,zoom,zoom);
            glRotated(a,0,1,0);
            draw();
        glPopMatrix();
        glColor3d(0,0,0);
        drawStrokeText("UP: W, DOWN: S, LEFT: A, RIGHT: D, MAIN MENU: M",-8,0.9,0);
        //drawStrokeText("SCORE : ",2,0,0);

        while(TIME){
            mod=TIME%10;
            number=number*10+mod;
            TIME/=10;
        }
//        float tmp=0;
        //printf("%d\n",number);
        while(number){
            mod=number%10;
            drawStrokeChar(mod+48,4+tmp,0,0);
            number/=10;
            tmp+=0.2;
        }
    }

    else if(START == false and gameOver == true){
        glPushMatrix();
            glTranslated(0,2,0);
            glRotated(aa,0,1,0);
            glScaled(1.5,1.5,1.5);
            glColor3d(1,0,0);
            glutSolidSphere (0.7, 50, 50);
        glPopMatrix();

        glColor3d(0.5,0,1);
        drawStrokeText2("Game Over",-2,0,0);
        drawStrokeText("Press ESC to exit the game",-4,-1,0);
        //drawStrokeText("SCORE: ",-4,-2,0);
        drawStrokeText("Press P to restart the game",-4,-2,0);

    }

    else if(START == false and gameOver == false){

        glPushMatrix();
            glTranslated(0,2,0);
            glRotated(aa,0,1,0);
            glScaled(1.5,1.5,1.5);
            glColor3d(1,0,0);
            glutSolidSphere (0.7, 50, 50);
        glPopMatrix();

        //TIME = number = mod = 0;

        glColor3d(0.5,0,1);
        drawStrokeText2("Moving Ball",-2,0,0);
        drawStrokeText("Press P to play the game",-4,-1,0);
        drawStrokeText("INSTRUCTIONS: ",-4,-2,0);
        drawStrokeText("UP: W, DOWN: S, LEFT: A, RIGHT: D, MAIN MENU: M, Exit: E/Esc",-4,-3,0);
//        drawStrokeText("SCORE :",-4,-4,0);
//        while(number){
//            mod=number%10;
//            drawStrokeChar(mod+48,0,0,0);
//            number/=10;
//            tmp+=0.2;
//        }
         // printf("%d \n",number);
    }

    glutSwapBuffers();
}


static void key(unsigned char key, int x, int y)
{
    float frac = 0.5;
    float rotFrac = 1;
    switch (key)
    {
        case 27 :
        case 'x':
            exit(0);
            break;
        case 'r':
            rot=true;
            break;
        case 't':
            rot=false;
            break;
        case 'z':
            zoom+=0.05;
            break;
        case 'Z':
            zoom-=0.05;
        case 'w':
            tY-=frac;
            rotZ+=rotFrac;
            break;
        case 's':
            tY+=frac;
            rotZ-=rotFrac;
            break;
        case 'a':
            tX+=frac;
            rotX-=rotFrac*3;
            rotY+=rotFrac/2;
            break;
        case 'd':
            tX-=frac;
            rotX+=rotFrac*3;
            rotY-=rotFrac/2;
            break;
        case 'p':
            START=true;
            break;
        case 'm':
            START=false;
            break;
        case 'g':
            START=true;
            gameOver = false;
            //number = 0;mod =0;
            break;
    }

    glutPostRedisplay();
}

static void idle(void)
{
   glutPostRedisplay();
}

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };



int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowPosition(0,0);
	glutInitWindowSize(1366,720);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

    glutCreateWindow("GLUT Shapes");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);


    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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

    glutMainLoop();

    return EXIT_SUCCESS;
}
