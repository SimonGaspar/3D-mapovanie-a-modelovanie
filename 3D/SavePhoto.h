#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace cuda;

void SavePhoto(VideoCapture cap0, VideoCapture cap1, string StereoPath="", string PhotoPath ="",int MaxPhotoPair=25, int WindowsSize=1) {
	int Key = 0;
	int Counter = 0;
	for (;;)
	{
		bool IsValid = true;

		cv::Mat FrameLeft;
		cv::Mat FrameRight;
		cv::Mat FrameLeftChanged;
		cv::Mat FrameRightChanged;

		try
		{
			cap0.grab();
			cap1.grab();
			cap0.retrieve(FrameLeft);
			cap1.retrieve(FrameRight);
		}
		catch (cv::Exception& e)
		{
			std::cout << "an exception occurred. ignoring frame. " << e.err << std::endl;
			IsValid = false;
		}

		if (IsValid)
		{
			try
			{
				//capture frames and save them
				while (Key != 27 && Counter < MaxPhotoPair)
				{
					cap0.grab();
					cap1.grab();
					cap0.retrieve(FrameLeft);
					cap1.retrieve(FrameRight);

					//velkost okien do XML
					cv::resize(FrameLeft, FrameLeftChanged, Size(FrameLeft.cols / WindowsSize, FrameLeft.rows / WindowsSize)); // to half size or even smaller
					cv::resize(FrameRight, FrameRightChanged, Size(FrameRight.cols / WindowsSize, FrameRight.rows / WindowsSize));
					cv::imshow("LeftFrame", FrameLeftChanged);
					cv::imshow("RightFrame", FrameRightChanged);

					if (Key == 99)
					{

						//XML cesta na ulozenie fotiek
						string l = "l.bmp";
						string r = "r.bmp";
						imwrite(PhotoPath+to_string(Counter) + l, FrameLeft);
						imwrite(PhotoPath+to_string(Counter)+ r, FrameRight);
						Counter++;
					}
					Key = waitKey(70);
				}
			}
			catch (cv::Exception& e)
			{
				std::cout << "an exception occurred. ignoring frame. " << e.err << std::endl;
			}
		}
		break;
	}

	destroyAllWindows();

	ofstream StereoFile;
	StereoFile.open(StereoPath+"stereo_calib.xml");

	StereoFile << "<?xml version=\"1.0\"?>\n";
	StereoFile << "<opencv_storage>\n";
	StereoFile << "<imagelist>\n";
	for (int x = 0; x < Counter; x++) {
		StereoFile << "\"" << PhotoPath << x << "l.bmp\"\n";
		StereoFile << "\"" << PhotoPath << x << "r.bmp\"\n";
	}
	StereoFile << "</imagelist>\n";
	StereoFile << "</opencv_storage>\n";
	StereoFile.close();
}