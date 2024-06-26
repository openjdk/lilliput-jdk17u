/*
 * Copyright (c) 2020, 2024, Oracle and/or its affiliates. All rights reserved.
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

#ifndef CPU_AARCH64_C2_MACROASSEMBLER_AARCH64_HPP
#define CPU_AARCH64_C2_MACROASSEMBLER_AARCH64_HPP

// C2_MacroAssembler contains high-level macros for C2

 public:
  // Code used by cmpFastLock and cmpFastUnlock mach instructions in .ad file.
  void fast_lock(Register object, Register box, Register tmp, Register tmp2, Register tmp3);
  void fast_unlock(Register object, Register box, Register tmp, Register tmp2);
  // Code used by cmpFastLockLightweight and cmpFastUnlockLightweight mach instructions in .ad file.
  void fast_lock_lightweight(Register object, Register t1, Register t2, Register t3);
  void fast_unlock_lightweight(Register object, Register t1, Register t2, Register t3);

  void string_compare(Register str1, Register str2,
                      Register cnt1, Register cnt2, Register result,
                      Register tmp1, Register tmp2, FloatRegister vtmp1,
                      FloatRegister vtmp2, FloatRegister vtmp3, int ae);

  void string_indexof(Register str1, Register str2,
                      Register cnt1, Register cnt2,
                      Register tmp1, Register tmp2,
                      Register tmp3, Register tmp4,
                      Register tmp5, Register tmp6,
                      int int_cnt1, Register result, int ae);

  void string_indexof_char(Register str1, Register cnt1,
                           Register ch, Register result,
                           Register tmp1, Register tmp2, Register tmp3);

  void stringL_indexof_char(Register str1, Register cnt1,
                           Register ch, Register result,
                           Register tmp1, Register tmp2, Register tmp3);

  // SIMD&FP comparison
  void neon_compare(FloatRegister dst, BasicType bt, FloatRegister src1,
                    FloatRegister src2, int cond, bool isQ);

  void load_nklass_compact(Register dst, Register obj, Register index, int scale, int disp);

#endif // CPU_AARCH64_C2_MACROASSEMBLER_AARCH64_HPP
