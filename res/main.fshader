#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D display;

void main()
{
    FragColor = texture(display, TexCoord);
    //FragColor = vec4(1.0, 1.0, 1.0, 0.0);
}