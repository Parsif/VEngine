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
        if(m_id)
        {
            delete_framebuffer();
        }
    	
        m_specs = spec;
        glGenFramebuffers(1, &m_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    	if(spec.frame_buffer_type == FrameBufferType::COLOR_DEPTH_STENCIL)
    	{
    		if(spec.samples > 1)
    		{
                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_color_attachment);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_RGBA, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_color_attachment, 0);

                glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_depth_stencil_attachment);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depth_stencil_attachment);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_specs.samples, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height, GL_TRUE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, m_depth_stencil_attachment, 0);
    		}
            else
            {
                glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment);
                glBindTexture(GL_TEXTURE_2D, m_color_attachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_specs.width, m_specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment, 0);

                glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_stencil_attachment);
                glBindTexture(GL_TEXTURE_2D, m_depth_stencil_attachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_stencil_attachment, 0);
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

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR("Framebuffer error, status code: " + glCheckFramebufferStatus(GL_FRAMEBUFFER));
        }

        unbind();
    }

    void FrameBufferGL::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);
        glViewport(0, 0, m_specs.width, m_specs.height);
    }

    void FrameBufferGL::unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void FrameBufferGL::blit_framebuffer(unsigned int width, unsigned int height, unsigned int input_fbo, unsigned int output_fbo)
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, input_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, output_fbo);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    void FrameBufferGL::delete_framebuffer() const
    {
        glDeleteFramebuffers(1, &m_id);
        glDeleteTextures(1, &m_color_attachment);
        glDeleteTextures(1, &m_depth_stencil_attachment);
        glDeleteTextures(1, &m_depth_attachment);
    }
}
