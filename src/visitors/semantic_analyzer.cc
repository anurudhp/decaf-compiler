#include <cstdio>
#include <cstdarg>
#include <cassert>

#include "semantic_analyzer.hh"
#include "../ast/ast.hh"
// #include "../ast/literals.hh"
// #include "../ast/operators.hh"
#include "../ast/variables.hh"
// #include "../ast/statements.hh"
// #include "../ast/blocks.hh"
#include "../ast/methods.hh"
// #include "../ast/program.hh"
#include "../exceptions.hh"

/*** SemanticAnalyzer::SymbolTable ***/
SemanticAnalyzer::SymbolTable::SymbolTable() {
	// global scope
	scope_depth = 1;
	variables.emplace_back();
}

void SemanticAnalyzer::SymbolTable::block_start() {
	variables.emplace_back();
	scope_depth++;
}
void SemanticAnalyzer::SymbolTable::block_end() {
	assert (scope_depth > 1);
	variables.pop_back();
	scope_depth--;
}

// add
void SemanticAnalyzer::SymbolTable::add_variable(VariableDeclaration *variable, SemanticAnalyzer& analyzer) {
	auto& current_scope = variables.back();
	if (current_scope.count(variable->id)) {
		auto previous_decl = current_scope[variable->id];
		analyzer.log_error(1, variable->location,
						   "Redeclaration of variable `%s` (previously declared at [%s]: `%s %s`)",
						   variable->id.c_str(),
						   previous_decl->location.c_str(),
						   value_type_to_string(previous_decl->type).c_str(),
						   previous_decl->id.c_str());
		return;
	}
	
	if (scope_depth == 1 && methods.count(variable->id)) {
		// method name collisions only occur at global scope
		auto previous_decl = methods[variable->id];
		analyzer.log_error(1, variable->location,
						   "Invalid reuse of method name `%s` for variable (previously declared at [%s]: `%s`)",
						   variable->id.c_str(),
						   previous_decl->location.c_str(),
						   "");
		return;
	}

	current_scope[variable->id] = variable;
}

void SemanticAnalyzer::SymbolTable::add_method(MethodDeclaration *method, SemanticAnalyzer& analyzer) {
	if (methods.count(method->name)) {
		auto previous_decl = methods[method->name];
		analyzer.log_error(1, method->location,
						   "Reuse of method name `%s` (previously declared at [%s]: `%s`)",
						   method->name.c_str(),
						   previous_decl->location.c_str(),
						   "");
		return;
	}

	auto& global_scope = variables[0];
	if (global_scope.count(method->name)) {
		auto previous_decl = global_scope[method->name];
		analyzer.log_error(1, method->location,
						   "Invalid reuse of variable name `%s` for method (previously declared at [%s]: `%s %s`)",
						   method->name.c_str(),
						   previous_decl->location.c_str(),
						   value_type_to_string(previous_decl->type).c_str(),
						   previous_decl->id.c_str());
		return;
	}
	
	methods[method->name] = method;
}

// lookup
VariableDeclaration* SemanticAnalyzer::SymbolTable::lookup_variable(Location *varloc, SemanticAnalyzer& analyzer) {
	for (int i = scope_depth - 1; i >= 0; i--) {
		if (variables[i].count(varloc->id)) {
			auto decl = variables[i][varloc->id];
			// check access type: array/scalar
			// if (varloc->index_expr == NULL) {
				
			// }
			return decl;
		}
	}

	if (methods.count(varloc->id)) {
		analyzer.log_error(9, varloc->location,
						   "Invalid use of method `%s` as location (declared at [%s])",
						   varloc->id.c_str(),
						   methods[varloc->id]->location.c_str());
	} else {
		analyzer.log_error(2, varloc->location,
						   "Variable `%s` not declared",
						   varloc->id.c_str());
	}
	return NULL;
}
MethodDeclaration* SemanticAnalyzer::SymbolTable::lookup_method(MethodCall *mcall, SemanticAnalyzer& analyzer) {
	for (int i = scope_depth - 1; i >= 0; i--) {
		if (variables[i].count(mcall->id)) {
			auto decl = variables[i][mcall->id];
			analyzer.log_error(2, mcall->location,
							   "Invalid use of variable `%s` as method (declared at [%s]: `%s %s`",
							   mcall->id.c_str(),
							   decl->location.c_str(),
							   value_type_to_string(decl->type).c_str(),
							   decl->id.c_str());
			return NULL;
		}
	}

	if (methods.count(mcall->id)) {
		return methods[mcall->id];
	} 

	analyzer.log_error(2, mcall->location,
					   "Method `%s` not declared",
					   mcall->id.c_str());
	return NULL;
}



/*** SemanticAnalyzer ***/
void SemanticAnalyzer::log_error(const int error_type, 
								 const std::string& location,
								 const std::string& fmt,
								 ...) {
	va_list args;
	va_start(args, fmt);
	vsnprintf(_buffer, BUFFER_LENGTH, fmt.c_str(), args);
	va_end(args);

	// errors
	errors.emplace_back(error_type, location + " " + std::string(_buffer));
}

bool SemanticAnalyzer::check(ASTnode& root) {
	// root.visit(*this);

	return errors.empty();
}
void SemanticAnalyzer::display(std::ostream& out, const bool show_rules) {
	for (auto& err: errors) {
		if (show_rules) {
			out << "[Rule:" << err.first << "] ";
		}
		out << err.second << "\n";
	}
}

// Visit functions
void SemanticAnalyzer::visit(ASTnode& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

// literals.hh
void SemanticAnalyzer::visit(Literal& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(IntegerLiteral& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(BooleanLiteral& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(StringLiteral& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

// variables.hh
void SemanticAnalyzer::visit(Location& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(VariableLocation& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(ArrayLocation& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

// operators.hh
void SemanticAnalyzer::visit(UnaryOperator& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(BinaryOperator& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(ArithBinOperator& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(CondBinOperator& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(RelBinOperator& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(EqBinOperator& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(UnaryMinus& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(UnaryNot& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

// statements.hh
void SemanticAnalyzer::visit(ReturnStatement& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(BreakStatement& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(ContinueStatement& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(IfStatement& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(ForStatement& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(AssignStatement& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

// blocks.hh
void SemanticAnalyzer::visit(StatementBlock& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

// methods.hh
void SemanticAnalyzer::visit(MethodDeclaration& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(MethodCall& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
void SemanticAnalyzer::visit(CalloutCall& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}

// program.hh
void SemanticAnalyzer::visit(Program& node) {
	throw invalid_call_error(__PRETTY_FUNCTION__);
}
