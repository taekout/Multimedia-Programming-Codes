#include <gl/glut.h>
#include <math.h>

/*scalex - scaling of sphere around x-axis
scaley - scaling of sphere around y-axis
r - radius of sphere
*/


class MyVector4
{
public:
	float p[4];
	MyVector4()
	{
		p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 1.0;
	}
	MyVector4(float x, float y, float z)
	{
		p[0] = x; p[1] = y; p[2] = z; p[3] = 1.0;
	}
};

class MyMat4x4
{
public:
	float p[4][4];		//	00 01 02 03
						//  10 11 12 13
						//  20 21 22 23
						//  30 31 32 33
	MyMat4x4()
	{
		SetIdentity();
	}
	void SetIdentity()
	{
		for(int i=0; i<4; i++)
			for(int j=0; j<4; j++)
				p[i][j] = 0.0;
		p[0][0] = 1;	p[1][1] = 1;
		p[2][2] = 1;	p[3][3] = 1;
	}
	void RotateX(float theta)	
	{
		SetIdentity();
		float rad = theta * 3.141592/180.0f;
		p[1][1] = cos(rad);
		p[1][2] = -sin(rad);
		p[2][1] = sin(rad);
		p[2][2] = cos(rad);
	}
	void RotateZ(float theta)	
	{
		SetIdentity();
		float rad = theta * 3.141592/180.0f;
		p[0][0] = cos(rad);
		p[0][1] = -sin(rad);
		p[1][0] = sin(rad);
		p[1][1] = cos(rad);
	}
	void RotateY(float theta)	
	{
		SetIdentity();
		float rad = theta * 3.141592/180.0f;
		p[0][0] = cos(rad);
		p[0][2] = sin(rad);
		p[2][0] = -sin(rad);
		p[2][2] = cos(rad);
	}
	void Translate(float x, float y, float z)
	{
		SetIdentity();
		p[0][3] = x;
		p[1][3] = y;
		p[2][3] = z;
	}
	MyVector4 operator * (MyVector4 in)
	{
		MyVector4 out;
		for(int j=0; j<4; j++)
		{
			out.p[j] = 0.0;
			for(int i=0; i<4; i++)
				out.p[j] += p[j][i]*in.p[i]; 
		}
		return out;
	}

	MyMat4x4 operator * (MyMat4x4 in)
	{
		MyMat4x4 out;
		for(int i=0; i<4; i++)
			for(int j=0; j<4; j++)
			{
				out.p[i][j] = 0;
				for(int k=0; k<4; k++)
					out.p[i][j]+=
						p[i][k]*in.p[k][j];
			}
		
		return out;
	}

};

float g_aspect;


#define M_PI 3.141592

void drawQuarterSphere(int scaley, int scalex, GLfloat r)//, MyMat4x4 multM)
{
	int i, j;
	float **v;
	v = new float*[scalex * scaley];
	for(int i = 0 ; i < scalex * scaley ; i++)
	{
		v[i] = new float[3];
	}
	//GLfloat v[scalex*scaley][3];

	for (i=0; i<scalex; ++i) {
		for (j=0; j<scaley; ++j) {
			v[i*scaley+j][0]=r*cos(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));
			v[i*scaley+j][1]=r*sin(i*M_PI/(2*scalex));
			v[i*scaley+j][2]=r*sin(j*2*M_PI/scaley)*cos(i*M_PI/(2*scalex));
		}
	}

	glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(3, 0, 0);
		glColor3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 3, 0);
		glColor3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 3);
	glEnd();
	glBegin(GL_QUADS);
	for (i=0; i<scalex-1; ++i) {
		for (j=0; j<scaley/2; ++j) {
			glColor3f(0, 0.3, 0.5);
			glVertex3fv(v[i*scaley+j]);
			glColor3f(1, 0.2, 0.1);
			glVertex3fv(v[i*scaley+(j+1)%scaley]);
			glVertex3fv(v[(i+1)*scaley+(j+1)%scaley]);
			glVertex3fv(v[(i+1)*scaley+j]);
		}
	}
	glEnd();
}


void MyInit()
{
	glClearColor(0,0,0,0);

	// Somewhere in the initialization part of your programâ¦
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
 	glEnable(GL_DEPTH_TEST);
 	glDepthFunc(GL_LESS);

	// Create light components
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8, 1.0f };
	GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { -1.5f, 1.0f, -5.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_COLOR_MATERIAL);
}


void MyReshape(int w, int h)
{
	glViewport(0,0,w,h);
	g_aspect = float(w)/float(h);
}


void DrawSeashell(int nShells);
void MySphere(float r, int div);
void MyDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	////////////////////////////////////////////////
	//	parallel projection

	//	glOrtho(-1,1,-1/g_aspect, 1/g_aspect, 1, 10);

	////////////////////////////////////////////////
	//	Perspective projection by using glFrustum

	//	float size = 1.0f;
	//	glFrustum(-size, size, -size/g_aspect, size/g_aspect, 1, 10);	

	////////////////////////////////////////////////
	//	Perspective projection by using glPerspective

	float angle = 30.0f;
	gluPerspective(angle, g_aspect, 0.1, 10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(-5,-5,5, 0,0,0, 0,1,0);

	//DrawSeashell(2);
	glTranslatef(1,0,0);
	GLUquadric *qobj = gluNewQuadric();
	gluCylinder(qobj, 1.0, 1.0, 2.0, 5, 5);

	glutSwapBuffers();
	glFlush();
}

void MyIdle()
{
	//	g_time += g_dt;

	glutPostRedisplay();
}

void MyKeyboard(unsigned char key,int x , int y)
{
	switch(key)
	{
	case 27 : exit(0);
	}
}

int main()
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(400,300);
	glutInitWindowPosition(100,100);
	glutCreateWindow("3D Perspective Viewing");

	MyInit();
	glutReshapeFunc(MyReshape);
	glutIdleFunc(MyIdle);
	glutDisplayFunc(MyDraw);
	glutKeyboardFunc(MyKeyboard);

	glutMainLoop();

	return 0;
}

void CalFibonacci(GLfloat *fibonacci);

GLfloat fibonacci_series[500] = {};
void DrawSeashell(int nShells)
{
	fibonacci_series[0] = 1.0f;
	fibonacci_series[1] = 1.0f;
	CalFibonacci(fibonacci_series);
 	glRotatef(+90, 0, 1, 0);
	drawQuarterSphere(24, 24, 1);
	// later glRotatef ==> replaced -> drawQuarterSphere(24, 24, 1 , RotationMatrix); ==> Inside , multiply with the M then save it in list<PLANE>

	MyMat4x4 wholeMat;
	int nSignCount = 0;
	int curSign = -1;
	for(int i = 2 ; i < nShells ; i++)
	{
		MyMat4x4 r_mat;
		r_mat.RotateX(-90);
		MyMat4x4 t_mat;
		if(nSignCount == 2)
		{
			curSign *= -1;
			nSignCount = 0;
		}
		nSignCount ++;
		float trans = fibonacci_series[i] - fibonacci_series[i-1];
		trans *= curSign;
		if(i % 2 == 0) // even
		{
			t_mat.Translate(0, 0, trans);
			glRotatef(-90, 1, 0, 0);
			glTranslatef(0, 0, trans);
		}
		else	// odd
		{
			t_mat.Translate(0, trans, 0);
			glRotatef(-90, 1, 0, 0);
			glTranslatef(0, 0, trans);
			// !!!!!!!! I think that when I use matrix to rotate the obj, the axis does not change.
			// However, when I use Rotatef, I think openGL changes the axis. That is kind of obvious. so 
			// I should do glTranslatef(0, 0, trans) consistently, and then when I change it to matrix ways,
			// I should use Matrix.Translate(0, trans, 0) instead of M.translate(0, 0, trans). I am not very sure about this.
		}
		wholeMat = t_mat * r_mat * wholeMat;
		drawQuarterSphere(24, 24, fibonacci_series[i]);
	}
}

void CalFibonacci(GLfloat *fibonacci)
{
	for(int i = 2 ; i < 500 ; i++)
	{
		fibonacci[i] = fibonacci[i-1] + fibonacci[i-2];
	}
}


void MySphere(float r, int div)
{
	if(div<2) return;

	float ** v;
	int num_vertex = (div-1)*div + 2;
	v = new float * [num_vertex];
	int i,j;
	for(i=0; i<num_vertex; i++)
		v[i] = new float[3];

	int ** f;
	int num_face = div*2 + div*2*(div-1);
	f = new int * [num_face];
	for(i=0; i<num_face; i++)
		f[i] = new int[3];


	int vcount = 0;
	v[vcount][0] = r;
	v[vcount][1] = 0;
	v[vcount][2] = 0;
	vcount++;

	float dt1 = 180.0f/div* 3.141592/180.0f;
	float dt2 = 360.0f/div* 3.141592/180.0f;

	for(int i=1; i<div; i++)
	{
		float th1 = dt1*i;
		float x = r*cos(th1);
		float r1 = r*sin(th1);

		for(int j=0; j<div; j++)
		{
			float th2 = dt2*j;
			float y = r1*cos(th2);
			float z = r1*sin(th2);
			v[vcount][0] = x;
			v[vcount][1] = y;
			v[vcount][2] = z;
			vcount ++;
		}
	}

	v[vcount][0] = -r;
	v[vcount][1] = 0;
	v[vcount][2] = 0;
	vcount++;

//	glBegin(GL_POINTS);
//	for(i=0; i<num_vertex; i++)
//		glVertex3fv(v[i]);
//	glEnd();


	int fcount = 0;
	for(int i=0; i<div; i++)
	{
		f[fcount][0] = 0;
		f[fcount][1] = i+1;
		if(i<div-1)
			f[fcount][2] = i+2;
		else
			f[fcount][2] = 1;
		fcount++;
	}

	for(int i=0; i<div-2; i++)
	{
		int v_st = div*i + 1;

		for(int j=0; j<div; j++)
		{
			if(j<div-1)
			{
				f[fcount][0] = v_st+j;
				f[fcount][1] = v_st+div+j;
				f[fcount][2] = v_st+div+j+1;
				fcount ++;
				f[fcount][0] = v_st+div+j+1;
				f[fcount][1] = v_st+j+1;
				f[fcount][2] = v_st+j;
				fcount ++;
			}
			else
			{
				f[fcount][0] = v_st+j;
				f[fcount][1] = v_st+div+j;
				f[fcount][2] = v_st+div;
				fcount ++;
				f[fcount][0] = v_st+div;
				f[fcount][1] = v_st;
				f[fcount][2] = v_st+j;
				fcount ++;
			}
		}
	}

	int v_st = div*(div-2)+1;
	for(int i=0; i<div; i++)
	{
		f[fcount][0] = v_st+i;
		f[fcount][1] = div*(div-1)+1;
		if(i<div-1)
			f[fcount][2] = v_st+i+1;
		else
			f[fcount][2] = v_st;
		fcount++;
	}

	//glRotatef(90, 0, 1, 0);
	glBegin(GL_TRIANGLES);
	for(i=0; i<fcount/2; i++)
	{
		MyMat4x4 myMat;
		myMat.RotateY(180);
		MyVector4 vec1, vec2, vec3;
		vec1.p[0] = v[f[i][0]][0];
		vec1.p[1] = v[f[i][0]][1];
		vec1.p[2] = v[f[i][0]][2];
		vec2.p[0] = v[f[i][1]][0];
		vec2.p[1] = v[f[i][1]][1];
		vec2.p[2] = v[f[i][1]][2];
		vec3.p[0] = v[f[i][2]][0];
		vec3.p[1] = v[f[i][2]][1];
		vec3.p[2] = v[f[i][2]][2];
		MyVector4 pt1 = myMat * vec1;
		MyVector4 pt2 = myMat * vec2;
		MyVector4 pt3 = myMat * vec3;
		glVertex3fv(pt1.p);
		glVertex3fv(pt2.p);
		glVertex3fv(pt3.p);
	}
	glEnd();


	for(i=0; i<num_vertex; i++)
		delete [] v[i];
	delete [] v;

	for(i=0; i<num_face; i++)
		delete [] f[i];
	delete [] f;


}

/*

void CreateNFF(string filename, MyVector4 *flowerbase_left,
			   MyVector4 *flowerbase_right, MyVector4 *flower_pole)
{
	ofstream out;
	out.open(filename.c_str());
	out << "b" << " " << "0.078 " << "0.361 " << "0.753" << endl;
	out << "v" << endl;
	out << "from 0 0 0" << endl;
	out << "at 0 0 1" << endl;
	out << "up 0 1 0" << endl;
	out << "angle 45" << endl;
	out << "hither 0.05" << endl;
	out << "resolution 512 512" << endl;
	out << "l 2 28 -5" << endl;
	out << "f 1 0.53 0 1 0 100000 0 0" << endl;
	// Now polygons
	// Flower Pole First.
	out << "p 3" << endl;
	for(int i = 0 ; i < 3 ; i++)
	{
		out << flower_pole[i].p[0] << " " << flower_pole[i].p[1] << " " 
			<< flower_pole[i].p[2] << endl;
	}
	out << "p 3" << endl;
	for(int i = 3 ; i < 6 ; i++)
	{
		out << flower_pole[i].p[0] << " " << flower_pole[i].p[1] << " " 
			<< flower_pole[i].p[2] << endl;
	}
	// Center base of Daisy flower first.
	for(int i = 0 ; i < 8 ; i++)
	{
		out << "p 3" << endl;
		out << "0 0 0" << endl; // vertex 1 - always 0,0,0
		out << flowerbase_left[i].p[0] << " " << flowerbase_left[i].p[1] << " " << flowerbase_left[i].p[2] << " " << endl;
		out << flowerbase_right[i].p[0] << " " << flowerbase_right[i].p[1] << " " << flowerbase_right[i].p[2] << " " << endl;
	}
	// Draw Information for the Flower Center Circle
	out << "s 0 0 0.01 0.07" << endl;
	out << "s 0 0 -0.01 0.07" << endl;
}
*/
