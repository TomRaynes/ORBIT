# Installation Guide for Windows

## 1. Install MinGW-w64
Download and install **MinGW-w64** from:
- https://www.mingw-w64.org/

During installation:
- Choose **posix** threads and **seh** exception handling (for 64-bit)
- Add MinGW `bin` directory to your system path so `gcc` works in the terminal

## 2. Download SDL2 Development Library
- Visit: https://www.libsdl.org/download-2.0.php
- Download: `SDL2-devel-2.x.x-mingw.tar.gz` (choose the MinGW variant)
- Extract to a location like `C:\SDL2\SDL2`

## 3. Download SDL2_image Development Library
- Visit: https://www.libsdl.org/projects/SDL_image/
- Download: `SDL2_image-devel-2.x.x-mingw.tar.gz`
- Extract to: `C:\SDL2\SDL2_image`

You should now have:
```
C:\SDL2\
├── SDL2\
│   ├── include\
│   └── lib\
├── SDL2_image\
    ├── include\
    └── lib\
```

## 4. Clone the repo and navigate to its top level:
```
git clone git@github.com:TomRaynes/ORBIT.git
cd ORBIT
```

## 5. Compile the code:

```bash
gcc orbit.c -IC:\SDL2\SDL2\include -IC:\SDL2\SDL2_image\include ^
-LC:\SDL2\SDL2\lib -LC:\SDL2\SDL2_image\lib ^
-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -o orbit.exe
```

## 6. Copy Required DLLs

Copy the following DLLs to your ORBIT directory containing `orbit.exe`:
- `SDL2.dll` (from `SDL2/lib`)
- `SDL2_image.dll` (from `SDL2_image/lib`)
- Plus any dependencies from the `lib` directories, such as:
  - `zlib1.dll`
  - `libpng16-16.dll`
  - `jpeg-9.dll`

## 7. Run the program:
```bash
orbit.exe
```
