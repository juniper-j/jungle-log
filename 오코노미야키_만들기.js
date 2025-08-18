/**
 * BOJ 32946 - 오코노미야키 만들기
 * Node.js (read from stdin)
 * 
 * [Problem & Solvings]
 * - 두 반죽(동일 취급 가정)의 위치, 각 반죽의 면 상태(토글), 고기 처리 상태를 묶어 상태로 정의하고 BFS로 최단 동작 수를 구한다.
 * - 같은 상태를 중복 방문하지 않도록 visited에 정규화된 키를 저장한다.
 * - 한 번의 동작에서 {A 또는 B} 중 하나만 상하좌우로 1칸 이동하며, 이동한 반죽의 면이 뒤집힌다(ori ^= 1).
 * - 고기 칸에 진입했을 때의 규칙은 별도 훅(applyMeat)으로 캡슐화한다.
 *
 * [Pseudocode]
 * 1) 입력 파싱: R, C, 격자 R줄. A/B 시작 위치, 고기 칸 수·인덱스 기록.
 * 2) 상태 표현: (posA, oriA, posB, oriB, meatMask). A/B 구분이 없다면 normalize로 정규화.
 * 3) BFS:
 *    - q에 시작 상태 푸시, visited에 key 저장.
 *    - while q: 상태 팝 → 목표(isGoal)면 거리 반환
 *      - A를 움직이기: 4방향 이웃 → 범위/충돌 검사 → oriA 토글 → applyMeat → normalize → 미방문이면 push
 *      - B를 움직이기: 위 동일
 * 4) 탐색 종료까지 목표가 없으면 -1
 *
 * [Time Complexity]
 * - 상태 수 대략 O((R*C)^2 * 2^2 * M)   // 두 위치 조합 × 두 면 비트 × 고기 상태 수
 * - 각 상태에서 최대 8전이(두 반죽 × 4방향) → 전체 시간은 상태 수에 선형 배수
 * - 메모리는 visited와 큐에 비례
 */

const fs = require('fs');
const data = fs.readFileSync(0, 'utf8').trim().split(/\s+/).map(Number);
let i = 0;

const N = data[i++];            // 프라이팬 칸 개수
const p1 = data[i++], p2 = data[i++]; // 두 반죽 초기 위치 (서로 다름)
const M = data[i++];            // 고기 위치
const S = data[i++];            // 최종 목표 칸 (고기가 붙은 반죽이 있어야 할 칸)

// ------------------------------------------------------------------
// 모델링
// - 한 번 뒤집기 = 선택한 반죽이 인접 1칸으로 이동.
// - 고기는 "그 반죽이 M까지 이동한 총 이동 횟수"가 홀수일 때만 붙는다.
//   (두 반죽 모두 동일한 규칙; |pi - M|가 홀수여야 그 반죽으로 집을 수 있음)
// - 두 반죽은 같은 칸에 있을 수 없음.
// - 경로에 다른 반죽이 있으면, 경로의 "바깥"으로 한 칸 더 밀어내야 함.
//   (세그먼트 [a,b]를 통과하려면, 다른 반죽 j가 그 안에 있거나 끝점에 있으면
//    j를 b+1(오른쪽) 또는 a-1(왼쪽)으로 보내야 함. 이때 1칸 더가 붙는다.)
// - 가장자리에서 밀어낼 수 없으면 불가능.
//
// 이 규칙을 2구간 합으로 계산:
//   1) carrier: pi -> M   (고기 집기; |pi-M| 홀수 필요)
//   2) carrier: M  -> S   (고기 운반)
//   각 구간에서 blocker를 바깥으로 밀어내는 최소 비용을 더한다.
// ------------------------------------------------------------------

function inSeg(x, a, b) {
  const lo = Math.min(a, b), hi = Math.max(a, b);
  return lo <= x && x <= hi;
}

// 구간 [a,b]를 carrier가 끝점 a(또는 b)에서 b(또는 a)로 통과.
// blocker가 구간/끝점에 있으면 바깥으로 내보낸 뒤 통과.
// carrierAtLeft == true 이면 carrier가 왼쪽 끝 a에서 시작해 오른쪽으로 진행.
// 반환: [추가비용, 이동 후 blocker 위치]  (불가능 시 [Infinity, null])
function clearBlocker(N, a, b, blockerPos, carrierAtLeft) {
  const lo = Math.min(a, b), hi = Math.max(a, b);
  let j = blockerPos;

  // carrier가 시작하는 끝점에는 blocker가 없음 (입력 보장: 초기 서로 다른 칸)
  // 통과에 방해가 되는 경우: j ∈ [lo, hi]
  if (!inSeg(j, lo, hi)) return [0, j];

  if (carrierAtLeft) {
    // 오른쪽 바깥(hi+1)로 밀어내야 함
    if (hi === N) return [Infinity, null]; // 더 밀 공간 없음 → 불가능
    const cost = (hi - j) + 1;
    j = hi + 1;
    return [cost, j];
  } else {
    // 왼쪽 바깥(lo-1)로 밀어내야 함
    if (lo === 1) return [Infinity, null];
    const cost = (j - lo) + 1;
    j = lo - 1;
    return [cost, j];
  }
}

// carrier(=선택된 반죽)로 전체를 수행했을 때의 최소 비용을 계산
function solveWithCarrier(N, carrierPos, otherPos, M, S) {
  // 고기 부착 가능 조건: |carrierPos - M| 이 홀수여야 함
  if (Math.abs(carrierPos - M) % 2 === 0) return Infinity;

  let cost = 0;
  let j = otherPos;

  // 1) carrier: carrierPos -> M
  let a = carrierPos, b = M;
  const carrierAtLeft1 = (a < b);

  // blocker 정리
  let [extra1, j1] = clearBlocker(N, a, b, j, carrierAtLeft1);
  if (!isFinite(extra1)) return Infinity;
  cost += extra1;
  j = j1;

  // carrier 이동
  cost += Math.abs(carrierPos - M);

  // 2) carrier: M -> S  (이때 carrier는 M 끝점에서 출발)
  a = M; b = S;
  if (a === b) return cost; // 더 움직일 필요 없음

  const carrierAtLeft2 = (a < b);

  let [extra2, j2] = clearBlocker(N, a, b, j, carrierAtLeft2);
  if (!isFinite(extra2)) return Infinity;
  cost += extra2;

  // 이동
  cost += Math.abs(M - S);

  return cost;
}

let ans = Math.min(
  solveWithCarrier(N, p1, p2, M, S),
  solveWithCarrier(N, p2, p1, M, S)
);

if (!isFinite(ans)) ans = -1;
console.log(ans.toString());