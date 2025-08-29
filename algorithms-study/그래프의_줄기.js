/*
 * 그래프의 줄기 (BOJ 24461)
 * https://www.acmicpc.net/problem/24461
 *
 * [Problem & Solvings]
 * - 문제: 사이클이 없는 무방향 그래프(트리)에서 "줄기"에 해당하는 정점들을 구하라.
 *   - 줄기란: 트리의 리프 노드들을 바깥에서부터 반복적으로 제거했을 때 끝까지 남는 정점들
 * - 해결 전략:
 *   - 각 정점의 차수를 세고, 차수가 1인 리프 노드를 큐에 넣는다.
 *   - 리프 노드를 반복적으로 제거하면서 연결된 노드들의 차수를 갱신한다.
 *   - 큐의 크기가 2 이하가 되면, 더 이상 줄기를 제거할 수 없음 → 남아있는 정점들이 줄기
 *
 * [Pseudocode]
 * 1. N과 N-1개의 간선을 입력받아 인접 리스트 graph와 차수 배열 degree를 구성
 * 2. degree가 1인 리프 노드들을 큐에 삽입
 * 3. 큐의 길이가 2 이하가 될 때까지 다음 반복 수행:
 *    - 큐에서 모든 리프 노드를 꺼냄
 *    - 해당 노드를 removed[]로 표시
 *    - 연결된 노드들의 차수를 감소시키고, 차수가 1이 된 노드를 큐에 삽입
 * 4. 반복 종료 후, removed되지 않은 정점들을 오름차순 출력
 *
 * [Time Complexity]
 * - 인접 리스트 구성: O(N)
 * - 리프 노드 제거 루프: O(N)
 * - 최종 결과 탐색 및 정렬: O(N log N)
 * - 총 시간 복잡도: O(N log N) (정렬 포함), O(N) (정렬 제외)
 */

// Run by Node.js
const readline = require("readline");

(async () => {
  const rl = readline.createInterface({ input: process.stdin });
  const input = [];

  for await (const line of rl) {
    input.push(line.trim());
    const N = Number(input[0]);
    if (input.length === N) {
      rl.close();
      break;
    }
  }

  const N = Number(input[0]);
  const edges = input.slice(1).map((line) => line.split(" ").map(Number));

  // 인접 리스트 초기화
  const graph = Array.from({ length: N }, () => []);
  const degree = Array(N).fill(0); // 각 노드의 연결 개수 (차수)
  const removed = Array(N).fill(false); // 제거된 노드 추적용

  // 간선 정보 입력 및 차수 계산
  for (const [u, v] of edges) {
    graph[u].push(v);
    graph[v].push(u);
    degree[u]++;
    degree[v]++;
  }

  // 리프 노드 큐 초기화
  const queue = [];
  for (let i = 0; i < N; i++) {
    if (degree[i] === 1) {
      queue.push(i);
    }
  }

  // 리프 제거 반복 (큐에 리프 노드가 2개 이하로 남을 때까지 반복)
  while (queue.length > 2) {
    const leafCount = queue.length;

    for (let i = 0; i < leafCount; i++) {
      const leaf = queue.shift(); // 큐에서 리프 노드 하나 꺼냄
      if (removed[leaf]) continue; // 이미 제거된 노드면 건너뜀 (중복 방지)

      removed[leaf] = true;

      for (const neighbor of graph[leaf]) {
        if (removed[neighbor]) continue;
        degree[neighbor]--;
        if (degree[neighbor] === 1) {
          queue.push(neighbor);
        }
      }
    }
  }

  // 제거되지 않은 노드가 줄기
  const result = [];
  for (let i = 0; i < N; i++) {
    if (!removed[i]) {
      result.push(i);
    }
  }

  console.log(result.sort((a, b) => a - b).join(" ")); // 오름차순 정렬 + 공백으로 연결해 한 줄 출력
  process.exit();
})();
