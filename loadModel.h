#ifndef LOADMODEL_H
#define LOADMODEL_H

//#include <assimp/scene.h>
// ----------------------------------------------------------------------------
void color4_to_float4(const  aiColor4D *c, float f[4]);

// ----------------------------------------------------------------------------
void set_float4(float f[4], float a, float b, float c, float d);

void apply_material(const  aiMaterial *mtl);

void get_bounding_box_for_node (const aiNode* nd,
     aiVector3D* min,
     aiVector3D* max,
     aiMatrix4x4* trafo
);

// ----------------------------------------------------------------------------
void get_bounding_box (aiVector3D* min, aiVector3D* max);

int loadasset (const char* path);
void printMatri4x4(double *m, char *tip);
void printMatri4x4(aiMatrix4x4 &m,char *tip);
void set_aiMatrix4x4(aiMatrix4x4 &mat, float m[]);
void set_aiMatrix4x4(aiMatrix4x4 &mat, double m[]);
void aiMatrix4x4_to_float(const aiMatrix4x4 &mat, float m[]);

void recursive_render (const struct aiScene *sc, const struct aiNode* nd, float scale);
//extern const struct aiScene* scene ;
//extern GLuint scene_list ;
//extern aiVector3D scene_min, scene_max, scene_center;

#endif // LOADMODEL_H
