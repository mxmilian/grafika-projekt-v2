#version 430 core

uniform vec3 objectColor;
uniform vec3 sunPos;

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;

uniform vec3 cameraPos;
uniform vec3 cameraFront;
uniform vec3 cameraFront2;

in vec3 interpNormal;
in vec3 fragPos;
in vec2 interpTexCoord;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;


vec3 CalcMapping();
vec3 CalcSpotLight(vec3 lightPosition, vec3 lightDirection);

void main()
{
	vec3 result = CalcMapping();
    result += CalcSpotLight(cameraPos, cameraFront);
    result += CalcSpotLight(cameraPos, cameraFront2);
	gl_FragColor = vec4(result, 1.0);
}

vec3 CalcMapping()
{
	vec3 normal = texture2D(normalSampler, interpTexCoord).rgb;
	normal = normalize(normal*2-1);
	vec3 textureLoad = texture2D(textureSampler, interpTexCoord).rgb;

	// ambient
	vec3 ambient = 0.2 * textureLoad;

	// diffuse
	vec3 lightDir = normalize(TangentLightPos-TangentFragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * textureLoad;

	// specular
	vec3 viewDir = normalize(TangentViewPos-TangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  

    float spec = pow(max(dot(normal, halfwayDir), 0.0), 20.0);
    vec3 specular = vec3(0.5) * spec;

    return(ambient + diffuse + specular);
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
	float spec = pow(max(0,dot(reflecting,view)),20);

    // attenuation
    float distance = length(lightPosition - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));  // attenuation = 1.0 / (light constant + light linear * distance + light quadratic * (distance * distance));  

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