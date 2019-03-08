/*
    FbxLoader.cpp

    Defines routines for loading FbxScene and converting it to GraphicsDemo project objects.

    Dependencies:
        FBX SDK 2019 - for FbxScene importing
        stb_image.h - for texture file loading

    Reference:
        FbxSdk 'ViewScene' Demo

    Todo:
        Currently texture objects are saved in FbxNode UserDataPtr. Move it to somewhere else.

    Changes:
        190303 - Removed dependency on glm
*/
#include "Common.h"
#include "SystemComponent.h"
#include "Errors.h"
#include "Object.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "stb_image.h"

namespace
{
    const int TRIANGLE_VERTEX_COUNT = 3;

    void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
    {
        //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
        pManager = FbxManager::Create();
        if (!pManager)
        {
            FBXSDK_printf("Error: Unable to create FBX Manager!\n");
            exit(1);
        }
        else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

        //Create an IOSettings object. This object holds all import/export settings.
        FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
        pManager->SetIOSettings(ios);

        //Load plugins from the executable directory (optional)
        FbxString lPath = FbxGetApplicationDirectory();
        pManager->LoadPluginsDirectory(lPath.Buffer());

        //Create an FBX scene. This object holds most objects imported/exported from/to files.
        pScene = FbxScene::Create(pManager, "My Scene");
        if (!pScene)
        {
            FBXSDK_printf("Error: Unable to create FBX scene!\n");
            exit(1);
        }
    }

    // Find all the cameras under this node recursively.
    void FillCameraArrayRecursive(FbxNode* pNode, FbxArray<FbxNode*>& pCameraArray)
    {
        if (pNode)
        {
            if (pNode->GetNodeAttribute())
            {
                if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera)
                {
                    pCameraArray.Add(pNode);
                }
            }

            const int lCount = pNode->GetChildCount();
            for (int i = 0; i < lCount; i++)
            {
                FillCameraArrayRecursive(pNode->GetChild(i), pCameraArray);
            }
        }
    }

    // Find all the cameras in this scene.
    void FillCameraArray(FbxScene* pScene, FbxArray<FbxNode*>& pCameraArray)
    {
        pCameraArray.Clear();

        FillCameraArrayRecursive(pScene->GetRootNode(), pCameraArray);
    }

    // Find all poses in this scene.
    void FillPoseArray(FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray)
    {
        const int lPoseCount = pScene->GetPoseCount();

        for (int i = 0; i < lPoseCount; ++i)
        {
            pPoseArray.Add(pScene->GetPose(i));
        }
    }

    void LoadRecursive(FbxNode* pNode, std::vector<std::shared_ptr<Object>>& objectStack, int depth = 0)
    {
        FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
        if (pAttribute)
        {
            std::map<int, std::vector<int>> submeshes;
            if (pAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
            {
                std::shared_ptr<Object> pObject(new Object());

                // Bake material and hook as user data.
                const int materialCount = pNode->GetMaterialCount();
                for (int mi = 0; mi < materialCount; ++mi)
                {
                    FbxSurfaceMaterial * pMaterial = pNode->GetMaterial(mi);
                    if (pMaterial)
                    {
                        std::shared_ptr<Material> pMyMaterial(new Material());
                        pMyMaterial->Initialize(pMaterial);
                        pObject->AddMaterial(pMyMaterial);
                    }
                }

                FbxMesh* pMesh = pNode->GetMesh();
                int polygonCount = pMesh->GetPolygonCount();
                int elementMaterialCount = pMesh->GetElementMaterialCount();
                FbxLayerElementArrayTemplate<int>* materialIndices;
                pMesh->GetMaterialIndices(&materialIndices);
                FbxGeometryElement::EMappingMode mappingMode = pMesh->GetElementMaterial()->GetMappingMode();
                FbxGeometryElement::EReferenceMode referenceMode = pMesh->GetElementMaterial()->GetReferenceMode();
                if (materialIndices && mappingMode == FbxGeometryElement::eByPolygon)
                {
                    if (materialIndices->GetCount() == polygonCount)
                    {
                        // Count the faces of each material
                        for (int pi = 0; pi < polygonCount; ++pi)
                        {
                            const int materialIndex = materialIndices->GetAt(pi);
                            submeshes[materialIndex].push_back(pi);
                        }
                    }
                }

                FbxStringList lUVNames;
                pMesh->GetUVSetNames(lUVNames);
                const char * lUVName = NULL;
                const int uvCount = lUVNames.GetCount();
                if (uvCount)
                {
                    lUVName = lUVNames[0];
                }

                std::vector<float> positions;
                std::vector<float> normals;
                std::vector<float> uvs;

                for (std::map<int, std::vector<int>>::iterator it = submeshes.begin(); it != submeshes.end(); ++it)
                {
                    std::vector<int> subMeshPolygonsIndice = it->second;
                    for (int i : subMeshPolygonsIndice)
                    {
                        for (int j = 0; j < TRIANGLE_VERTEX_COUNT; ++j)
                        {
                            int controlPointIndex = pMesh->GetPolygonVertex(i, j);
                            FbxVector4 controlPoint = pMesh->GetControlPointAt(controlPointIndex);
                            positions.push_back(static_cast<float>(controlPoint[0]));
                            positions.push_back(static_cast<float>(controlPoint[1]));
                            positions.push_back(static_cast<float>(controlPoint[2]));

                            FbxVector4 fbxNormal;
                            pMesh->GetPolygonVertexNormal(i, j, fbxNormal);
                            normals.push_back(static_cast<float>(fbxNormal[0]));
                            normals.push_back(static_cast<float>(fbxNormal[1]));
                            normals.push_back(static_cast<float>(fbxNormal[2]));

                            FbxVector2 fbxUv;
                            bool unmapped;
                            pMesh->GetPolygonVertexUV(i, j, lUVName, fbxUv, unmapped);
                            uvs.push_back(static_cast<float>(fbxUv[0]));
                            uvs.push_back(static_cast<float>(fbxUv[1]));
                        }
                    }
                }

                std::shared_ptr<Mesh> pMyMesh(new Mesh());
                if (!submeshes.empty())
                {
                    int begin = 0;
                    for (std::map<int, std::vector<int>>::iterator it = submeshes.begin(); it != submeshes.end(); ++it)
                    {
                        Mesh::SubMesh subMesh;
                        subMesh.begin = begin;
                        subMesh.vertCount = static_cast<int>(it->second.size() * TRIANGLE_VERTEX_COUNT);
                        pMyMesh->AddSubMesh(subMesh);
                        begin += subMesh.vertCount;
                    }
                }
                AttributeArray aaPositions(3, GL_FLOAT, 0, 0);
                aaPositions.Fill(positions.size() * sizeof(positions[0]), positions.data());
                pMyMesh->AddAttributeArray(aaPositions);

                AttributeArray aaUvs(2, GL_FLOAT, 0, 0);
                aaUvs.Fill(uvs.size() * sizeof(uvs[0]), uvs.data());
                pMyMesh->AddAttributeArray(aaUvs);

                AttributeArray aaNormals(3, GL_FLOAT, 0, 0);
                aaNormals.Fill(normals.size() * sizeof(normals[0]), normals.data());
                pMyMesh->AddAttributeArray(aaNormals);

                pObject->SetMesh(pMyMesh);
                objectStack.push_back(pObject);
            }
        }

        for (int i = 0; i < pNode->GetChildCount(); ++i)
            LoadRecursive(pNode->GetChild(i), objectStack, depth + 1);
    }

    bool LoadTextureFromFile(const FbxString & pFilePath, unsigned int & pTextureObject)
    {
        int x, y, n;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(pFilePath.Buffer(), &x, &y, &n, 0);


        // Transfer the texture date into GPU
        glGenTextures(1, &pTextureObject);
        glBindTexture(GL_TEXTURE_2D, pTextureObject);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if (n == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if (n == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "Not supported" << std::endl;
        }

        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(data);

        return true;
    }
}

void LoadFbxSdkObject(const char* const filename, std::vector<std::shared_ptr<Object>>& objectStack)
{
    FbxManager* pManager;
    FbxScene* pScene;
    InitializeSdkObjects(pManager, pScene);
    FbxImporter* pImporter;
    FbxArray<FbxString*> mAnimStackNameArray;
    FbxArray<FbxNode*> mCameraArray;
    FbxArray<FbxPose*> mPoseArray;
    FbxTime mFrameTime;

    if (pManager)
    {
        // Create the importer.
        int lFileFormat = -1;
        pImporter = FbxImporter::Create(pManager, "");
        if (!pManager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat))
        {
            // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
            lFileFormat = pManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
        }

        // Initialize the importer by providing a filename.
        if (pImporter->Initialize(filename, lFileFormat))
        {
            if (pImporter->Import(pScene))
            {
                // Convert Axis System to what is used in this example, if needed
                FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
                FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
                if (SceneAxisSystem != OurAxisSystem)
                {
                    OurAxisSystem.ConvertScene(pScene);
                }

                // Convert Unit System to what is used in this example, if needed
                FbxSystemUnit SceneSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();
                if (SceneSystemUnit.GetScaleFactor() != 1.0)
                {
                    //The unit in this example is centimeter.
                    FbxSystemUnit::cm.ConvertScene(pScene);
                }

                // Get the list of all the animation stack.
                pScene->FillAnimStackNameArray(mAnimStackNameArray);

                // Get the list of all the cameras in the scene.
                FillCameraArray(pScene, mCameraArray);

                // Convert mesh, NURBS and patch into triangle mesh
                FbxGeometryConverter lGeomConverter(pManager);
                lGeomConverter.Triangulate(pScene, /*replace*/true);

                // Load the textures into GPU, only for file texture now
                const int textureCount = pScene->GetTextureCount();
                for (int ti = 0; ti < textureCount; ++ti)
                {
                    FbxTexture * pTexture = pScene->GetTexture(ti);
                    FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(pTexture);
                    if (lFileTexture && !lFileTexture->GetUserDataPtr())
                    {
                        // Try to load the texture from absolute path
                        const FbxString lFileName = lFileTexture->GetFileName();

                        GLuint lTextureObject = 0;
                        bool lStatus = LoadTextureFromFile(lFileName, lTextureObject);

                        const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(lFileName);
                        const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);
                        if (!lStatus)
                        {
                            // Load texture from relative file name (relative to FBX file)
                            const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lFileTexture->GetRelativeFileName());
                            lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
                        }

                        if (!lStatus)
                        {
                            // Load texture from file name only (relative to FBX file)
                            const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
                            const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
                            lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
                        }

                        if (!lStatus)
                        {
                            FBXSDK_printf("Failed to load texture file: %s\n", lFileName.Buffer());
                            continue;
                        }

                        if (lStatus)
                        {
                            GLuint * lTextureName = new GLuint(lTextureObject);
                            lFileTexture->SetUserDataPtr(lTextureName);
                        }
                    }
                }
                // Bake the scene for one frame
                LoadRecursive(pScene->GetRootNode(), objectStack);

                //// Convert any .PC2 point cache data into the .MC format for 
                //// vertex cache deformer playback.
                //PreparePointCacheData(mScene, mCache_Start, mCache_Stop);

                // Get the list of pose in the scene
                FillPoseArray(pScene, mPoseArray);

                // Initialize the frame period.
                mFrameTime.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
            }
        }
    }
}