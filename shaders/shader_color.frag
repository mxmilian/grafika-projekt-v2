#version 430 core

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec2 interpTexCoord;
in vec3 interpNormal;
in vec3 vertPos;

void main()
{
	vec3 lightDir = normalize(lightPos-vertPos);
	vec3 V = normalize(cameraPos-vertPos);
	vec3 normal = normalize(interpNormal);
	vec3 R = reflect(-lightDir,normal);
	
	vec3 color = texture2D(textureSampler, interpTexCoord).rgb;

	float specular = pow(max(0,dot(R,V)),10);
	float diffuse = max(0,dot(normal,lightDir));
	float ambient = 0.1;


	gl_FragColor.rgb = mix(color, color * diffuse + vec3(1) * specular, 1.0 - ambient);
	gl_FragColor.a = 1.0;
}