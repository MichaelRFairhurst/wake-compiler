namespace wake::ast {

	class Assignment : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
