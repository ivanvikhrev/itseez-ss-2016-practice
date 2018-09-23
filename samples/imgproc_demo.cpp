#include <iostream>
#include <string>
#include <image_processing.hpp>
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

const char* kAbout =
    "This is an empty application that can be treated as a template for your "
    "own doing-something-cool applications.";

//const char* kOptions =
//    "{ v video        |        | video to process         }"
//    "{ h ? help usage |        | print help message       }";

const char* kOptions =
"{ @image         | <none> | image to process            }"
"{ gray           |        | convert ROI to gray scale   }"
"{ median         |        | apply median filter for ROI }"
"{ edges          |        | detect edges in ROI         }"
"{ pix            |        | pixelize ROI                }"
"{ h ? help usage |        | print help message          }";

struct MouseCallbackState
{
   bool is_selection_started;
   bool is_selection_finished;
   Point point_first;
   Point point_second;
};

void OnMouse(int event, int x, int y, int flags, void* param)
{	
	MouseCallbackState* mouse = (MouseCallbackState*)param;

	switch (event)
	{
	case EVENT_LBUTTONDOWN:


		mouse->is_selection_started = true;
		mouse->is_selection_finished = false;
		mouse->point_first.x = x;
		mouse->point_first.y = y;
		break;
	case EVENT_LBUTTONUP:
		mouse->is_selection_started = false;
		mouse->is_selection_finished = true;
		if (mouse->point_first.x != x && mouse->point_first.y != y)
		{
			mouse->point_second.x = x;
			mouse->point_second.y = y;
		}
		//rectangle(,Rect(mouse->point_first,mouse->point_second));
		break;
	    case EVENT_MOUSEMOVE:
		if (mouse->is_selection_finished == false)
		{
			mouse->point_second.x = x;
			mouse->point_second.y = y;
		}
		break;
	}
	
}
int main(int argc, const char** argv) {
  // Parse command line arguments.
  CommandLineParser parser(argc, argv, kOptions);
  
  parser.about(kAbout);
  // If help option is given, print help message and exit.
if (parser.get<bool>("help"))
	{
	  parser.printMessage();
	  return 0;
	}
  Mat img,src,dst;
  string imageName(parser.get<string>(0)); 
  img = imread(imageName.c_str(), 1);
  
  if (img.empty())                      
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

  namedWindow("Original", CV_WINDOW_NORMAL);
  MouseCallbackState p;
  ImageProcessorImpl obj;
  setMouseCallback("Original", OnMouse, &p);
  while (1) 
  {
	  char c = waitKey(100);//?
	
	  imshow("Original", img);
	  Rect imgSize(Point(), img.size());
	  Rect roi(p.point_first, p.point_second);
	  if (p.point_first.x != 0 && p.point_second.x != 0 && p.point_first.y != 0 
		  && p.point_second.y != 0 && imgSize.contains(p.point_first) && imgSize.contains(p.point_second))
	  {
		  if (parser.has("gray"))
		  {
			  dst = obj.CvtColor(img, roi);
		  }
		  else
			  if (parser.has("median"))
			  {
				  int _size = 11;// size must be odd
				  dst = obj.Filter(img, roi, 11);
			  }
			  else 
				  if (parser.has("edges"))
				  {	
					  int _filter_size = 1,
						  _low_threshold = 50,
						  _ratio = 4,
					      _ksize = 1;
					  dst = obj.DetectEdges(img, roi, _filter_size,_low_threshold , _ratio, _ksize);
				  }
				  else
					  if (parser.has("pix"))
					  {		
						  int _divs = 29;
						  dst = obj.Pixelize(img, roi, _divs); // crashes periodically
					  }
		  imshow("Result", dst);
	  }
	  if (c == 27) 
	  { 
		  break;
	  }
  }


  waitKey(0);
  return 0;
}

