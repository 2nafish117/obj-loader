#include "obj_loader.h"

int main()
{
    tf::ObjMesh mesh;
    tf::load_objfile("unit_cube_triangulated.obj", &mesh);
    
    printf("mesh!!\n");
    printf("%u\n", mesh.num_vertex);
    printf("%u\n", mesh.num_normal);
    printf("%u\n", mesh.num_texcoord);
    printf("%u\n", mesh.num_index);
    for(int i = 0;i < mesh.num_vertex;++i)
    {
        printf("v %f %f %f\n", mesh.vertex[i].x, mesh.vertex[i].y, mesh.vertex[i].z);
    }
    for(int i = 0;i < mesh.num_texcoord;++i)
    {
        printf("vt %f %f\n", mesh.texcoord[i].x, mesh.texcoord[i].y);
    }
    for(int i = 0;i < mesh.num_normal;++i)
    {
        printf("vn %f %f %f\n", mesh.normal[i].x, mesh.normal[i].y, mesh.normal[i].z);
    }
    for(int i = 0;i < mesh.num_index;++i)
    {
        printf("i %d/%d/%d\n", mesh.index[i].vertex, mesh.index[i].texcoord, mesh.index[i].normal);
    }
    return 0;
}