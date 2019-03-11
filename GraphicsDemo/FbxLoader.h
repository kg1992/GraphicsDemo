/*
    FbxLoader.h

    Defines routines for loading FbxScene and converting it to GraphicsDemo project objects.

    Dependencies:
        FBX SDK 2019 - for FbxScene importing
        stb_image.h - for texture file loading
*/

class Material;
class Skeleton;
class Bone;
class Object;
class Mesh;
class Animation;
class Animator;

class FbxLoader
{
public:
    FbxLoader();
    ~FbxLoader();

    //
    // Load an fbx file saved in given 'filename'
    //
    void Load(const char* filename);

    //
    // Clearup states filled up by last call to 'FbxLoader::Load' method.
    //
    void Clear();

    std::shared_ptr<Object> GetObjectByIndex(int index)
    {
        return m_objects[index];
    }

private:
    struct TextureCache
    {
        GLuint textureObject;
    };

    // FbxManager class instnace. according to the documentation, only one FbxManager is required for entire application lifetime.
    FbxManager* m_pFbxManager;
    // Scene to be filled up by importing a fbx file.
    FbxScene* m_pScene;
    std::vector<TextureCache> m_textures;
    std::vector<std::shared_ptr<Material>> m_materials;
    std::vector<std::shared_ptr<Object>> m_objects;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Skeleton> m_pSkeleton;
    std::vector<std::shared_ptr<Animation>> m_animations;
    std::shared_ptr<Animator> m_pAnimator;

    void LoadTextures();
    void LoadMaterial();
    void LoadSkeleton();
    void LoadMesh();
    void LoadAnimation(FbxImporter* pImporter);
    void WriteSceneHierarchyTo(std::ostream& os, int indentSize = 2);
};
