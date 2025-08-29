/* 
 * Sort 마스터 배지훈의 후계자 (BOJ 20551)
 * https://www.acmicpc.net/problem/20551
 * 
 * [Problem & Solvings]
 * - N개의 정수 배열을 정렬한 후, M개의 쿼리에 대해 각 값이 처음 등장하는 인덱스를 출력하라.
 * - 정렬 후 중복된 값이 있을 수 있으므로, '처음 등장하는 인덱스'를 구해야 함
 * - 정렬된 배열에서 각 쿼리 값을 이분 탐색으로 찾되, lower bound 방식으로 가장 처음 등장하는 위치를 찾음
 * - 값이 없을 경우 -1 출력
 * 
 * [Pseudocode]
 * 1. N, M과 배열 A, 쿼리 Q 입력
 * 2. 배열 A 정렬 -> B
 * 3. 각 쿼리에 대해 이분 탐색으로 A 내에서 처음 등장하는 인덱스를 찾고 저장
 * 4. 결과 출력
 * 
 * [Time Complexity]
 * - 정렬: O(N log N)
 * - 각 쿼리: O(log N)
 * - 전체: O(N log N + M log N)
 */

// Run by Node.js
const readline = require("readline");

const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout,
});

let input = [];

rl.on("line", (line) => {
    input.push(line.trim());  // 한 줄씩 입력 받아 배열에 저장
}).on("close", () => {
    // 모든 입력이 끝난 후 실행
    const [N, M] = input[0].split(" ").map(Number); // 첫 줄에서 N, M 추출
    const A = input.slice(1, N + 1).map(Number);  // 다음 N줄: 배열 A
    const queries = input.slice(N + 1).map(Number); // 다음 M줄: 쿼리들

    A.sort((a, b) => a - b);  // 배열 A 오름차순으로 정렬

    const results = [];

    // 이진 탐색 함수: lower_bound 방식 (첫 등장 인덱스 찾기)
    const binarySearch = (target) => {
        let left = 0;
        let right = N - 1;
        let idx = -1;

        while (left <= right) {
            const mid = Math.floor((left + right) / 2);
            if (A[mid] === target) {
                idx = mid; // 일단 찾았으니 기록
                right = mid - 1; // 더 왼쪽에 같은 값이 있는지 탐색
            } else if (A[mid] < target) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        return idx; // 존재하지 않으면 -1
    };

    // 각 쿼리에 대해 결과 저장
    for (const q of queries) {
        results.push(binarySearch(q));
    }

    // 결과 출력
    console.log(results.join("\n"));
});

/*
 * [Problem & Solvings]
 * - 정렬된 배열에서 여러 쿼리 값의 최초 인덱스를 빠르게 찾아야 한다.
 * - 같은 값을 여러 번 찾아야 하므로 이진 탐색 + Map으로 최적화한다.
 *
 * [Pseudocode]
 * 1. N, M 입력 받고
 * 2. N개의 수를 정렬하고 Map으로 최초 등장 index 저장
 * 3. M개의 쿼리값에 대해 Map에서 빠르게 index 가져오기
 * 
 * [Time Complexity]
 * - 정렬: O(N log N)
 * - Map 생성: O(N)
 * - 쿼리 처리: O(1) * M => 총 O(N log N + M)
 */

const readline = require("readline");

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

let lineCount = 0;
let N = 0, M = 0;
let arr = [], queries = [];

rl.on("line", (line) => {
  const nums = line.trim().split(" ").map(Number);

  if (lineCount === 0) {
    [N, M] = nums;
  } else if (lineCount <= N) {
    arr.push(nums[0]);
  } else {
    queries.push(nums[0]);
  }

  lineCount++;

  if (lineCount === N + M + 1) {
    rl.close();
  }
});

rl.on("close", () => {
  arr.sort((a, b) => a - b);

  const indexMap = new Map(); // value -> first index

  for (let i = 0; i < N; i++) {
    if (!indexMap.has(arr[i])) {
      indexMap.set(arr[i], i);
    }
  }

  const result = [];
  for (const q of queries) {
    result.push(indexMap.has(q) ? indexMap.get(q) : -1);
  }

  console.log(result.join("\n"));
});