#version 460 core

layout (location = 0) out vec4 gLightAccumulation;
layout (location = 1) out vec4 gDiffuse;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gSpec;

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
vec4 calcNormalMapping(mat3 TBN, sampler2D normalTex, vec2 uv)
{
    vec3 normal = texture(normalTex, uv).xyz;
    normal = expandNormal(normal);

    normal = TBN * normal;
    return normalize(vec4(normal, 0.0));
}

in vec2 TexCoords;
in vec3 NormalVS;
in vec3 TangentVS;
in vec3 BinormalVS;

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

    vec4 ambient = mat.AmbientColor;
    if(mat.HasAmbientTexture)
    {
        vec4 ambientFromTex = texture(texture_ambient0, TexCoords);
        if(any(ambient.rgb))
        {
            ambient *= ambientFromTex;
        }
        else
        {
            ambient = ambientFromTex;
        }
    }

    ambient *= mat.GlobalAmbient;

    vec4 emissive = mat.EmissiveColor;
    if(mat.HasEmissiveTexture)
    {
        vec4 emissiveFromTex = texture(texture_emissive0, TexCoords);
        if(any(emissive.rgb))
        {
            emissive *= emissiveFromTex;
        }
        else
        {
            emissive = emissiveFromTex;
        }
    }

    vec4 specular = mat.SpecularColor;
    float specularPower = mat.SpecularPower;
    if(mat.HasSpecularTexture)
    {
        vec4 specularFromTex = texture(texture_specular0, TexCoords);
        specularPower = specularFromTex.r;
        if(any(specular.rgb))
        {
            specular *= specularFromTex;
        }
        else
        {
            specular = specularFromTex;
        }
    }

    vec4 N = vec4(normalize(NormalVS), 0.0);
    if(mat.HasNormalTexture)
    {
        mat3 TBN = mat3(normalize(TangentVS), normalize(BinormalVS), normalize(NormalVS));
        N = calcNormalMapping(TBN, texture_normal0, TexCoords);
    }

    gLightAccumulation = (ambient + emissive);
    gDiffuse = diffuse;
    gNormal = N;
    gSpec = vec4(specular.rgb, log2(specularPower)/10.5);
}