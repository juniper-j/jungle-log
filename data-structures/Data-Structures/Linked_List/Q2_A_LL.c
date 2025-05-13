//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 2 

이 코드는 두 개의 연결 리스트를 교대로 합치는 함수 alternateMergeLL()을 구현하는 문제입니다.
- alternateMergeLL() : 첫 번째 리스트(ll1)의 노드들과 두 번째 리스트(ll2)의 노드들을
                       번갈아가며 합치되, 두 번째 리스트의 노드는 우선 순서대로 끼워 넣고
                       남은 노드는 ll2에 남기거나, 다 사용했으면 ll2가 비게 됨.
- 그 외의 insertNode(), removeNode(), findNode() 등은 별도로 제공된 함수를 사용합니다.*/

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>		/*malloc(), free(), exit(), atoi()*/

//////////////////////////////////////////////////////////////////////////////////
// 노드(ListNode) 및 연결리스트(LinkedList) 구조체 정의

/* 각 노드는 item(정수)과 다음 노드를 가리키는 포인터(next)를 가짐 */
typedef struct _listnode {
	int item;
	struct _listnode *next;
} ListNode;		

/* 연결 리스트는 head(첫 노드 포인터)와 size(노드 개수)를 가짐 */
typedef struct _linkedlist {
	int size;
	ListNode *head;
} LinkedList;			


// 문제에서 구현을 요구하는 함수 (Q2: 교대 병합)
void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2);

// 출력, 삽입, 삭제 등에 사용될 함수들 (이미 제공된 코드)
void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode *findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);


//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	// 연결 리스트 초기화
	LinkedList ll1, ll2;
	int c, i, j;
	c = 1;
	
	// ll1 구조체 내부 필드 초기화 (빈 리스트)
	ll1.head = NULL;	// 아직 노드가 하나도 없으므로 head는 NULL
	ll1.size = 0;		// 노드 개수도 0

	// ll1 구조체 내부 필드 초기화 (빈 리스트)
	ll2.head = NULL;	// 아직 노드가 하나도 없으므로 head는 NULL
	ll2.size = 0;		// 노드 개수도 0

	printf("1: Insert an integer to the linked list 1:\n");
	printf("2: Insert an integer to the linked list 2:\n");
	printf("3: Create the alternate merged linked list:\n");
	printf("0: Quit:\n");

	while (c != 0)
	{
		printf("Please input your choice(1/2/3/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list 1: ");
			scanf("%d", &i);
			// (Q1의 insertSortedLL() 대신) 여기선 단순히 맨 뒤나 원하는 위치에 삽입
			j = insertNode(&ll1, ll1.size, i);
			printf("Linked list 1: ");
			printList(&ll1);
			break;

		case 2:
			printf("Input an integer that you want to add to the linked list 2: ");
			scanf("%d", &i);
			j = insertNode(&ll2, ll2.size, i);
			printf("Linked list 2: ");
			printList(&ll2);
			break;

		case 3:
		    printf("The resulting linked lists after merging the given linked list are:\n");
			// *** Question 2: 교대 병합 함수 호출 (Q1과 다른 핵심 부분) ***
			alternateMergeLinkedList(&ll1, &ll2);
			printf("The resulting linked list 1: ");
			printList(&ll1);
			printf("The resulting linked list 2: ");
			printList(&ll2);
			removeAllItems(&ll1);
			removeAllItems(&ll2);
			break;

		case 0:
			// 0번 선택 시 프로그램 종료 직전 리스트 정리
			removeAllItems(&ll1);
			removeAllItems(&ll2);
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
// *** alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2) ***
//
// - Q1의 insertSortedLL()과는 달리, 여기서는 "두 리스트를 교대(번갈아)로 합치는" 동작만 함
// - ll1과 ll2 모두 순회하면서, ll1 한 노드 뒤에 ll2의 노드를 하나씩 끼워넣는다.
// - ll2의 노드가 더 많다면, 일부 노드는 ll2에 남아 있고, 
//   ll1이 더 커서 ll2 노드를 다 소진하면 ll2는 비게 될 수 있음
//
//////////////////////////////////////////////////////////////////////////////////
void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2)
{
	// 두 리스트 중 하나라도 NULL이면 종료
	if (ll1 == NULL || ll2 == NULL) return;

	// ll1이 비어있으면 삽입할 위치가 없으므로 종료
	if (ll1->head == NULL) return;

	// 각각 리스트의 현재 노드를 가리키는 포인터 설정
	ListNode *cur1 = ll1->head;
	ListNode *cur2 = ll2->head;
	ListNode *next1;
	ListNode *next2;

	// 두 리스트 모두 노드가 남아있을 동안 반복
	while (cur1 != NULL && cur2 != NULL) {
		// 현재 노드의 다음 노드를 미리 저장해 둠 -> 나중에 끼워넣고 연결하기 위해 필요
		next1 = cur1->next;
		next2 = cur2->next;

		cur1->next = cur2;	// cur2 노드를 cur1 뒤에 삽입
		cur2->next = next1;	// next1 노드를 cur2 뒤에 삽입

		cur1 = next1;		// ll1의 다음 위치로 포인터 이동
		cur2 = next2;		// ll2의 다음 위치로 포인터 이동
	}
	// ll2의 head를 남은 노드로 갱신 (남은 노드가 없다면 NULL이 됨)
	ll2->head = cur2;
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

	if (ll->head == NULL || index == 0){
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}

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

	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	if (index == 0){
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

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
