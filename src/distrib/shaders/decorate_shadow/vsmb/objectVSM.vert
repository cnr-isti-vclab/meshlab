uniform mat4 mvpl;
varying vec4 shadowCoord;

void main() { 
  shadowCoord= mvpl * gl_Vertex;
  gl_Position = ftransform(); 
} 