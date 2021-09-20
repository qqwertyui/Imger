Very simple and limited project of image viewer. It uses plain SDL2 (without image additions). It's purpose was to get familiar with different image formats and it's internals. I wouldn't recommend using it as a normal image viewer - if I would like to make a real application I would use Qt or at least libpng, libjpeg etc. and would not try inventing wheel again.

It has partial support for following formats:
- PNG
- BMP
- CUR (it is actually an wrapper for BMP, PNG and other formats)

1. Requirements:
- SDL2
- g++ with c++17 support (I'm using x86_64-posix-seh-rev0, 8.1.0)

2. Build:
It's pretyy straightforward if you have all dependencies:
- git clone https://github.com/qqwertyui/Imger.git && cd Imger
- make

3. Usage:
.\imger.exe <path_to_image>

In case of errors you will hopefully get notificated by error message and exit code, for example:

C:\Users\qwertyy\Desktop\imger> .\imger.exe .\image.gif

[!] Couldn't load image: Unrecognized format

C:\Users\qwertyy\Desktop\imger> echo $LASTEXITCODE

3
