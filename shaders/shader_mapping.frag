#version 430 core

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec3 fragPos;
in vec2 interpTexCoord;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

void main()
{
	vec3 normal = texture2D(normalSampler, interpTexCoord).rgb;
	normal = normalize(normal*2-1);
	vec3 color = texture2D(textureSampler, interpTexCoord).rgb;

	// ambient
	vec3 ambient = 0.2 * color;

	// diffuse
	vec3 lightDir = normalize(TangentLightPos-TangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;

	// specular
	vec3 viewDir = normalize(TangentViewPos-TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  

    float spec = pow(max(dot(normal, halfwayDir), 0.0), 20.0);
    vec3 specular = vec3(0.5) * spec;

    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}