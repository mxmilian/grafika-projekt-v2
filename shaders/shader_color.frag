#version 430 core

uniform vec3 objectColor;
uniform vec3 sunPos;

uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform vec3 cameraFront2;

in vec3 interpNormal;
in vec3 fragPos;

vec3 CalcDirLight(vec3 lightPosition);
vec3 CalcSpotLight(vec3 lightPosition, vec3 lightDirection);

void main()
{
	vec3 result = CalcDirLight(sunPos);
    result += CalcSpotLight(cameraPos, cameraFront);
    result += CalcSpotLight(cameraPos, cameraFront2);
	gl_FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(vec3 lightPosition)
{
	vec3 lightDir = normalize(lightPosition-fragPos);
	vec3 view = normalize(cameraPos-fragPos);
	vec3 normal = normalize(interpNormal);

	vec3 reflecting = reflect(-lightDir,normal);
	float specular = pow(max(0,dot(reflecting,view)),32);
	float diffuse = max(0,dot(normal, lightDir));

	return (objectColor * diffuse + vec3(1) * specular);
}

vec3 CalcSpotLight(vec3 lightPosition, vec3 lightDirection)
{
	vec3 lightDir = normalize(lightPosition - fragPos);
    vec3 view = normalize(cameraPos-fragPos);
    vec3 normal = normalize(interpNormal);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflecting = reflect(-lightDir,normal);
	float spec = pow(max(0,dot(reflecting,view)),32);

    // attenuation
    float distance = length(lightPosition - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-lightDirection)); 
    float epsilon = cos(radians(12.5)) - cos(radians(15.0));                            // epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - cos(radians(14.0))) / epsilon, 0.0, 1.0);          // intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = vec3(0.1);
    vec3 diffuse = vec3(1.0) * diff;
    vec3 specular = vec3(1.0) * spec;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}