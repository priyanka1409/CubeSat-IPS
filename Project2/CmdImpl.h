#pragma once
#include <string.h>
#include <ctime>
#include "cpl_string.h"
using namespace System;
using namespace System::Diagnostics;

public ref class CmdImpl {
	public:
		static Process ^process = gcnew Process();
		int num = 1;
		void createCommands(std::string outputPath, std::string inputPath, std::string fileName, 
			double topLeftXGeo, double topLeftYGeo, double bottomRightXGeo, double bottomRightYGeo, int EPSG);
		void createCommands2(std::string outputPath, std::string ogFilePath, std::string fileName, 
			int width, int height, double topLeftXGeo, double topLeftYGeo, double topRightXGeo, 
			double topRightYGeo, double bottomLeftXGeo, double bottomLeftYGeo, double bottomRightXGeo,
			double bottomRightYGeo, double centerXGeo, double centerYGeo, int EPSG, std::clock_t start);
		String^ convertStr(std::string str);
		void runCommands(std::string ogFile, std::string outFile, String^ gdal_translate, 
			String^ gdal_warp, std::clock_t start);
		void cmd(CmdImpl^ g, String^ command);
		void displayImg(std::string ogFile, std::string outFile);
		void printTime(std::clock_t start);
};