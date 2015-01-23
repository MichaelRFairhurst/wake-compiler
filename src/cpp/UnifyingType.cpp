#include "UnifyingType.h"

void wake::UnifyingType::unifyWith(Type* unifyWithType) {
	if(failedToUnify1.get()) return;

	if(currentUnification.get() == NULL) {
		currentUnification.reset(new Type(unifyWithType));
	} else if(currentUnification->type == TYPE_UNUSABLE ^ unifyWithType->type == TYPE_UNUSABLE) {
		failedToUnify1.reset(new Type(currentUnification.get()));
		failedToUnify2.reset(new Type(unifyWithType));
		currentUnification.reset(new Type(TYPE_UNUSABLE));
	} else if(currentUnification->type != TYPE_UNUSABLE && unifyWithType->type != TYPE_UNUSABLE) {
		boost::optional<Type*> newUnification = analyzer->getCommonSubtypeOf(currentUnification.get(), unifyWithType);
		if(!newUnification) {
			failedToUnify1.reset(new Type(currentUnification.get()));
			failedToUnify2.reset(new Type(unifyWithType));
		} else {
			currentUnification.reset(*newUnification);
		}
	}
}

Type* wake::UnifyingType::getCurrentUnification() {
	return currentUnification.get();
}

Type* wake::UnifyingType::getUnificationFailure1() {
	return failedToUnify1.get();
}

Type* wake::UnifyingType::getUnificationFailure2() {
	return failedToUnify2.get();
}
