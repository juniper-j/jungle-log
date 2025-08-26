/* 
 * 추월 (BOJ 2002)
 * https://www.acmicpc.net/problem/2002
 * 
 * [Problem & Solvings]
 * - 차량이 터널에 들어간 순서와 나온 순서를 비교해 추월한 차량의 수를 세는 문제
 * - 들어간 순서와 나온 순서 사이에 상대적인 순서가 뒤바뀐 차량 쌍의 수를 세면 됨
 * - 이는 순열에서 순서가 뒤틀린 쌍의 수를 세는 것과 동일하므로 O(N^2) 브루트포스 방식 or 좌표 압축 + 세그먼트 트리로 O(N log N) 가능
 * 
 * [Pseudocode]
 * 1. N을 입력받고, enter[]: 들어간 순서 저장
 * 2. exit[]: 나온 순서 저장
 * 3. enter 배열의 차량 이름에 순서를 매김 (Map)
 * 4. exit 배열을 enter 순서 기준으로 숫자 배열로 바꿈
 * 5. 이 숫자 배열에서 앞에 있는 숫자가 뒤에 있는 숫자보다 클 경우 추월
 * 6. 이중 for문으로 추월 횟수 세기
 * 
 * [Time Complexity]
 * - O(N^2) 브루트포스 방식 (최대 1000이므로 허용)
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
    const N = Number(input[0]);             // 차량 수
    const enter = input.slice(1, N + 1);    // 들어간 순서
    const exit = input.slice(N + 1, 2 * N + 1);        // 나온 순서

    // 들어간 순서에 차량 이름별로 순서 매기기
    const orderMap = new Map();
    enter.forEach((car, idx) => {
        orderMap.set(car, idx);
    });

    // 나온 순서를 들어간 순서 기준의 숫자 배열로 변환
    const exitOrder = exit.map(car => orderMap.get(car));

    // 이중 for문으로 추월 횟수 세기
    let overtakes = 0; // 추월 횟수
    for (let i = 0; i < N; i++) {
        for (let j = i + 1; j < N; j++) {
            if (exitOrder[i] > exitOrder[j]) {
                overtakes++;
                break; // i번째 차량이 추월했으면 다음 i로 넘어감
            }
        }
    }

    console.log(overtakes); // 결과 출력
    process.exit();
}
);