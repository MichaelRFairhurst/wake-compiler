#include "UnifyingType.h"

void wake::UnifyingType::unifyWith(PureType* unifyWithType) {
	if(failedToUnify1.get()) return;

	if(currentUnification.get() == NULL) {
		currentUnification.reset(new PureType(*unifyWithType));
	} else if(currentUnification->type == TYPE_UNUSABLE ^ unifyWithType->type == TYPE_UNUSABLE) {
		failedToUnify1.reset(new PureType(*currentUnification.get()));
		failedToUnify2.reset(new PureType(*unifyWithType));
		currentUnification.reset(new PureType(TYPE_UNUSABLE));
	} else if(currentUnification->type != TYPE_UNUSABLE && unifyWithType->type != TYPE_UNUSABLE) {
		boost::optional<PureType*> newUnification = analyzer->getCommonSubtypeOf(currentUnification.get(), unifyWithType);
		if(!newUnification) {
			failedToUnify1.reset(new PureType(*currentUnification.get()));
			failedToUnify2.reset(new PureType(*unifyWithType));
		} else {
			currentUnification.reset(*newUnification);
		}
	}
}

PureType* wake::UnifyingType::getCurrentUnification() {
	return currentUnification.get();
}

PureType* wake::UnifyingType::getUnificationFailure1() {
	return failedToUnify1.get();
}

PureType* wake::UnifyingType::getUnificationFailure2() {
	return failedToUnify2.get();
}
