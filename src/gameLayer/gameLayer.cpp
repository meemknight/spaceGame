#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>
#include <IconsForkAwesome.h>
#include <imguiTools.h>
#include <logs.h>



gl2d::Renderer2D renderer;
gl2d::ShaderProgram shader;
GLuint iResolution = -1;
GLuint iTime = -1;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();


	platform::log("Init");

	shader = gl2d::createShaderFromFile(RESOURCES_PATH "space.frag");
	iResolution = glGetUniformLocation(shader.id, "iResolution");
	iTime = glGetUniformLocation(shader.id, "iTime");

	return true;
}


//IMPORTANT NOTICE, IF YOU WANT TO SHIP THE GAME TO ANOTHER PC READ THE README.MD IN THE GITHUB
//https://github.com/meemknight/cmakeSetup
//OR THE INSTRUCTION IN THE CMAKE FILE.
//YOU HAVE TO CHANGE A FLAG IN THE CMAKE SO THAT RESOURCES_PATH POINTS TO RELATIVE PATHS
//BECAUSE OF SOME CMAKE PROGBLMS, RESOURCES_PATH IS SET TO BE ABSOLUTE DURING PRODUCTION FOR MAKING IT EASIER.

bool gameLogic(float deltaTime, platform::Input &input)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion
	static float timer = 0;
	timer += deltaTime;

	//background
	renderer.pushCamera();
	renderer.pushShader(shader);
	glUniform2f(iResolution, w, h);
	glUniform1f(iTime, timer);
	renderer.renderRectangle({0,0, w, h}, Colors_Blue);
	renderer.flush();
	renderer.popShader();
	renderer.popCamera();




	renderer.flush();


	//ImGui::ShowDemoWindow();
	ImGui::PushMakeWindowNotTransparent();
	ImGui::Begin("Test Imgui");


	ImGui::Text("Emoji moment: " ICON_FK_AMBULANCE);

	ImGui::helpMarker("test");

	ImGui::addErrorSymbol();
	ImGui::addWarningSymbol();

	ImGui::BeginChildFrameColoured(1, Colors_Gray, {100,100}, 0);

	ImGui::Text("Test");

	ImGui::EndChildFrame();


	ImGui::PopMakeWindowNotTransparent();
	ImGui::End();

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{


}
