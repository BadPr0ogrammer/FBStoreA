#pragma unmanaged

#ifdef FBXSDK_SHARED

#include <fbxsdk.h>
#include <fbxsdk/core/math/fbxaffinematrix.h>

#include "FbxLoader.h"

#include <iostream>

using namespace std;
using namespace fbxsdk;

namespace FBStoreA
{
	FbxLoader::FbxLoader()
	{
		_manager = FbxManager::Create();
		if (!_manager)
		{
			cout << "FBX SDK Manager is null!\n";
			return;
		}
		FbxIOSettings* ios = FbxIOSettings::Create(_manager, IOSROOT);
		if (!ios)
		{
			cout << "FBX SDK Manager: IO Settings is null!\n";
			return;
		}
		_manager->SetIOSettings(ios);
		_scene = FbxScene::Create(_manager, "MyScene");
		if (!_scene)
		{
			cout << "FBX SDK Manager: Create scene failed!\n";
			return;
		}
		_importer = FbxImporter::Create(_manager, "");
		if (!_importer)
		{
			cout << "FBX SDK Manager: Importer is null!\n";
			return;
		}
	}

	FbxLoader::~FbxLoader()
	{
		if (_manager)
			_manager->Destroy();
		if (_scene)
			_scene->Destroy();
		if (_importer)
			_importer->Destroy();
	}

	bool FbxLoader::importFile(const char* fileName)
	{
		if (!_importer->Initialize(fileName, -1, _manager->GetIOSettings()))
		{
			cout << "FBX SDK Importer: Initialize failed!\n";
			return false;
		}
		if (!_importer->IsFBX())
		{
			cout << "FBX SDK Importer: Is DBX failed!\n";
			return false;
		}
		if (!_importer->Import(_scene))
		{
			cout << "FBX SDK Importer: Import Failed!\n";
			return false;
		}
		return true;
	}

}
#else

#include "ufbx.h"

#include "FbxLoader.h"

#include <cstring>
#include <iostream>

using namespace std;

namespace FBStoreA
{
	FbxLoader::~FbxLoader()
	{
		if (_scene)
			ufbx_free_scene(_scene);
	}

	bool FbxLoader::importFile(const char* fileName)
	{
		ufbx_load_opts opts;
		std::memset(&opts, 0, sizeof(opts));
		opts.target_axes = ufbx_axes_right_handed_y_up;
		opts.target_unit_meters = 1.0f;

		ufbx_error error;
		ufbx_scene* scene = ufbx_load_file(fileName, &opts, &error);
		if (!scene) 
		{
			cout << "FBX scene load file failed! " << error.description.data << endl;
			return false;
		}
		_scene = scene;
		return true;
	}

}
#endif
