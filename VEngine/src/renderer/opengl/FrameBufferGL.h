#pragma once

#include "core/Vtypes.h"

namespace vengine
{
    struct FrameBufferSpecifications
    {
        uint32_t width, height;
        bool use_color_attachment;
        bool use_depth_stencil_attachment;
        uint32_t samples = 1;
    };

    class FrameBufferGL
    {
    public:
        FrameBufferGL() = default;
        ~FrameBufferGL();

        void create(const FrameBufferSpecifications& spec);
    	
        void bind() const;
        void unbind() const;

        [[nodiscard]] inline auto get_color_attachment_id() const { return m_color_attachment; }

    private:
        void delete_framebuffer() const;
    
    private:
        unsigned int m_id;
        unsigned int m_color_attachment, m_depth_stencil_attachment;
        FrameBufferSpecifications m_specs;
    };
}




