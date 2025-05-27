#include <fbxsdk.h>

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