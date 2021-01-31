#version 300 es            
uniform mat4 uCamera;
uniform mat4 uModelMatrix;
in vec4 aPosition;

void main() {
  gl_Position = uCamera * uModelMatrix * aPosition;
}
