/*
 * 연결 요소의 개수 (BOJ 11724)
 * https://www.acmicpc.net/problem/11724

[Problem & Solvings]
- 주어진 그래프에서 연결된 요소(컴포넌트)의 개수를 구하는 문제입니다.
- Union-Find(Disjoint Set Union, DSU) 알고리즘을 사용하여 연결 요소를 찾습니다.
- Union by Size 또는 Union by Rank를 사용하여 트리의 균형을 맞추고 효율적인 연산을 구현합니다.

[Pseudocode]
1.	입력
    노드 수 v와 간선 수 e 입력받기
    Union-Find 초기화 (각 노드는 자기 자신을 부모로 설정)
2.	Union-Find 구현
    find(x): x의 루트를 찾는 함수 (경로 압축)
    union(u, v): u와 v를 연결하는 함수 (Union by Size)
3.	간선 처리
    각 간선에 대해 union(a, b) 호출
4.	연결 요소의 개수 구하기
    모든 노드에 대해 find(i) 호출하여 루트를 구하고, 그 값을 집합에 추가
    집합의 크기를 출력
 */

// Run by Node.js
const readline = require('readline');

const rl = readline.createInterface({ input: process.stdin });

let input = [];
rl.on('line', line => input.push(line.trim()));
rl.on('close', () => {
  const [n, m] = input[0].split(' ').map(Number); // 정점 개수 n, 간선 개수 m
  const parent = Array(n + 1).fill(0).map((_, idx) => idx); // 부모 배열 초기화

  // find 함수 (경로 압축 적용)
  function find(x) {
    if (parent[x] !== x) {
      parent[x] = find(parent[x]); // 루트를 찾아서 경로 압축
    }
    return parent[x];
  }

  // union 함수
  function union(a, b) {
    const rootA = find(a);
    const rootB = find(b);
    if (rootA !== rootB) {
      parent[rootB] = rootA; // 한 쪽을 다른 쪽에 붙임
    }
  }

  // 간선 연결 처리
  for (let i = 1; i <= m; i++) {
    const [u, v] = input[i].split(' ').map(Number);
    union(u, v);
  }

  // 연결 요소 개수 세기 (루트들의 개수)
  const roots = new Set();
  for (let i = 1; i <= n; i++) {
    roots.add(find(i));
  }

  console.log(roots.size);
});
