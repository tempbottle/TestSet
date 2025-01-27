// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "courgette/ensemble.h"

// #include "base/basictypes.h"
// #include "base/strings/string_number_conversions.h"

#include "courgette/region.h"
#include "courgette/simple_delta.h"
#include "courgette/streams.h"

namespace courgette {

Element::Element(ExecutableType kind,
                 Ensemble* ensemble,
                 const Region& region)
    : kind_(kind), ensemble_(ensemble), region_(region) {
}

Element::~Element() {}

std::string Element::Name() const {
//   return ensemble_->name() + "("
//       + base::IntToString(kind()) + ","
//       + base::Uint64ToString(offset_in_ensemble()) + ","
//       + base::Uint64ToString(region().length()) + ")";

  char temp[200] = "";
  sprintf_s(temp, sizeof(temp), "(%d,%I64u,%I64u)", kind(), offset_in_ensemble(), region().length());
  return ensemble_->name() + temp;
}

// Scans the Ensemble's region, sniffing out Elements.  We assume that the
// elements do not overlap.
Status Ensemble::FindEmbeddedElements() {

  size_t length = region_.length();
  const uint8* start = region_.start();

  size_t position = 0;
  while (position < length) {
    ExecutableType type;
    size_t detected_length;

    Status result = DetectExecutableType(start + position,
                                         length - position,
                                         &type, &detected_length);

    if (result == C_OK) {
      Region region(start + position, detected_length);

      Element* element = new Element(type, this, region);
      owned_elements_.push_back(element);
      elements_.push_back(element);
      position += region.length();
    } else {
      position++;
    }
  }
  return C_OK;
}

Ensemble::~Ensemble() {
  for (size_t i = 0;  i < owned_elements_.size();  ++i)
    delete owned_elements_[i];
}

}  // namespace
