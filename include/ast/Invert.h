namespace wake::ast {

	class Invert : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
