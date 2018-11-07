#include "methods.hh"
#include "../visitors/visitor.hh"

MethodDeclaration::~MethodDeclaration() {
	delete body;
	for (auto param: parameters) {
		delete param;
	}
}
void MethodDeclaration::accept(ASTvisitor& V) {
	V.visit(*this);
}
std::string MethodDeclaration::to_string() {
	std::string res = value_type_to_string(return_type) + " " + name;
	res += "(";
	bool first = true;
	for (auto param: parameters) {
		if (!first) res += ", ";
		res += value_type_to_string(param->type);
		first = false;
	}
	res += ")";
	return res;
}

MethodCall::~MethodCall() {
	for (ASTnode* arg: this->arguments) {
		delete arg;
	}
}
void MethodCall::accept(ASTvisitor& V) {
	V.visit(*this);
}

void CalloutCall::accept(ASTvisitor& V) {
	V.visit(*this);
}
