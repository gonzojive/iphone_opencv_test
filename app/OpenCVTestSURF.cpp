/*
 *  OpenCVTestSURF.cpp
 *  OpenCVTest
 *
 *  Created by Raymond Daly on 11/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <opencv/cv.h>//#include "OpenCVTestSURF.h"

extern "C" {
	
	float fRound(float x) {
		return floor(x + .5);
	}
	//! Draw all the Ipoints in the provided vector
	void drawIpoints(IplImage *img, std::vector<cv::KeyPoint> &ipts, int tailSize)
	{
		cv::KeyPoint *ipt;
		float s, o;
		int r1, c1, r2, c2, resp;
		
		for(unsigned int i = 0; i < ipts.size(); i++) 
		{
			ipt = &ipts.at(i);
			s = ((9.0f/1.2f) * ipt->octave) / 3.0f;
			o = ipt->angle;
			resp = ipt->response;
			r1 = fRound(ipt->pt.y);
			c1 = fRound(ipt->pt.x);
			c2 = fRound(s * cos(o)) + c1;
			r2 = fRound(s * sin(o)) + r1;
			
			if (o) // Green line indicates orientation
				cvLine(img, cvPoint(c1, r1), cvPoint(c2, r2), cvScalar(0, 255, 0));
			else  // Green dot if using upright version
				cvCircle(img, cvPoint(c1,r1), 1, cvScalar(0, 255, 0),-1);
			
			if (resp >= 0)
			{ // Blue circles indicate light blobs on dark backgrounds
				cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(255, 0, 0),1);
			}
			else
			{ // Red circles indicate light blobs on dark backgrounds
				cvCircle(img, cvPoint(c1,r1), fRound(s), cvScalar(0, 0, 255),1);
			}
			
			// Draw motion from ipoint dx and dy
			if (tailSize)
			{
				//cvLine(img, cvPoint(c1,r1),
//					   cvPoint(int(c1+ipt->dx*tailSize), int(r1+ipt->dy*tailSize)),
//					   cvScalar(255,255,255), 1);
			}
		}
	}
	
	void detect_and_draw_surf ( IplImage* img)//, CvScalar color )
	{
		std::vector<cv::KeyPoint> keypoints;
		std::vector<float> descriptors;
		cv::SURF surfer = cv::SURF();
		cv::Mat mat = cv::Mat(img, false);
		printf("Mat is %d x %d\n", mat.rows, mat.cols);
		//mat = cv::Mat(mat, cv::Rect(100, 100, 100, 100));
		cv::Mat matgray = cv::Mat();
		cv::cvtColor(mat, matgray,CV_RGB2GRAY);
		
		cv::Mat mask = cv::Mat::ones(mat.rows, mat.cols, cv::DataType<unsigned char>::type);
		surfer(matgray, mask, keypoints, descriptors, false);
		
		drawIpoints(img, keypoints, 0);
		
		printf("Detected %i surf points", keypoints.size());
	}

}