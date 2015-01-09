namespace wake::ast {

	class Retrieval : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
