#version 460 core
#define DIRECTIONAL_LIGHT 0
#define SPOTLIGHT 1
#define POINTLIGHT 2

struct Light
{
	vec3 position;
	vec3 direction;
	vec3 color;
	float spotlightAngle;
	float radius;
	float intensity;
	float linear;
	float quadratic;
	bool enable;
	uint type;
};

out vec4 FragColor;

uniform sampler2D gPos;
uniform sampler2D gDiffuseSpec;
uniform sampler2D gNormal;


uniform Light light;
uniform vec3 viewPos;

void main()
{

	vec2 TexCoord = gl_FragCoord.xy/vec2(1920, 1080); //doesnt matter which tex, they are all the same size

	vec3 FragPos = texture(gPos, TexCoord).rgb;
	vec3 Normal = texture(gNormal, TexCoord).rgb;
	vec3 Diffuse = texture(gDiffuseSpec, TexCoord).rgb;
	float Specular = texture(gDiffuseSpec, TexCoord).a;

	vec3 result = Diffuse * 0.1;
	vec3 viewDir = normalize(viewPos - FragPos);

	if(light.type == POINTLIGHT)
	{
		vec3 lightDir = normalize(light.position - FragPos);
		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.color;

		vec3 halfwayDir = normalize(lightDir + viewDir);
		float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
		vec3 specular = light.color * spec * Specular;

//		//attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (1.0 + light.linear * distance + light.quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
		result += diffuse + specular;
	}
	else if(light.type == SPOTLIGHT)
	{
	}
	else if(light.type == DIRECTIONAL_LIGHT)
	{
	}
	else
	{
		result = vec3(0.0, 1.0, 0.0);
	}

	FragColor = vec4(result, 1.0);
}