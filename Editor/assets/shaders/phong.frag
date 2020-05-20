#version 420

#include shaders/vertex_texture_blend.shader
#include shaders/gamma_correction.shader
#include shaders/lights.shader

in vec3 WorldPos;
in vec3 CameraPos;
in vec2 TexCoords;
in vec3 Normal;
in vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

out vec4 FragColor;

// global props

uniform float ambiance;
uniform float blinn;

// material parameters

uniform vec4 mat_baseColor;
uniform float mat_reflectance;
uniform float mat_ao;

uniform float mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float mat_hasNormalMap;
uniform sampler2D mat_normalMap;

uniform float mat_hasReflectanceMap;
uniform sampler2D mat_reflectanceMap;

uniform float mat_hasAoMap;
uniform sampler2D mat_aoMap;

uniform float mat_hasAlphaMaskMap;
uniform sampler2D mat_alphaMaskMap;

uniform float     mat_hasShadowMap;       // take out of material at some point
uniform sampler2D mat_shadowMap;          // take out of material at some point

uniform float       mat_hasShadowMap2;    // take out of material at some point
uniform samplerCube mat_shadowMap2;       // take out of material at some point

const float PI = 3.14159265359f;

// Math functions

float lerp(
	float a, 
	float b, 
	float w)
{
	return a + w * (b - a);
}

float linstep(
	float l, 
	float h, 
	float v)
{
	return clamp((v - l) / (h - l), 0.0, 1.0);
}

// Shadows

float DirectionalLightShadow(
	vec4 coords4)
{
	if (mat_hasShadowMap == 0) {
		return 1.0f;
	}

	vec3 coords = (coords4.xyz / coords4.w) * 0.5 + 0.5;
	vec2 moments = texture(mat_shadowMap, coords.xy).rg;
	float compare = coords.z;

	if (compare > 1.0) { // test if like not having this brantch is faster?
		return 1.0;
	}

	float p = step(compare, moments.x);
	float v = max(moments.y - moments.x * moments.x, 0.00002);

	float d = compare - moments.x;
	float pMax = linstep(0.2, 1.0, v / (v + d * d));

	return min(max(p, pMax), 1.0);
}

float PointLightShadow(
	vec3 NegL,
	float R)
{
	if (mat_hasShadowMap2 == 0) {
		return 1.0f;
	}

	float closestDepth = texture(mat_shadowMap2, NegL).r;
	float currentDepth = length(NegL);

	float bias = 0.05;
	float shadow = R * closestDepth > currentDepth - bias ? 1.0 : 0.0;

	return shadow;
}

// attenuation

float getDistanceAtt(
	vec3 unormalizedLightVector,
	float invSqrRadius)
{
	float dist2 = dot(unormalizedLightVector, unormalizedLightVector);
	return clamp(1.0 - dist2 * invSqrRadius, 0.0, 1.0) * 0.1f;
}

// BRDF

vec3 BRDF(
	vec3 N, 
	vec3 V, 
	vec3 L, 
	vec3 baseColor,
	float reflectance)
{
	vec3 nL = normalize(L);

	float NdotL = clamp(dot(N,  nL), 0.0f, 1.0f);

	// diffuse
    float diffuse = max(NdotL, 0.0);
    
	// specular
    float spec = 0.0;
    
	//if(blinn == 1.0) {
		//vec3 halfwayDir = normalize(L + V);  
		//spec = pow(max(dot(N, halfwayDir), 0.0), reflectance * 256);
 //   }

    //else {
        vec3 reflectDir = reflect(-L, N);
        spec = pow(max(dot(V, reflectDir), 0.0), reflectance * 8.0);
    //}

    vec3 specular = vec3(1.0) * spec; // assuming bright white light color
    return baseColor * diffuse;
}

void main() {
	// Discard is in alpha mask

	if (mat_hasAlphaMaskMap == 1) {
		if (0.5 > texture(mat_alphaMaskMap, TexCoords).r) {
			discard;
		}
	}

	// Color

	vec4 baseColor = mat_baseColor;
	if (mat_hasDiffuseMap == 1) {
		baseColor = texture(mat_diffuseMap, TexCoords);
	}

	baseColor.rgb = sRGBToLinear(baseColor.rgb);

	// Normal

	vec3 normal = Normal;
	if (mat_hasNormalMap == 1) {
		normal = texture(mat_normalMap, TexCoords).xyz * vec3(2) - vec3(1);
	}

	// Reflectance

	float reflectance = mat_reflectance;
	if (mat_hasReflectanceMap == 1) {
		reflectance = texture(mat_reflectanceMap, TexCoords).r;
	}

	// Ambient occlusion

	float ao = mat_ao;
	if (mat_hasAoMap == 1) {
		ao = texture(mat_aoMap, TexCoords).r;
	}

	vec3 N = normalize(normal);
	vec3 V = normalize(CameraPos - WorldPos);

	vec3 color = vec3(0);

	for (int i = 0; i < pointLightCount; i++) {
		vec3 P = pointLights[i].Position;
		vec3 L = P - WorldPos;

		float R = pointLights[i].Radius;

		color += BRDF(N, V, L, baseColor.xyz, reflectance)
		       * getDistanceAtt(L, 1 / pow(R, 2))
			   * PointLightShadow(-L, R);
	}
	
	for (int i = 0; i < directionalLightCount; i++) {
		vec3 L = directionalLights[i].InvDirection;

		color += BRDF(N, V, L, baseColor.xyz, reflectance)
		       * DirectionalLightShadow(DirectionalLightPos[i]);
	}

	vec3 ambient = max(ambiance - ao, 0) * baseColor.rgb;
	color += ambient;

	FragColor = vec4(linearToSRGB(color), baseColor.a);
}
