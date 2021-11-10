#include "BakedMesh.h"

std::vector<FVector> FBakedMesh::calculate_normals() const {
  std::vector<FVector> normals(vertices->size());
  /* Unnecessary; zero values are default-inserted. */
  /*
  for(auto&& n : normals) {
    n = FVector(0.0f);
  }
  */
  /* For each triangle... */
  for(auto it = indices->cbegin(); it != indices->cend();) {
    /* (The three points of the triangle) */
    auto a_index = *it++;
    auto b_index = *it++;
    auto c_index = *it++;
    auto& a = (*vertices)[a_index];
    auto& b = (*vertices)[b_index];
    auto& c = (*vertices)[c_index];
    auto d = b-a;
    auto e = c-a;
    /* and we have a normal! */
    auto n = FVector::CrossProduct(d, e);
    /* it's not a unit normal, but its magnitude is proportional to the area
       of the triangle. this provides free area-weighting of the contributions
       of each triangle to the resulting normal. I guess? */
    /* accumulate this at each of the three points */
    normals[a_index] += n;
    normals[b_index] += n;
    normals[c_index] += n;
  }
  for(auto it = normals.begin(); it != normals.end(); ++it) {
    it->Normalize(1.0 / 131072.0);
  }
  return normals;
}
