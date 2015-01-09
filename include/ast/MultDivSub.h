namespace wake::ast {

	class MultDivSub : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
