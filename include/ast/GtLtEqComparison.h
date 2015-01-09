namespace wake::ast {

	class GtLtEqComparison : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
