#version 330 core
in vec2 TexCoords;
in vec4 VertexColor;

out vec4 FragColor;

uniform sampler2D texture0;
// uniform bool useTexture;

void main()
{
    vec4 texColor = texture(texture0, TexCoord);
    FragColor = texColor * VertexColor;
    // vec4 color = VertexColor;

    // if (useTexture)
    // {
    //     vec4 texColor = texture(texture0, TexCoord);
    //     // tint - transparency
    //     color *= texColor;
    // }

    // FragColor = color;
}