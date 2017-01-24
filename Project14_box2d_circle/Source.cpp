#include <Windows.h>
#include <iostream>
#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#define M_PI 3.14159265358979323846
using namespace std;

bool dyn=true;
GLFWwindow* window;
GLfloat start;
const int WIDTH=640;
const int HEIGHT=480;
b2World* world;
b2Body* body;
b2Vec2 points[4];
const float M2P=60;
const float P2M=1/M2P;
double currentTime,previousTime=0.0,delta;

static void mouse_callback(GLFWwindow* window,int button,int action,int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


GLuint textureID[2];

GLuint loadTextures(const char* filename) {
	unsigned char* image;
	int width, height;
	//generating texture object and attaching to the target
	glGenTextures(1, &textureID[2]);
	glBindTexture(GL_TEXTURE_2D, textureID[2]);
	//wrapping along the s and t axis of texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//filtering so thet the resolution fits to the object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//load the img data
	image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	//free the image memory from GPL
	SOIL_free_image_data(image);
	//unbind the texture
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID[2];
}

/*b2MouseJoint* createJoint(b2Body* body,b2Vec2& point)
{
	static b2Body* groundBody=NULL;
	if(groundBody==NULL){
		b2BodyDef bodydef;
		groundBody = world->CreateBody(&bodydef);
	}
	body->SetAwake(true);
	b2MouseJointDef def;
	def.bodyA=groundBody;
	def.bodyB=body;
	def.target=point;
	def.maxForce=1000.0f*body->GetMass();
	return (world->CreateJoint(&def));
}*/


b2Body* addRect(int x,int y,int w,int h,bool dyns=true)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(x*P2M,y*P2M);
	if(dyns){
		dyn=false;
	bodyDef.type=b2_staticBody;
	}else{
		dyn=true;
		bodyDef.type=b2_dynamicBody;
		
	
	}
	body=world->CreateBody(&bodyDef);
	b2PolygonShape shape;
	shape.SetAsBox(P2M*w/2,P2M*h/2);
	b2FixtureDef fixtureDef;
	fixtureDef.shape=&shape;
	fixtureDef.density=1.0;
	fixtureDef.friction=0.0;//step-1 try giving 0.9 they dont fly usually this is given
	fixtureDef.restitution=0.8;//now it is like a ball it can bounce
	body->CreateFixture(&fixtureDef);
	return body;
}


b2Body* addCircle(int x,int y,int r,bool byns=true)
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(x*P2M,y*P2M);
	if(byns){		
	bodyDef.type=b2_staticBody;
	}else{		
		bodyDef.type=b2_dynamicBody;
		
	}
	
	body=world->CreateBody(&bodyDef);
	b2CircleShape shape;
	shape.m_radius=r*P2M;
	shape.m_p.Set(0,0);
	b2FixtureDef fixtureDef;
	fixtureDef.shape=&shape;
	fixtureDef.density=1.0;
	fixtureDef.friction=0.0;
	fixtureDef.restitution=0.8;
	body->CreateFixture(&fixtureDef);
	return body;
}


void drawCircle(b2Vec2 center,float r,float angle,GLuint textureID1)
{
const int PRECISION=60;
glColor3f(1,1,1);
glPushMatrix();
glTranslatef(center.x*M2P,center.y*M2P,0);
glRotatef(angle*180.0/M_PI,0,0,1);
/*glBegin(GL_TRIANGLE_FAN);
glVertex2f(0,0);
for(float i=0.0;i<=360.0;i+=360.0/PRECISION)
	glVertex2f((cos(i*M_PI/180.0)*r)*M2P,(sin(i*M_PI/180.0)*r)*M2P);
glEnd();*/
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID1);
glBegin(GL_TRIANGLE_FAN);
glVertex2f(0,0);
for (float i=0.0; i<360.0; i+=360.0/PRECISION)
    {
        
float radian = i * (M_PI/180.0f);

float xcos = (float)cos(radian);
float ysin = (float)sin(radian);
float x = (xcos * r)*M2P;
float y = (ysin * r)*M2P;
float tx = xcos * 0.5 + 0.5;
float ty = ysin * 0.5 + 0.5;

glTexCoord2f(tx, ty);
glVertex2f(x, y);
}
glEnd();
//glDisable(GL_TEXTURE_2D);
glPopMatrix();
}

void drawSquare(b2Vec2 points[],b2Vec2 center,float angle,GLuint textureID)
{
	glColor3f(1,1,1);
	glPushMatrix();
	glTranslatef(center.x*M2P,center.y*M2P,0);
	glRotatef(angle*180.0/M_PI,0,0,1);
	
	if(dyn){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,textureID);
	glBegin(GL_QUADS);
	//for(int i=0;i<4;i++)
		glTexCoord2f(250/256,250/256);glVertex2f(points[0].x*M2P,points[0].y*M2P);
		glTexCoord2f(256/256,250/256);glVertex2f(points[1].x*M2P,points[1].y*M2P);
		glTexCoord2f(256/256,256/256);glVertex2f(points[2].x*M2P,points[2].y*M2P);
		glTexCoord2f(250/256,256/256);glVertex2f(points[3].x*M2P,points[3].y*M2P);
	glEnd();
	//glDisable(GL_TEXTURE_2D);
	}else{
		glBegin(GL_QUADS);
		for(int i=0;i<4;i++)
			glVertex2f(points[i].x*M2P,points[i].y*M2P);
		glEnd();
	}

	glPopMatrix();
}

b2Body* myRect;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	b2Body* tmp=world->GetBodyList();

	
	
	while(tmp){
			if((tmp->GetFixtureList()->GetShape()->GetType())==0){
				b2CircleShape* c= (b2CircleShape*)tmp->GetFixtureList()->GetShape();
				
				//drawCircle(tmp->GetWorldCenter(),c->m_radius,tmp->GetAngle(),textureID[1]);
				drawCircle(tmp->GetWorldCenter(),0.4,tmp->GetAngle(),textureID[1]);
			}else{
		for(int i=0;i<4;i++)
			points[i]=((b2PolygonShape*)tmp->GetFixtureList()->GetShape())->GetVertex(i);
			
		drawSquare(points,tmp->GetWorldCenter(),tmp->GetAngle(),textureID[0]);
				
			}
		tmp=tmp->GetNext();
	}
}

void init()
{
	glMatrixMode(GL_PROJECTION);	
	glOrtho(0,WIDTH,HEIGHT,0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0,0,0,1);
	world=new b2World(b2Vec2(0.0,9.8));
	textureID[0]=loadTextures("./images/1.jpg");
	textureID[1]=loadTextures("./images/11.jpg");
	addRect(WIDTH/2,HEIGHT-50,WIDTH,20,true);		
	myRect=addRect(150,300,50,50,false);
}

int main(int argc,char **argv)
{
	glfwInit();
	window=glfwCreateWindow(WIDTH,HEIGHT,"HELLO WORLD",NULL,NULL);
	glfwMakeContextCurrent(window);
	glViewport(0,0,WIDTH,HEIGHT);
	glfwSetMouseButtonCallback(window,mouse_callback);
	glfwSetKeyCallback(window,key_callback);
	init();	
	


	do{		
		display();
		world->Step(1.0/30.0,8,3);
		glfwSwapBuffers(window);
		glfwPollEvents();
		glfwSwapInterval(3);
	}while(!glfwWindowShouldClose(window));
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(key== GLFW_KEY_SPACE && action == GLFW_PRESS){
		double x;
		double y;
		glfwGetCursorPos(window,&x,&y);
		addCircle(x,y,15,false);
		b2Vec2 vel = myRect->GetLinearVelocity();
		vel.y = 10;//upwards - don't change x velocity
		myRect->SetLinearVelocity( vel );
		//myRect->ApplyLinearImpulse(b2Vec2(10,0),myRect->GetWorldPoint(points[1]),false);
	}
}

static void mouse_callback(GLFWwindow* window,int button,int action,int mods)
{
	
	if(button==GLFW_MOUSE_BUTTON_RIGHT && action==GLFW_PRESS)
	{
		double x;
		double y;
		glfwGetCursorPos(window,&x,&y);
		addRect(x,y,40,40,false);
	}
	/*if(button==GLFW_MOUSE_BUTTON_LEFT && action==GLFW_PRESS)
	{
		double x;
		double y;
		glfwGetCursorPos(window,&x,&y);
		addCircle(x,y,15,false);
	}*/
}