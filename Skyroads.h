#pragma once
#include <Component/SimpleScene.h>
#include <Component/Transform/Transform.h>
#include <Core/GPU/Mesh.h>

class Skyroads : public SimpleScene
{
public:
	Skyroads();
	~Skyroads();

	void Init() override;

private:
	struct platform {
		float x;
		float y;
		float z;
		int color;
		float scaleZ;
	};

	struct sphere {
		float x;
		float y;
		float z;
	};

	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color = glm::vec3(1));

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;
	void GeneratePlatforms(platform& platform);
	void checkPlatformIsOut(std::vector<struct platform>& platformsVector);
	void setFirstPerson();
	void setThirdPerson();
	void RenderCustomMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

	glm::vec3 lightPosition;
	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	int platforms;
	float lastPlatformLeftZ;
	float lastPlatformRightZ;
	float lastPlatformCenterZ;
	float lastPlatformLeftScaleZ;
	float lastPlatformRightScaleZ;
	float lastPlatformCenterScaleZ;
	float scaleX, scaleY;
	float platformsSpeed;
	bool isWPressed;
	bool isSPressed;
	bool isSpacePressed;
	bool isSpehereInAir;
	float isSphereOnPlatform;
	int platformNumber;
	float fuelBarNewPosition;
	float stepFuelBar;
	bool upward;
	bool gameOverFall;
	float time;
	bool speedActivated;
	float timeToLive;
	int cameraType;
	float currentFuel;
	bool deformation;
	float deformationTime;

	sphere sphere;
	std::vector<platform> platformsLeft;
	std::vector<platform> platformsCenter;
	std::vector<platform> platformsRight;

};