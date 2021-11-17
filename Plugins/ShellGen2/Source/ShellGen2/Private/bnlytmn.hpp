#ifndef BNLYTMN_HPP
#define BNLYTMN_HPP

#include "Containers/Array.h"
#include "Math/Vector2D.h"

struct LineSeg {
  FVector2D a, b;
  bool operator==(const LineSeg& other) const {
    return a == other.a && b == other.b;
  }
};

TArray<FVector2D> bnlytmn(const TArray<LineSeg>& segments);

#endif
