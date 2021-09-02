
#include "Init.h"
#include "api_factory.h"
#include "texture.h"
#include "window.h"

#include "../commons/application.h"
#include "../commons/utilities.h"
#include <memory>

class cube_app : public example_support_library::application {
private:
  float m_rot_y = 0.0f;
  std::shared_ptr<renderer::mesh> cube_mesh;
  std::shared_ptr<renderer::texture> cube_texture;
  std::shared_ptr<renderer::shader> cube_shader;

  glm::mat4 compute_mvp(float delta_seconds) {
    auto &api = get_graphics_api();

    auto projection = glm::perspective(static_cast<float>(M_PI) * 0.7f,
                                       800.0f / 600.0f, 0.01f, 10000.0f);
    auto view = glm::lookAt(glm::vec3(0.0f, 30.0f, -30.0f), glm::vec3(0.0f),
                            glm::vec3(0, 1, 0));
    auto model =
        glm::scale(glm::vec3(10.0f)) * glm::rotate(m_rot_y, glm::vec3(0, 1, 0));
    auto mv = view * model;
    auto mvp = projection * mv;
    return mvp;
  }

public:
  cube_app()
      : example_support_library::application("Cube application", 800, 600) {}
  void on_app_startup() override {

    auto &api = get_graphics_api();
    cube_mesh = utilities::mesh_load_from_path(api, "assets/cube.obj")[0];
    cube_shader =
        api->compile_shader(utilities::read_file("assets/vertex.vs"),
                            utilities::read_file("assets/fragment.fs"));
    cube_texture = utilities::texture_load_from_path(api, "assets/dice.png");
  }

  void on_app_loop(float delta_seconds) override {
    auto &api = get_graphics_api();
    auto mvp = compute_mvp(delta_seconds);

    // Bind the cube program and configure it
    cube_shader->use();
    cube_shader->set_uniform_matrix4x4("mvp", &mvp[0][0]);
    cube_shader->bind_textures({{"diffuse", cube_texture}});

    // The pre_draw hook should be called before any rendering operation occurs
    api->pre_draw();

    cube_mesh->draw();

    api->post_draw();

    m_rot_y += M_PI * 0.25f * delta_seconds;
  }
};

int main(int argc, char **argv) {
  cube_app app;
  return app.loop();
}