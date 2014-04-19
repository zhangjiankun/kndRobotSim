#include<QtDebug>
#include <QtOpenGL>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// the global Assimp scene object
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
aiVector3D scene_min, scene_max, scene_center;

// current rotation angle
static float angle = 0.f;

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)
//e zjk
// ----------------------------------------------------------------------------
void color4_to_float4(const  aiColor4D *c, float f[4])
{
    f[0] = c->r;
    f[1] = c->g;
    f[2] = c->b;
    f[3] = c->a;
}

// ----------------------------------------------------------------------------
void set_float4(float f[4], float a, float b, float c, float d)
{
    f[0] = a;
    f[1] = b;
    f[2] = c;
    f[3] = d;
}

void set_aiMatrix4x4(aiMatrix4x4 &mat, float m[])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            mat[i][j] = m[4*i+j];
        }
    }
}

void aiMatrix4x4_to_float(const aiMatrix4x4 &mat, float m[])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[4*i+j] = mat[i][j];
        }
    }
}

void set_aiMatrix4x4(aiMatrix4x4 &mat, double m[])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            mat[i][j] = m[4*i+j];
        }
    }
}

void apply_material(const  aiMaterial *mtl)
{
    float c[4];

    GLenum fill_mode;
    int ret1, ret2;
     aiColor4D diffuse;
     aiColor4D specular;
     aiColor4D ambient;
     aiColor4D emission;
    float shininess, strength;
    int two_sided;
    int wireframe;
    unsigned int max;
#if 0
    set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
        color4_to_float4(&diffuse, c);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, c);

    set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
        color4_to_float4(&specular, c);
    glMaterialfv(GL_FRONT, GL_SPECULAR, c);

    set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
        color4_to_float4(&ambient, c);
    glMaterialfv(GL_FRONT, GL_AMBIENT, c);

    set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
    if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
        color4_to_float4(&emission, c);
    glMaterialfv(GL_FRONT, GL_EMISSION, c);
#endif
    max = 1;
    ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
    if(ret1 == AI_SUCCESS) {
        max = 1;
        ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
        if(ret2 == AI_SUCCESS)
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
        else
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        qDebug("get MaterialFloat array Success!");
    }
    else {
        glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
        set_float4(c, 0.3f, 0.6f, 0.4f, 1.0f);

        //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, c);
        qDebug("get MaterialFloat array fail!c:%f %f %f %f",c[0],c[1],c[2],c[3]);
    }

    max = 1;
    if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
        fill_mode = wireframe ? GL_LINE : GL_FILL;
    else
        fill_mode = GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

    max = 1;
    if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
}

void get_bounding_box_for_node (const aiNode* nd,
     aiVector3D* min,
     aiVector3D* max,
     aiMatrix4x4* trafo
){
    aiMatrix4x4 prev;
    unsigned int n = 0, t;

    prev = *trafo;
    aiMultiplyMatrix4(trafo,&nd->mTransformation);

    for (; n < nd->mNumMeshes; ++n)
    {
        const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
        for (t = 0; t < mesh->mNumVertices; ++t)
        {

            aiVector3D tmp = mesh->mVertices[t];
            aiTransformVecByMatrix4(&tmp,trafo);

            min->x = aisgl_min(min->x,tmp.x);
            min->y = aisgl_min(min->y,tmp.y);
            min->z = aisgl_min(min->z,tmp.z);

            max->x = aisgl_max(max->x,tmp.x);
            max->y = aisgl_max(max->y,tmp.y);
            max->z = aisgl_max(max->z,tmp.z);
        }
    }

    for (n = 0; n < nd->mNumChildren; ++n)
    {
        get_bounding_box_for_node(nd->mChildren[n],min,max,trafo);
    }
    *trafo = prev;
}

// ----------------------------------------------------------------------------
void get_bounding_box (aiVector3D* min, aiVector3D* max)
{
    aiMatrix4x4 trafo;
    aiIdentityMatrix4(&trafo);

    min->x = min->y = min->z =  1e10f;
    max->x = max->y = max->z = -1e10f;
    get_bounding_box_for_node(scene->mRootNode, min, max, &trafo);
}

const aiScene* loadasset (const char* path)
{
    const aiScene* scene = NULL;

    scene = aiImportFile(path,aiProcessPreset_TargetRealtime_Fast);

    if (scene)
    {
        get_bounding_box(&scene_min,&scene_max);
        scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
        scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
        scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
        qDebug("min:%f %f %f;max:%f %f %f",scene_min.x,scene_min.y,scene_min.z,
               scene_max.x,scene_max.y,scene_max.z);
        return scene;
    }
    return scene;
}
void printMatri4x4(aiMatrix4x4 &m, char *tip)
{
    qDebug("%s:%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n",tip,
           m[0][0],m[0][1],m[0][2],m[0][3],m[1][0],m[1][1],m[1][2],m[1][3],
           m[2][0],m[2][1],m[2][2],m[2][3],m[3][0],m[3][1],m[3][2],m[3][3]);
}
void printMatri4x4(double *m, char *tip)
{
    qDebug("%s:%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n",tip,
           m[0],m[1],m[2],m[3],m[4],m[5],m[6],m[7],
           m[8],m[9],m[10],m[11],m[12],m[13],m[14],m[15]);
}
void recursive_render (const struct aiScene *sc, const struct aiNode* nd, float scale)
{
    unsigned int i;
    unsigned int n = 0, t;
    aiMatrix4x4 m = nd->mTransformation;
//    qDebug("%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n",
//           m[0][0],m[0][1],m[0][2],m[0][3],m[1][0],m[1][1],m[1][2],m[1][3],
//           m[2][0],m[2][1],m[2][2],m[2][3],m[3][0],m[3][1],m[3][2],m[3][3]);
    // upfate transform
    aiTransposeMatrix4(&m);
    glPushMatrix();
    // scale the whole asset to fit into our view frustum
//    float tmp = scene_max.x-scene_min.x;
//    tmp = aisgl_max(scene_max.y - scene_min.y,tmp);
//    tmp = aisgl_max(scene_max.z - scene_min.z,tmp);
    qDebug("scale:%f",scale);
//    tmp = 5.f / tmp;
    float tmp = 1.0f / scale;//zjk

    glScalef(tmp, tmp, tmp);
    // center the model
    //glTranslatef( -scene_center.x, -scene_center.y, -scene_center.z );

    glMultMatrixf((float*)&m);
    //qDebug("%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n%f %f %f %f \n",
    //       m[0][0],m[0][1],m[0][2],m[0][3],m[1][0],m[1][1],m[1][2],m[1][3],
    //       m[2][0],m[2][1],m[2][2],m[2][3],m[3][0],m[3][1],m[3][2],m[3][3]);

    // draw all meshes assigned to this node
    for (; n < nd->mNumMeshes; ++n)
    {
        const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

        apply_material(sc->mMaterials[mesh->mMaterialIndex]);

        if(mesh->mNormals == NULL)
        {
            glDisable(GL_LIGHTING);
        }
        else
        {
            glEnable(GL_LIGHTING);
        }

        for (t = 0; t < mesh->mNumFaces; ++t)
        { //facet数目
            const struct aiFace* face = &mesh->mFaces[t];
            GLenum face_mode;

            switch(face->mNumIndices)
            {
                case 1: face_mode = GL_POINTS; break;
                case 2: face_mode = GL_LINES; break;
                case 3: face_mode = GL_TRIANGLES; break;
                default: face_mode = GL_POLYGON; break;
            }
            //qDebug("face_mode=4",face_mode);

            glBegin(face_mode);

            for(i = 0; i < face->mNumIndices; i++)
            {
                int index = face->mIndices[i];
                if(mesh->mColors[0] != NULL)
                {
                    glColor4fv((GLfloat*)&mesh->mColors[0][index]);
                    qDebug("color");
                }
                if(mesh->mNormals != NULL && i == 0)
                {
                    glNormal3fv(&mesh->mNormals[index].x);
                    //qDebug("normal%f %f %f",mesh->mNormals[index].x,mesh->mNormals[index].y,mesh->mNormals[index].z);
                }
                //qDebug("index=%d,i=%d,%f,%f,%f",index,i,mesh->mVertices[index].x,mesh->mVertices[index].y,mesh->mVertices[index].z);
                glVertex3f(mesh->mVertices[index].x,mesh->mVertices[index].y,mesh->mVertices[index].z);
            }

            glEnd();
        }

//        qDebug("nd->mNumChildren=%d,nd->mNumMeshes=%d,mesh->mNormals=%d, mesh->mNumFaces=%d",
//               nd->mNumChildren,nd->mNumMeshes,mesh->mNormals, mesh->mNumFaces);
    }

    // draw all children
    for (n = 0; n < nd->mNumChildren; ++n)
    {
        recursive_render(sc, nd->mChildren[n], scale);
    }


    glPopMatrix();
}
