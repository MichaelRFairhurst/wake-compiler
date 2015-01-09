namespace wake::ast {

	class Cast : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
