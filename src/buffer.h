#ifndef THE_SRC_BUFFER_H
#define THE_SRC_BUFFER_H

#include <cstddef>
#include <cstdint>

// Buffer is for consuming bytes across an area of memory, tracking the start location
// the current position, and the end.
//
struct Buffer {
	using iterator = const char *;
	using const_iterator = const char *;
	
    const char *m_start{nullptr};
    const char *m_cur{nullptr};
    const char *m_end{nullptr};

    constexpr Buffer() noexcept {}
    constexpr Buffer(iterator start, iterator end) noexcept
        : m_start{start}
        , m_cur{start}
        , m_end{end}
    {
    }
    constexpr Buffer(iterator start, size_t len) noexcept
        : Buffer{start, start + len}
    {
    }
    template <size_t Size>
    constexpr Buffer(const char (&str)[Size]) noexcept
        : Buffer{&str[0], &str[0] + Size}
    {
    }

    void Assign(iterator start, size_t length) noexcept
    {
        m_start = start;
        m_cur = start;
        m_end = m_start + length;
    }

    constexpr iterator begin() const noexcept { return m_start; }
    constexpr iterator end() const noexcept { return m_end; }
    constexpr const_iterator cbegin() const noexcept { return m_start; }
    constexpr const_iterator cend() const noexcept { return m_end; }
    constexpr const_iterator it() const noexcept { return m_cur; }

    constexpr bool   Eof() const noexcept { return it() >= end(); }
    constexpr size_t Size() const noexcept { return end() - begin(); }

    uint8_t Peek() const noexcept { return !Eof() ? *m_cur : 0; }
    uint8_t Read() noexcept { return (!Eof()) ? *(m_cur++) : 0; }
    void    Skip() noexcept { m_cur += !Eof() ? 1 : 0; }

    void Close() noexcept { Assign(nullptr, 0); }
};

#endif  // THE_SRC_BUFFER_H
