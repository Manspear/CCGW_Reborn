#include "Billboard.h"

void BillboardProgram::use()
{
	glUseProgram( mProgramID );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void BillboardProgram::unUse()
{
	glUseProgram( 0 );
	glDisable(GL_BLEND);
}

void BillboardProgram::begin( Camera* camera )
{
	glBindVertexArray( mVertexArray );
	glUniformMatrix4fv( mViewLocation, 1, GL_FALSE, &camera->getView()[0][0]);
	glUniformMatrix4fv( mProjectionLocation, 1, GL_FALSE, &camera->getPerspective()[0][0]);
}

void BillboardProgram::end()
{
	glBindVertexArray(0);
}

void BillboardProgram::draw( Camera* camera, glm::vec3 position, glm::vec2 size )
{
	//glm::mat4 world= glm::translate( glm::mat4(), position );
	glm::mat4 world;
	world[3][0] = position.x;
	world[3][1] = position.y;
	world[3][2] = position.z;
	glUniformMatrix4fv( mWorldLocation, 1, GL_FALSE, &world[0][0] );
	glUniform2f( mSizeLocation, size.x, size.y );
	
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
}

BillboardProgram& BillboardProgram::operator=( const BillboardProgram& ref )
{
	throw -1;
}

BillboardProgram::BillboardProgram( const BillboardProgram& ref )
{
	throw -1;
}

BillboardProgram::BillboardProgram()
{
	throw -1;
}

BillboardProgram::BillboardProgram( const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath )
{
	compileShaders( vertexPath, fragmentPath, geometryPath );

	glUseProgram( mProgramID );
	mWorldLocation = glGetUniformLocation( mProgramID, "world" );
	mViewLocation = glGetUniformLocation( mProgramID, "view" );
	mProjectionLocation = glGetUniformLocation( mProgramID, "projection" );
	mSizeLocation = glGetUniformLocation( mProgramID, "size" );
	glUseProgram( 0 );
	
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glEnableVertexAttribArray(0);

	const GLfloat vdata[] = { 0.0f, 0.0f, 0.0f };
	const GLuint idata = 0;

	GLuint vbo, ibo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3, vdata, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint), &idata, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
}

BillboardProgram::~BillboardProgram()
{
	glDeleteVertexArrays(1, &mVertexArray);
}