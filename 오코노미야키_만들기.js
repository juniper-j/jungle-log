/**
 * BOJ 32946 - 오코노미야키 만들기
 * Node.js (read from stdin)
 
 * [Problem]
 * - 프라이팬에는 1 ~ N의 칸이 일렬로 존재
 * - 오코노미야키 반죽 2개가 서로 다른 칸 p1, p2에 있고, 고기 한 덩어리가 칸 M에 있음
 * - 뒤집기는 "선택한 반죽을 인접 1칸으로 이동"시키는 동작 (한 번 = 1칸)
 * - 고기는 "해당 반죽이 M까지 이동한 거리의 홀짝이 홀수일 때" 그 반죽에 붙음
 *   → 즉, 어떤 반죽으로 고기를 집으려면 |pi - M| 이 홀수여야 함
 * - 고기가 반죽에 붙은 이후엔 그 반죽을 움직이면 고기도 함께 이동함
 * - 두 반죽은 같은 칸을 동시에 점유할 수 없음
 *  → 반죽 A가 x → y 구간을 통과하려는데 다른 반죽 B가 그 구간(끝점 포함)에 서 있으면, B를 구간 바깥으로 1칸 더 밀어내야 함 (추가 뒤집기 비용이 듬)
 *  → 가장자리(1 또는 N) 때문에 바깥으로 못 밀면 불가능
 * - 목표: "고기가 붙은 반죽"이 칸 S에 위치하도록 만드는 최소 뒤집기 횟수, 불가능하면 -1
 * 
 * [Solvings]
 * 1) 어느 반죽을 carrier(운반자)로 쓸지 2가지 경우를 모두 계산해 더 작은 값을 답으로
 * 2) carrier가 수행하는 이동은 두 구간 합:
 *   (1) carrier: pi -> M   (고기 집기)   -> 이때 |pi - M|이 홀수여야 가능
 *   (2) carrier: M  -> S   (고기 운반)
 * 각 구간에서 다른 반죽(blocker)이 구간 안/끝점에 있으면 "바깥(좌/우 끝 바깥쪽)"으로 밀어내는 비용을 계산해 더해야 함 (구간 길이의 일부 + 1칸)
 *
 * [Pseudocode]
 * 1) 입력 파싱: N, p1, p2, M, S
 * 2) 함수 inSeg(x, a, b): x가 [min(a,b), max(a,b)] 안에 있는지 여부
 * 3) 함수 clearBlocker(N, a, b, blockerPos, carrierAtLeft):
 *   - 구간 [a, b]를 carrier가 통과할 때 blocker가 그 안/끝에 있으면 바깥(오른쪽은 hi+1, 왼쪽은 lo-1)으로 밀어낸다.
 *   - 가장자리라 못 밀면 불가능(∞)을 반환.
 *   - 반환: [추가비용, 밀림 이후 blocker 위치]
 * 4) 함수 solveWithCarrier(N, carrierPos, otherPos, M, S):
 *   - |carrierPos - M|이 짝수면 고기 못 붙임 → ∞
 *   - cost=0, blocker=otherPos
 *   - (carrierPos→M) 구간에 대해 clearBlocker 실행 → cost에 더하고 carrier 이동거리 더함
 *   - (M→S) 구간에 대해 clearBlocker 실행 → cost에 더하고 이동거리 더함
 *   - 총 cost 반환
 * 5) ans = min( solveWithCarrier(p1, p2), solveWithCarrier(p2, p1) )
 * 6) ans가 ∞면 -1, 아니면 ans 출력
 *
 * [Time Complexity]
 * - 모든 연산은 상수 시간 계산(구간 포함 여부/덧셈뺄셈)으로 구성
 * - 분기 2개(어느 반죽을 carrier로 쓸지)만 평가 -> O(1)
 * - 공간도 O(1)
 */

//////////////////////////
// 1) 입력 파싱 (Node.js 제출 템플릿)
//////////////////////////

// 파일 시스템 모듈 불러오기 (표준 입력을 파일처럼 읽기 위해)
const fs = require('fs');

// 표준 입력 전체를 문자열로 읽고, 양끝 공백 제거 후 공백 기준 분리, 숫자로 변환
const tokens = fs.readFileSync(0, 'utf8').trim().split(/\s+/).map(Number);

// 토큰을 순서대로 꺼내기 위한 인덱스
let t = 0;

// 문제에서 주어지는 값들 순서대로 꺼내기
const N  = tokens[t++];   // 프라이팬 칸 개수 (1..N)
const p1 = tokens[t++];   // 반죽1 초기 위치
const p2 = tokens[t++];   // 반죽2 초기 위치
const M  = tokens[t++];   // 고기 위치
const S  = tokens[t++];   // 최종 목표 칸 (고기가 붙은 반죽의 위치)

// 안전 장치: 입력이 모자라면 -1 출력하고 종료
if ([N, p1, p2, M, S].some(v => Number.isNaN(v))) {
  console.log(-1);
  process.exit(0);
}

//////////////////////////
// 2) 보조 함수들
//////////////////////////

/**
 * x가 [a, b] 구간(양끝 포함)에 있는지 확인
 * a, b의 대소 관계는 알 수 없으므로 먼저 정렬
 */
function inSeg(x, a, b) {
  const lo = Math.min(a, b);      // 작은 끝
  const hi = Math.max(a, b);      // 큰 끝
  return lo <= x && x <= hi;      // 포함 여부 반환
}

/**
 * carrier가 구간 [a, b]를 통과할 때 blocker가 방해하면
 * 바깥으로 밀어내고 그 비용을 계산한다.
 *
 * 매개변수:
 *  - N: 프라이팬 길이 (1..N)
 *  - a, b: carrier가 통과할 구간의 양 끝점
 *  - blockerPos: 다른 반죽의 현재 위치
 *  - carrierAtLeft: carrier가 왼쪽 끝에서 오른쪽으로 진행하면 true,
 *                   오른쪽 끝에서 왼쪽으로 진행하면 false
 *
 * 반환값:
 *  - [extraCost, newBlockerPos]
 *    * extraCost: blocker를 치우느라 든 추가 뒤집기 횟수 (없으면 0)
 *    * newBlockerPos: 치운 뒤 blocker의 새 위치
 *  - 불가능하면 [Infinity, null]
 */
function clearBlocker(N, a, b, blockerPos, carrierAtLeft) {
  // a, b의 정렬된 끝점 계산
  const lo = Math.min(a, b);
  const hi = Math.max(a, b);

  // 현재 blocker 위치를 지역 변수로 복사(이 함수 안에서만 변경)
  let j = blockerPos;

  // blocker가 구간 안/끝에 없으면 추가 비용 없음
  if (!inSeg(j, lo, hi)) return [0, j];

  // carrier가 왼쪽에서 오른쪽으로 지나간다면,
  // blocker는 오른쪽 바깥(hi+1)으로 내보내야 함
  if (carrierAtLeft) {
    // hi가 이미 가장 오른쪽 끝(N)이면 더 밀 공간이 없어 불가능
    if (hi === N) return [Infinity, null];
    // blocker가 j에서 hi+1까지 가야 하므로 (hi - j)칸 + 바깥 1칸
    const cost = (hi - j) + 1;
    j = hi + 1;              // 새 blocker 위치
    return [cost, j];
  } else {
    // carrier가 오른쪽→왼쪽 진행이면, 왼쪽 바깥(lo-1)으로 내보냄
    if (lo === 1) return [Infinity, null];
    // j에서 lo-1까지 가야 하므로 (j - lo)칸 + 바깥 1칸
    const cost = (j - lo) + 1;
    j = lo - 1;              // 새 blocker 위치
    return [cost, j];
  }
}

//////////////////////////
// 3) 핵심 계산 함수
//////////////////////////

/**
 * 특정 반죽을 carrier(운반자)로 선택했을 때의 최소 뒤집기 비용 계산
 *
 * 매개변수:
 *  - carrierPos: carrier의 현재 칸 (p1 또는 p2)
 *  - otherPos:   다른 반죽의 현재 칸
 *  - M, S:       고기 위치, 최종 목표 칸
 *
 * 반환값:
 *  - 가능하면 최소 뒤집기 수, 불가능하면 Infinity
 */
function solveWithCarrier(N, carrierPos, otherPos, M, S) {
  // 1) 고기 부착 가능 여부: |carrierPos - M|이 홀수여야 함
  if (Math.abs(carrierPos - M) % 2 === 0) return Infinity;

  // 총 비용 누적 변수
  let cost = 0;

  // blocker(다른 반죽)의 현재 위치
  let blocker = otherPos;

  // --------- (1) carrier: carrierPos -> M (고기 집기) ----------
  // carrier가 왼쪽→오른쪽으로 가는지 여부
  const carrierAtLeft1 = (carrierPos < M);

  // 구간에 blocker가 있으면 바깥으로 밀어내기
  const [extra1, newBlocker1] = clearBlocker(N, carrierPos, M, blocker, carrierAtLeft1);
  if (!isFinite(extra1)) return Infinity;  // 가장자리로 못 밀면 불가능
  cost += extra1;                          // 추가 비용 누적
  blocker = newBlocker1;                   // blocker 위치 갱신

  // carrier가 실제로 이동하는 거리(|carrierPos - M|)만큼 뒤집기 추가
  cost += Math.abs(carrierPos - M);

  // --------- (2) carrier: M -> S (고기 운반) ----------
  // 목표가 이미 M이면 이동 필요 없음
  if (M !== S) {
    // 이번에도 진행 방향 판별
    const carrierAtLeft2 = (M < S);

    // 다시 blocker가 구간에 있으면 바깥으로 밀어냄
    const [extra2, newBlocker2] = clearBlocker(N, M, S, blocker, carrierAtLeft2);
    if (!isFinite(extra2)) return Infinity;
    cost += extra2;
    blocker = newBlocker2;

    // M에서 S까지 이동 거리 추가
    cost += Math.abs(M - S);
  }

  // 누적된 총 비용 반환
  return cost;
}

//////////////////////////
// 4) 두 경우 중 최소값 선택 & 출력
//////////////////////////

// p1을 carrier로 쓰는 경우와 p2를 carrier로 쓰는 경우를 각각 계산
const cand1 = solveWithCarrier(N, p1, p2, M, S);
const cand2 = solveWithCarrier(N, p2, p1, M, S);

// 두 값 중 더 작은 값 선택
let answer = Math.min(cand1, cand2);

// 불가능(Infinity)이면 -1로 바꿔서 출력
if (!isFinite(answer)) answer = -1;

// 정답 출력
console.log(String(answer));