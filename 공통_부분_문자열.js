/* 
 * 공통 부분 문자열 (BOJ 5582)
 * https://www.acmicpc.net/problem/5582
 * 
 * [Problem & Solvings]
 * - 두 문자열에서 **공통으로 존재하는 연속된 부분 문자열** 중 가장 긴 길이를 구한다.
 * - 부분 문자열이 연속되어야 하므로, **Longest Common Substring** 문제로 볼 수 있다.
 * - 동적 계획법(DP)을 사용하여 해결:
 *   dp[i][j] = A의 i번째 문자와 B의 j번째 문자가 같을 때,
 *             두 문자열의 (i-1, j-1) 위치까지의 공통 부분 문자열 길이에 +1
 *
 * [Pseudocode]
 * 1. 두 문자열 입력 받기
 * 2. dp 테이블 생성 (A.length+1) x (B.length+1), 초기값 0
 * 3. 이중 루프로 문자 비교
 *    - 같으면 dp[i][j] = dp[i-1][j-1] + 1
 *    - 다르면 dp[i][j] = 0
 * 4. 모든 dp[i][j] 중 최대값을 추적
 * 5. 최대값 출력
 *
 * [Time Complexity]
 * - 시간복잡도: O(N*M) (N, M은 각 문자열 길이)
 * - 공간복잡도: O(N*M)
 */

// Run by Node.js
const readline = require("readline");

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
});

const input = [];

rl.on("line", (line) => {
    input.push(line.trim());
    if (input.length === 2) {
        rl.close();
    }
});

rl.on("close", () => {
    const A = input[0];
    const B = input[1];
    const n = A.length;
    const m = B.length;

    // dp[i][j] = A[i-1], B[j-1] 까지의 공통 부분 문자열 최대 길이
    const dp = Array.from({ length: n + 1 }, () => Array(m + 1).fill(0));   // n+1개의 행과 m+1개의 열을 갖는 2차원 배열 dp -> 0으로 초기화
    let maxLength = 0; // 최대 길이 추적용

    for (let i = 1; i <= n; i++) {
        for (let j = 1; j <= m; j++) {
            if (A[i - 1] === B[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1; // 문자 같으면 이전 위치에서 +1
                maxLength = Math.max(maxLength, dp[i][j]); // 최대 길이 갱신
            }
        }
    }

    console.log(maxLength);
    process.exit();
});