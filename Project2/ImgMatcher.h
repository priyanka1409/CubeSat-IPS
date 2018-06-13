#pragma once
#include <stdio.h>
#include <iostream>
#include <algorithm> 
#include <vector>
#include <ctime>
#include <string.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/flann.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

public class ImgMatcher {
private:
	cv::Mat img_object;
	cv::Mat img_scene;
	std::string ogFilePath;
	std::string outputPath;
	std::string inputPath;
	std::string fileName;
		
public:
	ImgMatcher() {};
	void setObjMat(std::string source);
	void setScMat(std::string geo);
	void setOutPath(std::string output);
	void setFileName(std::string fName);
	std::string getInPath();
	std::string getOutPath();
	std::string getFileName();
	std::string getOgFilePath();
	void matchImgs();
	void passCoordinates(std::string outputPath, const char* inputPath, std::string fileName, std::vector<cv::Point2f>);
};
