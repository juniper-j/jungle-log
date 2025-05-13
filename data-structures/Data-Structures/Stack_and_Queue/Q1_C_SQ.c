//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section C - Stack and Queue Questions
Purpose: Implementing the required functions for Question 1 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode {
	int item;
	struct _listnode *next;
} ListNode;	// 단일 노드 구조체

typedef struct _linkedlist {
	int size;
	ListNode *head;
} LinkedList;	// 연결리스트 구조체


typedef struct _queue {
	LinkedList ll;
} Queue;  // 연결리스트 기반 큐 구조체

///////////////////////// function prototypes ////////////////////////////////////

// 문제에서 구현을 요구하는 함수들 (Q1: 연결리스트 ll에 저장된 정수들을 순서대로 큐 q에 enqueue)
void createQueueFromLinkedList(LinkedList *ll, Queue *q);	// 연결리스트의 값을 순서대로 큐에 복사
void removeOddValues(Queue *q);		// 큐에서 홀수 값을 제거

// Queue를 다루기 위한 기본 연산 함수들
void enqueue(Queue *q, int item);		// 큐의 뒤(rear)에 값을 삽입
int dequeue(Queue *q);					// 큐의 앞(front) 값을 제거하고 반환
int isEmptyQueue(Queue *q);				// 큐가 비어있는지 확인
void removeAllItemsFromQueue(Queue *q);	// 큐의 모든 항목 제거

// 연결리스트 관련 기본 함수들
void printList(LinkedList *ll);					// 연결리스트의 값을 순서대로 출력
ListNode * findNode(LinkedList *ll, int index);	// 인덱스 위치의 노드를 반환
int insertNode(LinkedList *ll, int index, int value);	// 연결리스트의 특정 위치에 노드 삽입
int removeNode(LinkedList *ll, int index);		// 연결리스트의 특정 위치의 노드를 삭제
void removeAllItems(LinkedList *ll);			// 연결리스트의 모든 노드 제거

//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	int c, i;
	LinkedList ll;
	Queue q;

	c = 1;

	ll.head = NULL;		// 연결 리스트 초기화
	ll.size = 0;

	q.ll.head = NULL;	// 큐 초기화
	q.ll.size = 0;

	printf("1: Insert an integer into the linked list:\n");
	printf("2: Create the queue from the linked list:\n");
	printf("3: Remove odd numbers from the queue:\n");
	printf("0: Quit:\n");

	while (c != 0)
	{
		printf("Please input your choice(1/2/3/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:		// 연결 리스트에 값 삽입
			printf("Input an integer that you want to insert into the List: ");
			scanf("%d", &i);
			insertNode(&ll, ll.size, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:		// 연결 리스트를 큐로 변환
			createQueueFromLinkedList(&ll, &q); // You need to code this function
			printf("The resulting queue is: ");
			printList(&(q.ll));
			break;
		case 3:		// 큐에서 홀수 제거
			removeOddValues(&q); // You need to code this function
			printf("The resulting queue after removing odd integers is: ");
			printList(&(q.ll));
			removeAllItemsFromQueue(&q);	// 테스트 종료를 위해 큐와 리스트 초기화
			removeAllItems(&ll);
			break;
		case 0:		// 프로그램 종료 전 메모리 해제
			removeAllItemsFromQueue(&q);
			removeAllItems(&ll);
			break;
		default:	// 잘못된 입력 처리
			printf("Choice unknown;\n");
			break;
		}

	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////
//
// createQueueFromLinkedList(LinkedList *ll, Queue *q)
//
// - 연결 리스트 ll에 저장된 값을 순서대로 큐 q에 삽입
// - 큐가 이미 비어있지 않다면 먼저 모든 항목을 제거
// - 연결 리스트의 첫 노드부터 끝까지 순회하며 값들을 enqueue
//
///////////////////////////////////////////////////////////////////////////////////
void createQueueFromLinkedList(LinkedList *ll, Queue *q)
{
	// 큐 초기화: 기존 큐가 비어있지 않으면 제거
	removeAllItemsFromQueue(q);
	// 연결 리스트가 NULL이거나 비어 있다면 아무 작업도 하지 않음
	if (ll == NULL || ll->head == NULL) return;

	// 연결 리스트 순회
	ListNode *temp = ll->head;
	while (temp != NULL) {
		enqueue(q, temp->item);		// 현재 노드의 값을 큐에 삽입
		temp = temp->next;			// 다음 노드로 이동
	}
}


///////////////////////////////////////////////////////////////////////////////////
//
// removeOddValues(Queue *q)
//
// - 큐에서 홀수 값을 제거하고, 짝수 값만 다시 큐에 유지
// - 기존 큐를 순회하면서 짝수만 임시로 다시 enqueue
// - 최종적으로 홀수는 모두 제거되고, 큐에는 짝수만 남음
//
///////////////////////////////////////////////////////////////////////////////////
void removeOddValues(Queue *q)
{
	// q가 NULL이거나 비어 있다면 아무 작업도 하지 않음
	if (q == NULL || isEmptyQueue(q)) return;
	
	int size = q->ll.size;

	for (int i = 0; i < size; i++) {	// 큐의 크기만큼 순회
		int val = dequeue(q);				// 꺼낸 값이
		if (val % 2 == 0) enqueue(q, val);	// 짝수면 다시 큐에 삽입
	}										// 반복문 종료되면 큐에서 홀수 제거 완료
}


///////////////////////////////////////////////////////////////////////////////////
//
// enqueue(Queue *q, int item)
//
// - 큐의 가장 뒤(rear)에 새 값을 삽입
// - 내부적으로 연결 리스트의 맨 끝(q->ll.size 위치)에 노드를 삽입
//
///////////////////////////////////////////////////////////////////////////////////
void enqueue(Queue *q, int item) {
	insertNode(&(q->ll), q->ll.size, item);
}


///////////////////////////////////////////////////////////////////////////////////
//
// dequeue(Queue *q)
//
// - 큐의 가장 앞(front)에 있는 값을 제거하고 그 값을 반환
// - 큐가 비어 있으면 -1 반환
// - 내부적으로 연결 리스트의 head 노드를 삭제
//
///////////////////////////////////////////////////////////////////////////////////
int dequeue(Queue *q) {
	int item;
	if (!isEmptyQueue(q)) {
		item = ((q->ll).head)->item;
		removeNode(&(q->ll), 0);
		return item;
	}
	return -1;
}


///////////////////////////////////////////////////////////////////////////////////
//
// isEmptyQueue(Queue *q)
//
// - 큐가 비어 있는지 확인
// - 큐 내부 연결 리스트의 size가 0이면 1(참), 아니면 0(거짓)을 반환
//
///////////////////////////////////////////////////////////////////////////////////
int isEmptyQueue(Queue *q) {
	if ((q->ll).size == 0)
		return 1;
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////
//
// removeAllItemsFromQueue(Queue *q)
//
// - 큐의 모든 항목을 제거 (초기화)
// - 내부적으로 dequeue()를 호출하여 하나씩 front부터 제거
//
///////////////////////////////////////////////////////////////////////////////////
void removeAllItemsFromQueue(Queue *q)
{
	int count, i;
	if (q == NULL)
		return;
	count = q->ll.size;

	for (i = 0; i < count; i++)		// while (!isEmptyQueue(q)) dequeue(q);
		dequeue(q);
}


///////////////////////////////////////////////////////////////////////////////////
//
// printList(LinkedList *ll)
//
// - 연결 리스트의 모든 노드를 순서대로 출력
// - 만약 노드가 하나도 없으면 "Empty" 출력
//
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


//////////////////////////////////////////////////////////////////////////////////
//
// removeAllItems(LinkedList *ll)
//
// - 연결 리스트 내 모든 노드를 순회하면서 free()로 메모리를 해제(=노드를 제거, 리스트 초기화)
// - 리스트를 완전히 비운 뒤, head를 NULL, size를 0으로 초기화
//
//////////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////////
//
// findNode(LinkedList *ll, int index)
//
// - index 위치의 노드를 찾아서 그 노드의 포인터를 반환
// - 유효 범위가 아니면(NULL이나 index가 범위를 벗어나면) NULL 반환
//
//////////////////////////////////////////////////////////////////////////////////
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


//////////////////////////////////////////////////////////////////////////////////
//
// insertNode(LinkedList *ll, int index, int value)
//
// - 연결리스트 ll의 index 위치에 value를 갖는 새 노드를 삽입
// - 삽입 성공 시 0, 실패 시 -1 반환
//
//////////////////////////////////////////////////////////////////////////////////
int insertNode(LinkedList *ll, int index, int value){

	ListNode *pre, *cur;

	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	if (ll->head == NULL || index == 0){
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		if (ll->head == NULL)
		{
			exit(0);
		}
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}

	if ((pre = findNode(ll, index - 1)) != NULL){
		cur = pre->next;
		pre->next = malloc(sizeof(ListNode));
		if (pre->next == NULL)
		{
			exit(0);
		}
		pre->next->item = value;
		pre->next->next = cur;
		ll->size++;
		return 0;
	}

	return -1;
}


//////////////////////////////////////////////////////////////////////////////////
//
// removeNode(LinkedList *ll, int index)
//
// - 연결 리스트 ll의 'index' 위치에 있는 노드를 제거
// - 제거 성공 시 0, 실패 시 -1을 반환
//
//////////////////////////////////////////////////////////////////////////////////
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