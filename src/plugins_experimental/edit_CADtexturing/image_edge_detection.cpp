#include "image_edge_detection.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/line_descriptor.hpp>

using namespace cv;
using namespace std;

cv::Mat qimage_to_mat_ref(QImage &img)
{
	return cv::Mat(img.height(), img.width(),
		CV_8UC4, img.bits(), img.bytesPerLine());
}

int detect_edges(QImage input, const  char * filename)
{
	Mat src1 = qimage_to_mat_ref(input);
	Mat src2;

	Mat gray, edge, draw;
	cv::imwrite("src1.jpg", src1);
	cvtColor(src1, gray, CV_RGB2GRAY);
	cv::resize(gray, src2, cv::Size(gray.cols / 2, gray.rows / 2 ));

	//Canny(src2, edge, 150, 350, 3);

	//edge.convertTo(draw, CV_8U);
	//imwrite("image_edges.jpg",draw);
	cv::Mat output = src2.clone();
	cv::Mat mask = Mat::ones(src1.size(), CV_8UC1);
	std::vector< cv::line_descriptor::KeyLine >   	keypoints;
	cv::Ptr<cv::line_descriptor::LSDDetector> lsd = cv::line_descriptor::LSDDetector::createLSDDetector();
	lsd->detect(src2, keypoints, 1, 1);

	/* draw lines extracted from octave 0 */
	if (output.channels() == 1)
		cvtColor(output, output, COLOR_GRAY2BGR);
	for (size_t i = 0; i < keypoints.size(); i++)
	{
		cv::line_descriptor::KeyLine kl = keypoints[i];
		if (kl.octave == 0)
		{
			/* get a random color */
			int R = (rand() % (int)(255 + 1));
			int G = (rand() % (int)(255 + 1));
			int B = (rand() % (int)(255 + 1));

			/* get extremes of line */
			Point pt1 = Point2f(kl.startPointX, kl.startPointY);
			Point pt2 = Point2f(kl.endPointX, kl.endPointY);

			/* draw line */
			line(output, pt1, pt2, Scalar(B, G, R), 3);
		}

	}
	cv::imwrite(filename, output);

	//cv::Mat outputC = src2.clone();

	//std::vector<std::vector<cv::Point> > contours;
	//std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(src2, contours, hierarchy,
	//	RETR_LIST /*RETR_EXTERNAL  RETR_LIST ,RETR_CCOMP ,RETR_TREE RETR_FLOODFILL  */,
	//	CHAIN_APPROX_SIMPLE);

	//for (int ii = 0; ii < contours.size(); ++ii){
	//	int R = (rand() % (int)(255 + 1));
	//	int G = (rand() % (int)(255 + 1));
	//	int B = (rand() % (int)(255 + 1));

	//	for (int jj = 0; jj < contours[ii].size(); ++jj)
	//		line(outputC, contours[ii][jj], contours[ii][(jj + 1) % contours[ii].size()], Scalar(B, G, R), 3);
	//}
	//cv::imwrite("contour.jpg", outputC);
 	return 0;



}