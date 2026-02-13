# Sheet Image Processor

This is Sheet Image Processor, an image processor designed for processing images containing multiple sub-images.

A "sheet image" is an image containing multiple sub-images: a sheet of (sub-)images.  
Sheet images are often used for things like tile sheets and sprite sheets, allowing many images to be contained in a single image - and, therefore, a single texture.

This header-only library contains its own Image type (allowing conversion between image types as well as simple importing and exporting from pixel arrays) as well as its own Atlas type (a texture atlas is way of storing rectangles required to contain each sub-image).  
It also has its own smaller types (including Xy, containing x and y components, as well as Pixel and Rect). Note that these types all unsigned types.

As well as common processes such as clearing, flipping, resizing an image, some graphics editing abilities such as flood fill, and more specific processes are available including things such as expanding a tile (sub-image), 'spreading' the edge pixels outwards, "packing" a set of rectangles (in the Atlas), allowing compacting of tiles within an image, and generating an Atlas from a grid.

Currently, the Image type can work with the following formats: top-down or bottom-up, RGBA or BGRA.  
Using Image's setPixel and getPixel automatically takes into account this format so it's seamless when converting between the types, which is also simple to do.

To ease import or export, a pointer to an array of bytes can be used.  
For export, Image provides a pointer to std::uint8_t, containing the internal data. Note that this is in the format of the Image so make sure this is the format you expect (or convert it using Image).  
For import, Image takes a pointer to std::uint8_t, which should contain the byte data. Note that this should be in the format that Image expects. If not, first convert Image to the format required (you can always convert back after import).
