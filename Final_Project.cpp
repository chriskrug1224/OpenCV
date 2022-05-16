#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat img;
vector<vector<int>> newPoints;

//majority of code is using https://www.youtube.com/watch?v=2FYm3GOonhk as a framework

// hmin, smin, vmin, hmax, smax, vmax, ypos(for text position)
vector<vector<int>> myColors{ //{51,56,116,128,170,255,100},				// light green highlighter
							{162,130,69,179,201,134,150},			// red colored pencil
							{0,94,154,13,255,255,200}					// orange colored pencil
};

vector<Scalar> myColorValues{ //{0, 255, 0},
								{0, 0, 255},
									{0, 165, 255}
};

vector<string> myColorNames{ //"Green", 
	"Red", "Orange" };			//  for imShow() in findColor, commented out currently

Point getContours(Mat imgDil)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);
	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			// drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);										// draw purple contours around object
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);						// draw green rectangle around object

		}
	}

	return myPoint;

};

vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		//imshow(myColorNames[i], mask);
		Point myPoint = getContours(mask);
		//circle(img, myPoint, 10, Scalar(255, 0, 255), FILLED);			// draws purple dot at myPoint coordinate (top-middle point)

		if (myPoint.x != 0)
		{
			newPoints.push_back({ myPoint.x, myPoint.y, i, myColors[i][6] });			// myPoint called from getContours(), i = color in the for loop
		}
	}

	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{
	putText(img, "Colors Detected", Point(355, 50), FONT_HERSHEY_COMPLEX, 1, (255, 0, 255), 2, LINE_AA);
	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED);																		// paints at top-middle point of rectangle		
	}
}

//Custom generated function
void detectColorText(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{
	putText(img, "Colors Detected", Point(355, 50), FONT_HERSHEY_COMPLEX, 1, (255, 0, 255), 2, LINE_AA);
	for (int i = 0; i < newPoints.size(); i++)
	{
		putText(img, myColorNames[newPoints[i][2]], Point(500, newPoints[i][3]), FONT_HERSHEY_COMPLEX, 1, myColorValues[newPoints[i][2]], 2, LINE_AA);
	}
}

void main()
{
	VideoCapture cap(0);
	while (true) {
		cap.read(img);
		newPoints = findColor(img);
		detectColorText(newPoints, myColorValues);
		drawOnCanvas(newPoints, myColorValues);
		imshow("Image", img);
		waitKey(1);
	}
}