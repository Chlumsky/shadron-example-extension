
# Shadron Example Extension

This extension serves as an introduction to the [Shadron extension API](shadron-api.h).
You may use this as a basis for your own extension or as a reference.
See the [full documentation](http://www.arteryengine.com/shadron/doc/extension-api/index.html)
to make sure you use the API correctly.

## Build and installation

Use Microsoft Visual Studio 2015 to build the project in release configuration,
and run `install-to-shadron.bat` to copy the DLL to the appropriate location.
Shadron searches for extensions in `extensions` directory in its own directory,
and in `%APPDATA%\Shadron\extensions`.

## Usage

To use an extension in a script, it must first be enabled with the `#extension` directive:

    #extension example

Now, you may generate an image or an animation with the extension like this:

    image ImageTest = example();
    animation AnimationTest = example();

Try out the [extension test](extension-test.shadron) script, which does this.
