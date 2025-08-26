const input = require('fs').readFileSync('/dev/stdin').toString().trim().split('\n').map(Number);
const multiple = String(input[0] * input[1] * input[2]);
const mul_str = new Array(10).fill(0);

for (let m of multiple) {
    m = parseInt(m);
    mul_str[m]++;
}

console.log(mul_str.join(['\n']));
