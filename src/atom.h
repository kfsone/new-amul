#ifndef THE_SRC_ATOM_H
#define THE_SRC_ATOM_H

// "atom"s are the preliminary unit of classification used by the parser to identify
// contiguous sequences of characters (whitespace, digits, letters), and symbols.

#include <cstddef>
#include <cstdint>

enum AtomType {
    A_INVALID,
    A_END,     // \r or \n or \n\r or \r\n
    A_SPACE,   // ' '
    A_LETTER,  // a-z A-Z
    A_DIGIT,   // 0-9
    A_PUNCT,   // punctuation
};


///TODO: Introduce
struct SourceLocation final {
	const char *m_sourceFilename;
	uint32_t	m_lineNo;
	uint32_t	m_lineOffset;		// 1-based position on line for non-end atoms
									// note: A_END at position 0 => empty line
};


// Atom is very simply a view on a text buffer that identifies one or more characters
// of being a given type. The parser can then use sequences of atoms to match lexemes.
//
// TODO: Atom should track file, line and offset.
//
struct Atom final {
    const char *m_start;
    AtomType    m_type;
    const char *m_end;

  public:
    explicit constexpr Atom(AtomType at, const char *start, size_t len) noexcept
        : m_start{start}
        , m_type{at}
        , m_end{start + len}
    {
    }

    explicit constexpr Atom(AtomType at) noexcept
        : Atom{at, "", 0}
    {
    }
    constexpr Atom() noexcept
        : Atom{A_INVALID}
    {
    }
    explicit constexpr Atom(const char *text) noexcept
        : Atom{A_PUNCT, text, 1}
    {
    }

    explicit Atom(struct Buffer &buffer) noexcept;

    constexpr const AtomType &Type() const noexcept { return m_type; }
    constexpr char            First() const noexcept { return m_end > m_start ? *m_start : 0; }
    constexpr bool            operator==(AtomType at) const noexcept { return m_type == at; }
    constexpr bool            operator==(char punct) const noexcept
    {
        return m_type == A_PUNCT && *m_start == punct;
    }
    constexpr bool operator!=(AtomType at) const noexcept { return !(operator==(at)); }
    constexpr bool operator!=(char punct) const noexcept { return !(operator==(punct)); }
};

AtomType NextAtomType(struct Buffer &);

#endif  // THE_SRC_ATOM_H
