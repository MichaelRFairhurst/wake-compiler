namespace wake::ast {

	class ArrayAccess : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
