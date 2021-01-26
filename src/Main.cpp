#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <glm.hpp>
#include "gtc/type_ptr.hpp"
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#include "SkyBox.cpp"
#include <vector>
#include <string>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint programColor, programTexture, programSun, programSkyBox;

GLuint cubemapTexture;

Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sphereModel;

glm::vec3 randomPosition[10];
glm::vec3 cameraPos = glm::vec3(0, 0, 25);
glm::vec3 cameraDir;	// Wektor "do przodu" kamery
glm::vec3 cameraSide;	// Wektor "w bok" kamery

float cameraAngle = 0;
float differenceX = 0;
float prevX = 0;
float differenceY = 0;
float prevY = 0;

glm::mat4 cameraMatrix, perspectiveMatrix;
glm::vec3 lightDir = glm::normalize(glm::vec3(0,1,0));
glm::quat rotation = glm::quat(1, 0, 0, 0);

glm::vec3 lightPos = glm::vec3(0, 0, 0);

GLuint textureAsteroid, textureShip, textureSun;
GLuint normalTextureAsteroid;
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
	differenceX = (x - prevX) * 0.01;		// pomno¿one przez 0.01, by kamera za szybko sie nie obracala
	prevX = x;

	differenceY = (y - prevY) * 0.01;
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

	cameraDir= glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos,rotation);
}


void drawObjectTexture(GLuint program, obj::Model* model, glm::mat4 modelMatrix, GLuint textureId)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);
	glUseProgram(0);
}

void drawObjectNormalMapping(GLuint program, obj::Model * model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalmapId)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectSkyBox(GLuint program, GLuint textureId) {

	glUseProgram(program);
	

	glm::mat4 view = glm::mat4(glm::mat3(cameraMatrix));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, (float*)&view);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*)&projection);
	Core::DrawSkyBox(cubemapTexture);
	glUseProgram(0);
}


void renderScene()
{
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.1f, 0.3f, 1.0f);

	

	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(-2.5,-3,-5)) * glm::rotate(glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.15f));	
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * shipInitialTransformation;

	// statek
	drawObjectTexture(programColor, &shipModel, shipModelMatrix, textureShip);
	// TODO: statek ma byc zrodlem swiatla

	// s³oñce
	drawObjectTexture(programSun, &sphereModel, glm::translate(glm::vec3(0, 0, 0)) * glm::scale(glm::vec3(15.0)), textureSun);

	for (int i = 0; i < 10; i++)
	{
		drawObjectNormalMapping(programColor, &sphereModel, glm::translate(randomPosition[i]), textureAsteroid, normalTextureAsteroid);
		// TODO: zamiast randomowego ustawiania planet, planety pojawiaja sie w okresloncyh miejscach (+ jezeli sie uda to maja sie obracac), 
	};

	
	drawObjectSkyBox(programSkyBox, cubemapTexture);

	glutSwapBuffers();
}


void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < 10; i++)
	{
		randomPosition[i] = glm::ballRand(20.0);
	}

	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");	// <-- test, swiatlo sie odbija, normal mapping nie dziala, tekstury dzialaja
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");		// <--  normal mapping i tekstury dzialaja, zle odbija swiatlo
	programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");
	programSkyBox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");

	textureShip = Core::LoadTexture("textures/ship.png");
	textureAsteroid = Core::LoadTexture("textures/asteroid.png");
	normalTextureAsteroid = Core::LoadTexture("textures/asteroid_normals.png");
	textureSun = Core::LoadTexture("textures/sun.png");
	

	std::vector<std::string> faces;
	faces.push_back("textures/skybox/right.jpg");
	faces.push_back("textures/skybox/left.jpg");
	faces.push_back("textures/skybox/top.jpg");
	faces.push_back("textures/skybox/bottom.jpg");
	faces.push_back("textures/skybox/front.jpg");
	faces.push_back("textures/skybox/back.jpg");
	for (int i = 0; i < faces.size(); i++) {
		std::cout << faces[i] << std::endl;
	}

	cubemapTexture = Core::LoadCubeMap(faces);

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
	glutCreateWindow("OpenGL Pierwszy Program");
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
