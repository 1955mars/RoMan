#type vertex
#version 430

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
//layout(location = 2) in vec3 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;
uniform vec3 u_LightPos;

out vec3 v_Color;

void main()
{
	vec3 lightDir = normalize(u_LightPos - a_Position);
	float intensity = max(dot(a_Normal, lightDir), 0.0);
	v_Color = intensity * vec3(1.0, 1.0, 0.0);

	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430

layout(location = 0) out vec4 finalColor;

in vec3 v_Color;

void main()
{
	finalColor = vec4(v_Color, 1.0f);
	//finalColor = vec4((v_Normal * 0.5 + 0.5), 1.0);
}