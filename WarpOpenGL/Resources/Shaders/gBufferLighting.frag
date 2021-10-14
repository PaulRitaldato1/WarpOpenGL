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

struct Material
{
    vec4 GlobalAmbient;
    vec4 AmbientColor;
    
    vec4 EmissiveColor;
    vec4 DiffuseColor;
    vec4 SpecularColor;
    
    vec4 Reflectance;
    
    float Opacity;
    float SpecularPower;
    float IndexOfRefraction;
    
    bool HasAmbientTexture;
    bool HasEmissiveTexture;
    bool HasDiffuseTexture;
    bool HasSpecularTexture;
    bool HasSpecularPowerTexture;
    bool HasNormalTexture;
    
    float SpecularScale;
    float AlphaThreshold;
};

struct LightResult
{
	vec3 Diffuse;
	vec3 Specular;
};

vec3 DoDiffuse(Light light, vec3 L, vec3 N)
{
	float nDotL = max(dot(N,L), 0);
	return light.color * nDotL;
}

vec3 DoSpecular(Light light, Material mat, vec3 V, vec3 L, vec3 N)
{
	vec3 R = normalize(reflect(-L, N));
	float rDotV = max(dot(R, V), 0);

	return light.color * pow(rDotV, mat.SpecularPower);
}

float DoAttenuation(Light light, float distance)
{
	return 1.0 - smoothstep(light.radius * 0.75f, light.radius, distance);
}

LightResult DoPointLight(Light light, Material mat, vec3 V, vec3 P, vec3 N, vec3 posVS)
{
	vec3 L = posVS - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);

	LightResult lit;
	lit.Diffuse = DoDiffuse(light, L, N) * attenuation * light.intensity;
	lit.Specular = DoSpecular(light, mat, V, L, N) * attenuation * light.intensity;

	return lit;
}


out vec4 FragColor;

uniform sampler2D gPos;
uniform sampler2D gDiffuseSpec;
uniform sampler2D gNormal;


uniform Light light;
uniform vec3 viewPos;

in mat4 View;
in mat4 Model;

void main()
{

	vec2 TexCoord = gl_FragCoord.xy/textureSize(gPos, 0); //doesnt matter which tex, they are all the same size

	vec3 FragPos = texture(gPos, TexCoord).rgb;
	vec3 N = texture(gNormal, TexCoord).rgb;
	vec3 Diffuse = texture(gDiffuseSpec, TexCoord).rgb;
	float Specular = texture(gDiffuseSpec, TexCoord).a;

	//convert light position to view space
	vec4 lightPosVS = View * Model * vec4(light.position, 1.0);
	vec4 P = vec4(FragPos, 1.0);
	vec4 eyePos = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 V = normalize(eyePos - P);
	
	Material mat;
	mat.SpecularPower = Specular;

	LightResult lit;
	if(light.type == POINTLIGHT)
	{
//		vec3 lightDir = normalize(light.position - FragPos);
//		vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.color;
//
//		vec3 halfwayDir = normalize(lightDir + viewDir);
//		float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
//		vec3 specular = light.color * spec * Specular;
//
////		attenuation
//        float dist = distance(light.position, FragPos);
//        float attenuation = 1.0 / (1.0 + light.linear * dist + light.quadratic * dist * dist);
//        diffuse *= attenuation;
//        specular *= attenuation;
//		result += diffuse + specular;
		lit = DoPointLight(light, mat, V.xyz, P.xyz, N.xyz, lightPosVS.xyz);
	}
	else if(light.type == SPOTLIGHT)
	{
	}
	else if(light.type == DIRECTIONAL_LIGHT)
	{
	}
	else
	{
		lit.Diffuse = vec3(0.0, 1.0, 0.0);
		lit.Specular = vec3(0.0, 1.0, 0.0);
	}

	FragColor = vec4((Diffuse * lit.Diffuse), 1.0) + vec4((Specular * lit.Specular), 1.0);
}