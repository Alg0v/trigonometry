# Trigonometry

An SDL program to show the link between circles and the sine and cosine functions.

##  How to build

### Dependencies
`extra/libx11 extra/sdl2`
On arch Linux

### Build
Just use g++ :
```bash
g++ main.cpp -o trigonometry -lX11 -lSDL2
```

## Running

### Window mode

By default the script runs as an X11 wallpaper. To run it as a window, you may want to use the `-w` option.

### Wallpaper mode

You can specify the number of the screen whose wallpaper will be changed with `-s NUM`

### Customization

#### Changing the color
To change the color, you can just change the RGB values with the `-r NUM`, `-g NUM` and `-b NUM`.

#### Changing the radius of the circle

To change the radius of the circle, you can use the `-R NUM` option.
Be careful though, because high values can crash the program.
