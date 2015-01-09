namespace wake::ast {

	class MethodInvocationBase : public ExpressionNode {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
