namespace wake::ast {

	class Ternary : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
