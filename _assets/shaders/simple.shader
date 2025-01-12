#shader Vertex
#version 330

#include shaders/camera.shader

uniform mat4 model;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

uniform vec4 mat_color;
uniform float mat_hasTexture;
uniform sampler2D mat_texture;

in vec2 TexCoords;

out vec4 FragColor;

void main() {
	vec4 color = mat_color;
	if (mat_hasTexture == 1.0) {
		color = texture(mat_texture, TexCoords);
	}

	if (color.a < 0.5) {
		discard;
	}

	FragColor = color;
}