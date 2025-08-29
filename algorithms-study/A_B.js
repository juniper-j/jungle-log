/*
 * A → B (BOJ 16953)
 * https://www.acmicpc.net/problem/16953
 *
 * [Problem & Solvings]
 * - 문제: 정수 A를 B로 바꾸는 최소 연산 횟수를 구하라.
 *   - 사용 가능한 연산:
 *     1. A를 2배
 *     2. A의 뒤에 1을 추가 (예: 4 → 41)
 * - 목표: A → B를 만드는 최소 연산 횟수를 구하고, 불가능할 경우 -1 출력
 * - 해결 전략:
 *   - B에서 A로 역방향으로 추적 (BFS or 그리디)
 *   - B를 2로 나누거나, 끝자리가 1이면 1을 제거하는 방식으로 줄이기
 *   - A에 도달하면 성공, 그 외에는 반복
 *
 * [Pseudocode]
 * 1. A, B를 입력받는다.
 * 2. count를 1로 초기화
 * 3. while B >= A:
 *    - B === A → count 반환
 *    - B % 10 === 1 → B에서 1 제거 (B = Math.floor(B / 10))
 *    - B % 2 === 0 → B /= 2
 *    - 그 외의 경우 → 불가능 → -1 반환
 * 4. 루프 종료 시에도 A에 도달 못 하면 -1 출력
 * 
 * [Time Complexity]
 * - 매번 B를 2로 나누거나 자릿수를 줄임 → 최대 log(B)
 * - 총 시간 복잡도: O(log B)
 */

// Run by Node.js
// 역방향 Greedy
const readline = require("readline");

(async () => {
    const rl = readline.createInterface({ input: process.stdin });
    const input = [];

    for await (const line of rl) {
        input.push(...line.trim().split(' ').map(Number));
        rl.close();
    }

    let [A, B] = input; // A와 B를 입력받음
    let count = 1;      // 연산 횟수 초기화

    while (B > A) {
        if (B % 10 === 1) {
            B = Math.floor(B / 10); // 뒤에 1을 제거
        } else if (B % 2 === 0) {
            B /= 2; // 2로 나누기
        } else {
            break;
        }
        count++; // 연산 횟수 증가
    }

    // B가 A와 같아지면 count 출력, 아니면 -1 출력
    console.log(B === A ? count : -1);
    process.exit();
}
)();

// // BFS
// const readline = require("readline");

// (async () => {
//   const rl = readline.createInterface({ input: process.stdin });
//   const input = [];

//   for await (const line of rl) {
//     input.push(...line.trim().split(' ').map(Number));
//     rl.close();
//   }

//   const [A, B] = input;
//   const queue = [[A, 1]]; // (현재 값, 연산 횟수)

//   let found = false;

//   while (queue.length > 0) {
//     const [current, count] = queue.shift();

//     if (current === B) {
//       console.log(count);
//       found = true;
//       break;
//     }

//     const next1 = current * 2;
//     const next2 = current * 10 + 1;

//     if (next1 <= B) queue.push([next1, count + 1]);
//     if (next2 <= B) queue.push([next2, count + 1]);
//   }

//   if (!found) {
//     console.log(-1);
//   }

//   process.exit();
// })();
