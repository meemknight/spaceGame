#define GLM_ENABLE_EXPERIMENTAL
#include "renderingThing.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <shader.h>
#include <glm/gtx/vector_angle.hpp>

void ShakeMotionState::update(float deltaTime)
{
	time += deltaTime;

	//position
	{
		float dist = glm::distance(glm::vec2(position), glm::vec2(desiredPosition));
		float smoothness = 10;
		desiredRotation = 0;

		if (dist < 0.001f)
		{
			position = desiredPosition;
			

			desiredOrientation = {0,0,1};

			if(staticAnimate)
			{
				// Small back-and-forth wiggle angle
				float angle = time * 1.f; // adjust speed/amplitude as needed
				// Rotate around Z axis (top axis)
				glm::mat4 wiggleRot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1));
				// Apply wiggle to the base orientation vector
				glm::vec4 rotated = wiggleRot * glm::vec4(glm::normalize(glm::vec3(0,0.2,1)),
					0.0f);
				desiredOrientation = glm::normalize(glm::vec3(rotated));
				
				//desiredRotation = sin(time * 2.0f) * 1.0f;
			}
		}
		else
		{
			glm::vec2 dir = glm::vec2(desiredPosition) - glm::vec2(position);

			// Scale the interpolation by distance and time to create a velocity effect
			float t = 1.0f - std::exp(-smoothness * deltaTime);

			glm::vec2 newPos = glm::vec2(position) + dir * t;
			position = glm::vec3(newPos, position.z);

			glm::vec2 clampedDir = glm::vec2(dir);
			if (glm::length(clampedDir) > 2)
			{
				clampedDir = glm::normalize(clampedDir) * 2.f;
			}

			desiredOrientation = glm::normalize(glm::vec3{dir*0.3f,1.f});

		}

	}

	//rotation
	{
		float smoothness = 10;
		float t = 1.0f - std::exp(-smoothness * deltaTime);
		t = glm::clamp(t, 0.f, 1.f);
		currentRotation = currentRotation + (desiredRotation - currentRotation) * t;

	}

	//direction
	{

		float smoothness = 12.0f;

		smoothness /=
			(
			glm::pow(
			glm::clamp(glm::dot(desiredOrientation, glm::vec3(0, 0, 1)), 0.f, 1.f),
			2.f) * 2.f
			);

		float t = 1.0f - std::exp(-smoothness * deltaTime);
		t = glm::clamp(t, 0.f, 1.f);

		currentOrientation = (glm::mix(currentOrientation, desiredOrientation, t));

		float l = glm::length(currentOrientation);
		if (l > 0.0001) { currentOrientation /= l; }
		else
		{
			currentOrientation = desiredOrientation;
		}


	}


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

	return rotation;
}

void RenderingThing::render(gl2d::Renderer2D &renderer,
	gl2d::Camera3D &camera3D, AssetManager &assetManager,
	gl2d::Texture &texture, Shader &shader, glm::vec2 displacement,
	float w, float h, float timer, bool addShadow, glm::vec4 color, glm::mat4 customMatrix)
{

	glm::mat4 positionMatrix = glm::translate(shakeMotionState.position);

	glm::mat4 rotationMatrix = shakeMotionState.getRotationMatrix() *
		glm::translate(glm::vec3(displacement, 0)) * customMatrix;

	//auto project = glm::perspective(glm::radians(90.f), (float)w / (float)h, 0.01f, 100.f);
	//rotationMatrix = project * rotationMatrix;

	glm::mat4 modelMatrix = positionMatrix * rotationMatrix;

	glm::vec4 fullQuadSize = {0, 0, renderer.windowW, renderer.windowH};
	glm::mat4 viewProjMatrix = camera3D.getViewProjectionMatrix();


	auto bindShaderAndSendUniforms = [&](Shader &s,
		glm::mat4 modelView, glm::mat4 model)
	{
		glUseProgram(s.shader.id);
		glUniform2f(s.iResolution, w, h);
		glUniform1f(s.iTime, timer);
		glUniformMatrix4fv(s.u_viewProjection, 1, 0, &modelView[0][0]);
		glUniformMatrix4fv(s.u_model, 1, 0, &model[0][0]);

		glUniform1i(s.u_background, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, assetManager.background.texture.id);

	};

	auto renderShadow = [&]()
	{
		float shadowDistance = -0.1;

		glm::mat4 positionMatrixShadow = glm::translate(shakeMotionState.position + glm::vec3{shadowDistance,shadowDistance,0});
		glm::mat4 shadowModelMatrix = positionMatrixShadow * rotationMatrix;

		renderer.pushShader(assetManager.default3DShader.shader);
		bindShaderAndSendUniforms(assetManager.default3DShader, 
			viewProjMatrix, shadowModelMatrix);
		renderer.renderRectangle(fullQuadSize,
			texture, {0,0,0,0.5}, {}, shakeMotionState.currentRotation);
		renderer.flush();
		renderer.popShader();

	};

	if(addShadow) renderShadow();

	renderer.pushShader(shader.shader);
	
	bindShaderAndSendUniforms(shader,
		viewProjMatrix, modelMatrix);
	
	renderer.renderRectangle(fullQuadSize, texture,
		color, {}, shakeMotionState.currentRotation, GL2D_DefaultTextureCoords, 0.0);
	renderer.flush();
	renderer.popShader();



}

void RenderingThing::renderLine(gl2d::Renderer2D &renderer, gl2d::Camera3D &camera3D,
	AssetManager &assetManager, gl2d::Texture &texture, 
	Shader &shader, glm::vec2 displacement, glm::vec2 start, 
	glm::vec2 end, float w, float h, float timer, bool addShadow, glm::vec4 color)
{
	start /= 2.f;
	end /= 2.f;

	glm::vec2 direction = end - start;
	float length = glm::length(direction);
	if (length == 0.0f) return;

	glm::vec2 midPoint = (start + end) * 0.5f;
	float angle = atan2(direction.y, direction.x);

	float scaleBias = 0.04;
	float displacementBias = 0.9;

	glm::mat4 transform(1.0f);
	transform = glm::translate(transform, glm::vec3(midPoint*2.f * displacementBias + displacement, 0.0f));
	transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(length - scaleBias, 0.02f, 1.0f));

	render(renderer, camera3D, assetManager, texture, shader, displacement,
		w, h, timer, addShadow, color, transform);
}
