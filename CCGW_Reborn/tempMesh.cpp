#include "tempMesh.h"

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

bool tempMesh::load(string file)
{
	bool result = false;

	std::ifstream stream(file);
	if (stream.is_open())
	{
		std::vector<glm::vec3> v;
		std::vector<glm::vec2> vt;
		std::vector<glm::vec3> vn;
		std::vector<objFace> faces;

		while (stream)
		{
			// read the file line by line
			std::string line;
			std::getline(stream, line);

			if( line.size() > 0 )
			{
				std::stringstream ss(line);
				if (line[0] == 'v')
				{
					ss.ignore();

					if (line[1] == 't') // add texture coordinates
					{
						ss.ignore();

						glm::vec2 uv;
						ss >> uv.x >> uv.y;

						vt.push_back(uv);
					}
					else if (line[1] == 'n') // add vertex normal
					{
						ss.ignore();

						glm::vec3 normal;
						ss >> normal.x >> normal.y >> normal.z;

						vn.push_back(normal);
					}
					else // add vertex position
					{
						glm::vec3 vertex;
						ss >> vertex.x >> vertex.y >> vertex.z;

						v.push_back(vertex);
					}
				}
				else if (line[0] == 'f') // add face
				{
					ss.ignore(1);

					objFace face;
					ss >> face.p1 >> face.u1 >> face.n1;
					ss >> face.p2 >> face.u2 >> face.n2;
					ss >> face.p3 >> face.u3 >> face.n3;

					faces.push_back(face);
				}
			}
		}

		stream.close();

		// compile data into an indexed buffer
		std::vector<objVertex> vertices;
		std::vector<GLuint> indices;
		int curIndex = 0;
		for (std::vector<objFace>::const_iterator it = faces.begin(); it != faces.end(); it++)
		{
			
			objVertex v1;
			v1.px = v[it->p1 - 1].x;
			v1.py = v[it->p1 - 1].y;
			v1.pz = v[it->p1 - 1].z;
			v1.u = vt[it->u1 - 1].x;
			v1.v = vt[it->u1 - 1].y;
			v1.nx = vn[it->n1 - 1].x;
			v1.ny = vn[it->n1 - 1].y;
			v1.nz = vn[it->n1 - 1].z;

			objVertex v2;
			v2.px = v[it->p2 - 1].x;
			v2.py = v[it->p2 - 1].y;
			v2.pz = v[it->p2 - 1].z;
			v2.u = vt[it->u2 - 1].x;
			v2.v = vt[it->u2 - 1].y;
			v2.nx = vn[it->n2 - 1].x;
			v2.ny = vn[it->n2 - 1].y;
			v2.nz = vn[it->n2 - 1].z;

			objVertex v3;
			v3.px = v[it->p3 - 1].x;
			v3.py = v[it->p3 - 1].y;
			v3.pz = v[it->p3 - 1].z;
			v3.u = vt[it->u3 - 1].x;
			v3.v = vt[it->u3 - 1].y;
			v3.nx = vn[it->n3 - 1].x;
			v3.ny = vn[it->n3 - 1].y;
			v3.nz = vn[it->n3 - 1].z;

			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);

			for (int i = 0; i<3; i++)
				indices.push_back(curIndex++);
		}
		mSize = faces.size() * 3;
		// Push data to the GPU
		glGenVertexArrays(1, &mVertexArray);
		glBindVertexArray(mVertexArray);

		// It wörks
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &mVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(objVertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &mIndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objVertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(objVertex), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(objVertex), (void*)(sizeof(float) * 5));

		glBindVertexArray(0);
		result = true;
	}
	return result;
}

void tempMesh::unload()
{
	if (mVertexArray > 0)
		glDeleteVertexArrays(1, &mVertexArray);
	if (mVertexBuffer > 0)
		glDeleteBuffers(1, &mVertexBuffer);
	if (mIndexBuffer > 0)
		glDeleteBuffers(1, &mIndexBuffer);
	mVertexArray = mVertexBuffer = mIndexBuffer = 0;
}

void tempMesh::draw() {
	glBindVertexArray(mVertexArray);
	glDrawElements(GL_TRIANGLES, mSize, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

tempMesh& tempMesh::operator=(const tempMesh& ref)
{
	mVertexArray = ref.mVertexArray;
	mVertexBuffer = ref.mVertexBuffer;
	mIndexBuffer = ref.mIndexBuffer;
	return *this;
}

tempMesh::tempMesh(const tempMesh& ref)
	: mVertexArray(ref.mVertexArray), mVertexBuffer(ref.mVertexBuffer), mIndexBuffer(ref.mIndexBuffer)
{
}

tempMesh::tempMesh()
	: mVertexArray(0), mVertexBuffer(0), mIndexBuffer(0), mSize(0)
{
}

tempMesh::~tempMesh()
{
}
