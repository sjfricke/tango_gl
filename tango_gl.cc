/*
 * Copyright 2016 Google Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <tango-gl/mesh.h>
#include "tango-gl/tango-gl.h"

namespace tango_gl {

GLuint Material::fallback_shader_program_ = 0;

//void Render(const StaticMesh& mesh, const Material& material,
//            const Transform& transform, const Camera& camera) {
//  glm::mat4 model_mat = transform.GetTransformationMatrix();
//  glm::mat4 view_mat = camera.GetViewMatrix();
//  glm::mat4 projection_mat = camera.GetProjectionMatrix();
//
//  glUseProgram(material.GetShaderProgram());
//
//  // Set up shader uniforms.
//  GLint uniform_mvp_mat = material.GetUniformModelViewProjMatrix();
//  if (uniform_mvp_mat != -1) {
//    glm::mat4 mvp_mat = projection_mat * view_mat * model_mat;
//    glUniformMatrix4fv(uniform_mvp_mat, 1, GL_FALSE, glm::value_ptr(mvp_mat));
//  }
//
//  GLint uniform_mv_mat = material.GetUniformModelViewMatrix();
//  if (uniform_mv_mat != -1) {
//    glm::mat4 mv_mat = view_mat * model_mat;
//    glUniformMatrix4fv(uniform_mv_mat, 1, GL_FALSE, glm::value_ptr(mv_mat));
//  }
//
//  GLint uniform_m_mat = material.GetUniformModelMatrix();
//  if (uniform_m_mat != -1) {
//    glm::mat4 m_mat = model_mat;
//    glUniformMatrix4fv(uniform_m_mat, 1, GL_FALSE, glm::value_ptr(m_mat));
//  }
//
//  GLint uniform_normal_mat = material.GetUniformNormalMatrix();
//  if (uniform_normal_mat != -1) {
//    glm::mat4 normal_mat = glm::transpose(glm::inverse(view_mat * model_mat));
//    glUniformMatrix4fv(uniform_normal_mat, 1, GL_FALSE,
//                       glm::value_ptr(normal_mat));
//  }
//
//  material.BindParams();
//
//  // Set up shader attributes.
//  GLint attrib_vertices = material.GetAttribVertices();
//  glEnableVertexAttribArray(attrib_vertices);
//  glVertexAttribPointer(attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0,
//                        mesh.vertices.data());
//
//  GLint attrib_normal = material.GetAttribNormals();
//  if (attrib_normal != -1 && !mesh.normals.empty()) {
//    glEnableVertexAttribArray(attrib_normal);
//    glVertexAttribPointer(attrib_normal, 3, GL_FLOAT, GL_FALSE, 0,
//                          mesh.normals.data());
//  }
//
//  GLint attrib_color = material.GetAttribColors();
//  if (attrib_color != -1 && !mesh.colors.empty()) {
//    glEnableVertexAttribArray(attrib_color);
//    glVertexAttribPointer(attrib_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,
//                          mesh.colors.data());
//  }
//
//  GLint attrib_uv = material.GetAttribUVs();
//  if (attrib_uv != -1 && !mesh.uv.empty()) {
//    glEnableVertexAttribArray(attrib_uv);
//    glVertexAttribPointer(attrib_uv, 2, GL_FLOAT, GL_FALSE, 0, mesh.uv.data());
//  }
//
//  glDrawElements(mesh.render_mode, mesh.indices.size(), GL_UNSIGNED_INT,
//                 mesh.indices.data());
//
//  // Clean up state
//  glDisableVertexAttribArray(attrib_vertices);
//  if (attrib_normal != -1) {
//    glDisableVertexAttribArray(attrib_normal);
//  }
//  if (attrib_color != -1) {
//    glDisableVertexAttribArray(attrib_color);
//  }
//
//  glUseProgram(0);
//
//  util::CheckGlError("Render");
//}
//
//void Render(const Mesh& mesh, const Material& material,
//            const glm::mat4& projection_mat, const glm::mat4& view_mat) {
//
//  glUseProgram(material.GetShaderProgram());
//
//  glm::mat4 model_mat = mesh.GetTransformationMatrix();
//  glm::mat4 mv_mat = view_mat * model_mat;
//  glm::mat4 mvp_mat = projection_mat * mv_mat;
//
//  // Set up shader uniforms.
//  GLint uniform_mvp_mat = material.GetUniformModelViewProjMatrix();
//  if (uniform_mvp_mat != -1) {
//    glUniformMatrix4fv(uniform_mvp_mat, 1, GL_FALSE, glm::value_ptr(mvp_mat));
//  }
//
//  GLint uniform_mv_mat = material.GetUniformModelViewMatrix();
//  if (uniform_mv_mat != -1) {
//    glUniformMatrix4fv(uniform_mv_mat, 1, GL_FALSE, glm::value_ptr(mv_mat));
//  }
//
//  GLint uniform_m_mat = material.GetUniformModelMatrix();
//  if (uniform_m_mat != -1) {
//    glUniformMatrix4fv(uniform_m_mat, 1, GL_FALSE, glm::value_ptr(model_mat));
//  }
//
//  GLint uniform_normal_mat = material.GetUniformNormalMatrix();
//  if (uniform_normal_mat != -1) {
//    glm::mat4 normal_mat = glm::transpose(glm::inverse(view_mat * model_mat));
//    glUniformMatrix4fv(uniform_normal_mat, 1, GL_FALSE,
//                       glm::value_ptr(normal_mat));
//  }
//
//  material.BindParams();
//
//  // Set up shader attributes.
//  GLint attrib_vertices = material.GetAttribVertices();
//  glEnableVertexAttribArray(attrib_vertices);
//  glVertexAttribPointer(attrib_vertices, 3, GL_FLOAT, GL_FALSE,
//                        3 * sizeof(GLfloat), &mesh.normals_[0]);
////  glVertexAttribPointer(attrib_vertices, 3, GL_FLOAT, GL_FALSE, 0, mesh.vertices_.data());
//
//  GLint attrib_normal = material.GetAttribNormals();
//  if (attrib_normal != -1 && !mesh.normals_.empty()) {
//    glEnableVertexAttribArray(attrib_normal);
//    glVertexAttribPointer(attrib_normal, 3, GL_FLOAT, GL_FALSE, 0,
//                          mesh.normals_.data());
//  }
//
//  /*GLint attrib_color = material.GetAttribColors();
//  if (attrib_color != -1 && !mesh.colors.empty()) {
//    glEnableVertexAttribArray(attrib_color);
//    glVertexAttribPointer(attrib_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0,
//                          mesh.colors.data());
//  }*/
//
//  GLint attrib_uv = material.GetAttribUVs();
//  if (attrib_uv != -1 && !mesh.textures_.empty()) {
//    glEnableVertexAttribArray(attrib_uv);
//    glVertexAttribPointer(attrib_uv, 2, GL_FLOAT, GL_FALSE, 0, mesh.textures_.data());
//  }
//
//  glDrawElements(mesh.render_mode, mesh.indices.size(), GL_UNSIGNED_INT,
//                 mesh.indices.data());
//
//  // Clean up state
//  glDisableVertexAttribArray(attrib_vertices);
//  if (attrib_normal != -1) {
//    glDisableVertexAttribArray(attrib_normal);
//  }
//  if (attrib_color != -1) {
//    glDisableVertexAttribArray(attrib_color);
//  }
//
//  glUseProgram(0);
//
//  util::CheckGlError("Render");
//}

}  // namespace tango_gl
