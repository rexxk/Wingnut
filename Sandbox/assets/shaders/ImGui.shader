#type vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
// override uint;
layout(location = 2) in vec4 a_Color;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec4 v_Color;

layout(set = 0, binding = 0) uniform UBWorld {
	mat4 ViewProjection;
} ubWorld;


void main()
{
	gl_Position = ubWorld.ViewProjection * vec4(a_Position.xy, 0.0, 1.0);

	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec4 v_Color;

layout(set = 1, binding = 0) uniform sampler2D u_Texture;


void main()
{
//	o_Color = texture(u_Texture, v_TexCoord) * v_Color;
//	o_Color = texture(u_Texture, v_TexCoord);
	o_Color = v_Color;
}
