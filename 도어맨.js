/*
도어맨 (BOJ 5002)
https://www.acmicpc.net/problem/5002

[Problem & Solvings]
- 클럽 입구에 남(W)∙여(M) 대기열이 한 줄로 서 있다. (문제 원문은 W=여성, M=남성)
- 도어맨은 "언제나" 남녀 수 차이가 X 이하가 되도록 입장을 관리해야 한다.
- 규칙: 맨 앞사람을 통과시키되, 필요하다면 "앞의 두 사람"의 순서를 바꿔서(=둘 중 하나를 먼저 들여보내서)
        조건을 만족시킬 수 있다. 두 사람 뒤로는 건드릴 수 없다.
- 목표: 최대 몇 명까지 입장시킬 수 있는지 구하라.
- 핵심 전략(그리디):
  1) 기본적으로 줄의 맨 앞을 들인다.
  2) 다음 사람을 들이면 성비 차이가 X+1로 넘어가면 입장 불가 → 바로 뒤의 사람과 “교체 시도”.
     - 두 번째 사람을 먼저 들였을 때 성비 차기가 X 이내면 교체하여 들인다.
     - 그래도 불가면 더는 누구도 들일 수 없으므로 종료.
- 줄 길이가 최대 100(원문은 1e6까지도 알려져 있으나, 본 풀이의 알고리즘은 O(N)이라 길이에 무관하게 통과 가능)

[Greedy Idea]
1) 가능하면 맨 앞을 바로 들인다.
2) 맨 앞을 들이면 성비 차이를 초과하는 경우에만 “앞의 두 사람” 스왑을 시도:
   - 두 번째를 먼저 들였을 때 허용(X 이하)이면 두 사람의 순서를 실제로 스왑하고,
     다음 루프에서 바뀐 맨 앞을 처리한다.
   - 그래도 불가면 더 진행 불가 → 종료.
→ 각 위치를 최대 한두 번만 검사하므로 O(N)에 해결.

[Pseudocode]
1. 입력: read X, S
2. cntW, cntM ← 0, 0
3. arr ← S as array
4. i ← 0
    while i < arr.length:
        first ← arr[i]
        if canEnter(first):          // 입장해도 |W-M| ≤ X ?
            admit(first); i++
        else if i+1 < arr.length and canEnter(arr[i+1]):
            swap(arr[i], arr[i+1])   // 두 번째를 앞으로 (실제 스왑)
            // i는 그대로 두고, 다음 루프에서 새 맨 앞을 다시 평가
        else:
            break
5. 출력: print cntW + cntM

[Time Complexity]
- 각 원소를 최대 몇 번만 확인/스왑 → O(N)
- 추가 메모리 O(1)
*/

// Run by Node.js
const readline = require('readline');

(async () => {
    const rl = readline.createInterface({ input: process.stdin });
    const lines = [];
    for await (const line of rl) lines.push(line.trim());
    rl.close();
  
    // ---------- 입력 ----------
    const X = parseInt(lines[0], 10);      // 허용되는 남녀 수 차이의 최대값
    const arr = (lines[1] || '').split(''); // 대기열을 배열로 (스왑을 위해)
  
    // ---------- 상태 ----------
    let cntW = 0; // 현재까지 입장한 여성 수
    let cntM = 0; // 현재까지 입장한 남성 수
  
    // 커스텀 검사: ch를 한 명 더 입장시키면 |W-M| ≤ X 인지?
    const canEnter = (ch) => {
      if (ch === 'W') return Math.abs((cntW + 1) - cntM) <= X;
      return Math.abs(cntW - (cntM + 1)) <= X; // ch === 'M'
    };
  
    // 실제 입장 처리 (카운터 갱신)
    const admit = (ch) => {
      if (ch === 'W') cntW++;
      else cntM++;
    };
  
    // ---------- 메인 루프 ----------
    let i = 0; // 현재 대기열의 맨 앞 인덱스
    while (i < arr.length) {
      const first = arr[i];
  
      if (canEnter(first)) {
        // 1) 맨 앞을 바로 들일 수 있으면 들이고 다음으로 진행
        admit(first);
        i++;
      } else {
        // 2) 맨 앞은 규칙을 깨므로, "앞의 두 사람" 스왑으로만 구제 가능
        if (i + 1 < arr.length && canEnter(arr[i + 1])) {
          // 핵심: 실제로 앞의 두 사람을 스왑해 두 번째가 앞으로 오게 만든다.
          // 이렇게 해야 두 번째를 다시 세는 이중 집계를 방지할 수 있다.
          [arr[i], arr[i + 1]] = [arr[i + 1], arr[i]];
          // i는 그대로 둔다. 다음 반복에서 바뀐 맨 앞(arr[i])을 재평가.
        } else {
          // 3) 스왑해도 불가능 → 더는 진행 불가
          break;
        }
      }
    }
  
    // ---------- 출력 ----------
    console.log(cntW + cntM);
    process.exit(0);
  })();
