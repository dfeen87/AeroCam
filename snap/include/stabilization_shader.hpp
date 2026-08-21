#pragma once

namespace snap {

constexpr const char* kStabilizationShaderGLSL = R"(#version 300 es
precision highp float;

// 3x3 Projective Homography Warp Matrix from AeroCam Motion Stabilizer
uniform mat3 u_homographyMatrix;
uniform sampler2D u_inputTexture;

in vec2 v_texCoord;
out vec4 fragColor;

void main() {
    // Convert normalized UV coordinates [0, 1] to homogeneous projective space [-1, 1]
    vec3 homogeneousCoord = vec3(v_texCoord * 2.0 - 1.0, 1.0);

    // Apply inverse homography transformation for GPU pre-capture warp correction
    vec3 warpedCoord = u_homographyMatrix * homogeneousCoord;

    // Convert back from projective coordinates
    vec2 sampleUV = (warpedCoord.xy / warpedCoord.z) * 0.5 + 0.5;

    // Edge clamping check to prevent texture sampling artifacts
    if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0) {
        fragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black border on frame crop boundary
    } else {
        fragColor = texture(u_inputTexture, sampleUV);
    }
}
)";

} // namespace snap
