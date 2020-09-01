#ifndef MARCHINGCUBES
#define MARCHINGCUBES

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>

#include "marchingcubeslookup.h"

using namespace std;

struct Cell {
    glm::vec3 p[8];
    float val[8];
    size_t x, y, z;
};

struct Intersection;

struct Face {
	Intersection *iList[3];
	glm::vec3 normal;
};

struct Intersection {
    glm::vec3 position;
	glm::vec3 normal;
	vector<Face*> faceList;
};

class MarchingCubes {
    GLubyte *raw_data;
    int raw_dimension[3];
    size_t raw_size;
    vector<Cell> cells;
    int cells_dimension[3];
    vector<Face*> faces;
    unordered_map<int, Intersection*> intersections;
public: 
    GLfloat scale;
    void loadModel(std::string texture_path, int x, int y, int z) {
        raw_size = x * y * z;
        raw_dimension[0] = x;
        raw_dimension[1] = y;
        raw_dimension[2] = z;
        raw_data = load_3d_raw_data(texture_path);

        /* for (size_t i = 0; i < raw_size; ++i) */
            /* cout << hex << setfill('0') << setw(2) << raw_data[i] << " "; */
            /* cout << (float)raw_data[i] << endl; */
    }

    void setCuts(size_t cuts) {
        size_t xti = cuts, yti = cuts, zti = cuts;
        size_t xsi = xti+2, ysi = yti+2, zsi = yti+2;
        vector<GLfloat> points(xsi * ysi * zsi);
        vector<glm::vec3> normals(xsi * ysi * zsi);
        cells.clear();
        cells.resize((xsi-1)*(ysi-1)*(zsi-1));
        cells_dimension[0] = xsi-1;
        cells_dimension[1] = ysi-1;
        cells_dimension[2] = zsi-1;
        GLfloat xdi = 1.0f * (raw_dimension[0]) / (xti-1);
        GLfloat ydi = 1.0f * (raw_dimension[1]) / (yti-1);
        GLfloat zdi = 1.0f * (raw_dimension[2]) / (zti-1);

        for(size_t z = 1; z < zsi - 1; z++) {
            for(size_t y = 1; y < ysi - 1; y++) {
                for(size_t x = 1; x < xsi - 1; x++) {
                    points[index(x, y, z, xsi, ysi)] = trilinear(xdi*(x-1), ydi*(y-1), zdi*(z-1));
                    /* cout<<setw(8)<<points[index(x,y,z,xsi, ysi)]<<" "; */
                }
            /* cout<<endl; */
            }
            /* cout<<endl; */
        }

        for(size_t z = 0; z < zsi-1; z++) {
            for(size_t y = 0; y < ysi-1; y++) {
                for(size_t x = 0; x < xsi-1; x++) {
                    GLfloat x1 = xdi*x;
                    GLfloat x2 = x1 + xdi;
                    GLfloat y1 = ydi*y;
                    GLfloat y2 = y1 + ydi;
                    GLfloat z1 = zdi*z;
                    GLfloat z2 = z1 + zdi;
                    int xp = x+1;
                    int yp = y+1;
                    int zp = z+1;
                    cells[index(x, y, z, xsi-1, ysi-1)] = {
                        {
                            glm::vec3(x1, y1, z1),
                            glm::vec3(x2, y1, z1),
                            glm::vec3(x2, y2, z1),
                            glm::vec3(x1, y2, z1),
                            glm::vec3(x1, y1, z2),
                            glm::vec3(x2, y1, z2),
                            glm::vec3(x2, y2, z2),
                            glm::vec3(x1, y2, z2)
                        },
                        {
                            points[index(x, y, z, xsi, ysi)],
                            points[index(xp, y, z, xsi, ysi)],
                            points[index(xp, yp, z, xsi, ysi)],
                            points[index(x, yp, z, xsi, ysi)],
                            points[index(x, y, zp, xsi, ysi)],
                            points[index(xp, y, zp, xsi, ysi)],
                            points[index(xp, yp, zp, xsi, ysi)],
                            points[index(x, yp, zp, xsi, ysi)],
                        },
                        x, y, z
                    };
                }
            }
        }
    }

    vector<Face*> construct(GLfloat level) {
        if(level < 0.01) {
            level = 0.01;
        }
        if(level > 0.99) {
            level = 0.99;
        }
        GLfloat xmu = 1.0f / raw_dimension[0];
        GLfloat ymu = 1.0f / raw_dimension[1];
        GLfloat zmu = 1.0f / raw_dimension[2];
        glm::vec3 mu(xmu, ymu, zmu);

        for (size_t i = 0; i < cells.size(); ++i) {
            int cubeIndex = 0;
            glm::vec3 vertList[12];
            if(cells[i].val[0] < level) cubeIndex |= 1;
            if(cells[i].val[1] < level) cubeIndex |= 2;
            if(cells[i].val[2] < level) cubeIndex |= 4;
            if(cells[i].val[3] < level) cubeIndex |= 8;
            if(cells[i].val[4] < level) cubeIndex |= 16;
            if(cells[i].val[5] < level) cubeIndex |= 32;
            if(cells[i].val[6] < level) cubeIndex |= 64;
            if(cells[i].val[7] < level) cubeIndex |= 128;

            if(MCEdgeTable[cubeIndex] == 0) continue;
            if(MCEdgeTable[cubeIndex] & 1)
                vertList[0] = vertexLinear(level, cells[i], 0, 1);
            if(MCEdgeTable[cubeIndex] & 2)
                vertList[1] = vertexLinear(level, cells[i], 1, 2);
            if(MCEdgeTable[cubeIndex] & 4)
                vertList[2] = vertexLinear(level, cells[i], 2, 3);
            if(MCEdgeTable[cubeIndex] & 8)
                vertList[3] = vertexLinear(level, cells[i], 3, 0);
            if(MCEdgeTable[cubeIndex] & 16)
                vertList[4] = vertexLinear(level, cells[i], 4, 5);
            if(MCEdgeTable[cubeIndex] & 32)
                vertList[5] = vertexLinear(level, cells[i], 5, 6);
            if(MCEdgeTable[cubeIndex] & 64)
                vertList[6] = vertexLinear(level, cells[i], 6, 7);
            if(MCEdgeTable[cubeIndex] & 128)
                vertList[7] = vertexLinear(level, cells[i], 7, 4);
            if(MCEdgeTable[cubeIndex] & 256)
                vertList[8] = vertexLinear(level, cells[i], 0, 4);
            if(MCEdgeTable[cubeIndex] & 512)
                vertList[9] = vertexLinear(level, cells[i], 1, 5);
            if(MCEdgeTable[cubeIndex] & 1024)
                vertList[10] = vertexLinear(level, cells[i], 2, 6);
            if(MCEdgeTable[cubeIndex] & 2048)
                vertList[11] = vertexLinear(level, cells[i], 3, 7);

                    /* cout<<"new cell"<< " "; */
            for(size_t k = 0; MCTriTable[cubeIndex][k] != -1; k += 3) {
                Face *f = new Face();
                for(size_t j = 0; j < 3; j++) {
                    int vId = MCTriTable[cubeIndex][k+j];
                    int edgeId = 0;
                    int x = cells[i].x;
                    int y = cells[i].y;
                    int z = cells[i].z;
                    int dx = cells_dimension[0] + 1;
                    int dy = cells_dimension[1] + 1;
                    if(vId == 0) {
                        edgeId = index(x, y, z, dx, dy) * 3 + 0;
                    } else if(vId == 3) {
                        edgeId = index(x, y, z, dx, dy) * 3 + 1;
                    } else if(vId == 8) {
                        edgeId = index(x, y, z, dx, dy) * 3 + 2;
                    } else if(vId == 4) {
                        edgeId = index(x, y, z+1, dx, dy) * 3 + 0;
                    } else if(vId == 7) {
                        edgeId = index(x, y, z+1, dx, dy) * 3 + 1;
                    } else if(vId == 1) {
                        edgeId = index(x+1, y, z, dx, dy) * 3 + 1;
                    } else if(vId == 9) {
                        edgeId = index(x+1, y, z, dx, dy) * 3 + 2;
                    } else if(vId == 2) {
                        edgeId = index(x, y+1, z, dx, dy) * 3 + 0;
                    } else if(vId == 11) {
                        edgeId = index(x, y+1, z, dx, dy) * 3 + 2;
                    } else if(vId == 10) {
                        edgeId = index(x+1, y+1, z, dx, dy) * 3 + 2;
                    } else if(vId == 5) {
                        edgeId = index(x+1, y, z+1, dx, dy) * 3 + 1;
                    } else if(vId == 6) {
                        edgeId = index(x, y+1, z+1, dx, dy) * 3 + 0;
                    }
                    /* cout<<vId<<">"<<edgeId<< " "; */
                    Intersection *point = intersections[edgeId];
                    if(!point) {
                        point = new Intersection();
                        intersections[edgeId] = point;
                    }
                    point->position = vertList[vId];
                    point->faceList.push_back(f);
                    f->iList[j] = point;
                }
                
                glm::vec3 cross = glm::cross(f->iList[1]->position - f->iList[0]->position, f->iList[2]->position - f->iList[0]->position);
                f->normal = glm::normalize(cross);
                faces.push_back(f);
            }
        }

        for(auto const& i : intersections) {
            Intersection *point = i.second;
            glm::vec3 normal(0.0f);
            /* cout << "int" <<endl; */
            for(size_t j = 0; j < point->faceList.size(); j++) {
                /* cout << point->faceList[j]->normal.x <<" "; */
                /* cout << point->faceList[j]->normal.y <<" "; */
                /* cout << point->faceList[j]->normal.z <<endl; */
                normal += point->faceList[j]->normal;// *1.0f/point->faceList[j]->area;
                /* normal = point->faceList[j]->normal; */
            }
            point->normal = glm::normalize(normal);
            point->position *= mu;
        }

        return faces;
    }

    void cleanUp() {
        for(auto const& i : intersections) {
            delete i.second;
        }
        intersections.clear();
        for(size_t i = 0; i < faces.size(); i++) {
            delete faces[i];
        }
        faces.clear();
    }


private:
    GLfloat trilinear(GLfloat x, GLfloat y, GLfloat z) {
        /* GLfloat sx = x * (raw_dimension[0] - 1); */
        /* GLfloat sy = y * (raw_dimension[1] - 1); */
        /* GLfloat sz = z * (raw_dimension[2] - 1); */
        int v0x = floor(x);
        int v0y = floor(y);
        int v0z = floor(z);
        if(v0x >= raw_dimension[0]-1)
            v0x = raw_dimension[0]-2;
        if(v0y >= raw_dimension[1]-1)
            v0y = raw_dimension[1]-2;
        if(v0z >= raw_dimension[2]-1)
            v0z = raw_dimension[2]-2;
        GLfloat lx = x - v0x; 
        GLfloat ly = y - v0y; 
        GLfloat lz = z - v0z; 

        /* cout << sx <<endl<< sy<<endl << sz <<endl; */
        /* cout << lx <<endl<< ly<<endl << lz <<endl; */
        /* cout << v0x <<endl<< v0y<<endl << v0z <<endl; */
        /* cout << raw(v0x, v0y, v0z)<<endl; */

        GLfloat vxyz = raw(v0x, v0y, v0z)*(1-lx)*(1-ly)*(1-lz) +
            raw(v0x+1, v0y, v0z)*lx*(1-ly)*(1-lz) +
            raw(v0x+1, v0y+1, v0z)*lx*ly*(1-lz) +
            raw(v0x+1, v0y+1, v0z+1)*lx*ly*lz +
            raw(v0x, v0y+1, v0z+1)*(1-lx)*ly*lz +
            raw(v0x, v0y, v0z+1)*(1-lx)*(1-ly)*lz +
            raw(v0x+1, v0y, v0z+1)*lx*(1-ly)*lz +
            raw(v0x, v0y+1, v0z)*(1-lx)*ly*(1-lz);

        return vxyz / 255.0f;
    }

    GLfloat raw(int x, int y, int z) {
        size_t index = z*raw_dimension[0]*raw_dimension[1] + y*raw_dimension[0] + x;
        return raw_data[index];
    }

    int index(int x, int y, int z, int xsi, int ysi) {
        return xsi*ysi*z + xsi*y + x;
    }

    glm::vec3 vertexLinear(GLfloat level, Cell cell, int p1, int p2) {
        GLfloat pos = (level - cell.val[p1]) / (cell.val[p2] - cell.val[p1]);
        return cell.p[p1] + pos * (cell.p[p2] - cell.p[p1]);
    }

    GLubyte* load_3d_raw_data(std::string texture_path) {
        FILE *fp;
        GLubyte *data = new GLubyte[raw_size];			  // 8bit
        if (!(fp = fopen(texture_path.c_str(), "rb"))) {
            std::cout << "Error: opening .raw file failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        if (fread(data, sizeof(char), raw_size, fp) != raw_size) {
            std::cout << "Error: read .raw file failed" << std::endl;
            exit(1);
        }
        fclose(fp);
        return data;
    }
};

#endif
