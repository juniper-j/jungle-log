const input = require('fs').readFileSync('/dev/stdin').toString().trim().split('\n');
const N = Number(input[0]);

for (let i = 1; i <= N; i++) {
    let marking = input[i];
    let score = 0;
    let streak = 0;

    for (let j = 0; j < marking.length; j++) {
        if (marking[j] === 'O') {
            streak++;
            score += streak;
        } else {
            streak = 0; // 연속된 O가 아니면 streak 초기화
        }
    }
    console.log(score);
}
