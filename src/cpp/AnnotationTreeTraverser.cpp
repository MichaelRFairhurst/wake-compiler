#include "AnnotationTreeTraverser.h"

vector<Annotation*> AnnotationTreeTraverser::getAnnotations(Node* tree) {
	vector<Annotation*> annotations;
	for(int i = 0; i < tree->subnodes; i++) {
		annotations.push_back(getSingleAnnotation(tree->node_data.nodes[i]));
	}

	return annotations;
}

Annotation* AnnotationTreeTraverser::getSingleAnnotation(Node* tree) {
	Annotation* ann = new Annotation();

	ann->name = strdup(tree->node_data.nodes[0]->node_data.string);

	if(tree->subnodes == 1) {
		return ann;
	}

	Node* vals = tree->node_data.nodes[1];
	for(int i = 0; i < vals->subnodes; i++) {
		AnnotationVal* val = new AnnotationVal();
		ann->vals.push_back(val);

		switch(vals->node_data.nodes[i]->node_type) {
			case NT_STRINGLIT:
				val->type = ANNOTATION_VAL_TYPE_TEXT;
				val->valdata.text = strdup(vals->node_data.nodes[i]->node_data.string);
				break;

			case NT_NUMBERLIT:
				val->type = ANNOTATION_VAL_TYPE_NUM;
				val->valdata.num = vals->node_data.nodes[i]->node_data.number;
				break;

			case NT_NOTHING:
				val->type = ANNOTATION_VAL_TYPE_NOTHING;
				break;

			case NT_BOOLLIT:
				val->type = ANNOTATION_VAL_TYPE_BOOL;
				val->valdata.num = vals->node_data.nodes[i]->node_data.number;
				break;
		}
	}

	return ann;
}
