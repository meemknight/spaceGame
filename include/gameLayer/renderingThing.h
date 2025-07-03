#pragma once
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>


struct AssetManager;
struct Shader;

//used to move the cards slightly
struct ShakeMotionState
{
	float currentRotation = 0;
	float desiredRotation = 0;
	float time = 0.0f;

	glm::vec3 currentOrientation = {0,0,1};
	glm::vec3 desiredOrientation = {0,0,1};

	void update(float deltaTime);

	glm::mat4 getRotationMatrix();


	glm::vec3 position = {};
	glm::vec3 desiredPosition = {};

	bool staticAnimate = 1; //animates it when not moving;

};

//todo disolve this
struct RenderingThing
{

	ShakeMotionState shakeMotionState;

	void render(gl2d::Renderer2D &renderer,
		gl2d::Camera3D &camera3D, AssetManager &assetManager,
		gl2d::Texture &texture, Shader &shader, glm::vec2 displacement,
		float w, float h, float timer, bool addShadow, 
		glm::vec4 color, glm::mat4 customMatrix = glm::mat4(1.f));

	void renderLine(gl2d::Renderer2D &renderer,
		gl2d::Camera3D &camera3D, AssetManager &assetManager,
		gl2d::Texture &texture, Shader &shader, glm::vec2 displacement,
		glm::vec2 start, glm::vec2 end,
		float w, float h, float timer, bool addShadow,
		glm::vec4 color);

};