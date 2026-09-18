# Contributing

## General Guidelines

If you feel fit to contribute, feel free to create a pull request! Someone will review and merge it (or provide feedback) as soon as possible.

Please keep your pull requests small and understandable; you may be able to shoot ahead and make a lot of progress in a short amount of time, but this is a collaborative project, so you must allow others to catch up and follow along. Large pull requests become significantly more unwieldy to review, and as such make it exponentially more likely for a mistake or error to go undetected. They also make it harder to merge other pull requests because the more files you modify, the more likely it is for a merge conflict to occur. A general guideline is to keep submissions limited to one class at a time. Sometimes two or more classes may be too interlinked for this to be feasible, so this is not a hard rule, however if your PR is starting to modify more than 10 or so files, it's probably getting too big.

This repository has achieving platform independence as its primary goal. Any changes that modify code in `game-src` and `editor-src` are unlikely to be accepted, unless they directly serve to increase platform compatibility.

## Overview

* [`3rdparty`](/3rdparty): Contains code obtained from third parties, not including MDF/DraHma. Generally, these are libraries that have been placed in the public domain or are freely available on the web. As these are unaltered files, our style guide (see below) does not apply.
* [`compatlib`](/compatlib): Source code providing shims for DOS functions, as well as alternative implementations of classes and functions from the original game.
* [`editor-src`](/edito-src): Source code based on the original Polanie Editor code donated by Mirosław Dymek to the author of the polanie.prv.pl website. It is not prohibited to completely replace classes, functions, and files in the Editor, provided that it works and looks exactly the same as the original under DOS.
* [`game-src`](/game-src): Source code based on the original Polan code donated by Mirosław Dymek to the author of the polanie.prv.pl website. It is strictly prohibited to replace classes not related to computer hardware: `battle.cpp`, `decision.cpp`, `graphics.cpp`, `mapa.cpp`, `mover1.cpp`, `world.cpp`. It is not prohibited to replace the `image13h.cpp` file, but you must be careful not to introduce graphics bugs when doing so.

## Code Style

In general, we aren't strict about coding style, but we'd really appreciate it if you could follow these guidelines:

### Original codebase

When editing the original game code (while, of course, taking all of the above assumptions into account), you should interfere with the original formatting as little as possible. This is because the formatting used there is quite ambiguous, and incorrect indentation could completely change how the game works.

If you modify the game code (except when completely replacing a class or function with a new one), mark all changes made to the code with comments. For example:
```c++
extern int licznik; // [PORT] Added licznik back
```

If your changes span multiple lines, use blocks consisting of multi-line comments. For example:
```c++
/* [PORT] Comment out unnecessary code */
/*
counter = 0;
PlayTrack(2);
....
*/
/* END [PORT] Comment out unnecessary code */
```

If you add new functions from the `compatlib` library to your game code, those functions must have names that begin with `PORT`, in addition to the accompanying comments when using them.

### Everything else

For the remaining code (including replacement classes), there are no strict requirements; you should simply follow the format of the existing files and the conventions used there.
