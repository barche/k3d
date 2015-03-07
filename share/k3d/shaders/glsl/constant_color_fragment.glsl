#version 410 core
out vec4 color;

uniform vec3 ConstantColor;

void main(){
		color = vec4(ConstantColor,1);
}
