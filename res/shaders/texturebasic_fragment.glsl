#version 330 core
out vec4 color;
in vec2 texCoords_interpolated;
uniform sampler2D diffuseTexture;

void main(){
	color = vec4(1,1,1,1);
    //color = texture(diffuseTexture, texCoords_interpolated);
}