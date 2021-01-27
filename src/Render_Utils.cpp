#include "Render_Utils.h"

#include <algorithm>

#include "glew.h"
#include "freeglut.h"
#include "SkyBox.cpp"

void Core::DrawVertexArray(const float * vertexArray, int numVertices, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void Core::DrawVertexArrayIndexed( const float * vertexArray, const int * indexArray, int numIndexes, int elementSize )
{
	glVertexAttribPointer(0, elementSize, GL_FLOAT, false, 0, vertexArray);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, numIndexes, GL_UNSIGNED_INT, indexArray);
}


void Core::DrawVertexArray( const VertexData & data )
{
	int numAttribs = std::min(VertexData::MAX_ATTRIBS, data.NumActiveAttribs);
	for(int i = 0; i < numAttribs; i++)
	{
		glVertexAttribPointer(i, data.Attribs[i].Size, GL_FLOAT, false, 0, data.Attribs[i].Pointer);
		glEnableVertexAttribArray(i);
	}
	glDrawArrays(GL_TRIANGLES, 0, data.NumVertices);
}

void Core::DrawModel( obj::Model * model )
{
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, &model->vertex[0]);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, &model->texCoord[0]);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, &model->normal[0]);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	unsigned short * tmp = &model->faces["default"][0];
	glDrawElements(GL_TRIANGLES, model->faces["default"].size(), GL_UNSIGNED_SHORT, tmp);
}

void Core::DrawSkyBox(GLuint textureID)
{
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);


	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);
	glDepthFunc(GL_LESS);

}