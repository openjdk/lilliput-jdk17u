/*
 * Copyright (c) 2006, 2019, Oracle and/or its affiliates. All rights reserved.
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

#ifndef SHARE_OOPS_MARKWORD_INLINE_HPP
#define SHARE_OOPS_MARKWORD_INLINE_HPP

#include "oops/markWord.hpp"

#include "oops/klass.hpp"
#include "oops/oop.inline.hpp"
#include "runtime/globals.hpp"
#include "runtime/safepoint.hpp"

// Should this header be preserved during GC?
inline bool markWord::must_be_preserved(const oopDesc* obj) const {
  if (UseBiasedLocking) {
    if (has_bias_pattern()) {
      // Will reset bias at end of collection
      // Mark words of biased and currently locked objects are preserved separately
      return false;
    }
    markWord prototype_header = prototype_for_klass(obj->klass());
    if (prototype_header.has_bias_pattern()) {
      // Individual instance which has its bias revoked; must return
      // true for correctness
      return true;
    }
  }
  return (!is_unlocked() || !has_no_hash());
}

// Should this header be preserved in the case of a promotion failure during scavenge?
inline bool markWord::must_be_preserved_for_promotion_failure(const oopDesc* obj) const {
  if (UseBiasedLocking) {
    // We don't explicitly save off the mark words of biased and
    // currently-locked objects during scavenges, so if during a
    // promotion failure we encounter either a biased mark word or a
    // klass which still has a biasable prototype header, we have to
    // preserve the mark word. This results in oversaving, but promotion
    // failures are rare, and this avoids adding more complex logic to
    // the scavengers to call new variants of
    // BiasedLocking::preserve_marks() / restore_marks() in the middle
    // of a scavenge when a promotion failure has first been detected.
    if (has_bias_pattern() || prototype_for_klass(obj->klass()).has_bias_pattern()) {
      return true;
    }
  }
  return (!is_unlocked() || !has_no_hash());
}

inline markWord markWord::prototype_for_klass(const Klass* klass) {
  markWord prototype_header = klass->prototype_header();
  assert(prototype_header == prototype() || prototype_header.has_bias_pattern(), "corrupt prototype header");

  return prototype_header;
}

#ifdef _LP64
narrowKlass markWord::narrow_klass() const {
  assert(UseCompactObjectHeaders, "only used with compact object headers");
  return narrowKlass(value() >> klass_shift);
}

Klass* markWord::klass() const {
  assert(UseCompactObjectHeaders, "only used with compact object headers");
  assert(!CompressedKlassPointers::is_null(narrow_klass()), "narrow klass must not be null: " INTPTR_FORMAT, value());
  return CompressedKlassPointers::decode_not_null(narrow_klass());
}

Klass* markWord::klass_or_null() const {
  assert(UseCompactObjectHeaders, "only used with compact object headers");
  return CompressedKlassPointers::decode(narrow_klass());
}

markWord markWord::set_narrow_klass(const narrowKlass nklass) const {
  assert(UseCompactObjectHeaders, "only used with compact object headers");
  return markWord((value() & ~klass_mask_in_place) | ((uintptr_t) nklass << klass_shift));
}

Klass* markWord::safe_klass() const {
  assert(UseCompactObjectHeaders, "only used with compact object headers");
  assert(SafepointSynchronize::is_at_safepoint(), "only call at safepoint");
  markWord m = *this;
  if (m.has_displaced_mark_helper()) {
    m = m.displaced_mark_helper();
  }
  return CompressedKlassPointers::decode_not_null(m.narrow_klass());
}

markWord markWord::set_klass(const Klass* klass) const {
  assert(UseCompactObjectHeaders, "only used with compact object headers");
  assert(UseCompressedClassPointers, "expect compressed klass pointers");
  // TODO: Don't cast to non-const, change CKP::encode() to accept const Klass* instead.
  narrowKlass nklass = CompressedKlassPointers::encode(const_cast<Klass*>(klass));
  return set_narrow_klass(nklass);
}
#endif

#endif // SHARE_OOPS_MARKWORD_INLINE_HPP
