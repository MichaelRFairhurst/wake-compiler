namespace wake::ast {

	class TypeSafeArrayAccess : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
