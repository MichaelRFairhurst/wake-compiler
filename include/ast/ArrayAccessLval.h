namespace wake::ast {

	class ArrayAccessLval : public ArrayAccess {

		public:
			Type* typeCheck(bool forceArrayIdentifier);

	}

}
