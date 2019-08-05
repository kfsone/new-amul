# New-AMUL Parsing

This document describes the principles behind the parser that New-AMUL uses to
compile game definition code.

Bear in mind that this is _not_ the game-play parser, we are in-fact consuming
a descriptive language rather than simply processing a pre-known symbol table.

# TL;DR

New-AMUL is going to be part lexer and part fsm. Each scoped-parser will
provide a simple fsm description which the parser will then use to resolve
parsing atoms into tokens.

# Origin Story: AMUL

AMUL didn't have a single, central parsing mechanism. It has case-specific
parsers for each type of compilation unit. The few common routines largely
dealt with finding/consuming/manipulating whitespace.

There are different text consumption strategies across the various parsers
and versions of AMUL, with the earliest using raw file reads, to the most
recent finally starting to experiment with memory mapped files.

Most of the AMUL parsers follow a structure roughly this:

```
    while not eof:
        find the next non-whitespace non-comment character
        read to end of line, store in "block"
        replace tabs with spaces
        if block starts with the optional unit-preifx (e.g room=) skip that
        copy remaining leading non-whitespace into "Word"
        test if 'Word' is a valid id

        repeat use of Word to consume any supported flags to end of line

        write this data to disk

        consume the next line ...
```

# The New Hotness

New-AMUL will parse by consuming a character stream and turning it into a
sequence of parsing atoms (whitespace, letter, digit, ...) which can then be
incrementally lexed into a token stream with some.

Each per-unit will then be able to specify a relatively simple state machine
for parsing the given unit, while remaining a high degree of commonality.

# Core concepts

## Line and Block

New-AMUL will aim to continue AMUL's aversion to "special characters", in
favor of using layout (i.e whitespace) significance.

An Original-AMUL object description was like this:

```
    \s* "object=" name ( \s <flag> )* \n
    \s* location+ \n
    \s* weight \s value \s dps \s <durability> \s description \s flags* \n
    \n
```

e.g

```
    object=torch flammable light
        westwing
            10 10 2 5 "There is an unlit torch here."
            10 15 3 4 "There is a lit torch here." lit burning
            2  0  0 1 "A burned out torch has been dropped here."

    # empty line ended the above object
```
    
## Atoms

The lowest-level matching a simple character classification.

```
    eof     :=      \0
    eol     :=      \r\n | \n\r | \n | \r
    space   :=      (' ' | \t)+
    letter  :=      [a-zA-Z]+
    digit   :=      [0-9]+
    symbol  :=      <any other isprint>
```

Note: `space`, `letter` and `digit` consume multiple characters.

Atom classification is context independent.

## Lexemes

Lexemes are the intermediates between atoms and terminals. They are the
units in which terminals are generally described.

_Note: {...} indicates a 'peek ahead'_

```
    # things that cleanly end a token
    separator   :=      eof | eol | space

    string      :=      symbol('"') (.*) symbol('"')    # note: over-simplified
                |       symbol("'") (.*) symbol("'")
    float       :=      digit symbol('.') digit? {separator}
                |       symbol('.') digit {separator}
    neg_float   :=      symbol('-') float
    number      :=      digit {separator}
    neg_number  :=      symbol('-') number
    word        :=      letter (symbol('-', '_') | letter)* {separator}
    special-id  :=      symbol('$', '.', '!') identifier
    label       :=      letter (symbol('-') letter)? symbol('=')
```
_Note: 'special-id's: the system reserves '$id' for system identifiers,
such as system message identifiers and built-in verbs that allow you to
override game behavior; '.' is a "hide from player" prefix for verbs etc,
because it is the statement-terminator as far as the gameplay parser is
concerned; and '!' is used to distinguish background tasks._

Lexeme classification should be context independent.

## Tokens

Tokens are tuples of (classification, enumeration), such as (flag, lit).

Because we are parsing a declarative language, the onus for classifying
some tokens will be on the caller: the parser can't tell you what the ID
of a string literal will be.

# Parse

Each compilation unit will consist of a number of FSM definitions, which
specify allowable sequences of tokens/lexemes that describe allowable
sequences of input in the source stream.

This will be a C/C++ implementation of BNF-like FSM descriptions, but I want
this to be a clean, readable implementation.

BNF for an object:

```
    state0:
        label("obj=", word)     ->  Token(Identifier = word) -> state1
        word(word)              ->  Token(Identifier = word) -> state1
    state1:
        from-list(obj-flags)    ->  Token(ObjFlag = number) -> state1
        eol                     ->  state2
    state2:
        word(word)              ->  Token(Location = word) -> state3
        eol                     ->  exception
    state3:
        word(word)              ->  Token(Location = word) -> state3
        eol                     ->  state4
    state4:
        eol                     ->  exception("No states defined")
        -> state5
    state5:
        number                  ->  Token(Weight = value) -> state6
    state6:
        number                  ->  Token(Value = value) -> state7
    state7:
        string                  ->  Token(String = value) -> state8
    state8:
        from-list(state-flags)  ->  Token(StateFlag = word) -> state8
        eol                     ->  state9
    state9:
        eol                     -> end
        -> state5  # next state
```

