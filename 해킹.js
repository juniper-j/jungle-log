/* 
 * 해킹 (BOJ 10282)
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
class MinHeap {
    constructor() {
        this.heap = [];
    }

    push(item) {
        this.heap.push(item);
        this._bubbleUp();
    }

    pop() {
        if (this.heap.length === 1) return this.heap.pop();
        const top = this.heap[0];
        this.heap[0] = this.heap.pop();
        this._sinkDown();
        return top;
    }

    _bubbleUp() {
        let i = this.heap.length - 1;
        const item = this.heap[i];

        while (i > 0) {
            const parseIdx = Math.floor((i - 1) / 2);
            if (this.heap[parseIdx][0] <= item[0]) break;
            this.heap[i] = this.heap[parseIdx];
            i = parseIdx;
        }
        this.heap[i] = item;
    }

    _sinkDown() {
        let i = 0;
        const length = this.heap.length;
        const item = this.heap[0];

        while (true) {
            let leftIdx = 2 * i + 1;
            let rightIdx = 2 * i + 2;
            let smallest = i;

            if (leftIdx < length && this.heap[leftIdx][0] < this.heap[smallest][0]) {
                smallest = leftIdx;
            }
            if (rightIdx < length && this.heap[rightIdx][0] < this.heap[smallest][0]) {
                smallest = rightIdx;
            }
            if (smallest === i) break;

            this.heap[i] = this.heap[smallest];
            i = smallest;
        }
        this.heap[i] = item;
    }

    isEmpty() {
        return this.heap.length === 0;
    }
}

const readline = require("readline");

const rl = readline.createInterface({ 
    input: process.stdin,
    output: process.stdout,
});

const input = [];

rl.on("line", (line) => {
    input.push(line.trim());
}).on("close", () => {
    let T = parseInt(input[0]);
    let idx = 1;

    for (let t = 0; t < T; t++) {
        const [n, d, c] = input[idx++].split(" ").map(Number);
        const graph = Array.from({ length: n + 1 }, () => []);  // n+1개의 행을 갖는 빈 배열

        // 의존성: b가 감염되면 s초 뒤 a도 감염
        for (let i = 0; i < d; i++) {
            const [a, b, s] = input[idx++].split(" ").map(Number);
            graph[b].push([a, s]);
        }

        const INF = Infinity;
        const dist = Array(n + 1).fill(INF);
        dist[c] = 0;

        const pq = new MinHeap();
        pq.push([0, c]);    // [시간, 노드]

        while (!pq.isEmpty()) {
            const [curTime, curNode] = pq.pop();

            if (dist[curNode] < curTime) continue;

            for (const [next, time] of graph[curNode]) {
                const newTime = curTime + time;
                if (newTime < dist[next]) {
                    dist[next] = newTime;
                    pq.push([newTime, next]);
                }
            }
        }

        let infected = 0;
        let maxTime = 0;
        
        for (let i = 1; i <= n; i++) {
            if (dist[i] !== INF) {
                infected++;
                maxTime = Math.max(maxTime, dist[i]);
            }
        }

        console.log(`${infected} ${maxTime}`);
    }

    process.exit();
});



/* 
const readline = require('readline');
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});
class Heap {
  list;
  constructor() {
    this.list = [null];
  }

  get size() {
    return this.list.length - 1;
  }
  getChildIdx(idx) {
    const leftIdx = idx * 2;
    const rightIdx = leftIdx + 1;
    const left = this.list[leftIdx];
    const right = this.list[rightIdx];
    if (!right) return leftIdx;
    if (left[0] < right[0]) return leftIdx;
    return rightIdx;
  }
  isSmall(a, b) {
    return a < b;
  }
  push(node) {
    this.list.push(node);
    this.heapUp();
  }
  pop() {
    if (!this.size) return 0;
    let node = this.list[1];
    this.list[1] = this.list[this.size];
    this.list[this.size] = this.list[1];
    this.list.pop();
    this.heapDown();
    return node;
  }
  heapUp() {
    let curIdx = this.size;
    while (1 < curIdx) {
      const parentIdx = Math.floor(curIdx / 2);
      const parent = this.list[parentIdx];
      const cur = this.list[curIdx];
      if (this.isSmall(parent[0], cur[0])) return;

      this.list[parentIdx] = cur;
      this.list[curIdx] = parent;
      curIdx = parentIdx;
    }
  }
  heapDown() {
    let curIdx = 1;
    while (curIdx * 2 <= this.size) {
      const childIdx = this.getChildIdx(curIdx);
      const child = this.list[childIdx];
      const cur = this.list[curIdx];
      if (this.isSmall(cur[0], child[0])) return;
      this.list[childIdx] = cur;
      this.list[curIdx] = child;
      curIdx = childIdx;
    }
  }
}

function dijkstra(graph, start, n) {
  const distance = Array(n + 1).fill(Infinity);
  distance[start] = 0; // 시작 컴퓨터는 즉시 감염

  const heap = new Heap();
  heap.push([0, start]);

  while (heap.size) {
    const current = heap.pop();
    if (!current) break;

    const [currentTime, currentNode] = current;

    if (currentTime > distance[currentNode]) continue;

    for (const { to, time } of graph[currentNode]) {
      const newTime = currentTime + time;

      if (newTime < distance[to]) {
        distance[to] = newTime;
        heap.push([newTime, to]);
      }
    }
  }

  return distance;
}

let testCase = 0;
let currentCase = 0;
let lineCount = 0;
let n = 0,
  d = 0,
  c = 0;
let graph = [];
let dCount = 0;
let results = [];
rl.on('line', (line) => {
  if (testCase === 0) {
    testCase = Number(line);
    return;
  }
  if (lineCount === 0) {
    [n, d, c] = line.split(' ').map(Number);
    graph = Array.from({ length: n + 1 }, () => []);
    lineCount++;
    dCount = 0;
    return;
  }
  const [a, b, s] = line.split(' ').map(Number);
  graph[b].push({ to: a, time: s });
  dCount++;
  if (dCount === d) {
    const dijkstraTime = dijkstra(graph, c, n);

    let count = 0;
    let time = 0;

    for (let i = 1; i <= n; i++) {
      if (dijkstraTime[i] !== Infinity) {
        count++;
        time = Math.max(time, dijkstraTime[i]);
      }
    }

    results.push(`${count} ${time}`);

    currentCase++;
    lineCount = 0;

    if (currentCase === testCase) {
      console.log(results.join('\n'));
      rl.close();
    }
  }
}).on('close', () => {
  process.exit();
});
*/
