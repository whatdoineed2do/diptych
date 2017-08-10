# diptych
creates di/tri/n-typch images from input files; scales them so them fit on the final image which can be a specified image size or an image size determined based on image inputs.

## Compiling / Dependancies
This util depends on `ImageMagick` and `exiv2` - to complie:
```
g++ -DGEN_EXIF -DNEED_UCHAR_UINT_T \
    $(pkg-config Magick++ --cflags) $(pkg-config exiv2 --cflags) \
    diptych.cc \
    $(pkg-config Magick++ --libs) $(pkg-config exiv2 --libs) \
  -o diptych
```
Remove `-DGEN_EXIF` and references to `pkg-config exiv2` if exif info is not required in the output.

## Example Usage
### Typical ###
create a horizontal dipytch (`-s 1:1`) from the 2 images, creating an internal border (`-b`) of 50pxls and external boder (`-B`) of 10pxls using white (`-C white` which is default) as the border colour.

![Alt text](blue.jpg?raw=true "BLUE")
![Alt text](red.jpg?raw=true "RED")

```
diptych -O 300 -B 10 -b 50 -C white -s 1:1  blue.jpg red.jpg -o final.jpg
```
![Alt text](final.jpg?raw=true "RED")

To create a vertical dipytch using the same files above, use `-s 2`

### Resizing and adding Borders ###
whilst this can be done with `ImageMagick` directly, getting the syntax correct has always annoyed me.  To do the same with this util:

```
diptych -O 480 -B 20 -b 35 -C white -q 85 original.jpg -o scaled.jpg
```
This will scale (`-O`) the original image to 480pxls on the longest edge which includes the boders as specified.  The quality (`-q`) of the image is set of 85%.

### Extended ###
Slightly more complex renderings can be acheived via `-s`.

```
diptych -O 1024 -B 20 -b 35 -C white -s 2:1  a.jpg b.jpg c.jpg -o x.jpg
```
This will produce a vertical image (stacking `a.jpg` and `b.jpg`) and then using that result to create a horizontal diptypch with `c.jpg` - the output will be scalled so that all images proportions will fit the final out.
