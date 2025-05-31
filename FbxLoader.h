#pragma once

#ifdef FBXSDK_SHARED

namespace fbxsdk
{
    class FbxManager;
    class FbxScene;
    class FbxImporter;
}
namespace FBStoreA
{
    public class FbxLoader
    {
    public:
        fbxsdk::FbxManager* _manager = nullptr;
        fbxsdk::FbxScene* _scene = nullptr;
        fbxsdk::FbxImporter* _importer = nullptr;

        FbxLoader();
        ~FbxLoader();

        bool importFile(const char* fileName);
    };
}
#else

struct ufbx_scene;

namespace FBStoreA
{
    public class FbxLoader
    {
    public:
        ufbx_scene* _scene = nullptr;

        ~FbxLoader();
        bool importFile(const char* fileName);
    };
}

#endif

