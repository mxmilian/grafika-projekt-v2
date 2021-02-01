#version 430 core

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec3 interpNormal;
in vec3 fragPos;
in vec2 vertexCoord;

void main()
{
	vec3 lightDir = normalize(lightPos-fragPos);
	vec3 view = normalize(cameraPos-fragPos);
	vec3 normal = normalize(interpNormal);
	vec3 reflecting = reflect(-lightDir,normal);
	
	float specular = pow(max(0,dot(reflecting,view)),100);
	float diffuse = max(0,dot(normal, lightDir));
	float ambient = 0.1;
	gl_FragColor = vec4(objectColor * diffuse + vec3(1) * specular, 1.0 - ambient);
}
