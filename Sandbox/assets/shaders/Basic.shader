#type vertex
#version 450 core


layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;
layout(location = 5) in vec4 a_Color;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec4 v_Color;
layout(location = 3) out vec3 v_WorldPosition;
layout(location = 4) out mat3 v_TBNMatrix;

layout(set = 0, binding = 0) uniform UBWorld {
	mat4 ViewProjection;
} ubWorld;

layout(set = 1, binding = 0) uniform UBTransform{
	mat4 ModelMatrix;
} ubTransform;

void main()
{
	gl_Position = ubWorld.ViewProjection * ubTransform.ModelMatrix * vec4(a_Position, 1.0);
//	gl_Position = vec4(a_Position, 1.0);

	v_Color = a_Color;
	v_TexCoord = a_TexCoord;

	v_WorldPosition = normalize(vec3(ubTransform.ModelMatrix * vec4(a_Position, 1.0)));
	v_Normal = vec3(ubTransform.ModelMatrix * vec4(a_Normal, 0.0));

	vec3 tangent = vec3(ubTransform.ModelMatrix * vec4(a_Tangent, 0.0));
	vec3 bitangent = vec3(ubTransform.ModelMatrix * vec4(a_Bitangent, 0.0));

	v_TBNMatrix = mat3(normalize(tangent), normalize(bitangent), normalize(v_Normal));
}

///////////////////////////////////////

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec4 v_Color;
layout(location = 3) in vec3 v_WorldPosition;
layout(location = 4) in mat3 v_TBNMatrix;

layout(set = 2, binding = 0) uniform UBMaterial{
	vec4 AlbedoColor;
	
	float Metallic;
	float Roughness;
	
	uint UseAlbedoTexture;
	uint UseNormalMap;
	uint UseMetalnessMap;
	uint UseRoughnessMap;
} ubMaterial;

layout(set = 2, binding = 1) uniform sampler2D u_AlbedoTexture;
layout(set = 2, binding = 2) uniform sampler2D u_NormalMap;
layout(set = 2, binding = 3) uniform sampler2D u_MetalnessMap;
layout(set = 2, binding = 4) uniform sampler2D u_RoughnessMap;

layout(set = 3, binding = 0) uniform Light{
	vec3 LightPosition;
	float padding;
	vec3 LightColor;
	float padding2;
	vec3 CameraPosition;
} ubLight;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float numerator = a2;
	float denominator = (NdotH2 * (a2 - 1.0) + 1.0);

	denominator = 3.1415926 * denominator * denominator;

	return numerator / denominator;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float numerator = NdotV;
	float denominator = NdotV * (1.0 - k) + k;

	return numerator / denominator;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}


void main()
{
//	o_Color = vec4(0.5); // v_Color;

//	vec3 lightDirection = ubLight.LightDirection;

	vec3 normal = v_Normal;

	if (ubMaterial.UseNormalMap == 1)
	{
		normal = texture(u_NormalMap, v_TexCoord).rgb;
		normal = normalize(v_TBNMatrix * (normal * 2.0 - 1.0));
	}

//	float diffuse = max(dot(normal, lightDirection), 0.0);
//	vec3 diffuseLight = diffuse * vec3(1.0, 1.0, 1.0);

	vec3 albedoColor = ubMaterial.AlbedoColor.rgb;

	if (ubMaterial.UseAlbedoTexture == 1)
	{
//		albedoColor = texture(u_AlbedoTexture, v_TexCoord) * v_Color;
		albedoColor = texture(u_AlbedoTexture, v_TexCoord).rgb;
//		albedoColor = pow(texture(u_AlbedoTexture, v_TexCoord).rgb, 2.2);
	}

	float metalness = ubMaterial.Metallic;

	if (ubMaterial.UseMetalnessMap == 1)
	{
		metalness = texture(u_MetalnessMap, v_TexCoord).r;
	}

	float roughness = ubMaterial.Roughness;

	if (ubMaterial.UseRoughnessMap == 1)
	{
		roughness = texture(u_RoughnessMap, v_TexCoord).r;
	}

	vec3 N = normalize(normal);
	vec3 V = normalize(ubLight.CameraPosition - v_WorldPosition);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedoColor, metalness);

	vec3 Lo = vec3(0.0);

	{
		vec3 L = normalize(ubLight.LightPosition - v_WorldPosition);
		vec3 H = normalize(V + L);

		float distance = length(ubLight.LightPosition - v_WorldPosition);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = ubLight.LightColor * attenuation; //lightColors[i] * attenuation;

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metalness;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.00001;
		vec3 specular = numerator / denominator;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedoColor / 3.1415926 + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03) * albedoColor; // *ao;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / 2.2));

	o_Color = vec4(color, 1.0);

//	o_Color = vec4(albedoColor, 1.0) * vec4(diffuseLight, 1.0) * roughness * metalness; // *v_Color;
//	o_Color = vec4(albedoColor, 1.0) * vec4(diffuseLight, 1.0) * roughness * metalness; // *v_Color;

//	o_Color = texture(u_AlbedoTexture, v_TexCoord) * v_Color;
}
