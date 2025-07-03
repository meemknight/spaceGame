#pragma once
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>


struct AssetManager;


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


};


struct RenderingThing
{

	ShakeMotionState shakeMotionState;

	void render(gl2d::Renderer2D &renderer,
		gl2d::Camera3D &camera3D,
		AssetManager &assetManager,
		float w, float h, float timer);


};