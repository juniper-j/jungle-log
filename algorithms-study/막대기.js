/*
 * BOJ 1094 - 막대기
 * https://www.acmicpc.net/problem/1094
 *
 * [Problem & Solvings]
 * - 문제: 길이가 64cm인 막대를 절반씩 자르면서 Xcm 막대를 만들기 위한 최소 막대 개수를 구하라
 * - 조건: 자를 때는 절반으로만 자를 수 있음, 붙이는 것은 자유
 * - 핵심 아이디어: 이진수에서 1의 개수 == 필요한 막대 개수 (2의 제곱수로만 구성됨)
 *
 * [Pseudocode]
 * 1. X를 입력받음
 * 2. X를 이진수로 변환
 * 3. 이진수에서 '1'의 개수를 셈
 * 4. 이 개수를 출력
 *
 * [Time Complexity]
 * - O(log X): 이진수 변환 및 비트 탐색
 * - 입력 X의 최대값이 64이므로 거의 상수 시간
 */

const readline = require('readline');

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

rl.on("line", (line) => {
  const X = parseInt(line.trim());
  const binary = X.toString(2); // 2진수로 변환
  let count = 0;

  for (let bit of binary) {
    if (bit === '1') count++;
  }

  console.log(count);
  rl.close();
});
