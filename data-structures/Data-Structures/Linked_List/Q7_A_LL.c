//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 7 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList;			// You should not change the definition of LinkedList


//////////////////////// function prototypes /////////////////////////////////////

// You should not change the prototype of this function
void recursiveReverse(ListNode **ptrHead);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode * findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);


//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	LinkedList ll;
	int c, i, j;
	c = 1;
	//Initialize the linked list 1 as an empty linked list
	ll.head = NULL;
	ll.size = 0;


	printf("1: Insert an integer to the linked list:\n");
	printf("2: Reversed the linked list:\n");
	printf("0: Quit:\n");

	while (c != 0)
	{
		printf("Please input your choice(1/2/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertNode(&ll, ll.size, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			recursiveReverse(&(ll.head)); // You need to code this function
			printf("The resulting linked list after reversed the given linked list is: ");
			printList(&ll);
			removeAllItems(&ll);
			break;
		case 0:
			removeAllItems(&ll);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//
// *** recursiveReverse(ListNode **ptrHead) ***
//
// - 연결 리스트를 재귀적으로 뒤집는다
// - 리스트를 재귀적으로 끝까지 들어간 뒤, 되돌아오면서 각 노드의 방향을 바꾼다
// - 단, 되돌아올 때 while 문을 이용해 마지막 노드를 찾아 현재 노드를 붙인다
//
// ⚠️ 이 구현은 매 단계마다 리스트의 끝까지 순회하므로 시간복잡도는 O(n^2)
//
// [포인터 설명]
// - ptrHead: ListNode* 를 가리키는 포인터 (즉, head 의 주소)
// - *ptrHead: 현재 연결 리스트의 head 노드
// - first: 현재 함수 호출에서의 첫 번째 노드
// - others: first 이후 나머지 리스트의 시작점
// - cur: 재귀 호출 이후, others 리스트의 끝을 찾아 first를 연결하기 위한 순회 포인터
//
//////////////////////////////////////////////////////////////////////////////////
void recursiveReverse(ListNode **ptrHead)
{
    // base case: 빈 리스트이거나, 노드가 하나뿐이면 그대로 종료
	if (*ptrHead == NULL || (*ptrHead)->next == NULL) return;

	ListNode *first = *ptrHead;
	ListNode *others = first->next;

    // 나머지 리스트를 재귀적으로 뒤집음
	recursiveReverse(&others);

    // others는 이미 뒤집힌 리스트의 head
    // 그 끝까지 이동하여 cur를 마지막 노드로 만든다
	ListNode *cur = others;
	while (cur->next != NULL)
		cur = cur->next;

    // 뒤집힌 리스트의 끝(cur)에 현재 노드(first)를 연결
	cur->next = first;
    // first는 리스트의 꼬리가 되므로 next를 NULL로 설정
	first->next = NULL;

    // head 포인터를 others(뒤집힌 리스트의 head)로 갱신
	*ptrHead = others;
}
//////////////////////////////////////////////////////////////////////////////////


void printList(LinkedList *ll){

	ListNode *cur;
	if (ll == NULL)
		return;
	cur = ll->head;

	if (cur == NULL)
		printf("Empty");
	while (cur != NULL)
	{
		printf("%d ", cur->item);
		cur = cur->next;
	}
	printf("\n");
}

ListNode * findNode(LinkedList *ll, int index){

	ListNode *temp;

	if (ll == NULL || index < 0 || index >= ll->size)
		return NULL;

	temp = ll->head;

	if (temp == NULL || index < 0)
		return NULL;

	while (index > 0){
		temp = temp->next;
		if (temp == NULL)
			return NULL;
		index--;
	}

	return temp;
}

int insertNode(LinkedList *ll, int index, int value){

	ListNode *pre, *cur;

	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	// If empty list or inserting first node, need to update head pointer
	if (ll->head == NULL || index == 0){
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}


	// Find the nodes before and at the target position
	// Create a new node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL){
		cur = pre->next;
		pre->next = malloc(sizeof(ListNode));
		pre->next->item = value;
		pre->next->next = cur;
		ll->size++;
		return 0;
	}

	return -1;
}


int removeNode(LinkedList *ll, int index){

	ListNode *pre, *cur;

	// Highest index we can remove is size-1
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	// If removing first node, need to update head pointer
	if (index == 0){
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// Free the target node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL){

		if (pre->next == NULL)
			return -1;

		cur = pre->next;
		pre->next = cur->next;
		free(cur);
		ll->size--;
		return 0;
	}

	return -1;
}

void removeAllItems(LinkedList *ll)
{
	ListNode *cur = ll->head;
	ListNode *tmp;

	while (cur != NULL){
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
	ll->head = NULL;
	ll->size = 0;
}
