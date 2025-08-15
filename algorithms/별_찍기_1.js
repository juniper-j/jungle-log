// fs 모듈로 입력 받기
const input = require('fs').readFileSync('/dev/stdin').toString().trim();

const n = Number(input); // 문자열 → 숫자 변환
const result = [];       // 결과를 담을 배열

for (let i = 1; i <= n; i++) {
    result.push('*'.repeat(i)); // '*'를 i번 반복한 문자열을 배열에 추가
}

// 배열을 줄바꿈으로 합쳐서 한 번에 출력
console.log(result.join('\n'));



// const input = require('fs').readFileSync('/dev/stdin').toString().trim();

// let answer = '';

// for (let i = 1; i <= input; i++) {
//     console.log('*'.repeat(i));
// }
