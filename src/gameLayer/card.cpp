#include <card.h>
#include <shader.h>
#include <random>
#include "randomStuff.h"
#include <algorithm>

void Card::render(RenderingThing &renderingThing, gl2d::Renderer2D &renderer,
	gl2d::Camera3D &camera3D, AssetManager &assetManager,
	float w, float h, float timer)
{

	auto defaultCardShader = assetManager.holographicShader;

	renderingThing.render(renderer, camera3D,
		assetManager, assetManager.emptyCard, defaultCardShader, {},
		w, h, timer, true, {1,1,1,1}, glm::mat4(1.f), rotation * glm::radians(90.f));

	renderingThing.render(renderer, camera3D,
		assetManager, assetManager.cardGrid, defaultCardShader, {},
		w, h, timer, false, {1,1,1,1}, glm::mat4(1.f), rotation * glm::radians(90.f));

	for (auto &j : joints)
	{
		glm::vec2 start = getJointOffset(j.start);
		glm::vec2 end = getJointOffset(j.end);

		renderingThing.renderLine(renderer, camera3D,
			assetManager,assetManager.white1pxSquareTexture, defaultCardShader, {},
			start, end,
			w, h, timer, false, {0.8,0.8,0.8,1});

	}

	auto renderElement = [&](CardElement &element, glm::vec2 displacement)
	{

		if (element.type == none) { return; }

		auto t = getTextureFromType(assetManager, element.type);
		auto c = getColor(element.color);

		renderingThing.render(renderer, camera3D,
			assetManager, t, defaultCardShader, displacement,
			w, h, timer, false, c);

	};
	
	float displacement = 0.45;
	renderElement(topLeftCardElement, glm::vec2(-1, 1) * displacement);
	renderElement(topRightCardElement, glm::vec2(1, 1) * displacement);
	renderElement(bottomLeftCardElement, glm::vec2(-1, -1) * displacement);
	renderElement(bottomRightCardElement, glm::vec2(1, -1) * displacement);

}

void Card::rotateLeft()
{
	CardElement tmp = topLeftCardElement;
	topLeftCardElement = topRightCardElement;
	topRightCardElement = bottomRightCardElement;
	bottomRightCardElement = bottomLeftCardElement;
	bottomLeftCardElement = tmp;

	for (auto &j : joints)
	{
		j.rotateLeft();
	}
	
	rotation--;
	if (rotation < 0) { rotation = 3; }
}

void Card::rotateRight()
{
	CardElement tmp = topLeftCardElement;
	topLeftCardElement = bottomLeftCardElement;
	bottomLeftCardElement = bottomRightCardElement;
	bottomRightCardElement = topRightCardElement;
	topRightCardElement = tmp;

	for (auto &j : joints)
	{
		j.rotateRight();
	}

	rotation++;
	if (rotation > 3) { rotation = 0; }
}

glm::vec4 getColor(int type)
{
	switch (type)
	{
	case white: return Colors_White;
	case red: return Colors_Red;
	case yellow: return Colors_Yellow;
	case blue: return Colors_Blue;
	case green: return Colors_Green;
	}

	return Colors_White;
}

gl2d::Texture getTextureFromType(AssetManager &assetManager, int type)
{

	switch (type)
	{
		case star: return assetManager.star;


	}

	return gl2d::Texture{};
}


CardElement generateRandomCardElement(std::mt19937 &rng)
{
	static std::uniform_int_distribution<int> typeDist(0, 2); // 0 = none, 1 = star
	static std::uniform_int_distribution<int> colorDist(1, COLOR_COUNT - 1);

	CardElement element;

	if (typeDist(rng) >= star)
	{
		element.type = star;
		element.color = colorDist(rng);
	}
	else
	{
		element.type = none;
		element.color = white;
	}

	return element;
}

glm::vec2 getJointOffset(int pos)
{
	if (pos > Joint::bottom || pos <= Joint::center) { return {}; }

	glm::vec2 offset[Joint::JOINTS_TYPE_COUNT] = {
		glm::vec2(0.0f, 0.0f),   // center
		glm::vec2(-0.5f, 0.5f),  // middleTopLeft
		glm::vec2(0.5f, 0.5f),   // middleTopRight
		glm::vec2(-0.5f, -0.5f), // middleBottomLeft
		glm::vec2(0.5f, -0.5f),  // middleBottomRight
		glm::vec2(-1.0f, 1.0f),  // topLeft
		glm::vec2(1.0f, 1.0f),   // topRight
		glm::vec2(-1.0f, -1.0f), // bottomLeft
		glm::vec2(1.0f, -1.0f),  // bottomRight
		glm::vec2(-1.0f, 0.0f),  // left
		glm::vec2(1.0f, 0.0f),   // right
		glm::vec2(0.0f, 1.0f),   // top
		glm::vec2(0.0f, -1.0f),  // bottom
	};

	return offset[pos];
}

std::vector<Joint> getRandomJoints(Card &c)
{
	std::vector<Joint> rez;

	static std::random_device rd;
	static std::ranlux24_base rng(rd());

	//rez.push_back({Joint::top, Joint::middleTopLeft});
	//rez.push_back({Joint::middleTopLeft, Joint::left});
	//rez.push_back({Joint::top, Joint::middleTopRight});
	
	auto doOneElement = [&](Joint combination[], int count)
	{
		rez.push_back(combination[getRandomInt(rng, 0, 3)]);

		if (getRandomChance(rng, 0.5))
		{
			rez.push_back(combination[getRandomInt(rng, 0, 3)]);
		}

		if (getRandomChance(rng, 0.1))
		{
			rez.push_back(combination[getRandomInt(rng, 0, 3)]);
		}
	};

	if (c.topLeftCardElement.type != none)
	{
		Joint combination[] = {
		Joint(Joint::middleTopLeft, Joint::topLeft),
		Joint(Joint::middleTopLeft, Joint::top),
		Joint(Joint::middleTopLeft, Joint::center),
		Joint(Joint::middleTopLeft, Joint::left),
		};

		doOneElement(combination, 4);
	}

	if (c.topRightCardElement.type != none)
	{
		Joint combination[] = {
		Joint(Joint::middleTopRight, Joint::topRight),
		Joint(Joint::middleTopRight, Joint::top),
		Joint(Joint::middleTopRight, Joint::center),
		Joint(Joint::middleTopRight, Joint::right),
		};

		doOneElement(combination, 4);
	}

	if (c.bottomLeftCardElement.type != none)
	{
		Joint combination[] = {
		Joint(Joint::middleBottomLeft, Joint::bottomLeft),
		Joint(Joint::middleBottomLeft, Joint::bottom),
		Joint(Joint::middleBottomLeft, Joint::center),
		Joint(Joint::middleBottomLeft, Joint::left),
		};

		doOneElement(combination, 4);
	}

	if (c.bottomRightCardElement.type != none)
	{
		Joint combination[] = {
		Joint(Joint::middleBottomRight, Joint::bottomRight),
		Joint(Joint::middleBottomRight, Joint::bottom),
		Joint(Joint::middleBottomRight, Joint::center),
		Joint(Joint::middleBottomRight, Joint::right),
		};

		doOneElement(combination, 4);
	}



	// This requires that operator== is defined for Joint
	std::sort(rez.begin(), rez.end(), [](const Joint &a, const Joint &b)
	{
		// Provide a consistent ordering (e.g., compare by ID or position)
		return (a.start < b.start) || (a.start == b.start && a.end < b.end);
	});

	rez.erase(std::unique(rez.begin(), rez.end()), rez.end());


	return rez;
}

Card getRandomCard()
{
	static std::random_device rd;
	static std::mt19937 rng(rd());

	Card card;

	card.topLeftCardElement = generateRandomCardElement(rng);
	card.topRightCardElement = generateRandomCardElement(rng);
	card.bottomLeftCardElement = generateRandomCardElement(rng);
	card.bottomRightCardElement = generateRandomCardElement(rng);

	card.joints = getRandomJoints(card);


	return card;
}

