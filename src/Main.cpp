#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor, programTexture, programSun;

Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;

glm::vec3 cameraPos = glm::vec3(40, 0, 25);
glm::vec3 cameraDir;	// Wektor "do przodu" kamery
glm::vec3 cameraSide;	// Wektor "w bok" kamery

float differenceX = 0;
float prevX = 0;
float differenceY = 0;
float prevY = 0;

glm::mat4 cameraMatrix, perspectiveMatrix;
glm::vec3 lightDir = glm::normalize(glm::vec3(0,1,0));
glm::quat rotation = glm::quat(1, 0, 0, 0);

glm::vec3 lightPos = glm::vec3(0, 0, 0);

GLuint textureShip, textureSun, texturePlanet1, texturePlanet2, texturePlanet3, texturePlanet4, texturePlanet5;
GLuint normalTextureP1, normalTextureP2, normalTextureP3, normalTextureP4, normalTextureP5;

void keyboard(unsigned char key, int x, int y)
{
	
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	case 'z': rotation = glm::angleAxis(angleSpeed, glm::vec3(0, 0, 1)) * rotation; break;
	case 'x': rotation = glm::angleAxis(angleSpeed, glm::vec3(0, 0, -1)) * rotation; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	}
}

void mouse(int x, int y)
{
	differenceX = (x - prevX) * 0.05;
	prevX = x;

	differenceY = (y - prevY) * 0.05;
	prevY = y;
}

glm::mat4 createCameraMatrix()
{
	glm::quat quatX = glm::angleAxis(differenceX, glm::vec3(0, 1, 0));
	glm::quat quatY = glm::angleAxis(differenceY, glm::vec3(1, 0, 0));

	differenceX = 0;
	differenceY = 0;

	glm::quat rotationChange = glm::normalize(quatX * quatY);
	rotation = rotationChange * rotation;

	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos,rotation);
}


void drawObject(GLuint program, obj::Model * model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalmapId)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);	
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::DrawModel(model);
	glUseProgram(0);
}


void renderScene()
{
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.1f, 0.3f, 1.0f);

	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(-2.5,-3,-5)) * glm::rotate(glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.15f));	
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * shipInitialTransformation;

	// statek
	drawObject(programColor, &shipModel, shipModelMatrix, textureShip, 0);
	// TODO: statek ma byc zrodlem swiatla

	// s³oñce
	drawObject(programSun, &sphereModel, glm::translate(lightPos) * glm::scale(glm::vec3(20)), textureSun, 0);
	// 1 planeta
	drawObject(programTexture, &sphereModel, glm::translate(glm::vec3(30,0,0)) * scale(glm::vec3(3)), texturePlanet1, normalTextureP1);
	// 2 planeta
	drawObject(programTexture, &sphereModel, glm::translate(glm::vec3(50, 0, 0)) * scale(glm::vec3(5)), texturePlanet2, normalTextureP2);
	// 3 planeta
	drawObject(programTexture, &sphereModel, glm::translate(glm::vec3(80, 0, 0)) * scale(glm::vec3(5)), texturePlanet3, normalTextureP3);
	// ksiezyce 3 planety

	//  glm::eulerAngleY(time / 2)  <-- obraca sie dookola slonca
	drawObject(programTexture, &sphereModel, glm::translate(glm::vec3(70, 0, 0)) * scale(glm::vec3(1.5)), texturePlanet4, normalTextureP4);
	drawObject(programTexture, &sphereModel, glm::translate(glm::vec3(70, 0, 10)) * scale(glm::vec3(1.5)), texturePlanet5, normalTextureP5);

	glutSwapBuffers();
}


void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);

	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");	// <-- swiatlo sie odbija, tekstury dzialaja, normal mapping nie dziala 
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");		// <--  normal mapping i tekstury dzialaja, zle odbija swiatlo
	programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");

	textureShip = Core::LoadTexture("textures/ship.png");
	textureSun = Core::LoadTexture("textures/sun.png");

	texturePlanet1 = Core::LoadTexture("textures/planet1.png");
	texturePlanet2 = Core::LoadTexture("textures/planet2.png");
	texturePlanet3 = Core::LoadTexture("textures/planet3.png");
	texturePlanet4 = Core::LoadTexture("textures/planet4.png");
	texturePlanet5 = Core::LoadTexture("textures/planet5.png");

	normalTextureP1 = Core::LoadTexture("textures/planet1_norm.png");
	normalTextureP2 = Core::LoadTexture("textures/planet2_norm.png");
	normalTextureP3 = Core::LoadTexture("textures/planet3_norm.png");
	normalTextureP4 = Core::LoadTexture("textures/planet4_norm.png");
	normalTextureP5 = Core::LoadTexture("textures/planet5_norm.png");
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
	shaderLoader.DeleteProgram(programSun);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Project");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
