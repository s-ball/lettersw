# LettersW

-----

## Table Of Contents
* [Goal](#goal)
* [Status](#status)
* [Installation](#installation)
    * [End User](#end-user-installation)
    * [Developer](#developer-installation)
* [Usage](#usage)
* [Compatibility](#compatibility)
* [Contributing](#contributing)
* [Acknowledgements](#acknowledgments)
* [License](#license)

## Goal

This Windows application is intended to help finding words is games like
*Garden of Words*. Specifically it uses a dictionary to find words composed
of a given list of letters and matching a pattern.

Currently only a French dictionary is available. So it can only be used for
the French localized *Jardin des mots* game, or other French games.

## Status

The application is currently beta quality. It still lacks tests, including
real world ones and a decent documentation.

Starting with the 0.5 version, the application and its sources are available
from GitHub.

## Installation

### End User Installation

For releases starting at the 0.7 version, the application is available on GitHub.

Just download it and install it on your system.

Disclaimer: The application should not rely on third party software.
Nevertheless, current version will probably depend on an
installation of Visual C++ redistributable libraries, because
it uses the standard C++ library (untested).

### Developer Installation

You should get the full source from [GitHub](https://github.com/s-ball/lettersw.git)

## Usage

You will have to declare the available letters to build the words.

Next, you declare the mask for the words you are searching. Here you
will use a `_` character for any unknow letter, and the relevant character
for any known one. For example when searching for a 4 letters word having
a `t` in second position, you would use `_t__` as mask.

As a special case, the `*` mask will return all the possible words
starting at length 3. It is intended as a help for the Daily Puzzle.

NB: `LettersW` only accept and uses upper case ASCII letters. Lower case
ones will be mapped to their lower case equivalent, as will accented ones
. So `éèÉçï` should become
`EEECI`.

## Compatibility

The application is built and tested on a Windows 11 system with Visual Studio 2022.
It is expected to run on any post Vista system but nothing has been tested yet.
Feel free to submit a bug report if something goes wrong
on any system starting with XP.

## Contributing

I shall always be glad to receive issues or Pull Requests on GitHub. But as
I am the only maintainer, I cannot guarantee to react quickly to them. Please
feel free to contact me by mail if I do not answer quickly enough...

## Acknowledgments

The French dictionary is taken from the project
[dict-fr-AU-DELA](https://github.com/HubTou/dict-fr-AU-DELA/) from
Hubert Tournier.

## License

`LettersWord` is distributed under the terms of the [MIT](https://spdx.org/licenses/MIT.html) license.