/*
 *  ImageConversion.c
 *  Retarget
 *
 *  Created by Raymond Daly on 3/11/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "ImageConversion.h"


CGContextRef CreateARGBBitmapContext (CGImageRef inImage, int width, int height)
{
    CGContextRef    context = NULL;
    CGColorSpaceRef colorSpace;
    void *          bitmapData;
    int             bitmapByteCount;
    int             bitmapBytesPerRow;
	
	// Get image width, height. We'll use the entire image.
    size_t pixelsWide = width;
    size_t pixelsHigh = height;
	
    // Declare the number of bytes per row. Each pixel in the bitmap in this
    // example is represented by 4 bytes; 8 bits each of red, green, blue, and
    // alpha.
    bitmapBytesPerRow   = (pixelsWide * 4);
    bitmapByteCount     = (bitmapBytesPerRow * pixelsHigh);
	
    // Use the generic RGB color space.
    colorSpace = CGColorSpaceCreateDeviceRGB();//CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
    if (colorSpace == NULL)
    {
        //NSLog("Error allocating color space\n");
        return NULL;
    }
	
    // Allocate memory for image data. This is the destination in memory
    // where any drawing to the bitmap context will be rendered.
	// apparently we do not need to alocate our own memory, but
	// we do any way because we can simply use the same buffer
	// when we create an IplImage from the CGImage
    bitmapData = malloc( bitmapByteCount );
    if (bitmapData == NULL) 
    {
        //fprintf (stderr, "Memory not allocated!");
        CGColorSpaceRelease( colorSpace );
        return NULL;
    }
	
    // Create the bitmap context. We want pre-multiplied ARGB, 8-bits 
    // per component. Regardless of what the source image format is 
    // (CMYK, Grayscale, and so on) it will be converted over to the format
    // specified here by CGBitmapContextCreate.
    context = CGBitmapContextCreate (bitmapData,
									 pixelsWide,
									 pixelsHigh,
									 8,      // bits per component
									 bitmapBytesPerRow,
									 colorSpace,
									 kCGImageAlphaPremultipliedFirst);
	
    // Make sure and release colorspace before returning
    CGColorSpaceRelease( colorSpace );
	//free (bitmapData);
	
    return context;
}

IplImage * CGImageToIplImg(CGImageRef cgImage, int width, int height)
{
	IplImage * result = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	
	// this image data is in 4 byte [alpha red green blue] chunks.  to get the
	// pixel at (x, y) you must go 4 * (x + y * width) bytes into the array
	CGContextRef bmContext = CreateARGBBitmapContext(cgImage, width, height);
	unsigned char * cgImageData = (unsigned char*)CGBitmapContextGetData(bmContext);
	
	//CGContextRef contextRef = CGBitmapContextCreate(img->imageData, img->height, img->width, 8, img->width*3, CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB ), kCGImageAlphaNone);
	CGRect rect = CGRectMake(0, 0, width, height);
	CGContextDrawImage(bmContext, rect, cgImage);
	CGContextRelease(bmContext);
	
	for (int i=0; i < width; i++)
	{
		for (int j=0; j < height; j++)
		{
			unsigned char * pixelOffset = cgImageData + (i + j * width);
			unsigned char r = pixelOffset[1];
			unsigned char g = pixelOffset[2];
			unsigned char b = pixelOffset[3];
			CvScalar value = cvScalar(r, g, b, 0);
			cvSet2D(result, j, i, value);
		}
	}
	
	return result;
}

CGAffineTransform transformForImageConversionOrientation(ImageConversionOrientation orientation, int width, int height, CGRect * o_bounds)
{
	CGAffineTransform transform = CGAffineTransformIdentity;
	CGRect bounds = CGRectMake(0, 0, width, height);
	CGSize size = CGSizeMake(width, height);
	float boundHeight;
	
	
	switch(orientation)
	{
			
		case ImageConversionOrientationUp:
			transform = CGAffineTransformIdentity;
			break;
			
		case ImageConversionOrientationUpMirrored: 
			transform = CGAffineTransformMakeTranslation(size.width, 0.0);
			transform = CGAffineTransformScale(transform, -1.0, 1.0);
			break;
			
		case ImageConversionOrientationDown: 
			transform = CGAffineTransformMakeTranslation(size.width, size.height);
			transform = CGAffineTransformRotate(transform, M_PI);
			break;
			
		case ImageConversionOrientationDownMirrored:
			transform = CGAffineTransformMakeTranslation(0.0, size.height);
			transform = CGAffineTransformScale(transform, 1.0, -1.0);
			break;
			
		case ImageConversionOrientationLeftMirrored://
//			boundHeight = bounds.size.height;
//			bounds.size.height = bounds.size.width;
//			bounds.size.width = boundHeight;
			transform = CGAffineTransformMakeTranslation(size.height, size.width);
			transform = CGAffineTransformScale(transform, -1.0, 1.0);
			transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
			break;
			
		case ImageConversionOrientationLeft: //
//			boundHeight = bounds.size.height;
//			bounds.size.height = bounds.size.width;
//			bounds.size.width = boundHeight;
			transform = CGAffineTransformMakeTranslation(0.0, size.width);
			transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
			break;
			
		case ImageConversionOrientationRightMirrored://
//			boundHeight = bounds.size.height;
//			bounds.size.height = bounds.size.width;
//			bounds.size.width = boundHeight;
			transform = CGAffineTransformMakeScale(-1.0, 1.0);
			transform = CGAffineTransformRotate(transform, M_PI / 2.0);
			break;
			
		case ImageConversionOrientationRight: //
			boundHeight = bounds.size.height;
			o_bounds->size.height = bounds.size.width;
			o_bounds->size.width = boundHeight;
			//float rotation = 3.0 * M_PI / 2.0;
			//transform = CGAffineTransformMakeTranslation(size.width, size.height);
			//transform = CGAffineTransformRotate(transform, rotation);
			
			transform = CGAffineTransformMakeTranslation(0.0, size.height);
			transform = CGAffineTransformRotate(transform, 3.0 * M_PI / 2.0);
			break;
	}
	
	return transform;
}



void CGImageToIplImg2(CGImageRef cgImage, IplImage * img, int width, int height, ImageConversionOrientation orientation)
{	
	img->nSize = sizeof(IplImage);
	img->ID = 0;
	img->nChannels = 3;
	img->depth = IPL_DEPTH_8U;
	img->dataOrder = 0; // interleaved color channels
	img->origin = 0; // top left
	img->align = 4; // apparently this is ignored in favor of width step
	img->width = width;
	img->height = height;
	img->imageId = NULL;
	img->maskROI = NULL;
	img->tileInfo = NULL; // must be null
	img->roi = NULL;
	img->widthStep = img->width*img->nChannels; // ???? // size of aligned row in bytes
	img->imageSize = img->height*img->widthStep;
	img->colorModel[0] = 'R'; img->colorModel[1] = 'G'; img->colorModel[2] = 'B';
	img->channelSeq[0] = 'B'; img->channelSeq[1] = 'G'; img->channelSeq[2] = 'R';
	
	CGContextRef bmContext = CreateARGBBitmapContext(cgImage, width, height);
	
	img->imageData = (char*)CGBitmapContextGetData(bmContext);
	img->imageDataOrigin = img->imageData;
	
	//CGContextRef contextRef = CGBitmapContextCreate(img->imageData, img->height, img->width, 8, img->width*3, CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB ), kCGImageAlphaNone);
	CGRect imageDrawBounds = CGRectMake(0, 0, img->width, img->height);
	CGAffineTransform transform = transformForImageConversionOrientation(orientation, width, height, &imageDrawBounds);
	CGContextConcatCTM(bmContext, transform);
	
	CGContextDrawImage(bmContext, imageDrawBounds, cgImage);
	CGContextRelease(bmContext);
	
	char * imgData =img->imageData;
	int pixelIndex = 0;
	for (int i=0; i < width * height; i++)
	{
		int iTimes4 = i*4;
		unsigned char r = imgData[iTimes4+1];
		unsigned char g = imgData[iTimes4+2];
		unsigned char b = imgData[iTimes4+3];
		// convert from ARGB to BGR
		//pixelIndex++;
		imgData[pixelIndex++] = b;
		imgData[pixelIndex++] = g;
		imgData[pixelIndex++] = r;
	}
	
	// now our data is the ARGB values of the image
}

void makeIplImageFromCGBitmapGraphicsContext(CGContextRef context, bool copyData, IplImage * o_IplImage)
{
	// first establish some shit about the context
	int cWidth = CGBitmapContextGetWidth(context);
	int cHeight = CGBitmapContextGetHeight(context);
	CGImageAlphaInfo cAlphaInfo = CGBitmapContextGetAlphaInfo(context);
	CGBitmapInfo cBitmapInfo = CGBitmapContextGetBitmapInfo(context);
	int cBitsPerComponent = CGBitmapContextGetBitsPerComponent(context);
	char * cData = (char *)CGBitmapContextGetData(context);
	int cBitsPerPixel = CGBitmapContextGetBitsPerPixel(context);
	int cBytesPerRow = CGBitmapContextGetBytesPerRow(context);
	int cDataSize = cBytesPerRow * cHeight;
	int cNumChannels = 4;
	if (cAlphaInfo ==  kCGImageAlphaNone)
		cNumChannels = 3;
	else if (cAlphaInfo == kCGImageAlphaOnly)
		cNumChannels = 1;
    // this is the one thing we don't use* CGBitmapContextGetColorSpace
	
	if (cBitsPerComponent != 8)
	{
		printf("makeIplImageFromCGBitmapGraphicsContext must have 8 bits per component");
		return;
	}
	if (cBitsPerPixel != cBitsPerComponent * cNumChannels)
	{
		printf("cBitsPerPixel != cBitsPerComponent * cNumChannels");
		return;
	}
	
	IplImage * img = o_IplImage;
	img->nSize = sizeof(IplImage);
	img->ID = 0;
	img->nChannels = cNumChannels;
	img->depth = IPL_DEPTH_8U; // we only do this at the moment but we hsould probably support different color depths
	img->dataOrder = 0; // interleaved color channels
	img->origin = 0; // top left
	//img->align = 4; // apparently this is ignored in favor of width step
	img->width = cWidth;
	img->height = cHeight;
	img->imageId = NULL;
	img->maskROI = NULL;
	img->tileInfo = NULL; // must be null
	img->roi = NULL;
	img->colorModel[0] = 'R'; img->colorModel[1] = 'G'; img->colorModel[2] = 'B';
	img->channelSeq[0] = 'B'; img->channelSeq[1] = 'G'; img->channelSeq[2] = 'R';
	
	if (copyData)
	{
		img->imageData = malloc(cDataSize);
		memcpy(img->imageData, cData, cDataSize);
	}
	else
	{
		img->imageData = cData;
	}
	img->imageDataOrigin = img->imageData;
	
	img->widthStep = cBytesPerRow; // ???? // size of aligned row in bytes
	img->imageSize = img->height*img->widthStep;
	
	
	if (img->nChannels > 1)
	{
		char * imgData =img->imageData;
		int pixelIndex = 0;
		for (int i=0; i < cWidth * cHeight; i++)
		{
			int iTimes4 = i*4;
			unsigned char r = imgData[iTimes4+1];
			unsigned char g = imgData[iTimes4+2];
			unsigned char b = imgData[iTimes4+3];
			// convert from ARGB to BGR
			//pixelIndex++;
			imgData[pixelIndex++] = b;
			imgData[pixelIndex++] = g;
			imgData[pixelIndex++] = r;
		}
	}
	
	// now our data is the ARGB values of the image
}


void IplImageToCGImage2(IplImage * img, CGImageRef * o_cgImage)
{
	
}

CGImageRef CGCreateImageFromIplImage(IplImage * img)
{
	CGImageRef result = NULL;
	if (!img)
		return NULL;
	
	//char filename[2048];
	//sprintf(filename, "%s/documents/tmpImage.png", NSHomeDirectory())
	//cvSaveImage(filename, img)
	//CGBitmapContextCreate(NULL, img->width, img->height, img->depth * img->nChannels, img->width * img->nChannels * img->depth,
	//					  kCGColorSpaceGenericRGB, kCGImageAlphaLast | kCGBitmapByteOrderDefault);
	
	// FIXME TODO we should really be using CGImageCreate()
	
	int componentSize = img->nChannels * img->depth / 8;
	
	int componentSizeInBits = img->depth;
	int pixelSizeInBits = img->depth * img->nChannels;
	int bytesPerRow = img->width * img->nChannels;
	
	int widthStep = img->width * img->nChannels;
	int bufferSize = widthStep * img->height ;
	unsigned char * imageBuffer = (unsigned char *)malloc(bufferSize);
	int srcChannels = img->nChannels;
	
	
	assert(img->depth == 8);
	
	int rowOffset = 0;
	//CFDataRef data = CFDataCreate(NULL, 
	for (int i =0; i < img->height; i++)
	{
		// this is a memcpy optimized version but it doesn't work right now because RGB values
		// are not the same for both image formats
		//unsigned char * dstRowStart = imageBuffer + rowOffset;
//		unsigned char * srcRowStart = (unsigned char*)img->imageData + i * img->widthStep;
//		memcpy(dstRowStart, srcRowStart, img->width * img->nChannels);
//		rowOffset += widthStep;
//		continue;
		for (int j=0; j < img->width; j++)
		{
			unsigned char * srcPixel = (unsigned char *)img->imageData + img->widthStep * i + j * img->nChannels;
			//CvScalar scalar = cvGet2D(img, i, j);
			
			unsigned char b = srcPixel[0]; //round(scalar.val[0]);
			unsigned char g = srcPixel[1]; //round(scalar.val[1]);
			unsigned char r = srcPixel[2]; //round(scalar.val[2]);
			unsigned char a = 0;//round(scalar.val[3]);
			// byte offset of the component we are currently copying
			int byteOffset = img->nChannels * j + rowOffset;
			
			unsigned char * dstComponent = imageBuffer + byteOffset;
			//char * srcComponent = img->imageData + byteOffset;// + j * n
			
			//memcpy(<#void * #>, <#const void * #>, <#size_t #>)
			if (srcChannels > 1)
			{
				//r = b = g = a = 0;
				//r = 255;
				dstComponent[0] = r;
				dstComponent[1] = g;
				dstComponent[2] = b;
				if (srcChannels == 4)
					dstComponent[3] = a;
			}
			else
				*dstComponent = r; // memcpy(dstComponent, srcComponent, componentSize);
			
		}
		rowOffset += widthStep;
	}
	
	CFDataRef imageData = CFDataCreate(NULL, (unsigned char *)imageBuffer, bufferSize);
	free(imageBuffer);
	//free(imageBuffer);
	
	
	//CGDataProviderRef provider
	CGDataProviderRef provider = CGDataProviderCreateWithCFData(imageData);
	CFRelease(imageData);
	
	const CGFloat * decodeArray = NULL;
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); // kCGColorSpaceGenericRGB
	
	
	//*o_cgImage = CGImageCreate(img->width, img->height, img->depth, img->depth * img->nChannels, widthStep * componentSize,
	result = CGImageCreate(img->width, img->height, componentSizeInBits, pixelSizeInBits, bytesPerRow,
							   colorSpace,
							   kCGBitmapByteOrderDefault, //kCGImageAlphaLast | kCGBitmapByteOrderDefault,
							   provider,
							   decodeArray,
							   true,
							   kCGRenderingIntentDefault);
	
	CGDataProviderRelease(provider);
	CGColorSpaceRelease(colorSpace);
	
	return result;
	
	
	//char outFileName[2064];
	
	//sprintf(outFileName, "%s/documents/tmp.png", NSHomeDirectory());
	
	//cvSaveImage(outFileName, img);
	//CGImageRef cgImg = CGImageCreateWithPNGDataProvider(<#CGDataProviderRef source#>, <#const CGFloat [] decode#>, <#bool shouldInterpolate#>, <#CGColorRenderingIntent intent#>)
	
}

