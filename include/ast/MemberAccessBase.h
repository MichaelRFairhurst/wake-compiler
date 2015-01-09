namespace wake::ast {

	class MemberAccess : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
