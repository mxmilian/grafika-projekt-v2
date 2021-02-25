#include "Render_Utils.h"

#include <algorithm>

#include "glew.h"
#include "freeglut.h"
#include "SkyBox.cpp"

void Core::RenderContext::initFromOBJ(obj::Model& model)
{
	vertexArray = 0;
	vertexBuffer = 0;
	vertexIndexBuffer = 0;
	unsigned int vertexDataBufferSize = sizeof(float) * model.vertex.size();
	unsigned int vertexNormalBufferSize = sizeof(float) * model.normal.size();
	unsigned int vertexTexBufferSize = sizeof(float) * model.texCoord.size();

	size = model.faces["default"].size();
	unsigned int vertexElementBufferSize = sizeof(unsigned short) * size;

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexElementBufferSize, &model.faces["default"][0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBufferData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize + vertexTexBufferSize, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataBufferSize, &model.vertex[0]);

	glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize, vertexNormalBufferSize, &model.normal[0]);

	glBufferSubData(GL_ARRAY_BUFFER, vertexDataBufferSize + vertexNormalBufferSize, vertexTexBufferSize, &model.texCoord[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertexDataBufferSize));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(vertexNormalBufferSize + vertexDataBufferSize));
}

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

void Core::DrawContext(Core::RenderContext& context)
{

	glBindVertexArray(context.vertexArray);
	glDrawElements(
		GL_TRIANGLES,			// mode
		context.size,			// count
		GL_UNSIGNED_SHORT,		// type
		(void*)0				 // element array buffer offset
	);
	glBindVertexArray(0);
}
