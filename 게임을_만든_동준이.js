/* 
 * 게임을 만든 동준이 (BOJ 2847)
 * https://www.acmicpc.net/problem/2847
 * 
 * [Problem & Solvings]
 * - 각 레벨의 점수가 오름차순이 되면 안 됨 (뒤에서부터 보며 감소시키기)
 * - 앞 레벨이 뒤보다 점수가 높으면 OK, 낮거나 같으면 감소시켜야 함
 * - 최소한의 감소 수를 계산하면 됨
 *
 * [Pseudocode]
 * 1. 입력을 역순으로 순회하면서 이전 점수가 현재보다 크거나 같으면 감소
 * 2. 감소량을 누적해서 출력
 *
 * [Time Complexity]
 * - O(N) : N개의 레벨을 한번 순회함
 */

// Run by Node.js
const readline = require("readline");

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
});

let input = [];

rl.on("line", (line) => {
    input.push(Number(line.trim()));
}).on("close", () => {
    const N = input[0];
    const scores = input.slice(1);
    let result = 0;

    // 뒤에서부터 순회하며 감소 필요 여부 확인
    for (let i = N - 1; i > 0; i--) {
        if (scores[i - 1] >= scores[i]) {
            const decrease = scores[i - 1] - scores[i] + 1;
            scores[i - 1] -= decrease;
            result += decrease;
        }
    }
    
    console.log(result);
    process.exit();
});