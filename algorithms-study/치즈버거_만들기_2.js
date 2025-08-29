// Run by Node.js
const readline = require('readline');

const rl = readline.createInterface({ input: process.stdin });
let input = [];

rl.on('line', (line) => {
  input.push(line.trim());
  rl.close();
});

rl.on('close', () => {
  const [A, B] = input[0].split(' ').map(Number); // A: 패티, B: 치즈
  const K = A - B; // 버거 개수 = 패티 - 치즈

  if (K <= 0) {
    console.log('NO');
    return;
  }

  let found = false;

  // 백트래킹: idx번째 버거부터 시작해서 A개 패티, B개 치즈를 맞춰가는 방식
  // 중복 제거 위해 이전 치즈 개수(prevC) 이상만 탐색
  const backtrack = (burgerList, pattiesLeft, cheeseLeft, prevC) => {
    if (burgerList.length > K) return; // 너무 많이 만듦
    if (burgerList.length === K) {
        if (pattiesLeft === 0 && cheeseLeft === 0) {
        found = true;
        console.log('YES');
        console.log(K);
        for (let [p, c] of burgerList) {
        let burger = '';
          for (let i = 0; i < p + c; i++) {
            burger += (i % 2 === 0) ? 'a' : 'b';
          }
          console.log(burger);
        }
        }
      return;
    }

    // 각 버거는 최소 1치즈 → 2패티부터 시작 (패티 = 치즈 + 1)
    for (let cheese = 1; cheese <= cheeseLeft; cheese++) {
      const patty = cheese + 1;
      if (patty > pattiesLeft) break;
      burgerList.push([patty, cheese]);
      backtrack(burgerList, pattiesLeft - patty, cheeseLeft - cheese);
      burgerList.pop();
      if (found) return;
    }
  };

  backtrack([], A, B);

  if (!found) {
    console.log('NO');
  }
});
