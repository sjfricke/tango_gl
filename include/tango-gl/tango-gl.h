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

#ifndef TANGO_GL_TANGO_GL_H_
#define TANGO_GL_TANGO_GL_H_

#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>
#include <GLES3/gl3.h>

#include <vector>
#include <unordered_map>
#include <tango-gl/texture.h>
#include <tango-gl/material.h>

#include "glm/glm.hpp"

namespace tango_gl {

class Camera;
class Transform;
class Material;

// A simple mesh that does not animate at runtime.
class StaticMesh {
 public:
  // The rendering style for the mesh, e.g. TRIANGLES, LINES, etc.
  GLenum render_mode;

  // Vertices for the mesh.  Required.
  std::vector<glm::vec3> vertices;

  // Per-vertex normal vectors for the mesh.  If filled in, this must
  // be the same length as vertices.
  std::vector<glm::vec3> normals;

  // Per-vertex color for the mesh, stored as ABGR.  If filled in,
  // this must be the same length as vertices.
  //
  // Red              = 0xFF0000FF
  // Green            = 0xFF00FF00
  // Blue             = 0xFFFF0000
  // White, 50% alpha = 0x80FFFFFF
  std::vector<uint32_t> colors;

  // Indices for the mesh. Required.
  std::vector<uint32_t> indices;

  // UV coords for texture
  std::vector<glm::vec2> uv;
};


// Draw a thing to the screen.
//void Render(const StaticMesh& mesh, const Material& material,
//            const Transform& transform, const Camera& camera);

// Meshes inhierit transforms
//void Render(const Mesh& mesh, const Material& material,
//            const glm::mat4& projection_mat, const glm::mat4& view_mat);

}  // namespace tango_gl
#endif  // TANGO_GL_TANGO_GL_H_
