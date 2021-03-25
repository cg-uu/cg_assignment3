// This file provides a starting point for generating shadow maps in part 4 of
// assignment 3. Just copy this code to your model_viewer.cpp, and extend the
// parts that are indicated in the comments.
//

// Struct for representing a shadow casting point light
struct ShadowCastingLight {
    glm::vec3 position;      // Light source position
    glm::mat4 shadowMatrix;  // Camera matrix for shadowmap
    GLuint shadowmap;        // Depth texture
    GLuint shadowFBO;        // Depth framebuffer
    float shadowBias;        // Bias for depth comparison
};

// Update the shadowmap and shadow matrix for a light source
void update_shadowmap(Context &ctx, ShadowCastingLight &light, GLuint shadowFBO)
{
    // Set up rendering to shadowmap framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, shadowFBO);
    if (shadowFBO) glViewport(0, 0, 512, 512);  // TODO Set viewport to shadowmap size
    glClear(GL_DEPTH_BUFFER_BIT);               // Clear depth values to 1.0

    // Set up pipeline
    glUseProgram(ctx.shadowProgram);
    glEnable(GL_DEPTH_TEST);  // Enable Z-buffering

    // TODO Define view and projection matrices for the shadowmap camera. The
    // view matrix should be a lookAt-matrix computed from the light source
    // position, and the projection matrix should be a frustum that covers the
    // parts of the scene that shall recieve shadows.
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 proj = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(ctx.shadowProgram, "u_view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(ctx.shadowProgram, "u_proj"), 1, GL_FALSE, &proj[0][0]);

    // Store updated shadow matrix for use in draw_scene()
    light.shadowMatrix = proj * view;

    // Draw scene
    for (unsigned i = 0; i < ctx.asset.nodes.size(); ++i) {
        const gltf::Node &node = ctx.asset.nodes[i];
        const gltf::Drawable &drawable = ctx.drawables[node.mesh];

        // TODO Define the model matrix for the drawable
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(ctx.shadowProgram, "u_model"), 1, GL_FALSE, &model[0][0]);

        // Draw object
        glBindVertexArray(drawable.vao);
        glDrawElements(GL_TRIANGLES, drawable.indexCount, drawable.indexType,
                       (GLvoid *)(intptr_t)drawable.indexByteOffset);
        glBindVertexArray(0);
    }

    // Clean up
    cg::reset_gl_render_state();
    glUseProgram(0);
    glViewport(0, 0, ctx.width, ctx.height);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
