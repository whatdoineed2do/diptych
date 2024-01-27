# diptych
creates di/tri/n-typch images from input files; scales them so them fit on the final image which can be a specified image size or an image size determined based on image inputs.

## Compiling / Dependancies
This util depends on `C++ 17`, `ImageMagick` and optionally `exiv2` which will preserving common EXIF amognst the input images onto the final output image.

## Example Usage
### Typical ###
create a horizontal dipytch (`-s 1:1`) from the 2 images, creating an internal border (`-b`) of 50pxls and external boder (`-B`) of 10pxls using black (`-C black` where white is default) as the border colour.

Inputs:
- ![Alt text](doc/blue.jpg?raw=true "BLUE 300x300") 300x300
- ![Alt text](doc/red.jpg?raw=true "RED 200x400") 200x400
- ![Alt text](doc/green.jpg?raw=true "GREEN 500x500") 500x500

```
diptych -O 300 -B 10 -b 50 -C black -s 1:1  blue.jpg red.jpg -o final.jpg
```
Resulting in an 300x185 image that is scaled to the requested output (note the size of text on 'red')
![Alt text](doc/final.jpg?raw=true "final.jpg")

To create a _vertical dipytch_ (stacked on top of each other) using the same files above, use `-s 2`

### Resizing, adding Borders, output quality ###
```
diptych \
    -O 450 -B 20 -b 35 -C white -q 85 \
    -s 2:1 red.jpg green.jpg blue.jpg \
  -o rgb.jpg
```
This will scale (`-O`) the combined images to 480 pixels on the longest edge which includes the boders as specified.  The quality (`-q`) of the image is set of 85%.  All the `ImageMagick` resize scaling (_filter_) algorithms can be specified when scaling via `-f <IM filter name>`.

This will produce a vertical image (stacking `red.jpg` and `green.jpg`) and then using that result to create a horizontal diptypch with `blue.jpg` - the output will be scalled so that all images proportions will fit the final out.

Results in a 467x340 image that is then saled to the requested 450x328 output.
![Alt text](doc/rgb.jpg?raw=true "rgb.jpg")

We can also specify a single image to scale, resize, add borders etc if desired.
