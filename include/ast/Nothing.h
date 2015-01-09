namespace wake::ast {

	class Nothing : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
