//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 1 

이 코드는 정렬된(오름차순) 연결 리스트에 새로운 정수를 삽입하는 함수를 구현하는 문제입니다.
- insertSortedLL() : 정수를 적절한 위치에 삽입 (이미 동일한 값이 있으면 삽입 실패).
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

///////////////////////// function prototypes ////////////////////////////////////

// 문제에서 구현을 요구하는 함수 (Q1: 순서에 맞춰 노드 삽입)
int insertSortedLL(LinkedList *ll, int item);

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
	LinkedList ll;
	int c, i, j;
	c = 1;

	// ll 구조체 내부 필드 초기화 (빈 리스트)
	ll.head = NULL;	// 아직 노드가 하나도 없으므로 head는 NULL
	ll.size = 0;	// 노드 개수도 0

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
			
			// 오름차순 상태를 유지하며 i를 삽입하는 함수
			j = insertSortedLL(&ll, i);

			printf("The resulting linked list is: ");
			printList(&ll);
			break;

		case 2:
			// 마지막에 삽입한 값(i)의 반환 인덱스(j)를 출력
			printf("The value %d was added at index %d\n", i, j);
			break;

		case 3:
			// 현재 연결 리스트를 출력하고, removeAllItems() 호출하여 리스트 초기화
			printf("The resulting sorted linked list is: ");
			printList(&ll);
			// removeAllItems(&ll);
			break;

		case 0:
			// 0번 선택 시 프로그램 종료 직전 리스트 정리
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
// insertSortedLL(LinkedList *ll, int item)
//
// - 이미 오름차순으로 정렬된 연결 리스트에 새로운 정수 item을 삽입
// - 중복 값이 있으면 삽입하지 않고 -1 반환
// - 삽입 성공 시 해당 위치(index)를 반환
// - insertNodeWithPre()을 사용해 중복 순회 없이 삽입 처리
//
///////////////////////////////////////////////////////////////////////////////////
int insertSortedLL(LinkedList *ll, int item)
{
    if (ll == NULL) return -1; // 연결 리스트 자체가 NULL이면 실패

    ListNode *cur = ll->head;  // 현재 노드를 가리키는 포인터
    ListNode *pre = NULL;      // 현재 노드의 이전 노드를 추적
    int index = 0;             // 삽입할 위치 (리턴용)

    // 삽입 위치 탐색 및 중복 체크
    while (cur != NULL) {
        if (cur->item == item) return -1; // 중복 값 존재 시 실패
        if (cur->item > item) break;      // 삽입 위치 도달
        pre = cur;
        cur = cur->next;
        index++;
    }

		// 정상 삽입 시 삽입된 index 반환, 실패 시 -1 반환
		return insertNodeWithPre(ll, pre, item) == 0 ? index : -1;
}


///////////////////////////////////////////////////////////////////////////////////
// 
// *** 새로 정의한 함수 ***
// *** insertNodeWithPre(LinkedList *ll, ListNode *pre, int item) ***
//
// - pre 노드 뒤에 item 값을 가지는 새 노드를 삽입
// - pre가 NULL이면 head 앞에 삽입 (리스트의 맨 앞)
// - 메모리 할당 실패 시 -1 반환, 성공 시 0 반환
//
///////////////////////////////////////////////////////////////////////////////////
int insertNodeWithPre(LinkedList *ll, ListNode *pre, int item)
{
    // 새 노드 동적 할당
    ListNode *newNode = malloc(sizeof(ListNode));
    if (newNode == NULL) return -1; // 메모리 할당 실패 시

    newNode->item = item; // 새 노드에 데이터 설정

    if (pre == NULL) {
        // 리스트 맨 앞에 삽입
        newNode->next = ll->head;
        ll->head = newNode;
    } else {
        // pre 뒤에 삽입
        newNode->next = pre->next;
        pre->next = newNode;
    }

    ll->size++; // 리스트 크기 증가
    return 0;   // 삽입 성공
}


///////////////////////////////////////////////////////////////////////////////////
//
// printList(LinkedList *ll)
//
// - 연결 리스트의 모든 노드를 순서대로 출력
// - 만약 노드가 하나도 없으면 "Empty" 출력
//
///////////////////////////////////////////////////////////////////////////////////
void printList(LinkedList *ll)
{
	// ListNode의 포인터 cur의 역할: 현재 출력할 노드를 가리킴
	ListNode *cur;
	if (ll == NULL)		// 연결 리스트 자체가 없음 -> LinkedList* 자체가 할당되지 않았거나, 초기화되지 않은 상태
		return;			// ll 자체가 NULL이면 아무것도 하지 않음
	cur = ll->head;		// cur 포인터를 연결 리스트의 첫 노드(head)로 초기화

	if (cur == NULL)		// 리스트는 존재하지만, 노드가 없거나 끝에 도달한 상태면 "Empty" 출력
		printf("Empty");

	while (cur != NULL)		// 노드가 있으면 while문을 통해 순회하며 모든 노드를 출력
	{
		// 현재 노드의 값(item)을 공백과 함께 출력 후 다음 노드로 이동
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
void removeAllItems(LinkedList *ll)	// LinkedList 구조체의 포인터 = 매개변수 ll
{	
	// ListNode의 포인터 cur의 역할: 현재 순회 중인 노드를 가리킴
	ListNode *cur = ll->head;	// cur를 head로 초기화: ll은 연결리스트를 가리키는 포인터. 즉, 연결 리스트의 첫번째 노드(= head 노드)의 주소를 가리킴
	// ListNode의 포인터 tmp의 역할: 삭제 전에 다음 노드를 기억하기 위함
	ListNode *tmp;

	while (cur != NULL){	// 리스트 끝까지 순회
		tmp = cur->next;	// tmp에 cur->next(주소) 저장
		free(cur);			// 펑! free()는 포인터를 해제하는게 아님. 포인터가 가리키고 있는 메모리 공간(노드)을 해제하는 것 = Heap 메모리에서 할당된 공간을 반환하는 것
		cur = tmp;			// cur를 다음 노드로 이동 = tmp가 가리키고 있는 메모리 주소를 cur 포인터에 복사
	}
	// 모든 노드를 해제했으므로 리스트는 빈 상태
	ll->head = NULL;		// head 포인터를 Null로 초기화
	ll->size = 0;			// 리스트 크기 정보도 0으로 초기화
}	// ll은 Null일까? NO!


//////////////////////////////////////////////////////////////////////////////////
//
// findNode(LinkedList *ll, int index)
//
// - index 위치의 노드를 찾아서 그 노드의 포인터를 반환
// - 유효 범위가 아니면(NULL이나 index가 범위를 벗어나면) NULL 반환
//
//////////////////////////////////////////////////////////////////////////////////
ListNode *findNode(LinkedList *ll, int index){

	// ListNode의 포인터 temp의 역할: 현재 탐색 중인 노드를 가리키는 포인터
	ListNode *temp;

	/*
	* 1) LinkedList* 자체가 할당되지 않았거나, 초기화되지 않은 상태이면 탐색 불가
	* 2) index가 0 미만이거나, ll->size와 같거나 크면(= 리스트 범위를 벗어나면) 탐색 불가
	* 이므로 NULL을 반환하고 종료
	*/
	if (ll == NULL || index < 0 || index >= ll->size)
		return NULL;

	temp = ll->head;	// temp를 연결 리스트의 head 노드로 초기화

	/*
	* 리스트는 존재하지만, head가 NULL이면 (노드가 없거나 끝에 도달한 상태면) 찾을 수 없으므로 NULL 반환
	* 또한 index가 음수인지 재검사 (추가 안전장치)
	*/
	if (temp == NULL || index < 0)
		return NULL;

	/*
	* index 만큼 temp를 다음(next) 노드로 이동시키면서
	* index가 0이 될 때(이동 완료) 해당 노드를 반환
	*/
	while (index > 0){
		// temp가 이동 중 NULL이 되면(끝까지 간 뒤) 유효 노드를 못 찾은 것이므로 NULL 반환
		temp = temp->next;
		if (temp == NULL)
			return NULL;
		// 한 칸 이동할 때마다 index를 1 감소시켜 index == 0이 될 때까지 최초 index 값 만큼 이동
		index--;
	}
	// 이동이 끝나면 temp가 index 번 째 노드를 가리키므로 그것을 반환
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

	// ListNode의 포인터 pre의 역할: 새 노드를 삽입할 위치의 직전 노드를 가리키는 포인터
	// cur의 역할: 실제 삽입되는 위치의 노드(원래 index 위치 노드)를 가리키는 포인터
	ListNode *pre, *cur;

	/* 삽입 실패 case #1
	* 1) LinkedList* 자체가 할당되지 않았거나, 초기화되지 않은 상태이면 삽입 불가
	* 2) index가 0 미만이거나, ll->size+1 보다 크면 삽입 불가 (= 리스트의 마지막 위치 바로 다음까지 허용)
	* 이므로 -1 반환하고 종료 */
	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	/* 삽입 성공 case #1 - index 0
	* 리스트는 존재하지만, head가 NULL이거나 (노드가 없거나 끝에 도달한 상태)
	* 혹은 맨 앞에 삽입하는 경우 */
	if (ll->head == NULL || index == 0){
		cur = ll->head;		// cur를 기존 연결리스트의 head 노드로 초기화 -> 동적할당으로 새로 만든 노드의 next 포인터가 기존 노드를 가리킬 수 있도록 준비
		ll->head = malloc(sizeof(ListNode));	// 새 노드를 힙 메모리에 동적 할당 후, 그 주소를 반환해 ll->head에 다시 저장
		ll->head->item = value;		// 새 노드에 데이터(value) 저장
		ll->head->next = cur;		// 새 노드의 next를 기존 head(cur)로 연결
		ll->size++;					// 리스트 크기 증가
		return 0;					// 삽입 성공이므로 0 반환
	}

	/* 삽입 성공 case #2 - 중간 혹은 맨 뒤
	* index-1의 위치를 찾고, 그 노드(pre)의 next에 새 노드 연결 */
	if ((pre = findNode(ll, index - 1)) != NULL){
		cur = pre->next;	// pre -> next가 원래 index 위치의 노드
		pre->next = malloc(sizeof(ListNode));	// 새 노드를 동적 할당 후, pre -> next로 연결
		pre->next->item = value;	// 새 노드에 데이터(value) 저장
		pre->next->next = cur;		// 새 노드의 next를 cur로 연결
		ll->size++;					// 리스트 크기 증가
		return 0;					// 삽입 성공이므로 0 반환
	}
	// 삽입 실패 case #2 - pre를 찾지 못한 경우, etc
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

	// ListNode의 포인터 pre의 역할: 삭제할 노드의 직전 노드를 가리키는 포인터
	// cur의 역할: 실제 삭제되는 위치의 노드(원래 index 위치 노드)를 가리키는 포인터
	ListNode *pre, *cur;

	/* 삭제 실패 case #1
	* 1) LinkedList* 자체가 할당되지 않았거나, 초기화되지 않은 상태이면 삭제 불가
	* 2) index가 0 미만이거나, ll->size+와 같거나 크면 삭제 불가
	* 이므로 -1 반환하고 종료 */
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	/* 삭제 성공 case #1 - index 0
	* head가 가리키고 있는 노드를 삭제해야 하므로 head 포인터를 새로 조정 */
	if (index == 0){
		cur = ll->head->next;	// cur에 head->next를 저장 (두번째 노드)
		free(ll->head);		// 현재 head 노드 메모리 해제
		ll->head = cur;		// head가 두번째 노드를 가리키도록 갱신
		ll->size--;			// 리스트 크기 감소
		return 0;			// 삭제 성공이므로 0 반환
	}

	/* 삭제 성공 case #2 - 중간 혹은 맨 뒤
	* index-1의 위치를 찾고, 그 노드(pre)의 next가 삭제 대상 */
	if ((pre = findNode(ll, index - 1)) != NULL){
		if (pre->next == NULL)	// pre->next가 삭제 대상 노드이므로, 없으면 에러
			return -1;
		cur = pre->next;	// cur에 삭제 대상 노드 저장
		pre->next = cur->next;	// pre->next가 '삭제 대상 노드의 다음 노드'를 가리키도록 연결
		free(cur);			// cur 메모리 해제
		ll->size--;			// 리스트 크기 감소
		return 0;			// 삭제 성공이므로 0 반환
	}
	// 삭제 실패 case #2 - pre를 찾지 못한 경우, etc
	return -1;
}