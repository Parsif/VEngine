#pragma once

namespace vengine
{
    enum class FrameBufferType
    {
        COLOR_DEPTH_STENCIL, COLOR_ONLY, DEPTH_ONLY, CUBE_MAP_DEPTH_ONLY, ENVIRONMENT_MAP, MULTI_TARGET
    };
	
    struct FrameBufferSpecifications
    {
        uint32_t width, height;
        FrameBufferType frame_buffer_type;
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
    	
        [[nodiscard]] auto get_id() const { return m_frame_buffer; }
        [[nodiscard]] auto get_color_attachment0() const { return m_color_attachment0; }
        [[nodiscard]] auto get_color_attachment1() const { return m_color_attachment1; }
        [[nodiscard]] auto get_depth_attachment() const { return m_depth_attachment; }
        [[nodiscard]] auto get_cubemap_attachment() const { return m_cubemap_attachment; }

        [[nodiscard]] auto get_width() const { return m_specs.width; }
        [[nodiscard]] auto get_height() const { return m_specs.height; }

        void bind_texture(unsigned int slot = 0, unsigned int attachment_number = 0) const;
        void attach_cubemap_face_as_texture2d(unsigned int face_count = 0) const;

    public:
        static void blit_framebuffer(unsigned int width, unsigned int height, unsigned int input_fbo, unsigned int output_fbo, unsigned int color_attachment_number = 0);
        static void set_draw_buffers(unsigned int fbo, unsigned int number_of_attachments);


    private:
        void delete_framebuffer() const;
    
    private:
        unsigned int m_frame_buffer;
        unsigned int m_render_buffer;
        unsigned int m_color_attachment0, m_color_attachment1;
        unsigned int m_depth_stencil_attachment, m_depth_attachment, m_cubemap_attachment;

        FrameBufferSpecifications m_specs;
    };
}




