#include "precheader.h"

#include "VertexArray.h"

#include <GL/glew.h>

namespace VEngine
{
    VertexArray::VertexArray()
    {
        glGenVertexArrays(1, &m_id);
    }

    void VertexArray::unbind()
    {
        m_is_data_bound = false;
        glBindVertexArray(0);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_id);
    }

    void VertexArray::bind_data()
    {
        m_is_data_bound = true;
        glBindVertexArray(m_id);
    }

    void VertexArray::bind_draw()
    {
        glBindVertexArray(m_id);
    }
}
