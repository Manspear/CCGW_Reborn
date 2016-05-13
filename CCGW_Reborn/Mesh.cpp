#include "Mesh.h"

// NOTE: Debug structs, don't ship
struct objFace
{
	int p1, u1, n1;
	int p2, u2, n2;
	int p3, u3, n3;
};

struct objVertex
{
	float px, py, pz;
	float u, v;
	float nx, ny, nz;
};

bool Mesh::load( Assets* assets, string file )
{
	/**
	What will happen when you've more than one mesh in the same mesh-thing?
	Add the vertex vectors to a vector containing vertex vectors? THis way meshes get separated.
	But do we want them separated? For us to have as few draw-calls as possible we've gotta have
	a "combined" array of vertices.
	The problem with having a combined array though is that the loading time will get much higher.
	The reason behind increased loading times is because of the way we do "relationship stuff" between objects.
	When an object has a "child thing" it holds an index to that child.
	And when we increase the size of our list, we must also change the indexes that refer to children.
	That means that in order to keep the same structure we've gotta "add the old indexes" on top of the current index.

	All right. Now I've read myself up about the topic of draw calls.
	Apparently you typically have one draw call per "independently moving mesh", and one draw call for
	the entire static scene. This works since static scene objects needn't worry as much about
	"relativity stuff".

	For dynamic meshes it makes sense that they've one draw call each, since if we wanna have the "index to children"
	way to handle relativity stuff we gotta think about the fact that these meshi have four joint indices per vertex.

	How to automagically determine if a "meshCluster" is an animated mesh? By looping through all meshi, and checking the "isAnimated" flag.
	If isAnimated == true for ANY mesh, treat is as a character. A character gets it's own draw call.
	**/

	bool result = false;
	MoleReader moleImporter;
	moleImporter.readFromBinary(file.c_str());

	const std::vector<sVertex>* vertices;

	const sMainHeader header = moleImporter.getMainHeader();
	const int meshCount = header.meshCount;

	bool isDynamicModel = false;

	for (int meshCounter = 0; meshCounter < meshCount; meshCounter++)
	{
		const sMesh checkM = moleImporter.getMesh(meshCounter);
		if (checkM.isAnimated)
		{
			isDynamicModel = true;
			break;
		}
	}

	if (isDynamicModel)
	{
		/**
		If the model is dynamic, i.e. animated, it will get it's own draw call.
		Meaning that the contents of THIS FILE will have it's own draw call,
		and not get mashed together into a "global vertex list".

		Hmm... We do need to separate between models to make many things work,
		but we'll need their vertices to be unified when putting them into the
		vertex buffer, or else we'll need one draw call for every single mesh.
		**/
	}

	std::vector<sVertex> vertexList;

	if (isDynamicModel == false)
	{
		/**
		If the model is static, it's okay to mash it into a lump of vertices.
		Basically a static model is one with no joint animation.
		**/

		for (int meshCounter = 0; meshCounter < meshCount; meshCounter++)
		{
			const std::vector<sVertex> vHolder = moleImporter.getVertexList(meshCounter);
			for (int vC = 0; vC < vHolder.size(); vC++) {
				vertexList.push_back(vHolder[vC]);
			}
		}

	}
	vertexList;
	int pp = 5;

	//Making a "fake" index list.
	//std::vector<GLuint> indices;
	//for (int i = 0; i < vertexList.size(); i++)
	//{
	//	indices.push_back(i);
	//}

	/*glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);*/

	// It wörks
	/*glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);*/

	mSize = vertexList.size();
	glGenBuffers(1, &mVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sVertex)*vertexList.size(), vertexList.data(), GL_STATIC_DRAW);

	/*glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW);
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 8));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 11));

	//glBindVertexArray(0);
	result = true;
	return result;
}

bool Mesh::load( MoleReader* reader, int index )
{
	bool result = false;

	//const std::vector<sVertex>* vertexList = reader->getVertexList( index );
	const sMesh mesh = reader->getMesh( index );

	if( mesh.vertexCount > 0 )
	{
		std::vector<sVertex> vertexList = reader->getVertexList( index );

		glGenBuffers(1, &mVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sVertex) * vertexList.size(), vertexList.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 6));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 8));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 11));

		mSize = vertexList.size();
		mMaterialIndex = mesh.materialID;
		result = true;
		
		vertexList.clear();
	}
	
	else if (mesh.skelAnimVertexCount > 0)
	{
		std::vector<sSkelAnimVertex> vertexList = reader->getSkelVertexList(index);

		glGenBuffers(1, &mVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sSkelAnimVertex)*vertexList.size(), vertexList.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray( 0 );
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 6));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 8));
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 11));
		glVertexAttribIPointer(5, 4, GL_INT, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 14));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 14 + sizeof(int)*4));

		mSize = vertexList.size();
		mMaterialIndex = mesh.materialID;
		vertexList.clear();
		result = true;
	}
	return result;
}

void Mesh::unload()
{
	if (mVertexArray > 0)
		glDeleteVertexArrays(1, &mVertexArray);
	if (mVertexBuffer > 0)
		glDeleteBuffers(1, &mVertexBuffer);
	if (mIndexBuffer > 0)
		glDeleteBuffers(1, &mIndexBuffer);
	mVertexArray = mVertexBuffer = mIndexBuffer = 0;
}

void Mesh::draw() 
{
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 8));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 11));
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 14));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(sSkelAnimVertex), (void*)(sizeof(float) * 14+sizeof(int)*4));
	glDrawArrays(GL_TRIANGLES, 0, mSize);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::drawNonAni()
{
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 6));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 8));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(sVertex), (void*)(sizeof(float) * 11));
	glDrawArrays(GL_TRIANGLES, 0, mSize);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int Mesh::getMaterialIndex() const
{
	return mMaterialIndex;
}

Mesh& Mesh::operator=(const Mesh& ref)
{
	mVertexArray = ref.mVertexArray;
	mVertexBuffer = ref.mVertexBuffer;
	mIndexBuffer = ref.mIndexBuffer;
	return *this;
}

Mesh::Mesh(const Mesh& ref)
	: mVertexArray(ref.mVertexArray), mVertexBuffer(ref.mVertexBuffer), mIndexBuffer(ref.mIndexBuffer)
{
}

Mesh::Mesh()
	: mVertexArray(0), mVertexBuffer(0), mIndexBuffer(0), mSize(0)
{
}

Mesh::~Mesh()
{
	
}
