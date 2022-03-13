// R220307 Image Processing Basic.cpp
// This file contains the 'main' function. Program execution begins and ends there.

#include "opencv2/core.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>

using namespace std;
using namespace cv;


double getAngle(Point pt1, Point pt2, Point pt0)
{
    double dy2 = (double)pt2.y - (double)pt0.y;
    double dx1 = (double)pt1.x - (double)pt0.x;
    double dy1 = (double)pt1.y - (double)pt0.y;
    double dx2 = (double)pt2.x - (double)pt0.x;
    return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
}

bool isVertical(Point a, Point b) {
    if (a.x == b.x)
        return true;

    double lineGradient = (a.y - b.y) / (a.x - b.x);
    if (fabs(lineGradient) > 1)
        return true;
    else
        return false;
}

void getCenterOfSquare(vector<Point> squarePoints, Point &centerOfSquare) {
    Point p1 = squarePoints[0];
    Point p2 = squarePoints[1];
    Point p3 = squarePoints[2];
    Point p4 = squarePoints[3];
    centerOfSquare.x = (p1.x + p2.x + p3.x + p4.x) / 4;
    centerOfSquare.y = (p1.y + p2.y + p3.y + p4.y) / 4;
}

void getAvgWidthHeight(vector<Point> squarePoints, double &lineWidthAvg, double &lineHeightAvg) {
    double totalLineHeight = 0;
    double totalLineWidth = 0;

    for (int i = 0; i < 4; i++) {

        Point p1 = squarePoints[i];
        Point p2 = squarePoints[(i + 1) % 4];

        if (isVertical(p1, p2))
            totalLineHeight += fabs(sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)));
        else
            totalLineWidth += fabs(sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)));
    }
    lineWidthAvg = totalLineWidth / 2;
    lineHeightAvg = totalLineHeight / 2;
}

int main()
{
    //string path = "Images/1.jpg";
    //string paths[] = {"Images/1.jpg"};

    string paths[] = { "Images/1.jpg", "Images/2.jpg", "Images/3.jpg","Images/4.jpg", "Images/5.jpg" };
    
    for (string path : paths)
    {
        //const int THRESHOLD = 135;

        Mat img = imread(path);

        // Preprocess images
        Mat imgGray, imgThresh;

        cvtColor(img, imgGray, COLOR_BGR2GRAY);
        threshold(imgGray, imgThresh, 0, 255, THRESH_OTSU);

        // Unused Functions
        // 
        //bitwise_not(img, img);
        //GaussianBlur(imgGray, imgBlur, Size(9, 9), 3, 0);
        //bilateralFilter(imgGray, imgBil, 15, 80, 80);
        //Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
        //morphologyEx(imgThresh, imgMor, MORPH_OPEN, kernel);
        //GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
        //Canny(imgThresh, imgThresh, 100, 200, 3, false);
        //Mat kernal = getStructuringElement(MORPH_RECT, Size(3, 3));
        //dilate(imgCanny, imgDil, kernal);
        //getContours(imgDil, img);

        // To find contours
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(imgThresh, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

        vector<Point> approx;
        vector<vector<Point>> squares;
        //cout << contours.size();

        for (size_t i = 0; i < contours.size(); i++)
        {
            // Approximate polygon
            approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.05, true);

            // Find Square: 4 vertices & large area
            if (approx.size() == 4 && fabs(contourArea(approx)) > 1000)
            {
                double maxCos = 0;
                for (int j = 2; j < 5; j++)
                {
                    // find the maximum cosine of the getAngle between joint edges
                    double cos = (double)fabs(getAngle(approx[j % 4], approx[j - 2], approx[j - 1]));
                    maxCos = MAX(maxCos, cos);
                }
                // if cosines of all angles are small, then write the Coordinates into squares
                if (maxCos < 0.3)
                    squares.push_back(approx);
            }

        }


        // Remove boundary square, only take inner one square, not the best way but it works
        // It works means it works
        vector<Point> square = squares[0];
        //cout << square << endl;

        // Getting 4 points coordinate & center
        Point centerOfSquare;
        getCenterOfSquare(square, centerOfSquare);

        // Get the width & height
        double lineWidthAvg, lineHeightAvg;
        getAvgWidthHeight(square, lineWidthAvg, lineHeightAvg);


        //cout << p1 << p2 << p3 << p4 << endl;

        // Print out the result(s)
        cout << path << endl;
        cout << "Average Width: " << lineWidthAvg << endl;
        cout << "Average Height: " << lineHeightAvg << endl;
        cout << "Center: " << centerOfSquare << endl;
        cout << "------------------------------------------" << endl;

        circle(img, centerOfSquare, 2, Scalar(0, 0, 255), 2);
        polylines(img, square, true, Scalar(0, 255, 0), 2, LINE_AA);
        imshow(path, img);
        //imshow("Image Thresh", imgThresh);
        //imshow("Image Mor", imgMor);
    }

    //imshow("Image Thresh", imgThresh);
    //imshow("Image Mor", imgMor);
    //imshow("Image Canny", imgCanny);
    //imshow("Image dilate", imgDil);
    waitKey(0);


}