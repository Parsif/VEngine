#include "precheader.h"

#include "Buffer.h"

#include <GL/glew.h>

namespace VEngine
{
    //////////////////////////////////////////////////
    //VertexBuffer////////////////////////////////////
    //////////////////////////////////////////////////

    VertexBuffer::VertexBuffer(void *data, const unsigned int size) : m_data(data), m_size(size)
    {
        glGenBuffers(1, &m_id);
    }

    void VertexBuffer::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        glBufferData(GL_ARRAY_BUFFER, m_size, m_data, GL_STATIC_DRAW);
    }

    void VertexBuffer::unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_id);
    }

    //////////////////////////////////////////////////
    //IndexBuffer/////////////////////////////////////
    //////////////////////////////////////////////////

    IndexBuffer::IndexBuffer(unsigned int* data, unsigned int size, size_t count, size_t offset) :
	m_data(data), m_size(size), m_count(count), m_offset(offset)
    {
        glGenBuffers(1, &m_id);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_id);
    }

    void IndexBuffer::bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size, m_data, GL_STATIC_DRAW);
    }

    void IndexBuffer::unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}