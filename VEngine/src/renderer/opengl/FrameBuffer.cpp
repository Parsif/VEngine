#include "precheader.h"

#include "FrameBuffer.h"

#include <GL/glew.h>

namespace VEngine
{
    FrameBuffer::FrameBuffer(const FrameBufferSpecifications& spec) : m_specifications(spec)
    {
        glGenFramebuffers(1, &m_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);

        glGenTextures(1, &m_color_attachment);
        glBindTexture(GL_TEXTURE_2D, m_color_attachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_specifications.width, m_specifications.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_attachment, 0);

        glGenTextures(1, &m_depth_stencil_attachment);
        glBindTexture(GL_TEXTURE_2D, m_depth_stencil_attachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_specifications.width, m_specifications.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depth_stencil_attachment, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Framebuffer is not complete'\n";
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    FrameBuffer::~FrameBuffer()
    {
        glDeleteFramebuffers(1, &m_id);
        glDeleteTextures(1, &m_color_attachment);
        glDeleteTextures(1, &m_depth_stencil_attachment);
    }

    void FrameBuffer::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);
        glViewport(0, 0, m_specifications.width, m_specifications.height);
    }

    void FrameBuffer::unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
