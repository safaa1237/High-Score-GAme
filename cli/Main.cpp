#include "Helpers.h"
#include "CmdLineParser.h"
#include "TimedScope.h"
#include "Rendering.h"

#include <core_api/lsproject.h>
#include <core_api/lsprocessing.h>
#include <core_api/lspointcloud.h>
#include <core_api/lspointcloudrenderer.h>

#include <standalone_api/lsstandalonecontext.h>

using namespace SCENE_API;

// Still uses the legacy API key to be able to support older runtimes
const wchar_t* apiKey = L"RBHE4868TGTXFLNWNMFVSNDPL";

const std::wstring benchmarkVersion = L"1.0";
const size_t loadCycleCount = 5;
const size_t frameCount = 100;
const bool dumpFrames = false;
uint16_t resolution[2] = {1024, 2014};
const double fovY = 3.14 / 3;
const double zNear = 0.2;
const double zFar = 2000.0;

int wmain(int argc, wchar_t** argv)
{
	// Parse CLI arguments, vars will be initialized by parser
	std::wstring workingDir;
	std::wstring inputDir;
	std::wstring outputDir;
	bool teamCity;
	parseCliOptions(argc, argv, teamCity, workingDir, inputDir, outputDir);

	// Delete any existing temp data from previous runs
	removeFolder(workingDir);

	// Get some PC details
	std::map<std::wstring, std::wstring> results;
	getComputerInfo(results);

	{
		TimedScope timedScope(teamCity);

		// Initialize SCENE API context
		LSStandaloneContext context(apiKey);
		results[L"StartupTime"] = std::to_wstring(timedScope.timedStep(L"StartupTime"));

		// Log Benchmark Version
		std::wcout << L"Benchmark Version: " << benchmarkVersion << std::endl;
		results[L"BenchmarkVersion"] = benchmarkVersion;

		// Log SCENE API Version
		LSString apiVersion = context.getApiVersion();
		std::wcout << L"SCENE API Version: " << apiVersion.toWcharStr() << std::endl;
		results[L"ApiVersion"] = apiVersion.toWcharStr();

		// Create files for new empty project
		LSResult::Result createResult = LSProject::createProject(workingDir.c_str(), L"benchmark");
		if (createResult != LSResult::Ok)
		{
			std::wcout << L"Project creation error: " << std::to_wstring(createResult) << std::endl;
			return 1;
		}
		results[L"ProjectCreateTime"] = std::to_wstring(timedScope.timedStep(L"ProjectCreateTime"));

		// Load the empty project
		ref_ptr<LSProject> project;
		LSResult::Result loadResult = LSProject::loadProject(workingDir.c_str(), project);
		if (loadResult != LSResult::Ok)
		{
			std::wcout << L"Project loading error: " << std::to_wstring(loadResult) << std::endl;
			return 1;
		}
		loadResult = context.setCurrentWorkspace(project);
		if (loadResult != LSResult::Ok)
		{
			std::wcout << L"Set current workspace error: " << std::to_wstring(loadResult) << std::endl;
			return 1;
		}
		results[L"ProjectLoadTime"] = std::to_wstring(timedScope.timedStep(L"ProjectLoadTime"));

		// Count and import the scans
		int64_t scanCount = 0;
		for (const auto& absolutePath : getSubFolders(inputDir))
		{
			std::wcout << L"Importing scan " << absolutePath << std::endl;
			if (!project->importData(LSString(absolutePath.c_str())))
			{
				std::wcout << L"Import error for scan " << absolutePath << std::endl;
				return 1;
			}
			scanCount++;
		}
		if (scanCount == 0)
		{
			std::wcout << L"Failed find any scans in " << inputDir << std::endl;
			return 1;
		}
		results[L"ImportScansCount"] = std::to_wstring(scanCount);
		results[L"ImportScansTime"] = std::to_wstring(timedScope.timedStep(L"ImportScansTime"));

		// Save scans after import
		LSResult::Result saveResult = project->saveRevision();
		if (saveResult != LSResult::Ok)
		{
			std::wcout << L"Failed to save project: " << std::to_wstring(saveResult) << std::endl;
			return 1;
		}
		results[L"ImportSaveTime"] = std::to_wstring(timedScope.timedStep(L"ImportSaveTime"));

		// Process all scans
		for (ref_ptr<LSScan> scan : project->getScans().get())
		{
			LSResult::Result processResult = processScan(*scan, true);
			if (processResult != LSResult::Ok)
			{
				std::wcout << L"Process error for scan " << scan->getName().toWcharStr() << L": " << std::to_wstring(processResult) << std::endl;
				return 1;
			}
		}
		results[L"ProcessScansTime"] = std::to_wstring(timedScope.timedStep(L"ProcessScansTime"));

		// Save scans after processing
		saveResult = project->saveRevision();
		if (saveResult != LSResult::Ok)
		{
			std::wcout << L"Failed to save project: " << std::to_wstring(saveResult) << std::endl;
			return 1;
		}
		results[L"ProcessSaveTime"] = std::to_wstring(timedScope.timedStep(L"ProcessSaveTime"));

		// Get the scan cluster
		ref_ptr<LSObject> root = project->getRootObject();
		ref_ptr<LSObject> cluster = root->getChild(L"Scans");
		if (!cluster)
		{
			std::wcout << L"Could not find Scans cluster!" << std::endl;
			return 1;
		}

		// Register scans with Top View
		bool topView = false;
		LSResult::Result topViewResult = registerCluster(*cluster, RegistrationMethod::RM_TopViewBased, topView);
		if (topViewResult != LSResult::Ok)
		{
			std::wcout << L"Top View Registration error: " << std::to_wstring(topViewResult) << std::endl;
			return 1;
		}
		std::wcout << L"Top View Registration Result: " << topView << std::endl;
		results[L"RegistrationTopViewResult"] = topView ? L"true" : L"false";
		results[L"RegistrationTopViewTime"] = std::to_wstring(timedScope.timedStep(L"RegistrationTopViewTime"));

		// Register scans with C2C
		bool c2c = false;
		LSResult::Result c2cResult = registerCluster(*cluster, RegistrationMethod::RM_CloudToCloud, c2c);
		if (c2cResult != LSResult::Ok)
		{
			std::wcout << L"Cloud 2 Cloud Registration error: " << std::to_wstring(c2cResult) << std::endl;
			return 1;
		}
		std::wcout << L"Cloud 2 Cloud Registration Status: " << c2c << std::endl;
		results[L"RegistrationCloudToCloudResult"] = c2c ? L"true" : L"false";
		results[L"RegistrationCloudToCloudTime"] = std::to_wstring(timedScope.timedStep(L"RegistrationCloudToCloudTime"));

		// Save scans after registration
		saveResult = project->saveRevision();
		if (saveResult != LSResult::Ok)
		{
			std::wcout << L"Failed to save project: " << std::to_wstring(saveResult) << std::endl;
			return 1;
		}
		results[L"RegistrationSaveTime"] = std::to_wstring(timedScope.timedStep(L"RegistrationSaveTime"));

		// Create PPC
		LSResult::Result ppcResult = project->createPointCloud();
		if (ppcResult != LSResult::Ok)
		{
			std::wcout << L"PPC creation error: " << std::to_wstring(ppcResult) << std::endl;
			return 1;
		}
		results[L"ProjectPointCloudTime"] = std::to_wstring(timedScope.timedStep(L"ProjectPointCloudTime"));

		// Save project after PPC creation
		saveResult = project->saveRevision();
		if (saveResult != LSResult::Ok)
		{
			std::wcout << L"Failed to save project: " << std::to_wstring(saveResult) << std::endl;
			return 1;
		}
		results[L"ProjectPointCloudSaveTime"] = std::to_wstring(timedScope.timedStep(L"ProjectPointCloudSaveTime"));

		// Reload the project as workaround for the exporter bug in older API versions (SC-3165)
		// (did not export when there were exceptions attached to the root object)
		// Create new empty project, load empty project, load test project again.
		// We cannot just load the same project again because this will fail
		// silently when the project is already open (due to another API bug).
		{
			std::wstring emptyProjectDir = workingDir + L"empty\\";
			createResult = LSProject::createProject(emptyProjectDir.c_str(), L"empty");
			if (createResult != LSResult::Ok)
			{
				std::wcout << L"Empty Project creation error: " << std::to_wstring(createResult) << std::endl;
				return 1;
			}
			project = nullptr;
			loadResult = LSProject::loadProject(emptyProjectDir.c_str(), project);
			if (loadResult != LSResult::Ok)
			{
				std::wcout << L"Empty project loading error: " << std::to_wstring(loadResult) << std::endl;
				return 1;
			}
			loadResult = context.setCurrentWorkspace(project);
			if (loadResult != LSResult::Ok)
			{
				std::wcout << L"Set empty current workspace error: " << std::to_wstring(loadResult) << std::endl;
				return 1;
			}
			project = nullptr;
			loadResult = LSProject::loadProject(workingDir.c_str(), project);
			if (loadResult != LSResult::Ok)
			{
				std::wcout << L"Project reloading error: " << std::to_wstring(loadResult) << std::endl;
				return 1;
			}
			loadResult = context.setCurrentWorkspace(project);
			if (loadResult != LSResult::Ok)
			{
				std::wcout << L"Set reloaded current workspace error: " << std::to_wstring(loadResult) << std::endl;
				return 1;
			}
			results[L"ProjectReloadTime"] = std::to_wstring(timedScope.timedStep(L"ProjectReloadTime"));
		}

		// Measure the time for multiple unload/load cycles
		results[L"LoadScansCycles"] = std::to_wstring(loadCycleCount);
		for (int64_t i = 0; i < loadCycleCount; i++)
		{
			for (ref_ptr<LSScan> scan : project->getScans().get())
			{
				scan->unloadData();
				LSResult::Result loadResult = scan->loadData();
				if (loadResult != LSResult::Ok)
				{
					std::wcout << L"Failed load scan " << scan->getName().toWcharStr() << L": " << std::to_wstring(loadResult) << std::endl;
					return 1;
				}
			}
		}
		results[L"LoadScansTime"] = std::to_wstring(timedScope.timedStep(L"LoadScansTime"));

		// Export panorama images for all scans
		for (ref_ptr<LSScan> scan : project->getScans().get())
		{
			LSString pathImageFile = workingDir.c_str() + scan->getName() + L".jpg";
			std::wcout << L"Exporting image to file " << pathImageFile.toWcharStr() << std::endl;
			LSResult::Result exportImageResult = scan->exportData(pathImageFile, LSScan::ExportFormat::JPG);
			if (exportImageResult != LSResult::Ok)
			{
				std::wcout << L"Export image error for scan " << scan->getName().toWcharStr() << L": " << std::to_wstring(exportImageResult) << std::endl;
				return 1;
			}
		}
		results[L"ExportScansToJpgTime"] = std::to_wstring(timedScope.timedStep(L"ExportScansToJpgTime"));

		// Export points for all scans
		for (ref_ptr<LSScan> scan : project->getScans().get())
		{
			LSString pathPointFile = workingDir.c_str() + scan->getName() + L".e57";
			std::wcout << L"Exporting point to file " << pathPointFile.toWcharStr() << std::endl;
			LSResult::Result exportPointsResult = scan->exportData(pathPointFile, LSScan::ExportFormat::E57);
			if (exportPointsResult != LSResult::Ok)
			{
				std::wcout << L"Export points error for scan " << scan->getName().toWcharStr() << L": " << std::to_wstring(exportPointsResult) << std::endl;
				return 1;
			}
		}
		results[L"ExportScansToE57Time"] = std::to_wstring(timedScope.timedStep(L"ExportScansToE57Time"));

		// Unload all scan data to free memory for PC rendering
		for (ref_ptr<LSScan> scan : project->getScans().get())
			scan->unloadData();

		// Get first and last scan positions
		Eigen::Vector3d first, last;
		Eigen::Vector4d scanPosLocal = {0, 0, 0, 1};
		std::vector<ref_ptr<LSScan>> scans = project->getScans().get();
		{
			Eigen::Matrix4d localToGlobal = scans[0]->getTransformationToGlobal();
			Eigen::Vector4d scanPosGlobal = localToGlobal * scanPosLocal;
			first = {scanPosGlobal[0], scanPosGlobal[1], scanPosGlobal[2]};
		}
		{
			Eigen::Matrix4d localToGlobal = scans[scans.size() - 1]->getTransformationToGlobal();
			Eigen::Vector4d scanPosGlobal = localToGlobal * scanPosLocal;
			last = {scanPosGlobal[0], scanPosGlobal[1], scanPosGlobal[2]};
		}

		// PC2 Rendering
		results[L"ProjectPointCloudRenderFrameCount"] = std::to_wstring(frameCount);
		ref_ptr<LSPointCloud> ppc = project->getPointCloud();
		if (!ppc)
		{
			std::wcout << L"Unable to get the project point cloud!" << std::endl;
			return 1;
		}
		ref_ptr<LSPointCloudRenderer> renderer = ppc->createPointCloudRenderer();
		if (!renderer)
		{
			std::wcout << L"Unable to create the project point cloud renderer!" << std::endl;
			return 1;
		}
		renderer->setResolution(resolution);
		renderer->setPerspectiveCamera(fovY);
		renderer->setNearFar(zNear, zFar);
		Eigen::Matrix4d projectionMatrix = createProjectionMatrix(fovY, 1.0, zNear, zFar);
		Eigen::Vector3d stepSize = (last - first) / double(frameCount);
		size_t overallPointCount = 0;
		Eigen::Vector3d cameraUp = { 0, 0, 1 };
		Eigen::Vector3d cameraDir = first - last;
		if (cameraDir.norm() > 0.0)
			cameraDir = cameraDir.normalized();
		else
			cameraDir = { 1, 0, 0 };
		for (int i = 0; i < frameCount; ++i)
		{
			LSPointXYZFRGBA* points = nullptr;
			size_t numberOfPoints = 0;
			bool completed = false;
			Eigen::Vector3d cameraPos = first + stepSize * double(i);
			while (!completed)
				renderer->queryVisiblePoints(cameraPos, cameraUp, cameraDir, &points, numberOfPoints, completed);
			overallPointCount += numberOfPoints;
			if (dumpFrames)
			{
				std::wstring filePath = workingDir + L"frame_" + std::to_wstring(i) + L".xyz";
				dumpFrame(projectionMatrix, numberOfPoints, points, filePath);
			}
		}
		results[L"ProjectPointCloudRenderDumpFrames"] = dumpFrames ? L"true" : L"false";
		results[L"ProjectPointCloudRenderTime"] = std::to_wstring(timedScope.timedStep(L"ProjectPointCloudRenderTime"));
		results[L"ProjectPointCloudRenderPoints"] = std::to_wstring(overallPointCount);

		// Measure overall time
		results[L"SceneBenchmarkTime"] = std::to_wstring(timedScope.overallTime(L"SceneBenchmarkTime"));
	}

	// Check size of the resulting project folder
	size_t projectFolderSize = getFolderSize(workingDir);
	std::wcout << L"Generated Project Folder Size in Bytes: " << projectFolderSize << std::endl;
	if (teamCity)
		std::wcout << L"##teamcity[buildStatisticValue key='ProjectFolderSize' value='" << projectFolderSize << L"']" << std::endl;
	results[L"ProjectFolderSize"] = std::to_wstring(projectFolderSize);

	// Write results to JSON file
	std::wstring fileName = std::wstring(L"results_") + results[L"ApiVersion"] + L"_" + getTimeStampString() + L".json";
	std::wstring filePath = outputDir + fileName;
	if (exportResults(results, filePath))
		std::wcout << "Saved JSON result to " << filePath << std::endl;
	else
	{
		std::wcout << "Failed to write JSON result to " << filePath << std::endl;
		return 1;
	}

	return 0;
}
