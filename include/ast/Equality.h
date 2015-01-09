namespace wake::ast {

	class Equality : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
