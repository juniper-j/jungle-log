const input = require('fs').readFileSync('/dev/stdin').toString().trim().split('\n');
const C = Number(input[0]);

for (let i = 1; i <= C; i++) {
    let cases = input[i].split(' ').map(Number);
    let N = cases[0];
    let scores = cases.slice(1);

    let avg = scores.reduce((a, b) => a + b, 0) / N;
    let aboveAvgCount = scores.filter(score => score > avg).length;

    let pct = ((aboveAvgCount / N) * 100).toFixed(3);
    console.log(`${pct}%`);
}