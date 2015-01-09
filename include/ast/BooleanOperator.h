namespace wake::ast {

	class BooleanOperator : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
