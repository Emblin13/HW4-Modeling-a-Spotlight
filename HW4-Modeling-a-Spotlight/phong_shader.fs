#version 430

in vec3 fColor;

out vec4 color;



void main(void)
{

color = min(vec4(fColor, 1.0), vec4(1.0));


}


