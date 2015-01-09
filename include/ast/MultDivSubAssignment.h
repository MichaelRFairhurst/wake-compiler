namespace wake::ast {

	class MultDivSubAssignment : public MultDivSub {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
