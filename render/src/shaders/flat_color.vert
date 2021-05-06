#version 300 es            
uniform mat4 uCamera;
uniform mat4 uModelMatrix;
in vec4 aPosition;

void main() {
  vec4 camera_space = uCamera * uModelMatrix * aPosition;
  gl_Position.x = camera_space.x;
  gl_Position.y = camera_space.y;
  gl_Position.w = camera_space.z + 1.0;
  gl_Position.z = 0.5;
}

