/*
 Copyright 2017 BIG VISION LLC ALL RIGHTS RESERVED
 
 This program is distributed WITHOUT ANY WARRANTY to the
 Plus and Premium membership students of the online course
 titled "Computer Visionfor Faces" by Satya Mallick for
 personal non-commercial use.
 
 Sharing this code is strictly prohibited without written
 permission from Big Vision LLC.
 
 For licensing and other inquiries, please email
 spmallick@bigvisionllc.com
 
 */

#ifndef FaceBlendCommon_HPP_
#define FaceBlendCommon_HPP_

#include "MainHeader.hpp"


#ifndef M_PI
  #define M_PI 3.14159
#endif


using namespace dlib;

Mat getCroppedFaceRegion(Mat image, std::vector<Point2f> landmarks, cv::Rect &selectedRegion);
void readLabelNameMap(const string& filename, std::vector<string>& names, std::vector<int>& labels, std::map<int, string>& labelNameMap, char separator = ';');
// read descriptors saved on disk
void readDescriptors(const string& filename, std::vector<string>& faceLabels, std::vector<matrix<float,0,1>>& faceDescriptors, char separator = ';');

// find nearest face descriptor from vector of enrolled faceDescriptor
// to a query face descriptor
void nearestNeighbor(dlib::matrix<float, 0, 1>& faceDescriptorQuery,
                     std::vector<dlib::matrix<float, 0, 1>>& faceDescriptors,
                     std::vector<string>& faceLabels, string &label, float& minDistance);
void filterFiles(string dirPath, std::vector<string>& fileNames, std::vector<string>& filteredFilePaths, std::vector<string> extensions);
void listdir(string dirName, std::vector<string>& folderNames, std::vector<string>& fileNames, std::vector<string>& symlinkNames);

bool isRootDir(string rootDir);
bool is_dir(const char* path);
bool is_file(const char* path);

#endif // BIGVISION_faceBlendCommon_HPP_
