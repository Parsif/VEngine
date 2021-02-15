#include "precheader.h"

#include "FrameBufferGL.h"

#include <GL/glew.h>

namespace vengine
{
    FrameBufferGL::~FrameBufferGL()
    {
        delete_framebuffer();
    }

    void FrameBufferGL::create(const FrameBufferSpecifications& spec)
    {
        m_specs = spec;
    	
    	if(m_id)
    	{
            delete_framebuffer();
    	}
        glGenFramebuffers(1, &m_id);
        bind();
    	if(spec.use_color_attachment)
    	{
            glGenTextures(1, &m_color_attachment);
            glBindTexture(GL_TEXTURE_2D, m_color_attachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_specs.width, m_specs.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    		
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment, 0);
    	}
    	
    	if(spec.use_depth_stencil_attachment)
    	{
            glGenTextures(1, &m_depth_stencil_attachment);
            glBindTexture(GL_TEXTURE_2D, m_depth_stencil_attachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specs.width, m_specs.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_stencil_attachment, 0);
    	}
    	
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Framebuffer error, status code: " << glCheckFramebufferStatus(GL_FRAMEBUFFER) << '\n';
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

    void FrameBufferGL::delete_framebuffer() const
    {
        glDeleteFramebuffers(1, &m_id);
        glDeleteTextures(1, &m_color_attachment);
        glDeleteTextures(1, &m_depth_stencil_attachment);
    }
}
