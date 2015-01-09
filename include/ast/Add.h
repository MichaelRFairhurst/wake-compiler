namespace wake::ast {

	class Add : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
