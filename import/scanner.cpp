#include "scanner.h"
#include "buffer.h"

// Include the table.
#include "char-to-atom.h"

/*

This code is tasked with taking an incoming stream of characters, matching them
to a description of allowed input sequences and finally enumerating them so that
the domain-specific code can consume them meaningfully.

Rooms are a simple example.

    room=firstroom  light small
    Welcome to the first room.
    This is my description which can run many lines.

Because AMUL is line and paragraph based, we can be opinionated about some parts
of the process. For a start, we can recognize that the grammar will be line based
and we can treat EOL as a natural checkpoint. Secondly, we can recognize that the
consumer (parser) is going to prefer dealing with tokens block-at-a-time; it will
exclusively want one room at a time.

My current thinking is this:

1. atomizing:
   hard coded classification of characters into groupings and strings:
    illegal characters (non-printable),
    terminating (end) characters such as '\0', eof, eol,
    whitespace (space and tab),
    letters (ascii 'a-z' and 'A-Z'),
    digits (0-9),
    symbols (all other printable punctuation)

2. lexxing:
   meta-coded classification of atoms into standard groupings:
   Underscore := symbol('_')
   Hyphen     := symbol('-')
   Dollar     := symbol('$')
   Bang       := symbol('!')
   Dot        := symbol('.')
   Equals     := symbol('=')
   Separator  := (end | whitespace)+

*NOTE: '~thing' denotes "check" - lookahead/skip;

   word       := letters (letters | digits | Hyphen | Underscore)*
   Word       := letters (letters | digits | Hyphen | Underscore)* ~Separator
   Float      := (digits dot digits? | dot digits) ~Separator
   Number     := digits ~Separator

   Identifier := (Dollar | Bang | Dot) Word

   label	  := word Equals
   Label      := ~label Word		// the label won't be present in the resulting token


3. tokenizing:
   identifying specific instances of the lexemes, such as checking for a *specific*
   label or enumerating a word from a list of keywords.

   RoomFlag   := Keyword(room_flags)

4. parsing:
   parsing should actually guide step 3 by specifying the allowed sequences in the
   current state, and specifying which values should thus be captured for the ast
   or equivalent.

 



-----------------------------------
Example:

Consider two lines:

    room=first

    second \t light nolook

Atomized:

    <letters:room><symbol:=><letters:first><end:\n>
    <leters:second><space: \t ><letters:light><space: ><letters:nolook>

There are multiple Lex patterns that can match each line, for the first:

    <letters:room><symbol:=><letters:first><end:\n>		// legitimate lex result
    <label:room=><Word:first><end:\n>
    <Label:first><end:\n>		// note 'room=' got skipped

Instead of guessing, the room-specific parser will need to provide a definition
that says "look for this" so that the lexer is guided.

This is what a room block looks like:

discard:
    TT_COMMENT

state 0:
    TT_LABEL,		["room=",]		state 1
    TT_IDENTIFIER,	NULL,			state 2
    TT_WHITESPACE,	NULL,			state 0
    TT_EOL,			NULL,			state 0

state 1:
    TT_IDENTIFIER,	NULL,			state 2

state 2:
    TT_EOL,			NULL,			state 3
    TT_IDENTIFIER,	ROOMFLAGS,		state 2

state 3:
    LINE_OF_TEXT,	NULL,			state 4

state 4:
    LINE_OF_TEXT,	NULL,			state 4

I'm contemplating using something like overloading operator&:

RoomId      := Optional(Label("room=")) & Capture(Word(), "id") & Optional(WhiteSpace);	// consume
whitespace if present RoomFlags   := OnePlus(Keyword(room_flags), WhiteSpace())			// room flag
words separated by whitespaces RoomIdLine  := RoomId & Optional(RoomFlags) & EOL;

SYSMSG		:= Optional(Label("msgid=")) >> Value(Identifier('$'), "id") >> EOL
               Value(OnePlus(RawLine(), EOL, "text") >>
               EOL;


*/

/*

Token streaming has to be relatively fluent, so it has to be composable, so it probably needs to
involve expression types.

Seperator = OnePlus(whitespace | end);
Word      = word() & Check(Separator);

RoomId = Labeled("room=", Word());

template<typename LhsT, typename RhsT, typename... Args>
struct Expression : public Expression<RhsT, Args...>
{
    


}
*/