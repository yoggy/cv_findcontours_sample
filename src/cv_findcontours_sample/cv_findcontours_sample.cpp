#pragma once

#ifdef _WIN32
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma warning(disable: 4819)
#ifdef _DEBUG
#pragma comment(lib, "opencv_world300d.lib")
#else
#endif
#pragma comment(lib, "opencv_world300.lib")
#endif

#include <vector>
#include <algorithm>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int conter_area_cmp(const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) {
	return cv::contourArea(b) > cv::contourArea(a);
}

cv::Mat process(cv::Mat &target_img)
{
	std::vector<cv::Scalar> colors;
	colors.push_back(cv::Scalar(0, 0, 255));
	colors.push_back(cv::Scalar(0, 255, 0));
	colors.push_back(cv::Scalar(255, 0, 0));
	colors.push_back(cv::Scalar(0, 255, 255));
	colors.push_back(cv::Scalar(255, 255, 0));
	colors.push_back(cv::Scalar(255, 0, 255));

	cv::Mat gray_img;
	cv::cvtColor(target_img, gray_img, cv::COLOR_BGR2GRAY);

	cv::Mat bin_img;
	cv::threshold(gray_img, bin_img, 128, 255, cv::THRESH_BINARY);

	// cv::findContoursÇÃì¸óÕâÊëúÇÕÅAÇ†ÇÁÇ©Ç∂ÇﬂCV_8U1Ç…â¡çHÇµÇƒÇ®Ç≠Ç±Ç∆
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bin_img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

	std::sort(contours.begin(), contours.end(), conter_area_cmp);

	cv::Mat result_img;
	target_img.copyTo(result_img);

	for (unsigned int i = 0; i < contours.size(); ++i) {
		// ñ êœÇ™Ç†ÇÈíˆìxëÂÇ´Ç¢ïîï™ÇëŒè€Ç∆Ç∑ÇÈ
		if (cv::contourArea(contours[i]) > 100) {
			cv::polylines(result_img, contours[i], true, colors[i%colors.size()], 2);

			cv::Rect r = cv::boundingRect(contours[i]);
			cv::rectangle(result_img, r, cv::Scalar(0, 255, 0), 1);

			cv::RotatedRect rr = cv::minAreaRect(contours[i]);
			cv::Point2f ps[4];
			rr.points(ps);
			for (int i = 0; i < 4; ++i) {
				cv::line(result_img, ps[i], ps[(i + 1) % 4], cv::Scalar(255, 0, 0), 1);
			}

			cv::RotatedRect re = cv::fitEllipse(contours[i]);

			cv::ellipse(result_img, re, cv::Scalar(0, 0, 255), 1);
		}
	}

	return result_img;
}

int main(int argc, char* argv[])
{
	cv::Mat target_img;
	target_img = cv::imread("target.png");

	while (true) {
		cv::Mat result_img = process(target_img);
		if (!result_img.empty()) {
			cv::imshow("result", result_img);
		}

		int c = cv::waitKey(1);
		if (c == 27) {
			break;
		}
	}
	cv::destroyAllWindows();

	return 0;
}

