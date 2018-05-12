#include "stdafx.h"
////
////#include "opencv2/core/core.hpp"
////#include "opencv2/core/cuda.hpp"
////#include "opencv2/calib3d/calib3d.hpp"
////#include <opencv2/highgui/highgui.hpp>
////#include <opencv2/imgproc/imgproc.hpp>
////#include "opencv2/opencv.hpp"
////#include "opencv2/cudastereo.hpp"
////#include <stdio.h>
////#include <string.h>
////#include <iostream>
////
////using namespace cv;
////using namespace std;
////using namespace cuda;
////
////int minDisparity = 0;
////int sadWindowSize = 51;
////int numDisparities = 256;
////int preFilterCap = 1;
////int speckleWindowSize = 0;
////int speckleRange = 0;
////int preFilterSize = 0;
////int disp12MaxDiff = 0;
////int uniquenessRatio = 0;
////int textureThreshold = 0;
////
////Mat cameraMatLeft, cameraMatRight;
////Mat distorionMatLeft, distorionMatRight;
////
////Size imageSize;
////
////Mat makeCanvas(std::vector<Mat>& vecMat, int windowHeight, int nRows);
////
////void Calibrate();
////
////void ChangeTrackbarSADWindowSize(int p, void* data);
////void ChangeTrackbarPreFilterCap(int p, void* data);
////void ChangeTrackbarNumDisparities(int p, void* data);
////
////int main()
////{
////	Mat LeftFrame, RightFrame;
////	Mat depthMap, depthMapNormalize;
////	Mat leftImageGray, rightImageGray;
////	LeftFrame = imread("C:\\Users\\Xandis\\Desktop\\Fotky\\0.png");
////	RightFrame = imread("C:\\Users\\Xandis\\Desktop\\Fotky\\1.png");
////
////	namedWindow("3D", WINDOW_NORMAL);
////
////	createTrackbar("MinDisparity", "3D", &minDisparity, 255, NULL);
////	createTrackbar("SADWindowSize", "3D", &sadWindowSize, 255, ChangeTrackbarSADWindowSize);
////	createTrackbar("NumDisparities", "3D", &numDisparities, 256, ChangeTrackbarNumDisparities);
////	createTrackbar("PreFilterCap", "3D", &preFilterCap, 63, ChangeTrackbarPreFilterCap);
////	createTrackbar("SpeckleWindowSize", "3D", &speckleWindowSize, 255, NULL);
////	createTrackbar("PreFilterSize", "3D", &preFilterSize, 255, NULL);
////	createTrackbar("SpeckleRange", "3D", &speckleRange, 255, NULL);
////	createTrackbar("Disp12MaxDiff", "3D", &disp12MaxDiff, 255, NULL);
////	createTrackbar("UniquenessRatio", "3D", &uniquenessRatio, 255, NULL);
////	createTrackbar("TextureThreshold", "3D", &textureThreshold, 255, NULL);
////
////	/*Calibrate();
////
////
////	VideoCapture cap0(0);
////	VideoCapture cap1(1);
////
////	cap0.set(CV_CAP_PROP_FPS, 30);
////	cap1.set(CV_CAP_PROP_FPS, 30);
////
////	cap0.set(CV_CAP_PROP_FRAME_WIDTH, imageSize.width);
////	cap0.set(CV_CAP_PROP_FRAME_HEIGHT, imageSize.height);
////
////	cap1.set(CV_CAP_PROP_FRAME_WIDTH, imageSize.width);
////	cap1.set(CV_CAP_PROP_FRAME_HEIGHT, imageSize.height);
////	*/
////	Ptr<cuda::StereoBM> sbm = cuda::createStereoBM();
////	Ptr<cuda::StereoConstantSpaceBP> sbm = cuda::createStereoConstantSpaceBP();
////	Ptr<cuda::StereoBeliefPropagation> sbm = cuda::createStereoBeliefPropagation();
////	sbm->setMsgType(CV_16SC1);
////	Ptr<cv::StereoSGBM> sbm = cv::StereoSGBM::create();
////	sbm->setMode(StereoSGBM::MODE_HH);
////	while (1)
////	{
////		cv::cvtColor(LeftFrame, leftImageGray, CV_BGR2GRAY);
////		cv::cvtColor(RightFrame, rightImageGray, CV_BGR2GRAY);
////
////		sbm->setBlockSize(sadWindowSize);
////		sbm->setSpeckleWindowSize(speckleWindowSize);
////		sbm->setSpeckleRange(speckleRange);
////		sbm->setPreFilterCap(preFilterSize);
////		sbm->setPreFilterCap(preFilterCap);
////		sbm->setNumDisparities(numDisparities);
////		sbm->setMinDisparity(-minDisparity);
////		sbm->setDisp12MaxDiff(disp12MaxDiff);
////		sbm->setUniquenessRatio(uniquenessRatio);
////		sbm->setTextureThreshold(textureThreshold);
////		sbm->compute(leftImageGray, rightImageGray, depthMap);
////		cv::normalize(depthMap, depthMapNormalize, 0.1, 255, CV_MINMAX, CV_8U);
////		Mat Visual= depthMapNormalize;
////		
////
////		cv::resize(Visual, Visual, Size(Visual.cols / 4, Visual.rows / 4)); // to half size or even smaller
////		namedWindow("Display frame", CV_WINDOW_AUTOSIZE);
////		imshow("Display frame", Visual);
////
////
////		waitKey(1);
////		/*GpuMat R1, R2, P1, P2, Q;
////		stereoRectify(cameraMatLeft, distorionMatLeft, cameraMatRight, distorionMatRight, imageSize, Mat(),Mat(), R1, R2, P1, P2, Q);
////		GpuMat Cloud;
////		cv::cuda::reprojectImageTo3D(depthMap, Cloud, Q);*/
////	}
////
////	return(0);
////}
////
////Mat makeCanvas(std::vector<Mat>& vecMat, int windowHeight, int nRows) {
////	int N = vecMat.size();
////	nRows = nRows > N ? N : nRows;
////	int edgeThickness = 10;
////	int imagesPerRow = ceil(double(N) / nRows);
////	int resizeHeight = floor(2.0 * ((floor(double(windowHeight - edgeThickness) / nRows)) / 2.0)) - edgeThickness;
////	int maxRowLength = 0;
////
////	std::vector<int> resizeWidth;
////	for (int i = 0; i < N;) {
////		int thisRowLen = 0;
////		for (int k = 0; k < imagesPerRow; k++) {
////			double aspectRatio = double(vecMat[i].cols) / vecMat[i].rows;
////			int temp = int(ceil(resizeHeight * aspectRatio));
////			resizeWidth.push_back(temp);
////			thisRowLen += temp;
////			if (++i == N) break;
////		}
////		if ((thisRowLen + edgeThickness * (imagesPerRow + 1)) > maxRowLength) {
////			maxRowLength = thisRowLen + edgeThickness * (imagesPerRow + 1);
////		}
////	}
////	int windowWidth = maxRowLength;
////	Mat canvasImage(windowHeight, windowWidth, CV_8UC3, Scalar(0, 0, 0));
////
////	for (int k = 0, i = 0; i < nRows; i++) {
////		int y = i * resizeHeight + (i + 1) * edgeThickness;
////		int x_end = edgeThickness;
////		for (int j = 0; j < imagesPerRow && k < N; k++, j++) {
////			int x = x_end;
////			cv::Rect roi(x, y, resizeWidth[k], resizeHeight);
////			cv::Size s = canvasImage(roi).size();
////			 change the number of channels to three
////			Mat target_ROI(s, CV_8UC3);
////			if (vecMat[k].channels() != canvasImage.channels()) {
////				if (vecMat[k].channels() == 1) {
////					cv::cvtColor(vecMat[k], target_ROI, CV_GRAY2RGB);
////				}
////			}
////			else {
////				vecMat[k].copyTo(target_ROI);
////			}
////			cv::resize(target_ROI, target_ROI, s);
////			if (target_ROI.type() != canvasImage.type()) {
////				target_ROI.convertTo(target_ROI, canvasImage.type());
////			}
////			target_ROI.copyTo(canvasImage(roi));
////			x_end += resizeWidth[k] + edgeThickness;
////		}
////	}
////	return canvasImage;
////}
////

////}
////
////void Calibrate() {
////	Mat LeftMap1, LeftMap2;
////	Mat RightMap1, RightMap2;
////
////	FileStorage fsCam("C:\\Users\\Xandis\\source\\repos\\3D\\Calibration\\out_camera_data0.xml", FileStorage::READ);
////	distorionMatLeft = fsCam["distortion_coefficients"].mat();
////	cameraMatLeft = fsCam["camera_matrix"].mat();
////	imageSize.width = fsCam["image_width"];
////	imageSize.height = fsCam["image_height"];
////
////	initUndistortRectifyMap(
////		cameraMatLeft, distorionMatLeft, Mat(),
////		getOptimalNewCameraMatrix(cameraMatLeft, distorionMatLeft, imageSize, 1, imageSize, 0), imageSize,
////		CV_32FC1, LeftMap1, LeftMap2);
////	LeftGpuMap1.upload(LeftMap1);
////	LeftGpuMap2.upload(LeftMap2);
////
////	FileStorage fsCam2("C:\\Users\\Xandis\\source\\repos\\3D\\Calibration\\out_camera_data1.xml", FileStorage::READ);
////	distorionMatRight = fsCam2["distortion_coefficients"].mat();
////	cameraMatRight = fsCam2["camera_matrix"].mat();
////
////	initUndistortRectifyMap(
////		cameraMatRight, distorionMatRight, Mat(),
////		getOptimalNewCameraMatrix(cameraMatRight, distorionMatRight, imageSize, 1, imageSize, 0), imageSize,
////		CV_32FC1, RightMap1, RightMap2);
////	RightGpuMap1.upload(RightMap1);
////	RightGpuMap2.upload(RightMap2);
////
////}