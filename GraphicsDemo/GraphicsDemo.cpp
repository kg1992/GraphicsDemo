#include "SystemComponent.h"
#include "GraphicsDemo.h"
#include "Errors.h"
#include <iostream>
#include <fstream>
#include <glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <fbxsdk.h>
#include <map>
#include "System.h"
#include "Object.h"
#include "Camera.h"
#include "Material.h"
#include "Mesh.h"
#include "stb_image.h"

struct SubMesh
{
    SubMesh() : IndexOffset(0), TriangleCount(0) {}

    int IndexOffset;
    int TriangleCount;
};

namespace
{
    const int TRIANGLE_VERTEX_COUNT = 3;
    GLint s_mvLocation;
    GLint s_projLocation;
    GLint s_eyeLocation;
    std::vector<std::shared_ptr<Object>> s_objectStack;
    std::shared_ptr<Material> s_pMaterial(new Material());

    Object object;
    Camera camera;

    std::string GetShaderInfoLog(GLuint shader)
    {
        GLsizei bufSize;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

        std::vector<GLchar> buf;
        buf.resize(bufSize + 1);

        // Get InfoLog
        glGetShaderInfoLog(shader, bufSize, nullptr, (GLchar*)buf.data());

        // return buffer
        return std::string(buf.data());
    }

    std::string GetProgramInfoLog(GLuint program)
    {
        GLsizei bufSize;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

        std::vector<GLchar> buf;
        buf.resize(bufSize + 1);

        // Get InfoLog
        glGetProgramInfoLog(program, bufSize, nullptr, (GLchar*)buf.data());

        // return buffer
        return std::string(buf.data());
    }

    GLuint CompileShaderFromSourceFile(GLenum type, const std::string& fileName)
    {
        GLuint shader = glCreateShader(type);
        std::ifstream fin(fileName);
        std::vector<GLchar> source((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        GLchar* pSource = source.data();
        GLint sourceSize = static_cast<GLint>(source.size());
        glShaderSource(shader, 1, &pSource, &sourceSize);
        glCompileShader(shader);
        std::string infoLog = GetShaderInfoLog(shader);
        if (infoLog.size() != 0) {
            std::cout << infoLog << std::endl;
        }
        return shader;
    }

    GLuint PrepareShaderProgram()
    {
        GLuint vertexShader = CompileShaderFromSourceFile(GL_VERTEX_SHADER, "vs.glsl");
        GLuint fragmentShader = CompileShaderFromSourceFile(GL_FRAGMENT_SHADER, "fs.glsl");
        //GLuint tessControlShader = CompileShaderFromSourceFile(GL_TESS_CONTROL_SHADER, "tcs.glsl");
        //GLuint tessEvalShader = CompileShaderFromSourceFile(GL_TESS_EVALUATION_SHADER, "tes.glsl");
        //GLuint geometryShader = CompileShaderFromSourceFile(GL_GEOMETRY_SHADER, "gs.glsl");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        //glAttachShader(program, tessControlShader);
        //glAttachShader(program, tessEvalShader);
        //glAttachShader(program, geometryShader);
        glLinkProgram(program);

        std::string programInfoLog = GetProgramInfoLog(program);
        if (programInfoLog.size() != 0) {

            std::cout << " * Program Info Log : " << std::endl
                << GetProgramInfoLog(program) << std::endl;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        s_mvLocation = glGetUniformLocation(program, "mvMatrix");
        GET_AND_HANDLE_GL_ERROR();
        s_projLocation = glGetUniformLocation(program, "projMatrix");
        GET_AND_HANDLE_GL_ERROR();
        s_eyeLocation = glGetUniformLocation(program, "eyeMatrix");
        GET_AND_HANDLE_GL_ERROR();

        return program;
    }

    void generate_texture(float* data, int width, int height)
    {
        int end = width * height;
        for (int i = 0; i < end; ++i)
        {
            int offset = i * 4;
            int region = i * 3 / end;
            if (region == 0)
            {
                data[offset] = 1.0f;
                data[offset + 1] = 0.0f;
                data[offset + 2] = 0.0f;
                data[offset + 3] = 1.0f;
            }
            else if (region == 1)
            {
                data[offset] = 0.0f;
                data[offset + 1] = 1.0f;
                data[offset + 2] = 0.0f;
                data[offset + 3] = 1.0f;
            }
            else if (region = 2)
            {
                data[offset] = 0.0f;
                data[offset + 1] = 0.0f;
                data[offset + 2] = 1.0f;
                data[offset + 3] = 1.0f;
            }
        }
    }

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

                std::vector<glm::vec3> positions;
                std::vector<glm::vec3> normals;
                std::vector<glm::vec2> uvs;

                for (std::map<int, std::vector<int>>::iterator it = submeshes.begin(); it != submeshes.end(); ++it)
                {
                    std::vector<int> subMeshPolygonsIndice = it->second;
                    for (int i : subMeshPolygonsIndice)
                    {
                        for (int j = 0; j < TRIANGLE_VERTEX_COUNT; ++j)
                        {
                            int controlPointIndex = pMesh->GetPolygonVertex(i, j);
                            FbxVector4 controlPoint = pMesh->GetControlPointAt(controlPointIndex);
                            positions.push_back(glm::vec3(controlPoint[0], controlPoint[1], controlPoint[2]));

                            FbxVector4 fbxNormal;
                            pMesh->GetPolygonVertexNormal(i, j, fbxNormal);
                            normals.push_back(glm::vec3(fbxNormal[0], fbxNormal[1], fbxNormal[2]));

                            FbxVector2 fbxUv;
                            bool unmapped;
                            pMesh->GetPolygonVertexUV(i, j, lUVName, fbxUv, unmapped);
                            uvs.push_back(glm::vec2(fbxUv[0], fbxUv[1]));
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
                        subMesh.vertCount = it->second.size() * TRIANGLE_VERTEX_COUNT;
                        pMyMesh->AddSubMesh(subMesh);
                        begin += subMesh.vertCount;
                    }
                }
                AttributeArray aaPositions(3, GL_FLOAT, 0, 0);
                aaPositions.Fill(positions.size() * sizeof(glm::vec3), positions.data());
                pMyMesh->AddAttributeArray(aaPositions);

                AttributeArray aaUvs(2, GL_FLOAT, 0, 0);
                aaUvs.Fill(uvs.size() * sizeof(glm::vec2), uvs.data());
                pMyMesh->AddAttributeArray(aaUvs);

                AttributeArray aaNormals(3, GL_FLOAT, 0, 0);
                aaNormals.Fill(normals.size() * sizeof(glm::vec3), normals.data());
                pMyMesh->AddAttributeArray(aaNormals);

                pObject->SetMesh(pMyMesh);
                pObject->SetMaterial(s_pMaterial);
                objectStack.push_back(pObject);
            }
        }

        for (int i = 0; i < pNode->GetChildCount(); ++i)
            LoadRecursive(pNode->GetChild(i), objectStack, depth + 1);
    }

    // Load a texture file (TGA only now) into GPU and return the texture object name
    bool LoadTextureFromFile(const FbxString & pFilePath, unsigned int & pTextureObject)
    {
        //    int x,y,n;
        //    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
        //    // ... process data if not NULL ...
        //    // ... x = width, y = height, n = # 8-bit components per pixel ...
        //    // ... replace '0' with '1'..'4' to force that many components per pixel
        //    // ... but 'n' will always be the number that it would have been if you said 0
        //    stbi_image_free(data)

        int x, y, n;
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

    void LoadFbxSdkObject(const char* const filename)
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
                    LoadRecursive(pScene->GetRootNode(), s_objectStack);

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
}



void GraphicsDemo::OnStart()
{
    static const GLfloat vertex_positions[] =
    {
        -0.25f,  0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
         0.25f, -0.25f, -0.25f,

         0.25f, -0.25f, -0.25f,
         0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,

         0.25f, -0.25f, -0.25f,
         0.25f, -0.25f,  0.25f,
         0.25f,  0.25f, -0.25f,

         0.25f, -0.25f,  0.25f,
         0.25f,  0.25f,  0.25f,
         0.25f,  0.25f, -0.25f,

         0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f,  0.25f,
         0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
         0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f, -0.25f,
        -0.25f,  0.25f, -0.25f,
        -0.25f,  0.25f,  0.25f,

        -0.25f, -0.25f,  0.25f,
         0.25f, -0.25f,  0.25f,
         0.25f, -0.25f, -0.25f,

         0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f, -0.25f,
        -0.25f, -0.25f,  0.25f,

        -0.25f,  0.25f, -0.25f,
         0.25f,  0.25f, -0.25f,
         0.25f,  0.25f,  0.25f,

         0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f,  0.25f,
        -0.25f,  0.25f, -0.25f
    };

    std::shared_ptr<Mesh> pMesh(new Mesh());
    AttributeArray aaPositions(3, GL_FLOAT, 0, 0);
    aaPositions.Fill(sizeof(vertex_positions), (void*)vertex_positions);
    pMesh->AddAttributeArray(aaPositions);
    object.SetMesh(pMesh);

    camera.LookAt(glm::vec3(50, 50, 50), glm::vec3(), glm::vec3(0, 1, 0));
    int width = 512;
    int height = 512;
    float* data = new float[width * height * 4];
    generate_texture(data, width, height);
    s_pMaterial->SetProgram(PrepareShaderProgram());
    s_pMaterial->Fill(data, width, height);
    object.SetMaterial(s_pMaterial);
    
    delete[] data;

    LoadFbxSdkObject("./Resources/56-fbx/fbx/Dragon 2.5_fbx.fbx");
}

void GraphicsDemo::Update(float dt)
{
    const float M_PI = 3.14159f;
    const float currentTime = System::Instance()->CurrentTime();
    const float f = (float)currentTime * (float)M_PI * 0.1f;

    glm::mat4 mvMatrix = glm::identity<glm::mat4>();

    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    mvMatrix = glm::translate(mvMatrix, glm::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) *0.5f, sinf(1.3f*f)));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 45.0f / M_PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
    mvMatrix = glm::rotate(mvMatrix, (float)currentTime * 81.0f / M_PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));

    object.SetTransformMatrix(mvMatrix);
}

void GraphicsDemo::Render()
{
    GLfloat color[] = { 0, 0.8f, 0.7f, 1.0f };

    glClearBufferfv(GL_COLOR, 0, color);

    // Render(object);
    for (auto& pObject : s_objectStack)
    {
        Render(*pObject);
    }
}

void GraphicsDemo::Free()
{
    object.Free();
}

void GraphicsDemo::OnWndProc(HWND hwnd, UINT uMsg, WPARAM wParama, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        short width = LOWORD(lParam);
        short height = HIWORD(lParam);
        camera.SetFrustum(glm::pi<float>() * 0.25f, width / 0.5f, height / 0.5f,
            0.1f, 1000.0f);
    }
}

void GraphicsDemo::Render(Object & object)
{
    glUseProgram(object.GetMaterial()->GetProgram());
    
    glUniformMatrix4fv(s_mvLocation, 1, GL_FALSE, (float*)&object.GetTransformMatrix()[0][0]);
    glUniformMatrix4fv(s_projLocation, 1, GL_FALSE, (float*)&camera.ProjectionMatrix()[0][0]);
    glUniformMatrix4fv(s_eyeLocation, 1, GL_FALSE, (float*)&camera.EyeMatrix()[0][0]);

    object.Render();
}
