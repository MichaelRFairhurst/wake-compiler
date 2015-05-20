/**************************************************
 * Source Code for the Original Compiler for the
 * Programming Language Wake
 *
 * UnifyingType.cpp
 *
 * Licensed under the MIT license
 * See LICENSE.TXT for details
 *
 * Author: Michael Fairhurst
 * Revised By:
 *
 **************************************************/

#include "UnifyingType.h"

using namespace wake;

void UnifyingType::unifyWith(PureType<QUALIFIED>* unifyWithType) {
	if(failedToUnify1.get()) return;

	if(currentUnification.get() == NULL) {
		currentUnification.reset(new PureType<QUALIFIED>(*unifyWithType));
	} else if(currentUnification->type == TYPE_UNUSABLE ^ unifyWithType->type == TYPE_UNUSABLE) {
		failedToUnify1.reset(new PureType<QUALIFIED>(*currentUnification.get()));
		failedToUnify2.reset(new PureType<QUALIFIED>(*unifyWithType));
		currentUnification.reset(new PureType<QUALIFIED>(TYPE_UNUSABLE));
	} else if(currentUnification->type != TYPE_UNUSABLE && unifyWithType->type != TYPE_UNUSABLE) {
		boost::optional<PureType<QUALIFIED>*> newUnification = analyzer->getCommonSubtypeOf(currentUnification.get(), unifyWithType);
		if(!newUnification) {
			failedToUnify1.reset(new PureType<QUALIFIED>(*currentUnification.get()));
			failedToUnify2.reset(new PureType<QUALIFIED>(*unifyWithType));
		} else {
			currentUnification.reset(*newUnification);
		}
	}
}

PureType<QUALIFIED>* UnifyingType::getCurrentUnification() {
	return currentUnification.get();
}

PureType<QUALIFIED>* UnifyingType::getUnificationFailure1() {
	return failedToUnify1.get();
}

PureType<QUALIFIED>* UnifyingType::getUnificationFailure2() {
	return failedToUnify2.get();
}
