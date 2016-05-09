
#include <GL\glew.h>
#include <fstream>
#include <vector>
#include <iostream>
#include "ReadHeaderData.h"
#include "BinaryReader.h"

using namespace std;

void BinaryReader::readFromBinary(GLuint &vao)
{

	//Read from binary


	std::ifstream infile("testBin.bin", std::ifstream::binary);//		�ppnar filen vi nyss skapade men ska nu l�sa ist�llet



	infile.read((char*)&read_mainHeader, sizeof(read_sMainHeader));//				Information av hur m�nga meshes som senare kommer att komma, och efter det hur m�nga material osv, samt hur mycket minne den inten som ber�ttar detta tar upp (reservation f�r v�r header)
	//cout << mainHeader.lightCount << endl;
	//cout << mainHeader.cameraCount << endl;


	for (int i = 0; i < read_mainHeader.meshCount; i++)
	{
		cout << "Mesh: " << i << endl;

		read_meshList.resize(read_mainHeader.meshCount);

		infile.read((char*)&read_meshList[i], sizeof(read_sMesh));//													Information av hur m�nga vertices som senare kommer att komma, och efter det hur m�nga skelAnim verticear som kommer komma osv, samt hur mycket minne den inten som ber�ttar detta tar upp(reservation f�r v�r header).En int kommer f�rst, den har v�rdet 100.  Och den inten kommer ta upp 4 bytes.


		read_mList.resize(read_mainHeader.meshCount);
		read_mList[i].vList.resize(read_meshList[i].vertexCount);

		read_mList[i].vList.resize(read_meshList[i].vertexCount);

		infile.read((char*)read_mList[i].vList.data(), sizeof(read_sVertex) * read_meshList[i].vertexCount);//				Skriver ut alla vertices i f�r vArray, pos, nor, rgba 100 g�nger. Och minnet 100 Vertices tar upp.

																											//cout << "SkelAnimVert vector: NULL" << endl;

																											//cout << "Joint vector: NULL" << endl;
		GLuint vertexBuff;


		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// It w�rks
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);

		glGenBuffers(1, &vertexBuff);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);
		glBufferData(GL_ARRAY_BUFFER, sizeof(read_sVertex) * read_mList[i].vList.size(), read_mList[i].vList.data(), GL_STATIC_DRAW);
	
	}

	read_materialList.resize(read_mainHeader.materialCount);

	for (int i = 0; i < read_mainHeader.materialCount; i++)
	{
		infile.read((char*)&read_materialList[i], sizeof(read_sMaterial) * read_mainHeader.materialCount);//				Information av hur m�nga material som senare kommer att komma, samt hur mycket minne den inten som ber�ttar detta tar upp.
	}
	infile.close();




}