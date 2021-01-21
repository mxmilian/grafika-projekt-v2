#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelMatrix;

uniform vec3 lightDir;
uniform vec3 cameraPos;

out vec2 interpTexCoord;
out vec3 lightDirTS;
out vec3 viewDirTS;

void main()
{
	vec3 vertPos = (modelMatrix * vec4(vertexPosition, 1.0)).xyz;
	
	vec3 normal = (modelMatrix * vec4(vertexNormal, 0.0)).xyz;
	vec3 tangent = (modelMatrix * vec4(vertexTangent, 0.0)).xyz;
	vec3 bitangent = (modelMatrix * vec4(vertexBitangent, 0.0)).xyz;
	
	mat3 TBN = transpose(mat3(tangent, bitangent, normal));

	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0);

	vec3 viewDir = normalize(cameraPos - vertPos);
	lightDirTS = TBN * lightDir;
	viewDirTS = TBN * viewDir;

	interpTexCoord = vertexTexCoord;
}
