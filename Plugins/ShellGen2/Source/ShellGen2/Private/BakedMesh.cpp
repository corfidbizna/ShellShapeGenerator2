/*
 * This file is part of Shell Shape Generator 2.
 *
 * Copyright ©2023 Olivia Jenkins
 *
 * Shell Shape Generator 2 is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Shell Shape Generator 2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Shell Shape Generator 2. If not, see <https://www.gnu.org/licenses/>.
 */

#include "BakedMesh.h"

std::vector<FVector> FBakedMesh::calculate_normals() const {
  std::vector<FVector> normals(vertices->size());
  for(auto&& n : normals) {
    n = FVector(0.0f);
  }
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

// It's been 19 years since the last time I did this, so I confess I had to use
// a reference: <https://stackoverflow.com/a/5257471>
void FBakedMesh::build_tangent_space
(const TArray<FVertexInstanceID>& viid_map,
 TMeshAttributesRef<FVertexInstanceID, FVector>& out_normals,
 TMeshAttributesRef<FVertexInstanceID, FVector>& out_tangents,
 TMeshAttributesRef<FVertexInstanceID, float>& out_binormal_signs) const {
  std::vector<FVector> normals(vertices->size());
  std::vector<FVector> tangents(vertices->size());
  std::vector<FVector> binormals(vertices->size());
  for(auto&& n : normals) n = FVector(0.0f);
  for(auto&& t : tangents) t = FVector(0.0f);
  for(auto&& b : binormals) b = FVector(0.0f);
  /* For each triangle... */
  for(auto it = indices->cbegin(); it != indices->cend();) {
    /* (The three points of the triangle) */
    auto a_index = *it++;
    auto b_index = *it++;
    auto c_index = *it++;
    auto& a = (*vertices)[a_index];
    auto& b = (*vertices)[b_index];
    auto& c = (*vertices)[c_index];
    auto& h = (*texcoords)[a_index];
    auto& k = (*texcoords)[b_index];
    auto& l = (*texcoords)[c_index];
    auto d = b-a;
    auto e = c-a;
    auto f = k-h;
    auto g = l-h;
    /* Calculate the face normal */
    auto n = FVector::CrossProduct(d, e);
    normals[a_index] += n;
    normals[b_index] += n;
    normals[c_index] += n;
    /* Calculate the actual tangent and binormal */
    float fs = f.X;
    float ft = f.Y;
    float gs = g.X;
    float gt = g.Y;
    float det = 1.0f / ((fs * gt) - (ft * gs));
    FVector t = det * (gt * d + -ft * e);
    FVector u = det * (-gs * d + fs * e);
    FVector tprime = t - (n | t) * n;
    FVector uprime = u - (n | u) * n - (tprime | u) * tprime;
    tangents[a_index] += tprime;
    tangents[b_index] += tprime;
    tangents[c_index] += tprime;
    binormals[a_index] += uprime;
    binormals[b_index] += uprime;
    binormals[c_index] += uprime;
  }
  for(unsigned int i = 0; i < vertices->size(); ++i) {
    /* Output = normalized input */
    FVector n = normals[i];
    FVector t = tangents[i];
    FVector u = binormals[i];
    n.Normalize(1.0 / 131072.0);
    t.Normalize(1.0 / 131072.0);
    u.Normalize(1.0 / 131072.0);
    // handedness, not sign, sigh.
    float u_sign = ((n ^ t) | u) < 0.0 ? -1.0f : 1.0f;
    out_normals.Set(viid_map[i], n);
    out_tangents.Set(viid_map[i], t);
    out_binormal_signs.Set(viid_map[i], u_sign);
  }
}
