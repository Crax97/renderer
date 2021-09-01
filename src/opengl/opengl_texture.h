#include "texture.h"

#include <glad/glad.h>

namespace renderer {
    class opengl_texture final : public texture {
    private:
        GLuint m_texture;
    public:
        explicit opengl_texture(GLuint tex_id) noexcept;
        void bind_to_unit(int unit) override;
        ~opengl_texture() noexcept override;
    };
}