/*

     ** Bu proje kyze. tarafýndan geliþtirilmektedir. **

*/

#pragma once


class nLogger {
public:
    nLogger() { this->clear(); }

    void clear() noexcept
    {
        this->buffer.clear();
        this->lineOffsets.clear();
        this->lineOffsets.push_back(0);
    }

    void addLog(const char* fmt, ...) noexcept
    {
        auto old_size{ this->buffer.size() };
        va_list args;
        va_start(args, fmt);
        buffer.appendfv(fmt, args);
        va_end(args);
        for (const auto new_size{ this->buffer.size() }; old_size < new_size; ++old_size) {
            if (this->buffer[old_size] == '\n')
                this->lineOffsets.push_back(old_size + 1);
        }
    }

    ImGuiTextBuffer buffer;
    ImGuiTextFilter filter;
    ImVector<int> lineOffsets;

};