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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint programColor, programNormalMapping, programSun, programSkyBox;

Core::Shader_Loader shaderLoader;

obj::Model shipModel, shipEnemyModel, sphereModel;

glm::vec3 cameraPos = glm::vec3(40, 0, 20);
glm::vec3 cameraDir;	// Wektor "do przodu" kamery
glm::vec3 cameraSide;	// Wektor "w bok" kamery

float differenceX = 0;
float prevX = 0;
float differenceY = 0;
float prevY = 0;

float frustumScale = 1.f;

glm::mat4 cameraMatrix, perspectiveMatrix;
glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::vec3 lightPos = glm::vec3(0, 0, 0);

GLuint textureSun, texturePlanet1, texturePlanet2, texturePlanet3, texturePlanet4, texturePlanet5, textureParticle, cubemapTexture;
GLuint normalTextureP1, normalTextureP2, normalTextureP3, normalTextureP4, normalTextureP5, normalTextureAsteroid;

void keyboard(unsigned char key, int x, int y)
{
	
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch(key)
	{
		case 'q': rotation = glm::angleAxis(angleSpeed, glm::vec3(0, -1, 0)) * rotation; break;
		case 'e': rotation = glm::angleAxis(angleSpeed, glm::vec3(0, 1, 0)) * rotation; break;
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

void drawObjectColor(GLuint program, obj::Model* model, glm::mat4 modelMatrix, glm::vec3 color)
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawModel(model);
	glUseProgram(0);
}

void drawObject(GLuint program, obj::Model * model, glm::mat4 modelMatrix, GLuint textureId, GLuint normalmapId)
{
    glUseProgram(program);
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
	perspectiveMatrix = Core::createPerspectiveMatrix(0.1, 100, frustumScale);
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.f;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.1f, 0.3f, 1.0f);

	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(-2.5,-3,-5)) * glm::rotate(glm::radians(90.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.15));	
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * shipInitialTransformation;

	// statek
	drawObjectColor(programColor, &shipModel, shipModelMatrix, glm::vec3(0.0, 0.0, 0.4));
	// TODO: statek ma byc zrodlem swiatla



	// statki wroga
	glm::mat4 enemyShipMatrix = glm::translate(glm::vec3(65, 0, 40)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.4));
	glm::mat4 enemyShipMatrix2 = glm::translate(glm::vec3(65, 0, 30)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.4));
	glm::mat4 enemyShipMatrix3 = glm::translate(glm::vec3(65, 0, 20)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.4));
	glm::mat4 enemyShipMatrix4 = glm::translate(glm::vec3(65, 10, 30)) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.8));

	drawObjectColor(programColor, &shipEnemyModel, enemyShipMatrix, glm::vec3(1.0, 0.0, 0.0));
	drawObjectColor(programColor, &shipEnemyModel, enemyShipMatrix2, glm::vec3(1.0, 0.0, 0.0));
	drawObjectColor(programColor, &shipEnemyModel, enemyShipMatrix3, glm::vec3(1.0, 0.0, 0.0));
	drawObjectColor(programColor, &shipEnemyModel, enemyShipMatrix4, glm::vec3(1.0, 0.0, 0.0));

	// slonce
	drawObject(programSun, &sphereModel, glm::translate(lightPos) * glm::scale(glm::vec3(20)) * glm::eulerAngleY(time / 100), textureSun, 0);

	// planety
	glm::mat4 planet1Position = glm::translate(glm::vec3(30, 0, 0));
	glm::mat4 planet2Position = glm::translate(glm::vec3(50, 0, 0));
	glm::mat4 planet3Position = glm::translate(glm::vec3(90, 0, 0));
	glm::mat4 rotateAroundSun = glm::translate(lightPos) * glm::eulerAngleY(time / 2);
	glm::mat4 rotateAroundSun2 = glm::translate(lightPos) * glm::eulerAngleY(-time / 2);
	glm::mat4 moonDistance = glm::translate(glm::vec3(10, 0, 0));
	glm::mat4 moonDistance2 = glm::translate(glm::vec3(-10, 0, 0));

	drawObject(programNormalMapping, &sphereModel, rotateAroundSun * planet1Position * glm::scale(glm::vec3(3)) * glm::eulerAngleY(time / 3), texturePlanet1, normalTextureP1);			// obraca sie wokol slonca i osi Y
	drawObject(programNormalMapping, &sphereModel, rotateAroundSun2 * planet2Position * glm::scale(glm::vec3(5)) * glm::eulerAngleY(-time / 5), texturePlanet2, normalTextureP2);		// obraca sie wokol slonca (przeciwny kierunek) i osi Y
	drawObject(programNormalMapping, &sphereModel, planet3Position * glm::scale(glm::vec3(7)) * glm::eulerAngleY(time / 10), texturePlanet3, normalTextureP3);							// obraca sie wokol osi Y

	// ksiezyce 3 planety
	drawObject(programNormalMapping, &sphereModel, planet3Position * glm::eulerAngleY(time / 5) * moonDistance * glm::eulerAngleY(-time / 2) * scale(glm::vec3(1.5)), texturePlanet4, normalTextureP4);		// obraca sie wokol planety i osi Y
	drawObject(programNormalMapping, &sphereModel, planet3Position * glm::eulerAngleY(time / 5) * moonDistance2 * glm::eulerAngleY(-time / 2) * scale(glm::vec3(1.5)), texturePlanet5, normalTextureP5);	// obraca sie wokol planety i osi Y

	// skybox
	drawObjectSkyBox(programSkyBox, cubemapTexture);

	// prawe spaliny
	for (int i = 10; i < 100; i++)
	{
		glm::mat4 particleTransformation = glm::translate(glm::vec3(0.7 + -0.1 * i / 90, -2.9, -4 * sin(time*i))) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.015f));
		glm::mat4 particleMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * particleTransformation;
		drawObject(programNormalMapping, &sphereModel, particleMatrix, textureParticle, 0);
	}

	// lewe spaliny
	for (int i = 10; i < 100; i++)
	{
		glm::mat4 particleTransformation = glm::translate(glm::vec3(-0.7 + -0.1 * i / 90, -2.9, -4 * sin(time*i))) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.015f));
		glm::mat4 particleMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * glm::mat4_cast(glm::inverse(rotation)) * particleTransformation;
		drawObject(programNormalMapping, &sphereModel, particleMatrix, textureParticle, 0);
	}

	glutSwapBuffers();
}


void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);

	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");					// wczytywanie shaderow do modeli bez tekstur (kolor)
	programNormalMapping = shaderLoader.CreateProgram("shaders/shader_mapping.vert", "shaders/shader_mapping.frag");		// wczytywanie shaderow do modeli z teksturami i normal mappingiem
	programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");							// wczytywanie shaderow slonca
	programSkyBox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");					// wczytywanie shaderow skyboxa

	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	shipEnemyModel = obj::loadModelFromFile("models/enemy_spaceship.obj");

	textureSun = Core::LoadTexture("textures/sun.png");
	textureParticle = Core::LoadTexture("textures/particle.png");
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
  
  std::vector<std::string> faces;
	faces.push_back("textures/skybox/right.png");
	faces.push_back("textures/skybox/left.png");
	faces.push_back("textures/skybox/top.png");
	faces.push_back("textures/skybox/bottom.png");
	faces.push_back("textures/skybox/front.png");
	faces.push_back("textures/skybox/back.png");
	for (int i = 0; i < faces.size(); i++) {
		std::cout << faces[i] << std::endl;
	}

	cubemapTexture = Core::LoadCubeMap(faces);
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programNormalMapping);
	shaderLoader.DeleteProgram(programSun);
	shaderLoader.DeleteProgram(programSkyBox);
}

void idle()
{
	glutPostRedisplay();
}

void onReshape(int width, int height)
{

	glViewport(0, 0, width, height);
	frustumScale = (float)width / (float)height;
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(700, 700);
	glutCreateWindow("OpenGL Project");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);
	glutReshapeFunc(onReshape);

	glutMainLoop();

	shutdown();

	return 0;
}
