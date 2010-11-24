/*
 *  ImageConversion.h
 *  Retarget
 *
 *  Created by Raymond Daly on 3/11/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>

#include <CoreGraphics/CoreGraphics.h>

#include <opencv/cv.h>

#ifndef IMAGE_CONVERSION_H
#define IMAGE_CONVERSION_H

typedef enum {
	ImageConversionOrientationUp, // normal
	ImageConversionOrientationDown, // 180 degree rotation
	ImageConversionOrientationLeft, // 90 deg CCW
	ImageConversionOrientationRight, // 90 deg CW
	// as above but mirrored along other axis
	ImageConversionOrientationUpMirrored, // horizontal flip
	ImageConversionOrientationDownMirrored, //horizontal flip
	ImageConversionOrientationLeftMirrored, // vertical flip
	ImageConversionOrientationRightMirrored // vertical flip
} ImageConversionOrientation;

// convert an ipl image to a cg image
CGImageRef CGCreateImageFromIplImage(IplImage * img);

// convert a CG image to an IplImage of the provided size
IplImage * CGImageToIplImg(CGImageRef cgImage, int width, int height);

// convert a CG image to an IplImage, reusing memory and size data
void CGImageToIplImg2(CGImageRef cgImage, IplImage * img, int width, int height, ImageConversionOrientation orientation);

// implementation?
void makeIplImageFromCGBitmapGraphicsContext(CGContextRef context, bool copyData, IplImage * o_IplImage);

#endif
