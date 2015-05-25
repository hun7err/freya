#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normalVector_modelspace;
layout(location = 2) in vec2 texCoords_modelspace;
uniform mat4 MVP;
out vec2 texCoords_interpolated;
 
void main(){
    vec4 vertexPosition = vec4(vertexPosition_modelspace,1);
    gl_Position = MVP * vertexPosition;
    texCoords_interpolated = texCoords_modelspace;
}