
# Shadron Example Extension

This extension serves as an introduction to the [Shadron extension API](shadron-api.h).
You may use this as a basis for your own extension or as a reference.
See the [full documentation](http://www.arteryengine.com/shadron/doc/extension-api/index.html)
to make sure you use the API correctly.

## Build and installation

### Windows

Use Microsoft Visual Studio 2015 or later to build the project in release configuration,
and run `install-to-shadron.bat` to copy the DLL to the appropriate location.
Shadron searches for extensions in the `extensions` subdirectory within its own directory,
and in `%APPDATA%\Shadron\extensions`.

### Mac

Run `make` to build `shadron-example.dylib` and then run `make install`, which will
copy it to `~/.config/Shadron/extensions` where it will be detected by Shadron.

## Usage

To use an extension in a script, it must first be enabled with the `#extension` directive:

    #extension example

Now, you may generate an example image or an animation with this extension like so:

    image ImageTest = example();
    animation AnimationTest = example();

You can use the premade [extension test](extension-test.shadron) script, which does this.
