/*
    FbxLoader.h

    Defines routines for loading FbxScene and converting it to GraphicsDemo project objects.

    Dependencies:
        FBX SDK 2019 - for FbxScene importing
        stb_image.h - for texture file loading
*/
#include "Material.h"

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

    void Load(const char* filename);

    struct TextureCache
    {
        GLuint textureObject;
    };

    std::shared_ptr<Object> GetObjectByIndex(int index)
    {
        return m_objects[index];
    }

private:
    FbxManager* m_pFbxManager;
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
};
