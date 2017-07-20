#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>

#define max(x, y) x > y ? x : y
#define min(x, y) x < y ? x : y

using namespace std;
using namespace cv;

std::string statusFileName = "..//Data//Statusinfo.txt";
enum ClickStatus { CENTER_CLICKED, CORNER_CLICKED, NO_TARGET_CLICKED }; // possible click status

// Mouse Callback Data Structure
struct MouseCallbackStatus 
	{
	cv::Point hit_point;
	cv::Point pickUp_point;
	ClickStatus clickStatus;
	bool moveRect;
	bool resize_rect;
	bool resize_rect_right;
	bool resize_rect_top;
	bool resize_rect_bottom;
	MouseCallbackStatus
		(cv::Point _hit_point,
		ClickStatus _clickStatus,
		bool _moveRect = false,
		bool _resize_rect = false,
		bool _resize_rect_right = false,
		bool _resize_rect_top = false,
		bool _resize_rect_bottom = false)
		{
		this->hit_point = _hit_point; 
		this->clickStatus = _clickStatus;
		this->moveRect = _moveRect;
		this->resize_rect = _resize_rect;
		this->pickUp_point = _hit_point;
		this->resize_rect_top = _resize_rect_top;
		this->resize_rect_bottom = _resize_rect_bottom;
		}
	};

// Parameters Callback Data Structure
struct CvCallbackParam
	{
	std::string w1nam;
	std::string w2nam;
	std::string outputFolder;
	std::string templateFilename;
	int savedNegFileIdx, savedPosFileIdx;
	cv::Mat img_src;
	cv::Mat img_display;
	cv::Point refRecPoint;
	cv::Point endRecPoint;
	cv::Rect ROI;
	cv::Scalar color;
	float maxRecSiz;
	bool cap;				//Is this a video or a list of images
	cv::VideoCapture videoCapture;
	int  frame, fileiter;
	string fileName;
	float zoomLevel;
	std::vector <std::string> fileList;
	int numOfNegSampleperFrame;
	CvCallbackParam(string _fileName, int _negIdx = 0, int _posIdx = 0,  int _idx = 0)
		{
		this->fileName = _fileName;
		this->w1nam = "Image Clipping Program Window";
		this->w2nam = "Cropped Image Window";
		this->frame = _idx;
		this->fileiter = _idx;
		this->refRecPoint = cvPoint(0, 0);
		this->endRecPoint = cvPoint(10, 10);
		this->ROI = cv::Rect(this->refRecPoint.x, this->endRecPoint.x, this->refRecPoint.y, this->endRecPoint.y);
		this->color = cvScalar(0, 255, 0);
		this->savedNegFileIdx = _negIdx;
		this->savedPosFileIdx = _posIdx;
		this->templateFilename = "Patch";
		this->outputFolder = "";
		this->maxRecSiz = 10;
		this->zoomLevel = 1;
		this->numOfNegSampleperFrame = 40;
		}
	void updateRecPoint()
		{
		this->refRecPoint.x = this->ROI.x;
		this->endRecPoint.x = this->ROI.x + this->ROI.width;
		this->refRecPoint.y = this->ROI.y;
		this->endRecPoint.y = this->ROI.y + this->ROI.height;
		}
	void updateDisplayImg()
		{
		this->zoomLevel = 1;
		this->img_display = this->img_src;
		}
	};

static MouseCallbackStatus status = MouseCallbackStatus(
	cvPoint(0, 0),		// point0
	NO_TARGET_CLICKED,	// click status
	false,				// move_rect
	false,				// resize_rect_left
	false,				// resize_rect_right
	false,				// resize_rect_top
	false				// resize_rect_bottom
);


// Function prototypes
void hndleKey(CvCallbackParam *param, char key);
void key_callback(CvCallbackParam *param);
void mouse_Callback(int event, int x, int y, int flags, void* paramData);
cv::Mat renderWindows(CvCallbackParam *param);
bool load_reference(CvCallbackParam* param);
ClickStatus checkPoint2Box(CvCallbackParam *param, cv::Point mousePos);
void RandomCapture(CvCallbackParam* param);
bool writeImage(CvCallbackParam *param, bool detectType, cv::Mat img);

/**
//* Main Entry Function
//*/
int main(int argc, char *argv[])
	{
	//// parse arguments
	//gui_usage();
	
	string fileName = "Data//testk12.avi";  // Enter video file name or directory with images 
	CvCallbackParam cvCallbackParam(fileName, 0, 0);
	if (!load_reference(&cvCallbackParam))
		{
		return -1;
		}
	//Create Windows
	cvNamedWindow(cvCallbackParam.w1nam.c_str(), CV_WINDOW_AUTOSIZE);
	cvNamedWindow(cvCallbackParam.w2nam.c_str(), CV_WINDOW_AUTOSIZE);

	// Mouse and Key callback
	cv::setMouseCallback(cvCallbackParam.w1nam.c_str(), mouse_Callback, &cvCallbackParam);
	key_callback(&cvCallbackParam);

	//Release Windows 
	cvDestroyWindow(cvCallbackParam.w1nam.c_str());
	cvDestroyWindow(cvCallbackParam.w2nam.c_str());
	}

/**
//* Initialise Reference Parameters
//*/
bool load_reference(CvCallbackParam* param)
	{
	//Initialize prarameters 
	bool is_directory = false;
	bool is_image = false;
	bool is_video = false;
	//~ DWORD dwAttrib = GetFileAttributes(param->fileName.c_str());
//~ 
	//~ // Attempt to load last writing position
	//~ std::ifstream myfile(statusFileName);
	//~ 
	//~ if (myfile.is_open())
		//~ {
		//~ 
		//~ try
			//~ {
			//~ std::string line;
			//~ std::vector<std::string> allLines;
			//~ while (std::getline(myfile, line))
				//~ {
				//~ 
				//~ allLines.push_back(line);
				//~ }
			//~ param->outputFolder = allLines.at(0);
			//~ param->savedNegFileIdx = atoi(allLines.at(1).c_str());
			//~ param->savedPosFileIdx = atoi(allLines.at(2).c_str());
			//~ }
		//~ catch (exception e)
			//~ {
//~ 
			//~ }
		//~ myfile.close();
		//~ }


	//Load picture file(s) to be clipped
	//~ if (dwAttrib == -1)      // Check if is valid file directory
		//~ {
		//~ cerr << "Invalid file path!";
		//~ return false;
		//~ }	
	//~ else if(dwAttrib == FILE_ATTRIBUTE_DIRECTORY)     // Check if is folder directory
		//~ {
		//~ is_directory = true;
		//~ param->cap = false;
		//~ cout << "Now loading files in directory..... " << endl; 
		//~ WIN32_FIND_DATA ffd;
		//~ HANDLE hFind = INVALID_HANDLE_VALUE;
		//~ hFind = FindFirstFile(param->fileName.c_str(), &ffd);
		//~ if (INVALID_HANDLE_VALUE == hFind)
			//~ {
			//~ cerr << "Invalid file path! Unable to iterate files";
			//~ return false;
			//~ }
		//~ // populate vector  with file name of all file in the given folder
		//~ do
			//~ {
			//~ //Ignore sub-folders
			//~ if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				//~ {
				//~ cout << "Sub-folder detected. Ignoring ..." << endl;
				//~ }
			//~ else
				//~ {
				//~ //check if file is a readable image
				//~ if(cv::imread(ffd.cFileName).data) 
					//~ param->fileList.push_back(ffd.cFileName);
				//~ }
			//~ } 
		//~ while (FindNextFile(hFind, &ffd) != 0);
//~ 
		//~ if (param->fileiter < param->fileList.size())
			//~ {
			//~ param->img_src = cv::imread(param->fileList.at(param->fileiter));
			//~ param->updateDisplayImg();
			//~ param->ROI.x = param->img_display.size().width / 2 - 20;
			//~ param->ROI.y = param->img_display.size().height / 2 - 20;
			//~ param->ROI.width = 40;
			//~ param->ROI.height = 40;
//~ 
//~ 
			//~ }
		//~ else
			//~ {
			//~ cerr << "Image index position out of range!" << endl;
			//~ return false;
			//~ }
		//~ }
	//~ else    // Assume its a video
		//~ {
		is_video = true;
		param->cap = true;
		cout << "Now reading video..... " << param->fileName<< endl;
		param->videoCapture = cv::VideoCapture(param->fileName);
		//param->videoCapture.set(CV_CAP_PROP_POS_FRAMES, (double)param->frame);
		if (!param->videoCapture.read(param->img_src))
			{
			cerr << "Image index position out of range!";
			return false;
			}
		param->updateDisplayImg();
		param->ROI.x = param->img_display.size().width / 2 - 20;
		param->ROI.y = param->img_display.size().height / 2 - 20;
		param->ROI.width = 40;
		param->ROI.height = 40;
		//~ }




	return true;
	}



/**
//* Key Callback Function
//*/
void key_callback(CvCallbackParam *param)
	{
	string filename;
	 renderWindows(param);


	while (true) // key callback
		{
		
		char key = cvWaitKey(0);

		if (key == '+') 
			{
			//Zoom into the Image
			}
		if (key == '-') 
			{
			//Zoom out of the Immage 
			} 
		if (key == 'r')
			{
			param->savedNegFileIdx--;
			param->savedNegFileIdx = max(param->savedNegFileIdx, 0);
			param->savedPosFileIdx--;
			param->savedPosFileIdx = max(param->savedPosFileIdx, 0);
			}
	
		// Save
		// Press 's' to save positive instances
		if (key == 's') 
			{
			if (param->ROI.width > 0 && param->ROI.height > 0)
				{
				cv::Mat croppedIm = renderWindows(param);
				std::cout << "Saving..." << endl;
				writeImage(param, true, croppedIm);
				}
			else  std::cout << "Invalid Bounding Box!" << endl;
			}
		//Press SHIFT + 's' to save negative instance
		if (key == 'S')
			{
			RandomCapture(param);
			}

		// Forward
		//Press 'f' to move forward to the next frame
		if (key == 'f' ) // 32 is SPACE
			{
			if (param->cap)
				{
				if (param->videoCapture.read(param->img_src))
					{
					param->frame++;
					param->updateDisplayImg();
					cout << "Now showing " << param->fileName << " @ Frame: " << param->frame << endl;
					}
				}
			else
				{
				if (param->fileiter + 1 != param->fileList.size())
					{
					param->fileiter++;
					filename = param->fileList.at(param->fileiter);
					param->img_src = cv::imread(filename);
					param->updateDisplayImg();
					cout << "Now showing " << filename << endl;
					}
				}
			}

		// Backward
		//Press 'b' to move backwards to the previous frame
		else if (key == 'b')
			{
			if (param->cap)
				{	

				param->frame = max(1, param->frame - 1);
				param->videoCapture.set(CV_CAP_PROP_POS_FRAMES, param->frame - 1);
				if (param->videoCapture.read(param->img_src))
					{
					param->frame--;
					param->updateDisplayImg();
					cout << "Now showing " << param->fileName << " @ Frame: " << param->frame << endl;
					}
				}
			else
				{
				if (param->fileiter != 0)
					{
					param->fileiter--;
					filename = param->fileList.at(param->fileiter);
					param->img_src = cv::imread(filename);
					param->updateDisplayImg();
					cout << "Now showing " << filename << endl;
					}
				}
			}
		
		// Exit
		else if (key == 'q' || key == 27) // 27 is ESC
			{
			//~ std::ofstream myfile;
			//~ if (!param->outputFolder.empty())
				//~ {
				//~ myfile.open(statusFileName);
				//~ myfile << param->outputFolder << endl;
				//~ myfile << param->savedNegFileIdx << endl;
				//~ myfile << param->savedPosFileIdx << endl;
				//~ myfile.close();
				//~ }
			
			break;
			}
		hndleKey(param, key);
		
		}
	}

/**
//* Randomly sample for non-face region
//*/
void RandomCapture(CvCallbackParam* param)
	{
	int  M = 200, minWinSize = 70;
	for (int i = 0; i < param->numOfNegSampleperFrame; i++)
		{
		cv::Point tl(std::rand() % (param->img_src.size().width - M), std::rand() % (param->img_src.size().height - M));
		
		cv::Point siz(std::rand() % (M - minWinSize - 1) + minWinSize, std::rand() % (M - minWinSize - 1) + minWinSize);
		
		cv::Rect roi(tl.x, tl.y, siz.x, siz.y);
		cv::Mat imgBk = param->img_src(roi);

		
		if ((abs(roi.x - param->ROI.x) * 2 < (roi.width + param->ROI.width)) &&
			(abs(roi.y - param->ROI.y) * 2 < (roi.height + param->ROI.height)))
			continue;
		else
			writeImage(param, false, imgBk); // Write negative capture
		

		}
	}



/**
//* cvSetMouseCallback function
//*/
void mouse_Callback(int event, int x, int y, int flags, void* paramData)
	{
	CvCallbackParam *param = (CvCallbackParam *)paramData;

	// Fix Error
	if (param == NULL || !param->img_display.data)
		return;

	// Handle zoom
	double zoomFactor = 0.9, maxZoomLevel = 0.7;
	if (flags == (cv::EVENT_FLAG_CTRLKEY + cv::EVENT_RBUTTONDOWN) && param->zoomLevel > maxZoomLevel)
		{
		cout << "Zoom In  while pressing CTRL key - position (" << x << ", " << y << ")" << endl;
		param->zoomLevel = param->zoomLevel * zoomFactor;
		cv::Rect roi((int)(x * param->zoomLevel), (int)(y * param->zoomLevel),
			(int)(param->zoomLevel * param->img_src.size().width - x),
			(int)(param->zoomLevel * param->img_src.size().height - y));


		param->img_display = param->img_src(roi);
		resize(param->img_display, param->img_display, cv::Size(param->img_src.size()));
		renderWindows(param);
		}
	else if (flags == (cv::EVENT_FLAG_CTRLKEY + cv::EVENT_LBUTTONDOWN) && param->zoomLevel < 1)
		{
		param->updateDisplayImg();
		renderWindows(param);
		}
	
	status.hit_point = cvPoint(x, y);
	switch (event)
		{


		case CV_EVENT_MOUSEMOVE:
			if (status.moveRect)
				{
				if (status.hit_point.x + status.pickUp_point.x > 0 &&
					status.hit_point.y + status.pickUp_point.y > 0 &&
					status.hit_point.x + status.pickUp_point.x + param->ROI.width < param->img_display.size().width &&
					status.hit_point.y + status.pickUp_point.y + param->ROI.height < param->img_display.size().height)
					{
					param->ROI.x = status.hit_point.x + status.pickUp_point.x;
					param->ROI.y = status.hit_point.y + status.pickUp_point.y;

					}
				renderWindows(param);
				}
			else if (status.resize_rect)
				{

				if (param->ROI.x + max(status.hit_point.x - param->ROI.x, param->maxRecSiz) < param->img_display.size().width && 
					param->ROI.y + max(status.hit_point.y - param->ROI.y, param->maxRecSiz) < param->img_display.size().height)
					{
					param->ROI.width = (int)max(status.hit_point.x - param->ROI.x, param->maxRecSiz);
					param->ROI.height = (int)max(status.hit_point.y - param->ROI.y, param->maxRecSiz);
					}
				renderWindows(param);
				}
			break;
			// Click Left Button
		case CV_EVENT_LBUTTONDOWN:
				
			status.moveRect = false;
			status.resize_rect = false;
			status.clickStatus = checkPoint2Box(param, status.hit_point);
			if (status.clickStatus == CENTER_CLICKED)		// Just move
				{
				status.moveRect = true;
				status.pickUp_point = - status.hit_point;
				status.pickUp_point.x += param->ROI.x;
				status.pickUp_point.y += param->ROI.y;
				
				}
			else if (status.clickStatus == CORNER_CLICKED)	// Resize
				status.resize_rect = true;
				
			break;
		
			// Right button release
		case CV_EVENT_LBUTTONUP:
			status.moveRect = false;
			status.resize_rect = false;
			break;
		}	
		
		
		
		
		
	}






/**
//* Handle Key Pressed Switch case function
//*/
void hndleKey(CvCallbackParam *param, char key)
	{

	switch (key)
		{
			//
			// Rectangle Move.
			//
		 

			//
			// Rectangle Resize.
			//

			// Shrink width.
		case 'y':
			param->ROI.width = max(0, param->ROI.width - 1);
			break;
			// Expand height.
		case 'u':
			param->ROI.height += 1;
			break;

			// Shrink height.
		case 'i':
			param->ROI.height = max(0, param->ROI.height - 1);
			break;
			//  Expand width.
		case 'o':
			param->ROI.width += 1;
			break;

			// Expand.
		case 'e':
			param->ROI.x = max(0, param->ROI.x - 1);
			param->ROI.width += 2;
			param->ROI.y = max(0, param->ROI.y - 1);
			param->ROI.height += 2;
			break;
			// Shrink.
		case 'E':
			param->ROI.x = min(param->img_display.size().width, param->ROI.x + 1);
			param->ROI.width = max(0, param->ROI.width - 2);
			param->ROI.y = min(param->img_display.size().height, param->ROI.y + 1);
			param->ROI.height = max(0, param->ROI.height - 2);
			break;
		}

	
	if (param->img_display.data)
		{
		renderWindows(param);
		}
	}



///**
//* Save cropped images 
//*/
bool writeImage(CvCallbackParam *param, bool detectType, cv::Mat img)
	{
	return false;
	}

///**
//* Update images onto both windows
//*/
cv::Mat renderWindows(CvCallbackParam *param)
	{
	param->updateRecPoint();
	// Indicate Region of Interest window
	cv::Mat tmpIm = param->img_display.clone();
	cv::rectangle(tmpIm, param->refRecPoint, param->endRecPoint, param->color);
	cv::imshow(param->w1nam, tmpIm);

	// Crop and show image in second window
	cv::Mat croppedIm = param->img_display(param->ROI); // Don't forget scaling
	cv::Mat displayCroppedIm;
	resize(croppedIm, displayCroppedIm, cvSize(300, 300 * param->ROI.height / param->ROI.width), 3);
	cv::imshow(param->w2nam, displayCroppedIm);
	return displayCroppedIm;
	}



///**
//* Print out usage
//*/
void usage(const CvCallbackParam* arg)
	{
	cout << "ImageClipper - image clipping helper tool." << endl;
	cout << "  Options" << endl;
	cout << "        Determine the output file path format." << endl;
	cout << "        This is a syntax sugar for -i and -v. " << endl;
	cout << "        Format Expression)" << endl;
	cout << "            %d - dirname of the original" << endl;
	cout << "            %i - filename of the original without extension" << endl;
	cout << "            %e - filename extension of the original" << endl;
	cout << "            %x - upper-left x coord" << endl;
	cout << "            %y - upper-left y coord" << endl;
	cout << "            %w - width" << endl;
	cout << "            %h - height" << endl;
	cout << "            %r - rotation degree" << endl;
	cout << "            %. - shear deformation in x coord" << endl;
	cout << "            %, - shear deformation in y coord" << endl;
	cout << "            %f - frame number (for video)" << endl;
	cout << "        Example) ./$i_%04x_%04y_%04w_%04h.%e" << endl;
	cout << "    -f" << endl;
	cout << "    --frame <frame = 1> (video)" << endl;
	cout << "        Determine the frame number of video to start to read." << endl;
	cout << "    -h" << endl;
	cout << "    --help" << endl;
	cout << "        Show this help" << endl;
	cout << endl;
	
	}

///**
//* Print Application Usage
//*/
void gui_usage()
	{
	cout << "Application Usage:" << endl;
	cout << "  Mouse Usage:" << endl;
	cout << "    Left  (select)          : Select or initialize a rectangle region." << endl;
	cout << "      Ctrl + Left           : Select a square region. It is combinable." << endl;
	cout << "      Alt + Left            : Select from center. It is combinable." << endl;
	cout << "    Right (move or resize)  : Move by dragging inside the rectangle." << endl;
	cout << "                              Resize by dragging outside the rectangle." << endl;
	cout << "    Middle or SHIFT + Left  : Initialize the watershed marker. Drag it. " << endl;
	cout << "  Keyboard Usage:" << endl;
	cout << "    + (zoom in)             : zoom in current image." << endl;
	cout << "    - (zoom out)            : zoom out current image." << endl;
	cout << "    s (save)                : Save the selected region as an image." << endl;
	cout << "    f (forward)             : Forward. Show next image." << endl;
	cout << "    SPACE                   : Save and Forward." << endl;
	cout << "    b (backward)            : Backward. " << endl;
	cout << "    q (quit) or ESC         : Quit. " << endl;
	cout << "    r (rotate) R (counter)  : Rotate rectangle in clockwise." << endl;
	cout << "    e (expand) E (shrink)   : Expand the rectagle size." << endl;
	cout << "    h (left) j (down) k (up) l (right) : Move rectangle. (vi-like keybinds)" << endl;
	cout << "                                         (+shift to move faster)" << endl;
	cout << "    y (left) u (down) i (up) o (right) : Resize rectangle (Move boundaries)." << endl;
	}

///**
//* Check Clicked Region
//*/
ClickStatus checkPoint2Box(CvCallbackParam *param, cv::Point mousePos)
	{
	int buff = 5;
	double dist = std::pow(std::pow(mousePos.x - param->endRecPoint.x, 2)
		+ std::pow(mousePos.y - param->endRecPoint.y, 2), 0.5);
	if (dist < buff)
		{
		//cout << "corner" << mousePos.x << " " << param->refRecPoint.x << " " << param->endRecPoint.x << endl;
		//std::cout << "corner" << endl;
		return CORNER_CLICKED;
		}
	else if(mousePos.x > param->refRecPoint.x - buff && mousePos.x <  param->endRecPoint.x + buff)
		{
		if (mousePos.y > param->refRecPoint.y - buff && mousePos.y < param->endRecPoint.y + buff)
			{
			//std::cout << "body" << endl;
			return CENTER_CLICKED;
			}
		}
	//std::cout << "missed" << endl;
	return NO_TARGET_CLICKED;
	}
