#pragma once

#include <string>
#include <include/glm.h>
#include <Core/GPU/Mesh.h>
#include <gl\GL.h>

namespace Transform
{
	// Translate matrix
	inline glm::mat3 Translate(float translateX, float translateY)
	{
		return glm::transpose(
			glm::mat3(1, 0, translateX,
				0, 1, translateY,
				0, 0, 1));
	}

	// Scale matrix
	inline glm::mat3 Scale(float scaleX, float scaleY)
	{
		return glm::mat3(scaleX, 0, 0,
			0, scaleY, 0,
			0, 0, 1);
	}

	// Rotate matrix
	inline glm::mat3 Rotate(float radians)
	{
		float c = cos(radians);
		float s = sin(radians);
		return glm::transpose(
			glm::mat3(c, -s, 0,
				s, c, 0,
				0, 0, 1));
	}

	// linie
	Mesh* CreateRectangle(std::string name, float length, float width, glm::vec3 color, bool fill)
	{
		std::vector<VertexFormat> vertices =
		{
			VertexFormat(glm::vec3(0, 0, 0), color),
			VertexFormat(glm::vec3(length, 0, 0), color),
			VertexFormat(glm::vec3(length, width, 0), color),
			VertexFormat(glm::vec3(0, width, 0), color)
		};

		Mesh* square = new Mesh(name);
		std::vector<unsigned short> indices = { 0, 1, 2, 3 };

		if (!fill) {
			square->SetDrawMode(GL_LINE_LOOP);
		}
		else {
			indices.push_back(0);
			indices.push_back(2);
		}

		square->InitFromData(vertices, indices);
		return square;
	}
}