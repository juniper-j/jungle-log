// fs 모듈을 이용해 입력을 받는 코드
const fs = require('fs');
const input = fs.readFileSync('/dev/stdin').toString().split(' ');

// parseInt를 사용해 입력값을 정수로 변환
const A = parseInt(input[0]);
const B = parseInt(input[1]);

// 결과값 출력 (단, 나누기의 경우는 소수점을 제거하고 몫만 출력하기 위해 parseInt를 사용)
console.log(A + B);
console.log(A - B);
console.log(A * B);
console.log(parseInt(A / B));
console.log(A % B);
