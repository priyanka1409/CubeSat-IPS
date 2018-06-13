#include <msclr\marshal_cppstd.h>
#include <iostream>
#include <ctime>
#include "CmdImpl.h"
#include "cpl_conv.h"
#include "cpl_string.h"
#include <string.h>
#include <opencv2/core.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

	//using namespace std;
	//using namespace cv;
	using namespace msclr::interop;
	using namespace System;
	using namespace System::Configuration;
	using namespace System::IO;
	using namespace System::Diagnostics;

	void CmdImpl::cmd(CmdImpl^ g, String^ command) {
		g->process->StandardInput->WriteLine(command);
		g->process->StandardInput->Flush();
	}

	String^ CmdImpl::convertStr(std::string str) {
		String^ result;
		result = marshal_as<String^>(str);
		return result;
	}

	void CmdImpl::displayImg(std::string ogFile, std::string outFile) {
		
		const char* output_window = "1: Original Img, 2: Output GeoTiff";
		cv::namedWindow(output_window, cv::WINDOW_AUTOSIZE);

		cv::Mat img_og = cv::imread(ogFile, cv::IMREAD_GRAYSCALE);
		cv::Mat img_out = cv::imread(outFile, cv::IMREAD_GRAYSCALE);

		if (!img_out.data)
		{
			std::cout << " --(!) Error in GeoTiff output creation" << std::endl; return;
		}

		int size = 250;
		int x = img_og.cols; int y = img_og.rows;

		// w - Maximum number of images in a row
		// h - Maximum number of images in a column
		int w = 2; int h = 1;
		// scale - How much we have to resize the image
		float scale;
		int max;

		cv::Mat displayImage = cv::Mat::zeros(cv::Size(100 + size * w, 60 + size * h), CV_8UC1);

		int i = 0; int m = 20; int n = 20;
		
		max = (x > y) ? x : y;
		scale = (float)((float)max / size);

		cv::Mat imageROI = displayImage(cv::Rect(m, n, (int)(x / scale), (int)(y / scale)));
		cv::resize(img_og, imageROI, cv::Size((int)(x / scale), (int)(y / scale)));

		i++; m += (20 + size);
		
		x = img_out.cols; y = img_out.rows;

		max = (x > y) ? x : y;
		scale = (float)((float)max / size);

		if (i%w == 0 && m != 20) {
			m = 20; n += 20 + size;
		}

		imageROI = displayImage(cv::Rect(m, n, (int)(x / scale), (int)(y / scale)));
		cv::resize(img_out, imageROI, cv::Size((int)(x / scale), (int)(y / scale)));

		cv::imshow(output_window, displayImage);

		cv::waitKey(0);

		printf("DISPLAYED");
	}

	void CmdImpl::runCommands(std::string ogFile, std::string outFile, String^ gdal_translate, 
		String^ gdal_warp, std::clock_t start) {

			CmdImpl^ g = gcnew CmdImpl;
			
			String^ gdal_ver = "gdalinfo --version";

			g->process->StartInfo->FileName = "cmd.exe";
			g->process->StartInfo->CreateNoWindow = true;
			g->process->StartInfo->RedirectStandardInput = true;
			g->process->StartInfo->RedirectStandardOutput = true;
			g->process->StartInfo->UseShellExecute = false;
			g->process->Start();
			cmd(g, gdal_ver);
			//cmd(g, image_dir);
			cmd(g, gdal_translate);
			cmd(g, gdal_warp);

			g->process->StandardInput->Close();
			g->process->WaitForExit();
			Console::WriteLine(g->process->StandardOutput->ReadToEnd());

			printTime(start);
			
			displayImg(ogFile, outFile);

			Console::ReadKey();
		}

	void CmdImpl::printTime(std::clock_t start) {
		std::clock_t stop;
		//timer ends
		stop = clock();

		double time = (stop - start) / double(CLOCKS_PER_SEC) ;
		int minutes = time / 60;
		double seconds = time - minutes * 60;

		std::cout << std::endl << "Time taken: " << minutes << " minutes " << seconds << " seconds" << std::endl;
	}

	void CmdImpl::createCommands(std::string outputPath, std::string inputPath, std::string fileName, double topLeftXGeo, double topLeftYGeo, double bottomRightXGeo, double bottomRightYGeo, int EPSG) {
		
		String^ outputVRTPath = convertStr(outputPath) + "\\" + convertStr(fileName) + ".vrt";
		String^ outputFilePath = convertStr(outputPath) + "\\" + convertStr(fileName) + ".tif";
		std::string outFilePath = outputPath + "\\" + fileName + ".tif";
		
		String^ gdal_translate = "gdal_translate -of VRT -projwin " + topLeftXGeo + " " + topLeftYGeo + " " + bottomRightXGeo + " " + bottomRightYGeo + " " + convertStr(inputPath) + " " + outputVRTPath;
		
		String^ gdalwarp = "gdalwarp -wo SOURCE_GRID=YES -wo SOURCE_EXTRA=1000 -t_srs EPSG:" + EPSG
			+ " -r near -dstalpha -co COMPRESS=LZW " + outputVRTPath + " " + outputFilePath;

		////runCommands("", outFilePath, gdal_translate, gdalwarp);
	}

	void CmdImpl::createCommands2(std::string outputPath, std::string ogFilePath, std::string fileName, int width, int height, double topLeftXGeo, double topLeftYGeo,
		double topRightXGeo, double topRightYGeo, double bottomLeftXGeo, double bottomLeftYGeo, double bottomRightXGeo, double bottomRightYGeo, double centerXGeo,
		double centerYGeo, int EPSG, std::clock_t start) {
		
		std::string outFilePath = outputPath + "\\" + fileName + ".tif";
		String^ GCPFilePath = convertStr(outputPath) + "\\" + convertStr(fileName) + "-GCP.vrt";
		String^ outputFilePath = convertStr(outputPath) + "\\" + convertStr(fileName) + ".tif";

		String^ gdal_translate = "gdal_translate -of VRT -gcp 0 0 " + topLeftXGeo + " " + topLeftYGeo +
			" -gcp " + width + " 0 " + topRightXGeo + " " + topRightYGeo + " -gcp 0 " + height +
			" " + bottomLeftXGeo + " " + bottomLeftYGeo + " -gcp " + width + " " + height + " " +
			bottomRightXGeo + " " + bottomRightYGeo + " -gcp " + width/2 + " " + height/2 + " " + 
			centerXGeo + " " + centerYGeo + " \"" + convertStr(ogFilePath) + "\" \"" + 
			GCPFilePath + "\"";

		String^ gdalwarp = "gdalwarp -wo SOURCE_GRID=YES -wo SOURCE_EXTRA=1000 -r bilinear -dstalpha -co COMPRESS=LZW -t_srs EPSG:" +
			EPSG + " \"" + GCPFilePath + "\" \"" + outputFilePath + "\"";

		runCommands(ogFilePath, outFilePath, gdal_translate, gdalwarp, start);
	}