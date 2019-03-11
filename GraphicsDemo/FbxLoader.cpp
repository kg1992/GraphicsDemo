/*
    FbxLoader.cpp

    Defines routines for loading FbxScene and converting it to GraphicsDemo project objects.

    Dependencies:
        FBX SDK 2019 - for FbxScene importing
        stb_image.h - for texture file loading
        glm - for vector, matrix representation

    Reference:
        FbxSdk 'ViewScene' Demo
        Help Page - http://help.autodesk.com/view/FBX/2019/ENU/?guid=FBX_Developer_Help_welcome_to_the_fbx_sdk_what_new_fbx_sdk_2019_html
        FBX SDK API Reference - http://help.autodesk.com/cloudhelp/2018/ENU/FBX-Developer-Help/cpp_ref/annotated.html
            FbxSkeleton - http://help.autodesk.com/cloudhelp/2018/ENU/FBX-Developer-Help/cpp_ref/class_fbx_skeleton.html
            FbxDeformer - http://help.autodesk.com/cloudhelp/2018/ENU/FBX-Developer-Help/cpp_ref/class_fbx_deformer.html
            FbxSkin - http://help.autodesk.com/cloudhelp/2018/ENU/FBX-Developer-Help/cpp_ref/class_fbx_skin.html
            FbxCluster - http://help.autodesk.com/cloudhelp/2018/ENU/FBX-Developer-Help/cpp_ref/class_fbx_cluster.html

    Changes:
        190303 - Removed dependency on glm
        190311 - Animation loading support

    Todo:
        Figure out what FbxCluster::ELinkMode does in FbxCluster
        Figure out 'correct' way of finding bind pose transform
        Add legacy animation support
        Optimize FbxLoader::LoadAnimation method

*/
#include "Common.h"
#include "FbxLoader.h"
#include "SystemComponent.h"
#include "Errors.h"
#include "Object.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "stb_image.h"
#include "Skeleton.h"
#include "Bone.h"
#include "Animation.h"
#include "Animator.h"

namespace
{
    const char* const Invalid = "[INVALID]";

    enum : int
    {
        TRIANGLE_VERTEX_COUNT = 3,
        COMPONENT_COUNT_POSITION = 3,
        COMPONENT_COUNT_UV = 2,
        COMPONENT_COUNT_NORMAL = 3,
        COMPONENT_COUNT_BONE = 3,
        COMPONENT_COUNT_WEIGHT = 3
    };
    
    void TraverseFbxNodeDepthFirst(FbxNode* pNode, std::function<void(FbxNode* node, int depth)> delegator, int depth = 0)
    {
        const int ChildCount = pNode->GetChildCount();

        delegator(pNode, depth);

        for (int i = 0; i < ChildCount; ++i)
        {
            TraverseFbxNodeDepthFirst(pNode->GetChild(i), delegator, depth + 1);
        }
    }

    struct BoneWeight
    {
        BoneWeight(int boneIndex, float weight)
            : boneIndex(boneIndex)
            , weight(weight)
        {
        }

        int boneIndex;
        float weight;
    };

    // 
    // Dependency : stb_image.h 
    // Loads texture file and generates 2D OpenGL texture object
    // 
    bool LoadTextureFromFile(const FbxString & pFilePath, unsigned int & pTextureObject)
    {
        int imageWidth, imageHeight, channelCount;

        // OpenGL Texture pixel data must start from bottom-left whereas actual file data usually has data starting from top-left
        stbi_set_flip_vertically_on_load(true);

        unsigned char* data = stbi_load(pFilePath.Buffer(), &imageWidth, &imageHeight, &channelCount, 0);

        // Transfer the texture date into GPU
        glGenTextures(1, &pTextureObject);
        GET_AND_HANDLE_GL_ERROR();
        glBindTexture(GL_TEXTURE_2D, pTextureObject);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        GET_AND_HANDLE_GL_ERROR();
        // RGB
        if (channelCount == 3)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        // RGBA
        else if (channelCount == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            // GRAY -> channel count == 1
            // GRAY ALPHA -> channel count == 2
            std::cout << "Not supported" << std::endl;
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        GET_AND_HANDLE_GL_ERROR();

        stbi_image_free(data);

        return true;
    }

    void LoadRecursive(FbxNode* pFbxNode)
    {
        
    }

    const char* ToString(FbxNodeAttribute::EType type)
    {
        switch (type)
        {
        case FbxNodeAttribute::EType::eUnknown: return "eUnknown";
        case FbxNodeAttribute::EType::eNull: return "eNull";
        case FbxNodeAttribute::EType::eMarker: return "eMarker";
        case FbxNodeAttribute::EType::eSkeleton: return "eSkeleton";
        case FbxNodeAttribute::EType::eMesh: return "eMesh";
        case FbxNodeAttribute::EType::eNurbs: return "eNurbs";
        case FbxNodeAttribute::EType::ePatch: return "ePatch";
        case FbxNodeAttribute::EType::eCamera: return "eCamera";
        case FbxNodeAttribute::EType::eCameraStereo: return "eCameraStereo";
        case FbxNodeAttribute::EType::eCameraSwitcher: return "eCameraSwitcher";
        case FbxNodeAttribute::EType::eLight: return "eLight";
        case FbxNodeAttribute::EType::eOpticalReference: return "eOpticalReference";
        case FbxNodeAttribute::EType::eOpticalMarker: return "eOpticalMarker";
        case FbxNodeAttribute::EType::eNurbsCurve: return "eNurbsCurve";
        case FbxNodeAttribute::EType::eTrimNurbsSurface: return "eTrimNurbsSurface";
        case FbxNodeAttribute::EType::eBoundary: return "eBoundary";
        case FbxNodeAttribute::EType::eNurbsSurface: return "eNurbsSurface";
        case FbxNodeAttribute::EType::eShape: return "eShape";
        case FbxNodeAttribute::EType::eLODGroup: return "eLODGroup";
        case FbxNodeAttribute::EType::eSubDiv: return "eSubDiv";
        case FbxNodeAttribute::EType::eCachedEffect: return "eCachedEffect";
        case FbxNodeAttribute::EType::eLine: return "eLine";
        }
        return Invalid;
    }

    const char* ToString(FbxDeformer::EDeformerType type)
    {
        switch (type)
        {
        case FbxDeformer::EDeformerType::eUnknown: return "eUnknown";           // Unknown deformer type
        case FbxDeformer::EDeformerType::eSkin: return "eSkin";                 // Type FbxSkin
        case FbxDeformer::EDeformerType::eBlendShape: return "eBlendShape";     // Type FbxBlendShape
        case FbxDeformer::EDeformerType::eVertexCache: return "eVertexCache";   // Type FbxVertexCacheDeformer
        }
        return Invalid;
    }

    const char* ToString(FbxSkeleton::EType type)
    {
        switch (type)
        {
        case FbxSkeleton::EType::eRoot: return "eRoot";             // First element of a chain.
        case FbxSkeleton::EType::eLimb: return "eLimb";             // Chain element.
        case FbxSkeleton::EType::eLimbNode: return "eLimbNode";     // Chain element.
        case FbxSkeleton::EType::eEffector: return "eEffector";     // Last element of a chain.
        }
        return Invalid;
    }

    const char* ToString(FbxCluster::ELinkMode linkMode)
    {
        switch (linkMode)
        {
        case FbxCluster::ELinkMode::eNormalize:return "eNormalize";
        case FbxCluster::ELinkMode::eAdditive:return "eAdditive";
        case FbxCluster::ELinkMode::eTotalOne:return "eTotalOne";
        }
        return Invalid;
    }
    
    std::ostream& operator<<(std::ostream& os, const FbxColor& color)
    {
        return os << "FbxColor("
            << color.mRed << ','
            << color.mGreen << ','
            << color.mBlue << ','
            << color.mAlpha << ")";
    }

    std::ostream& operator<<(std::ostream& os, const glm::mat4& rhs)
    {
        return
        os << std::setw(4) << std::setprecision(2) << std::fixed
           << rhs[0][0] << ' ' << rhs[1][0] << ' ' << rhs[2][0] << ' ' << rhs[3][0] << std::endl
           << rhs[0][1] << ' ' << rhs[1][1] << ' ' << rhs[2][1] << ' ' << rhs[3][1] << std::endl
           << rhs[0][2] << ' ' << rhs[1][2] << ' ' << rhs[2][2] << ' ' << rhs[3][2] << std::endl
           << rhs[0][3] << ' ' << rhs[1][3] << ' ' << rhs[2][3] << ' ' << rhs[3][3];
    }

    void FbxAMatrixToGlmMat4(const FbxAMatrix& src, glm::mat4& dest)
    {
        std::transform(static_cast<const double*>(src),
            static_cast<const double*>(src) + 16,
            &dest[0][0],
            [](double d) { return static_cast<float>(d); });
    }

    glm::mat4 ToMat4(const FbxAMatrix& src)
    {
        glm::mat4 dest;
        std::transform(static_cast<const double*>(src),
            static_cast<const double*>(src) + 16,
            &dest[0][0],
            [](double d) { return static_cast<float>(d); });
        return dest;
    }

    glm::vec3 ToVec3(const FbxDouble3& src)
    {
        return glm::vec3(static_cast<float>(src[0]),
            static_cast<float>(src[1]),
            static_cast<float>(src[2]));
    }

}

FbxLoader::FbxLoader()
    : m_pFbxManager(nullptr)
    , m_pScene(nullptr)
{
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    m_pFbxManager = FbxManager::Create();
    if (!m_pFbxManager)
    {
        std::cerr << "Error: Unable to create FBX Manager!" << std::endl;
    }
    else
    {
        std::cout << "Autodesk FBX SDK version : " << m_pFbxManager->GetVersion() << std::endl;
    }

    //Create an IOSettings object. This object holds all import/export settings.
    FbxIOSettings* ios = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
    m_pFbxManager->SetIOSettings(ios);

    //Load plugins from the executable directory (optional)
    FbxString lPath = FbxGetApplicationDirectory();
    m_pFbxManager->LoadPluginsDirectory(lPath.Buffer());
}

FbxLoader::~FbxLoader()
{
    if (m_pScene)
    {
        m_pScene->Destroy();
        m_pScene = nullptr;
    }

    if (m_pFbxManager)
    {
        m_pFbxManager->Destroy();
        m_pFbxManager = nullptr;
    }
}

void FbxLoader::Load(const char* filename)
{
    Clear();

    //Create an FBX scene. This object holds most objects imported/exported from/to files.
    m_pScene = FbxScene::Create(m_pFbxManager, "");
    if (!m_pScene)
    {
        std::cerr << "Error: Unable to create FBX scene!" << std::endl;
    }

    FbxImporter* pImporter;

    // Create the importer.
    int fileFormat = -1;
    pImporter = FbxImporter::Create(m_pFbxManager, "");
    if (!m_pFbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, fileFormat))
    {
        // Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
        fileFormat = m_pFbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
    }

    // Initialize the importer by providing a filename.
    bool success = pImporter->Initialize(filename, fileFormat);
    assert(success);
    if (!success)
    {
        return;
    }

    success = pImporter->Import(m_pScene);
    assert(success);
    if (!success)
    {
        pImporter->Destroy();
        m_pScene = nullptr;
        return;
    }

    // Convert Axis System to OpenGL
    FbxAxisSystem SceneAxisSystem = m_pScene->GetGlobalSettings().GetAxisSystem();
    FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    if (SceneAxisSystem != OurAxisSystem)
    {
        OurAxisSystem.ConvertScene(m_pScene);
    }

    // Convert Unit System to what is used in this example, if needed
    FbxSystemUnit SceneSystemUnit = m_pScene->GetGlobalSettings().GetSystemUnit();
    if (SceneSystemUnit.GetScaleFactor() != 1.0)
    {
        //The unit in this example is centimeter.
        FbxSystemUnit::cm.ConvertScene(m_pScene);
    }

    // Takes Too much time....
    // Convert mesh, NURBS and patch into triangle mesh
    FbxGeometryConverter lGeomConverter(m_pFbxManager);
    lGeomConverter.Triangulate(m_pScene, /*replace*/true);

    LoadTextures();

    LoadMaterial();

    LoadSkeleton();

    LoadMesh();

    LoadAnimation(pImporter);

    std::shared_ptr<Object> pObject(new Object);

    for (auto& material : m_materials)
    {
        pObject->AddMaterial(material);
    }

    for (int i = 0; i < m_meshes.size(); ++i)
    {
        pObject->AddMesh(m_meshes[i]);
    }

    if (m_pSkeleton)
    {
        pObject->SetSkeleton(m_pSkeleton);
    }

    if (m_pAnimator)
    {
        pObject->SetAnimator(m_pAnimator);
    }

    m_objects.push_back(pObject);

    pImporter->Destroy();
}

void FbxLoader::Clear()
{
    if (m_pScene)
    {
        m_pScene->Destroy();
        m_pScene = nullptr;
    }

    m_textures.clear();
    m_materials.clear();
    m_objects.clear();
    m_meshes.clear();
    m_pSkeleton.reset();
    m_animations.clear();
    m_pAnimator.reset();
}

void FbxLoader::LoadTextures()
{
    assert(m_pScene != nullptr);

    const int TextureCount = m_pScene->GetTextureCount();
    for (int ti = 0; ti < TextureCount; ++ti)
    {
        FbxTexture * pTexture = m_pScene->GetTexture(ti);
        FbxFileTexture * pFileTexture = FbxCast<FbxFileTexture>(pTexture);
        if (pFileTexture)
        {
            const FbxString Filename = pFileTexture->GetFileName();
            const FbxString AbsoluteFileName = FbxPathUtils::Resolve(Filename);
            const FbxString AbsoluteFolderName = FbxPathUtils::GetFolderName(AbsoluteFileName);

            FbxString resolvedName = Filename;

#ifndef NDEBUG
            std::cout << "Loading Texture at index " << ti << ": " << Filename << std::endl;
#endif
            GLuint textureObject = 0;
            bool success = LoadTextureFromFile(resolvedName, textureObject);

            if (!success)
            {
#ifndef NDEBUG
                std::cout << "Load texture from relative file name (relative to FBX file)" << std::endl;
#endif
                resolvedName = FbxPathUtils::Bind(AbsoluteFolderName, pFileTexture->GetRelativeFileName());
                success = LoadTextureFromFile(resolvedName, textureObject);
            }

            if (!success)
            {
#ifndef NDEBUG
                std::cout << "Load texture from file name only (relative to FBX file)" << std::endl;
#endif
                const FbxString textureFileName = FbxPathUtils::GetFileName(Filename);
                const FbxString loadName = FbxPathUtils::Bind(AbsoluteFolderName, textureFileName);
                success = LoadTextureFromFile(loadName, textureObject);
            }

            if (!success)
            {
                std::cerr << "Failed to load texture file: " << Filename << std::endl;
                continue;
            }
            
            assert(success);
            if (success)
            {
                TextureCache textureCache(std::string(Filename), std::string(resolvedName.Buffer()), textureObject);
                m_textures.push_back(textureCache);
            }
        }
    }
}

//
// Loads all material in the scene
// FbxSurfacecMaterial - http://help.autodesk.com/cloudhelp/2018/ENU/FBX-Developer-Help/cpp_ref/class_fbx_surface_material.html
// Inheritance:
//     FbxEmitter < FbxObject < FbxSurfaceMaterial < FbxSurfaceLambert < FbxSurfacePhong
// 
void FbxLoader::LoadMaterial()
{
    assert(m_pScene != nullptr);
    
    const int MaterialCount = m_pScene->GetMaterialCount();
    for (int i = 0; i < MaterialCount; ++i)
    {
#ifndef NDEBUG
        std::cout << "Loading Material at " << i << std::endl;
#endif

        FbxSurfaceMaterial* pFbxMaterial = m_pScene->GetMaterial(i);

        std::shared_ptr<Material> pMaterial(new Material());

        InitializeMaterial(pFbxMaterial, &*pMaterial);

        m_materials.push_back(pMaterial);
    }
}

void FbxLoader::LoadSkeleton()
{
    assert(m_pScene != nullptr);

    FbxNode* pRootNode = m_pScene->GetRootNode();

    m_pSkeleton.reset(new Skeleton());

    TraverseFbxNodeDepthFirst(pRootNode, [this](FbxNode* pNode, int depth)
    {
        FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
        FbxNodeAttribute::EType attributeType;
        
        // Attribute can be absent
        if (!pAttribute)
            return;

        attributeType = pAttribute->GetAttributeType();

        if (attributeType == FbxNodeAttribute::EType::eSkeleton)
        {
            FbxSkeleton* pSkeleton = pNode->GetSkeleton();

            FbxNode* pParent = pNode->GetParent();

            Bone bone;

            bone.SetName(pNode->GetName());

            int parentIndex = m_pSkeleton->FindBoneIndex(pParent->GetName());

            bone.SetParent(parentIndex);

            m_pSkeleton->AddBone(bone);
        }
    });
}

void FbxLoader::LoadMesh()
{
    assert(m_pScene != nullptr);

    FbxNode* pRootNode = m_pScene->GetRootNode();
    TraverseFbxNodeDepthFirst(pRootNode, [this](FbxNode* pNode, int depth)
    {
        FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();

        // Attribute can be absent.
        if (!pAttribute)
            return;

        // Only interested in mesh node
        if (pAttribute->GetAttributeType() != FbxNodeAttribute::eMesh)
            return;

        // Sub meshes. One submesh per material.
        std::map<int, std::vector<int>> submeshes;


        const int MaterialCount = pNode->GetMaterialCount();
        for (int mi = 0; mi < MaterialCount; ++mi)
        {
            FbxSurfaceMaterial * pMaterial = pNode->GetMaterial(mi);
            if (pMaterial)
            {
                std::cout << pMaterial->GetName() << std::endl;
            }
        }

        FbxMesh* pMesh = pNode->GetMesh();
        const int PolygonCount = pMesh->GetPolygonCount();
        const int ElementMaterialCount = pMesh->GetElementMaterialCount();
        FbxLayerElementArrayTemplate<int>* materialIndices;
        pMesh->GetMaterialIndices(&materialIndices);
        FbxGeometryElement::EMappingMode mappingMode = pMesh->GetElementMaterial()->GetMappingMode();
        FbxGeometryElement::EReferenceMode referenceMode = pMesh->GetElementMaterial()->GetReferenceMode();
        if (materialIndices)
        {
            if (mappingMode == FbxGeometryElement::eByPolygon)
            {
                assert(materialIndices->GetCount() == PolygonCount);
                // Count the faces of each material
                for (int pi = 0; pi < PolygonCount; ++pi)
                {
                    const int materialIndex = materialIndices->GetAt(pi);
                    submeshes[materialIndex].push_back(pi);
                }
            }
            else if (mappingMode == FbxGeometryElement::EMappingMode::eAllSame)
            {
                for (int pi = 0; pi < PolygonCount; ++pi)
                {
                    submeshes[0].push_back(pi);
                }
            }
        }

        const int DeformerCount = pMesh->GetDeformerCount();
#ifndef NDEBUG
        std::cout << "....Loading Deformer...."
            << "Deformer Count : " << DeformerCount << std::endl;
#endif
        std::vector<std::vector<BoneWeight>> controlPointIndexToBoneWeights(pMesh->GetControlPointsCount());
        for (int i = 0; i < DeformerCount; ++i)
        {
            FbxDeformer* pDeformer = pMesh->GetDeformer(i);
            FbxDeformer::EDeformerType type = pDeformer->GetDeformerType();
#ifndef NDEBUG
            std::cout << " Deformer Type : " << ToString(type) << std::endl;
#endif
            if (type == FbxDeformer::EDeformerType::eSkin || type == FbxDeformer::EDeformerType::eVertexCache)
            {
                FbxSkin* pSkin = FbxCast<FbxSkin>(pDeformer);
                assert(pSkin != nullptr);
                if (pSkin)
                {
                    int ClusterCount = pSkin->GetClusterCount();

#ifndef NDEBUG
                    std::cout << " Cluster Count : " << ClusterCount << std::endl;
#endif
                    for (int j = 0; j < ClusterCount; ++j)
                    {
                        FbxCluster* pCluster = pSkin->GetCluster(j);

                        FbxNode* pLinkNode = pCluster->GetLink();
                        assert(pLinkNode != nullptr);

                        std::string linkNodeName = pLinkNode->GetName();
                        int boneIndex = m_pSkeleton->FindBoneIndex(linkNodeName);
                        if (boneIndex == Skeleton::DUMMY_PARENT_NODE_INDEX)
                            continue;

                        Bone& bone = m_pSkeleton->GetBoneByIndex(boneIndex);
                        FbxAMatrix clusterMatrix;
                        pCluster->GetTransformMatrix(clusterMatrix);
                        FbxAMatrix clusterLinkMatrix;
                        pCluster->GetTransformLinkMatrix(clusterLinkMatrix);

                        bone.SetTransform(ToMat4(clusterMatrix));
                        bone.SetLinkTransform(ToMat4(clusterLinkMatrix));

                        int* pIndices = pCluster->GetControlPointIndices();
                        double* pWeights = pCluster->GetControlPointWeights();

                        const int ControlPointIndicesCount = pCluster->GetControlPointIndicesCount();
                        for (int k = 0; k < ControlPointIndicesCount; ++k)
                        {
                            int controlPointIndex = pIndices[k];
                            double weight = pWeights[k];
                            std::vector<BoneWeight>& boneWeights = controlPointIndexToBoneWeights[controlPointIndex];
                            boneWeights.push_back(BoneWeight(boneIndex, static_cast<float>(weight)));
                        }
                    }
                }
            }
        }

        FbxStringList uvNames;
        pMesh->GetUVSetNames(uvNames);
        const char * pUvName = NULL;
        const int uvCount = uvNames.GetCount();
        if (uvCount)
        {
            pUvName = uvNames[0];
        }

        const int PolygonVertexCount = pMesh->GetPolygonVertexCount();
        std::vector<float> positions;
        positions.reserve(PolygonVertexCount * COMPONENT_COUNT_POSITION);
        std::vector<float> normals;
        normals.reserve(PolygonVertexCount * COMPONENT_COUNT_NORMAL);
        std::vector<float> uvs;
        uvs.reserve(PolygonVertexCount * COMPONENT_COUNT_UV);
        std::vector<int> boneIndices(PolygonVertexCount * COMPONENT_COUNT_BONE);
        std::vector<float> weights(PolygonVertexCount * COMPONENT_COUNT_WEIGHT);

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
                    pMesh->GetPolygonVertexUV(i, j, pUvName, fbxUv, unmapped);
                    uvs.push_back(static_cast<float>(fbxUv[0]));
                    uvs.push_back(static_cast<float>(fbxUv[1]));
                }
            }
        }

        for (int i = 0; i < PolygonCount; ++i)
        {
            for (int j = 0; j < TRIANGLE_VERTEX_COUNT; ++j)
            {
                int controlPointIndex = pMesh->GetPolygonVertex(i, j);
                std::vector<BoneWeight>& boneWeights = controlPointIndexToBoneWeights[controlPointIndex];
                const int CountWeights = std::min(static_cast<int>(boneWeights.size()), static_cast<int>(COMPONENT_COUNT_BONE));
                for (int k = 0; k < CountWeights; ++k)
                {
                    int l = i * TRIANGLE_VERTEX_COUNT * COMPONENT_COUNT_BONE + j * COMPONENT_COUNT_BONE + k;
                    boneIndices[l] = boneWeights[k].boneIndex;
                    weights[l] = boneWeights[k].weight;
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
        AttributeArray aaPositions(COMPONENT_COUNT_POSITION, GL_FLOAT, 0, 0);
        aaPositions.Fill(positions.size() * sizeof(positions[0]), positions.data());
        pMyMesh->AddAttributeArray(aaPositions);

        AttributeArray aaUvs(COMPONENT_COUNT_UV, GL_FLOAT, 0, 0);
        aaUvs.Fill(uvs.size() * sizeof(uvs[0]), uvs.data());
        pMyMesh->AddAttributeArray(aaUvs);

        AttributeArray aaNormals(COMPONENT_COUNT_NORMAL, GL_FLOAT, 0, 0);
        aaNormals.Fill(normals.size() * sizeof(normals[0]), normals.data());
        pMyMesh->AddAttributeArray(aaNormals);

        AttributeArray aaBoneIndices(COMPONENT_COUNT_BONE, GL_INT, 0, 0);
        aaBoneIndices.Fill(boneIndices.size() * sizeof(boneIndices[0]), boneIndices.data());
        pMyMesh->AddAttributeArray(aaBoneIndices);

        AttributeArray aaWeights(COMPONENT_COUNT_WEIGHT, GL_FLOAT, 0, 0);
        aaWeights.Fill(weights.size() * sizeof(weights[0]), weights.data());
        pMyMesh->AddAttributeArray(aaWeights);

        m_meshes.push_back(pMyMesh);
    });
}

void FbxLoader::LoadAnimation(FbxImporter* pImporter)
{
    assert(m_pScene != nullptr);

    m_pAnimator.reset(new Animator);

    // Animation name stack array
    FbxArray<FbxString*> animArray;
    FbxDocument* pDocument = dynamic_cast<FbxDocument*>(m_pScene);
    if( pDocument != NULL )
        pDocument->FillAnimStackNameArray(animArray);

    std::cout << " Anim Stack Name Array ( Count : " << animArray.GetCount() << " ): " << std::endl;
    for (int i = 0; i < animArray.GetCount(); ++i)
    {
        std::cout << i << " : " << animArray[i]->Buffer() << std::endl;

        FbxTakeInfo* pTakeInfo = pImporter->GetTakeInfo(i);

        FbxString animationName = pTakeInfo->mName;
        
        FbxTimeSpan span = pTakeInfo->mLocalTimeSpan;

        double startTime = span.GetStart().GetSecondDouble();
        double endTime = span.GetStop().GetSecondDouble();
        
        if (startTime < endTime)
        {
            std::shared_ptr<Animation> pAnimation(new Animation(animationName.Buffer()));
            pAnimation->SetLength(static_cast<float>(endTime - startTime));
            m_pAnimator->SetCurrentAnimation(pAnimation);

            
            const double FrameRate = 30.0;
            std::vector<double> keyFrameTimes;
            for (double time = 0.0; time < endTime; time += 1.0 / FrameRate)
            {
                keyFrameTimes.push_back(time);
            }
            std::vector<KeyFrame> keyFrames(keyFrameTimes.size());

            // Generate KeyFrame
            TraverseFbxNodeDepthFirst(m_pScene->GetRootNode(), [this, &keyFrames, endTime, &keyFrameTimes](FbxNode* pNode, int depth)
            {
                FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
                if (pAttribute == nullptr)
                    return;
                FbxNodeAttribute::EType attributeType = pAttribute->GetAttributeType();
                if (attributeType == FbxNodeAttribute::EType::eSkeleton)
                {
                    const char* pBoneName = pNode->GetName();
                    Bone& bone = m_pSkeleton->GetBone(pBoneName);
                    
                    for( int i = 0; i < keyFrameTimes.size(); ++i )
                    {
                        double keyFrameTime = keyFrameTimes[i];

                        FbxTime animationTime;
                        animationTime.SetSecondDouble(keyFrameTime);

                        glm::mat4 localTransform;
                        FbxAMatrixToGlmMat4(pNode->EvaluateLocalTransform(animationTime), localTransform);

                        BoneTransform transform;
                        glm::vec3 scale, skew;
                        glm::vec4 perp;
                        glm::decompose(localTransform, scale, transform.rotation, transform.position, skew, perp);
                        keyFrames[i].AddPose(pBoneName, transform);
                        keyFrames[i].SetTimestamp(static_cast<float>(keyFrameTime));
                    }
                }
                else if (attributeType == FbxNodeAttribute::EType::eMesh)
                {
                    // Legacy animation
                }
            });

            for( int i = 0; i < keyFrames.size(); ++i )
                pAnimation->AddKeyFrame(keyFrames[i]);

            m_animations.push_back(pAnimation);
        }
    }
}

void FbxLoader::WriteSceneHierarchyTo(std::ostream& os, int indentSize)
{
    TraverseFbxNodeDepthFirst(m_pScene->GetRootNode(), [this, indentSize](FbxNode* pNode, int depth)
    {
        const std::string Indent(depth * indentSize, ' ');
        const char* pNodeName = pNode->GetName();
        std::string strAttributeType;
        FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
        if (pAttribute)
        {
            FbxNodeAttribute::EType attributeType = pAttribute->GetAttributeType();
            strAttributeType = ToString(attributeType);
        }

#ifndef NDEBUG
        std::cout << Indent << pNodeName << "(" << depth << ") - " << strAttributeType << std::endl;
#endif
    });
}

void FbxLoader::InitializeMaterial(const FbxSurfaceMaterial* pFbxMaterial, Material* pMaterial)
{
    GLuint textureObject;
    FbxDouble3 color;
    
    color = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, textureObject);
    pMaterial->SetEmissiveColor(glm::vec4(ToVec3(color), 1.0f));
    pMaterial->SetEmissiveMap(textureObject);

    color = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, textureObject);
    pMaterial->SetAmbientColor(glm::vec4(ToVec3(color), 1.0f));
    pMaterial->SetAmbientMap(textureObject);

    color = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, textureObject);
    pMaterial->SetDiffuseColor(glm::vec4(ToVec3(color), 1.0f));
    pMaterial->SetDiffuseMap(textureObject);

    color = GetMaterialProperty(pFbxMaterial, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, textureObject);
    pMaterial->SetSpecularColor(glm::vec4(ToVec3(color), 1.0f));
    pMaterial->SetSpecularMap(textureObject);
    
    FbxProperty shininess = pFbxMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
    if (shininess.IsValid())
    {
        double lShininess = shininess.Get<FbxDouble>();
        pMaterial->SetShininess(static_cast<GLfloat>(lShininess));
    }
}

int FbxLoader::FindIndexOfTextureCache(const std::string& filename)
{
    for (int i = 0; i < m_textures.size(); ++i)
    {
        if (m_textures[i].filename == filename)
            return i;
    }
    return -1;
}

FbxDouble3 FbxLoader::GetMaterialProperty(const FbxSurfaceMaterial * pMaterial, const char * pPropertyName, const char * pFactorPropertyName, GLuint& textureName)
{
    textureName = 0;

    FbxDouble3 result(0, 0, 0);
    const FbxProperty prop = pMaterial->FindProperty(pPropertyName);
    const FbxProperty factorProp = pMaterial->FindProperty(pFactorPropertyName);
    if (prop.IsValid() && factorProp.IsValid())
    {
        result = prop.Get<FbxDouble3>();
        double factor = factorProp.Get<FbxDouble>();
        if (factor != 1)
        {
            result[0] *= factor;
            result[1] *= factor;
            result[2] *= factor;
        }
    }

    if (prop.IsValid())
    {
        const int TextureCount = prop.GetSrcObjectCount<FbxFileTexture>();
        if (TextureCount)
        {
            const FbxFileTexture* pTexture = prop.GetSrcObject<FbxFileTexture>();
            if (pTexture)
            {
                const char* fileName = pTexture->GetFileName();
                int index = FindIndexOfTextureCache(fileName);
                if (index != -1)
                    textureName = m_textures[index].textureObject;
            }
        }
    }

    return result;
}
