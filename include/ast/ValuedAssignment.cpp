namespace wake::ast {

	class ValuedAssignment : public Assignment {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
