#include <SDL2/SDL_events.h>

#include "Init.h"
#include "api_factory.h"
#include "window.h"

#include "../commons/utilities.h"

struct uniform_block_data {
  glm::mat4 mvp{};
};

int main(int argc, char **argv) {
  using namespace utilities;

  renderer::Init();

  auto win = renderer::window("Textured cube demo", 800, 600);
  auto api = renderer::api_factory::create_api("opengl", win);

  auto cube_mesh = mesh_load_from_path(api, "assets/cube.obj");
  auto simple_shader = api->compile_shader(read_file("assets/vertex.vs"),
                                           read_file("assets/fragment.fs"));
  auto cube_texture = texture_load_from_path(api, "assets/dice.png");
  bool running = true;
  float rot_y = 0.0f;
  while (running) {
    SDL_Event evt;
    SDL_PollEvent(&evt);
    switch (evt.type) {
    case SDL_QUIT:
      running = false;
      break;
    }

    simple_shader->use();
    // calculate mvp
    auto projection = glm::perspective(static_cast<float>(M_PI) * 0.7f,
                                       800.0f / 600.0f, 0.01f, 10000.0f);
    auto view = glm::lookAt(glm::vec3(0.0f, 30.0f, -30.0f), glm::vec3(0.0f),
                            glm::vec3(0, 1, 0));

    auto model =
        glm::scale(glm::vec3(10.0f)) * glm::rotate(rot_y, glm::vec3(0, 1, 0));
    auto mv = view * model;
    auto mvp = projection * mv;
    simple_shader->set_uniform_matrix4x4("mvp", &mvp[0][0]);

    api->pre_draw();
    simple_shader->bind_textures({{"diffuse", cube_texture}});
    cube_mesh->draw();
    api->post_draw();
    rot_y += 0.001f;
  }

  renderer::Quit();
  return 0;
}