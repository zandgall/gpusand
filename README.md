# Quickwork

An OpenGL project framework intended to be incredibly easy and quick to set up and make a project with. It has limited functionality and utility, but provides the ability to start OpenGL and create a window within a line, along with the ability to quickly and easily load resources.

## stb_image and Glad

All necessary third-party files are kept in the third-party folder.

This project uses Glad (v2). Check it out [here](https://gen.glad.sh/) to make sure that the Glad version you use is compatible with your computer.
The Glad's version I downloaded can be found [here](https://gen.glad.sh/#generator=c&api=gl%3D4.6&profile=gl%3Dcompatibility%2Cgles1%3Dcommon), simply being OpenGL 4.6, with no extensions, and none of the options ticked.

If you wish to use your own version of Glad, know that this project isn't designed for header only Glad. Download your glad files, and put them in the "third-party" folder. There should be a `src/gl.c`, `include/glad/gl.h`, and `include/KHR/khrplatform.h` included.

This project also uses [stb_image](https://github.com/nothings/stb) in order to load images. All that is needed is `stb_image.h` inside the `third-party/include` directory.

## Design

This project is designed for testing and quickly prototyping programs or ideas. It therefore comes with several tools to make use in debugging, mostly in terms of graphics.
