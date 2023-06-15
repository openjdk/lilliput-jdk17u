/*
 * Copyright (c) 2023, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_GC_G1_G1FULLGCCOMPACTIONPOINT_INLINE_HPP
#define SHARE_GC_G1_G1FULLGCCOMPACTIONPOINT_INLINE_HPP

#include "gc/g1/g1FullGCCompactionPoint.hpp"
#include "gc/g1/heapRegion.hpp"
#include "gc/shared/slidingForwarding.inline.hpp"
#include "oops/oop.inline.hpp"

template<bool ALT_FWD>
void G1FullGCCompactionPoint::forward(oop object, size_t size) {
  assert(_current_region != NULL, "Must have been initialized");

  // Ensure the object fit in the current region.
  while (!object_will_fit(size)) {
    switch_region();
  }

  // Store a forwarding pointer if the object should be moved.
  if (cast_from_oop<HeapWord*>(object) != _compaction_top) {
    SlidingForwarding::forward_to<ALT_FWD>(object, cast_to_oop(_compaction_top));
  } else {
    assert(!SlidingForwarding::is_forwarded(object), "should not be forwarded");
    /*
    if (object->forwardee() != NULL) {
      // Object should not move but mark-word is used so it looks like the
      // object is forwarded. Need to clear the mark and it's no problem
      // since it will be restored by preserved marks. There is an exception
      // with BiasedLocking, in this case forwardee() will return NULL
      // even if the mark-word is used. This is no problem since
      // forwardee() will return NULL in the compaction phase as well.
      object->init_mark();
    } else {
      // Make sure object has the correct mark-word set or that it will be
      // fixed when restoring the preserved marks.
      assert(object->mark() == markWord::prototype_for_klass(object->klass()) || // Correct mark
             object->mark_must_be_preserved() || // Will be restored by PreservedMarksSet
             (UseBiasedLocking && object->has_bias_pattern()), // Will be restored by BiasedLocking
             "should have correct prototype obj: " PTR_FORMAT " mark: " PTR_FORMAT " prototype: " PTR_FORMAT,
             p2i(object), object->mark().value(), markWord::prototype_for_klass(object->klass()).value());
    }
    assert(object->forwardee() == NULL, "should be forwarded to NULL");
    */
  }

  // Update compaction values.
  _compaction_top += size;
  if (_compaction_top > _threshold) {
    _threshold = _current_region->cross_threshold(_compaction_top - size, _compaction_top);
  }
}

#endif // SHARE_GC_G1_G1FULLGCCOMPACTIONPOINT_INLINE_HPP
