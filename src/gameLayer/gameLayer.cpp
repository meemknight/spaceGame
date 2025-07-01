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
#include "shader.h"
#include "assetManager.h"


gl2d::Renderer2D renderer;
Shader backgroundShader;
Shader default3DShader;
Shader holographicShader;
AssetManager assetManager;
gl2d::FrameBuffer background;


bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();


	platform::log("Init");

	backgroundShader.load(RESOURCES_PATH "space.frag");
	holographicShader.load3DShader(RESOURCES_PATH "balatro.frag");
	default3DShader.loadDefault3DShader();
	assetManager.loadAll();
	background.create(1, 1);

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

	background.resize(w, h);

	//background
	{
		renderer.pushCamera();
		renderer.pushShader(backgroundShader.shader);
		glUseProgram(backgroundShader.shader.id);
		glUniform2f(backgroundShader.iResolution, w, h);
		glUniform1f(backgroundShader.iTime, timer);
		renderer.renderRectangle({0,0, w, h}, {1,1,1,0.5});
		renderer.flushFBO(background);
		renderer.popShader();

		renderer.renderRectangle({0,0, w, h}, background.texture);
		renderer.flush();


		renderer.popCamera();

	}


	//basic cards
	if(w != 0 && h != 0)
	{
		glm::mat4 rotationMatrix(1.0);
		glm::vec2 cursorPos = platform::getRelMousePosition();
		cursorPos /= glm::vec2(w, h);
		cursorPos = glm::clamp(cursorPos, glm::vec2(0), glm::vec2(1.f));
		cursorPos -= glm::vec2(0.5f); // Now range is [-0.5, 0.5]

		// Scale sensitivity — tweak this value to make rotation more or less pronounced
		float rotationScale = 0.7f; // Radians max rotation per axis
		float angleX = -cursorPos.y * rotationScale; // up/down moves rotate around X
		float angleY = cursorPos.x * rotationScale;  // left/right moves rotate around Y

		glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1, 0, 0));
		glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(0, 1, 0));

		rotationMatrix = rotY * rotX;
		//auto project = glm::perspective(glm::radians(90.f), (float)w / (float)h, 0.01f, 100.f);
		//rotationMatrix = project * rotationMatrix;

		renderer.pushShader(holographicShader.shader);
		glUseProgram(holographicShader.shader.id);
		glUniform2f(holographicShader.iResolution, w, h);
		glUniform1f(holographicShader.iTime, timer);
		glUniformMatrix4fv(holographicShader.u_viewProjection, 1, 0, &rotationMatrix[0][0]);
		renderer.renderRectangle({300, 300, 400, 400}, assetManager.emptyCard);
		renderer.flush();
		renderer.popShader();


		renderer.pushShader(default3DShader.shader);
		glUseProgram(default3DShader.shader.id);
		glUniformMatrix4fv(default3DShader.u_viewProjection, 1, 0, &rotationMatrix[0][0]);
		renderer.renderRectangle({300, 300, 400, 400}, assetManager.earthCard);
		renderer.flush();
		renderer.popShader();

	}


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
