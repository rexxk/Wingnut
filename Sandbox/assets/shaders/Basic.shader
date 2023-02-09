#type vertex
#version 450 core


layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec4 v_Color;

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
}

///////////////////////////////////////

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec4 v_Color;

layout(set = 2, binding = 0) uniform sampler2D u_Texture;

layout(set = 3, binding = 0) uniform Light{
	mat3 LightDirection;
} light;


void main()
{
//	o_Color = vec4(0.5); // v_Color;
	o_Color = texture(u_Texture, v_TexCoord) * v_Color;
}
