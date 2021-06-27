#include "precheader.h"

#include "Buffer.h"

#include <GL/glew.h>

namespace vengine
{
    //////////////////////////////////////////////////
    //VertexBuffer////////////////////////////////////
    //////////////////////////////////////////////////

    VertexBuffer::VertexBuffer(void *data, const unsigned int size) : m_size(size)
    {
        m_data = malloc(size);
        std::memcpy(m_data, data, size);
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
        free(m_data);
    }

    //////////////////////////////////////////////////
    //IndexBuffer/////////////////////////////////////
    //////////////////////////////////////////////////

    IndexBuffer::IndexBuffer(unsigned int* data, unsigned int size, size_t count, size_t offset) :
	m_data(data), m_size(size), m_count(count), m_offset(offset)
    {
        m_data = malloc(size);
        std::memcpy(m_data, data, size);
        glGenBuffers(1, &m_id);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_id);
        free(m_data);
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