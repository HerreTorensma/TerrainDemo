#version 330 core

in vec3 textureDirection;

out vec4 fColor;

uniform samplerCube skyboxTexture;

void main() {
  fColor = texture(skyboxTexture, textureDirection);
}
