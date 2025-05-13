# 아홉 난쟁이의 키를 저장할 리스트, 키 입력 받기, 모든 난쟁이 키의 합
height = []
for i in range(9):
    height.append(int(input()))
tall = sum(height)

# 가짜 난쟁이 2명을 찾기 위한 변수
fake1, fake2 = 0, 0
found = False  # 찾았는지 여부를 체크하는 변수

# 완전탐색: 두 명을 선택하여 합에서 제외했을 때 100이 되는 경우 찾기
for i in range(9):
    for j in range(i + 1, 9):  # 중복 제거를 위해 j = i + 1부터 시작
        if tall - (height[i] + height[j]) == 100:
            fake1, fake2 = height[i], height[j]
            found = True  # 찾았으므로 플래그 설정
            break  # 내부 루프 탈출
    if found:
        break  # 외부 루프도 탈출

# 가짜 난쟁이 제거 및 키 오름차순 정렬
height.remove(fake1)
height.remove(fake2)
height.sort()

# 최종적으로 남은 7명의 키 출력
for h in height:
    print(h)