#include "renderingThing.h"
#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>

void ShakeMotionState::update(float deltaTime)
{
	time += deltaTime;

	// Small back-and-forth wiggle angle
	float angle = time * 2.f; // adjust speed/amplitude as needed

	// Rotate around Z axis (top axis)
	glm::mat4 wiggleRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1));

	// Apply wiggle to the base orientation vector
	glm::vec4 rotated = wiggleRot * glm::vec4(glm::normalize(glm::vec3(0,0.3,1)),
		0.0f);
	currentOrientation = glm::normalize(glm::vec3(rotated));

	currentRotation = sin(time * 0.2f) * 0.5f;
}

glm::mat4 ShakeMotionState::getRotationMatrix()
{
	
	glm::vec3 targetDir = glm::normalize(currentOrientation); // Your new direction

	// Handle edge case: if targetDir is exactly opposite of Z+
	if (glm::length(targetDir) < 1e-4f)
		return glm::mat4(1.0f); // Fallback to identity if invalid

	glm::vec3 forward = targetDir;
	glm::vec3 up = glm::vec3(0, 1, 0);
	if (glm::abs(glm::dot(forward, up)) > 0.999f) // forward is nearly parallel to up
		up = glm::vec3(1, 0, 0);

	glm::vec3 right = glm::normalize(glm::cross(up, forward));
	glm::vec3 adjustedUp = glm::cross(forward, right);

	// Build rotation matrix with Z+ as forward
	glm::mat4 rotation(1.0f);
	rotation[0] = glm::vec4(right, 0.0f);
	rotation[1] = glm::vec4(adjustedUp, 0.0f);
	rotation[2] = glm::vec4(forward, 0.0f);

	glm::mat4 perspectiveOnlyW = glm::mat4(1.0f);
	// Set perspective effect strength
	float perspectiveStrength = -1.0f; // negative to make +z shrink away
	// This sets w = w + z * perspectiveStrength
	perspectiveOnlyW[3][2] = perspectiveStrength;

	return perspectiveOnlyW * rotation;
}

void RenderingThing::render(gl2d::Renderer2D &renderer, 
	gl2d::Camera3D &camera3D, AssetManager &assetManager
	, float w, float h, float timer)
{

	glm::mat4 rotationMatrix = shakeMotionState.getRotationMatrix();
	//auto project = glm::perspective(glm::radians(90.f), (float)w / (float)h, 0.01f, 100.f);
	//rotationMatrix = project * rotationMatrix;

	glm::mat4 modelVireProjMatrix = camera3D.getViewProjectionMatrix() * rotationMatrix;

	glm::vec4 fullQuadSize = {0, 0, renderer.windowW, renderer.windowH};

	//shadow
	//renderer.pushShader(assetManager.default3DShader.shader);
	//glUseProgram(assetManager.default3DShader.shader.id);
	//glUniformMatrix4fv(assetManager.default3DShader.u_viewProjection, 1, 0, &modelVireProjMatrix[0][0]);
	//renderer.renderRectangle(fullQuadSize,
	//	assetManager.cardPacket, {0,0,0,0.4}, {}, 0, GL2D_DefaultTextureCoords, 0.5);
	//renderer.flush();
	//renderer.popShader();


	renderer.pushShader(assetManager.holographicShader.shader);
	glUseProgram(assetManager.holographicShader.shader.id);
	glUniform2f(assetManager.holographicShader.iResolution, w, h);
	glUniform1f(assetManager.holographicShader.iTime, timer);
	glUniformMatrix4fv(assetManager.holographicShader.u_viewProjection, 1, 0, &modelVireProjMatrix[0][0]);
	glUniformMatrix4fv(assetManager.holographicShader.u_model, 1, 0, &rotationMatrix[0][0]);
	renderer.renderRectangle(fullQuadSize, assetManager.cardPacket,
		Colors_White, {}, 0, GL2D_DefaultTextureCoords, 0.5);
	renderer.flush();
	renderer.popShader();



}



