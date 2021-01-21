#version 430 core

uniform vec3 objectColor;
//uniform vec3 lightDir;
uniform vec3 cameraPos;
uniform vec3 lightPos;

in vec3 interpNormal;
in vec3 fragPos;

void main()
{
	vec3 lightDir = normalize(fragPos - lightPos);
	vec3 V = normalize(cameraPos - fragPos);
	vec3 N = normalize(interpNormal);

	vec3 R = reflect(lightDir, N);

	float diffuse = max(0, dot(N, lightDir));
	
	float specular_pow = 100;
	float specular = pow(max(0, dot(R, V)), specular_pow);

	vec3 lightColor = vec3(1);
	vec3 shadedColor = objectColor * diffuse + lightColor * specular;
	
	float ambient = 0.2;
	gl_FragColor = vec4(objectColor * shadedColor + vec3(1.0) * specular, 1.0);
}
