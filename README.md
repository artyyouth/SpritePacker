SpritePacker
======================================================

## The problem

Given a list of NxM images, pack them into one or more 1024x1024 images ("spritesheets"). The algorithm should create the fewest number of spritesheets possible.

Given a list of image sizes in stdin in the following format:

864x480 78x107 410x321 188x167 315x274 229x163 629x236 39x32 193x56 543x155

You should produce on stdout like the following. Each image line is the dimensions of the image, and then the X and Y coordinate of where it should be placed in the spritesheet:

    sheet 1
    864x480 0 0
    410x321 0 480
    315x274 410 480
    188x167 725 480
    229x163 0 801
    543x155 229 801
    78x107 913 0
    193x56 725 647
    39x32 913 107

    sheet 2
    629x236 0 0

## The solution

The code is compiled and run successfully with both (VS2010 + Win7) and (Mac OSX 10.7.4 + Xcode 4.2.1).
