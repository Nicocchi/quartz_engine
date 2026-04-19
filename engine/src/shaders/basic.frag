#version 330 core
in vec2 TexCoords;
in vec4 VertexColor;

out vec4 FragColor;

uniform sampler2D texture0;

void main()
{
    vec4 texColor = texture(texture0, TexCoord);
    FragColor = texColor * VertexColor;
}