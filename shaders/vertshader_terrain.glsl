#version 330 core

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;
layout(location = 1) in vec3 normal_in;
layout(location = 2) in vec2 textureCoordinates_in;

// Specify the Uniforms of the vertex shader
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 vertCoords;
out vec3 normal;
out vec2 textureCoords;

void main() {
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertCoordinates_in, 1.0);

  vertCoords = vertCoordinates_in;
  normal = normal_in;
  textureCoords = textureCoordinates_in;
}
