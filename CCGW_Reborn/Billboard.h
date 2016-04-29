#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "ShaderProgram.h"
#include "Camera.h"

class BillboardProgram : public ShaderProgram
{
public:
	/*Sets this shader as the active one.*/
	void use();
	/*Sets the active shader to 0.*/
	void unUse();
	/*Draw a billboard with the specified position and scale.*/
	void draw( Camera* camera, glm::vec3 position, glm::vec2 size );
	/*Binds items for the GPU up for rendering billboards.*/
	void begin( Camera* camera );
	/*Unbinds items for the GPU when rendering of billboards is no longer required.*/
	void end();

	BillboardProgram& operator=( const BillboardProgram& ref );
	BillboardProgram( const BillboardProgram& ref );
	BillboardProgram();
	BillboardProgram( const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath );
	~BillboardProgram();

private:
	GLuint mVertexArray;
	GLuint mWorldLocation, mViewLocation, mProjectionLocation, mSizeLocation;
};