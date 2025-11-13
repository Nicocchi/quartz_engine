#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 aColor;

out vec2 TexCoords;
out vec4 VertexColor;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(pos, 0.0, 1.0);
    TexCoords = uv;
    VertexColor = aColor;
}
