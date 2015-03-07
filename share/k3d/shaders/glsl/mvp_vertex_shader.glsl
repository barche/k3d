#version 410 core
layout(location = 0) in vec3 vertex_position_modelspace;

out gl_PerVertex
{
  vec4 gl_Position;
};

uniform mat4 MVP;

void main(){
  vec4 v = vec4(vertex_position_modelspace,1);
  gl_Position = MVP * v;
}
