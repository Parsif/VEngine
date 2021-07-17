#include "precheader.h"

#include "FrameBufferGL.h"

#include <GL/glew.h>

#include "core/Logger.h"


namespace vengine
{
    FrameBufferGL::~FrameBufferGL()
    {
        delete_framebuffer();
    }

    void FrameBufferGL::create(const FrameBufferSpecifications& spec)
    {
        if(m_frame_buffer)
        {
            delete_framebuffer();
        }
    	
        m_specs = spec;

        glGenFramebuffers(1, &m_frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
    	if(spec.frame_buffer_type == FrameBufferType::COLOR_DEPTH_STENCIL)
    	{
    		if(spec.samples > 1)
    		{
               
                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_color_attachment0);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment0);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_RGBA, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment0, 0);

                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_depth_stencil_attachment);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depth_stencil_attachment);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_depth_stencil_attachment, 0);
    		}
            else
            {
                
                glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment0);
                glBindTexture(GL_TEXTURE_2D, m_color_attachment0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_specs.width, m_specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment0, 0);

                glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_stencil_attachment);
                glBindTexture(GL_TEXTURE_2D, m_depth_stencil_attachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_stencil_attachment, 0);
            }
        }

        else if (spec.frame_buffer_type == FrameBufferType::COLOR_ONLY)
        {
            if (spec.samples > 1)
            {

                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_color_attachment0);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment0);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_RGBA16F, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment0, 0);
            }
            else
            {
                glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment0);
                glBindTexture(GL_TEXTURE_2D, m_color_attachment0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_specs.width, m_specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment0, 0);
            }
        }
    	
        else if(spec.frame_buffer_type == FrameBufferType::DEPTH_ONLY)  
    	{
            glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_attachment);
            glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                m_specs.width, m_specs.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
          
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_attachment, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
    	}

        else if(spec.frame_buffer_type == FrameBufferType::ENVIRONMENT_MAP)
        {
            glGenRenderbuffers(1, &m_render_buffer);

            glBindRenderbuffer(GL_RENDERBUFFER, m_render_buffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_specs.width, m_specs.height); 
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_render_buffer);

            glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_cubemap_attachment);
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_attachment);
            for (unsigned int i = 0; i < 6; ++i)
            {
                // note that we store each face with 16 bit floating point values
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
                    m_specs.width, m_specs.height, 0, GL_RGB, GL_FLOAT, nullptr);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        else if (spec.frame_buffer_type == FrameBufferType::MULTI_TARGET)
        {
            if (spec.samples > 1)
            {
                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_color_attachment0);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment0);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_RGBA16F, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment0, 0);

                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_color_attachment1);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment1);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_RGBA16F, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment1, 0);

                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_depth_stencil_attachment);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depth_stencil_attachment);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_depth_stencil_attachment, 0);
            }
            else
            {
                glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment0);
                glBindTexture(GL_TEXTURE_2D, m_color_attachment0);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_specs.width, m_specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment0, 0);

                glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment1);
                glBindTexture(GL_TEXTURE_2D, m_color_attachment1);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_specs.width, m_specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_color_attachment1, 0);

                glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_stencil_attachment);
                glBindTexture(GL_TEXTURE_2D, m_depth_stencil_attachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_stencil_attachment, 0);
            }
        }
    	
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("Framebuffer error, status code: " + glCheckFramebufferStatus(GL_FRAMEBUFFER));
        }

        unbind();
    }

    void FrameBufferGL::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);
        glViewport(0, 0, m_specs.width, m_specs.height);
    }

    void FrameBufferGL::unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBufferGL::bind_texture(unsigned int slot, unsigned int attachment_number) const
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        switch (m_specs.frame_buffer_type)
        {
        case FrameBufferType::COLOR_DEPTH_STENCIL:
        case FrameBufferType::COLOR_ONLY:
            glBindTexture(GL_TEXTURE_2D, m_color_attachment0);
            break;
      
        case FrameBufferType::DEPTH_ONLY:
            glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
            break;
        case FrameBufferType::ENVIRONMENT_MAP:
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_attachment);
            break;

        case FrameBufferType::MULTI_TARGET:
            if(attachment_number == 0)
            {
                glBindTexture(GL_TEXTURE_2D, m_color_attachment0);
            }
            else if(attachment_number == 1)
            {
                glBindTexture(GL_TEXTURE_2D, m_color_attachment1);
            }
            break;
        }
    }

    void FrameBufferGL::attach_cubemap_face_as_texture2d(unsigned int face_count) const
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + face_count, m_cubemap_attachment, 0);
    }

    void FrameBufferGL::blit_framebuffer(unsigned int width, unsigned int height, unsigned int input_fbo, unsigned int output_fbo, unsigned int color_attachment_number)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, input_fbo);
        glNamedFramebufferReadBuffer(input_fbo, GL_COLOR_ATTACHMENT0 + color_attachment_number);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output_fbo);
        glNamedFramebufferDrawBuffer(output_fbo, GL_COLOR_ATTACHMENT0 + color_attachment_number);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBufferGL::set_draw_buffers(unsigned fbo, unsigned number_of_attachments)
    {

        if (number_of_attachments == 1)
        {
            unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
            glNamedFramebufferDrawBuffers(fbo, number_of_attachments, attachments);
        }
        else if (number_of_attachments == 2)
        {
            unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glNamedFramebufferDrawBuffers(fbo, number_of_attachments, attachments);
        }
    }

    void FrameBufferGL::delete_framebuffer() const
    {
        glDeleteFramebuffers(1, &m_frame_buffer);
        glDeleteTextures(1, &m_color_attachment0);
        glDeleteTextures(1, &m_color_attachment1);
        glDeleteTextures(1, &m_depth_stencil_attachment);
        glDeleteTextures(1, &m_depth_attachment);
    }
}
