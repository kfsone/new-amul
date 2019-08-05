```
          ####         ###     ### ##     ## ####
         ##  ##         ###   ###  ##     ##  ##            Amiga
        ##    ##        #########  ##     ##  ##            Multi
        ##    ##        #########  ##     ##  ##            User
        ########  ----  ## ### ##  ##     ##  ##            adventure
        ##    ##        ##     ##  ##     ##  ##            Language
       ####  ####      ####   ####  #######  #########
```

# AMUL - broke but not forgotten

AMUL is a game engine for running multi-user text-based adventure games, or
MUDs. It was originally released in the last 80s. The original was written
in oldest-of-old-school pure ANSI K&R C with the help of an awful lot of
caffeine and sugar.

I've taken various stabs at modernizing it over the years, porting it to
Unix as "SMUGL" in the mid-90s.

Unfortunately, I was really bad at version control, and bit rot has cost
me my snapshots of the code in any kind of working version.

# New Amul

Rather than trying to fix and support the various broken versions of AMUL I
can still find, I'm instead going to start with a completely modern approach
to try and quickly build up functionality to get the game data loaded and
running and then retcon parts of SMUGL/AMUL back into this codebase.

# Building

- Cross platform: Windows, Linux, hopefully MacOS
- CMake based until there is a good user story for using Bazel from Visual
  Studio or VS Code
- GoogleTest for unit tests
- GoogleFlags (gflags) for command line options
- GoogleLog (glog) for logging

# Running

AMUL aka SMUGL is actually a game engine, so in order to do anything with it
you need a game definition,

Original AMUL had separate compilation, daemon and client executables. I'm
leaning towards a single executable this tine,

# Building a Game

The game is defined in a series of text files which you run the game compiler
(amulcom) over.

# Playing a game

After compiling, launch the srver (aman) and connect with a client (amul).
