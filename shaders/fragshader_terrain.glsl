#version 330 core

in vec3 vertCoords;
in vec3 normal;
in vec2 textureCoords;

out vec4 fColor;

uniform float yScale;

uniform sampler2D flatTexture;
uniform sampler2D steepTexture;

void main() {

  // Ranges from 0 to 1
  float value = (vertCoords.y + (255.0f * yScale) / 2.0f) / (255.0f * yScale);

  if (vertCoords.y + (yScale * 255.0) / 2 < yScale * 8) {
    discard;
  }

  // if (value < 0.5) {
  // if (value < yScale) {
  //   discard;
  // }

  // if (value < 0.01) {
  //   discard;
  // }

  // vec3 baseColor = vec3(0.95f, 0.37f, 0.04f);
  vec3 baseColor = vec3(0.95, 0.32, 0.0);
  // vec3 baseColor = vec3(0.95, 0.59, 0.0);

  // --- Green with tints ---
  // 55, 214, 41
  // vec3 baseColor = vec3(0.22, 0.84, 0.16);
  value = 1.0 - value;
  value = pow(value, 16.0);

  // Make it less intense
  // value /= 6.0;
  
  // fColor = vec4(baseColor.x - value, baseColor.y - value, baseColor.z - value, 0.0);

  // --- Normal rendering ---
  // fColor = vec4(normal * 0.5 + 0.5, 1.0);

  // --- Blended steep and flat texture ---
  vec3 up = vec3(0.0, 1.0, 0.0);
  float steepness = dot(normalize(normal), up);
  steepness = clamp(steepness, 0.0, 1.0);
  steepness = pow(steepness, 16.0);

  vec4 steepColor = texture(steepTexture, textureCoords);
  vec4 flatColor = texture(flatTexture, textureCoords);

  // --- Diffuse ---
  vec3 lightDirection = normalize(vec3(-0.5, -1.0, -0.5));
  float diffuse = max(dot(normal, -lightDirection), 0.0);
  vec3 diffuseFactor = diffuse * vec3(0.95f, 0.37f, 0.04f);
  diffuseFactor += vec3(0.5, 0.5, 0.5);

  // fColor = mix(steepColor, flatColor, steepness) * vec4(diffuseFactor, 1.0) + value * vec4(baseColor, 1.0);
  // fColor = mix(steepColor, flatColor, steepness) * vec4(diffuseFactor, 1.0) * vec4(baseColor, 1.0) * value;
  fColor = mix(mix(steepColor, flatColor, steepness) * vec4(diffuseFactor, 1.0), vec4(baseColor, 1.0), value);
}
