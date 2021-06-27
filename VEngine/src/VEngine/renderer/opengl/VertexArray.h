#pragma once


namespace vengine
{
    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();
        void bind_data();
        void bind_draw();
        void unbind();

        [[nodiscard]] bool is_data_bound() const { return m_is_data_bound; }
    private:
        unsigned int m_id;
        bool m_is_data_bound = false;
    };

}


