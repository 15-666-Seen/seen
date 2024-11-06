#include "WalkMesh.hpp"

#include "read_write_chunk.hpp"
#include "util.hpp"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <fstream>
#include <iostream>
#include <string>

WalkMesh::WalkMesh(std::vector<glm::vec3> const &vertices_,
                   std::vector<glm::vec3> const &normals_,
                   std::vector<glm::uvec3> const &triangles_)
    : vertices(vertices_), normals(normals_), triangles(triangles_) {

  // construct next_vertex map (maps each edge to the next vertex in the
  // triangle):
  next_vertex.reserve(triangles.size() * 3);
  auto do_next = [this](uint32_t a, uint32_t b, uint32_t c) {
    auto ret = next_vertex.insert(std::make_pair(glm::uvec2(a, b), c));
    assert(ret.second);
  };
  for (auto const &tri : triangles) {
    do_next(tri.x, tri.y, tri.z);
    do_next(tri.y, tri.z, tri.x);
    do_next(tri.z, tri.x, tri.y);
  }

  // DEBUG: are vertex normals consistent with geometric normals?
  for (auto const &tri : triangles) {
    glm::vec3 const &a = vertices[tri.x];
    glm::vec3 const &b = vertices[tri.y];
    glm::vec3 const &c = vertices[tri.z];
    glm::vec3 out = glm::normalize(glm::cross(b - a, c - a));

    float da = glm::dot(out, normals[tri.x]);
    float db = glm::dot(out, normals[tri.y]);
    float dc = glm::dot(out, normals[tri.z]);

    assert(da > 0.1f && db > 0.1f && dc > 0.1f);
  }
}

// project pt to the plane of triangle a,b,c and return the barycentric weights
// of the projected point:
glm::vec3 barycentric_weights(glm::vec3 const &a, glm::vec3 const &b,
                              glm::vec3 const &c, glm::vec3 const &pt) {
  glm::vec3 v0 = b - a;
  glm::vec3 v1 = c - a;
  glm::vec3 v2 = pt - a;

  float d00 = glm::dot(v0, v0);
  float d01 = glm::dot(v0, v1);
  float d11 = glm::dot(v1, v1);
  float d20 = glm::dot(v2, v0);
  float d21 = glm::dot(v2, v1);

  float denom = d00 * d11 - d01 * d01;

  float lambda1 = (d11 * d20 - d01 * d21) / denom;
  float lambda2 = (d00 * d21 - d01 * d20) / denom;
  float lambda0 = 1.0f - lambda1 - lambda2;

  return glm::vec3(lambda0, lambda1, lambda2);
}

WalkPoint WalkMesh::nearest_walk_point(glm::vec3 const &world_point) const {
  assert(!triangles.empty() && "Cannot start on an empty walkmesh");

  WalkPoint closest;
  float closest_dis2 = std::numeric_limits<float>::infinity();

  for (auto const &tri : triangles) {
    // find closest point on triangle:

    glm::vec3 const &a = vertices[tri.x];
    glm::vec3 const &b = vertices[tri.y];
    glm::vec3 const &c = vertices[tri.z];

    // get barycentric coordinates of closest point in the plane of (a,b,c):
    glm::vec3 coords = barycentric_weights(a, b, c, world_point);

    // is that point inside the triangle?
    if (coords.x >= 0.0f && coords.y >= 0.0f && coords.z >= 0.0f) {
      // yes, point is inside triangle.
      float dis2 =
          glm::length2(world_point - to_world_point(WalkPoint(tri, coords)));
      if (dis2 < closest_dis2) {
        closest_dis2 = dis2;
        closest.indices = tri;
        closest.weights = coords;
      }
    } else {
      // check triangle vertices and edges:
      auto check_edge = [&world_point, &closest, &closest_dis2,
                         this](uint32_t ai, uint32_t bi, uint32_t ci) {
        glm::vec3 const &a = vertices[ai];
        glm::vec3 const &b = vertices[bi];

        // find closest point on line segment ab:
        float along = glm::dot(world_point - a, b - a);
        float max = glm::dot(b - a, b - a);
        glm::vec3 pt;
        glm::vec3 coords;
        if (along < 0.0f) {
          pt = a;
          coords = glm::vec3(1.0f, 0.0f, 0.0f);
        } else if (along > max) {
          pt = b;
          coords = glm::vec3(0.0f, 1.0f, 0.0f);
        } else {
          float amt = along / max;
          pt = glm::mix(a, b, amt);
          coords = glm::vec3(1.0f - amt, amt, 0.0f);
        }

        float dis2 = glm::length2(world_point - pt);
        if (dis2 < closest_dis2) {
          closest_dis2 = dis2;
          closest.indices = glm::uvec3(ai, bi, ci);
          closest.weights = coords;
        }
      };
      check_edge(tri.x, tri.y, tri.z);
      check_edge(tri.y, tri.z, tri.x);
      check_edge(tri.z, tri.x, tri.y);
    }
  }
  assert(closest.indices.x < vertices.size());
  assert(closest.indices.y < vertices.size());
  assert(closest.indices.z < vertices.size());
  return closest;
}

void WalkMesh::walk_in_triangle(WalkPoint const &start, glm::vec3 const &step,
                                WalkPoint *end_, float *time_) const {
  assert(end_);
  auto &end = *end_;
  assert(time_);
  auto &time = *time_;

  glm::vec3 const &a = vertices[start.indices.x];
  glm::vec3 const &b = vertices[start.indices.y];
  glm::vec3 const &c = vertices[start.indices.z];

  glm::vec3 step_coords = to_world_point(start) + step;
  glm::vec3 end_weights = barycentric_weights(a, b, c, step_coords);
  glm::vec3 v = end_weights - start.weights;

  // what is the first edge crossing of
  // start.weights + t * v   (0 <= t <= 1)

  float t_min = 2.0f;

  time = 1.0f;
  end.weights = end_weights;
  end.indices = start.indices;

  for (uint16_t i = 0; i < 3; ++i) {
    if (end_weights[i] < 0.0f && v[i] < 0.0f) {
      float t = (0.0f - start.weights[i]) / v[i];
      if (t < t_min) {
        t_min = t;
        time = t;
        // note: start.indices[c] move to end
        uint8_t c1 = (i + 1) % 3;
        uint8_t c2 = (i + 2) % 3;
        end.indices[0] = start.indices[c1];
        end.indices[1] = start.indices[c2];
        end.indices[2] = start.indices[i];
        end.weights[0] = start.weights[c1] + time * v[c1];
        end.weights[1] = start.weights[c2] + time * v[c2];
        end.weights[2] = 0.0f;
      }
    }
  }
}

bool WalkMesh::cross_edge(WalkPoint const &start, WalkPoint *end_,
                          glm::quat *rotation_) const {
  assert(end_);
  auto &end = *end_;

  assert(rotation_);
  auto &rotation = *rotation_;

  assert(start.weights.z == 0.0f); // *must* be on an edge.

  // Get the edge indices (assuming start.weights.z == 0 means we are on the
  // edge opposite to the third vertex)
  glm::uvec2 edge = glm::uvec2(
      start.indices.x,
      start.indices.y); // Assuming start.weights.z corresponds to edge (x, y)

  // Check if 'edge' is a non-boundary edge:
  auto adjacent = next_vertex.find(glm::uvec2(
      edge.y, edge.x)); // Reverse the edge to find the adjacent triangle
  if (adjacent != next_vertex.end()) {
    // It is a non-boundary edge! Cross into the adjacent triangle.
    glm::uvec3 adj_triangle =
        glm::uvec3(edge.y, edge.x,
                   adjacent->second); // The indices of the adjacent triangle

    // Find the "third" vertex in the adjacent triangle (the one that's not part
    // of the edge)
    int third_vertex = (adj_triangle.x != edge.x && adj_triangle.x != edge.y)
                           ? adj_triangle.x
                       : (adj_triangle.y != edge.x && adj_triangle.y != edge.y)
                           ? adj_triangle.y
                           : adj_triangle.z;

    // Update 'end' to reflect the same point in the new triangle:
    end.indices = glm::uvec3(edge.y, edge.x, third_vertex); // The new triangle

    // The barycentric coordinates in the new triangle will swap:
    // We need to swap the barycentric coordinates associated with the edge
    // vertices
    end.weights = glm::vec3(start.weights.y, start.weights.x,
                            0.0f); // Swap the first two coordinates

    // Compute the normals of the original and new triangles:
    glm::vec3 normal_start = glm::normalize(
        glm::cross(vertices[start.indices.y] - vertices[start.indices.x],
                   vertices[start.indices.z] - vertices[start.indices.x]));
    glm::vec3 normal_end = glm::normalize(
        glm::cross(vertices[end.indices.y] - vertices[end.indices.x],
                   vertices[end.indices.z] - vertices[end.indices.x]));

    // Calculate the quaternion rotation that takes the original triangle's
    // normal to the new triangle's normal:
    rotation = glm::rotation(normal_start, normal_end);
    rotation =
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity rotation (no rotation)

    return true;
  } else {
    // This is a boundary edge; we cannot cross it.
    end = start;
    rotation =
        glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity rotation (no rotation)
    return false;
  }
}

WalkMeshes::WalkMeshes(std::string const &filename) {
  std::ifstream file(filename, std::ios::binary);

  std::vector<glm::vec3> vertices;
  read_chunk(file, "p...", &vertices);

  std::vector<glm::vec3> normals;
  read_chunk(file, "n...", &normals);

  std::vector<glm::uvec3> triangles;
  read_chunk(file, "tri0", &triangles);

  std::vector<char> names;
  read_chunk(file, "str0", &names);

  struct IndexEntry {
    uint32_t name_begin, name_end;
    uint32_t vertex_begin, vertex_end;
    uint32_t triangle_begin, triangle_end;
  };

  std::vector<IndexEntry> index;
  read_chunk(file, "idxA", &index);

  if (file.peek() != EOF) {
    wait_and_exit("WARNING: trailing data in walkmesh file '" + filename + "'");
    std::cerr << "WARNING: trailing data in walkmesh file '" << filename << "'"
              << std::endl;
  }

  //-----------------

  if (vertices.size() != normals.size()) {
    throw std::runtime_error("Mis-matched position and normal sizes in '" +
                             filename + "'");
  }

  for (auto const &e : index) {
    if (!(e.name_begin <= e.name_end && e.name_end <= names.size())) {
      throw std::runtime_error("Invalid name indices in index of '" + filename +
                               "'");
    }
    if (!(e.vertex_begin <= e.vertex_end && e.vertex_end <= vertices.size())) {
      throw std::runtime_error("Invalid vertex indices in index of '" +
                               filename + "'");
    }
    if (!(e.triangle_begin <= e.triangle_end &&
          e.triangle_end <= triangles.size())) {
      throw std::runtime_error("Invalid triangle indices in index of '" +
                               filename + "'");
    }

    // copy vertices/normals:
    std::vector<glm::vec3> wm_vertices(vertices.begin() + e.vertex_begin,
                                       vertices.begin() + e.vertex_end);
    std::vector<glm::vec3> wm_normals(normals.begin() + e.vertex_begin,
                                      normals.begin() + e.vertex_end);

    // remap triangles:
    std::vector<glm::uvec3> wm_triangles;
    wm_triangles.reserve(e.triangle_end - e.triangle_begin);
    for (uint32_t ti = e.triangle_begin; ti != e.triangle_end; ++ti) {
      if (!((e.vertex_begin <= triangles[ti].x &&
             triangles[ti].x < e.vertex_end) &&
            (e.vertex_begin <= triangles[ti].y &&
             triangles[ti].y < e.vertex_end) &&
            (e.vertex_begin <= triangles[ti].z &&
             triangles[ti].z < e.vertex_end))) {
        wait_and_exit("Invalid triangle in '" + filename + "'");
        throw std::runtime_error("Invalid triangle in '" + filename + "'");
      }
      wm_triangles.emplace_back(triangles[ti].x - e.vertex_begin,
                                triangles[ti].y - e.vertex_begin,
                                triangles[ti].z - e.vertex_begin);
    }

    std::string name(names.begin() + e.name_begin, names.begin() + e.name_end);

    auto ret =
        meshes.emplace(name, WalkMesh(wm_vertices, wm_normals, wm_triangles));
    if (!ret.second) {
      wait_and_exit("WalkMesh with duplicated name '" + name + "' in '" +
                    filename + "'");
      throw std::runtime_error("WalkMesh with duplicated name '" + name +
                               "' in '" + filename + "'");
    }
  }
}

WalkMesh const &WalkMeshes::lookup(std::string const &name) const {
  auto f = meshes.find(name);
  if (f == meshes.end()) {
    wait_and_exit("WalkMesh with name '" + name + "' not found.");
    throw std::runtime_error("WalkMesh with name '" + name + "' not found.");
  }
  return f->second;
}