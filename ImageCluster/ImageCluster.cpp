//
//  ImageCluster.cpp
//  ImageCluster
//
//  Created by Durgesh Trivedi on 11/09/17.
//  Copyright © 2017 durgesh. All rights reserved.
//

#include "ImageCluster.hpp"

string rootDirPath  = "";
string resultDirPath = "";
string facesDirPath = "";
string testDataDir = "";
void imageCluster () {
    int index;
    cout << "You have mutiple options to chose from." << endl << endl;
    cout << "1. You want to Segregate all your images based on faces." << endl <<endl;;
    
    cout << "2. If you want to segregate only images with particular person init from a group of images\n"
    <<"   then you need to provide 1 or 2 images of that person in a folder with the name you would like  \n"
    <<"   to have so that system can learn which images only you want, you can also keep mutiple folder\n"
    <<"   with different person in it to get all the images of those person only in those folder."<< endl;
    cout << "3. 0 to exit the app." << endl << endl;
    cout << "Enter your option :";
    std::cin.clear();
    cin >> index;
    while(std::cin.fail()) {
        cout << "Input the correct value it should be integer only." << endl;
        std::cin.clear();
        std::cin.ignore(256,'\n');
        imageCluster ();
    }
    switch (index) {
        case 0: exit(0);
            break;
        case 1 :
             allImageDirector(OPTION_1_CLUSTER_ALL_FACES);
            break;
        case 2 :
            userFolderDir();
            break;

        default :
            cout << "Input the correct value" << endl;
            imageCluster ();
            break;
    }
}

void allImageDirector(OPTIONS option) {
      readRootDir() == false
                            ? allImageDirector(option)
                            :clusterFaces(option);

}

bool readRootDir() {
    cout << "Please enter the path for your images dir. It should be like   ../faces/images \n";
    cout << "Means your path suppose to end with structure /faces/images  other wise it will not work. This is a limitation for right now." << endl;
    cout << "Drag and Drop is not working for dir path so you need to type or copy paste it, its a limitation right now" << endl << endl;
    cout << "Type 'exit' for path to exit the app" << endl << endl;
    cout << "Enter the images path :";
    cin.clear();
    rootDirPath = "";
    // getline(cin, rootDirPath);
    cin >> rootDirPath;
    if (rootDirPath == "exit") {
        exit(0);
    }
    return isRootDir(rootDirPath);
}

bool userDir() {
    cout << endl<< endl;
    
    cout << "Enter the path for your folder where you want only specific images based on the folder inside it.\nIt should be like   ../MyFaces/foldername \n";
    cout << "Means your path suppose to end with structure /MyFaces all the images should be inside some folder in\n'MyFaces'folder other wise it will not work.This is a limitation for right now." << endl;
    cout << "Drag and Drop is not working for Dir path so you need to type or copy paste it, its a limitation right now" << endl << endl;
    cout << "Type 'exit' for path to exit the app" << endl << endl;
    cout << "Enter the path MyFaces :";
    cin.clear();
    facesDirPath = "";
    // getline(cin, rootDirPath);
    cin >> facesDirPath;
    
    if (facesDirPath == "exit") {
        exit(0);
    }
    return isFacesDir(facesDirPath);
}

void userFolderDir() {
    if (readRootDir() == true) {
        if (userDir() == true) {
            clusterFaces(OPTION_2_READ_FIRST_FOLDERS);
        } else {
            userFolderDir();
        }
    }else {
        userFolderDir();
    }
}
// ----------------------------------------------------------------------------------------

template<typename T>
void printVector(std::vector<T>& vec) {
    for (int i = 0; i < vec.size(); i++) {
        cout << i << " " << vec[i] << "; ";
    }
    cout << endl;
}


void clusterFaces(OPTIONS options) {
    cout << "It will take some time to read the  model keep patience " << endl;
    // Initialize face detector, facial landmarks detector and face recognizer
    string currentDir  = CURRENTDIR ;
    String predictorPath, faceRecognitionModelPath;
    predictorPath =  currentDir + pathlandmarkdetector;
    
    faceRecognitionModelPath =   currentDir + pathRESNETModel;
    frontal_face_detector faceDetector = get_frontal_face_detector();
    shape_predictor landmarkDetector;
    deserialize(predictorPath) >> landmarkDetector;
    anet_type net;
    deserialize(faceRecognitionModelPath) >> net;
    std::vector<string> imagePaths;
    std::vector<string> imageLabels;
    
    if (options == OPTION_2_READ_FIRST_FOLDERS) {
        testDataDir = rootDirPath; // stroe the path for root dir in testDir path which we use for filtering
        rootDirPath = facesDirPath;  // now facedir is rootdir to filter the face based up for enrolling.
    } else if (options == OPTION_2_READ_FIRST_THAN_CLUSTER) {
        rootDirPath = testDataDir;
    }
    readFolder(imagePaths, imageLabels);
    
    char alphabet = 'A';
    // iterate over images
    for (int i = 0; i < imagePaths.size(); i++) {
        string imagePath = imagePaths[i];
        // process training data
        // We will store face descriptors in vector faceDescriptors
        // and their corresponding labels in vector faceLabels
        std::vector<matrix<float,0,1>> faceDescriptors;
        std::vector<string> faceLabels;
        
        cout << "processing: " << imagePath << endl;
        
        // read image using OpenCV
        Mat im = cv::imread(imagePath, cv::IMREAD_COLOR);
        
        // convert image from BGR to RGB
        // because Dlib used RGB format
        Mat imRGB;
        cv::cvtColor(im, imRGB, cv::COLOR_BGR2RGB);
        
        // convert OpenCV image to Dlib's cv_image object, then to Dlib's matrix object
        // Dlib's dnn module doesn't accept Dlib's cv_image template
        dlib::matrix<dlib::rgb_pixel> imDlib(dlib::mat(dlib::cv_image<dlib::rgb_pixel>(imRGB)));
        
        // detect faces in image
        std::vector<dlib::rectangle> faceRects = faceDetector(imDlib);
        // Now process each face we found
        for (int j = 0; j < faceRects.size(); j++) {
            
            // Find facial landmarks for each detected face
            full_object_detection landmarks = landmarkDetector(imDlib, faceRects[j]);
            
            // object to hold preProcessed face rectangle cropped from image
            matrix<rgb_pixel> face_chip;
            
            // original face rectangle is warped to 150x150 patch.
            // Same pre-processing was also performed during training.
            extract_image_chip(imDlib, get_face_chip_details(landmarks, 150, 0.25), face_chip);
            
            // Compute face descriptor using neural network defined in Dlib.
            // It is a 128D vector that describes the face in img identified by shape.
            matrix<float,0,1> faceDescriptorQuery = net(face_chip);
            
            string descriptorDir = options == OPTION_2_READ_FIRST_THAN_CLUSTER ? facesDirPath : rootDirPath;
            // Match the face already in the model
            string label = faceMatch(descriptorDir, faceDescriptorQuery, faceLabels);
            
            switch (options) {
                case OPTION_1_CLUSTER_ALL_FACES:
                    clusterAllFaces(label, imagePath,faceDescriptorQuery,faceDescriptors,faceLabels,alphabet);
                    break;
                case OPTION_2_READ_FIRST_FOLDERS:
                     clusterUserFaces(label, imagePath,faceDescriptorQuery,faceDescriptors,faceLabels);
                    break;
                case OPTION_2_READ_FIRST_THAN_CLUSTER:
                    // This will save all known face and discreptor file should also already created
                    moveSelectedFaces(label, imagePath);
                    break;
                default:
                    break;
            }
        }
    }
    
    if (options == OPTION_2_READ_FIRST_FOLDERS) {
        clusterFaces(OPTION_2_READ_FIRST_THAN_CLUSTER);
    }
    return ;
}

void clusterAllFaces(string label,
                     string imagePath,
                     matrix<float,0,1> &faceDescriptorQuery,
                     std::vector<matrix<float,0,1>> &faceDescriptors,
                     std::vector<string> &faceLabels,
                     char  &alphabet)
{
   
    if (label == CREATE_DESCRIPTOR) {
        // If there is no descriptor file save model for first time
         string alpha(1, alphabet);
        saveDescriptor(imagePath, faceDescriptorQuery, faceDescriptors, faceLabels, alpha);
    }
    // update model with new Face Match
    else if  (label == NEW_FACE) {
        // Change the folder name before saving it as new face match
         alphabet++;
         string alpha(1, alphabet);
        saveDescriptor(imagePath, faceDescriptorQuery, faceDescriptors, faceLabels, alpha);
    }
    else {
        saveFile(label, imagePath);
    }
}


void clusterUserFaces(string label,
                     string imagePath,
                     matrix<float,0,1> &faceDescriptorQuery,
                     std::vector<matrix<float,0,1>> &faceDescriptors,
                     std::vector<string> &faceLabels)
{
    if (label == CREATE_DESCRIPTOR || label == NEW_FACE) {
        std::size_t pos = imagePath.find_last_of("/");
        string  imageDirPath = imagePath.substr(0, pos);
        string labelName = imageDirPath.substr(imageDirPath.find_last_of("/")+1);
        faceLabels.clear();
        faceDescriptors.clear();
        // add face descriptor and label for this face to
        // vectors faceDescriptors and faceLabels
        faceDescriptors.push_back(faceDescriptorQuery);
        // add label for this face to vector containing labels corresponding to
        // vector containing face descriptors
        faceLabels.push_back(labelName);
        
        string resultDir  =  rootDirPath + resultPath;
        // create result dir
        if (is_dir(resultDir.c_str()) == false) {
            string command = "mkdir -p " + resultDir;
            system(command.c_str());
        }
        // save the label and descriptor to the file
        writeDescriptors(faceLabels,faceDescriptors);
    }

}
void moveSelectedFaces(string label, string imagePath) {
    if (label != CREATE_DESCRIPTOR && label != NEW_FACE ) {
        saveFile(label, imagePath);
    }
}

void saveFile(string label, string imagePath) {
    string currentDir  = rootDirPath ;
    std::size_t pos = imagePath.find_last_of("/");
    string value = imagePath.substr(pos);
    string sorceDir = imagePath.substr(0, pos);
    string destDir = currentDir +  resultPath + label;
    copyFile(sorceDir, destDir, value);
}
void saveDescriptor(string imagePath,
                    matrix<float,0,1> &faceDescriptorQuery,
                    std::vector<matrix<float,0,1>> &faceDescriptors,
                    std::vector<string> &faceLabels,
                    string  &alphabet) {
    
    saveFile(alphabet, imagePath);
    
    // if label is -1 clear the faceLabels, faceDescriptors
    faceLabels.clear();
    faceDescriptors.clear();
    // add face descriptor and label for this face to
    // vectors faceDescriptors and faceLabels
    faceDescriptors.push_back(faceDescriptorQuery);
    // add label for this face to vector containing labels corresponding to
    // vector containing face descriptors
    faceLabels.push_back(alphabet);
    
    // save the label and descriptor to the file
    writeDescriptors(faceLabels,faceDescriptors);
}
void writeDescriptors(std::vector<string> &faceLabels, std::vector<matrix<float,0,1>> &faceDescriptors) {
    cout << "number of face descriptors " << faceDescriptors.size() << endl;
    cout << "number of face labels " << faceLabels.size() << endl;
    
    string currentDir = rootDirPath;
    
    
    // write face labels and descriptor to disk
    // each row of file descriptors.csv has:
    // 1st element as face label and
    // rest 128 as descriptor values
    const string descriptorsPath = currentDir + pathDescriptorsCSV;
    
    std::fstream ofs(descriptorsPath, std::ios::out | std::ios::app);
    // write descriptors
    for (int m = 0; m < faceDescriptors.size(); m++) {
        matrix<float,0,1> faceDescriptor = faceDescriptors[m];
        std::vector<float> faceDescriptorVec(faceDescriptor.begin(), faceDescriptor.end());
        // cout << "Label " << faceLabels[m] << endl;
        ofs << faceLabels[m];
        ofs << ";";
        for (int n = 0; n < faceDescriptorVec.size(); n++) {
            ofs << std::fixed << std::setprecision(8) << faceDescriptorVec[n];
            // cout << n << " " << faceDescriptorVec[n] << endl;
            if ( n == (faceDescriptorVec.size() - 1)) {
                ofs << "\n";  // add ; if not the last element of descriptor
            } else {
                ofs << ";";  // add newline character if last element of descriptor
            }
        }
    }
    ofs.close();
    
}

string faceMatch(string descriptorDir, matrix<float,0,1> &faceDescriptorQuery, std::vector<string> &faceLabels) {
    
    //string currentDir = rootDirPath;
    // read descriptors of enrolled faces from file
    const string faceDescriptorFile =  descriptorDir + pathDescriptorsCSV;
    
    if (fileExist(faceDescriptorFile) == false) {
        // The pathDescriptor file doesn't exist means file not yet created and it is first face
        return CREATE_DESCRIPTOR;
    }
    
    std::vector<matrix<float,0,1>> faceDescriptors;
    readDescriptors(faceDescriptorFile, faceLabels, faceDescriptors);
    
    // Find closest face enrolled to face found in frame
    string label;
    float minDistance;
    nearestNeighbor(faceDescriptorQuery, faceDescriptors, faceLabels, label, minDistance);
    return label;
}
void readFolder(std::vector<string> &imagePaths,
                std::vector<string> &imageLabels) {
    
    // Now let's prepare our training data
    // data is organized assuming following structure
    // faces folder has subfolders.
    // each subfolder has images of a person
    string faceDatasetFolder = rootDirPath;
    std::vector<string> subfolders, fileNames, symlinkNames;
    // fileNames and symlinkNames are useless here
    // as we are looking for sub-directories only
    listdir(faceDatasetFolder, subfolders, fileNames, symlinkNames);
    // variable to hold any subfolders within person subFolders
    std::vector<string> folderNames;
    // iterate over all subFolders within faces folder
    for (int i = 0; i < subfolders.size(); i++) {
        // read imagePaths from each person subFolder
        // clear vectors
        folderNames.clear();
        fileNames.clear();
        symlinkNames.clear();
        // folderNames and symlinkNames are useless here
        // as we are only looking for files here
        // read all files present in subFolder
        listdir(subfolders[i], folderNames, fileNames, symlinkNames);
        // filter only jpg files
        std::vector<string> extensions{"jpg","png"};
        filterFiles(subfolders[i], fileNames, imagePaths, extensions);
    }
}
