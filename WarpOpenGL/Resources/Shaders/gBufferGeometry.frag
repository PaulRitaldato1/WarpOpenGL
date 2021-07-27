#version 460 core

layout (location = 0) out vec3 gPos;
layout (location = 1) out vec4 gDiffuseSpec;
layout (location = 2) out vec3 gNormal;

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

bool any(vec3 vec)
{
    if(vec.r + vec.g + vec.b > 0)
        return true;
    else
        return false;
}

vec3 expandNormal(vec3 n)
{
    return n * 2.0 - 1.0;
}

vec3 calcNormalMapping(mat3 TBN, sampler2D normalTex, vec2 uv)
{
    vec3 normal = texture(normalTex, uv).xyz;
    normal = expandNormal(normal);

    normal = TBN * normal;
    return normalize(normal);
}

in vec2 TexCoords;
in vec3 Normal;
in vec3 Tangent;
in vec3 Binormal;
in vec3 FragPos;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_specular0;
uniform sampler2D texture_emissive0;
uniform sampler2D texture_ambient0;
uniform sampler2D texture_normal0;

uniform Material mat;

void main()
{
    vec4 diffuse = mat.DiffuseColor;
    if(mat.HasDiffuseTexture)
    {
        vec4 diffuseFromTex = texture(texture_diffuse0, TexCoords);
        if(any(diffuse.rgb))
        {
            diffuse *= diffuseFromTex;
        }
        else
        {
            diffuse = diffuseFromTex;
        }
    }

//    vec4 specular = mat.SpecularColor;
//    float specularPower = mat.SpecularPower;
//    if(mat.HasSpecularTexture)
//    {
//        vec4 specularFromTex = texture(texture_specular0, TexCoords);
//        specularPower = specularFromTex.r;
//        if(any(specular.rgb))
//        {
//            specular *= specularFromTex;
//        }
//        else
//        {
//            specular = specularFromTex;
//        }
//    }
//
    vec3 N = normalize(Normal);
    if(mat.HasNormalTexture)
    {
        mat3 TBN = mat3(normalize(Tangent), normalize(Binormal), normalize(Normal));
        N = calcNormalMapping(TBN, texture_normal0, TexCoords);
    }

    gPos = FragPos;
    gNormal = N;
    gDiffuseSpec.rgb = diffuse.rgb;
    gDiffuseSpec.a = texture(texture_specular0, TexCoords).r;
}