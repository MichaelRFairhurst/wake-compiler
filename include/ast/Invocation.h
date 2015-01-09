namespace wake::ast {

	class Invocation : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
