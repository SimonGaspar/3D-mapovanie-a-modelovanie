#include "stdafx.h"

#include "opencv2/core/core.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include "opencv2/cudastereo.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <opencv2/viz.hpp>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <thread>

#include "SavePhoto.h"
#include "Calibration.h" 
#include "SerialPort.h"

using namespace cv;
using namespace std;
using namespace cuda;

void ChangeTrackbarSADWindowSize(int p, void* data);
void ChangeTrackbarPreFilterCap(int p, void* data);
void ChangeTrackbarNumDisparities(int p, void* data);
void Serial(void);

int SadWindowSize = 9,
NumDisparities = 16,
PreFilterCap = 0,
PreFilterType = 0,
TextureThreshold = 0;

int main(int argc, char** argv)
{
	Mat RemapMat[2][2];
	int CreatePointCloud = 0,
		SaveDepthMap = 0,
		WindowsSize = 1;
	string	DepthMapPath,
		DepthMapPhotoPath,
		PointCloudPath,
		IMUPath,
		ConfigPath = argc > 1 ? argv[1] : "Config.xml";
	Size ImageSize;

	FileStorage fs(ConfigPath, FileStorage::READ); // Read the settings
	if (!fs.isOpened())
	{
		cout << "Could not open the configuration file: \"" << ConfigPath << "\"" << endl;
		return -1;
	}

	SaveDepthMap = fs["SaveDepthMap"];
	DepthMapPhotoPath = fs["DepthMapPhotoPath"];
	DepthMapPath = fs["DepthMapPath"];
	CreatePointCloud = fs["CreatePointCloud"];
	PointCloudPath = fs["PointCloudPath"];
	WindowsSize = fs["WindowsSize"];

	int CameraLeftId = 0,
		CameraRightId = 1,
		CameraHeight = 360,
		CameraWidth = 640,
		FPS = 30;

	CameraLeftId = fs["CameraLeftId"];
	CameraRightId = fs["CameraRightId"];
	CameraHeight = fs["CameraHeight"];
	CameraWidth = fs["CameraWidth"];
	FPS = fs["FPS"];

	VideoCapture cap0(CameraLeftId);
	VideoCapture cap1(CameraRightId);

	ImageSize.width = CameraWidth;
	ImageSize.height = CameraHeight;

	cap0.set(CV_CAP_PROP_FPS, FPS);
	cap1.set(CV_CAP_PROP_FPS, FPS);

	cap0.set(CV_CAP_PROP_FRAME_WIDTH, ImageSize.width);
	cap0.set(CV_CAP_PROP_FRAME_HEIGHT, ImageSize.height);

	cap1.set(CV_CAP_PROP_FRAME_WIDTH, ImageSize.width);
	cap1.set(CV_CAP_PROP_FRAME_HEIGHT, ImageSize.height);

	int CreatePhotoDataSet = 0;
	CreatePhotoDataSet = fs["CreatePhotoForCalibration"];
	if (CreatePhotoDataSet == 1) {
		string PhotoPath, StereoPath;
		int PhotoPair = 25,
			WindowsSize = 1;

		PhotoPair = fs["PhotoPairCount"];
		PhotoPath = fs["PhotoPath"];
		StereoPath = fs["StereoPath"];
		WindowsSize = fs["WindowsSize"];

		SavePhoto(cap0, cap1, StereoPath, PhotoPath, PhotoPair, WindowsSize);
	}

	int Calibrate = 0;
	Calibrate = fs["Calibrate"];
	if (Calibrate == 1) {
		string StereoCalibrationXml;
		int BoardHeight = 6,
			BoardWidth = 9,
			SquareSize = 25;

		StereoCalibrationXml = fs["StereoPath"];
		StereoCalibrationXml += "stereo_calib.xml";
		BoardHeight = fs["BoardHeight"];
		BoardWidth = fs["BoardWidth"];
		SquareSize = fs["SquareSize"];

		RunStereoCalibrate(StereoCalibrationXml, BoardWidth, BoardHeight, (float)SquareSize);
	}

	string IntrinsicsPath, ExtrinsicsPath;
	IntrinsicsPath = fs["Intrinsics"];
	ExtrinsicsPath = fs["Extrinsics"];

	FileStorage Intrinsics(IntrinsicsPath, FileStorage::READ);
	if (!Intrinsics.isOpened())
	{
		cout << "Could not open the configuration file: \"" << IntrinsicsPath << "\"" << endl;
		return -1;
	}

	FileStorage Extrinsics(ExtrinsicsPath, FileStorage::READ);
	if (!Extrinsics.isOpened())
	{
		cout << "Could not open the configuration file: \"" << ExtrinsicsPath << "\"" << endl;
		return -1;
	}

	Mat Q = Extrinsics["Q"].mat();

	Mat CameraFrame[2];
	GpuMat	Left,
		Right,
		VisualizateMap,
		ImageGray[2],
		DepthMap,
		DepthMapNormalize;

	namedWindow("3D", WINDOW_NORMAL);

	createTrackbar("SADWindowSize", "3D", &SadWindowSize, 51, ChangeTrackbarSADWindowSize);
	createTrackbar("NumDisparities", "3D", &NumDisparities, 256, ChangeTrackbarNumDisparities);
	createTrackbar("PreFilterCap", "3D", &PreFilterCap, 63, ChangeTrackbarPreFilterCap);
	createTrackbar("PreFilterType", "3D", &PreFilterType, 255, ChangeTrackbarPreFilterCap);
	createTrackbar("TextureThreshold", "3D", &TextureThreshold, 255, NULL);


	initUndistortRectifyMap(Intrinsics["M1"].mat(), Intrinsics["D1"].mat(), Extrinsics["R1"].mat(), Extrinsics["P1"].mat(), ImageSize, CV_16SC2, RemapMat[0][0], RemapMat[0][1]);
	initUndistortRectifyMap(Intrinsics["M2"].mat(), Intrinsics["D2"].mat(), Extrinsics["R2"].mat(), Extrinsics["P2"].mat(), ImageSize, CV_16SC2, RemapMat[1][0], RemapMat[1][1]);

	Ptr<cuda::StereoBM> sbm = cuda::createStereoBM();
	__int64 Now;
	setImuPath(fs["IMUPath"]);
	setPort(fs["IMUPort"]);
	thread IMU(Serial);
	while (1)
	{
		cap0.grab();
		cap1.grab();
		cap0.retrieve(CameraFrame[0]);
		cap1.retrieve(CameraFrame[1]);
		Now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		cv::remap(CameraFrame[0], CameraFrame[0], RemapMat[0][0], RemapMat[0][1], INTER_LINEAR);
		cv::remap(CameraFrame[1], CameraFrame[1], RemapMat[1][0], RemapMat[1][1], INTER_LINEAR);

		Left.upload(CameraFrame[0]);
		Right.upload(CameraFrame[1]);

		cuda::cvtColor(Left, ImageGray[0], CV_BGR2GRAY);
		cuda::cvtColor(Right, ImageGray[1], CV_BGR2GRAY);

		sbm->setBlockSize(SadWindowSize);
		sbm->setNumDisparities(NumDisparities);
		sbm->setPreFilterCap(PreFilterCap);
		sbm->setPreFilterType(PreFilterType);
		sbm->setTextureThreshold(TextureThreshold);

		sbm->compute(ImageGray[0], ImageGray[1], DepthMap);
		cuda::normalize(DepthMap, DepthMapNormalize, 0, 255, CV_MINMAX, CV_8U);
		cuda::drawColorDisp(DepthMap, VisualizateMap, NumDisparities);

		Mat Visual(VisualizateMap),
			LeftSmall(VisualizateMap),
			Color(VisualizateMap),
			RightSmall(Right);


		cv::resize(Visual, Visual, Size(Visual.cols / 2, Visual.rows / 2)); // to half size or even smaller
		cv::resize(CameraFrame[0], CameraFrame[0], Size(CameraFrame[0].cols / WindowsSize, CameraFrame[0].rows / WindowsSize)); // to half size or even smaller
		cv::resize(CameraFrame[1], CameraFrame[1], Size(CameraFrame[1].cols / WindowsSize, CameraFrame[1].rows / WindowsSize));
		namedWindow("Display frame", CV_WINDOW_AUTOSIZE);
		imshow("Display frame", Visual);
		namedWindow("Display LeftFrame", CV_WINDOW_AUTOSIZE);
		imshow("Display LeftFrame", CameraFrame[0]);
		namedWindow("Display RightFrame", CV_WINDOW_AUTOSIZE);
		imshow("Display RightFrame", CameraFrame[1]);
		waitKey(1);

		if (SaveDepthMap == 1) {
			FileStorage fs(DepthMapPath, FileStorage::WRITE);
			if (fs.isOpened())
			{
				fs << "DepthMap" << Mat(DepthMap);
				fs << "Time" << std::to_string(Now);
				imwrite(DepthMapPhotoPath, Color);
				fs.release();
			}
		}

		if (CreatePointCloud == 1) {
			Mat PointCloud(ImageSize, CV_32FC4);
			cv::reprojectImageTo3D(DepthMap, PointCloud, Q, false);
			cv::viz::writeCloud(PointCloudPath, PointCloud);
		}
	}
	fs.release();
	Intrinsics.release();
	Extrinsics.release();

	return(0);
}




void ChangeTrackbarSADWindowSize(int p, void* data) {
	int TEMP = SadWindowSize;
	if (TEMP <= 255 && TEMP >= 5) {
		SadWindowSize = TEMP % 2 == 0 ? TEMP + 1 : TEMP;
	}
	else {
		SadWindowSize = TEMP > 255 ? 255 : 5;
	}
}

void ChangeTrackbarNumDisparities(int p, void* data) {
	int TEMP = NumDisparities;
	if (TEMP < 16) {
		NumDisparities = 16;
	}
	else {
		NumDisparities = TEMP % 16 == 0 ? TEMP : TEMP - TEMP % 16;
	}
}

void ChangeTrackbarPreFilterCap(int p, void* data) {
	int TEMP = PreFilterCap;
	PreFilterCap = TEMP > 63 ? TEMP : 63;
	TEMP = PreFilterCap;
	PreFilterCap = TEMP < 1 ? 1 : TEMP;
}