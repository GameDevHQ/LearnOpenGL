#ifndef TANGENTSPACE_H
#define TANGENTSPACE_H
#include <vector>
#include <glm/glm.hpp>


void computeTangentBasis(
    // inputs
    std::vector<glm::vec3> & vertices,
    std::vector<glm::vec2> & uvs,
    std::vector<glm::vec3> & normals,
    // outputs
    std::vector<glm::vec3> & tangents,
    std::vector<glm::vec3> & bitangents
);

#endif
