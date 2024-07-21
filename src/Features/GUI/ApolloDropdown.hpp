/*#pragma once

class ApolloGUI {
public:
	struct CategoryPosition {
		float x = 0.f, y = 0.f;
		bool isDragging = false, isExtended = true, wasExtended = false;
		float yOffset = 0;
		float scrollEase = 0;
		glm::vec4<float> shadowRect = glm::vec4<float>();
		glm::vec2<float> dragVelocity = glm::vec2<float>();
		glm::vec2<float> dragAcceleration = glm::vec2<float>();
	};

	const float catWidth = 200.f;
	const float catHeight = 35.f;

	const float catGap = 40;

	float scrollSize = 0.f;
	std::vector<CategoryPosition> catPositions;

	ImColor textColor = ImColor(255, 255, 255);
	ImColor darkBlack = ImColor(15, 15, 15);
	ImColor mainBlack = ImColor(22, 22, 22);
	ImColor grayBlack = ImColor(40, 40, 40);
	ImColor enumBack = ImColor(30, 30, 30);

	void render(float animation, float inScale, int& scrollDirection) noexcept;
};*/