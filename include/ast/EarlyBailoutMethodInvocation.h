namespace wake::ast {

	class EarlyBailoutMethodInvocation : public MethodInvocationBase {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
