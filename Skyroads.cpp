#include "Skyroads.h"

#include <vector>
#include <string>
#include <iostream>
#include <chrono>

#include <Core/Engine.h>
#include <Laboratoare\Tema2\Transform.h>

using namespace std;

Skyroads::Skyroads()
{
}

Skyroads::~Skyroads()
{
}

void Skyroads::setFirstPerson() {
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(sphere.x, sphere.y, 1));
	camera->Update();
}

void Skyroads::setThirdPerson() {
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 2.f, sphere.z + 1.5f));
	camera->Update();
}

void Skyroads::Init()
{

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("plane");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("cube");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	// fuelbar
	Mesh* fuel = Transform::CreateRectangle("fuel", 1.2f, 0.1f, glm::vec3(1.f, 0.f, 0.f), true);
	meshes[fuel->GetMeshID()] = fuel;

	// Create a shader program for drawing face polygon with the color of the normal
	{
		Shader* shader = new Shader("ShaderTema2");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	//Light & material properties
	{
		lightPosition = glm::vec3(0, 1, 1);
		materialShininess = 30;
		materialKd = 0.5;
		materialKs = 0.5;
	}

	time = 0;
	speedActivated = false;
	scaleX = 0.8f;
	scaleY = 0.05f;
	platforms = 5;
	lastPlatformCenterScaleZ = 0;
	lastPlatformRightScaleZ = 0;
	lastPlatformLeftScaleZ = 0;
	platformsSpeed = 2;
	sphere.x = 0;
	sphere.y = 0.9f + scaleY;
	sphere.z = 1.5f;
	lastPlatformCenterZ = sphere.z;
	lastPlatformRightZ = sphere.z;
	lastPlatformLeftZ = sphere.z;
	fuelBarNewPosition = 1;
	stepFuelBar = 0.0003f;
	currentFuel = 1;
	isSphereOnPlatform = false;
	isSpehereInAir = false;
	upward = false;
	gameOverFall = false; 
	platformNumber = 0;
	timeToLive = 0;
	cameraType = 0;
	deformation = false;
	deformationTime = 0;

	for (int i = 0; i < platforms; i++) {
		platformsRight.push_back(platform());
		platformsLeft.push_back(platform());
		platformsCenter.push_back(platform());

		// right
		platformsRight[i].x = scaleX;
		platformsRight[i].y = 0.75f;
		platformsRight[i].scaleZ = rand() % 8 + 3;
		platformsRight[i].z = lastPlatformRightZ - lastPlatformRightScaleZ / 2.f - platformsRight[i].scaleZ / 2.f - rand() % 2;
		platformsRight[i].color = rand() % 5;
		lastPlatformRightZ = platformsRight[i].z;
		lastPlatformRightScaleZ = platformsRight[i].scaleZ;

		// left
		platformsLeft[i].x = -scaleX;
		platformsLeft[i].y = 0.75f;
		platformsLeft[i].scaleZ = rand() % 8 + 3;
		platformsLeft[i].z = lastPlatformLeftZ - lastPlatformLeftScaleZ / 2.f - platformsLeft[i].scaleZ / 2.f - rand() % 2;
		platformsLeft[i].color = rand() % 5;
		lastPlatformLeftZ = platformsLeft[i].z;
		lastPlatformLeftScaleZ = platformsLeft[i].scaleZ;

		// center
		platformsCenter[i].x = 0;
		platformsCenter[i].y = 0.75f;
		platformsCenter[i].scaleZ = rand() % 8 + 3;
		platformsCenter[i].z = lastPlatformCenterZ - lastPlatformCenterScaleZ / 2.f - platformsCenter[i].scaleZ / 2.f - rand() % 2;
		platformsCenter[i].color = rand() % 5;
		lastPlatformCenterZ = platformsCenter[i].z;
		lastPlatformCenterScaleZ = platformsCenter[i].scaleZ;
	}
}

void Skyroads::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Skyroads::GeneratePlatforms(platform &platform) {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(platform.x, platform.y,
		platform.z));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX, scaleY, platform.scaleZ));
	if (platform.color == 0) {
		// red
		RenderSimpleMesh(meshes["cube"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1, 0, 0));
	}
	else if (platform.color == 1) {
		// yellow
		RenderSimpleMesh(meshes["cube"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1, 1, 0));
	}
	else if (platform.color == 2) {
		// orange
		RenderSimpleMesh(meshes["cube"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1, 0.5f, 0));
	}
	else if (platform.color == 3) {
		// green
		RenderSimpleMesh(meshes["cube"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0, 1, 0));
	} else if (platform.color == 4) {
		// blue
		RenderSimpleMesh(meshes["cube"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0, 0, 1));
	} else if (platform.color == 5) {
		// purple
		RenderSimpleMesh(meshes["cube"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1, 0, 1));
	}
}

void Skyroads::checkPlatformIsOut(std::vector<struct platform>& platformsVector) {
	if (platformsVector[0].z - platformsVector[0].scaleZ / 2.f >= sphere.z + 0.2f) {
		platform aux = platformsVector[0];
		for (int i = 0; i < platforms - 1; i++) {
			platformsVector[i] = platformsVector[i + 1];
		}

		aux.scaleZ = rand() % 8 + 3;

		aux.z = platformsVector[platforms - 2].z - platformsVector[platforms - 2].scaleZ / 2.f -
			aux.scaleZ / 2.f - rand() % 2;
		aux.color = rand() % 5;
		platformsVector[platforms - 1] = aux;

		GeneratePlatforms(aux);

		//render again left platforms
		for (int i = 0; i < platforms - 1; i++) {
			GeneratePlatforms(platformsVector[i]);
		}
	}
}

void Skyroads::Update(float deltaTimeSeconds)
{
	if (cameraType != 0) {
		setFirstPerson();
	}
	else {
		setThirdPerson();
	}

	timeToLive++;

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(sphere.x, sphere.y, sphere.z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
		if (deformation == true) {
			deformationTime++;
			RenderCustomMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.752941f, 0.752941f, 0.752941f));
			if (deformationTime > 200) {
				deformationTime = 0;
				deformation = false;
			}
		}
		else {
			RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.752941f, 0.752941f, 0.752941f));
		}
	}

	currentFuel -= stepFuelBar;

	if (currentFuel < 0) {
		currentFuel = 0;
	}

	for (int i = 0; i < platforms && gameOverFall == false; i++) {
		platformsLeft[i].z += platformsSpeed * deltaTimeSeconds;
		platformsRight[i].z += platformsSpeed * deltaTimeSeconds;
		platformsCenter[i].z += platformsSpeed * deltaTimeSeconds;

		// left platforms
		GeneratePlatforms(platformsLeft[i]);

		// center platforms
		GeneratePlatforms(platformsCenter[i]);

		// right platforms
		GeneratePlatforms(platformsRight[i]);

		// if left platform is out
		checkPlatformIsOut(platformsLeft);

		//if center platform is out
		checkPlatformIsOut(platformsCenter);

		//if right platform is out
		checkPlatformIsOut(platformsRight);
	}


	if (speedActivated == true) {
		platformsSpeed = 6;
		time++;
	}

	if (time > 800 && speedActivated == true) {
		speedActivated = false;
		platformsSpeed = 2;
		time = 0;
	}

	// verificare z pe platforme
	if (platformNumber == -1 && sphere.z > platformsLeft[0].z - platformsLeft[0].scaleZ / 2.f - 0.5f &&
		sphere.z < platformsLeft[0].z + platformsLeft[0].scaleZ / 2.f + 0.5f) {

		if (platformsLeft[0].color == 0 && isSpehereInAir == false && timeToLive > 400) {
			cout << "GAME OVER!" << "\n";
			exit(0);
		}

		// +- combustibil
		// yellow
		if (platformsLeft[0].color == 1 && isSpehereInAir == false && timeToLive > 200) {
			currentFuel -= 0.25f;
		}
		else if (platformsLeft[0].color == 3 && isSpehereInAir == false) {
			currentFuel += 0.5f;
			deformation = true;
		} else if (platformsLeft[0].color == 2 && isSpehereInAir == false && timeToLive > 400) {
			speedActivated = true;
		}

		isSphereOnPlatform = true;
		if (isSpehereInAir == false && gameOverFall == false) {
			platformsLeft[0].color = 5;
		}


		
	} else if (platformNumber == 0 && sphere.z > platformsCenter[0].z - platformsCenter[0].scaleZ / 2.f - 0.5f &&
		sphere.z < platformsCenter[0].z + platformsCenter[0].scaleZ / 2.f + 0.5f) {

		if (platformsCenter[0].color == 0 && isSpehereInAir == false && timeToLive > 400) {
			cout << "GAME OVER!" << "\n";
			exit(0);
		}

		// +- combustibil
		if (platformsCenter[0].color == 1 && isSpehereInAir == false && timeToLive > 200) {
			currentFuel -= 0.25f;
		}
		else if (platformsCenter[0].color == 3 && isSpehereInAir == false) {
			deformation = true;
			currentFuel += 0.5f;
		}
		else if (platformsCenter[0].color == 2 && isSpehereInAir == false && timeToLive > 400) {
			speedActivated = true;
		}

		isSphereOnPlatform = true;
		if (isSpehereInAir == false && gameOverFall == false) {
			platformsCenter[0].color = 5;
		}

	} else if (platformNumber == 1 && sphere.z > platformsRight[0].z - platformsRight[0].scaleZ / 2.f - 0.5f &&
		sphere.z < platformsRight[0].z + platformsRight[0].scaleZ / 2.f + 0.5f) {

		if (platformsRight[0].color == 0 && isSpehereInAir == false && timeToLive > 400) {
			cout << "GAME OVER!" << "\n";
			exit(0);
		}

		// +- combustibil
		if (platformsRight[0].color == 1 && isSpehereInAir == false && timeToLive > 200) {
			currentFuel -= 0.25f;
		}
		else if (platformsRight[0].color == 3 && isSpehereInAir == false) {
			deformation = true;
			currentFuel += 0.5f; 
		}
		else if (platformsRight[0].color == 2 && isSpehereInAir == false && timeToLive > 400) {
			speedActivated = true;
		}

		isSphereOnPlatform = true;
		if (isSpehereInAir == false && gameOverFall == false) {
			platformsRight[0].color = 5;
		}

	} else {
		if (isSpehereInAir == false && timeToLive > 400) {
			gameOverFall = true;
			sphere.y -= 0.5f * deltaTimeSeconds;
		}
	}

	// verificare y pe platforme
	if (platformNumber == -1 && sphere.y > platformsLeft[0].y + 0.25f) {
		isSpehereInAir = true;
	} else if (platformNumber == 0 && sphere.y > platformsCenter[0].y + 0.25f) {
		isSpehereInAir = true;
	} else if (platformNumber == 1 && sphere.y > platformsRight[0].y + 0.25f) {
		isSpehereInAir = true;
	} else {
		isSpehereInAir = false;
	}
	
	if (platformsSpeed < 2) {
		platformsSpeed = 2;
	}

	if (upward == true) { // daca e in urcare
		sphere.y += 0.5f * deltaTimeSeconds;
	}
	else {				// daca e in coborare
		sphere.y -= 0.5f * deltaTimeSeconds;
	}

	if (sphere.y > 1.2f) { // daca a ajuns la o intaltimea maxima
		upward = false;
	}

	if (sphere.y < 0.9f + scaleY && gameOverFall == false) { // sa nu coboare prea mult
		sphere.y = 0.9f + scaleY;
	}

	if (sphere.y < -0.3f && timeToLive > 400) {
		cout << "GAME OVER!" << "\n";
		exit(0);
	}

	// first person
	if (cameraType != 0) {
		glm::mat3 modelMatrix = glm::mat3(1);
		modelMatrix *= Transform::Translate(sphere.x, sphere.y + 0.225f);
		modelMatrix *= Transform::Scale(currentFuel, 0.5f);
		RenderMesh2D(meshes["fuel"], shaders["VertexColor"], modelMatrix);
	}
	else {
		glm::mat3 modelMatrix = glm::mat3(1);
		modelMatrix *= Transform::Translate(1.6f, 2.71f);
		modelMatrix *= Transform::Scale(currentFuel, 1);
		RenderMesh2D(meshes["fuel"], shaders["VertexColor"], modelMatrix);
	}


	// daca nu in afara platformelor
	if (sphere.x != 0 && sphere.x != scaleX && sphere.x != -scaleX && timeToLive > 400) {
		cout << "GAME OVER!" << "\n";
		exit(0);
	}

	if (currentFuel <= 0) {
		exit(0);
	}

}

void Skyroads::FrameEnd()
{
	DrawCoordinatSystem();
}

void Skyroads::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);
	int location;

	location = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(location, color.x, color.y, color.z);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	GLint t = glGetUniformLocation(shader->program, "time");
	glUniform1f(t, 0);

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

void Skyroads::RenderCustomMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// render an object using the specified shader and the specified position
	glUseProgram(shader->program);
	int location;

	location = glGetUniformLocation(shader->program, "object_color");
	glUniform3f(location, color.x, color.y, color.z);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	GLint t = glGetUniformLocation(shader->program, "time");
	glUniform1f(t, (GLfloat)Engine::GetElapsedTime());

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Skyroads::OnInputUpdate(float deltaTime, int mods)
{
	
}

void Skyroads::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_C) {
		cameraType++;
		cameraType %= 2;
	}

	// add key press event
	if (key == GLFW_KEY_W && speedActivated == false) {
		platformsSpeed += 3.f;
	}
	
	else if (key == GLFW_KEY_S && speedActivated == false) {
		platformsSpeed -= 3.f;
	}

	if (key == GLFW_KEY_A) {
		sphere.x -= scaleX;
		platformNumber -= 1;
	}
	else if (key == GLFW_KEY_D) {
		sphere.x += scaleX;
		platformNumber += 1;
	}

	if (key == GLFW_KEY_SPACE && isSpehereInAir == false) {
		//sphere.y += 0.4f;
		upward = true;
		isSpehereInAir = true;
	}
}

void Skyroads::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Skyroads::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Skyroads::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Skyroads::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Skyroads::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Skyroads::OnWindowResize(int width, int height)
{
}
