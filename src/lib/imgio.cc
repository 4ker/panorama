//File: imgio.cc
//Author: Yuxin Wu <ppwwyyxx@gmail.com>

#include <cstdlib>
#include <vector>
#define cimg_display 0
#include "CImg.h"

#include "imgproc.hh"
#include "lib/utils.hh"
#include "lodepng/lodepng.h"

using namespace cimg_library;
using namespace std;

Mat32f read_png(const char* fname) {
	vector<unsigned char> img;
	unsigned w, h;
	unsigned error = lodepng::decode(img, w, h, fname);
	if (error)
		error_exit(ssprintf(
					"png encoder error %u: %s", error, lodepng_error_text(error)));
	Mat32f mat(h, w, 3);
	unsigned npixel = w * h;
	float* p = mat.ptr();
	unsigned char* data = img.data();
	REP(i, npixel) {
		*(p++) = (float)*(data++) / 255.0;
		*(p++) = (float)*(data++) / 255.0;
		*(p++) = (float)*(data++) / 255.0;
		data++;
	}
	return mat;
}

Mat32f read_img(const char* fname) {
	if (! exists_file(fname))
		error_exit(ssprintf("File %s not exists!", fname));
	if (endswith(fname, ".png"))
		return read_png(fname);
	CImg<float> img(fname);
	m_assert(img.spectrum() == 3 || img.spectrum() == 1);
	img = img / 255.0;
	Mat32f mat(img.height(), img.width(), 3);
	if (img.spectrum() == 3) {
		REP(i, mat.rows())
			REP(j, mat.cols()) {
				mat.at(i, j, 0) = img(j, i, 0);
				mat.at(i, j, 1) = img(j, i, 1);
				mat.at(i, j, 2) = img(j, i, 2);
			}
	} else {
		REP(i, mat.rows())
			REP(j, mat.cols()) {
				mat.at(i, j, 0) = mat.at(i, j, 1) = mat.at(i, j, 2) = img(j, i);
			}
	}
	return mat;
}


void write_png(const char* fname, const Mat32f& mat) {
	int n = mat.pixels();
	vector<unsigned char> img(n * 4);
	const float* p = mat.ptr();
	unsigned char* data = img.data();
	REP(i, n) {
		data[0] = (p[0] < 0 ? 0 : p[0]) * 255;
		data[1] = (p[1] < 0 ? 0 : p[1]) * 255;
		data[2] = (p[2] < 0 ? 0 : p[2]) * 255;
		data[3] = 255;
		data += 4; p += 3;
	}
	unsigned error = lodepng::encode(fname, img, mat.width(), mat.height());
	if(error)
		error_exit(ssprintf(
					"png encoder error %u: %s", error, lodepng_error_text(error)));
}

void write_rgb(const char* fname, const Mat32f& mat) {
	m_assert(mat.channels() == 3);
	if (endswith(fname, ".png")) {
		write_png(fname, mat);
		return;
	}
	CImg<float> img(mat.cols(), mat.rows(), 1, 3);
	REP(i, mat.rows())
		REP(j, mat.cols()) {
			// use white background. Color::NO turns to 1
			img(j, i, 0) = (mat.at(i, j, 0) < 0 ? 1 : mat.at(i, j, 0)) * 255;
			img(j, i, 1) = (mat.at(i, j, 1) < 0 ? 1 : mat.at(i, j, 1)) * 255;
			img(j, i, 2) = (mat.at(i, j, 2) < 0 ? 1 : mat.at(i, j, 2)) * 255;
		}
	img.save(fname);
}

