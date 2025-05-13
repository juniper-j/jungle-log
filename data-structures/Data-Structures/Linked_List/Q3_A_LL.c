//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 3 */

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
void moveOddItemsToBack(LinkedList *ll);

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
	c = 1;	// while 루프 진입을 위해 c를 초기값 1로 설정

	//Initialize the linked list 1 as an empty linked list
	ll.head = NULL;
	ll.size = 0;

	// 메뉴 출력
	printf("1: Insert an integer to the linked list:\n");
	printf("2: Move all odd integers to the back of the linked list:\n");
	printf("0: Quit:\n");

	// 사용자가 0(종료)을 선택할 때까지 무한 루프 실행
	while (c != 0)
	{
		printf("Please input your choice(1/2/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:		// 정수를 연결 리스트에 추가하는 옵션
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertNode(&ll, ll.size, i);	// 리스트의 맨 뒤에 새 정수를 추가
			printf("The resulting linked list is: ");	// 추가 후, 현재 리스트 상태 출력
			printList(&ll);
			break;
		case 2:		// 리스트에서 홀수 항목들을 뒤쪽으로 이동
			moveOddItemsToBack(&ll);
			printf("The resulting linked list after moving odd integers to the back of the linked list is: ");
			printList(&ll);
			removeAllItems(&ll);	// 리스트의 모든 노드를 해제하며 메모리 정리
			break;
		case 0:		// 종료 옵션 선택 시, 연결 리스트의 모든 노드를 해제하여 메모리 정리
			removeAllItems(&ll);
			break;
		default:	// 유효하지 않은 입력일 경우 사용자에게 오류 메시지 출력
			printf("Choice unknown;\n");
			break;
		}
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//
// *** moveOddItemsToBack(LinkedList *ll) ***
//
// - 본 함수는 입력된 연결 리스트에서 홀수 정수 노드들을 모두 리스트의 뒤쪽으로 이동시키는 기능을 수행함
// - 리스트를 한 번 순회하면서 각 노드를 홀수 리스트(odd list)와 짝수 리스트(even list)로 분리함
// - 분리 시 원래의 순서를 유지하기 위해, 각 리스트에 노드를 추가할 때 head와 tail 포인터를 사용함
// - 순회가 완료된 후, 짝수 리스트의 마지막 노드(even tail)와 홀수 리스트의 시작 노드(odd head)를 연결하여 최종 리스트를 구성함
//
//////////////////////////////////////////////////////////////////////////////////
void moveOddItemsToBack(LinkedList *ll)
{
	// 1. 연결리스트가 NULL이거나, 연결리스트가 비어있으면 종료
	if (ll == NULL || ll->head == NULL) return;

	// 2. cur 포인터(전체 노드 순회용)를 연결리스트의 head로 설정
	// 짝수, 홀수의 Head, Tail 포인터를 각각 NULL로 초기화
	ListNode *cur = ll->head;
	ListNode *evenHead = NULL, *evenTail = NULL;
	ListNode *oddHead = NULL, *oddTail = NULL;

	// 3. 연결리스트의 노드가 남아있을 때까지 순회하며 다음을 반복
	// - cur의 다음 노드를 nextNode에 저장
	// - cur를 리스트에서 분리하기 위해 NULL로 설정
	// - cur->item(데이터)가 짝수인지 아닌지, Head가 NULL인지 아닌지에 따라 
	//		Head, Tail 업데이트하며 even, odd 리스트 연결
	// - cur에 nextNode를 대입하여 다음 노드로 이동
	while (cur != NULL) {
		ListNode *nextNode = cur -> next;
		cur->next = NULL;

		if (cur->item % 2 == 0) {
			if (evenHead == NULL) {
				evenHead = evenTail = cur;
			} else {
				evenTail->next = cur;
				evenTail = cur;
			}
		} else {
			if (oddHead == NULL) {
				oddHead = oddTail = cur;
			} else {
				oddTail->next = cur;
				oddTail = cur;
			}
		}
		cur = nextNode;
	}

	// 4. 반복문 종료 후, even 리스트가 비어있다면 연결리스트의 head를 oddHead로 설정
	//	아니라면, evenTail의 next 포인터를 oddHead에 연결 및 연결리스트 head를 evenHead로 설정
	if (evenHead == NULL) {
		ll->head = oddHead;
	} else {
		ll->head = evenHead;
		evenTail->next = oddHead;
	}
}
///////////////////////////////////////////////////////////////////////////////////

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


ListNode *findNode(LinkedList *ll, int index){

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
