// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "courgette/assembly_program.h"

#include <memory.h>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <vector>

// #include "base/logging.h"
// #include "base/memory/scoped_ptr.h"

#include "courgette/courgette.h"
#include "courgette/encoded_program.h"

namespace courgette {

// Opcodes of simple assembly language
enum OP {
  ORIGIN,         // ORIGIN <rva> - set current address for assembly.
  MAKEPERELOCS,   // Generates a base relocation table.
  MAKEELFRELOCS,  // Generates a base relocation table.
  DEFBYTE,        // DEFBYTE <value> - emit a byte literal.
  REL32,          // REL32 <label> - emit a rel32 encoded reference to 'label'.
  ABS32,          // REL32 <label> - emit am abs32 encoded reference to 'label'.
  LAST_OP
};

// Base class for instructions.  Because we have so many instructions we want to
// keep them as small as possible.  For this reason we avoid virtual functions.
//
class Instruction {
 public:
  OP op() const { return static_cast<OP>(op_); }

 protected:
  explicit Instruction(OP op) : op_(op), info_(0) {}
  Instruction(OP op, unsigned int info) : op_(op), info_(info) {}

  uint32 op_   : 4;    // A few bits to store the OP code.
  uint32 info_ : 28;   // Remaining bits in first word available to subclass.

 private:
//  DISALLOW_COPY_AND_ASSIGN(Instruction);
};

namespace {

// Sets the current address for the emitting instructions.
class OriginInstruction : public Instruction {
 public:
  explicit OriginInstruction(RVA rva) : Instruction(ORIGIN, 0), rva_(rva) {}
  RVA origin_rva() const { return rva_; }
 private:
  RVA  rva_;
};

// Emits an entire PE base relocation table.
class PeRelocsInstruction : public Instruction {
 public:
  PeRelocsInstruction() : Instruction(MAKEPERELOCS) {}
};

// Emits an ELF relocation table.
class ElfRelocsInstruction : public Instruction {
 public:
  ElfRelocsInstruction() : Instruction(MAKEELFRELOCS) {}
};

// Emits a single byte.
class ByteInstruction : public Instruction {
 public:
  explicit ByteInstruction(uint8 value) : Instruction(DEFBYTE, value) {}
  uint8 byte_value() const { return info_; }
};

// A ABS32 to REL32 instruction emits a reference to a label's address.
class InstructionWithLabel : public Instruction {
 public:
  InstructionWithLabel(OP op, Label* label)
    : Instruction(op, 0), label_(label) {
//    if (label == NULL) NOTREACHED();
  }
  Label* label() const { return label_; }
 private:
  Label* label_;
};

}  // namespace

AssemblyProgram::AssemblyProgram()
  : image_base_(0) {
}

static void DeleteContainedLabels(const RVAToLabel& labels) {
  for (RVAToLabel::const_iterator p = labels.begin();  p != labels.end();  ++p)
    delete p->second;
}

AssemblyProgram::~AssemblyProgram() {
  for (size_t i = 0;  i < instructions_.size();  ++i) {
    Instruction* instruction = instructions_[i];
    if (instruction->op() != DEFBYTE)  // Will be in byte_instruction_cache_.
      delete instruction;
  }
  if (byte_instruction_cache_./*get*/size()) {
    for (size_t i = 0;  i < 256;  ++i)
      delete byte_instruction_cache_[i];
  }
  DeleteContainedLabels(rel32_labels_);
  DeleteContainedLabels(abs32_labels_);
}

CheckBool AssemblyProgram::EmitPeRelocsInstruction() {
  return Emit(new(std::nothrow) PeRelocsInstruction());
}

CheckBool AssemblyProgram::EmitElfRelocationInstruction() {
  return Emit(new(std::nothrow) ElfRelocsInstruction());
}

CheckBool AssemblyProgram::EmitOriginInstruction(RVA rva) {
  return Emit(new(std::nothrow) OriginInstruction(rva));
}

CheckBool AssemblyProgram::EmitByteInstruction(uint8 byte) {
//  return Emit(GetByteInstruction(byte));
  return Emit(new(std::nothrow) ByteInstruction(byte));
}

CheckBool AssemblyProgram::EmitRel32(Label* label) {
  return Emit(new(std::nothrow) InstructionWithLabel(REL32, label));
}

CheckBool AssemblyProgram::EmitAbs32(Label* label) {
  return Emit(new(std::nothrow) InstructionWithLabel(ABS32, label));
}

Label* AssemblyProgram::FindOrMakeAbs32Label(RVA rva) {
  return FindLabel(rva, &abs32_labels_);
}

Label* AssemblyProgram::FindOrMakeRel32Label(RVA rva) {
  return FindLabel(rva, &rel32_labels_);
}

void AssemblyProgram::DefaultAssignIndexes() {
  DefaultAssignIndexes(&abs32_labels_);
  DefaultAssignIndexes(&rel32_labels_);
}

void AssemblyProgram::UnassignIndexes() {
  UnassignIndexes(&abs32_labels_);
  UnassignIndexes(&rel32_labels_);
}

void AssemblyProgram::AssignRemainingIndexes() {
  AssignRemainingIndexes(&abs32_labels_);
  AssignRemainingIndexes(&rel32_labels_);
}

Label* AssemblyProgram::InstructionAbs32Label(
    const Instruction* instruction) const {
  if (instruction->op() == ABS32)
    return static_cast<const InstructionWithLabel*>(instruction)->label();
  return NULL;
}

Label* AssemblyProgram::InstructionRel32Label(
    const Instruction* instruction) const {
  if (instruction->op() == REL32)
    return static_cast<const InstructionWithLabel*>(instruction)->label();
  return NULL;
}

CheckBool AssemblyProgram::Emit(Instruction* instruction) {
  if (!instruction)
    return false;
  bool ok = instructions_.push_back(instruction);
  if (!ok)
    delete instruction;
  return ok;
}

Label* AssemblyProgram::FindLabel(RVA rva, RVAToLabel* labels) {
  Label*& slot = (*labels)[rva];
  if (slot == NULL) {
    slot = new(std::nothrow) Label(rva);
  }
  return slot;
}

void AssemblyProgram::UnassignIndexes(RVAToLabel* labels) {
  for (RVAToLabel::iterator p = labels->begin();  p != labels->end();  ++p) {
    Label* current = p->second;
    current->index_ = Label::kNoIndex;
  }
}

// DefaultAssignIndexes takes a set of labels and assigns indexes in increasing
// address order.
//
void AssemblyProgram::DefaultAssignIndexes(RVAToLabel* labels) {
  int index = 0;
  for (RVAToLabel::iterator p = labels->begin();  p != labels->end();  ++p) {
    Label* current = p->second;
//     if (current->index_ != Label::kNoIndex)
//       NOTREACHED();
    current->index_ = index;
    ++index;
  }
}

// AssignRemainingIndexes assigns indexes to any addresses (labels) that are not
// yet assigned an index.
//
void AssemblyProgram::AssignRemainingIndexes(RVAToLabel* labels) {
  // An address table compresses best when each index is associated with an
  // address that is slight larger than the previous index.

  // First see which indexes have not been used.  The 'available' vector could
  // grow even bigger, but the number of addresses is a better starting size
  // than empty.
  std::vector<bool> available(labels->size(), true);
  int used = 0;

  for (RVAToLabel::iterator p = labels->begin();  p != labels->end();  ++p) {
    int index = p->second->index_;
    if (index != Label::kNoIndex) {
      while (static_cast<size_t>(index) >= available.size())
        available.push_back(true);
      available.at(index) = false;
      ++used;
    }
  }

  //VLOG(1) << used << " of " << labels->size() << " labels pre-assigned";
  printf("%d of %d labels pre-assigned\n");

  // Are there any unused labels that happen to be adjacent following a used
  // label?
  //
  int fill_forward_count = 0;
  Label* prev = 0;
  for (RVAToLabel::iterator p = labels->begin();  p != labels->end();  ++p) {
    Label* current = p->second;
    if (current->index_ == Label::kNoIndex) {
      int index = 0;
      if (prev  &&  prev->index_ != Label::kNoIndex)
        index = prev->index_ + 1;
      if (index < static_cast<int>(available.size()) && available.at(index)) {
        current->index_ = index;
        available.at(index) = false;
        ++fill_forward_count;
      }
    }
    prev = current;
  }

  // Are there any unused labels that happen to be adjacent preceeding a used
  // label?
  //
  int fill_backward_count = 0;
  prev = 0;
  for (RVAToLabel::reverse_iterator p = labels->rbegin();
       p != labels->rend();
       ++p) {
    Label* current = p->second;
    if (current->index_ == Label::kNoIndex) {
      int prev_index;
      if (prev)
        prev_index = prev->index_;
      else
        prev_index = static_cast<uint32>(available.size());
      if (prev_index != 0  &&
          prev_index != Label::kNoIndex  &&
          available.at(prev_index - 1)) {
        current->index_ = prev_index - 1;
        available.at(current->index_) = false;
        ++fill_backward_count;
      }
    }
    prev = current;
  }

  // Fill in any remaining indexes
  int fill_infill_count = 0;
  int index = 0;
  for (RVAToLabel::iterator p = labels->begin();  p != labels->end();  ++p) {
    Label* current = p->second;
    if (current->index_ == Label::kNoIndex) {
      while (!available.at(index)) {
        ++index;
      }
      current->index_ = index;
      available.at(index) = false;
      ++index;
      ++fill_infill_count;
    }
  }

//   VLOG(1) << "  fill forward " << fill_forward_count
//           << "  backward " << fill_backward_count
//           << "  infill " << fill_infill_count;

  printf("  fill forward:%d  backward:%d infill:%d\n", 
      fill_forward_count,
      fill_backward_count,
      fill_infill_count);
}

typedef CheckBool (EncodedProgram::*DefineLabelMethod)(int index, RVA value);

#if defined(OS_WIN)
__declspec(noinline)
#endif
static CheckBool DefineLabels(const RVAToLabel& labels,
                              EncodedProgram* encoded_format,
                              DefineLabelMethod define_label) {
  bool ok = true;
  for (RVAToLabel::const_iterator p = labels.begin();
       ok && p != labels.end();
       ++p) {
    Label* label = p->second;
    ok = (encoded_format->*define_label)(label->index_, label->rva_);
  }
  return ok;
}

EncodedProgram* AssemblyProgram::Encode() const {
//   scoped_ptr<EncodedProgram> encoded(new(std::nothrow) EncodedProgram());
//   if (!encoded.get())
//     return NULL;

  EncodedProgram *encoded = new(std::nothrow) EncodedProgram();

  encoded->set_image_base(image_base_);

  if (!DefineLabels(abs32_labels_, encoded/*.get()*/,
                    &EncodedProgram::DefineAbs32Label) ||
      !DefineLabels(rel32_labels_, encoded/*.get()*/,
                    &EncodedProgram::DefineRel32Label)) {
    return NULL;
  }

  encoded->EndLabels();

  for (size_t i = 0;  i < instructions_.size();  ++i) {
    Instruction* instruction = instructions_[i];

    switch (instruction->op()) {
      case ORIGIN: {
        OriginInstruction* org = static_cast<OriginInstruction*>(instruction);
        if (!encoded->AddOrigin(org->origin_rva()))
          return NULL;
        break;
      }
      case DEFBYTE: {
        uint8 b = static_cast<ByteInstruction*>(instruction)->byte_value();
        if (!encoded->AddCopy(1, &b))
          return NULL;
        break;
      }
      case REL32: {
        Label* label = static_cast<InstructionWithLabel*>(instruction)->label();
        if (!encoded->AddRel32(label->index_))
          return NULL;
        break;
      }
      case ABS32: {
        Label* label = static_cast<InstructionWithLabel*>(instruction)->label();
        if (!encoded->AddAbs32(label->index_))
          return NULL;
        break;
      }
      case MAKEPERELOCS: {
        if (!encoded->AddPeMakeRelocs())
          return NULL;
        break;
      }
      case MAKEELFRELOCS: {
        if (!encoded->AddElfMakeRelocs())
          return NULL;
        break;
      }
      default: {
//        NOTREACHED() << "Unknown Insn OP kind";
      }
    }
  }

//  return encoded.release();
  return encoded;
}

Instruction* AssemblyProgram::GetByteInstruction(uint8 byte) {
  if (!byte_instruction_cache_./*get*/size()) {
//     byte_instruction_cache_.reset(new(std::nothrow) Instruction*[256]);
//     if (!byte_instruction_cache_.get())
//       return NULL;

    for (int i = 0; i < 256; ++i) {
      //byte_instruction_cache_[i] =
      byte_instruction_cache_.push_back(
          new(std::nothrow) ByteInstruction(static_cast<uint8>(i)));

      if (!byte_instruction_cache_[i]) {
        for (int j = 0; j < i; ++j)
          delete byte_instruction_cache_[j];
        //byte_instruction_cache_.reset();
        return NULL;
      }
    }
  }

  return byte_instruction_cache_[byte];
}

////////////////////////////////////////////////////////////////////////////////

Status Encode(AssemblyProgram* program, EncodedProgram** output) {
  *output = NULL;
  EncodedProgram *encoded = program->Encode();
  if (encoded) {
    *output = encoded;
    return C_OK;
  } else {
    return C_GENERAL_ERROR;
  }
}

}  // namespace courgette
