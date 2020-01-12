#pragma once

#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>

namespace tf // tuna-fish
{
    // works only on triangulated meshes
    // make sure to export to triangulated!!

    struct vec3
    { float x, y, z; };

    struct vec2
    { float x, y; };

    struct ObjIndex
    {
        unsigned int vertex;
        unsigned int normal;
        unsigned int texcoord;
    };

    struct ObjMesh
    {
        char name[128];
        bool has_normals;
        bool has_texture;
        bool smooth_shaded;
        bool use_material;
        unsigned int num_vertex;
        unsigned int num_texcoord;
        unsigned int num_normal;
        unsigned int num_index;
        vec3* vertex;
        vec2* texcoord;
        vec3* normal;
        ObjIndex* index;
    };

    static void _action_vertex(const char* line, ObjMesh* oMesh)
    {
        vec3 vertex;
        sscanf(line + 2, "%f %f %f", &vertex.x, &vertex.y, &vertex.z);
        unsigned int n = oMesh->num_vertex;
        oMesh->vertex[n] = vertex;
        oMesh->num_vertex += 1;
    }
    static void _action_texcoord(const char* line, ObjMesh* oMesh)
    {
        vec2 texcoord;
        sscanf(line + 2, "%f %f", &texcoord.x, &texcoord.y);
        unsigned int n = oMesh->num_texcoord;
        oMesh->texcoord[n] = texcoord;
        oMesh->num_texcoord += 1;
    }
    static void _action_normal(const char* line, ObjMesh* oMesh)
    {
        vec3 normal;
        sscanf(line + 2, "%f %f %f", &normal.x, &normal.y, &normal.z);
        unsigned int n = oMesh->num_normal;
        oMesh->normal[n] = normal;
        oMesh->num_normal += 1;
    }
    static void _action_index(const char* line, ObjMesh* oMesh)
    {
		// triangulated only!!
		ObjIndex index[3];
		sscanf(line + 2, "%u/%u/%u %u/%u/%u %u/%u/%u", 
			&index[0].vertex, &index[0].texcoord, &index[0].normal,
			&index[1].vertex, &index[1].texcoord, &index[1].normal,
			&index[2].vertex, &index[2].texcoord, &index[2].normal
		);

        // directly copy the 1 based index!!
		unsigned int n = oMesh->num_index;
		oMesh->index[n] 	= index[0];
		oMesh->index[n + 1] = index[1];
		oMesh->index[n + 2] = index[2];

		oMesh->num_index += 3;
    }
    static void _action_shading(const char* line, ObjMesh* oMesh)
    {
		char token[8];
		char shading_option[8];
		sscanf(line + 2, "%s", shading_option);
		if(strcmp("off", shading_option) == 0)
			oMesh->smooth_shaded = false;
		else
		{
			// TODO: ????
			oMesh->smooth_shaded = true;
		}
    }
    static void _action_meshname(const char* line, ObjMesh* oMesh)
    {
		sscanf(line + 2, "%s", oMesh->name);
    }
    static void _action_materialname(const char* line, ObjMesh* oMesh)
    {
		// TODO: ignore for now ????
    }
    static void _action_usematerial(const char* line, ObjMesh* oMesh)
    {
		char material_option[128];
		sscanf(line + 7, "%s", material_option);
		if(strcmp("None", material_option) == 0)
			oMesh->use_material = false;
		else
		{
			// TODO: ????
			oMesh->use_material = true;
		}
    }
    static void _action_ignore(const char* line, ObjMesh* oMesh)
    {
        // Do Nothing !!
    }

    #define _NUM_TOKENS 10
    static const char* _tokens[_NUM_TOKENS] = {
        "v",        // vertex
        "vt",       // texcoord
        "vn",       // normal
        "f",        // face/indices

        "#",        // comment

        "s",        // smooth shading on/off
        "o",        // mesh name
        "mtllib",   // material file
        "usemtl",   // use material None/<material name>
        "\n"        // ignore
    };

    typedef void (*Action) (const char* line, ObjMesh* oMesh);
    static Action _actions[_NUM_TOKENS] = 
    {
        _action_vertex,
        _action_texcoord,
        _action_normal,
        _action_index,
        
        _action_ignore,

        _action_shading,
        _action_meshname,
        _action_materialname,
        _action_usematerial,
        _action_ignore
    };

    static _allocate_mesh(ObjMesh* oMesh, unsigned int num_vertex, 
        unsigned int num_texcoord, unsigned int num_normal, unsigned int num_index)
    {
        // // allocate all at once and distribute
        // unsigned char* mem = (unsigned char*) malloc(
        //     num_vertex      * sizeof(vec3) +
        //     num_texcoord    * sizeof(vec2) +
        //     num_normal      * sizeof(vec3) +
        //     num_index       * sizeof(ObjIndex)
        // );

        // unsigned int offsetof_texcoord  = num_vertex * sizeof(vec3);
        // unsigned int offsetof_normal    = offsetof_texcoord + num_texcoord 	* sizeof(vec2);
        // unsigned int offsetof_index     = offsetof_normal   + num_normal 	* sizeof(vec3);

        // oMesh->vertex   = (vec3*) (mem + 0);
        // oMesh->texcoord = (vec2*) (mem + offsetof_texcoord);
        // oMesh->normal   = (vec3*) (mem + offsetof_normal);
        // oMesh->index    = (ObjIndex*) (mem + offsetof_index);
        
        oMesh->vertex   = (vec3*) malloc(sizeof(vec3) * num_vertex);
        oMesh->texcoord = (vec2*) malloc(sizeof(vec2) * num_texcoord);
        oMesh->normal   = (vec3*) malloc(sizeof(vec3) * num_normal);
        oMesh->index    = (ObjIndex*) malloc(sizeof(ObjIndex) * num_index);
        
        oMesh->num_vertex   = 0;
        oMesh->num_texcoord = 0;
        oMesh->num_normal   = 0;
        oMesh->num_index    = 0;
    }

    bool load_objfile(const char* path, ObjMesh* oMesh)
    {
        FILE* objfile = NULL;
        objfile = fopen(path, "r");
        if(objfile == NULL)
        {
            printf("[Error] %s not found\n", path);
            return false;
        }
        char token[32];
		char line[128];

        unsigned int num_vertex = 0;
        unsigned int num_texcoord = 0;
        unsigned int num_normal = 0;
        unsigned int num_index = 0;

        // find num_* to allocate
		while(fgets(line, 128, objfile) != NULL)
		{
			sscanf(line, "%s", token);
			if(strcmp("v", token) == 0)
				num_vertex++;
			else if(strcmp("vt", token) == 0)
				num_texcoord++;
			else if(strcmp("vn", token) == 0)
				num_normal++;
			else if(strcmp("f", token) == 0)
				num_index++;
		}

		// seek back to the beginning of the file
        rewind(objfile);

        _allocate_mesh(oMesh, num_vertex, num_texcoord, num_normal, num_index);

        // populate oMesh
		while(fgets(line, 128, objfile) != NULL)
		{
            //printf("%s", line);
			sscanf(line, "%s", token);
			for(int i = 0;i < _NUM_TOKENS;++i)
			{
				if(strcmp(_tokens[i], token) == 0)
				{
					Action action_to_use = _actions[i];
					action_to_use(line, oMesh);
					break;
				}
			}
		}

    printf("mesh!!\n");
    printf("%u\n", oMesh->num_vertex);
    printf("%u\n", oMesh->num_normal);
    printf("%u\n", oMesh->num_texcoord);
    printf("%u\n", oMesh->num_index);
    for(int i = 0;i < oMesh->num_vertex;++i)
    {
        printf("v %f %f %f\n", oMesh->vertex[i].x, oMesh->vertex[i].y, oMesh->vertex[i].z);
    }
    for(int i = 0;i < oMesh->num_texcoord;++i)
    {
        printf("vt %f %f\n", oMesh->texcoord[i].x, oMesh->texcoord[i].y);
    }
    for(int i = 0;i < oMesh->num_normal;++i)
    {
        printf("vn %f %f %f\n", oMesh->normal[i].x, oMesh->normal[i].y, oMesh->normal[i].z);
    }
    for(int i = 0;i < oMesh->num_index;++i)
    {
        printf("i %d/%d/%d\n", oMesh->index[i].vertex, oMesh->index[i].texcoord, oMesh->index[i].normal);
    }

        // TODO: crashes somewhere around here FIX !!!!!!!!!!!
        // printf("%d\n", objfile);
        // printf("done loading mesh!\n");
        // rewind(objfile);
        // printf("rewind\n");
        // int ret = fclose(objfile);
        // printf("%d\n", ret);
        
        // printf("done loading mesh!\n");
        // assert(num_vertex   == oMesh->num_vertex);
        // printf("done loading mesh!\n");
        // assert(num_texcoord == oMesh->num_texcoord);
        // printf("done loading mesh!\n");
        // assert(num_normal   == oMesh->num_normal);
        // printf("done loading mesh!\n");
        // assert(num_index    == oMesh->num_index);
        printf("done loading mesh!\n");
        return true;
    }

    #undef _NUM_TOKENS
}