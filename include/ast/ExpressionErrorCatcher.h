namespace wake::ast {

	class ExpressionErrorCatcher : public ExpressionNode {

		public:
			ExpressionErrorCatcher(ExpressionNode* child);
			Type* typeCheck(bool forceArrayIdentifier);


		private:
			ExpressionNode* child;

	}

}
