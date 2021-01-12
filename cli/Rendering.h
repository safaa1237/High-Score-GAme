#pragma once

#include <string>

#include <Eigen/Core>

#include <core_api/lspointtypes.h>

// Create a projection matrix to convert the points returned by the PC2 render API to flat 2D points
Eigen::Matrix4d createProjectionMatrix(const double fieldOfViewY, const double aspectRatio, double zNear, double zFar);

// Dumps a point list by the PC2 renderer to a file after converting it to 2D
bool dumpFrame(const Eigen::Matrix4d& projectionMatrix, size_t numberOfPoints, const SCENE_API::LSPointXYZFRGBA* points, const std::wstring& filePath);
