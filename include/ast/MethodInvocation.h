namespace wake::ast {

	class MethodInvocation : public MethodInvocationBase {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
