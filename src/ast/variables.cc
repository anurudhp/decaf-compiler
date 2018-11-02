#include "variables.hh"
#include "../visitors/visitor.hh"

std::string value_type_to_string(const ValueType& vt) {
	if (vt == ValueType::VOID) return "void";
	if (vt == ValueType::INT) return "int";
	if (vt == ValueType::BOOL) return "bool";
	if (vt == ValueType::CHAR) return "char";
	if (vt == ValueType::STRING) return "string";
	if (vt == ValueType::INT_ARRAY) return "int_array";
	if (vt == ValueType::BOOL_ARRAY) return "bool_array";
	return "none";
}

Location::~Location() {
	delete index_expr;
}
void Location::accept(ASTvisitor& V) {
	V.visit(*this);
}

void VariableLocation::accept(ASTvisitor& V) {
	V.visit(*this);
}
void ArrayLocation::accept(ASTvisitor& V) {
	V.visit(*this);
}

std::string VariableDeclaration::to_string() {
	return value_type_to_string(type) + " " + id;
}

std::string ArrayDeclaration::to_string() {
	return VariableDeclaration::to_string() + "[" + std::to_string(array_len) + "]";
}