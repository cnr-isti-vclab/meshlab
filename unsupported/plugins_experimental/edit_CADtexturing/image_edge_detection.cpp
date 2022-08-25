#include "image_edge_detection.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "C:/devel/opencv/build/install/include/opencv2/line_descriptor.hpp"
#include <opencv2\flann.hpp>

using namespace cv;
using namespace std;

cv::Mat qimage_to_mat_ref(QImage &img)
{
	return cv::Mat(img.height(), img.width(),
		CV_8UC4, img.bits(), img.bytesPerLine());
}




bool mergeSegments(std::vector< cv::line_descriptor::KeyLine > & kp, float closeendpointlessthan = 7, float parallelifanglelessthan = 15, float parallellinescloserthan = 7 ){
	std::vector< cv::line_descriptor::KeyLine > res;
	std::vector<bool> joined;
	bool retv;
	joined.resize(kp.size(),false);
	cv::Mat features = Mat::zeros(kp.size() * 2, 2, CV_32F);
	for (int i = 0; i < kp.size(); ++i){
		features.at<float>(i * 2    , 0) = kp[i].startPointX;
		features.at<float>(i * 2    , 1) = kp[i].startPointY;
		features.at<float>(i * 2 + 1, 0) = kp[i].endPointX;
		features.at<float>(i * 2 + 1, 1) = kp[i].endPointY;
	}
	flann::Index flann_index(features, flann::KDTreeIndexParams());

	for (int i = 0; i < kp.size()*2; ++i) if(!joined[i/2]){
		cv::Mat querypoint(1, 2, CV_32F);
		std::vector<int> indices;
		std::vector<float> distances(2,10000.0);

		querypoint.at<float>(0, 0) = features.at<float>(i, 0);
		querypoint.at<float>(0, 1) = features.at<float>(i, 1);
		flann_index.knnSearch(querypoint, indices, distances, 2);
		int j = indices[1];

		if (!joined[j / 2] && (i / 2 != j / 2) && distances[1] <= closeendpointlessthan){ // pixels
			cv::Point2f s00, s01, s10, s11;
			// test collinearity
			s00.x = features.at<float>(((i % 2) == 0) ? i : i - 1, 0);
			s00.y = features.at<float>(((i % 2) == 0) ? i : i - 1, 1);
			s01.x = features.at<float>(((i % 2) == 0) ? i + 1 : i, 0);
			s01.y = features.at<float>(((i % 2) == 0) ? i + 1 : i, 1);

			s10.x = features.at<float>(((j % 2) == 0) ? j : j - 1, 0);
			s10.y = features.at<float>(((j % 2) == 0) ? j : j - 1, 1);
			s11.x = features.at<float>(((j % 2) == 0) ? j + 1 : j, 0);
			s11.y = features.at<float>(((j % 2) == 0) ? j + 1 : j, 1);

			cv::Point2f v0 = s01 - s00;
			v0 = v0 / sqrt((float)v0.dot(v0));
			cv::Point2f v1 = s11 - s10;
			v1 = v1 / sqrt((float)v1.dot(v1));
			float alpha = fabs(asin(fabs(v0.cross(v1))));

			cv::Point2f ij0, ij1;
			ij0.x = features.at<float>(i, 0);
			ij0.y = features.at<float>(i, 1);
			ij1.x = features.at<float>(j, 0);
			ij1.y = features.at<float>(j, 1);

			float t0 =  (ij1 - s00).dot(v0);
			cv::Point2f c0 = s00 + v0*t0;
			float l0 = (c0 - ij1).dot(c0 - ij1);

			float t1 = (ij0 - s10).dot(v1);
			cv::Point2f c1 = s10 + v1*t1;
			float l1 = (c1 - ij0).dot(c1 - ij0);


			if ((alpha < M_PI * parallelifanglelessthan / 180.0) &&
				(l0 <parallellinescloserthan) &&
				(l1 <parallellinescloserthan))
			{
				cv::line_descriptor::KeyLine newSegm;
				newSegm.startPointX = features.at<float>(((i % 2) == 0) ? i + 1 : i - 1, 0);
				newSegm.startPointY = features.at<float>(((i % 2) == 0) ? i + 1 : i - 1, 1);


				newSegm.endPointX = features.at<float>(((j % 2) == 0) ? j + 1 : j - 1, 0);
				newSegm.endPointY = features.at<float>(((j % 2) == 0) ? j + 1 : j - 1, 1);

				newSegm.octave = 0;
				res.push_back(newSegm);

				//{	// DEBUG STUFF
				//	FILE*f = fopen("_qr.txt", "a"); 

				//	fprintf(f, "Merge:%d %d, %f\n  < %f  %f> < %f %f>\n < %f %f>  < %f %f> \n alpha %f, l0 %f l1 %f \n", 
				//													i, j, distances[1],
				//													features.at<float>(((i % 2) == 0) ? i : i - 1, 0),
				//													features.at<float>(((i % 2) == 0) ? i : i - 1, 1),
				//													features.at<float>(((i % 2) == 0) ? i + 1 : i, 0),
				//													features.at<float>(((i % 2) == 0) ? i + 1 : i, 1),
				//													features.at<float>(((j % 2) == 0) ? j : j - 1, 0),
				//													features.at<float>(((j % 2) == 0) ? j : j - 1, 1),
				//													features.at<float>(((j % 2) == 0) ? j + 1 : j, 0),
				//													features.at<float>(((j % 2) == 0) ? j + 1 : j, 1),
				//													alpha,l0,l1
				//													);

				//	fprintf(f, " v0 %f %f, v1 %f %f\n",s00.x,s00.y,s11.x,s11.y);
				//	fprintf(f, "new-> < %f  %f> < %f %f>    \n", newSegm.startPointX, newSegm.startPointY, newSegm.endPointX, newSegm.endPointY);
				//	fclose(f); 
				//}

				joined[j / 2] = true;
				joined[i / 2] = true;
			}
		}
	}
	for (int i = 0; i < kp.size(); ++i)
		if (!joined[i])
			res.push_back(kp[i]);


	retv = (kp.size() != res.size());

//	{FILE*f = fopen("_sizes.txt", "w"); fprintf(f, "%d %d", kp.size(), res.size()); fclose(f); }

	kp = res;
	return retv;
}

void	saveEdgesImage(const char * name, cv::Mat o, std::vector< cv::line_descriptor::KeyLine >   	keypoints)
{
	/* draw lines extracted from octave 0 */
	if (o.channels() == 1)
		cvtColor(o, o, COLOR_GRAY2BGR);
	for (size_t i = 0; i < keypoints.size(); i++)
	{
		cv::line_descriptor::KeyLine kl = keypoints[i];
		if (kl.octave == 0)
		{
			/* get a random color */
			int R = int(kl.startPointX + kl.startPointY) % 255;
			int G = int(kl.startPointX * kl.startPointY) % 255;
			int B = int(kl.startPointX - kl.startPointY) % 255;

			/* get extremes of line */
			Point pt1 = Point2f(kl.startPointX, kl.startPointY);
			Point pt2 = Point2f(kl.endPointX, kl.endPointY);

			/* draw line */
			line(o, pt1, pt2, Scalar(B, G, R), 3);
		}

	}
	cv::imwrite(name, o);
}
int detect_edges(QImage input, const  char * filename)
{
	Mat src1 = qimage_to_mat_ref(input);
	Mat src2;

	Mat gray, edge, draw;

//	cv::imwrite("src1.jpg", src1);
	cvtColor(src1, gray, CV_RGB2GRAY);
	cv::resize(gray, src2, cv::Size(gray.cols / 2, gray.rows / 2 ));


	cv::Mat output = src2.clone();
	cv::Mat output1 = src2.clone();
	cv::Mat corners = src2.clone();
	cv::Mat mask = Mat::ones(src1.size(), CV_8UC1);
	std::vector< cv::line_descriptor::KeyLine >   	keypoints;
	cv::Ptr<cv::line_descriptor::LSDDetector> lsd = cv::line_descriptor::LSDDetector::createLSDDetector();

	//{ // DEBUG READ   EDGES FROM FILE
	//	FILE * f = fopen("edges.txt", "r");
	//	int n = 0;
	//	fscanf(f, "%d\n", &n);
	//	keypoints.resize(n);
	//	for (int y = 00; y < n; ++y)
	//	{
	//		fscanf(f, "%f %f %f %f", &keypoints[y].startPointX, &keypoints[y].startPointY,
	//			&keypoints[y].endPointX, &keypoints[y].endPointY);
	//		keypoints[y].octave = 0;
	//	}
	//	fclose(f);
	//}


	lsd->detect(src2, keypoints, 2, 1);

	{ // corner harris 
		cv::cornerHarris(src2, corners, 2, 3, 0.04);
		/// Normalizing
		cv::Mat corners_norm, corners_norm_sclaed;
		normalize(corners, corners_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
		convertScaleAbs(corners_norm, corners_norm_sclaed);

		/// Drawing a circle around corners
		for (int j = 0; j < corners_norm.rows; j++)
		{
			for (int i = 0; i < corners_norm.cols; i++)
			{
				if ((int)corners_norm.at<float>(j, i) > 100)
				{
					circle(corners_norm_sclaed, Point(i, j), 5, Scalar(0), 2, 8, 0);
				}
			}
		}
		cv::imwrite("corners.jpg", corners_norm_sclaed);
	}
	//{ // DEBUG WRITE OUT EDGES
	//	FILE * f = fopen("edges.txt", "w");
	//	fprintf(f, "%d\n", keypoints.size());
	//	for (int y = 00; y < keypoints.size(); ++y)
	//	{
	//		fprintf(f, "%f %f %f %f\n", keypoints[y].startPointX, keypoints[y].startPointY,
	//			keypoints[y].endPointX, keypoints[y].endPointY);
	//	}
	//	fclose(f);
	//}



	saveEdgesImage("imageedges.jpg", output, keypoints);

	/* simple greedy edge simplfication */
	while(
			mergeSegments(keypoints)  
		);
	saveEdgesImage("merged.jpg", output1, keypoints);



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