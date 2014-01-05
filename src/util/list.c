#include <lao.h>

#include <stdlib.h>
#include <string.h>

void lao_list_insert(LaoList *l, LaoListElement *after, void *data) {
	LaoListElement *e, *n;
	for(e = l->first; e; e = e->next)
		if(e == after)
			break;
	
	n = malloc(sizeof(LaoListElement));
	n->prev = e;
	
	if(e == NULL) {
		n->next = l->first;
		l->first = n;
	} else {
		n->next = e->next;
		e->next = n;
	}
	
	n->data = data;
	l->size++;
}		

void lao_list_add(LaoList *l, void *data) {
	LaoListElement *n = malloc(sizeof(LaoListElement));
	
	n->next = l->first;
	l->first = n;
	n->prev = 0;
	
	n->data = data;
	l->size++;
}

void lao_list_remove(LaoList *l, LaoListElement *e) {
	if(e->next)
		e->next->prev = e->prev;
	if(e->prev)
		e->prev->next = e->next;
	if(e == l->first)
		l->first = e->next;
	
	free(e->data);
	free(e);
	
	l->size--;
}	

void lao_list_free(LaoList *l) {
	while(l->size > 0)		
		lao_list_remove(l, l->first);
}
