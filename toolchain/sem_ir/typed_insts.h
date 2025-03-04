// Part of the Carbon Language project, under the Apache License v2.0 with LLVM
// Exceptions. See /LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef CARBON_TOOLCHAIN_SEM_IR_TYPED_INSTS_H_
#define CARBON_TOOLCHAIN_SEM_IR_TYPED_INSTS_H_

#include "toolchain/parse/tree.h"
#include "toolchain/sem_ir/builtin_kind.h"
#include "toolchain/sem_ir/ids.h"
#include "toolchain/sem_ir/inst_kind.h"

// Representations for specific kinds of instructions.
//
// Each type should be a struct with up to four members:
//
// - Optionally, a `Parse::NodeId parse_node;` member, for instructions with an
//   associated location. Almost all instructions should have this, with
//   exceptions being things that are generated internally, without any relation
//   to source syntax, such as predeclared builtins.
// - Optionally, a `TypeId type_id;` member, for instructions that produce a
//   value. This includes instructions that produce an abstract value, such as a
//   `Namespace`, for which a placeholder type should be used.
// - Up to two `[...]Id` members describing the contents of the struct.
//
// The field names here matter -- the first two fields must have the names
// specified above, when present. When converting to a `SemIR::Inst`, they will
// become the parse node and type associated with the type-erased instruction.
//
// In addition, each type provides a constant `Kind` that associates the type
// with a particular member of the `InstKind` enumeration. This `Kind`
// declaration also defines the instruction kind by calling `InstKind::Define`
// and specifying additional information about the instruction kind. This
// information is available through the member functions of the `InstKind` value
// declared in `inst_kind.h`, and includes the name used in textual IR and
// whether the instruction is a terminator instruction.
namespace Carbon::SemIR {

struct AddressOf {
  static constexpr auto Kind = InstKind::AddressOf.Define("address_of");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId lvalue_id;
};

struct AddrPattern {
  static constexpr auto Kind = InstKind::AddrPattern.Define("addr_pattern");

  Parse::NodeId parse_node;
  TypeId type_id;
  // The `self` parameter.
  InstId inner_id;
};

struct ArrayIndex {
  static constexpr auto Kind = InstKind::ArrayIndex.Define("array_index");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId array_id;
  InstId index_id;
};

// Initializes an array from a tuple. `tuple_id` is the source tuple
// expression. `inits_id` contains one initializer per array element.
// `dest_id` is the destination array object for the initialization.
struct ArrayInit {
  static constexpr auto Kind = InstKind::ArrayInit.Define("array_init");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId inits_id;
  InstId dest_id;
};

struct ArrayType {
  static constexpr auto Kind = InstKind::ArrayType.Define("array_type");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId bound_id;
  TypeId element_type_id;
};

// Performs a source-level initialization or assignment of `lhs_id` from
// `rhs_id`. This finishes initialization of `lhs_id` in the same way as
// `InitializeFrom`.
struct Assign {
  static constexpr auto Kind = InstKind::Assign.Define("assign");

  Parse::NodeId parse_node;
  // Assignments are statements, and so have no type.
  InstId lhs_id;
  InstId rhs_id;
};

// A base in a class, of the form `base: base_type;`. A base class is an
// element of the derived class, and the type of the `BaseDecl` instruction is
// an `UnboundElementType`.
struct BaseDecl {
  static constexpr auto Kind = InstKind::BaseDecl.Define("base_decl");

  Parse::NodeId parse_node;
  TypeId type_id;
  TypeId base_type_id;
  ElementIndex index;
};

struct BindName {
  static constexpr auto Kind = InstKind::BindName.Define("bind_name");

  Parse::NodeId parse_node;
  TypeId type_id;
  NameId name_id;
  InstId value_id;
};

struct BindValue {
  static constexpr auto Kind = InstKind::BindValue.Define("bind_value");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId value_id;
};

struct BlockArg {
  static constexpr auto Kind = InstKind::BlockArg.Define("block_arg");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId block_id;
};

struct BoolLiteral {
  static constexpr auto Kind = InstKind::BoolLiteral.Define("bool_literal");

  Parse::NodeId parse_node;
  TypeId type_id;
  BoolValue value;
};

// A bound method, that combines a function with the value to use for its
// `self` parameter, such as `object.MethodName`.
struct BoundMethod {
  static constexpr auto Kind = InstKind::BoundMethod.Define("bound_method");

  Parse::NodeId parse_node;
  TypeId type_id;
  // The object argument in the bound method, which will be used to initialize
  // `self`, or whose address will be used to initialize `self` for an `addr
  // self` parameter.
  InstId object_id;
  InstId function_id;
};

struct Branch {
  static constexpr auto Kind =
      InstKind::Branch.Define("br", TerminatorKind::Terminator);

  Parse::NodeId parse_node;
  // Branches don't produce a value, so have no type.
  InstBlockId target_id;
};

struct BranchIf {
  static constexpr auto Kind =
      InstKind::BranchIf.Define("br", TerminatorKind::TerminatorSequence);

  Parse::NodeId parse_node;
  // Branches don't produce a value, so have no type.
  InstBlockId target_id;
  InstId cond_id;
};

struct BranchWithArg {
  static constexpr auto Kind =
      InstKind::BranchWithArg.Define("br", TerminatorKind::Terminator);

  Parse::NodeId parse_node;
  // Branches don't produce a value, so have no type.
  InstBlockId target_id;
  InstId arg_id;
};

struct Builtin {
  static constexpr auto Kind = InstKind::Builtin.Define("builtin");

  // Builtins don't have a parse node associated with them.
  TypeId type_id;
  BuiltinKind builtin_kind;
};

struct Call {
  static constexpr auto Kind = InstKind::Call.Define("call");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId callee_id;
  // The arguments block contains IDs for the following arguments, in order:
  //  - The argument for each implicit parameter.
  //  - The argument for each explicit parameter.
  //  - The argument for the return slot, if present.
  InstBlockId args_id;
};

struct ClassDecl {
  static constexpr auto Kind = InstKind::ClassDecl.Define("class_decl");

  Parse::NodeId parse_node;
  // No type: a class declaration is not itself a value. The name of a class
  // declaration becomes a class type value.
  // TODO: For a generic class declaration, the name of the class declaration
  // should become a parameterized entity name value.
  ClassId class_id;
  // The declaration block, containing the class name's qualifiers and the
  // class's generic parameters.
  InstBlockId decl_block_id;
};

struct ClassElementAccess {
  static constexpr auto Kind =
      InstKind::ClassElementAccess.Define("class_element_access");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId base_id;
  ElementIndex index;
};

struct ClassInit {
  static constexpr auto Kind = InstKind::ClassInit.Define("class_init");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId elements_id;
  InstId dest_id;
};

struct ClassType {
  static constexpr auto Kind = InstKind::ClassType.Define("class_type");

  Parse::NodeId parse_node;
  TypeId type_id;
  ClassId class_id;
  // TODO: Once we support generic classes, include the class's arguments here.
};

struct ConstType {
  static constexpr auto Kind = InstKind::ConstType.Define("const_type");

  Parse::NodeId parse_node;
  TypeId type_id;
  TypeId inner_id;
};

struct Converted {
  static constexpr auto Kind = InstKind::Converted.Define("converted");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId original_id;
  InstId result_id;
};

// A cross-reference between IRs.
struct CrossRef {
  static constexpr auto Kind = InstKind::CrossRef.Define("xref");

  // No parse node: an instruction's parse tree node must refer to a node in the
  // current parse tree. This cannot use the cross-referenced instruction's
  // parse tree node because it will be in a different parse tree.
  TypeId type_id;
  CrossRefIRId ir_id;
  InstId inst_id;
};

struct Deref {
  static constexpr auto Kind = InstKind::Deref.Define("deref");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId pointer_id;
};

// A field in a class, of the form `var field: field_type;`. The type of the
// `FieldDecl` instruction is an `UnboundElementType`.
struct FieldDecl {
  static constexpr auto Kind = InstKind::FieldDecl.Define("field_decl");

  Parse::NodeId parse_node;
  TypeId type_id;
  NameId name_id;
  ElementIndex index;
};

struct FunctionDecl {
  static constexpr auto Kind = InstKind::FunctionDecl.Define("fn_decl");

  Parse::NodeId parse_node;
  TypeId type_id;
  FunctionId function_id;
};

// An import corresponds to some number of IRs. The range of imported IRs is
// inclusive of last_cross_ref_ir_id, and will always be non-empty. If
// there was an import error, first_cross_ref_ir_id will reference a
// nullptr IR; there should only ever be one nullptr in the range.
struct Import {
  static constexpr auto Kind = InstKind::Import.Define("import");

  Parse::NodeId parse_node;
  TypeId type_id;
  CrossRefIRId first_cross_ref_ir_id;
  CrossRefIRId last_cross_ref_ir_id;
};

// Finalizes the initialization of `dest_id` from the initializer expression
// `src_id`, by performing a final copy from source to destination, for types
// whose initialization is not in-place.
struct InitializeFrom {
  static constexpr auto Kind =
      InstKind::InitializeFrom.Define("initialize_from");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId src_id;
  InstId dest_id;
};

struct InterfaceDecl {
  static constexpr auto Kind = InstKind::InterfaceDecl.Define("interface_decl");

  Parse::NodeId parse_node;
  // No type: an interface declaration is not itself a value. The name of an
  // interface declaration becomes a facet type value.
  // TODO: For a generic interface declaration, the name of the interface
  // declaration should become a parameterized entity name value.
  InterfaceId interface_id;
  // The declaration block, containing the interface name's qualifiers and the
  // interface's generic parameters.
  InstBlockId decl_block_id;
};

struct IntLiteral {
  static constexpr auto Kind = InstKind::IntLiteral.Define("int_literal");

  Parse::NodeId parse_node;
  TypeId type_id;
  IntId int_id;
};

// This instruction is not intended for direct use. Instead, it should be loaded
// if it's referenced through name resolution.
struct LazyImportRef {
  static constexpr auto Kind =
      InstKind::LazyImportRef.Define("lazy_import_ref");

  // No parse node: an instruction's parse tree node must refer to a node in the
  // current parse tree. This cannot use the cross-referenced instruction's
  // parse tree node because it will be in a different parse tree.

  CrossRefIRId ir_id;
  InstId inst_id;
};

struct NameRef {
  static constexpr auto Kind = InstKind::NameRef.Define("name_ref");

  Parse::NodeId parse_node;
  TypeId type_id;
  NameId name_id;
  InstId value_id;
};

struct Namespace {
  static constexpr auto Kind = InstKind::Namespace.Define("namespace");

  Parse::NodeId parse_node;
  TypeId type_id;
  NameScopeId name_scope_id;
};

struct NoOp {
  static constexpr auto Kind = InstKind::NoOp.Define("no_op");

  Parse::NodeId parse_node;
  // This instruction doesn't produce a value, so has no type.
};

struct Param {
  static constexpr auto Kind = InstKind::Param.Define("param");

  Parse::NodeId parse_node;
  TypeId type_id;
  NameId name_id;
};

struct PointerType {
  static constexpr auto Kind = InstKind::PointerType.Define("ptr_type");

  Parse::NodeId parse_node;
  TypeId type_id;
  TypeId pointee_id;
};

struct RealLiteral {
  static constexpr auto Kind = InstKind::RealLiteral.Define("real_literal");

  Parse::NodeId parse_node;
  TypeId type_id;
  RealId real_id;
};

struct Return {
  static constexpr auto Kind =
      InstKind::Return.Define("return", TerminatorKind::Terminator);

  Parse::NodeId parse_node;
  // This is a statement, so has no type.
};

struct ReturnExpr {
  static constexpr auto Kind =
      InstKind::ReturnExpr.Define("return", TerminatorKind::Terminator);

  Parse::NodeId parse_node;
  // This is a statement, so has no type.
  InstId expr_id;
};

struct SpliceBlock {
  static constexpr auto Kind = InstKind::SpliceBlock.Define("splice_block");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId block_id;
  InstId result_id;
};

struct StringLiteral {
  static constexpr auto Kind = InstKind::StringLiteral.Define("string_literal");

  Parse::NodeId parse_node;
  TypeId type_id;
  StringLiteralValueId string_literal_id;
};

struct StructAccess {
  static constexpr auto Kind = InstKind::StructAccess.Define("struct_access");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId struct_id;
  ElementIndex index;
};

struct StructInit {
  static constexpr auto Kind = InstKind::StructInit.Define("struct_init");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId elements_id;
  InstId dest_id;
};

struct StructLiteral {
  static constexpr auto Kind = InstKind::StructLiteral.Define("struct_literal");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId elements_id;
};

struct StructType {
  static constexpr auto Kind = InstKind::StructType.Define("struct_type");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId fields_id;
};

struct StructTypeField {
  static constexpr auto Kind =
      InstKind::StructTypeField.Define("struct_type_field");

  Parse::NodeId parse_node;
  // This instruction is an implementation detail of `StructType`, and doesn't
  // produce a value, so has no type, even though it declares a field with a
  // type.
  NameId name_id;
  TypeId field_type_id;
};

struct StructValue {
  static constexpr auto Kind = InstKind::StructValue.Define("struct_value");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId elements_id;
};

struct Temporary {
  static constexpr auto Kind = InstKind::Temporary.Define("temporary");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId storage_id;
  InstId init_id;
};

struct TemporaryStorage {
  static constexpr auto Kind =
      InstKind::TemporaryStorage.Define("temporary_storage");

  Parse::NodeId parse_node;
  TypeId type_id;
};

struct TupleAccess {
  static constexpr auto Kind = InstKind::TupleAccess.Define("tuple_access");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId tuple_id;
  ElementIndex index;
};

struct TupleIndex {
  static constexpr auto Kind = InstKind::TupleIndex.Define("tuple_index");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId tuple_id;
  InstId index_id;
};

struct TupleInit {
  static constexpr auto Kind = InstKind::TupleInit.Define("tuple_init");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId elements_id;
  InstId dest_id;
};

struct TupleLiteral {
  static constexpr auto Kind = InstKind::TupleLiteral.Define("tuple_literal");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId elements_id;
};

struct TupleType {
  static constexpr auto Kind = InstKind::TupleType.Define("tuple_type");

  Parse::NodeId parse_node;
  TypeId type_id;
  TypeBlockId elements_id;
};

struct TupleValue {
  static constexpr auto Kind = InstKind::TupleValue.Define("tuple_value");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstBlockId elements_id;
};

struct UnaryOperatorNot {
  static constexpr auto Kind = InstKind::UnaryOperatorNot.Define("not");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId operand_id;
};

// The type of an expression naming an unbound element of a class, such as
// `Class.field`. This can be used as the operand of a compound member access
// expression, such as `instance.(Class.field)`.
struct UnboundElementType {
  static constexpr auto Kind =
      InstKind::UnboundElementType.Define("unbound_element_type");

  Parse::NodeId parse_node;
  TypeId type_id;
  // The class that a value of this type is an element of.
  TypeId class_type_id;
  // The type of the element.
  TypeId element_type_id;
};

struct ValueAsRef {
  static constexpr auto Kind = InstKind::ValueAsRef.Define("value_as_ref");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId value_id;
};

struct ValueOfInitializer {
  static constexpr auto Kind =
      InstKind::ValueOfInitializer.Define("value_of_initializer");

  Parse::NodeId parse_node;
  TypeId type_id;
  InstId init_id;
};

struct VarStorage {
  static constexpr auto Kind = InstKind::VarStorage.Define("var");

  Parse::NodeId parse_node;
  TypeId type_id;
  NameId name_id;
};

// HasParseNodeMember<T> is true if T has a `Parse::NodeId parse_node` field.
template <typename T, typename ParseNodeType = Parse::NodeId T::*>
inline constexpr bool HasParseNodeMember = false;
template <typename T>
inline constexpr bool HasParseNodeMember<T, decltype(&T::parse_node)> = true;

// HasTypeIdMember<T> is true if T has a `TypeId type_id` field.
template <typename T, typename TypeIdType = TypeId T::*>
inline constexpr bool HasTypeIdMember = false;
template <typename T>
inline constexpr bool HasTypeIdMember<T, decltype(&T::type_id)> = true;

}  // namespace Carbon::SemIR

#endif  // CARBON_TOOLCHAIN_SEM_IR_TYPED_INSTS_H_
