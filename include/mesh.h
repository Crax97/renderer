#pragma once

#include <vector>
#include <array>
#include <optional>

namespace renderer {
    struct vertex {
        float x;
        float y;
        float z;

        vertex(float _x, float _y, float _z) noexcept
            : x(_x), y(_y), z(_z) {}
        vertex() : vertex(0.0f, 0.0f, 0.0f) {};
    };
    using normal = vertex;

    struct tex_coord {
        float u;
        float v;
        tex_coord(float _u, float _v) noexcept
            : u(_u), v(_v) {}
        tex_coord() : tex_coord(0.0f, 0.0f) {}
    };

    using vertices = std::vector<vertex>;
    using normals = std::vector<normal>;
    using tex_coords = std::vector<tex_coord>;
    using indices = std::vector<unsigned int>;

    struct face {
        std::array<vertex, 3> vertices{};
        std::optional<std::array<tex_coord, 3>> tex_coords;
        std::optional<std::array<normal, 3>> normals;
    };

    class mesh {
    public:
        virtual void add_face(const face& face) noexcept = 0;
        virtual void draw() const noexcept = 0;

        virtual ~mesh() noexcept = default;
    };
}; // namespace renderer