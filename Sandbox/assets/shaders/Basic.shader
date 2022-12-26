#type vertex
#version 450 core


layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

layout(location = 0) out vec4 v_Color;

layout(set = 0, binding = 0) uniform UBWorld{
	mat4 ViewProjection;
} ubWorld;


void main()
{
	//	gl_Position = ubWorld.ViewProjection * vec4(a_Position, 1.0);
	gl_Position = vec4(a_Position, 1.0);

	v_Color = a_Color;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec4 v_Color;

void main()
{
	o_Color = v_Color;
}
