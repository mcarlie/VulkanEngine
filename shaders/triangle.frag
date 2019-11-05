#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 2) in vec2 inTexcoords;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
  outColor = texture( texSampler, inTexcoords );
  //outColor = vec4( inTexcoords, vec2( 1.0 ) );
}
