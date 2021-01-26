#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoord;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

uniform mat4 transformation;
uniform mat4 modelMatrix;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec3 fragPos;
out vec2 interpTexCoord;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

void main()
{
	fragPos = vec3(modelMatrix * vec4(vertexPosition,1.0));
	interpTexCoord = vertexTexCoord;

	mat3 normalMatrix=transpose(inverse(mat3(modelMatrix)));
	
	vec3 T = normalize(normalMatrix * vertexTangent);
	vec3 N = normalize(normalMatrix * vertexNormal);
	T = normalize (T- dot(T,N)*N);
	vec3 B = cross(N,T);

	mat3 TBN = transpose(mat3(T, B, N));

	TangentLightPos = TBN * lightPos;
	TangentViewPos  = TBN * cameraPos;
	TangentFragPos  = TBN * fragPos;

	gl_Position = transformation * vec4(vertexPosition, 1.0);
}
