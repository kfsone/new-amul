#include "atom.h"
#include "buffer.h"
#include "char-to-atom.h"

// NextAtomType returns the next atom for the given buffer, and advances the
// cursor for the extent of that atom; for example, consumes '\r\n' as one
// atom, or consumes entire alpha sequences as 'letter'.
//
AtomType
NextAtomType(Buffer &buffer)
{
	// Consume the current character from the buffer and advance it's cursor
    const auto firstc = buffer.Read();

	// Identify the character and then scan forward for the extent of the sequence.
	// Symbols (all printable non-letter/digit/space characters) are single-character sequences.
    switch (const AtomType at = charToAtom[firstc & 0xff]; at) {

			// Consume unknown and symbol characters one at a time.
			// The lexer is responsible for identifying multi-symbol tokens.
		case A_INVALID:
			/*FALLTHROUGH*/
		case A_PUNCT:
			return at;

			// Account for cross-platform line endings: \r\n, \n\r, \r and \n.
		case A_END: {
			const auto nextc = buffer.Peek();
			if (firstc != 0) {
				// skip \r or \n after it's compliment
				if (nextc && nextc != firstc && charToAtom[nextc & 0xff] == A_END) {
					buffer.Skip();
				}
			}
			return at;
		}

			// Advance cursor along contiguous sequence of same atom type for whitespace,
			// letters and digits.
		case A_SPACE:
		case A_LETTER:
		case A_DIGIT:
			// check the *next* character
			while (charToAtom[buffer.Peek() & 0xff] == at) {
				buffer.Skip();
			}
			return at;

		default:
			///TODO: FATAL
    		/*UNREACHABLE*/
			break;
    }

    /*UNREACHABLE*/
    return A_INVALID;
}


Atom::Atom(Buffer &buffer) noexcept
    : m_start(buffer.it())
    , m_type(NextAtomType(buffer))
    , m_end(buffer.end())
{
}
