#pragma once
#include <GL/glew.h>

namespace vengine
{
    enum class FrameBufferType
    {
        COLOR_DEPTH_STENCIL, COLOR_ONLY, DEPTH_ONLY, CUBE_MAP_DEPTH_ONLY, CUBE_MAP, TWO_CHANNELS_ONLY, G_BUFFER
    };
	
    struct FrameBufferSpecifications
    {
        uint32_t width, height;
        FrameBufferType frame_buffer_type;
        uint32_t samples = 1;
        bool generate_mipmap = false;
    };

    class FrameBufferGL
    {
    public:
        FrameBufferGL() = default;
        ~FrameBufferGL();

        void create(const FrameBufferSpecifications& spec);
    	
        void bind() const;
        void unbind() const;
    	
        [[nodiscard]] auto get_id() const { return m_frame_buffer; }
        [[nodiscard]] auto get_color_attachment0() const { return m_color_attachment0; }
        [[nodiscard]] auto get_color_attachment1() const { return m_color_attachment1; }
        [[nodiscard]] auto get_color_attachment2() const { return m_color_attachment2; }
        [[nodiscard]] auto get_color_attachment3() const { return m_color_attachment3; }
        [[nodiscard]] auto get_color_attachment4() const { return m_color_attachment4; }
        [[nodiscard]] auto get_depth_attachment() const { return m_depth_attachment; }
        [[nodiscard]] auto get_depth_stencil_attachment() const { return m_depth_stencil_attachment; }

        [[nodiscard]] auto get_cubemap_attachment() const { return m_cubemap_attachment; }
        [[nodiscard]] auto get_rbo() const { return m_render_buffer; };
    	
        [[nodiscard]] auto get_width() const { return m_specs.width; }
        [[nodiscard]] auto get_height() const { return m_specs.height; }

        void bind_texture(uint32_t slot = 0, uint32_t color_attachment = 0) const;
        void attach_cubemap_face_as_texture2d(uint32_t face_count = 0) const;

    public:
        static void blit_framebuffer(uint32_t width, uint32_t height, uint32_t input_fbo, uint32_t output_fbo);

    private:
        void delete_framebuffer() const;
    
    private:
        uint32_t m_frame_buffer;
        uint32_t m_render_buffer;
        uint32_t m_color_attachment0, m_color_attachment1, m_color_attachment2, m_color_attachment3, m_color_attachment4;
        uint32_t m_depth_stencil_attachment, m_depth_attachment, m_cubemap_attachment;

        FrameBufferSpecifications m_specs;
    };
}




