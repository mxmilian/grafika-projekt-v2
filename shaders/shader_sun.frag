#version 430 core

uniform sampler2D textureSampler;
uniform vec3 lightPos;
uniform vec3 cameraPos;

in vec2 interpTexCoord;
in vec3 interpNormal;
in vec3 fragPos;

void main()
{
	vec3 normal = normalize(interpNormal);
	vec3 view = normalize(cameraPos-fragPos);
	float coef = max(0,dot(view,normal));

	vec3 texture = texture2D(textureSampler, interpTexCoord).rgb;
	vec3 color =  vec3(1,0.5,0.1);

	gl_FragColor = vec4(mix(texture, color, 1-coef), 1.0);
}
