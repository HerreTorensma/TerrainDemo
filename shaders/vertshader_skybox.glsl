#version 330 core

// Specify the input locations of attributes
layout(location = 0) in vec3 vertCoordinates_in;

// Specify the Uniforms of the vertex shader
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 textureDirection;

void main() {
  gl_Position = projectionMatrix * viewMatrix * vec4(vertCoordinates_in, 1.0);
  textureDirection = vertCoordinates_in;
}
