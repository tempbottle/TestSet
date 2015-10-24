// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COURGETTE_DISASSEMBLER_ELF_32_X86_H_
#define COURGETTE_DISASSEMBLER_ELF_32_X86_H_

//#include "base/basictypes.h"
#include "disassembler_elf_32.h"
#include "memory_allocator.h"
#include "types_elf.h"

namespace courgette {

class AssemblyProgram;

class DisassemblerElf32X86 : public DisassemblerElf32 {
 public:
  explicit DisassemblerElf32X86(const void* start, size_t length);

  virtual ExecutableType kind() { return EXE_ELF_32_X86; }

  virtual e_machine_values ElfEM() { return EM_386; }

 protected:
  virtual CheckBool RelToRVA(Elf32_Rel rel, RVA* result)
      const WARN_UNUSED_RESULT;

  virtual CheckBool ParseRelocationSection(
      const Elf32_Shdr *section_header,
      AssemblyProgram* program) WARN_UNUSED_RESULT;

  virtual CheckBool ParseRel32RelocsFromSection(
      const Elf32_Shdr* section) WARN_UNUSED_RESULT;

//  DISALLOW_COPY_AND_ASSIGN(DisassemblerElf32X86);
};

}  // namespace courgette

#endif  // COURGETTE_DISASSEMBLER_ELF_32_X86_H_
