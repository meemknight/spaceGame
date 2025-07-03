#pragma once
#include "renderingThing.h"
#include <gl2d/gl2d.h>

struct AssetManager;


enum Type
{
	none = 0,
	star = 1,
	//...

};

enum Color
{
	white = 0,
	red = 1,
	yellow,
	blue,
	green,
	COLOR_COUNT
	//...

};

glm::vec4 getColor(int type);

gl2d::Texture getTextureFromType(AssetManager &assetManager, int type);

struct CardElement
{

	int type = none;
	int color = white;
	
};


struct Joint
{
	enum JointPosition
	{
		center = 0,
		middleTopLeft,
		middleTopRight,
		middleBottomLeft,
		middleBottomRight,
		topLeft,
		topRight,
		bottomLeft,
		bottomRight,
		left,
		right,
		top,
		bottom,
		JOINTS_TYPE_COUNT
	};


	unsigned char start = 0;
	unsigned char end = 0;

	void rotateLeft()
	{
		static const unsigned char rotateLeftMap[JOINTS_TYPE_COUNT] = {
			center,             // center
			middleBottomLeft,   // middleTopLeft -> middleBottomLeft
			middleTopLeft,      // middleTopRight -> middleTopLeft
			middleBottomRight,  // middleBottomLeft -> middleBottomRight
			middleTopRight,     // middleBottomRight -> middleTopRight
			bottomLeft,         // topLeft -> bottomLeft
			topLeft,            // topRight -> topLeft
			bottomRight,        // bottomLeft -> bottomRight
			topRight,           // bottomRight -> topRight
			bottom,                // left -> 
			top,             // right -> 
			left,              // top -> 
			right                // bottom -> 
		};

		start = rotateLeftMap[start];
		end = rotateLeftMap[end];
	}

	void rotateRight()
	{
		static const unsigned char rotateRightMap[JOINTS_TYPE_COUNT] = {
			center,             // center
			middleTopRight,     // middleTopLeft -> middleTopRight
			middleBottomRight,  // middleTopRight -> middleBottomRight
			middleTopLeft,      // middleBottomLeft -> middleTopLeft
			middleBottomLeft,   // middleBottomRight -> middleBottomLeft
			topRight,           // topLeft -> topRight
			bottomRight,        // topRight -> bottomRight
			topLeft,            // bottomLeft -> topLeft
			bottomLeft,         // bottomRight -> bottomLeft
			top,             // left -> 
			bottom,                // right -> 
			right,               // top -> 
			left               // bottom -> 
		};

		start = rotateRightMap[start];
		end = rotateRightMap[end];
	}


};

glm::vec2 getJointOffset(int pos);


struct Card
{
	
	CardElement topLeftCardElement;
	CardElement topRightCardElement;
	CardElement bottomLeftCardElement;
	CardElement bottomRightCardElement;

	std::vector<Joint> joints;

	void render(RenderingThing &renderingThing, gl2d::Renderer2D &renderer,
		gl2d::Camera3D &camera3D, AssetManager &assetManager
		, float w, float h, float timer);

	void rotateLeft();
	void rotateRight();

};

struct CardWithPosition
{

	RenderingThing renderingThing;

	Card card;

};

Card getRandomCard();