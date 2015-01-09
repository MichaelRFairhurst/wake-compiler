namespace wake::ast {

	class ArrayDeclaration : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
