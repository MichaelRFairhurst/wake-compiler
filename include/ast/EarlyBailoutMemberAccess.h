namespace wake::ast {

	class EarlyBailoutMemberAccess : public MemberAccessBase {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
