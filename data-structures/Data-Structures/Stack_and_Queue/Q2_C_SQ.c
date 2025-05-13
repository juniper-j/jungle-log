//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section C - Stack and Queue Questions
Purpose: Implementing the required functions for Question 2 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#define MIN_INT -1000
//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode;	// 단일 노드 구조체

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList;	// 연결리스트 구조체

typedef struct _stack
{
	LinkedList ll;
} Stack;  // 연결리스트 기반 스택 구조체

///////////////////////// function prototypes ////////////////////////////////////

// 문제에서 구현을 요구하는 함수들 (Q2: 연결리스트 ll에 저장된 정수들을 순서대로 스택 s에 push)
void createStackFromLinkedList(LinkedList *ll , Stack *stack);	// 연결리스트의 값을 순서대로 스택에 복사
void removeEvenValues(Stack *s);	// 스택에서 짝수 값을 제거

// Stack을 다루기 위한 기본 연산 함수들
void push(Stack *s , int item);			// 스택의 위(top)에 값을 삽입
int pop(Stack *s);						// 스택의 위(top) 값을 제거하고 반환
int isEmptyStack(Stack *s);				// 스택이 비어있는지 확인
void removeAllItemsFromStack(Stack *s);	// 스택의 모든 항목 제거

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
	Stack s;

	c = 1;
	// Initialize the linked list as an empty linked list
	ll.head = NULL;
	ll.size = 0;

	// Initalize the stack as an empty stack
	s.ll.head = NULL;
	s.ll.size = 0;

	printf("1: Insert an integer into the linked list:\n");
	printf("2: Create the stack from the linked list:\n");
	printf("3: Remove even numbers from the stack:\n");
	printf("0: Quit:\n");

	while (c != 0)
	{
		printf("Please input your choice(1/2/3/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			insertNode(&ll, ll.size, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			createStackFromLinkedList(&ll, &s); // You need to code this function
			printf("The resulting stack is: ");
			printList(&(s.ll));
			break;
		case 3:
			removeEvenValues(&s); // You need to code this function
			printf("The resulting stack after removing even integers is: ");
			printList(&(s.ll));
			removeAllItemsFromStack(&s);
			removeAllItems(&ll);
			break;
		case 0:
			removeAllItemsFromStack(&s);
			removeAllItems(&ll);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////
//
// createStackFromLinkedList(LinkedList *ll, Stack *stack)
//
// - 연결 리스트 ll에 저장된 값을 스택 stack에 push하여 역순으로 저장
// - 연결 리스트의 첫 노드부터 순서대로 push하더라도, 스택은 후입선출이므로
//   결과적으로 마지막 노드가 top에 위치하게 됨
// - 스택이 비어있지 않으면 먼저 초기화
//
///////////////////////////////////////////////////////////////////////////////////
void createStackFromLinkedList(LinkedList *ll, Stack *s)
{
    removeAllItemsFromStack(s);
	if (ll == NULL || ll->head == NULL) return;

	ListNode *temp = ll->head;
	while (temp != NULL) {
		push(s, temp->item);
		temp = temp->next;
	}
}


///////////////////////////////////////////////////////////////////////////////////
//
// removeEvenValues(Stack *s)
//
// - 스택에서 짝수 값을 제거하고 홀수 값만 유지
// - 임시 스택을 사용하여 조건에 맞는 값만 보관한 후, 원래 스택으로 다시 복원함 (역순 복구)
// - 최종적으로 원래 스택에는 홀수 값만 남게 됨
//
///////////////////////////////////////////////////////////////////////////////////
void removeEvenValues(Stack *s)
{
	if (s == NULL || isEmptyStack(s)) return;

	// 임시 저장용 스택 초기화
	Stack temp_s;
	temp_s.ll.head = NULL;
	temp_s.ll.size = 0;

	// 스택이 빌 때까지 pop, 스택에서 꺼낸 값이 홀수면 임시 스택에 push
	while (!isEmptyStack(s)) 
	{
		int val = pop(s);
		if (val % 2 != 0) push(&temp_s, val);
	}
	
	// 임시 스택에 쌓아둔 값을 다시 원래 스택으로 push (원래 순서 복원)
	while (!isEmptyStack(&temp_s))
	{
		int val = pop(&temp_s);
		push(s, val);
	}
}


///////////////////////////////////////////////////////////////////////////////////
//
// push(Stack *s, int item)
//
// - 스택의 가장 위(top)에 새 값을 삽입
// - 연결 리스트의 맨 앞(head 위치)에 새 노드를 삽입하여 LIFO 구조를 구현
//
///////////////////////////////////////////////////////////////////////////////////
void push(Stack *s, int item) {
	insertNode(&(s->ll), 0, item);
}


///////////////////////////////////////////////////////////////////////////////////
//
// pop(Stack *s)
//
// - 스택의 가장 위(top) 값을 제거하고 반환
// - 연결 리스트의 head 노드를 제거하는 방식으로 구현
// - 스택이 비어있으면 MIN_INT를 반환
//
///////////////////////////////////////////////////////////////////////////////////
int pop(Stack *s) {
	int item;
	if (s->ll.head != NULL) {
		item = ((s->ll).head)->item;
		removeNode(&(s->ll), 0);
		return item;
	}
	return MIN_INT; 	// -1000
}


///////////////////////////////////////////////////////////////////////////////////
//
// isEmptyStack(Stack *s)
//
// - 스택이 비어 있는지 확인
// - 연결 리스트의 size가 0이면 1(참), 그렇지 않으면 0(거짓)을 반환
//
///////////////////////////////////////////////////////////////////////////////////
int isEmptyStack(Stack *s) {
	if ((s->ll).size == 0)
		return 1;
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////
//
// removeAllItemsFromStack(Stack *s)
//
// - 스택에 있는 모든 값을 제거 (초기화)
// - pop()을 반복 호출하여 스택을 완전히 비움
//
///////////////////////////////////////////////////////////////////////////////////
void removeAllItemsFromStack(Stack *s) {
	if (s == NULL)
		return;
	while (s->ll.head != NULL) pop(s);
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