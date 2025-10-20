#version 330 core

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 textureCoordinates_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform float time;

out vec3 vertCoords;
out vec3 normal;
out vec2 textureCoords;

void main() {
  // float wave = sin(time);

  vec3 position = vertCoordinates_in;
  // float wave = sin(position.x + time / 100.0) + cos(position.z + time / 100.0);
  // float wave = (sin(time * 0.01) / 0.5 + 0.5) / 10000.0;
  // position.y += wave;
  // position.y = sin(time / 1000);

  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);

  vertCoords = position;
  normal = normal_in;
  textureCoords = textureCoordinates_in * 1000.0;
}
