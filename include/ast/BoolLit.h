namespace wake::ast {

	class BoolLit : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
