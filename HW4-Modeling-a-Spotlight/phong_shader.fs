#version 430

in vec3 Color;

out vec4 fColor;



void main(void)
{

fColor = min(vec4(Color, 1.0), vec4(1.0));


}


