#pragma once
#include <stdio.h>
#include <iostream>
#include <algorithm> 
#include <vector>
#include <ctime>
#include <string.h>
#include "cpl_string.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"

public class GDALImpl {
private:

public:
	GDALImpl() {};
	std::string changeSlashes(std::string text);
	void transformCoords(std::string outputPath, const char* inputPath, std::string fileName, double *xMin, double *xMax, double *yMin, double *yMax);
	void transformCoords2(std::string outputPath, const char* inputPath, std::string ogFilePath, std::string fileName, int width, int height, cv::Point2f topLeft, cv::Point2f topRight, cv::Point2f bottomLeft, cv::Point2f bottomRight, cv::Point2f center, std::clock_t start);
	void assignProj(const char* outFilePath, const char* projection);
};