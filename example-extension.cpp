
#include "shadron-api.h"

#include <cstdlib>
#include <cstring>
#include <cmath>

#ifdef __cplusplus
extern "C" {
#endif

// The extension's name
#define EXAMPLE_EXTENSION_NAME "example"
// The extension's initializer name
#define EXAMPLE_INITIALIZER_NAME "example"
// The extension's object dimensions
#define EXAMPLE_OBJECT_WIDTH 640
#define EXAMPLE_OBJECT_HEIGHT 480

// A structure to hold an object's data
struct ObjectData {
    int animated;
};

// Function that will generate the output image
static void generateImage(void *pixels, int width, int height, int animated, float time);

// Register the extension - this will be called once at program start
int __declspec(dllexport) shadron_register_extension(int *magicNumber, int *flags, char *name, int *nameLength, int *version, void **context) {
    // Set magic number to the correct value, to prove that we are interfacing with the correct API
    *magicNumber = SHADRON_MAGICNO;
    // Report the capabilities of this extension and string encoding using the flags argument
    *flags = SHADRON_FLAG_IMAGE|SHADRON_FLAG_ANIMATION|SHADRON_FLAG_CHARSET_UTF8;
    // Verify that the length of extension name buffer is sufficient (this will be always 256 but just to be sure)
    int actualNameLength = sizeof(EXAMPLE_EXTENSION_NAME)-1;
    if (*nameLength < actualNameLength)
        return SHADRON_RESULT_UNEXPECTED_ERROR;
    // Copy the extension name to the supplied buffer and report its length
    memcpy(name, EXAMPLE_EXTENSION_NAME, actualNameLength);
    *nameLength = actualNameLength;
    // Set the extension version to 100 (1.00)
    *version = 100;
    // We do not need a global context, but otherwise, you can output a pointer here, which will be passed back to you in all of the following functions
    *context = NULL;
    // Success - now the extension can be registered
    return SHADRON_RESULT_OK;
}

// Unregister the extension before closing the program
int __declspec(dllexport) shadron_unregister_extension(void *context) {
    // We do not need to do any cleanup, so let's just return OK
    return SHADRON_RESULT_OK;
}

// Register initializers - this function will query initializer keywords until we return NO_MORE_ITEMS
int __declspec(dllexport) shadron_register_initializer(void *context, int index, int *flags, char *name, int *nameLength) {
    // If the first initializer is being requested, we output its type and name
    if (index == 0) {
        // The example object can be either an image or an animation
        *flags = SHADRON_FLAG_IMAGE|SHADRON_FLAG_ANIMATION;
        // Again, we verify that the name fits the buffer (again, just to be sure)
        int actualNameLength = sizeof(EXAMPLE_INITIALIZER_NAME)-1;
        if (*nameLength < actualNameLength)
            return SHADRON_RESULT_UNEXPECTED_ERROR;
        // And copy the name
        memcpy(name, EXAMPLE_INITIALIZER_NAME, actualNameLength);
        *nameLength = actualNameLength;
        // Submit the initializer using OK as the result code
        return SHADRON_RESULT_OK;
    }
    // Otherwise, we report that there are no more initializers to be listed
    return SHADRON_RESULT_NO_MORE_ITEMS;
}

// The extension's initializer has been detected, and we need to parse its arguments
int __declspec(dllexport) shadron_parse_initializer(void *context, int objectType, int index, const char *name, int nameLength, void **parseContext, int *firstArgumentTypes) {
    // The index and name indicate which initializer is being parsed. We can check that it is indeed our only one
    int actualNameLength = sizeof(EXAMPLE_INITIALIZER_NAME)-1;
    if (!(index == 0 && nameLength == actualNameLength && !memcmp(name, EXAMPLE_INITIALIZER_NAME, actualNameLength)))
        return SHADRON_RESULT_UNEXPECTED_ERROR;
    // We can check that the object is either image or an animation, otherwise, we don't know what to do with it
    if (!(objectType == SHADRON_FLAG_IMAGE || objectType == SHADRON_FLAG_ANIMATION))
        return SHADRON_RESULT_UNEXPECTED_ERROR;
    // Here we can prepare a data structure to store parsed data as they arrive, but since we won't accept any arguments, it is not necessary
    *parseContext = NULL;
    // Now we need to specify what we expect as the first argument. We don't accept any arguments though
    *firstArgumentTypes = SHADRON_ARG_NONE;
    // Return success
    return SHADRON_RESULT_OK;
}

// This will be called for each valid argument
int __declspec(dllexport) shadron_parse_initializer_argument(void *context, void *parseContext, int argNo, int argumentType, const void *argumentData, int *nextArgumentTypes) {
    // Since we do not accept any arguments, receiving one indicates an error
    return SHADRON_RESULT_PARSE_ERROR;
}

// This will be called after the initializer has been parsed, and needs to be either turned into an object, or dropped
int __declspec(dllexport) shadron_parse_initializer_finish(void *context, void *parseContext, int result, int objectType, const char *objectName, int nameLength, void **object) {
    // The result argument indicates whether the initializer has been parsed successfully. If not, there is nothing else to do here (make sure to destroy parseContext if you created one though)
    if (result != SHADRON_RESULT_OK)
        return SHADRON_RESULT_OK;
    // If successful, create the object. To avoid recreating the same object when reloading the script, you can use objectName to find a previous instance instead
    struct ObjectData *objectData = (struct ObjectData *) malloc(sizeof(struct ObjectData));
    if (!objectData)
        return SHADRON_RESULT_UNEXPECTED_ERROR;
    // Store whether this is an image or an animation
    if (objectType == SHADRON_FLAG_IMAGE)
        objectData->animated = 0;
    else if (objectType == SHADRON_FLAG_ANIMATION)
        objectData->animated = 1;
    else { // If it is neither, it's an error
        free(objectData);
        return SHADRON_RESULT_UNEXPECTED_ERROR;
    }
    // Store the object data. This pointer will be passed back in each of the shadron_object functions
    *object = objectData;
    // Return success
    return SHADRON_RESULT_OK;
}

// If a parse error is encountered, we can output an error message, but first its length
int __declspec(dllexport) shadron_parse_error_length(void *context, void *parseContext, int *length, int encoding) {
    // We do not have any error messages available, so we can instead return the NO_DATA result code
    return SHADRON_RESULT_NO_DATA;
}
int __declspec(dllexport) shadron_parse_error_string(void *context, void *parseContext, void *buffer, int *length, int bufferEncoding) {
    // Since we didn't output an error message length, this should never be called, but let's do the same anyway
    return SHADRON_RESULT_NO_DATA;
}

// Once the entire script is parsed successfully, the objects will be initialized with this function
int __declspec(dllexport) shadron_object_prepare(void *context, void *object, int *flags, int *width, int *height, int *format) {
    // We do not need to perform any initialization, but must output the object flags, dimensions, and format
    // The flags argument contains some hints for the preparation process (e.g. FULL_RANGE or HARD_RESET), but we ignore those and clear the flags to indicate no extra features are supported
    *flags = 0;
    // Set the dimensions
    *width = EXAMPLE_OBJECT_WIDTH;
    *height = EXAMPLE_OBJECT_HEIGHT;
    // We will use the regular 32-bit RGBA format
    *format = SHADRON_FORMAT_RGBA_BYTE;
    // Return success
    return SHADRON_RESULT_OK;
}

// The object's dimensions may be queried again if we specifically indicate they have changed
int __declspec(dllexport) shadron_object_size(void *context, void *object, int *width, int *height, int *format) {
    // Since we won't change the object's dimensions, this should never be called, but we will output the correct dimensions neverthless
    *width = EXAMPLE_OBJECT_WIDTH;
    *height = EXAMPLE_OBJECT_HEIGHT;
    *format = SHADRON_FORMAT_RGBA_BYTE;
    return SHADRON_RESULT_OK;
}

// This will be called if the user attempts to load a file into the object
int __declspec(dllexport) shadron_object_load_file(void *context, void *object, const void *path, int pathLength, int pathEncoding) {
    // Since we never set up the object to receive files (FLAG_FILE_INPUT), this call should never happen, or it is an error
    return SHADRON_RESULT_UNEXPECTED_ERROR;
}
int __declspec(dllexport) shadron_object_unload_file(void *context, void *object) {
    // Same as before
    return SHADRON_RESULT_UNEXPECTED_ERROR;
}

// This function is used to set the value of an expression that can dynamically change
int __declspec(dllexport) shadron_object_set_expression_value(void *context, void *object, int exprIndex, int valueType, const void *value) {
    // We never accepted any expressions as arguments, so this should never happen, but if it does, we will ignore it
    return SHADRON_RESULT_IGNORE;
}

// This function is called to supply the contents of another object to this one
int __declspec(dllexport) shadron_object_offer_source_pixels(void *context, void *object, int sourceIndex, int sourceType, int width, int height, int *format, void **pixelBuffer, void **pixelsContext) {
    // We never requested any source objects, so this should also never happen, but otherwise ignore it
    return SHADRON_RESULT_IGNORE;
}
int __declspec(dllexport) shadron_object_post_source_pixels(void *context, void *object, void *pixelsContext, int sourceIndex, int plane, int width, int height, int format, const void *pixels) {
    // Same as before
    return SHADRON_RESULT_IGNORE;
}

// Report special user commands to the object (currently only restarting animations using the R key)
int __declspec(dllexport) shadron_object_user_command(void *context, void *object, int command) {
    // We do not respond to any user commands
    return SHADRON_RESULT_IGNORE;
}

// Called to dispose of any object data
int __declspec(dllexport) shadron_object_destroy(void *context, void *object) {
    // Free the object data
    free(object);
    // Return success
    return SHADRON_RESULT_OK;
}

// In this function, we need to output the pixels for the object
int __declspec(dllexport) shadron_object_fetch_pixels(void *context, void *object, float time, float deltaTime, int realTime, int plane, int width, int height, int format, const void **pixels, void **pixelsContext) {
    // Retrieve the object data
    const struct ObjectData *objectData = (const struct ObjectData *) object;
    // Check that the requested format matches and that width and height make sense
    if (!(format == SHADRON_FORMAT_RGBA_BYTE && width > 0 && height > 0))
        return SHADRON_RESULT_UNEXPECTED_ERROR;
    // Allocate a pixel buffer
    void *pixelBuffer = malloc(4*width*height);
    if (!pixelBuffer)
        return SHADRON_RESULT_UNEXPECTED_ERROR;
    // Fill the pixel buffer
    generateImage(pixelBuffer, width, height, objectData->animated, time);
    // Output the pixels
    *pixels = pixelBuffer;
    // Store the pixel buffer into pixelsContext argument to be freed in release_pixels
    *pixelsContext = pixelBuffer;
    // Return success
    return SHADRON_RESULT_OK;
}

// This will be called afterwards, to free any data allocated during fetch_pixels
int __declspec(dllexport) shadron_object_release_pixels(void *context, void *object, void *pixelsContext) {
    // Free the bitmap buffer
    free(pixelsContext);
    // Return success
    return SHADRON_RESULT_OK;
}

// As this extension does not support exports, streams, or file decoding, we need not implement the rest of the API

// IMAGE GENERATION (may be ignored)
static void generateImage(void *pixels, int width, int height, int animated, float time) {
    const int q[5] = { 1, 1, -1, -1, 1 };
    unsigned char *p = (unsigned char *) pixels;
    double mult = 1.0/cos(2.0*time);
    int c[3] = { !animated || mult < 0, animated, animated && mult > 0 };
    for (int y = 0; y < height; ++y) {
        double py = 2.0*(y+0.5)/height-1.0;
        for (int x = 0; x < width; ++x) {
            double px = (2.0*(x+0.5)/width-1.0)*mult*width/height;
            double sd = 0.5-(abs(px)+abs(py));
            for (int i = 0; i < 4; ++i) {
                double s = q[i]*q[i+1];
                double xd = q[i]*(0.25-s*0.09)-px;
                double yd = q[i+1]*(0.25-s*0.09)-py;
                double b = sqrt(xd*xd+yd*yd)-0.11;
                double h = 0.5+0.5/0.11*(b-s*sd);
                h = h >= 0.0 && h <= 1.0 ? h : h > 0.0;
                sd = s*(s*sd*h+b*(1.0-h)-0.11*h*(1.0-h));
            }
            double a = 128.0*(sd*height+1.0);
            *p++ = (unsigned char) ((128.0+127.0*py)*c[0]);
            *p++ = (unsigned char) ((64.0+64.0*py)*c[1]);
            *p++ = (unsigned char) ((128.0+127.0*py)*c[2]);
            *p++ = (unsigned char) (a >= 0.0 && a <= 255.0 ? a : 0xff*(a > 0.0));
        }
    }
}

#ifdef __cplusplus
}
#endif
