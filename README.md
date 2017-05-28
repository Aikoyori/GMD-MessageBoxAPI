# GMD-MessageBoxAPI
Simply put, a C++ library for displaying custom message boxes onto Geometry Dash.

Only supported for the Steam release 2.102 of the game on Windows PCs.

## Setup

Add the [MBXAPI.h](source/MBXAPI.h) file to your headers directory and the [MBXAPI.cpp](source/MBXAPI.cpp) file in your sources directory,
then just include the header at the start of the project (no need to include Windows.h as it is included for you).

You can find examples in the [examples](examples) folder.

## Use

You can use colors, there are 7 valid colors (any invalid colors will be set to red): r = red, y = yellow, l = light blue, o = orange, g = green, j = turquoise, p = pink.
To use colors check the [color example](examples/color.cpp).

Do not go beyond 127 characters for the title, content and button text, bypassing this limitation for the content will result in a game crash.

Only use [printable ASCII characters](http://www.ascii-code.com/).
