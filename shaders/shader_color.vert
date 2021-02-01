#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 transformation;
uniform mat4 modelMatrix;

out vec3 interpNormal;
out vec3 fragPos;
out vec2 vertexCoord;

void main()
{
	gl_Position = transformation * vec4(vertexPosition, 1.0);
	interpNormal = normalize(vec3(modelMatrix * vec4(vertexNormal,0.0)));
	fragPos = vec3(modelMatrix * vec4(vertexPosition,1.0));

	//vertexCoord = -vertexTexCoord;
}
