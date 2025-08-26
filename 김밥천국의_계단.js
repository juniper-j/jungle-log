/*
 * 김밥천국의 계단 (BOJ 28069)
 * https://www.acmicpc.net/problem/28069
 * 
 * [Problem & Solvings]
 * - 문제: 0번 계단에서 출발해 n번 계단에 도달해야 한다.
 * - 이동 방식은 두 가지:
 *     1) +1 이동
 *     2) 현재 위치의 절반을 더해 전진 (i + Math.floor(i / 2))
 * - 목표: 최소 몇 번만에 도달 가능한지 계산하고, 주어진 k 이하로 도달하면 "minigimbob", 아니면 "water" 출력
 * - 해결 전략:
 *     - BFS로 최소 이동 횟수 탐색
 *     - 방문한 계단을 visited 배열로 체크하며 depth 기록
 *     - 도달 조건: visited[n] <= k
 *
 * [Pseudocode]
 * 1. n, k 입력
 * 2. visited 배열을 -1로 초기화
 * 3. 큐에 시작 위치 0과 depth 0을 삽입
 * 4. while 큐가 빌 때까지:
 *    - 현재 계단과 이동 횟수 꺼내기
 *    - +1, +(i//2) 위치로 이동 가능하면 큐에 추가
 * 5. visited[n]이 k 이하인지 판별 후 결과 출력
 *
 * [Time Complexity]
 * - 상태 공간: 0 ~ n
 * - 각 노드에서 연산 2회 → O(n)
 * - 전체 시간 복잡도: O(n)
 * 
 * [Insight]
 * - ✅ Python의 collections.deque.popleft()
 *   → 시간복잡도: O(1)
 *   → 큐의 앞에서 꺼내기 (진짜 큐)
 * - ❌ JavaScript의 Array.prototype.shift()
 *   → 시간복잡도: O(n)
 *   → 큐의 앞에서 꺼내긴 하지만, 남은 요소들을 모두 앞으로 밀기 때문에 느림
 */

// // Run by Node.js
// const readline = require("readline");

// (async () => {
//     const rl = readline.createInterface({ input: process.stdin });
//     const input = [];

//     for await (const line of rl) {
//         input.push(...line.trim().split(' ').map(Number));
//         rl.close();
//     }

//     const [n, k] = input;   // n: 목표 계단, k: 최대 이동 횟수

//     const visited = Array(n + 1).fill(-1); // 방문 여부 및 이동 횟수 기록
//     const queue = [];

//     visited[0] = 0; // 시작 계단 0은 방문 완료, 이동 횟수 0
//     queue.push(0);

//     while (queue.length > 0) {
//         const current = queue.shift(); // 현재 계단
//         const count = visited[current]; // 현재 이동 횟수

//         // +1 이동
//         const next1 = current + 1;
//         if (next1 <= n && visited[next1] === -1) {
//             visited[next1] = count + 1;
//             queue.push(next1);
//         }

//         // 절반 더하기 이동
//         const next2 = current + Math.floor(current / 2);
//         if (next2 <= n && visited[next2] === -1) {
//             visited[next2] = count + 1;
//             queue.push(next2);
//         }
//     }

//     if (visited[n] !== -1 && visited[n] <= k) {
//         console.log("minigimbob");
//     } else {
//         console.log("water");
//     }

//     process.exit();
// }
// )();


/*
 * 김밥천국의 계단 (BOJ 28069)
 * https://www.acmicpc.net/problem/28069
 *
 * [Problem & Solvings]
 * - 0에서 n까지 최소 몇 번에 도달 가능한지
 * - 이동 방법: +1 또는 i + floor(i/2)
 * - BFS로 최소 이동 횟수 계산
 *
 * [Pseudocode]
 * 1. 입력 n, k
 * 2. visited 배열 -1 초기화
 * 3. 큐에 0부터 시작
 * 4. +1 / +floor(i/2) 이동
 * 5. visited[n] <= k → minigimbob / water
 *
 * [Time Complexity]
 * - BFS: O(n)
 */

// // Run by Node.js
// const readline = require("readline");

// (async () => {
//     const rl = readline.createInterface({ input: process.stdin });
//     const input = [];

//     for await (const line of rl) {
//         input.push(...line.trim().split(' ').map(Number));
//         rl.close();
//     }

//     const [n, k] = input;   // n: 목표 계단, k: 최대 이동 횟수

//     const visited = Array(n + 1).fill(-1); // 방문 여부 및 이동 횟수 기록
//     const queue = [];
//     let head = 0;

//     queue.push(0);
//     visited[0] = 0; // 시작 계단 0은 방문 완료, 이동 횟수 0

//     while (head < queue.length) {
//         const current = queue[head++];
//         const count = visited[current];

//         // +1 이동
//         const next1 = current + 1;
//         if (next1 <= n && visited[next1] === -1) {
//             visited[next1] = count + 1;
//             queue.push(next1);
//         }

//         // 절반 더하기 이동
//         const next2 = current + Math.floor(current / 2);
//         if (next2 <= n && visited[next2] === -1) {
//             visited[next2] = count + 1;
//             queue.push(next2);
//         }

//         // 탈출 조건
//         if (current === n) break;
//     }

//     const result = visited[n] !== -1 && visited[n] <= k ? "minigimbob" : "water";
//     console.log(result);
// }
// )();


// [Problem & Solvings]
// - 0부터 시작하여 N까지 도달하는 최소 연산 횟수를 구한다.
//   연산 방법은 두 가지:
//   1) 현재 위치 + 1
//   2) 현재 위치 + ⌊현재 위치 / 2⌋
// - dp[i] = i까지 도달하는 최소 연산 횟수
// - 목표 위치 N까지 dp를 갱신하면서, dp[N]이 K 이하면 "minigimbob", 아니면 "water"

// [Pseudocode]
// 1. 입력값 N, K 읽기
// 2. dp 배열을 Infinity로 초기화하고 dp[0] = 0 설정
// 3. for i = 1 to N:
//    - dp[i] = min(dp[i], dp[i-1] + 1)  → +1 연산
//    - dp[i + floor(i/2)] = min(기존값, dp[i] + 1) → 순간이동 연산
// 4. dp[N] <= K이면 "minigimbob", 아니면 "water" 출력

// [Time Complexity]
// - 시간복잡도: O(N)
// - 공간복잡도: O(N)

const readline = require("readline");

(async () => {
  const rl = readline.createInterface({ input: process.stdin });
  const input = [];

  for await (const line of rl) {
    input.push(...line.trim().split(" ").map(Number));
    if (input.length >= 2) rl.close();
  }

  const [N, K] = input;

  const dp = new Array(N + 1).fill(Infinity);
  dp[0] = 0;

  for (let i = 1; i <= N; i++) {
    // 기본 이동: +1
    dp[i] = Math.min(dp[i], dp[i - 1] + 1);

    // 순간이동: i + floor(i/2)
    const teleport = i + Math.floor(i / 2);
    if (teleport <= N) {
      dp[teleport] = Math.min(dp[teleport], dp[i] + 1);
    }
  }

  console.log(dp[N] <= K ? "minigimbob" : "water");
}
)();
