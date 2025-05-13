//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 1 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode; // You should not change the definition of ListNode

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList; // You should not change the definition of LinkedList

///////////////////////// function prototypes ////////////////////////////////////

// You should not change the prototype of this function
int insertSortedLL(LinkedList *ll, int item);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode *findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);

//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	LinkedList ll;
	int c, i, j;
	c = 1;

	// Initialize the linked list 1 as an empty linked list
	ll.head = NULL;
	ll.size = 0;

	printf("1: Insert an integer to the sorted linked list:\n");
	printf("2: Print the index of the most recent input value:\n");
	printf("3: Print sorted linked list:\n");
	printf("0: Quit:");

	while (c != 0)
	{
		printf("\nPlease input your choice(1/2/3/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertSortedLL(&ll, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			printf("The value %d was added at index %d\n", i, j);
			break;
		case 3:
			printf("The resulting sorted linked list is: ");
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

int insertSortedLL(LinkedList *ll, int item)
{
	// 코드
}

///////////////////////////////////////////////////////////////////////////////////

void printList(LinkedList *ll)
{

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

	while (cur != NULL)
	{
		tmp = cur->next; // free를 먼저 해버리면 가리키던 노드가 사라지니까 임시로 저장
		free(cur);		 // 그다음 해제
		cur = tmp;		 // 임시 노드를 현재 노드로
	}
	ll->head = NULL; // 헤드도 NULL로 만들어버리자 !!
	ll->size = 0;
}

ListNode *findNode(LinkedList *ll, int index)
{ // 여기서 인덱스는 사이즈

	ListNode *temp; // 끝 노드를 찾을 임시 포인터

	if (ll == NULL || index < 0 || index >= ll->size)
		return NULL;

	temp = ll->head; // 첫 포인터 가리키기

	if (temp == NULL || index < 0) // 인덱스가 잘못되었거나 head 자체가 없으면 NULL
		return NULL;

	while (index > 0)
	{
		temp = temp->next; // 다음 노드 찾기
		if (temp == NULL)
			return NULL; // 인덱스가 리스트를 벗어나면 NULL 포인터 반환
		index--;
	}

	return temp; // 해당하는 노드 반환
}

int insertNode(LinkedList *ll, int index, int value)
{

	ListNode *pre, *cur;

	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	// If empty list or inserting first node, need to update head pointer
	if (ll->head == NULL || index == 0)
	{
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}

	// Find the nodes before and at the target position
	// Create a new node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL)
	{
		cur = pre->next;
		pre->next = malloc(sizeof(ListNode));
		pre->next->item = value;
		pre->next->next = cur;
		ll->size++;
		return 0;
	}

	return -1;
}

int removeNode(LinkedList *ll, int index) // 이 리스트 안에서 해당하는 인덱스 가져오기
{

	ListNode *pre, *cur;

	// Highest index we can remove is size-1
	if (ll == NULL || index < 0 || index >= ll->size) // 리스트 자체가 없거나, 인덱스가 잘못되었으면 -1 반환
		return -1;

	// If removing first node, need to update head pointer
	if (index == 0) // 첫번째 노드는 헤드이므로 새 헤드 포인터가 필요해
	{
		cur = ll->head->next; // 헤드의 다음 노드 가리키기
		free(ll->head);		  // 원래 헤드 노드는 해제시키기
		ll->head = cur;		  // 새 노드를 헤드로
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// Free the target node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL) // 이전 노드를 삭제할 노드 다음 노드와 이어줘야 하기 때문에
	{											 // 이전 노드 찾아오기 !!

		if (pre->next == NULL) // 찾은 노드가 마지막 노드일 경우
			return -1;

		cur = pre->next;	   // 삭제할 노드
		pre->next = cur->next; // 삭제할 노드의 이전 노드와 다음 노드를 연결시켜주기
		free(cur);			   // 삭제할 노드 해제
		ll->size--;
		return 0;
	}

	return -1;
}