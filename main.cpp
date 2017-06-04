// glutEx1.cpp : 定义控制台应用程序的入口点。
//


#include <stdlib.h>
#include "glut.h"
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#define BITMAP_ID 0x4D42
#define TEXH   128
#define TEXW   128
float fTranslate;
float fRotate;
float fScale = 1.0f;	// set inital scale value to 1.0f


int wHeight = 0;
int wWidth = 0;

void Draw_Leg();
GLuint texture[6];
bool btex[6] = { false, false, false, false, false, false };

typedef struct Point_* Point;
struct Point_ {
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
typedef struct Box_* Box;
struct Box_ {
	Point p[8];
};

// 纹理标示符数组，保存两个纹理的标示符
// 描述: 通过指针，返回filename 指定的bitmap文件中数据。
// 同时也返回bitmap信息头.（不支持-bit位图）
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;	// 文件指针
	BITMAPFILEHEADER bitmapFileHeader;	// bitmap文件头
	unsigned char	*bitmapImage;		// bitmap图像数据
	int	imageIdx = 0;		// 图像位置索引
	unsigned char	tempRGB;	// 交换变量

								// 以“二进制+读”模式打开文件filename 
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL) return NULL;
	// 读入bitmap文件图
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	// 验证是否为bitmap文件
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fprintf(stderr, "Error in LoadBitmapFile: the file is not a bitmap file\n");
		return NULL;
	}

	// 读入bitmap信息头
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	// 将文件指针移至bitmap数据
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
	// 为装载图像数据创建足够的内存
	bitmapImage = new unsigned char[bitmapInfoHeader->biSizeImage];
	// 验证内存是否创建成功
	if (!bitmapImage) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	// 读入bitmap图像数据
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);
	// 确认读入成功
	if (bitmapImage == NULL) {
		fprintf(stderr, "Error in LoadBitmapFile: memory error\n");
		return NULL;
	}

	//由于bitmap中保存的格式是BGR，下面交换R和B的值，得到RGB格式
	for (imageIdx = 0;
		imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}
	// 关闭bitmap图像文件
	fclose(filePtr);
	return bitmapImage;
}
unsigned char * RedBlackBlocks()
{
	unsigned char * blocks = new unsigned char [TEXW * TEXH * 3];
	int i, j;
	for (i = 0; i < TEXH; i++)
	{
		for (j = 0; j < TEXW; j++)
		{
			if (((j / 16) % 2) == ((i / 16) % 2))
			{
				blocks[i*TEXW * 3 + j * 3] = 153;
				blocks[i*TEXW * 3 + j * 3 + 1] = 15;
				blocks[i*TEXW * 3 + j * 3 + 2] = 30;
			}
			else
			{
				blocks[i*TEXW *3+ j*3] = 0;
				blocks[i*TEXW * 3 + j * 3 + 1] = 0;
				blocks[i*TEXW * 3 + j * 3 + 2] = 0;
			}
		}
	}

	return blocks;
}
void texload(int i, char *filename)
{
	int count = 0;
	BITMAPINFOHEADER bitmapInfoHeader;                                 // bitmap信息头
	unsigned char*   bitmapData;                                       // 纹理数据

	bitmapData = LoadBitmapFile(filename, &bitmapInfoHeader);
	glBindTexture(GL_TEXTURE_2D, texture[i]);
	// 指定当前纹理的放大/缩小过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//printf("%d %d\n",bitmapInfoHeader.biHeight, bitmapInfoHeader.biWidth);
	glTexImage2D(GL_TEXTURE_2D,
		0, 	    //mipmap层次(通常为，表示最上层) 
		GL_RGB,	//我们希望该纹理有红、绿、蓝数据
		bitmapInfoHeader.biWidth, //纹理宽带，必须是n，若有边框+2 
		bitmapInfoHeader.biHeight, //纹理高度，必须是n，若有边框+2 
		0, //边框(0=无边框, 1=有边框) 
		GL_RGB,	//bitmap数据的格式
		GL_UNSIGNED_BYTE, //每个颜色数据的类型
		bitmapData);	//bitmap数据指针  
	
}


void init()
{
	glGenTextures(3, texture);                                         // 第一参数是需要生成标示符的个数, 第二参数是返回标示符的数组
	texload(0, "Monet.bmp");
	texload(1, "Crack.bmp");
	texloat(3, "Spot.bmp");
	//下面生成自定义纹理
	unsigned char* tex = RedBlackBlocks(); //参考opengl red book，理解后请解释函数的步骤。
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //设置像素存储模式控制所读取的图像数据的行对齐方式.
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TEXW, TEXH, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

}
void fillTop(GLuint texture, Point p0, Point p1, Point p2, Point p3)
{
	printf("Get in f2\n");

	glBindTexture(GL_TEXTURE_2D, texture);
	//glRotatef(90, 1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3i(p0->x, p0->y, p0->z);
	glTexCoord2i(1, 0); glVertex3i(p1->x, p1->y, p1->z);
	glTexCoord2i(0, 0); glVertex3i(p2->x, p2->y, p2->z);
	glTexCoord2i(0, 1); glVertex3i(p3->x, p3->y, p3->z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void fillRect(GLuint texture, Point p0, Point p1, Point p2, Point p3)
{
	printf("Get in f2\n");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	//glRotatef(90, 1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2i(1, 1); glVertex3i(p0->x, p0->y, p0->z);
	glTexCoord2i(1, 0); glVertex3i(p1->x, p1->y, p1->z);
	glTexCoord2i(0, 0); glVertex3i(p2->x, p2->y, p2->z);
	glTexCoord2i(0, 1); glVertex3i(p3->x, p3->y, p3->z);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

// 
void fillSolidCube(GLuint texture, Box b, bool top)
{
	printf("Get in f1\n");
	fillRect(texture, b->p[3], b->p[7], b->p[5], b->p[1]);
	if(top)
		fillTop(texture, b->p[0], b->p[2], b->p[3], b->p[1]);
	else {
		fillRect(texture, b->p[0], b->p[2], b->p[3], b->p[1]);
	}
	fillRect(texture, b->p[4], b->p[6], b->p[2], b->p[0]);
	fillRect(texture, b->p[4], b->p[5], b->p[1], b->p[0]);
	fillRect(texture, b->p[4], b->p[6], b->p[7], b->p[5]);
	fillRect(texture, b->p[2], b->p[6], b->p[7], b->p[3]);
}

bool bPersp = true;
bool bAnim = false;
bool bWire = false;
bool bLight = false;
bool bPos = true;
bool bLight0 = true;
bool bx = false;
bool by = false;
bool bz = false;

void Draw_Leg();
Box b[2];
void Draw_Triangle() // This function draws a triangle with RGB colors
{
	
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 2);
	/*GLfloat teaPot[] = { 0.85f,0.698f,0.45f,1 };
	GLfloat s_teaPot[] = { 0.85f / 2.0f , 0.698f / 2.0f,0.45f / 2.0f,1 };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, teaPot);
	glMaterialfv(GL_FRONT, GL_SPECULAR, s_teaPot);*/
	if (btex[0])
	{
		glEnable(GL_TEXTURE_2D);
		if (btex[3])
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		else
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
	}
	else if (btex[2])
	{
		glEnable(GL_TEXTURE_2D);
		if (btex[3])
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		else
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
	}
	glPushMatrix();
	glTranslatef(0, 0, 4 + 1);
	glRotatef(90, 1, 0, 0);
	glutSolidTeapot(1);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	if (btex[1])
	{
		b[0] = new Box_;
		int i;
		for (i = 0; i < 8; i++)
		{
			b[0]->p[i] = new Point_;
		}
		b[0]->p[0]->x = -3.0f;
		b[0]->p[0]->y = -2.6f;
		b[0]->p[0]->z = 4.6f;
		b[0]->p[1]->x = -3.0f;
		b[0]->p[1]->y = 2.6f;
		b[0]->p[1]->z = 4.6f;
		b[0]->p[2]->x = 3.0f;
		b[0]->p[2]->y = -2.6f;
		b[0]->p[2]->z = 4.6f;
		b[0]->p[3]->x = 3.0f;
		b[0]->p[3]->y = 2.6f;
		b[0]->p[3]->z = 4.6f;
		b[0]->p[4]->x = -3.0f;
		b[0]->p[4]->y = -2.6f;
		b[0]->p[4]->z = 3.4f;
		b[0]->p[5]->x = -3.0f;
		b[0]->p[5]->y = 2.6f;
		b[0]->p[5]->z = 3.4f;
		b[0]->p[6]->x = 3.0f;
		b[0]->p[6]->y = -2.6f;
		b[0]->p[6]->z = 3.4f;
		b[0]->p[7]->x = 3.0f;
		b[0]->p[7]->y = 2.6f;
		b[0]->p[7]->z = 3.4f;
		fillSolidCube(texture[1], b[0], true);
	}
	else {
		glPushMatrix();
		glTranslatef(0, 0, 3.5);
		glScalef(5, 4, 1);
		glutSolidCube(1.0);
		glPopMatrix();
	}
	

	glPushMatrix();
	glTranslatef(1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, 1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.5, -1, 1.5);
	Draw_Leg();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1.5, -1, 1.5);
	Draw_Leg();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	//delete[] b;
}


void Draw_Leg()
{

	glScalef(0.5f, 0.5f, 1.5f);
	if (btex[1])
	{
		b[1]= new Box_;
		int i;
		for (i = 0; i < 8; i++)
		{
			b[1]->p[i] = new Point_;
		}
		b[1]->p[0]->x = -1;
		b[1]->p[0]->y = -1;
		b[1]->p[0]->z = 1;
		b[1]->p[1]->x = -1;
		b[1]->p[1]->y = 1;
		b[1]->p[1]->z = 1;
		b[1]->p[2]->x = 1;
		b[1]->p[2]->y = -1;
		b[1]->p[2]->z = 1;
		b[1]->p[3]->x = 1;
		b[1]->p[3]->y = 1;
		b[1]->p[3]->z = 1;
		b[1]->p[4]->x = -1;
		b[1]->p[4]->y = -1;
		b[1]->p[4]->z = -1;
		b[1]->p[5]->x = -1;
		b[1]->p[5]->y = 1;
		b[1]->p[5]->z = -1;
		b[1]->p[6]->x = 1;
		b[1]->p[6]->y = -1;
		b[1]->p[6]->z = -1;
		b[1]->p[7]->x = 1;
		b[1]->p[7]->y = 1;
		b[1]->p[7]->z = -1;
		fillSolidCube(texture[1], b[1], false);
	}
	else	
		glutSolidCube(2.0);
}

void updateView(int width, int height)
{
	glViewport(0, 0, width, height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	float whRatio = (GLfloat)width / (GLfloat)height;
	if (bPersp) {
		gluPerspective(45.0f, whRatio, 1.5f, 10.0f);
		//glFrustum(-3, 3, -3, 3, 3,100);
	}
	else {
		glOrtho(-3, 3, -3, 3, -100, 100);
	}

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

void reshape(int width, int height)
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}

	wHeight = height;
	wWidth = width;

	updateView(wHeight, wWidth);
}

void idle()
{
	glutPostRedisplay();
}

float eye[] = { 0, 0, 8 };
float center[] = { 0, 0, 0 };
GLfloat thieta = 10.0f;
GLfloat position[] = { -1,1,0,1 };
GLfloat lightDir[] = { 1,-1,0 };
void key(unsigned char k, int x, int y)
{
	switch (k)
	{
	case 27:
	case 'q': {exit(0); break; }
	case 'p': {bPersp = !bPersp; break; }

	case ' ': {bAnim = !bAnim; break; }
	case 'o': {bWire = !bWire; break; }

	case 'a': {
		eye[0] -= 0.2f;
		center[0] -= 0.2f;
		break;
	}
	case 'd': {
		eye[0] += 0.2f;
		center[0] += 0.2f;
		break;
	}
	case 'w': {
		eye[1] += 0.2f;
		center[1] += 0.2f;
		break;
	}
	case 's': {
		eye[1] -= 0.2f;
		center[1] -= 0.2f;
		break;

	}
	case 'z': {
		eye[2] -= 0.2f;
		center[2] -= 0.2f;
		break;
	}
	case 'c': {
		eye[2] += 0.2f;
		center[2] += 0.2f;
		break;
	}
	
	case 'u': {
		bLight = !bLight;
		break;
	}
	case '2':
	{
		bLight0 = !bLight0;
		break;
	}
	case '3':
	{
		btex[0] = !btex[0];
		break;
	}
	case '4':
	{
		btex[1] = !btex[1];
		break;
	}
	case '5':
	{
		btex[2] = !btex[2];
		break;
	}
	case 'e':
	{
		btex[3] = !btex[3];
	}
	case '1': {
		bPos = false;
		break;
	}

	}
	updateView(wHeight, wWidth);
}


void redraw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									// Reset The Current Modelview Matrix

	gluLookAt(eye[0], eye[1], eye[2],
		center[0], center[1], center[2],
		0, 1, 0);				// 场景（0，0，0）的视点中心 (0,5,50)，Y轴向上

	if (bWire) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_poss[] = { 5,5,5,1 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_poss);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glEnable(GL_LIGHT0);

	//	glTranslatef(0.0f, 0.0f,-6.0f);			// Place the triangle at Center
	glRotatef(fRotate, 0, 1.0f, 0);			// Rotate around Y axis
	glRotatef(-90, 1, 0, 0);
	glScalef(0.2, 0.2, 0.2);
	Draw_Triangle();						// Draw triangle

	if (bAnim) fRotate += 0.05f;
	if (!bPos) {
		if (eye[0] > 0 && !bx) {
			center[0] -= 0.004;
			eye[0] -= 0.004;
		}
		else if (eye[0] < -0.2)
		{
			eye[0] += 0.004;
			center[0] += 0.004;
		}
		else
			bx = true;

		if (eye[1] > 1 && !by) {
			center[1] -= 0.004;
			eye[1] -= 0.004;
		}
		else if (eye[1] < 0.8)
		{
			eye[1] += 0.004;
			center[1] += 0.004;
		}
		else
			by = true;

		if (eye[2] > 3 && !bz) {
			center[2] -= 0.004;
			eye[2] -= 0.004;
		}
		else if (eye[2] < 2.8)
		{
			eye[2] += 0.004;
			center[2] += 0.004;
		}
		else
			bz = true;

		if (bx && by && bz)
		{
			bz = bx = by = false;
			bPos = true;
		}

	}
	glutSwapBuffers();
}
int mouseX, mouseY;
bool mouseLeftDown;

void mouseCB(int button, int state, int x, int y) {
	mouseX = x;
	mouseY = y;
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}

}
float theta[2] = { 0,0 };
void mouseMotionCB(int x, int y) {
	if (mouseLeftDown) {
		theta[0] -= (float)(mouseX - x) / 500.0f;
		theta[1] += (float)(mouseY - y) / 500.0f;

		printf("%lf \n", theta[0]);
		center[0] = eye[0] + 8 * cos(theta[1]) * sin(theta[0]);
		center[1] = eye[1] + 8 * sin(theta[1]);
		center[2] = eye[2] - 8 * cos(theta[1])*cos(theta[0]);
		printf("%lf %lf %lf\n", center[0], center[1], center[2]);

		mouseX = x;
		mouseY = y;

	}

	glutPostRedisplay();
}
int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(480, 480);
	int windowHandle = glutCreateWindow("Simple GLUT App");
	init();
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);
	glutDisplayFunc(redraw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}



