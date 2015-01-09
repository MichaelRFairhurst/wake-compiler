namespace wake::ast {

	class MemberAccess : public MemberAccessBase {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
