#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
	mat4 view;
} ubo;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(location = 0) in vec3 positions;
layout(location = 0) out vec3 fragColor;

vec3 colors[3] = vec3[](
    ubo.view[0].xyz,
    ubo.proj[0].xyz,
    vec3(1.0, 1.0, 1.0)
);

void main() {
    gl_Position = ubo.proj * ubo.view * vec4(positions, 1.0);
    fragColor = colors[gl_VertexIndex];
}