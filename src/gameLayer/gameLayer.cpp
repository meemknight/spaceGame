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
#include "renderingThing.h"
#include <card.h>


gl2d::Renderer2D renderer;

AssetManager assetManager;
CardWithPosition handCard;
gl2d::Camera3D camera3D;

std::vector<CardWithPosition> cards;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();


	platform::log("Init");

	assetManager.loadAll();
	assetManager.background.create(1, 1);

	handCard.card = getRandomCard();


	return true;
}

glm::vec2 fromCursorToWorldPosition(
	float cursorX, float cursorY,
	int screenWidth, int screenHeight,
	const glm::mat4 &viewMatrix,
	const glm::mat4 &projectionMatrix)
{
	glm::vec4 viewport(0, 0, screenWidth, screenHeight);

	glm::vec3 pNear = glm::unProject(
		glm::vec3(cursorX, screenHeight - cursorY, 0.0f),
		viewMatrix,
		projectionMatrix,
		viewport
	);

	glm::vec3 pFar = glm::unProject(
		glm::vec3(cursorX, screenHeight - cursorY, 1.0f),
		viewMatrix,
		projectionMatrix,
		viewport
	);

	glm::vec3 dir = pFar - pNear;
	float t = -pNear.z / dir.z;  // intersect Z = 0
	glm::vec3 intersection = pNear + dir * t;
	return glm::vec2(intersection.x, intersection.y);
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

#if DEVELOPLEMT_BUILD == 1
	assetManager.tryReload();
#endif

	if (w != assetManager.background.w ||
		h != assetManager.background.h)
	{
		assetManager.background.clear();
		assetManager.background.resize(w, h);
	}

	glm::vec2 mousePosition = {input.mouseX, input.mouseY};
	mousePosition = glm::clamp(mousePosition, glm::vec2(0, 0), glm::vec2(w, h));

	glm::vec2 worldPosition = fromCursorToWorldPosition(mousePosition.x,
		mousePosition.y, w, h, camera3D.getViewMatrix(),
		camera3D.getProjectionMatrix());

	handCard.renderingThing.shakeMotionState.desiredPosition = {worldPosition, 0};

	//std::cout << worldPosition.x << " " << worldPosition.y << "\n";

	//background
	{
		renderer.pushCamera();
		renderer.pushShader(assetManager.backgroundShader.shader);
		glUseProgram(assetManager.backgroundShader.shader.id);
		glUniform2f(assetManager.backgroundShader.iResolution, w, h);
		glUniform1f(assetManager.backgroundShader.iTime, timer);
		renderer.renderRectangle({0,0, w, h});
		renderer.flushFBO(assetManager.background);
		renderer.popShader();

		renderer.renderRectangle({0,0, w, h}, assetManager.background.texture);
		renderer.flush();


		renderer.popCamera();

	}

	camera3D.fovRadians = glm::radians(90.f);
	camera3D.aspectRatio = w / (float)h;
	camera3D.position = {0,0, 5};

	if (input.isButtonPressed(platform::Button::Q))
	{
		handCard.card.rotateLeft();
	}

	if (input.isButtonPressed(platform::Button::E))
	{
		handCard.card.rotateRight();
	}

	if (input.isLMousePressed())
	{
		CardWithPosition c;
		c = handCard;
		c.renderingThing.shakeMotionState.desiredPosition =
			glm::vec3(glm::ivec2(glm::round(c.renderingThing.shakeMotionState.position)), 0);

		c.renderingThing.shakeMotionState.staticAnimate = false;

		cards.push_back(c);

		handCard.card = getRandomCard();

	}


	//basic cards
	if(w != 0 && h != 0)
	{
		renderer.pushCamera();

		//grid
	#pragma region grid
		glm::vec4 gridColor = {0.2,0.2,0.5, 0.5};
		glm::mat4 viewProj = camera3D.getViewProjectionMatrix();

		renderer.pushShader(assetManager.dither.shader);
		glUseProgram(assetManager.dither.shader.id);
		glUniform2f(assetManager.dither.iResolution, w, h);
		glUniform1f(assetManager.dither.iTime, timer);
		glUniform2f(assetManager.dither.u_CursorPosition, input.mouseX, input.mouseY);
		glUniformMatrix4fv(assetManager.dither.u_viewProjection, 1, 0, &viewProj[0][0]);
		glm::vec4 fullQuadSize = {0, 0, renderer.windowW, renderer.windowH};

		for (int i = -10; i < 10; i++)
		{
			glm::mat4 model =
				glm::translate(glm::vec3(i,0,0)) * glm::scale(glm::vec3{0.02,100,1});
			glUniformMatrix4fv(assetManager.dither.u_model, 1, 0, &model[0][0]);

			renderer.renderRectangle(fullQuadSize, gridColor);
			renderer.flush();
		}

		for (int i = -10; i < 10; i++)
		{
			glm::mat4 model =
				glm::translate(glm::vec3(0, i, 0)) * glm::scale(glm::vec3{100, 0.02, 1});
			glUniformMatrix4fv(assetManager.dither.u_model, 1, 0, &model[0][0]);

			renderer.renderRectangle(fullQuadSize, gridColor);
			renderer.flush();
		}

		renderer.popShader();
	#pragma endregion


		glm::mat4 rotationMatrix(1.0);
		glm::vec2 cursorPos = platform::getRelMousePosition();
		cursorPos /= glm::vec2(w, h);
		cursorPos = glm::clamp(cursorPos, glm::vec2(0), glm::vec2(1.f));
		cursorPos -= glm::vec2(0.5f); // Now range is [-0.5, 0.5]


		for (auto &c : cards)
		{
			c.renderingThing.shakeMotionState.update(deltaTime);
			c.card.render(c.renderingThing, renderer, camera3D, assetManager, w, h, timer);

		}


		handCard.renderingThing.shakeMotionState.update(deltaTime);
		handCard.card.render(handCard.renderingThing, renderer, camera3D, assetManager, w, h, timer);


		//renderer.pushShader(holographicShader.shader);
		//glUseProgram(holographicShader.shader.id);
		//glUniform2f(holographicShader.iResolution, w, h);
		//glUniform1f(holographicShader.iTime, timer);
		//glUniformMatrix4fv(holographicShader.u_viewProjection, 1, 0, &rotationMatrix[0][0]);
		//renderer.renderRectangle({300, 300, 400, 400}, assetManager.emptyCard);
		//renderer.flush();
		//renderer.popShader();
		//
		//
		//renderer.pushShader(default3DShader.shader);
		//glUseProgram(default3DShader.shader.id);
		//glUniformMatrix4fv(default3DShader.u_viewProjection, 1, 0, &rotationMatrix[0][0]);
		//renderer.renderRectangle({300, 300, 400, 400}, assetManager.earthCard);
		//renderer.flush();
		//renderer.popShader();

		renderer.popCamera();
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
