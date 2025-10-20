#version 330 core

in vec3 vertCoords;
in vec3 normal;
in vec2 textureCoords;

out vec4 fColor;

uniform sampler2D lavaTexture;

void main() {
  fColor = texture(lavaTexture, textureCoords);
}
