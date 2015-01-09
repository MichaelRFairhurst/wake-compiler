namespace wake::ast {

	class StringLit : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
