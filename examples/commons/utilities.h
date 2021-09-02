#pragma once

#include "mesh.h"
#include "graphics_api.h"
#include "api_factory.h"
#include "shader.h"
#include "window.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../commons/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION 
#include "../commons/tiny_obj_loader.h"


#include "../commons/glm/glm.hpp"
#include "../commons/glm/gtc/type_ptr.hpp"
#include "../commons/glm/gtx/quaternion.hpp"
#include "../commons/glm/gtx/transform.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

namespace utilities {
std::vector<std::shared_ptr<renderer::mesh>>
mesh_load_from_path(const std::unique_ptr<renderer::graphics_api> &api,
                    const std::string &file_path) {
  using namespace renderer;
  tinyobj::ObjReader reader;
  if (!reader.ParseFromFile(file_path)) {
    std::cerr << "Could not load obj file " << file_path << "\n";
    std::exit(-1);
  }

  auto& shapes = reader.GetShapes();
  auto& attrib = reader.GetAttrib();

  std::vector<std::shared_ptr<mesh>> meshes;

  for (size_t s = 0; s < shapes.size(); s++) {
      vertices verts;
      indices inds;
      tex_coords coords;
      normals norms;
      size_t index_offset = 0;

      auto& first_mesh = shapes[s].mesh;
      for (size_t f = 0; f < first_mesh.num_face_vertices.size(); f++) {
          size_t fv = static_cast<size_t>(first_mesh.num_face_vertices[f]);
          for (size_t v = 0; v < fv; v++) {
              // access to vertex
              tinyobj::index_t idx = first_mesh.indices[index_offset + v];
              inds.push_back(idx.vertex_index);

              float vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
              float vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
              float vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
              verts.emplace_back(vx, vy, vz);

              // Check if `normal_index` is zero or positive. negative = no normal data
              if (idx.normal_index >= 0) {
                  float nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                  float ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                  float nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                  norms.emplace_back(nx, ny, nz);
              }

              // Check if `texcoord_index` is zero or positive. negative = no texcoord data
              if (idx.texcoord_index >= 0) {
                  float tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                  float ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                  coords.emplace_back(tx, ty);
              }

              // Optional: vertex colors
              // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
              // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
              // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
          }
          index_offset += fv;
          meshes.emplace_back(api->create_mesh(verts, inds, norms, coords));
      }
  }

   return meshes;
}

std::string read_file(const std::string &file_path) {
  std::ifstream file;
  file.open(file_path);
  if (!file.is_open()) {
    std::cerr << "Could not read " << file_path << "\n";
    std::exit(-1);
  }
  std::string line;
  std::string file_content;
  while (std::getline(file, line)) {
    file_content.append(line + "\n");
  }
  return file_content;
}

std::shared_ptr<renderer::texture>
texture_load_from_path(const std::unique_ptr<renderer::graphics_api> &api,
                       const std::string &file_path) {
  using namespace renderer;
  int w, h, channels;
  stbi_uc *data = stbi_load(file_path.c_str(), &w, &h, &channels, 0);
  if (!data) {
    std::cerr << "Could not load " << file_path << "\n";
    std::exit(-1);
  }

  texture_format fmt = texture_format::unknown;
  switch (channels) {
  case 1:
    fmt = texture_format::red;
    break;
  case 3:
    fmt = texture_format::rgb;
    break;
  case 4:
    fmt = texture_format::rgba;
    break;
  }

  auto texture = api->create_texture(data, w, h, fmt);
  stbi_image_free(data);

  return texture;
}

} // namespace utilities