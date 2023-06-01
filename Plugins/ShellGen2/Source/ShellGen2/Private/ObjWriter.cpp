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

#include "ObjWriter.h"

#include <sstream>

static FString cook_path(const FString& filename) {
  return FPaths::ProjectContentDir() + TEXT("/") + filename;
}

void UObjWriter::OutputObjFile(const TArray<FString>& comments,
                               const TArray<FBakedMesh>& meshes,
                               const TArray<FTransform>& transforms,
                               const FString& filename,
                               bool& saving_succeeded,
                               FString& failure_reason) {
  failure_reason = "";
  saving_succeeded = false;
  std::ostringstream o;
  for(auto& comment : comments) {
    auto str = std::string(TCHAR_TO_UTF8(*comment));
    auto p = str.cbegin();
    o << "# ";
    while(p != str.cend()) {
      if(*p == '\r') ++p;
      else if(*p == '\n') { o << "\n# "; ++p; }
      else o << *p++;
    }
    o << "\n";
  }
  size_t index_offset = 1;
  for(int m = 0; m < meshes.Num(); ++m) {
    if(meshes.Num() != 1)
      o << "\n### Mesh " << m << "\no mesh" << m << "\ng mesh" << m << "\n";
    o << "\n# Vertices\n";
    auto& mesh = meshes[m];
    if(!mesh.vertices) {
      failure_reason = "A mesh was NULL";
      return;
    }
    if(!mesh.indices) {
      failure_reason = "A mesh had null indices";
      return;
    }
    if(!mesh.texcoords) {
      failure_reason = "A mesh had null texcoords";
      return;
    }
    auto& vertices = *mesh.vertices;
    auto& indices = *mesh.indices;
    auto& texcoords = *mesh.texcoords;
    if(m < transforms.Num()) {
      auto& transform = transforms[m];
      for(unsigned int n = 0; n < vertices.size(); ++n) {
        FVector& in = vertices[n];
        FVector out = TransformVector(transform, in);
        o << "v " << out.X << " " << out.Y << " " << out.Z << "\n";
      }
    }
    else {
      for(unsigned int n = 0; n < vertices.size(); ++n) {
        FVector& in = vertices[n];
        o << "v " << in.X << " " << in.Y << " " << in.Z << "\n";
      }
    }
    o << "\n# Texture coordinates\n";
    for(unsigned int n = 0; n < texcoords.size(); ++n) {
      o << "vt " << texcoords[n].X << " " << texcoords[n].Y << "\n";
    }
    o << "\n# Faces\n";
    for(unsigned int n = 0; n+2 < indices.size(); n += 3) {
      o << "f " << indices[n]+index_offset << "/"
        << indices[n]+index_offset << " "
        << indices[n+1]+index_offset << "/"
        << indices[n+1]+index_offset << " "
        << indices[n+2]+index_offset << "/"
        << indices[n+2]+index_offset << "\n";
    }
    index_offset += vertices.size() / 3;
  }
  auto all = o.str();
  IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
  FString path = cook_path(filename);
  if(!path.EndsWith(FString(".obj"))) path += ".obj";
  std::unique_ptr<IFileHandle> file(PlatformFile.OpenWrite(*path));
  if(!file || !file->Write(reinterpret_cast<const uint8_t*>(all.data()),
                           all.length())) {
    failure_reason = "Unable to write file";
    return;
  }
  saving_succeeded = true;
}
