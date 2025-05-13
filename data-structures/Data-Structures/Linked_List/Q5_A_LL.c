//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 5 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode{
	int item;
	struct _listnode *next;
} ListNode;			// You should not change the definition of ListNode

typedef struct _linkedlist{
	int size;
	ListNode *head;
} LinkedList;			// You should not change the definition of LinkedList


///////////////////////// function prototypes ////////////////////////////////////

// You should not change the prototype of this function
void frontBackSplitLinkedList(LinkedList* ll, LinkedList *resultFrontList, LinkedList *resultBackList);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *l);
ListNode * findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);


///////////////////////////// main() /////////////////////////////////////////////

int main()
{
	int c, i;
	c = -1;		// 이거 정의 안되어 있으면 계속 실행 되자마자 종료됨
	// 기본 리스트 및 분할 결과를 저장할 두 개의 연결리스트 초기화
	LinkedList ll;
	LinkedList resultFrontList, resultBackList;

	//Initialize the linked list as an empty linked list
	ll.head = NULL;
	ll.size = 0;

	//Initialize the front linked list as an empty linked list
	resultFrontList.head = NULL;
	resultFrontList.size = 0;

	// Initialize the back linked list as an empty linked list
	resultBackList.head = NULL;
	resultBackList.size = 0;

	// 메뉴 출력
	printf("1: Insert an integer to the linked list:\n");
	printf("2: Split the linked list into two linked lists, frontList and backList:\n");
	printf("0: Quit:\n");

	// 사용자 입력에 따라 처리하는 반복문 (0 입력 시 종료)
	while (c != 0)
	{
	    printf("Please input your choice(1/2/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			// 정수 삽입
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			// 리스트 끝에 삽입하는 함수 호출 (현재 인덱스 ll.size에 삽입)
			insertNode(&ll, ll.size, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			// 분할 및 결과 출력
			printf("The resulting linked lists after splitting the given linked list are:\n");
			frontBackSplitLinkedList(&ll, &resultFrontList, &resultBackList); // You need to code this function
			printf("Front linked list: ");
			printList(&resultFrontList);
			printf("Back linked list: ");
			printList(&resultBackList);
			printf("\n");
			// 분할 후 사용한 메모리 해제 (각 리스트를 모두 비움)
			removeAllItems(&ll);
			removeAllItems(&resultFrontList);
			removeAllItems(&resultBackList);
			break;
		case 0:
			// 프로그램 종료 전, 모든 리스트 메모리 해제
			removeAllItems(&ll);
			removeAllItems(&resultFrontList);
			removeAllItems(&resultBackList);
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
// *** frontBackSplitLinkedList(LinkedList *ll, LinkedList *resultFrontList, LinkedList *resultBackList) ***
//
// - 주어진 연결 리스트 (ll)를 앞쪽 리스트와 뒤쪽 리스트로 분할함
// - 총 노드 수가 홀수인 경우, front 리스트에 extra 노드를 포함함
// - resultFrontList: 앞쪽 리스트 결과, resultBackList: 뒤쪽 리스트 결과
//
//////////////////////////////////////////////////////////////////////////////////
void frontBackSplitLinkedList(LinkedList *ll, LinkedList *resultFrontList, LinkedList *resultBackList)
{
	// 1. 연결리스트가 NULL이거나, 연결리스트가 비어있으면 resultFL/resultBL도 빈 리스트로 초기화
	if (ll == NULL || ll->head == NULL) {
		resultFrontList->head = NULL;
		resultBackList->head = NULL;
		resultFrontList->size = 0;
		resultBackList->size = 0;
		return;
	}

	// 2. total, front, back Size 계산
	int totalSize = ll->size;
	int frontSize = (totalSize + 1) / 2;
	int backSize = totalSize - frontSize;

	// 3. front 리스트의 마지막 노드를 찾기 위해 리스트 순회
	// cur는 head에서 시작하며, (frontSize-1)번 이동하여 front 리스트의 마지막 노드 가리킴 
	ListNode *cur = ll->head;
	int count = 1;	// head가 첫번째 노드이므로 count는 1부터 시작
	while (cur != NULL && count < frontSize) {
		cur = cur->next;
		count++;
	}

	// 4. resultFrontList 결과 저장
	resultFrontList->head = ll->head;
	resultFrontList->size = frontSize;
	// 5. resultBackList 결과 저장: cur가 유효한 포인터인지 확인 후, 유효하지 않으면 NULL
	// 유효하다면 head는 front 리스트 마지막 노드의 next 노드, size는 backSize
	resultBackList->head = (cur != NULL) ? cur->next : NULL;
	resultBackList->size = backSize;

	// 5. front 리스트의 마지막 노드(cur)에서 뒤쪽 리스트와의 연결 끊기
	if(cur != NULL) {
		cur->next = NULL;
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
