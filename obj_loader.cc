/*
 * Copyright 2014 Google Inc. All Rights Reserved.
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
#include <string>
#include <fstream>

#include <android/asset_manager.h>
#include <unistd.h>
#include "tango-gl/obj_loader.h"

namespace tango_gl {
  bool obj_loader::LoadOBJData(AAssetManager* mgr, const char* path, std::vector<GLfloat>& vertices,
                               std::vector<GLfloat>& normals) {

  std::vector<unsigned int> vertexIndices, normalIndices;
  std::vector<GLfloat> temp_vertices, temp_normals;

  AAsset* asset = AAssetManager_open(mgr, path, AASSET_MODE_STREAMING);
  if (asset == NULL) {
    LOGE("Error opening asset %s", path);
    return false;
  }
  off_t length;
  off_t start;
  int fd = AAsset_openFileDescriptor(asset, &start, &length);
  lseek(fd, start, SEEK_CUR);
  FILE* file = fdopen(fd, "r");

  if (file == NULL) {
    LOGE("Failed to open file: %s", path);
    return false;
  }

  char lineHeader[128]; // max size of .obj line
  char* pNext; // used to get next pointer of token
  while(fgets(lineHeader, sizeof lineHeader, file) != NULL) {

    if (strncmp(lineHeader, "v ", 2) == 0) {
      GLfloat vertex[3];

      vertex[0] = (GLfloat) strtof(lineHeader + 2, &pNext); // 2 lines to ignore the v char
      vertex[1] = (GLfloat) strtof(pNext, &pNext);
      vertex[2] = (GLfloat) strtof(pNext, NULL);

      temp_vertices.push_back(vertex[0]);
      temp_vertices.push_back(vertex[1]);
      temp_vertices.push_back(vertex[2]);
    } else if (strncmp(lineHeader, "vt ", 3) == 0) {

    } else if (strncmp(lineHeader, "vn ", 3) == 0) {
      GLfloat normal[3];

      normal[0] = (GLfloat) strtof(lineHeader + 3, &pNext); // 3 lines to ignore the vn char
      normal[1] = (GLfloat) strtof(pNext, &pNext);
      normal[2] = (GLfloat) strtof(pNext, NULL);

      temp_normals.push_back(normal[0]);
      temp_normals.push_back(normal[1]);
      temp_normals.push_back(normal[2]);

    } else if (strncmp(lineHeader, "f ", 2) == 0) {
      LOGE("F : %s", lineHeader);
      unsigned int vertexIndex[4];
      unsigned int textureIndex[4];
      unsigned int normalIndex[4];

      vertexIndex[0] = (unsigned int) strtol(lineHeader + 2, &pNext, 10);
      textureIndex[0] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[0] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      vertexIndex[1] = (unsigned int) strtol(pNext, &pNext, 10);
      textureIndex[1] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[1] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      vertexIndex[2] = (unsigned int) strtol(pNext, &pNext, 10);
      textureIndex[2] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[2] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      vertexIndex[3] = (unsigned int) strtol(pNext, &pNext, 10);
      textureIndex[3] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[3] = (unsigned int) strtol(pNext + 1, NULL, 10);

      LOGE("CHECK: %u %u %u \n", vertexIndex[3], normalIndex[3], textureIndex[3]);

      if (vertexIndex[0] == 0 || normalIndex[0] == 0 || textureIndex[0] == 0) {
        // skip, last line
      } else if (vertexIndex[3] == 0 || normalIndex[3] == 0 || textureIndex[3] == 0) {
        //Only 3
        LOGE("PUSH: %u %u %u \n", vertexIndex[0] - 1, vertexIndex[1] - 1, vertexIndex[2] - 1);
        vertexIndices.push_back(vertexIndex[0] - 1);
        vertexIndices.push_back(vertexIndex[1] - 1);
        vertexIndices.push_back(vertexIndex[2] - 1);
        normalIndices.push_back(normalIndex[0] - 1);
        normalIndices.push_back(normalIndex[1] - 1);
        normalIndices.push_back(normalIndex[2] - 1);
      } else {
        // If quads provided.
        vertexIndices.push_back(vertexIndex[0] - 1);
        vertexIndices.push_back(vertexIndex[1] - 1);
        vertexIndices.push_back(vertexIndex[2] - 1);
        vertexIndices.push_back(vertexIndex[0] - 1);
        vertexIndices.push_back(vertexIndex[2] - 1);
        vertexIndices.push_back(vertexIndex[3] - 1);
        normalIndices.push_back(normalIndex[0] - 1);
        normalIndices.push_back(normalIndex[1] - 1);
        normalIndices.push_back(normalIndex[2] - 1);
        normalIndices.push_back(normalIndex[0] - 1);
        normalIndices.push_back(normalIndex[2] - 1);
        normalIndices.push_back(normalIndex[3] - 1);
      }

    } else {
      char comments_buffer[1000];
      fgets(comments_buffer, 1000, file);
    }

  } //while loop

  for (unsigned int i = 0; i < vertexIndices.size(); i++) {
    unsigned int vertexIndex = vertexIndices[i];
    unsigned int normalIndex = normalIndices[i];

    vertices.push_back(temp_vertices[vertexIndex * 3]);
    vertices.push_back(temp_vertices[vertexIndex * 3 + 1]);
    vertices.push_back(temp_vertices[vertexIndex * 3 + 2]);
    normals.push_back(temp_normals[normalIndex * 3]);
    normals.push_back(temp_normals[normalIndex * 3 + 1]);
    normals.push_back(temp_normals[normalIndex * 3 + 2]);
  }

  fclose(file);
  AAsset_close(asset);
  return true;
}

bool obj_loader::LoadOBJData(AAssetManager* mgr, const char* path, std::vector<GLfloat>& vertices,
                             std::vector<GLushort>& indices) {
//  std::vector<unsigned int> vertexIndices, normalIndices;
//  std::vector<GLfloat> temp_vertices, temp_normals;
//
//  FILE* file = fopen(path, "r");
//  if (file == NULL) {
//    LOGE("Failed to open file: %s", path);
//    return false;
//  }
//  while (1) {
//    char lineHeader[128];
//    int res = fscanf(file, "%s", lineHeader);
//    if (res == EOF) break;
//    if (strcmp(lineHeader, "v") == 0) {
//      GLfloat vertex[3];
//      int matches =
//          fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
//      if (matches != 3) {
//        LOGE("Format of 'v float float float' required for each vertice line");
//        return false;
//      }
//      temp_vertices.push_back(vertex[0]);
//      temp_vertices.push_back(vertex[1]);
//      temp_vertices.push_back(vertex[2]);
//    } else if (strcmp(lineHeader, "vn") == 0) {
//      GLfloat normal[3];
//      int matches =
//          fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
//      if (matches != 3) {
//        LOGE("Format of 'vn float float float' required for each normal line");
//        return false;
//      }
//      temp_normals.push_back(normal[0]);
//      temp_normals.push_back(normal[1]);
//      temp_normals.push_back(normal[2]);
//    } else if (strcmp(lineHeader, "f") == 0) {
//      unsigned int vertexIndex[4];
//      unsigned int normalIndex[4];
//      unsigned int textureIndex[4];
//      int matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n",
//                           &vertexIndex[0], &normalIndex[0], &vertexIndex[1],
//                           &normalIndex[1], &vertexIndex[2], &normalIndex[2],
//                           &vertexIndex[3], &normalIndex[3]);
//      // If the exported obj file includes texture face indices information,
//      // we still support the loading
//      if ((matches != 6) && (matches != 8)) {
//        // to use "%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n" since we already have
//        // one match in the previous scan.
//        matches = fscanf(file, "%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
//                         &textureIndex[0], &normalIndex[0], &vertexIndex[1],
//                         &textureIndex[1], &normalIndex[1], &vertexIndex[2],
//                         &textureIndex[2], &normalIndex[2], &vertexIndex[3],
//                         &textureIndex[3], &normalIndex[3]);
//        // Adding one count back to matches to count for the one we already
//        // matched.
//        matches += 1;
//      }
//      // .obj file is 1-indexed, so subtract 1 from all indices.
//      if (matches == 6 || matches == 9) {
//        // If triangles provided.
//        vertexIndices.push_back(vertexIndex[0] - 1);
//        vertexIndices.push_back(vertexIndex[1] - 1);
//        vertexIndices.push_back(vertexIndex[2] - 1);
//        normalIndices.push_back(normalIndex[0] - 1);
//        normalIndices.push_back(normalIndex[1] - 1);
//        normalIndices.push_back(normalIndex[2] - 1);
//      } else if (matches == 8 || matches == 12) {
//        // If quads provided.
//        vertexIndices.push_back(vertexIndex[0] - 1);
//        vertexIndices.push_back(vertexIndex[1] - 1);
//        vertexIndices.push_back(vertexIndex[2] - 1);
//        vertexIndices.push_back(vertexIndex[0] - 1);
//        vertexIndices.push_back(vertexIndex[2] - 1);
//        vertexIndices.push_back(vertexIndex[3] - 1);
//        normalIndices.push_back(normalIndex[0] - 1);
//        normalIndices.push_back(normalIndex[1] - 1);
//        normalIndices.push_back(normalIndex[2] - 1);
//        normalIndices.push_back(normalIndex[0] - 1);
//        normalIndices.push_back(normalIndex[2] - 1);
//        normalIndices.push_back(normalIndex[3] - 1);
//      } else {
//        LOGE(
//            "Format of 'f int/int/int int/int/int int/int/int (int/int/int)' "
//            "or 'f int//int int//int int//int (int//int)' required for each "
//            "face");
//        return false;
//      }
//    } else {
//      char comments_buffer[1000];
//      fgets(comments_buffer, 1000, file);
//    }
//  }
//  for (unsigned int i = 0; i < vertexIndices.size(); i++) {
//    unsigned int vertexIndex = vertexIndices[i];
//    unsigned int normalIndex = normalIndices[i];
//
//    vertices.push_back(temp_vertices[vertexIndex * 3]);
//    vertices.push_back(temp_vertices[vertexIndex * 3 + 1]);
//    vertices.push_back(temp_vertices[vertexIndex * 3 + 2]);
//    normals.push_back(temp_normals[normalIndex * 3]);
//    normals.push_back(temp_normals[normalIndex * 3 + 1]);
//    normals.push_back(temp_normals[normalIndex * 3 + 2]);
//  }
//  fclose(file);
  return true;
}

bool obj_loader::LoadOBJData(AAssetManager* mgr, const char* path, std::vector<GLfloat>& vertices,
                             std::vector<GLfloat>& normals, std::vector<GLfloat>& textures) {

  std::vector<unsigned int> vertexIndices, normalIndices, textureIndices;
  std::vector<GLfloat> temp_vertices, temp_normals, temp_textures;

  AAsset* asset = AAssetManager_open(mgr, path, AASSET_MODE_STREAMING);
  if (asset == NULL) {
    LOGE("Error opening asset %s", path);
    return false;
  }
  off_t length;
  off_t start;
  int fd = AAsset_openFileDescriptor(asset, &start, &length);
  lseek(fd, start, SEEK_CUR);
  FILE* file = fdopen(fd, "r");

  if (file == NULL) {
    LOGE("Failed to open file: %s", path);
    return false;
  }

  char lineHeader[128]; // max size of .obj line
  char* pNext; // used to get next pointer of token
  while(fgets(lineHeader, sizeof lineHeader, file) != NULL) {

    if (strncmp(lineHeader, "v ", 2) == 0) {
      GLfloat vertex[3];

      vertex[0] = (GLfloat) strtof(lineHeader + 2, &pNext); // 2 lines to ignore the v char
      vertex[1] = (GLfloat) strtof(pNext, &pNext);
      vertex[2] = (GLfloat) strtof(pNext, NULL);

      temp_vertices.push_back(vertex[0]);
      temp_vertices.push_back(vertex[1]);
      temp_vertices.push_back(vertex[2]);
    } else if (strncmp(lineHeader, "vt ", 3) == 0) {
      GLfloat texture[2];

      texture[0] = (GLfloat) strtof(lineHeader + 3, &pNext); // 3 lines to ignore the vt char
      texture[1] = (GLfloat) strtof(pNext, NULL);

      temp_textures.push_back(texture[0]);
      temp_textures.push_back(texture[1]);

    } else if (strncmp(lineHeader, "vn ", 3) == 0) {
      GLfloat normal[3];

      normal[0] = (GLfloat) strtof(lineHeader + 3, &pNext); // 3 lines to ignore the vn char
      normal[1] = (GLfloat) strtof(pNext, &pNext);
      normal[2] = (GLfloat) strtof(pNext, NULL);

      temp_normals.push_back(normal[0]);
      temp_normals.push_back(normal[1]);
      temp_normals.push_back(normal[2]);

    } else if (strncmp(lineHeader, "f ", 2) == 0) {
//      LOGE("F : %s", lineHeader);
      unsigned int vertexIndex[4];
      unsigned int textureIndex[4];
      unsigned int normalIndex[4];

      vertexIndex[0] = (unsigned int) strtol(lineHeader + 2, &pNext, 10);
      textureIndex[0] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[0] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      vertexIndex[1] = (unsigned int) strtol(pNext, &pNext, 10);
      textureIndex[1] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[1] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      vertexIndex[2] = (unsigned int) strtol(pNext, &pNext, 10);
      textureIndex[2] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[2] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      vertexIndex[3] = (unsigned int) strtol(pNext, &pNext, 10);
      textureIndex[3] = (unsigned int) strtol(pNext + 1, &pNext, 10);
      normalIndex[3] = (unsigned int) strtol(pNext + 1, NULL, 10);

//      LOGE("CHECK: %u %u %u \n", vertexIndex[3], normalIndex[3], textureIndex[3]);

      if (vertexIndex[0] == 0 || normalIndex[0] == 0 || textureIndex[0] == 0) {
        // skip, last line
      } else if (vertexIndex[3] == 0 || normalIndex[3] == 0 || textureIndex[3] == 0) {
        //Only 3
//        LOGE("PUSH: %u %u %u \n", vertexIndex[0] - 1, vertexIndex[1] - 1, vertexIndex[2] - 1);
        vertexIndices.push_back(vertexIndex[0] - 1);
        vertexIndices.push_back(vertexIndex[1] - 1);
        vertexIndices.push_back(vertexIndex[2] - 1);
        normalIndices.push_back(normalIndex[0] - 1);
        normalIndices.push_back(normalIndex[1] - 1);
        normalIndices.push_back(normalIndex[2] - 1);
        textureIndices.push_back(textureIndex[0] - 1);
        textureIndices.push_back(textureIndex[1] - 1);
        textureIndices.push_back(textureIndex[2] - 1);
      } else {
        // If quads provided.
        vertexIndices.push_back(vertexIndex[0] - 1);
        vertexIndices.push_back(vertexIndex[1] - 1);
        vertexIndices.push_back(vertexIndex[2] - 1);
        vertexIndices.push_back(vertexIndex[0] - 1);
        vertexIndices.push_back(vertexIndex[2] - 1);
        vertexIndices.push_back(vertexIndex[3] - 1);
        normalIndices.push_back(normalIndex[0] - 1);
        normalIndices.push_back(normalIndex[1] - 1);
        normalIndices.push_back(normalIndex[2] - 1);
        normalIndices.push_back(normalIndex[0] - 1);
        normalIndices.push_back(normalIndex[2] - 1);
        normalIndices.push_back(normalIndex[3] - 1);
        textureIndices.push_back(textureIndex[0] - 1);
        textureIndices.push_back(textureIndex[1] - 1);
        textureIndices.push_back(textureIndex[2] - 1);
        textureIndices.push_back(textureIndex[0] - 1);
        textureIndices.push_back(textureIndex[2] - 1);
        textureIndices.push_back(textureIndex[3] - 1);
      }

    } else {
      char comments_buffer[1000];
      fgets(comments_buffer, 1000, file);
    }

  } //while loop

  for (unsigned int i = 0; i < vertexIndices.size(); i++) {
    unsigned int vertexIndex = vertexIndices[i];
    unsigned int normalIndex = normalIndices[i];
    unsigned int textureIndex = textureIndices[i];

    vertices.push_back(temp_vertices[vertexIndex * 3]);
    vertices.push_back(temp_vertices[vertexIndex * 3 + 1]);
    vertices.push_back(temp_vertices[vertexIndex * 3 + 2]);
    normals.push_back(temp_normals[normalIndex * 3]);
    normals.push_back(temp_normals[normalIndex * 3 + 1]);
    normals.push_back(temp_normals[normalIndex * 3 + 2]);
    textures.push_back(temp_textures[textureIndex * 3]);
    textures.push_back(temp_textures[textureIndex * 3 + 1]);
  }

  fclose(file);
  AAsset_close(asset);
  return true;
}

}  // namespace tango_gl

/*GLushort vertexIndex[3];
      int matches = fscanf(file, "%hu %hu %hu\n", &vertexIndex[0],
                           &vertexIndex[1], &vertexIndex[2]);
      if (matches != 3) {
        LOGE("Format of 'f int int int' required for each face line");
        return false;
      }
      indices.push_back(vertexIndex[0] - 1);
      indices.push_back(vertexIndex[1] - 1);
      indices.push_back(vertexIndex[2] - 1);*/