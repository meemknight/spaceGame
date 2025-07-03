#include <card.h>
#include <shader.h>
#include <random>

void Card::render(RenderingThing &renderingThing, gl2d::Renderer2D &renderer,
	gl2d::Camera3D &camera3D, AssetManager &assetManager,
	float w, float h, float timer)
{

	auto defaultCardShader = assetManager.glass;

	renderingThing.render(renderer, camera3D,
		assetManager, assetManager.emptyCard, defaultCardShader, {},
		w, h, timer, true, {1,1,1,1});

	renderingThing.render(renderer, camera3D,
		assetManager, assetManager.cardGrid, defaultCardShader, {},
		w, h, timer, false, {1,1,1,1});

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
			assetManager, t, assetManager.paper, displacement,
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

	rez.push_back({Joint::top, Joint::left});
	rez.push_back({Joint::top, Joint::middleTopRight});

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

