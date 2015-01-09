namespace wake::ast {

	class AddAssignment : public Add {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
