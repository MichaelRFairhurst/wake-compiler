namespace wake::ast {

	class NumberLit : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
