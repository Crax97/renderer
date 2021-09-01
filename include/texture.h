#pragma once

namespace renderer {
    enum class texture_format {
        rgb,
        rgba,
        red,
        depth_texture,
        unknown
    };
    class texture {
    public:
        virtual ~texture() = default;
        virtual void bind_to_unit(int unit) = 0;
    };
};