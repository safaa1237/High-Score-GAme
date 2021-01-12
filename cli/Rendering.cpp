#include "Rendering.h"

#include <iostream>
#include <fstream>

Eigen::Matrix4d createProjectionMatrix(const double fieldOfViewY, const double aspectRatio, double zNear, double zFar)
{
	Eigen::Matrix4d result = Eigen::Matrix4d::Zero();
	const double yScale = std::cos(0.5f * fieldOfViewY) / std::sin(0.5f * fieldOfViewY);
	const double xScale = yScale / aspectRatio;
	result(0, 0) = xScale;
	result(1, 1) = yScale;
	result(2, 2) = -(zFar + zNear) / (zFar - zNear);
	result(2, 3) = -1.f;
	result(3, 2) = -2.f * zFar * zNear / (zFar - zNear);
	return result;
}

bool dumpFrame(const Eigen::Matrix4d& projectionMatrix, size_t numberOfPoints, const SCENE_API::LSPointXYZFRGBA* points, const std::wstring& filePath)
{
	std::wofstream ofs(filePath, std::ofstream::out);
	if (!ofs.is_open() || ofs.bad())
		return false;

	for (size_t i = 0; i < numberOfPoints; ++i)
	{
		Eigen::Vector4d projected = projectionMatrix * Eigen::Vector4d(points[i].x, points[i].y, points[i].z, 1);
		projected = projected / projected[3];
		ofs << projected[0] << " " << projected[1] << " " << 0.0 << " " << int(points[i].r) << " " << int(points[i].g) << " " << int(points[i].b) << std::endl;
	}

	ofs.close();
	std::wcout << L"Dumped frame with " << numberOfPoints << L" points to file" << filePath << std::endl;

	return true;
}
