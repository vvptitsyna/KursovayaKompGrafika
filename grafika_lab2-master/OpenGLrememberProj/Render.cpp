#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

double* Normal(double A[], double B[], double N[])
{
	double NB[] = { B[0] - N[0],B[1] - N[1],B[2] - N[2] };
	double NA[] = { A[0] - N[0],A[1] - N[1],A[2] - N[2] };


	double ax = NB[0], ay = NB[1], az = NB[2];
	double bx = NA[0], by = NA[1], bz = NA[2];


	double* Coord = new double[3];

	Coord[0] = ay * bz - by * az;

	Coord[1] = -ax * bz + bx * az;

	Coord[2] = ax * by - bx * ay;

	double dlina = sqrt(Coord[0] * Coord[0] + Coord[1] * Coord[1] + Coord[2] * Coord[2]);

	Coord[0] /= dlina;

	Coord[1] /= dlina;

	Coord[2] /= dlina;

	return Coord;
}

double FindX(double x0, double R, double degree)
{
	return x0 + R * cos(degree * 0.017);
}

double FindY(double y0, double R, double degree)
{
	return y0 + R * sin(degree * 0.017);
}



void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� ���������� ��������
	glVertex3f(3, 9, 0);	// A
	glVertex3f(2, 8, 0);	// B
	glVertex3f(2, 6, 0);	// C
	glVertex3f(5, 2, 0);	// D
	glVertex3f(6, 5, 0);	// E
	glVertex3f(9, 5, 0);	// F

	double A[] = { 3, 9, 0 };
	double B[] = { 2, 8, 0 };
	double C[] = { 2, 6, 0 };
	double D[] = { 5, 2, 0 };
	double E[] = { 6, 5, 0 };
	double F[] = { 9, 5, 0 };
	
	
	double A1[] = { 3, 9, 3 };
	double B1[] = { 2, 8, 3 };
	double C1[] = { 2, 6, 3 };
	double D1[] = { 5, 2, 3 };
	double E1[] = { 6, 5, 3 };
	double F1[] = { 9, 5, 3 };
	
	double* NormalVect = new double[3];
	NormalVect = Normal(B1,A,A1);
	
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(3, 9, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(3, 9, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 8, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 8, 0);
	glEnd();
	
	NormalVect = Normal(C1, B, B1);
	
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 8, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 8, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 6, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 6, 3);
	glEnd();
	
	NormalVect = Normal(D1, C, C1);
	
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 6, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 6, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(5, 2, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(5, 2, 0);
	glEnd();
	
	NormalVect = Normal(E1, D, D1);
	
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(5, 2, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(5, 2, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(6, 5, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(6, 5, 3);
	glEnd();
	
	NormalVect = Normal(F1, E, E1);
	
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(6, 5, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(6, 5, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(9, 5, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(9, 5, 0);
	glEnd();
	
	NormalVect = Normal(A1, F, F1);
	
	glColor3d(0.2, 0.1, 0.5);
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(9, 5, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(9, 5, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(3, 9, 0);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(3, 9, 3);
	glEnd();
	
	NormalVect = Normal(F1, B1, A1);
	
	glColor3d(0.5, 0.3, 0.5);
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
	glVertex3f(3, 9, 0);
	glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
	glVertex3f(2, 8, 0);
	glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
	glVertex3f(2, 6, 0);
	glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
	glVertex3f(5, 2, 0);
	glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
	glVertex3f(6, 5, 0);
	glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
	glVertex3f(9, 5, 0);
	glEnd();
	
	glBegin(GL_POLYGON);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(3, 9, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 8, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(2, 6, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(5, 2, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(6, 5, 3);
	glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
	glVertex3f(9, 5, 3);
	glEnd();
	
	
	double O[] = { 6,7,0 };
	double O1[] = { 6,7,3 };
	double xprev, yprev, x, y, R = 5, degree = 10;
	
	glColor3d(0.1, 0.6, 0.4);
	glBegin(GL_TRIANGLES);
	while (degree <= 105)
	{
		x = FindX(4.1, R, degree);
		y = FindY(4.1, R, degree);
	
		if (degree == 10)
		{
			xprev = x;
			yprev = y;
		}
	
		glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
		glVertex3dv(O);
		glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
		glVertex3d(xprev, yprev, 0);
		glNormal3d(NormalVect[0], NormalVect[1], -NormalVect[2]);
		glVertex3d(x, y, 0);
	
		glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
		glVertex3dv(O1);
		glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
		glVertex3d(xprev, yprev, 3);
		glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
		glVertex3d(x, y, 3);
	
	
	
		xprev = x;
		yprev = y;
	
		degree += 1;
	}
	glEnd();
	
	glColor3d(0.5, 0.6, 0.4);
	degree = 10;
	double normA[] = { 0,0,0 };
	double normB[] = { 0,0,0 };
	double normN[] = { 0,0,0 };
	
	glBegin(GL_QUADS);
	while (degree <= 105)
	{
		x = FindX(4.1, R, degree);
		y = FindY(4.1, R, degree);
	
		if (degree == 10)
		{
			xprev = x;
			yprev = y;
		}
	
		normN[0] = x;
		normN[1] = y;
		normN[2] = 3;
	
		normA[0] = xprev;
		normA[1] = yprev;
		normA[2] = 3;
	
		normB[0] = x;
		normB[1] = y;
		normB[2] = 0;
	
		NormalVect = Normal(normB, normA, normN);
	
		glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
		glVertex3d(xprev, yprev, 0);
		glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
		glVertex3d(x, y, 0);
		glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
		glVertex3d(x, y, 3);
		glNormal3d(NormalVect[0], NormalVect[1], NormalVect[2]);
		glVertex3d(xprev, yprev, 3);
	
		xprev = x;
		yprev = y;
	
		degree += 1;
	}
	glEnd();


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}