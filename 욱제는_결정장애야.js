/*
욱제는 결정장애야! (BOJ 14646)
https://www.acmicpc.net/problem/14646

[Problem & Solvings]
- 욱제는 N개의 메뉴 중에서 하나를 고르는 연습을 위해 매일 2N번의 연습을 진행함
- 각 연습에서 어떤 메뉴를 고르며, 같은 메뉴를 두 번 고르면 그 메뉴는 고정됨 (더 이상 고를 수 없음)
- 동일한 메뉴를 두 번 고르기 전까지, 즉 ‘고정되기 전까지’는 여러 개를 동시에 고른 상태가 될 수 있음
- 이 중 동시에 고른 메뉴의 수가 최대일 때 그 수를 출력

[Pseudocode]
1. N과 2N개의 메뉴 고르기 기록을 입력 받음
2. 메뉴별로 한번이라도 등장한 경우 Set에 추가
    - 만약 이미 Set에 있으면 두 번째 등장 → 고정 → 제거
    - 첫 등장 → 고름
3. Set의 크기로 현재 동시에 고르고 있는 메뉴 수를 축적
4. 그 중 가장 큰 수를 출력

[Time Complexity]
- 입력 처리: O(N)
- 메뉴 처리: O(2N)
- 최종 복잡도: O(N)
*/

// Run by Node.js
const readline = require('readline');

(async () => {
    const rl = readline.createInterface({ input: process.stdin });
    const input = [];

    for await (const line of rl) {
        input.push(...line.trim().split(' '));
        const N = Number(input[0]);
        if (input.length === 2 * N + 1) {
            rl.close();
            break;
        }
    }

    const N = Number(input[0]); // 메뉴 개수
    const logs = input.slice(1).map(Number); // 2N번 고른 메뉴 기록

    const selected = new Set(); // 현재 고르고 있는 메뉴들
    let max = 0; // 동시에 고른 최대 메뉴 수

    for (const menu of logs) {
        if (selected.has(menu)) {
            selected.delete(menu); // 두 번째 등장 → 고정 → 제거
        } else {
            selected.add(menu); // 첫 등장 → 고름
            max = Math.max(max, selected.size); // 최대 동시 선택 수 갱신
        }
    }

    console.log(max); // 결과 출력
    process.exit();
})();
