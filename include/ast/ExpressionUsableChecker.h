namespace wake::ast {

	class ExpressionUsableCatcher : public ExpressionNode {

		public:
			ExpressionUsableCatcher(ExpressionNode* child);
			Type* typeCheck(bool forceArrayIdentifier);


		private:
			ExpressionNode* child;

	}

}
