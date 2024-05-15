#version 450 core
layout(location = 0) out vec4 fColour;

uniform float transparency;  

void main()
{
    vec4 whiteColor = vec4(1.0, 1.0, 1.0, transparency);  
    fColour = whiteColor;
}
