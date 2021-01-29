#pragma once

#include "precheader.h"

namespace vengine
{
    //////////////////////////////////////////////////
    //VertexBuffer////////////////////////////////////
    //////////////////////////////////////////////////
    class VertexBuffer
    {
   
    public:
        explicit VertexBuffer(void* data, const unsigned int size);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;
    private:
        unsigned int m_id;
        void* m_data;
    	unsigned int m_size;
    };

    //////////////////////////////////////////////////
    //IndexBuffer/////////////////////////////////////
    //////////////////////////////////////////////////
    class IndexBuffer
    {

    public:
        explicit IndexBuffer(unsigned int* data, unsigned int size, size_t count, size_t offset = 0);
        ~IndexBuffer();

        void bind() const;
        void unbind() const;

        [[nodiscard]] auto get_count() const { return m_count; }
        [[nodiscard]] auto get_offset() const { return m_offset; }

    private:
        unsigned int m_id;
        void* m_data;
        unsigned int m_size;
        size_t m_count;
        size_t m_offset;
    };


    //////////////////////////////////////////////////
    //BufferLayout/////////////////////////////////////
    //////////////////////////////////////////////////
    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static size_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:    return 4;
            case ShaderDataType::Float2:   return 4 * 2;
            case ShaderDataType::Float3:   return 4 * 3;
            case ShaderDataType::Float4:   return 4 * 4;
            case ShaderDataType::Mat3:     return 4 * 3 * 3;
            case ShaderDataType::Mat4:     return 4 * 4 * 4;
            case ShaderDataType::Int:      return 4;
            case ShaderDataType::Int2:     return 4 * 2;
            case ShaderDataType::Int3:     return 4 * 3;
            case ShaderDataType::Int4:     return 4 * 4;
            case ShaderDataType::Bool:     return 1;
        }

        return 0;
    }

    struct BufferElement
    {
        ShaderDataType shader_type;
        bool normalized;
        size_t size;
        uint32_t offset;

        BufferElement(ShaderDataType shader_type, bool normalized = false) :
                shader_type(shader_type), normalized(normalized), size(ShaderDataTypeSize(shader_type)){}

        [[nodiscard]] uint32_t GetComponentCount() const
        {
            switch (shader_type)
            {
                case ShaderDataType::Float:   return 1;
                case ShaderDataType::Float2:  return 2;
                case ShaderDataType::Float3:  return 3;
                case ShaderDataType::Float4:  return 4;
                case ShaderDataType::Mat3:    return 3; // 3* float3
                case ShaderDataType::Mat4:    return 4; // 4* float4
                case ShaderDataType::Int:     return 1;
                case ShaderDataType::Int2:    return 2;
                case ShaderDataType::Int3:    return 3;
                case ShaderDataType::Int4:    return 4;
                case ShaderDataType::Bool:    return 1;
                case ShaderDataType::None:    break;
            }

            return 0;
        }
    };

    class BufferLayout
    {
    private:
        std::vector<BufferElement> m_elements;
        uint32_t m_stride;

    public:
        BufferLayout(std::initializer_list<BufferElement> buffer_elements) : m_elements(buffer_elements)
        {
            size_t offset = 0;
            m_stride = 0;   
            for (auto& element : m_elements)
            {
                element.offset = offset;
                offset += element.size;
                m_stride += element.size;
            }
        }

        std::vector<BufferElement>::iterator begin() {  return m_elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_elements.end(); }

        [[nodiscard]] inline auto& getElements() const { return m_elements; }
        [[nodiscard]] inline auto getStride() const { return m_stride; }
    };

}

