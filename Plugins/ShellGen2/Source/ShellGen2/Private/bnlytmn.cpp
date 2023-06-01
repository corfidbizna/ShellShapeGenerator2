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

#include "bnlytmn.hpp"

#include <cmath>
#include <queue>
#include <algorithm>
#include <functional>

// Shamelessly stealing my own Rust implementation of Bentley-Ottmann, then
// gutting it to make the creatively named Bnlytmn algorithm. (A more proper
// name for it would probably be "Reduced Bentley-Ottmann".) -SB

namespace {
  float twice_triangle_area(FVector2D a, FVector2D b, FVector2D c) {
    return (b.X - a.X) * (c.Y - a.Y) - (c.X - a.X) * (b.Y - a.Y);
  }
  // Returns which side of line a→b that c is on. -1 = left, 1 = right, 0 =
  // coincident.
  int side_of_line(FVector2D a, FVector2D b, FVector2D c) {
    auto area = twice_triangle_area(a, b, c);
    if(area < 0) return -1;
    else if(area > 0) return 1;
    else return 0;
  }
  bool has_intersection(LineSeg a, LineSeg b) {
    auto& a1 = a.a;
    auto& a2 = a.b;
    auto& b1 = b.a;
    auto& b2 = b.b;
    // Shared endpoint = assumed non-intersection.
    if(a1 == b1 || a1 == b2 || a2 == b1 || a2 == b2) return false;
    auto b1side = side_of_line(a1, a2, b1);
    auto b2side = side_of_line(a1, a2, b2);
    if(b1side == b2side) {
      // If both points of line B are on the same side of line A, then there
      // can't be an intersection.
      return false;
    }
    auto a1side = side_of_line(b1, b2, a1);
    auto a2side = side_of_line(b1, b2, a2);
    if(a1side == a2side) {
      // If both points of line A are on the same side of line B, then, again,
      // no possible intersection.
      return false;
    }
    // The lines cross!
    // (Note: This function may sometimes get false intersections, but only if
    // the infinite extrapolation of one line segment intersects with the other
    // line segment. The rest of the algorithm precludes this function from
    // being called in circumstances where that can matter.)
    return true;
  }
  FVector2D get_intersection(LineSeg a, LineSeg b) {
    auto& a1 = a.a;
    auto& a2 = a.b;
    auto& b1 = b.a;
    auto& b2 = b.b;
    // the normal of a1→a2
    FVector2D an(a2.Y - a1.Y, a1.X - a2.X);
    float b1dot = std::fabs(an | (b1 - a1));
    float b2dot = std::fabs(an | (b2 - a1));
    float totaldot = b1dot + b2dot;
    return b1 + (b2-b1) * b1dot / totaldot;
  }
  // Uh oh. My Rust code used Rust enums, which are pretty schway. C++ doesn't
  // have those.
  enum class BnlytmnEventType { RIGHT_ENDPOINT, LEFT_ENDPOINT };
  struct CandidateEvent {
    FVector2D p;
    LineSeg seg;
    BnlytmnEventType type;
    static CandidateEvent left(FVector2D p, LineSeg a) {
      return CandidateEvent{p, a, BnlytmnEventType::LEFT_ENDPOINT};
    }
    static CandidateEvent right(FVector2D p, LineSeg a) {
      return CandidateEvent{p, a, BnlytmnEventType::RIGHT_ENDPOINT};
    }
    bool operator>(const CandidateEvent& other) const {
      if(p.X == other.p.X) {
        if(type == other.type) {
          return p.Y > other.p.Y;
        }
        else return type > other.type;
      }
      else {
        return p.X > other.p.X;
      }
    }
  };
  typedef std::priority_queue<CandidateEvent, std::vector<CandidateEvent>,
                              std::greater<CandidateEvent>> EventQueue;
}

TArray<FVector2D> bnlytmn(const TArray<LineSeg>& segments) {
  TArray<FVector2D> ret;
  EventQueue queue;
  for(auto& line : segments) {
    if(line.a.X > line.b.X || (line.a.X == line.b.X && line.a.Y > line.b.Y)) {
      queue.push(CandidateEvent::left(line.b, line));
      queue.push(CandidateEvent::right(line.a, line));
    }
    else {
      queue.push(CandidateEvent::left(line.a, line));
      queue.push(CandidateEvent::right(line.b, line));
    }
  }
  std::vector<LineSeg> active;
  active.reserve(8); // one allocation that will definitely be enough for SSG2
  while(!queue.empty()) {
    auto event = queue.top();
    queue.pop();
    auto x = event.p.X;
    auto y = event.p.Y;
    switch(event.type) {
    case BnlytmnEventType::LEFT_ENDPOINT: {
      auto& seg = event.seg;
      for(size_t n = 0; n < active.size(); ++n) {
        auto& other = active[n];
        if(has_intersection(seg, other)) {
          ret.Add(get_intersection(seg, other));
        }
      }
      active.emplace_back(seg);
    } break;
    case BnlytmnEventType::RIGHT_ENDPOINT: {
      size_t index;
      auto& seg = event.seg;
      for(index = 0; index < active.size(); ++index) {
        if(active[index] == seg) goto ok;
      }
      // Line is already not in the list, nothing to do.
      break;
      ok:
      active.erase(active.begin() + index);
    } break;
    }
  }
  return ret;
}
