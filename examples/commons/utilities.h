#pragma once

#include "mesh.h"
#include "graphics_api.h"
#include "api_factory.h"
#include "shader.h"
#include "window.h"
#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../commons/stb_image.h"

#include "../commons/OBJ_Loader.h"
#include "../commons/glm/glm.hpp"
#include "../commons/glm/gtc/type_ptr.hpp"
#include "../commons/glm/gtx/quaternion.hpp"
#include "../commons/glm/gtx/transform.hpp"

#include <fstream>
#include <iostream>
#include <memory>

namespace utilities {
std::shared_ptr<renderer::mesh>
mesh_load_from_path(const std::unique_ptr<renderer::graphics_api> &api,
                    const std::string &file_path) {
  using namespace renderer;
  objl::Loader loader;
  if (!loader.LoadFile(file_path)) {
    std::cerr << "Could not load obj file " << file_path << "\n";
    std::exit(-1);
  }

  vertices verts;
  indices inds;
  tex_coords coords;
  normals norms;
  auto &first_mesh = loader.LoadedMeshes[0];
  inds = first_mesh.Indices;
  for (const auto &obj_vert : first_mesh.Vertices) {
    verts.push_back(
        {obj_vert.Position.X, obj_vert.Position.Y, obj_vert.Position.Z});
    norms.push_back({obj_vert.Normal.X, obj_vert.Normal.Y, obj_vert.Normal.Z});
    coords.push_back(
        {obj_vert.TextureCoordinate.X, obj_vert.TextureCoordinate.Y});
  }

  return api->create_mesh(verts, inds, norms, coords);
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