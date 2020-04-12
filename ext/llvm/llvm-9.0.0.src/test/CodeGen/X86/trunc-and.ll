; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=x86_64-- -mattr=avx512bw | FileCheck %s

; This would infinite loop by trying to truncate to i16 and then promote back to i32.

define i16 @PR40793(<8 x i16> %t1) {
; CHECK-LABEL: PR40793:
; CHECK:       # %bb.0:
; CHECK-NEXT:    vpextrw $5, %xmm0, %ecx
; CHECK-NEXT:    andb $15, %cl
; CHECK-NEXT:    movl $32733, %eax # imm = 0x7FDD
; CHECK-NEXT:    # kill: def $cl killed $cl killed $ecx
; CHECK-NEXT:    shrl %cl, %eax
; CHECK-NEXT:    # kill: def $ax killed $ax killed $eax
; CHECK-NEXT:    retq
  %t2 = and <8 x i16> %t1, <i16 15, i16 15, i16 15, i16 15, i16 15, i16 15, i16 15, i16 15>
  %t3 = lshr <8 x i16> <i16 25156, i16 -14844, i16 -26779, i16 -16956, i16 -13978, i16 32733, i16 -24894, i16 18530>, %t2
  %t4 = bitcast <8 x i16> %t3 to <2 x double>
  %t5 = call <2 x double> @llvm.fabs.v2f64(<2 x double> %t4)
  %t6 = bitcast <2 x double> %t5 to <8 x i16>
  %t7 = extractelement <8 x i16> %t6, i64 5
  ret i16 %t7
}

declare <2 x double> @llvm.fabs.v2f64(<2 x double>)
