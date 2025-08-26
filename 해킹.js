/* 
 * 추월 (BOJ 10282)
 * https://www.acmicpc.net/problem/10282
 * 
 * [Problem & Solvings]
 * - 한 컴퓨터가 해킹되면, 의존된 컴퓨터도 순차적으로 감염된다.
 * - 방향성 그래프에서 시작점으로부터 도달 가능한 노드 수와 최대 도달 시간을 구하는 문제.
 * - 다익스트라 알고리즘으로 해결 가능 (최단거리 + 감염 확산 시간).
 *
 * [Pseudocode]
 * 1. 테스트케이스 수 T 입력
 * 2. 각 테스트케이스에 대해 컴퓨터 수 n, 의존성 수 d, 시작 컴퓨터 c 입력
 * 3. d개의 의존성을 역방향으로 그래프에 저장 (b → a, s초)
 * 4. 시작점 c에서 다익스트라 수행
 * 5. 감염된 컴퓨터 수와 그 중 최대 감염 시간 출력
 *
 * [Time Complexity]
 * O((V + E) log V), 다익스트라
 * T = 100, V = 10^4, E = 10^5 → 시간 내 충분
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
}).on("close", () => {
    const T = Number(input[0]);
    const idx = 1;

    for (let t = 0; t < T; t++) {
        const [n, d, c] = input[idx + t].split(" ").map(Number);
        const graph = Array.from({ length: n + 1 }, () => []);
        const
    }
})