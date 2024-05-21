#include "Render.h"




#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "MyShaders.h"

#include "ObjLoader.h"
#include "GUItextRectangle.h"

#include "Texture.h"

GuiTextRectangle rec;

bool textureMode = true;
bool lightMode = true;


//íåáîëüøîé äåôàéí äëÿ óïðîùåíèÿ êîäà
#define POP glPopMatrix()
#define PUSH glPushMatrix()


ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[10];  //ìàññèâ÷èê äëÿ äåñÿòè øåéäåðîâ




//êëàññ äëÿ íàñòðîéêè êàìåðû
class CustomCamera : public Camera
{
public:
	//äèñòàíöèÿ êàìåðû
	double camDist;
	//óãëû ïîâîðîòà êàìåðû
	double fi1, fi2;

	
	//çíà÷íèÿ ìàñåðû ïî óìîë÷àíèþ
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//ñ÷èòàåò ïîçèöèþ êàìåðû, èñõîäÿ èç óãëîâ ïîâîðîòà, âûçûâàåòñÿ äâèæêîì
	virtual void SetUpCamera()
	{

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
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //ñîçäàåì îáúåêò êàìåðû


//êëàññ íåäîäåëàí!
class WASDcamera :public CustomCamera
{
public:
		
	float camSpeed;

	WASDcamera()
	{
		camSpeed = 0.4;
		pos.setCoords(5, 5, 5);
		lookPoint.setCoords(0, 0, 0);
		normal.setCoords(0, 0, 1);
	}

	virtual void SetUpCamera()
	{

		if (OpenGL::isKeyPressed('W'))
		{
			Vector3 forward = (lookPoint - pos).normolize()*camSpeed;
			pos = pos + forward;
			lookPoint = lookPoint + forward;
			
		}
		if (OpenGL::isKeyPressed('S'))
		{
			Vector3 forward = (lookPoint - pos).normolize()*(-camSpeed);
			pos = pos + forward;
			lookPoint = lookPoint + forward;
			
		}

		LookAt();
	}

} WASDcam;


//Êëàññ äëÿ íàñòðîéêè ñâåòà
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//íà÷àëüíàÿ ïîçèöèÿ ñâåòà
		pos = Vector3(-24, 0, 8);
	}

	
	//ðèñóåò ñôåðó è ëèíèè ïîä èñòî÷íèêîì ñâåòà, âûçûâàåòñÿ äâèæêîì
	void  DrawLightGhismo()
	{
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//ëèíèÿ îò èñòî÷íèêà ñâåòà äî îêðóæíîñòè
				glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//ðèñóåì îêðóíîñòü
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}
		/*
		if (f1)
			glEnable(GL_LIGHTING);
		if (f2)
			glEnable(GL_TEXTURE_2D);
		if (f3)
			glEnable(GL_DEPTH_TEST);
			*/
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.5, 0.5, 0.5, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { 1., 1., 1., 0 };
		GLfloat position[] = { pos.X(),pos.Y() , pos.Z(), 1.};

		// ïàðàìåòðû èñòî÷íèêà ñâåòà
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// õàðàêòåðèñòèêè èçëó÷àåìîãî ñâåòà
		// ôîíîâîå îñâåùåíèå (ðàññåÿííûé ñâåò)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// äèôôóçíàÿ ñîñòàâëÿþùàÿ ñâåòà
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// çåðêàëüíî îòðàæàåìàÿ ñîñòàâëÿþùàÿ ñâåòà
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //ñîçäàåì èñòî÷íèê ñâåòà



//ñòàðûå êîîðäèíàòû ìûøè
int mouseX = 0, mouseY = 0;




float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;

//îáðàáîò÷èê äâèæåíèÿ ìûøè
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//ìåíÿåì óãëû êàìåðû ïðè íàæàòîé ëåâîé êíîïêå ìûøè
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;
	}


	
	//äâèãàåì ñâåò ïî ïëîñêîñòè, â òî÷êó ãäå ìûøü
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

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

//îáðàáîò÷èê âðàùåíèÿ êîëåñà  ìûøè
void mouseWheelEvent(OpenGL *ogl, int delta)
{


	float _tmpZ = delta*0.003;
	if (ogl->isKeyPressed('Z'))
		_tmpZ *= 10;
	zoom += 0.2*zoom*_tmpZ;


	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;
}

//îáðàáîò÷èê íàæàòèÿ êíîïîê êëàâèàòóðû
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

	
	if (key == 'Q')
		Time = 0;
}

void keyUpEvent(OpenGL *ogl, int key)
{

}


void DrawQuad()
{
	double A[] = { 0,0 };
	double B[] = { 1,0 };
	double C[] = { 1,1 };
	double D[] = { 0,1 };
	glBegin(GL_QUADS);
	glColor3d(.5, 0, 0);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);
	glEnd();
}


ObjFile car,plane;

Texture carTexture,sythwave;

//âûïîëíÿåòñÿ ïåðåä ïåðâûì ðåíäåðîì
void initRender(OpenGL *ogl)
{

	//íàñòðîéêà òåêñòóð

	//4 áàéòà íà õðàíåíèå ïèêñåëÿ
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//íàñòðîéêà ðåæèìà íàëîæåíèÿ òåêñòóð
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//âêëþ÷àåì òåêñòóðû
	glEnable(GL_TEXTURE_2D);
	
	


	//êàìåðó è ñâåò ïðèâÿçûâàåì ê "äâèæêó"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// íîðìàëèçàöèÿ íîðìàëåé : èõ äëèíû áóäåò ðàâíà 1
	glEnable(GL_NORMALIZE);

	// óñòðàíåíèå ñòóïåí÷àòîñòè äëÿ ëèíèé
	glEnable(GL_LINE_SMOOTH); 


	//   çàäàòü ïàðàìåòðû îñâåùåíèÿ
	//  ïàðàìåòð GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ëèöåâûå è èçíàíî÷íûå ðèñóþòñÿ îäèíàêîâî(ïî óìîë÷àíèþ), 
	//                1 - ëèöåâûå è èçíàíî÷íûå îáðàáàòûâàþòñÿ ðàçíûìè ðåæèìàìè       
	//                ñîîòâåòñòâåííî ëèöåâûì è èçíàíî÷íûì ñâîéñòâàì ìàòåðèàëîâ.    
	//  ïàðàìåòð GL_LIGHT_MODEL_AMBIENT - çàäàòü ôîíîâîå îñâåùåíèå, 
	//                íå çàâèñÿùåå îò ñòî÷íèêîâ
	// ïî óìîë÷àíèþ (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);


	//texture1.loadTextureFromFile("textures\\texture.bmp"); 

	

	s[0].VshaderFileName = "shaders\\v.vert"; //èìÿ ôàéëà âåðøèííîãî øåéäåð
	s[0].FshaderFileName = "shaders\\light.frag"; //èìÿ ôàéëà ôðàãìåíòíîãî øåéäåðà
	s[0].LoadShaderFromFile(); //çàãðóæàåì øåéäåðû èç ôàéëà
	s[0].Compile(); //êîìïèëèðóåì

	s[1].VshaderFileName = "shaders\\v.vert"; //èìÿ ôàéëà âåðøèííîãî øåéäåð
	s[1].FshaderFileName = "shaders\\texture_LightShader.frag"; //èìÿ ôàéëà ôðàãìåíòíîãî øåéäåðà
	s[1].LoadShaderFromFile(); //çàãðóæàåì øåéäåðû èç ôàéëà
	s[1].Compile(); //êîìïèëèðóåì

	

	glActiveTexture(GL_TEXTURE0);
	loadModel("models\\car.obj_m", &car);
	loadModel("models\\plane.obj_m",&plane);
	carTexture.loadTextureFromFile("textures//tex.bmp");
	

	sythwave.loadTextureFromFile("textures//synthwave_grid.bmp");

	tick_n = GetTickCount();
	tick_o = tick_n;

	rec.setSize(300, 100);
	rec.setPosition(10, ogl->getHeight() - 100-10);
	rec.setText("T - âêë/âûêë òåêñòóð\nL - âêë/âûêë îñâåùåíèå\nF - Ñâåò èç êàìåðû\nG - äâèãàòü ñâåò ïî ãîðèçîíòàëè\nG+ËÊÌ äâèãàòü ñâåò ïî âåðòåêàëè",0,0,0);

	
}

double  plane1_X = 0, plane2_X = -24, plane3_X = -48;


void Render(OpenGL *ogl)
{   
	
	tick_o = tick_n;
	tick_n = GetTickCount();
	Time += (tick_n - tick_o) / 1000.0;


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);


	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);


	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	//àëüôàíàëîæåíèå
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//íàñòðîéêà ìàòåðèàëà
	GLfloat amb[] = { 0.5, 0.5, 0.5, 1. };
	GLfloat dif[] = { 0.4, 0.6, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.8f * 256;

	//ôîíîâàÿ
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//äèôóçíàÿ
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//çåðêàëüíàÿ
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//ðàçìåð áëèêà
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//Ïðîãàòü òóò  


	//


	
	plane1_X += 0.05;
	plane2_X += 0.05;
	plane3_X += 0.05;

	if (plane1_X > 30)
		plane1_X -= 72;
	if (plane2_X > 30)
		plane2_X -= 72;
	if (plane3_X > 30)
		plane3_X -= 72;

	Shader::DontUseShaders();	//ëàíäøàôò äâèæóùèéñÿ
	//glActiveTexture(GL_TEXTURE1);
	

	sythwave.bindTexture();
	glPushMatrix();
	glTranslated(plane1_X, 0, 0);
	plane.DrawObj();
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(plane2_X, 0, 0);
	plane.DrawObj();
	glPopMatrix();

	glPushMatrix();
	glTranslated(plane3_X, 0, 0);
	plane.DrawObj();
	glPopMatrix();
	//glActiveTexture(GL_TEXTURE0);
	

	//ìàøèíà

	s[1].UseShader();
	int l = glGetUniformLocationARB(s[1].program,"tex");
	glUniform1iARB(l, 0); 
	int location2 = glGetUniformLocationARB(s[1].program, "light_pos");
	//Øàã 2 - ïåðåäàåì åé çíà÷åíèå
	glUniform3fARB(location2, light.pos.X(), light.pos.Y(), light.pos.Z());

	location2 = glGetUniformLocationARB(s[1].program, "Ia");
	glUniform3fARB(location2, 0.2, 0.2, 0.2);

	location2 = glGetUniformLocationARB(s[1].program, "Id");
	glUniform3fARB(location2, 1.0, 1.0, 1.0);

	location2 = glGetUniformLocationARB(s[1].program, "Is");
	glUniform3fARB(location2, .7, .7, .7);


	location2 = glGetUniformLocationARB(s[1].program, "ma");
	glUniform3fARB(location2, 0.2, 0.2, 0.1);

	location2 = glGetUniformLocationARB(s[1].program, "md");
	glUniform3fARB(location2, 0.4, 0.65, 0.5);

	location2 = glGetUniformLocationARB(s[1].program, "ms");
	glUniform4fARB(location2, 0.9, 0.8, 0.3, 25.6);

	location2 = glGetUniformLocationARB(s[1].program, "camera");
	glUniform3fARB(location2, camera.pos.X(), camera.pos.Y(), camera.pos.Z());
	    //òàê êàê êîãäà ìû çàãðóæàëè òåêñòóðó ãðóçèëè íà GL_TEXTURE0
	glPushMatrix();
	//glRotated(-90, 0, 0, 1);
	carTexture.bindTexture();

	

	car.DrawObj();
	glPopMatrix();

	
	
	

	
	
	
	

	
	Shader::DontUseShaders();

	
	
}   //êîíåö òåëà ôóíêöèè


bool gui_init = false;

//ðèñóåò èíòåðôåéñ, âûçûâåòñÿ ïîñëå îáû÷íîãî ðåíäåðà
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	

	glActiveTexture(GL_TEXTURE0);
	rec.Draw();


		
	Shader::DontUseShaders(); 



	
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

