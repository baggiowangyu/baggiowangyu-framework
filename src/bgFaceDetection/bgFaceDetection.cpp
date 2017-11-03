#include "stdafx.h"

#include "base/rand_util.h"
#include "base/bind.h"
#include "base/location.h"

#include "bgFaceDetection.h"

bgFaceDetection::bgFaceDetection()
{

}

bgFaceDetection::~bgFaceDetection()
{

}

int bgFaceDetection::Init(const char *face_factor_path, const char *eyes_factor_path)
{
	int errCode = 0;

	// 加载特征文件
	bool result = face_cascade_.load(face_factor_path);
	if (!result)
		return -1;

	result = eyes_cascade_.load(eyes_factor_path);
	if (!result)
		return -2;

	return errCode;
}

void bgFaceDetection::Close()
{

}